#include <gtk/gtk.h>
#include <math.h>
#include "slope.h"

SlopeItem *series1;
SlopeScale *scale;
GtkWidget *view;


GtkWidget *g_lbl_hello;
GtkWidget *g_lbl_count;

int main(int argc, char *argv[])
{
  double *x, *y;
	x = g_malloc (12 * sizeof(double));
	y = g_malloc (12 * sizeof(double));
	x[0]  = 0;	y[0]  = 0;
	x[1]  = 15;	y[1]  = 8.803;
	x[2]  = 30;	y[2]  = 11.935;
	x[3]  = 45;	y[3]  = 15.316;
	x[4]  = 60;	y[4]  = 19.195;
	x[5]  = 90;	y[5]  = 28.426;
	x[6]  = 120;	y[6]  = 35.830;	
	x[7]  = 150;	y[7]  = 41.672;	
	x[8]  = 180;	y[8]  = 49.569;
	x[9]  = 240;	y[9]  = 62.338;
	x[10] = 300;	y[10] = 72.883;
	x[11] = 360;	y[11] = 79.588;
	const long n = 101;
	const SlopeSample time_samples_array[] = {
		{0.0, "0"},	{15.0, "15"},	{30.0, "30"},
		{45.0, "45"}, 	{60.0, "60"}, 	{90.0, "90"},
	    	{120.0, "120"}, {150.0, "150"}, {180.0, "180"},
	    	{240.0, "240"},	{300.0, "300"}, {360.0, "360"}
	};
	const SlopeSample percentage_samples_array[] = {
		{0.0, ""}, {10.0, "10%"}, {20.0, "20%"}, {30.0, "30%"},
		{40.0, "40%"}, {50.0, "50%"}, {60.0, "60%"}, {70.0, "70%"},
		{80.0, "80%"}, {90.0, "90%"}, {100.0, "100%"}
	};
  GtkBuilder      *builder; 
  GtkWidget       *window;
  gtk_init (&argc, &argv);

	view = slope_view_new ();
	/**/
	SlopeFigure *figure = slope_figure_new ();
	slope_view_set_figure (SLOPE_VIEW (view), figure);
	/**/
  	scale = slope_xyscale_new();
  	slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
	slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
	/**/
  SlopeItem *series = slope_xyseries_new_filled ("Measured data", x, y, 12, "kor");
	slope_scale_add_item (scale, series);
	SlopeItem *x_axis;
  x_axis = slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_BOTTOM);
	SlopeSampler *x_sampler;
	x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
	slope_sampler_set_samples(x_sampler, time_samples_array, 12);
//  GtkBuilder      *builder; 
//  GtkWidget       *window;
//  gtk_init (&argc, &argv);
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "farmafit.glade", NULL);
  GtkWidget *label = gtk_label_new ("prkno");
  GtkWidget *box = GTK_WIDGET(gtk_builder_get_object(builder, "key_box"));

	gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "top_window"));
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(builder);
  gtk_widget_show_all(window);                
  gtk_main();
  return 0;
}////i/iiiiiiiiiiiiii/

G_MODULE_EXPORT void on_btn_hello_clicked()
{
    static unsigned int count = 0;
    char str_count[30] = {0};
    
    gtk_label_set_text(GTK_LABEL(g_lbl_hello), "Hello, world!");
    count++;
    sprintf(str_count, "%d", count);
    gtk_label_set_text(GTK_LABEL(g_lbl_count), str_count);
}

G_MODULE_EXPORT void on_window_main_destroy()
{
    gtk_main_quit();
}

