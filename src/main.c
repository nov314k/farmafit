/*******************************************************************************
 * Copyright (C) 2019 Novak Petrovic                                           *
 * <dev2[at]novak5[.]33mail[.]com>                                             *
 *                                                                             *
 * This file is part of Farmafit.                                              *
 * For more details see the README (or README.md) file.                        *
 *                                                                             *
 *   Farmafit is free software: you can redistribute it and/or modify it       *
 *   under the terms of the GNU General Public License                         *
 *   as published by the Free Software Foundation;                             *
 *   either version 3 of the License, or (at your option) any later version.   *
 *                                                                             *
 *   Farmafit is distributed in the hope that it will be useful,               *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *   GNU General Public License for more details.                              *
 *                                                                             *
 *   You should have received a copy of the GNU General Public License         *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.     *
 ******************************************************************************/
/**
 * @file main.c
 * @author Novak Petrovic
 * @date 2019
 * @brief Main Farmafit function.
 *
 * @see README (or README.md) for more details.
 */
#include "globdefs.h"
#include "data_types.h"
#include "farmafit.h"
#include <slope/slope.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

SlopeItem *series1;
SlopeScale *scale;
GtkWidget *view;
GtkWidget **example_t;
GtkWidget **example_p;
GtkWidget **models_params;
GtkBuilder *builder; 
GtkWidget *window;
GtkWidget *box;
SlopeFigure *figure;
SlopeItem *series;
GtkWidget *g_lbl_hello;
GtkWidget *g_lbl_count;

void help_version ();

G_MODULE_EXPORT void on_btn_hello_clicked()
{
    static unsigned int count = 0;
    char str_count[30] = {0};
    
    gtk_label_set_text(GTK_LABEL(g_lbl_hello), "Hello, world!");
    count++;
    sprintf(str_count, "%d", count);
    gtk_label_set_text(GTK_LABEL(g_lbl_count), str_count);
}

void
G_MODULE_EXPORT on_btn_load_example_clicked ()
{
  gtk_entry_set_text (GTK_ENTRY(example_t[0]),"0");
  gtk_entry_set_text (GTK_ENTRY(example_t[1]),"15");
  gtk_entry_set_text (GTK_ENTRY(example_t[2]),"30");
  gtk_entry_set_text (GTK_ENTRY(example_t[3]),"45");
  gtk_entry_set_text (GTK_ENTRY(example_t[4]),"60");
  gtk_entry_set_text (GTK_ENTRY(example_t[5]),"90");
  gtk_entry_set_text (GTK_ENTRY(example_t[6]),"120");
  gtk_entry_set_text (GTK_ENTRY(example_t[7]),"150");
  gtk_entry_set_text (GTK_ENTRY(example_t[8]),"180");
  gtk_entry_set_text (GTK_ENTRY(example_t[9]),"240");
  gtk_entry_set_text (GTK_ENTRY(example_t[10]),"300");
  gtk_entry_set_text (GTK_ENTRY(example_t[11]),"360");  
  gtk_entry_set_text (GTK_ENTRY(example_p[0]),"0");  
  gtk_entry_set_text (GTK_ENTRY(example_p[1]),"8.803");  
  gtk_entry_set_text (GTK_ENTRY(example_p[2]),"11.935");  
  gtk_entry_set_text (GTK_ENTRY(example_p[3]),"15.316");  
  gtk_entry_set_text (GTK_ENTRY(example_p[4]),"19.195");  
  gtk_entry_set_text (GTK_ENTRY(example_p[5]),"28.426");  
  gtk_entry_set_text (GTK_ENTRY(example_p[6]),"35.830");  
  gtk_entry_set_text (GTK_ENTRY(example_p[7]),"41.672");  
  gtk_entry_set_text (GTK_ENTRY(example_p[8]),"49.569");  
  gtk_entry_set_text (GTK_ENTRY(example_p[9]),"62.338");  
  gtk_entry_set_text (GTK_ENTRY(example_p[10]),"72.883");  
  gtk_entry_set_text (GTK_ENTRY(example_p[11]),"79.588");  
  gtk_label_set_text (GTK_LABEL(models_params[0]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[1]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[2]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[3]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[4]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[5]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[6]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[7]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[8]),"Not calculated");  
}

