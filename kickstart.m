#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_WIDTH 425
#define WINDOW_HEIGHT 40
#define INPUT_FIELD_WIDTH 300
#define BUTTON_WIDTH 40
#define PADDING 10
#define LOGO_WIDTH 40
#define LOGO_HEIGHT 40
#define OPEN_URL_COMMAND "open https://yvonta.com/"

@interface CustomPanel : NSPanel
@end

@implementation CustomPanel
- (BOOL)canBecomeKeyWindow { return YES; }
- (BOOL)canBecomeMainWindow { return YES; }
@end

@interface ClickableButton : NSButton
@end

@implementation ClickableButton
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }
@end

@interface ClickableTextField : NSTextField
@end

@implementation ClickableTextField
- (BOOL)acceptsFirstMouse:(NSEvent *)event { return YES; }
@end

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    CustomPanel *window;
    NSTextField *textInput;
    NSButton *talkButton;
    NSButton *logoButton;
}
@end

@implementation AppDelegate

- (void)openUrl:(id)sender {
    system(OPEN_URL_COMMAND);
}

- (void)submitQuery:(id)sender {
    NSString *text = [textInput stringValue];
    if (text == nil || [text length] == 0) {
        return;
    }
    
    NSCharacterSet *allowedCharacters = [NSCharacterSet URLQueryAllowedCharacterSet];
    NSString *encodedText = [text stringByAddingPercentEncodingWithAllowedCharacters:allowedCharacters];
    
    if (encodedText) {
        NSString *urlString = [NSString stringWithFormat:@"open https://yvonta.ai/apppub/automation/ask.php?q=%@", encodedText];
        system([urlString UTF8String]);
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSRect visibleFrame = [[NSScreen mainScreen] visibleFrame];
    NSRect screenFrame = [[NSScreen mainScreen] frame];
    CGFloat padding = 30;
    CGFloat menuBarHeight = screenFrame.size.height - (visibleFrame.origin.y + visibleFrame.size.height) + padding;
    
    NSRect windowRect = NSMakeRect(
        (screenFrame.size.width - WINDOW_WIDTH) / 2, 
        screenFrame.size.height - menuBarHeight - WINDOW_HEIGHT, 
        WINDOW_WIDTH, 
        WINDOW_HEIGHT
    );

    window = [[CustomPanel alloc] initWithContentRect:windowRect
                                            styleMask:NSWindowStyleMaskBorderless | 
                                                      NSWindowStyleMaskNonactivatingPanel
                                              backing:NSBackingStoreBuffered
                                                defer:NO];
    
    [window setLevel:NSNormalWindowLevel];
    [window orderBack:nil];
    
    [window setCollectionBehavior:NSWindowCollectionBehaviorCanJoinAllSpaces | 
                                   NSWindowCollectionBehaviorStationary | 
                                   NSWindowCollectionBehaviorIgnoresCycle];

    [window setBackgroundColor:[NSColor colorWithCalibratedWhite:0.15 alpha:0.0]];
    [window setOpaque:NO];
    [window setMovableByWindowBackground:YES];
    [window makeKeyAndOrderFront:nil];

    NSView *contentView = [window contentView];

    int current_x = PADDING;
    int element_y = (WINDOW_HEIGHT - LOGO_HEIGHT) / 2;

    // --- Logo implemented as a borderless NSButton with layer enabled for pulsing ---
    NSRect logoRect = NSMakeRect(current_x, element_y, LOGO_WIDTH, LOGO_HEIGHT);
    logoButton = [[ClickableButton alloc] initWithFrame:logoRect];
    [logoButton setButtonType:NSButtonTypeMomentaryChange];
    [logoButton setBordered:NO];
    [logoButton setTarget:self];
    [logoButton setAction:@selector(openUrl:)];
    [logoButton setWantsLayer:YES];
    
    NSImage *logoImage = [[NSImage alloc] initWithContentsOfFile:@"logo.png"];
    if (logoImage) {
        [logoButton setImage:logoImage];
    } else {
        [logoButton setImage:[NSImage imageNamed:NSImageNameCaution]];
    }
    
    // Add slow pulsing animation (opacity fade from 1.0 to 0.4 continuously)
    CABasicAnimation *pulseAnimation = [CABasicAnimation animationWithKeyPath:@"opacity"];
    pulseAnimation.duration = 1.5;
    pulseAnimation.fromValue = @1.0;
    pulseAnimation.toValue = @0.4;
    pulseAnimation.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];
    pulseAnimation.autoreverses = YES;
    pulseAnimation.repeatCount = HUGE_VALF;
    [logoButton.layer addAnimation:pulseAnimation forKey:@"slowPulse"];

    [contentView addSubview:logoButton];
    current_x += LOGO_WIDTH + PADDING;

    // Text Input Field (Safe target-action handling for Enter key without crashing)
    NSRect inputRect = NSMakeRect(current_x, (WINDOW_HEIGHT - 24) / 2, INPUT_FIELD_WIDTH, 24);
    textInput = [[ClickableTextField alloc] initWithFrame:inputRect];
    [[textInput cell] setPlaceholderString:@"Ask Yvonta..."];
    [textInput setTarget:self];
    [textInput setAction:@selector(submitQuery:)];
    [contentView addSubview:textInput];
    current_x += INPUT_FIELD_WIDTH + PADDING;

    // Attributes for black button text
    NSDictionary *blackTextAttrs = @{ NSForegroundColorAttributeName : [NSColor blackColor] };

    // Talk Button (White Background / Black Text)
    NSRect talkRect = NSMakeRect(current_x, (WINDOW_HEIGHT - 28) / 2, BUTTON_WIDTH, 28);
    talkButton = [[ClickableButton alloc] initWithFrame:talkRect];
    [talkButton setAttributedTitle:[[NSAttributedString alloc] initWithString:@"GO!" attributes:blackTextAttrs]];
    [talkButton setButtonType:NSButtonTypeMomentaryLight];
    [talkButton setBezelStyle:NSBezelStyleRounded];
    [talkButton setWantsLayer:YES];
    [talkButton.layer setBackgroundColor:[NSColor whiteColor].CGColor];
    [talkButton setTarget:self];
    [talkButton setAction:@selector(submitQuery:)];
    [contentView addSubview:talkButton];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
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