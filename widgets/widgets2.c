#include <gtk/gtk.h>
#include <math.h>
#include "slope.h"

//SlopeItem *series1;
//SlopeScale *scale;
//GtkWidget *view;


//void CheckEvent (GtkWidget *widget, gpointer *data)
//{
//    	g_print ("checkbox event: %s\n", data);
//	if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
//	{	
//		slope_scale_remove_item (scale, series1);
//	}
//	else
//	{
//		slope_scale_add_item (scale, series1);
//	}
//	//slope_scale_remove_item (scale, series1);
//	slope_view_redraw(SLOPE_VIEW(view));
//}

static void
print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *grid;
  GtkWidget *button;

  /* create a new window, and set its title */
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* Here we construct the container that is going pack our buttons */
  grid = gtk_grid_new ();

  /* Pack the container in the window */
  gtk_container_add (GTK_CONTAINER (window), grid);

  //button = gtk_button_new_with_label ("Button 1");
  //g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  GtkWidget *label = gtk_label_new ("prkno");

  /* Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 3);

  button = gtk_button_new_with_label ("Button 2");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  gtk_grid_attach (GTK_GRID (grid), button, 1, 0, 1, 1);

  button = gtk_button_new_with_label ("Button 3");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);

  /* Place the second button in the grid cell (1, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), button, 1, 1, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 1, 2, 1, 1);

  /* Now that we are done packing our widgets, we show them all
   * in one go, by calling gtk_widget_show_all() on the window.
   * This call recursively calls gtk_widget_show() on all widgets
   * that are contained in the window, directly or indirectly.
   */
  gtk_widget_show_all (window);

}
int
main (int argc, char *argv[])
{
GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

//	double *x, *y;
//	x = g_malloc (12 * sizeof(double));
//	y = g_malloc (12 * sizeof(double));
//	x[0]  = 0;	y[0]  = 0;
//	x[1]  = 15;	y[1]  = 8.803;
//	x[2]  = 30;	y[2]  = 11.935;
//	x[3]  = 45;	y[3]  = 15.316;
//	x[4]  = 60;	y[4]  = 19.195;
//	x[5]  = 90;	y[5]  = 28.426;
//	x[6]  = 120;	y[6]  = 35.830;	
//	x[7]  = 150;	y[7]  = 41.672;	
//	x[8]  = 180;	y[8]  = 49.569;
//	x[9]  = 240;	y[9]  = 62.338;
//	x[10] = 300;	y[10] = 72.883;
//	x[11] = 360;	y[11] = 79.588;
//	const long n = 101;
//	double *x11, *y11;
//	x11 = g_malloc (n * sizeof(double));
//	y11 = g_malloc (n * sizeof(double));
//	for (int k = 0; k < n; k++) {
//		x11[k] = (double) k * 360/100;
//		y11[k] = 1.022 * pow (x11[k], 0.7449);
//	}
//	const SlopeSample time_samples_array[] = {
//		{0.0, "0"},	{15.0, "15"},	{30.0, "30"},
//		{45.0, "45"}, 	{60.0, "60"}, 	{90.0, "90"},
//	    	{120.0, "120"}, {150.0, "150"}, {180.0, "180"},
//	    	{240.0, "240"},	{300.0, "300"}, {360.0, "360"}
//	};
//	const SlopeSample percentage_samples_array[] = {
//		{0.0, ""}, {10.0, "10%"}, {20.0, "20%"}, {30.0, "30%"},
//		{40.0, "40%"}, {50.0, "50%"}, {60.0, "60%"}, {70.0, "70%"},
//		{80.0, "80%"}, {90.0, "90%"}, {100.0, "100%"}
//	};
//	/**/
//	//	/**/
//	gtk_init (&argc, &argv);
////	/**/	
//  	GtkWidget *window;
//	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//  	gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
// 	g_signal_connect (
//	      G_OBJECT (window), "destroy", G_CALLBACK (gtk_main_quit), NULL);
////	/**/
//	GtkWidget *box;
//	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
//	gtk_container_add (GTK_CONTAINER (window), box);
//	/* Checkbox goes here instead of a button */
//	GtkWidget *check;
//	check = gtk_check_button_new_with_label ("Show fitted data");
//	gtk_box_pack_start (GTK_BOX (box), check, FALSE, FALSE, 10);
//	gtk_widget_show (check);
//	g_signal_connect (G_OBJECT (check), "toggled", G_CALLBACK (CheckEvent), "test");
////	/**/
////	//GtkWidget *button;
////	//button = gtk_button_new_with_label ("Plot measurements and models");
////	//g_signal_connect (G_OBJECT (button), "clicked", NULL, NULL);
////	/**/
//	view = slope_view_new ();
////	/**/
//	SlopeFigure *figure = slope_figure_new ();
//	slope_view_set_figure (SLOPE_VIEW (view), figure);
////	/**/
//  	scale = slope_xyscale_new();
//  	slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
//	slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
////	/**/
//////	update_plot (button, NULL);
//	/**/
	//SlopeItem *series = slope_xyseries_new_filled ("Measured data", x, y, 12, "kor");
	//slope_scale_add_item (scale, series);
//	/**/
//	x11 = g_malloc (n * sizeof(double));
//	y11 = g_malloc (n * sizeof(double));
//	//update_plot_1 (button, NULL);
//	/**/
//	series1 = slope_xyseries_new_filled ("Fitted data", x11, y11, n, "b-");
//	slope_scale_add_item (scale, series1);
//	/**/
//	SlopeItem *series = slope_xyseries_new_filled ("Measured data", x, y, 12, "kor");
//	slope_scale_add_item (scale, series);
//	/**/
//	SlopeItem *x_axis;
//	x_axis = slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_BOTTOM);
//	SlopeSampler *x_sampler;
//	x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
//	slope_sampler_set_samples(x_sampler, time_samples_array, 12);
//	/**/
//	SlopeItem *y_axis;
//	y_axis = slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_LEFT);
//	SlopeSampler *y_sampler;
//	y_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(y_axis));
//	slope_sampler_set_samples(y_sampler, percentage_samples_array, 12);
//	/**/
//	gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);
//	//gtk_box_pack_start (GTK_BOX (box), button, FALSE, TRUE, 2);
//	/**/
//	gtk_widget_show_all (window);
//	gtk_main();
//	/**/
//	g_free(x);
//	g_free(y);
	return 0;
}