void
G_MODULE_EXPORT on_btn_clear_clicked ()
{
  gtk_entry_set_text (GTK_ENTRY(example_t[0]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[1]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[2]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[3]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[4]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[5]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[6]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[7]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[8]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[9]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[10]),"");
  gtk_entry_set_text (GTK_ENTRY(example_t[11]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[0]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[1]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[2]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[3]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[4]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[5]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[6]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[7]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[8]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[9]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[10]),"");  
  gtk_entry_set_text (GTK_ENTRY(example_p[11]),"");  
  gtk_label_set_text (GTK_LABEL(models_params[0]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[1]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[2]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[3]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[4]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[5]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[6]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[7]),"Not calculated");  
  gtk_label_set_text (GTK_LABEL(models_params[8]),"Not calculated");  
  slope_scale_remove_item (scale, series);
  slope_scale_set_layout_rect(scale, -1, -1, -1, -1);
  slope_view_redraw(SLOPE_VIEW(view));
}

void
dummy_plot ()
{
  view = slope_view_new ();
  SlopeFigure *figure = slope_figure_new ();
  slope_view_set_figure (SLOPE_VIEW (view), figure);
  scale = slope_xyscale_new();
  slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
  slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
  //SlopeItem *series = slope_xyseries_new_filled ("Example data", example_t,
  //    example_p, 12, "kor");
  //slope_scale_add_item (scale, series);
  //SlopeItem *x_axis;
  //x_axis = slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_BOTTOM);
  //SlopeSampler *x_sampler;
  //x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
  //slope_sampler_set_samples(x_sampler, example_t_ticks, numof_examples);
}

void
test_on_btn_load_example_clicked ()
{
  const int numof_examples = 12;
  double *example_t = g_malloc (numof_examples * sizeof(double));
  double *example_p = g_malloc (numof_examples * sizeof(double));
  example_t[0]  = 0;	example_p[0]  = 0;
  example_t[1]  = 15;	example_p[1]  = 8.803;
  example_t[2]  = 30;	example_p[2]  = 11.935;
  example_t[3]  = 45;	example_p[3]  = 15.316;
  example_t[4]  = 60;	example_p[4]  = 19.195;
  example_t[5]  = 90;	example_p[5]  = 28.426;
  example_t[6]  = 120;	example_p[6]  = 35.830;	
  example_t[7]  = 150;	example_p[7]  = 41.672;	
  example_t[8]  = 180;	example_p[8]  = 49.569;
  example_t[9]  = 240;	example_p[9]  = 62.338;
  example_t[10] = 300;	example_p[10] = 72.883;
  example_t[11] = 360;	example_p[11] = 79.588;
  const int plot_points = 101;
  const SlopeSample example_t_ticks[] = 
  {
    {0.0, "0"},	    {15.0, "15"},   {30.0, "30"},   {45.0, "45"},
    {60.0, "60"},   {90.0, "90"},   {120.0, "120"}, {150.0, "150"},
    {180.0, "180"}, {240.0, "240"}, {300.0, "300"}, {360.0, "360"}
  };
  const SlopeSample example_p_ticks[] = 
  {
    {0.0, ""},	    {10.0, "10%"},  {20.0, "20%"},  {30.0, "30%"},
    {40.0, "40%"},  {50.0, "50%"},  {60.0, "60%"},  {70.0, "70%"},
    {80.0, "80%"},  {90.0, "90%"},  {100.0, "100%"}
  };
  //view = slope_view_new ();
  //SlopeFigure *figure = slope_figure_new ();
  //slope_view_set_figure (SLOPE_VIEW (view), figure);
  scale = slope_xyscale_new();
  slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
  slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
  series = slope_xyseries_new_filled ("Example data", example_t,
      example_p, 12, "kor");
  slope_scale_add_item (scale, series);
  SlopeItem *x_axis;
  x_axis = slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_BOTTOM);
  SlopeSampler *x_sampler;
  x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
  slope_sampler_set_samples(x_sampler, example_t_ticks, numof_examples);
  slope_view_redraw(SLOPE_VIEW(view));
}

