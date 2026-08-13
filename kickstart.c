#ifdef __APPLE__
// ==========================================
// macOS Implementation (Pure Objective-C)
// ==========================================
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>
#import <stdlib.h>
#import <CoreGraphics/CoreGraphics.h>

#define WINDOW_WIDTH 468
#define TOOLBAR_HEIGHT 50
#define AD_HEIGHT 90
#define WINDOW_HEIGHT (TOOLBAR_HEIGHT + AD_HEIGHT)

@interface TransparentPanel : NSPanel
@end

@implementation TransparentPanel
- (BOOL)canBecomeKeyWindow { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }
@end

@interface AppDelegate : NSObject <NSApplicationDelegate, WKUIDelegate, WKNavigationDelegate> {
    TransparentPanel *window;
    NSTextField *textInput;
    WKWebView *webView;
}
@end

@implementation AppDelegate

- (void)submitQuery:(id)sender {
    NSString *text = [textInput stringValue];
    if (text == nil || [text length] == 0) return;
    
    NSString *encoded = [text stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    if (encoded) {
        NSString *urlString = [NSString stringWithFormat:@"https://yvonta.ai/apppub/automation/ask.php?q=%@", encoded];
        NSURL *url = [NSURL URLWithString:urlString];
        if (url) {
            [[NSWorkspace sharedWorkspace] openURL:url];
        }
    }
}

- (void)openUrl:(id)sender { 
    NSURL *url = [NSURL URLWithString:@"https://yvonta.com"];
    if (url) {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
}

// --- WKNavigationDelegate: Vang normale links af en open ze extern ---
- (void)webView:(WKWebView *)view decidePolicyForNavigationAction:(WKNavigationAction *)navigationAction decisionHandler:(void (^)(WKNavigationActionPolicy))decisionHandler {
    NSURL *url = navigationAction.request.URL;
    
    if (navigationAction.navigationType == WKNavigationTypeLinkActivated || 
        (navigationAction.targetFrame == nil && url != nil)) {
        [[NSWorkspace sharedWorkspace] openURL:url];
        decisionHandler(WKNavigationActionPolicyCancel);
        return;
    }
    
    decisionHandler(WKNavigationActionPolicyAllow);
}

// --- WKUIDelegate: Vang target="_blank" en window.open pop-ups af en open ze extern ---
- (WKWebView *)webView:(WKWebView *)view createWebViewWithConfiguration:(WKWebViewConfiguration *)configuration forNavigationAction:(WKNavigationAction *)navigationAction windowFeatures:(WKWindowFeatures *)windowFeatures {
    NSURL *url = navigationAction.request.URL;
    if (url != nil) {
        [[NSWorkspace sharedWorkspace] openURL:url];
    }
    return nil;
}

- (void)applicationDidFinishLaunching:(NSNotification * _Nonnull)aNotification {
    NSRect screenFrame = [[NSScreen mainScreen] frame];
    NSRect rect = NSMakeRect((screenFrame.size.width - WINDOW_WIDTH)/2, screenFrame.size.height - 160, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    window = [[TransparentPanel alloc] initWithContentRect:rect
                                                 styleMask:NSWindowStyleMaskBorderless
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
    
    [window setLevel:NSNormalWindowLevel];
    [window setCollectionBehavior:NSWindowCollectionBehaviorStationary | NSWindowCollectionBehaviorCanJoinAllSpaces];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0]];
    [window setHasShadow:NO];
    [window setHidesOnDeactivate:NO];
    [window setIgnoresMouseEvents:NO];
    [window makeKeyAndOrderFront:nil];

    NSView *content = [window contentView];
    [content setWantsLayer:YES];
    [content.layer setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0].CGColor];
    
    // --- Toolbar Container ---
    NSView *container = [[NSView alloc] initWithFrame:NSMakeRect(0, AD_HEIGHT, WINDOW_WIDTH, TOOLBAR_HEIGHT)];
    [container setWantsLayer:YES];
    [container.layer setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0].CGColor];
    [container.layer setCornerRadius:8.0];
    [content addSubview:container];
    
    // Logo Button
    NSButton *logoButton = [[NSButton alloc] initWithFrame:NSMakeRect(5, 5, 30, 30)];
    [logoButton setButtonType:NSButtonTypeMomentaryChange];
    [logoButton setBordered:NO];
    [logoButton setTarget:self];
    [logoButton setAction:@selector(openUrl:)];

    NSString *logoPath = [[NSBundle mainBundle] pathForResource:@"logo" ofType:@"png"];
    if (!logoPath) {
        logoPath = [[[NSFileManager defaultManager] currentDirectoryPath] stringByAppendingPathComponent:@"logo.png"];
    }
    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:logoPath];

    if (logoImage) {
        [logoImage setSize:NSMakeSize(24, 24)];
        [logoButton setImage:logoImage];
        [logoButton setImagePosition:NSImageOnly];
    } else {
        [logoButton setTitle:@"Y"];
    }
    [container addSubview:logoButton];

    // Text Input Field
    textInput = [[NSTextField alloc] initWithFrame:NSMakeRect(40, 8, 373, 24)];
    [[textInput cell] setPlaceholderString:@"Ask Yvonta..."];
    [textInput setTarget:self];
    [textInput setAction:@selector(submitQuery:)];
    [container addSubview:textInput];

    // GO! Button
    NSButton *goButton = [[NSButton alloc] initWithFrame:NSMakeRect(421, 6, 40, 28)];
    [goButton setTitle:@"GO!"];
    [goButton setButtonType:NSButtonTypeMomentaryLight];
    [goButton setBezelStyle:NSBezelStyleRounded];
    [goButton setTarget:self];
    [goButton setAction:@selector(submitQuery:)];
    [container addSubview:goButton];

    // --- Web Browser Ad Widget ---
    WKWebViewConfiguration *webConfig = [[WKWebViewConfiguration alloc] init];
    webView = [[WKWebView alloc] initWithFrame:NSMakeRect(0, 0, WINDOW_WIDTH, AD_HEIGHT) configuration:webConfig];
    [webView setValue:@YES forKey:@"drawsBackground"];
    
    webView.navigationDelegate = self;
    webView.UIDelegate = self;

    NSURL *adURL = [NSURL URLWithString:@"https://yvonta.ai/apppub/banners/kickstart.html"];
    NSURLRequest *request = [NSURLRequest requestWithURL:adURL];
    [webView loadRequest:request];
    [content addSubview:webView];
}
@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSApplication *app = [NSApplication sharedApplication];
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        AppDelegate *delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}

