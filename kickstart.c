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

#endif