void
G_MODULE_EXPORT on_btn_calculate_and_plot_clicked ()
{
  struct dp *data_set = (struct dp *) malloc (sizeof (struct dp));
  fmf_init_data_set (data_set);
  fmf_form_data_set ("example.json", data_set);
  struct models_params models_params_struct = fmf_calc_params (data_set);
  //
  //
  //
  char number[10];
  snprintf(number, 10, "%.4f", models_params_struct.k0);
  gtk_label_set_text (GTK_LABEL(models_params[0]), number);
  snprintf(number, 10, "%.4f", models_params_struct.rsq_k0);
  gtk_label_set_text (GTK_LABEL(models_params[1]),number);  
  snprintf(number, 10, "%.4f", models_params_struct.k1);
  gtk_label_set_text (GTK_LABEL(models_params[2]),number);
  snprintf(number, 10, "%.4f", models_params_struct.rsq_k1);
  gtk_label_set_text (GTK_LABEL(models_params[3]),number);
  snprintf(number, 10, "%.4f", models_params_struct.kh);
  gtk_label_set_text (GTK_LABEL(models_params[4]),number);
  snprintf(number, 10, "%.4f", models_params_struct.rsq_kh);
  gtk_label_set_text (GTK_LABEL(models_params[5]),number);
  snprintf(number, 10, "%.4f", models_params_struct.k);
  gtk_label_set_text (GTK_LABEL(models_params[6]),number);
  snprintf(number, 10, "%.4f", models_params_struct.tn);
  gtk_label_set_text (GTK_LABEL(models_params[7]),number);
  snprintf(number, 10, "%.4f", models_params_struct.rsq_k);
  gtk_label_set_text (GTK_LABEL(models_params[8]),number);

  test_on_btn_load_example_clicked ();
  //test_on_btn_load_example_clicked ();
  //gtk_builder_add_from_file (builder, "farmafit.glade", NULL);
  //box = GTK_WIDGET(gtk_builder_get_object(builder, "key_box"));

  //gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);
  //scale = slope_xyscale_new ();
  //slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
  //slope_scale_set_layout_rect (scale, 0, 0, 1, 1);
  //gtk_widget_show (box);
  //gtk_widget_show_all(window);                
}
  

