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
    
    if (text == NULL || strlen(text) == 0) {
        return;
    }

    gchar *escaped_text = g_uri_escape_string(text, NULL, FALSE);
    gchar *url = g_strdup_printf("xdg-open 'https://yvonta.ai/apppub/automation/ask.php?q=%s'", escaped_text);
    
    system(url);

    g_free(escaped_text);
    g_free(url);
}

static void on_logo_click(GtkWidget *widget, gpointer data) {
    system("xdg-open 'https://yvonta.com/'");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Kickstart");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    
    // Borderless
    gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    
    // Set as a utility window locked to the bottom layer (above desktop, below apps)
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_UTILITY);
    gtk_window_set_keep_below(GTK_WINDOW(window), TRUE);

    // Position cleanly in the upper middle of the primary monitor
    GdkDisplay *display = gdk_display_get_default();
    GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
    if (!monitor) {
        monitor = gdk_display_get_monitor(display, 0);
    }
    
    GdkRectangle geometry;
    gdk_monitor_get_geometry(monitor, &geometry);
    
    gint x = geometry.x + (geometry.width - WINDOW_WIDTH) / 2;
    gint y = geometry.y + 30; // 30 pixels down from the top edge
    gtk_window_move(GTK_WINDOW(window), x, y);

    // Force 100% transparent background via CSS and RGBA visual
    GdkScreen *default_screen = gtk_widget_get_screen(window);
    GdkVisual *visual = gdk_screen_get_rgba_visual(default_screen);
    if (visual != NULL) {
        gtk_widget_set_visual(window, visual);
    }

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "window {"
        "  background-color: rgba(0, 0, 0, 0.0);"
        "}"
        ".transparent-box {"
        "  background-color: rgba(30, 30, 30, 0.0);"
        "  border-radius: 8px;"
        "  padding: 5px;"
        "}", -1, NULL);
    
    gtk_style_context_add_provider_for_screen(default_screen,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Main layout container with styling
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_style_context_add_class(gtk_widget_get_style_context(box), "transparent-box");
    gtk_container_add(GTK_CONTAINER(window), box);

    // 1. Logo Button loaded from logo.png
    GtkWidget *logo_button = gtk_button_new();
    gtk_widget_set_size_request(logo_button, 30, 30);
    gtk_button_set_relief(GTK_BUTTON(logo_button), GTK_RELIEF_NONE);
    
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("logo.png", 30, 30, TRUE, &error);
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

    // 2. Text Input Field
    AppElements *elements = g_new(AppElements, 1);
    elements->text_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(elements->text_input), "Ask Yvonta...");
    g_signal_connect(elements->text_input, "activate", G_CALLBACK(on_submit), elements);
    gtk_box_pack_start(GTK_BOX(box), elements->text_input, TRUE, TRUE, 0);

    // 3. Go! Button
    GtkWidget *go_button = gtk_button_new_with_label("GO!");
    gtk_widget_set_size_request(go_button, 30, 30);
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