#elif defined(__linux__)
// ==========================================
// Linux Implementation (GTK3 + WebKit2GTK)
// ==========================================
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <stdlib.h>

#define WINDOW_WIDTH 468
#define TOOLBAR_HEIGHT 50
#define AD_HEIGHT 90
#define SPACER 5
#define WINDOW_HEIGHT (TOOLBAR_HEIGHT + AD_HEIGHT + SPACER)

static void on_submit(GtkEntry *entry, gpointer user_data) {
    const gchar *text = gtk_entry_get_text(entry);
    if (!text || text[0] == '\0') return;

    gchar *escaped = g_uri_escape_string(text, NULL, TRUE);
    gchar *url_str = g_strdup_printf("https://yvonta.ai/apppub/automation/ask.php?q=%s", escaped);
    
    gchar *cmd = g_strdup_printf("xdg-open \"%s\"", url_str);
    int status = system(cmd);
    (void)status;

    g_free(escaped);
    g_free(url_str);
    g_free(cmd);
}

static void on_logo_clicked(GtkButton *button, gpointer user_data) {
    int status = system("xdg-open \"https://yvonta.com\"");
    (void)status;
}

// --- FIX: Intercept WebKit2GTK link navigation & open externally ---
static gboolean decide_policy_cb(WebKitWebView *web_view, WebKitPolicyDecision *decision, WebKitPolicyDecisionType decision_type, gpointer user_data) {
    if (decision_type == WEBKIT_POLICY_DECISION_TYPE_NAVIGATION_ACTION ||
        decision_type == WEBKIT_POLICY_DECISION_TYPE_NEW_WINDOW_ACTION) {
        
        WebKitNavigationPolicyDecision *navigation_decision = WEBKIT_NAVIGATION_POLICY_DECISION(decision);
        WebKitNavigationAction *navigation_action = webkit_navigation_policy_decision_get_navigation_action(navigation_decision);
        WebKitURIRequest *request = webkit_navigation_action_get_request(navigation_action);
        const gchar *uri = webkit_uri_request_get_uri(request);

        // Allow initial load of the banner itself, but open clicked hyperlinks externally via xdg-open
        if (g_strcmp0(uri, "https://yvonta.ai/apppub/banners/kickstart.html") != 0) {
            gchar *cmd = g_strdup_printf("xdg-open \"%s\"", uri);
            int status = system(cmd);
            (void)status;
            g_free(cmd);

            webkit_policy_decision_ignore(decision);
            return TRUE;
        }
    }
    return FALSE;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Yvonta Widget");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    
    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (!monitor) {
        monitor = gdk_display_get_monitor(display, 0);
    }
    GdkRectangle workarea;
    gdk_monitor_get_workarea(monitor, &workarea);

    int pos_x = workarea.x + (workarea.width - WINDOW_WIDTH) / 2;
    int pos_y = workarea.y + 160;
    gtk_window_move(GTK_WINDOW(window), pos_x, pos_y);

    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_below(GTK_WINDOW(window), TRUE);
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_NORMAL); 
    gtk_window_set_accept_focus(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(window), TRUE);
    gtk_window_set_skip_pager_hint(GTK_WINDOW(window), TRUE);

    GdkScreen *screen = gtk_window_get_screen(GTK_WINDOW(window));
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL) {
        gtk_widget_set_visual(window, visual);
    }
    
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(cssProvider, 
        "window { background-color: rgba(0, 0, 0, 0.0); }"
        "box { background-color: rgba(0, 0, 0, 0.0); }"
        "button.flat, button { background-color: rgba(255, 255, 255, 0.3); background-image: none; border: none; box-shadow: none; border-radius: 10px; }"
        "button.flat:hover, button:hover { background-color: rgba(255, 255, 255, 0.6); border-radius: 10px; }"
        "button.logo-button { background-color: rgba(0, 0, 0, 1.0); background-image: none; border: none; box-shadow: none; border-radius: 50%; }"
        "button.logo-button:hover { background-color: rgba(255, 255, 255, 0.6); border-radius: 50%; }"
	".text-input { background-color: rgba(255, 255, 255, 0.8); border-radius: 10px; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, 
        GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, SPACER);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_size_request(toolbar, WINDOW_WIDTH, TOOLBAR_HEIGHT);
    gtk_box_pack_start(GTK_BOX(box), toolbar, FALSE, FALSE, 0);

    GtkWidget *logoButton = gtk_button_new();
    gtk_widget_set_size_request(logoButton, 40, 40);
    gtk_style_context_add_class(gtk_widget_get_style_context(logoButton), "logo-button");
    g_signal_connect(logoButton, "clicked", G_CALLBACK(on_logo_clicked), NULL);
    
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("logo.png", 40, 40, TRUE, &error);
    if (pixbuf) {
        GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
        gtk_button_set_image(GTK_BUTTON(logoButton), image);
        g_object_unref(pixbuf);
    } else {
        gtk_button_set_label(GTK_BUTTON(logoButton), "Y");
        if (error) g_error_free(error);
    }
    gtk_box_pack_start(GTK_BOX(toolbar), logoButton, FALSE, FALSE, 5);

    GtkWidget *textInput = gtk_entry_new();
    gtk_widget_set_can_focus(textInput, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(textInput), "Ask Yvonta...");
    g_signal_connect(textInput, "activate", G_CALLBACK(on_submit), NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(textInput), "text-input");
    gtk_box_pack_start(GTK_BOX(toolbar), textInput, TRUE, TRUE, 5);

    GtkWidget *goButton = gtk_button_new_with_label("GO!");
    g_signal_connect_swapped(goButton, "clicked", G_CALLBACK(on_submit), textInput);
    gtk_box_pack_start(GTK_BOX(toolbar), goButton, FALSE, FALSE, 5);

    // --- WebKit Ad Widget (Below Toolbar) ---
    GtkWidget *webView = webkit_web_view_new();
    gtk_widget_set_size_request(webView, WINDOW_WIDTH, AD_HEIGHT);
    
    // --- FIX: Connect decision-policy signal to handle links ---
    g_signal_connect(webView, "decide-policy", G_CALLBACK(decide_policy_cb), NULL);

    webkit_web_view_load_uri(WEBKIT_WEB_VIEW(webView), "https://yvonta.ai/apppub/banners/kickstart.html");
 
    gtk_box_pack_start(GTK_BOX(box), webView, TRUE, TRUE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_grab_focus(textInput);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
#endif