int gui_driver(int argc, char *argv[])
{
  puts ("GUI version starting...");
  gtk_init (&argc, &argv);
  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder, "farmafit.glade", NULL);
  box = GTK_WIDGET(gtk_builder_get_object(builder, "key_box"));
  //test_on_btn_load_example_clicked ();
  //dummy_plot ();
  view = slope_view_new ();
  figure = slope_figure_new ();
  slope_view_set_figure (SLOPE_VIEW (view), figure);
  //scale = slope_xyscale_new();
  //slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
  //slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
  gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);
  example_t = (GtkWidget *) g_malloc (12 * sizeof (GtkWidget *));
  example_t[0] = GTK_WIDGET(gtk_builder_get_object (builder, "t00"));
  example_t[1] = GTK_WIDGET(gtk_builder_get_object (builder, "t01"));
  example_t[2] = GTK_WIDGET(gtk_builder_get_object (builder, "t02"));
  example_t[3] = GTK_WIDGET(gtk_builder_get_object (builder, "t03"));
  example_t[4] = GTK_WIDGET(gtk_builder_get_object (builder, "t04"));
  example_t[5] = GTK_WIDGET(gtk_builder_get_object (builder, "t05"));
  example_t[6] = GTK_WIDGET(gtk_builder_get_object (builder, "t06"));
  example_t[7] = GTK_WIDGET(gtk_builder_get_object (builder, "t07"));
  example_t[8] = GTK_WIDGET(gtk_builder_get_object (builder, "t08"));
  example_t[9] = GTK_WIDGET(gtk_builder_get_object (builder, "t09"));
  example_t[10] = GTK_WIDGET(gtk_builder_get_object (builder, "t10"));
  example_t[11] = GTK_WIDGET(gtk_builder_get_object (builder, "t11"));
  example_p = (GtkWidget *) g_malloc (12 * sizeof (GtkWidget *));
  example_p[0] = GTK_WIDGET(gtk_builder_get_object (builder, "p00"));
  example_p[1] = GTK_WIDGET(gtk_builder_get_object (builder, "p01"));
  example_p[2] = GTK_WIDGET(gtk_builder_get_object (builder, "p02"));
  example_p[3] = GTK_WIDGET(gtk_builder_get_object (builder, "p03"));
  example_p[4] = GTK_WIDGET(gtk_builder_get_object (builder, "p04"));
  example_p[5] = GTK_WIDGET(gtk_builder_get_object (builder, "p05"));
  example_p[6] = GTK_WIDGET(gtk_builder_get_object (builder, "p06"));
  example_p[7] = GTK_WIDGET(gtk_builder_get_object (builder, "p07"));
  example_p[8] = GTK_WIDGET(gtk_builder_get_object (builder, "p08"));
  example_p[9] = GTK_WIDGET(gtk_builder_get_object (builder, "p09"));
  example_p[10] = GTK_WIDGET(gtk_builder_get_object (builder, "p10"));
  example_p[11] = GTK_WIDGET(gtk_builder_get_object (builder, "p11"));
  models_params = (GtkWidget *) g_malloc (9 * sizeof (GtkWidget *));
  models_params[0] = GTK_WIDGET(gtk_builder_get_object (builder, "k0"));
  models_params[1] = GTK_WIDGET(gtk_builder_get_object (builder, "rsq_k0"));
  models_params[2] = GTK_WIDGET(gtk_builder_get_object (builder, "k1"));
  models_params[3] = GTK_WIDGET(gtk_builder_get_object (builder, "rsq_k1"));
  models_params[4] = GTK_WIDGET(gtk_builder_get_object (builder, "kh"));
  models_params[5] = GTK_WIDGET(gtk_builder_get_object (builder, "rsq_kh"));
  models_params[6] = GTK_WIDGET(gtk_builder_get_object (builder, "k"));
  models_params[7] = GTK_WIDGET(gtk_builder_get_object (builder, "n"));
  models_params[8] = GTK_WIDGET(gtk_builder_get_object (builder, "rsq_k"));
  window = GTK_WIDGET(gtk_builder_get_object(builder, "top_window"));
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(builder);
  g_signal_connect (window, "delete_event", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all(window);                
  //gtk_widget_hide (box);
  gtk_main();
  return 0;
}

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      fprintf (stderr, BASIC_ERROR_MESSAGE);
      help_version ();
      exit (EXIT_FAILURE);
    }
  int c;
  while ((c = getopt (argc, argv, "hvgf:")) != -1)
    {
      switch (c)
	{
	case 'h':
	  help_version ();
	  exit (EXIT_SUCCESS);
	  break;
	case 'v':
	  help_version ();
	  exit (EXIT_SUCCESS);
	  break;
	case 'g':
	  gui_driver (argc, argv);
	  exit (EXIT_SUCCESS);
	  break;
	case 'f':
	  if (argc == 3)
	    {
	      fmf_print_params (optarg);
	      exit (EXIT_SUCCESS);
	    }
	  else
	    {
	      fprintf (stderr, BASIC_ERROR_MESSAGE);
	      help_version ();
	      exit (EXIT_FAILURE);
	    }
	  break;
	case '?':
	  fprintf (stderr, BASIC_ERROR_MESSAGE);
	  help_version ();
	  exit (EXIT_FAILURE);
	  break;
	case ':':
	  fprintf (stderr, BASIC_ERROR_MESSAGE);
	  help_version ();
	  exit (EXIT_FAILURE);
	  break;
	default:
	  fprintf (stderr, BASIC_ERROR_MESSAGE);
	  help_version ();
	  exit (EXIT_FAILURE);
	  break;
	}
    }
  return EXIT_SUCCESS;
}

void
help_version ()
{
  puts ("Basic usage: ./farmafit -f example.json,");
  puts ("where example.json contains experimental measurement data.");
  puts ("For further help and version information see README.md");
  return;
}
