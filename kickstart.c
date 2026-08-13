#ifdef __APPLE__
// ==========================================
// macOS Implementation (Pure Objective-C)
// ==========================================
#import <Cocoa/Cocoa.h>
#import <stdlib.h>
#import <CoreGraphics/CoreGraphics.h>

#define WINDOW_WIDTH 425
#define WINDOW_HEIGHT 40

@interface TransparentPanel : NSPanel
@end

@implementation TransparentPanel
- (BOOL)canBecomeKeyWindow { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }
@end

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    TransparentPanel *window;
    NSTextField *textInput;
}
@end

@implementation AppDelegate
- (void)submitQuery:(id)sender {
    NSString *text = [textInput stringValue];
    if (text == nil || [text length] == 0) return;
    NSString *encoded = [text stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
    if (encoded) {
        NSString *url = [NSString stringWithFormat:@"open https://yvonta.ai/apppub/automation/ask.php?q=%@", encoded];
        system([url UTF8String]);
    }
}
- (void)openUrl:(id)sender { system("open https://yvonta.com/"); }

- (void)applicationDidFinishLaunching:(NSNotification * _Nonnull)aNotification {
    NSRect screenFrame = [[NSScreen mainScreen] frame];
    NSRect rect = NSMakeRect((screenFrame.size.width - WINDOW_WIDTH)/2, screenFrame.size.height - 70, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    window = [[TransparentPanel alloc] initWithContentRect:rect
                                                 styleMask:NSWindowStyleMaskBorderless
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
    
    // Set window order to be at the lowest layer, right above the desktop background
    [window setLevel:kCGDesktopWindowLevel];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0]];
    [window setHasShadow:NO];
    [window setHidesOnDeactivate:NO];
    [window makeKeyAndOrderFront:nil];

    // Root content view with explicit alpha 0.0 clear background
    NSView *content = [window contentView];
    [content setWantsLayer:YES];
    [content.layer setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0].CGColor];
    
    // Floating toolbar container with alpha 0.0 background
    NSView *container = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT)];
    [container setWantsLayer:YES];
    [container.layer setBackgroundColor:[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.0].CGColor];
    [container.layer setCornerRadius:8.0];
    [content addSubview:container];
    
    // --- Logo Button ---
    NSButton *logoButton = [[NSButton alloc] initWithFrame:NSMakeRect(5, 5, 30, 30)];
    [logoButton setButtonType:NSButtonTypeMomentaryChange];
    [logoButton setBordered:NO];
    [logoButton setTarget:self];
    [logoButton setAction:@selector(openUrl:)];

    NSString *logoPath = [[NSFileManager defaultManager] currentDirectoryPath];
    logoPath = [logoPath stringByAppendingPathComponent:@"logo.png"];
    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:logoPath];

    if (logoImage) {
        [logoImage setSize:NSMakeSize(24, 24)];
        [logoButton setImage:logoImage];
        [logoButton setImagePosition:NSImageOnly];
    } else {
        [logoButton setTitle:@"Y"];
    }
    [container addSubview:logoButton];

    // --- Text Input Field ---
    textInput = [[NSTextField alloc] initWithFrame:NSMakeRect(40, 8, 330, 24)];
    [[textInput cell] setPlaceholderString:@"Ask Yvonta..."];
    [textInput setTarget:self];
    [textInput setAction:@selector(submitQuery:)];
    [container addSubview:textInput];

    // --- GO! Button ---
    NSButton *goButton = [[NSButton alloc] initWithFrame:NSMakeRect(378, 6, 40, 28)];
    [goButton setTitle:@"GO!"];
    [goButton setButtonType:NSButtonTypeMomentaryLight];
    [goButton setBezelStyle:NSBezelStyleRounded];
    [goButton setTarget:self];
    [goButton setAction:@selector(submitQuery:)];
    [container addSubview:goButton];
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

#else
// ==========================================
// Linux / Manjaro Implementation (Pure GTK3)
// ==========================================
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH 425
#define WINDOW_HEIGHT 40

typedef struct {
    GtkWidget *text_input;
} AppElements;

static void on_submit(GtkWidget *widget, gpointer data) {
    AppElements *app = (AppElements *)data;
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(app->text_input));
    if (text == NULL || strlen(text) == 0) return;

    gchar *escaped = g_uri_escape_string(text, NULL, FALSE);
    gchar *url = g_strdup_printf("xdg-open 'https://yvonta.ai/apppub/automation/ask.php?q=%s'", escaped);
    system(url);
    g_free(escaped);
    g_free(url);
}

static void on_logo_click(GtkWidget *widget, gpointer data) {
    system("xdg-open 'https://yvonta.com/'");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DESKTOP);
    gtk_window_set_keep_below(GTK_WINDOW(window), TRUE);
    gtk_widget_set_app_paintable(window, TRUE);

    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (!monitor) monitor = gdk_display_get_monitor(display, 0);
    
    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);
    gtk_window_move(GTK_WINDOW(window), geometry.x + (geometry.width - WINDOW_WIDTH) / 2, geometry.y + 30);

    GdkScreen *screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);
    if (visual && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(window, visual);
    }

    GtkCssProvider *css = gtk_css_provider_new();
    // Forces complete alpha 0.0 transparency across window and container box
    gtk_css_provider_load_from_data(css, "window, window.background { background-color: rgba(0,0,0,0.0); background-image: none; } .box { background-color: rgba(0,0,0,0.0); border-radius: 8px; padding: 5px; }", -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "box");
    gtk_container_add(GTK_CONTAINER(window), box);

    // Logo Button
    GtkWidget *logo_button = gtk_button_new();
    gtk_widget_set_size_request(logo_button, 40, 30);
    gtk_button_set_relief(GTK_BUTTON(logo_button), GTK_RELIEF_NONE);
    
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("logo.png", 24, 24, TRUE, &error);
    if (pixbuf) {
        GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
        gtk_container_add(GTK_CONTAINER(logo_button), image);
        g_object_unref(pixbuf);
    } else {
        gtk_button_set_label(GTK_BUTTON(logo_button), "Y");
        if (error) g_error_free(error);
    }
    
    g_signal_connect(logo_button, "clicked", G_CALLBACK(on_logo_click), NULL);
    gtk_box_pack_start(GTK_BOX(box), logo_button, FALSE, FALSE, 0);

    // Text Input Field
    AppElements *elements = g_new(AppElements, 1);
    elements->text_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(elements->text_input), "Ask Yvonta...");
    g_signal_connect(elements->text_input, "activate", G_CALLBACK(on_submit), elements);
    gtk_box_pack_start(GTK_BOX(box), elements->text_input, TRUE, TRUE, 0);

    // GO! Button
    GtkWidget *go_button = gtk_button_new_with_label("GO!");
    gtk_widget_set_size_request(go_button, 40, 30);
    g_signal_connect(go_button, "clicked", G_CALLBACK(on_submit), elements);
    gtk_box_pack_start(GTK_BOX(box), go_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.yvonta.kickstart", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
#endif