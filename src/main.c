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
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * TODO Change variable names to suit standard names
 * TODO Process data from the input table, instead of reading it from the file
 * TODO Clear up all warnings
 * */


#define NUMOF_PARAMETERS 9
#define NUMOF_EXAMPLE_DATA_POINTS 12
#define NUMOF_PLOT_POINTS 101
#define NUMOF_LINES 4
#define MSG_NOT_CALCULATED "Not calculated"
#define MSG_INCORRECT_ENTRY "ERROR: Incorrect entry!"
#define BASE_10 10
#define STR_LEN_FOR_CONVERSION 6 
#define LBL_MEASURED_DATA "Measured data"
#define LBL_ZERO_ORDER_KINETICS "Zero-order kinetics"
#define LBL_FIRST_ORDER_KINETICS "First-order kinetics"
#define LBL_HIGUCHIS_EQUATION "Higuchi's equation"
#define LBL_PEPPAS_EQUATION "Peppas' equation"
#define GLADE_FILE_NAME "farmafit.glade"
#define KEY_BOX "key_box"
static const double example_data_time_values[] =
{
  0, 15, 30, 45, 60, 90, 120, 150, 180, 240, 300, 360
};
static const double example_data_percentage_values[] =
{
  0, 8.803, 11.935, 15.316, 19.195, 28.426,
  35.830, 41.672, 49.569, 62.338, 72.883, 79.588 
};
static const SlopeSample example_data_time_ticks[] = 
{
  {0.0, "0"},	  {15.0, "15"},   {30.0, "30"},   {45.0, "45"},
  {60.0, "60"},   {90.0, "90"},   {120.0, "120"}, {150.0, "150"},
  {180.0, "180"}, {240.0, "240"}, {300.0, "300"}, {360.0, "360"}
};
/* TODO Check if these are used at all?  */
//static const SlopeSample example_data_percentage_ticks[] = 
//{
//  {0.0, ""},	  {10.0, "10%"},  {20.0, "20%"},  {30.0, "30%"},
//  {40.0, "40%"},  {50.0, "50%"},  {60.0, "60%"},  {70.0, "70%"},
//  {80.0, "80%"},  {90.0, "90%"},  {100.0, "100%"}
//};
static SlopeScale *scale;
static SlopeFigure *figure;
static SlopeItem *series_md;
static SlopeItem *series_pe;
static SlopeItem *series_zo;
static SlopeItem *series_fo;
static SlopeItem *series_he;
static GtkWidget *box;
static GtkWidget *view;
static GtkWidget *window;
static GtkWidget *tick_zo;
static GtkWidget *tick_fo;
static GtkWidget *tick_he;
static GtkWidget *tick_pe;
static GtkWidget **example_t;
static GtkWidget **example_p;
static GtkWidget **models_params;
static GtkBuilder *builder;
struct models_params models_params_struct;

void help_version ();
void G_MODULE_EXPORT on_btn_clear_clicked ();
void G_MODULE_EXPORT on_tick_zo_toggled (GtkWidget *widget, gpointer data);
void G_MODULE_EXPORT on_tick_fo_toggled (GtkWidget *widget, gpointer data);
void G_MODULE_EXPORT on_tick_he_toggled (GtkWidget *widget, gpointer data);
void G_MODULE_EXPORT on_tick_pe_toggled (GtkWidget *widget, gpointer data);
void G_MODULE_EXPORT on_btn_load_example_clicked ();
void generate_plots (int max_minutes);
void fill_out_labels ();
void G_MODULE_EXPORT show_msg_box (char *);
void load_io_widgets ();

int
correct_entry_digits_or_pt (char *entry)
{
  /* Return value: 0 = FALSE, 1 = TRUE  */
  int all_good_flag = 1;
  int numof_pts = 0;
  size_t str_len = strlen (entry);
  if (str_len == 0) return 0;
  for (int i = 0; i < str_len; ++i)
    if (entry[i] == '.') ++numof_pts;
  if (numof_pts > 1) return 0;
  for (int i = 0; i < str_len; ++i)
    if (!(isdigit(entry[i]) || entry[i] == '.')) return 0;
  return all_good_flag;
}

void
G_MODULE_EXPORT on_btn_calculate_and_plot_clicked ()
{
  struct dp *data_set = (struct dp *) g_malloc (sizeof (struct dp));
  struct dp *data_set_aux = (struct dp *) g_malloc (sizeof (struct dp));
  fmf_init_data_set (data_set);
  fmf_init_data_set (data_set_aux);
  char number_as_string[STR_LEN_FOR_CONVERSION];
  char *aux = (char *) g_malloc (STR_LEN_FOR_CONVERSION * sizeof (char));
  /* t00  */
  aux = gtk_entry_get_text (GTK_ENTRY (example_t[0]));
  if (!correct_entry_digits_or_pt (aux))
  {
    show_msg_box (MSG_INCORRECT_ENTRY);
    gtk_widget_grab_focus (example_t[0]);
    return;
  }
  
  // First work out how many data entries are there
  // There must be at least 3 valid data points
  // A valid data point is the one that has both entries non-zero and non-empty
  // This is obviously not relevant to the first data point where both 
  // values can be 0 (but cannot be emtpy)
   
  //fmf_form_data_set ("example.json", data_set);
  //models_params_struct = fmf_calc_params (data_set);
  //fill_out_labels ();
  //generate_plots (360);
  return;
}

void gui_driver(int argc, char *argv[])
{
  gtk_init (&argc, &argv);
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, GLADE_FILE_NAME, NULL);
  //
  box = GTK_WIDGET (gtk_builder_get_object (builder, KEY_BOX)); 
  //
  view = slope_view_new ();
  figure = slope_figure_new ();
  slope_view_set_figure (SLOPE_VIEW (view), figure);
  //
  gtk_box_pack_start (GTK_BOX (box), view, TRUE, TRUE, 0);
  //
  window = GTK_WIDGET (gtk_builder_get_object (builder, "top_window"));
  //
  load_io_widgets ();
  //
  gtk_builder_connect_signals (builder, NULL);
  g_object_unref (builder);
  g_signal_connect (window, "delete_event", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (window);                
  gtk_main ();
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
  while ((c = getopt (argc, argv, ":hvgf:?")) != -1)
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

void
G_MODULE_EXPORT on_btn_clear_clicked ()
{
  for (int i = 0; i < NUMOF_EXAMPLE_DATA_POINTS; ++i)
  {
    gtk_entry_set_text (GTK_ENTRY (example_t[i]), "");
    gtk_entry_set_text (GTK_ENTRY (example_p[i]), "");  
  }
  gtk_entry_set_text (GTK_ENTRY (example_t[0]), "0");
  gtk_entry_set_text (GTK_ENTRY (example_p[0]), "0");
  for (int i = 0; i < NUMOF_PARAMETERS; ++i)
  {
    gtk_label_set_text (GTK_LABEL (models_params[i]), MSG_NOT_CALCULATED);
  }
  // TODO Add a check here to see if the plot has been generated
  //slope_scale_remove_item (scale, series_md);
  /* Four -1's are key to clearing the graph properly!  */
  //slope_scale_set_layout_rect (scale, -1, -1, -1, -1);
  //slope_view_redraw(SLOPE_VIEW (view));
  gtk_toggle_button_set_active (tick_zo, FALSE);
  gtk_toggle_button_set_active (tick_fo, FALSE);
  gtk_toggle_button_set_active (tick_he, FALSE);
  gtk_toggle_button_set_active (tick_pe, FALSE);
  return;
}

void
G_MODULE_EXPORT on_tick_zo_toggled (GtkWidget *widget, gpointer data)
{
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    slope_scale_remove_item (scale, series_zo);
  else
    slope_scale_add_item (scale, series_zo);
  slope_view_redraw (SLOPE_VIEW (view));
  return;
}

void
G_MODULE_EXPORT on_tick_fo_toggled (GtkWidget *widget, gpointer data)
{
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    slope_scale_remove_item (scale, series_fo);
  else
    slope_scale_add_item (scale, series_fo);
  slope_view_redraw (SLOPE_VIEW (view));
  return;
}

void
G_MODULE_EXPORT on_tick_he_toggled (GtkWidget *widget, gpointer data)
{
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    slope_scale_remove_item (scale, series_he);
  else
    slope_scale_add_item (scale, series_he);
  slope_view_redraw (SLOPE_VIEW (view));
  return;
}

void
G_MODULE_EXPORT on_tick_pe_toggled (GtkWidget *widget, gpointer data)
{
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    slope_scale_remove_item (scale, series_pe);
  else
    slope_scale_add_item (scale, series_pe);
  slope_view_redraw (SLOPE_VIEW (view));
  return;
}

void
G_MODULE_EXPORT on_btn_load_example_clicked ()
{
  char number_as_string[STR_LEN_FOR_CONVERSION];
  for (int i = 0; i < NUMOF_EXAMPLE_DATA_POINTS; ++i)
  {
    snprintf(number_as_string, BASE_10, "%.0f", example_data_time_values[i]);
    gtk_entry_set_text (GTK_ENTRY(example_t[i]), number_as_string);
    snprintf(number_as_string, BASE_10, "%.3f", example_data_percentage_values[i]);
    gtk_entry_set_text (GTK_ENTRY(example_p[i]), number_as_string);
  }
  for (int i = 0; i < NUMOF_PARAMETERS; ++i)
  {
    gtk_label_set_text (GTK_LABEL (models_params[i]), MSG_NOT_CALCULATED);
  }
  return;
}

void
generate_plots (int max_minutes)
{
  scale = slope_xyscale_new ();
  slope_figure_add_scale (SLOPE_FIGURE (figure), scale);
  slope_scale_set_layout_rect (scale, 0, 0, 1, 1);
  /* Changing from 12 to 14 below does funny things.  */
  series_md = slope_xyseries_new_filled (LBL_MEASURED_DATA, example_data_time_values, example_data_percentage_values, 12, "kor");
  slope_scale_add_item (scale, series_md);
  /* Zero-order kinetics  */
  double *x_zo, *y_zo;
  x_zo = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  y_zo = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) 
  {
    x_zo[i] = (double) i * max_minutes / (NUMOF_PLOT_POINTS - 1);
    y_zo[i] = models_params_struct.k0 * x_zo[i];
  }
  series_zo = slope_xyseries_new_filled (LBL_ZERO_ORDER_KINETICS, x_zo, y_zo, NUMOF_PLOT_POINTS, "r-");
  /* First-order kinetics  */
  double *x_fo, *y_fo;
  x_fo = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  y_fo = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) 
  {
    x_fo[i] = (double) i * max_minutes / (NUMOF_PLOT_POINTS - 1);
    y_fo[i] = (1 - exp (-1 * models_params_struct.k1 * x_fo[i])) * 100;
  }
  series_fo = slope_xyseries_new_filled (LBL_FIRST_ORDER_KINETICS, x_fo, y_fo, NUMOF_PLOT_POINTS, "m-");
  /* Higuchi's equation  */
  double *x_he, *y_he;
  x_he = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  y_he = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) 
  {
    x_he[i] = (double) i * max_minutes / (NUMOF_PLOT_POINTS - 1);
    y_he[i] = models_params_struct.kh * sqrt (x_he[i]);
  }
  series_he = slope_xyseries_new_filled (LBL_HIGUCHIS_EQUATION, x_he, y_he, NUMOF_PLOT_POINTS, "g-");
  /* Peppas' equation  */
  double *x_pe, *y_pe;
  x_pe = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  y_pe = g_malloc (NUMOF_PLOT_POINTS * sizeof (double));
  for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) 
  {
    x_pe[i] = (double) i * max_minutes / (NUMOF_PLOT_POINTS - 1);
    y_pe[i] = models_params_struct.k * pow (x_pe[i], models_params_struct.tn);
  }
  series_pe = slope_xyseries_new_filled (LBL_PEPPAS_EQUATION, x_pe, y_pe, NUMOF_PLOT_POINTS, "b-");
  /* Setting up the graph  */
  SlopeItem *x_axis;
  x_axis = slope_xyscale_get_axis (SLOPE_XYSCALE (scale), SLOPE_XYSCALE_AXIS_BOTTOM);
  SlopeSampler *x_sampler;
  x_sampler = slope_xyaxis_get_sampler (SLOPE_XYAXIS (x_axis));
  slope_sampler_set_samples (x_sampler, example_data_time_ticks, NUMOF_EXAMPLE_DATA_POINTS);
  slope_view_redraw (SLOPE_VIEW (view));
  return;
}

void
fill_out_labels ()
{
  char number[STR_LEN_FOR_CONVERSION];
  snprintf(number, BASE_10, "%.4f", models_params_struct.k0);
  gtk_label_set_text (GTK_LABEL(models_params[0]), number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k0);
  gtk_label_set_text (GTK_LABEL(models_params[1]),number);  
  snprintf(number, BASE_10, "%.4f", models_params_struct.k1);
  gtk_label_set_text (GTK_LABEL(models_params[2]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k1);
  gtk_label_set_text (GTK_LABEL(models_params[3]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.kh);
  gtk_label_set_text (GTK_LABEL(models_params[4]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_kh);
  gtk_label_set_text (GTK_LABEL(models_params[5]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.k);
  gtk_label_set_text (GTK_LABEL(models_params[6]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.tn);
  gtk_label_set_text (GTK_LABEL(models_params[7]),number);
  snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k);
  gtk_label_set_text (GTK_LABEL(models_params[8]),number);
  return;
}

void
G_MODULE_EXPORT show_msg_box (char *msg_text)
{
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkDialog *dialog;
  dialog = gtk_message_dialog_new (window,
				   flags,
				   GTK_MESSAGE_ERROR,
				   GTK_BUTTONS_CLOSE,
				   "%s",
				   msg_text);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void
load_io_widgets ()
{
  example_t = (GtkWidget *) g_malloc (NUMOF_EXAMPLE_DATA_POINTS * sizeof (GtkWidget *));
  example_t[0] = GTK_WIDGET (gtk_builder_get_object (builder, "t00"));
  example_t[1] = GTK_WIDGET (gtk_builder_get_object (builder, "t01"));
  example_t[2] = GTK_WIDGET (gtk_builder_get_object (builder, "t02"));
  example_t[3] = GTK_WIDGET (gtk_builder_get_object (builder, "t03"));
  example_t[4] = GTK_WIDGET (gtk_builder_get_object (builder, "t04"));
  example_t[5] = GTK_WIDGET (gtk_builder_get_object (builder, "t05"));
  example_t[6] = GTK_WIDGET (gtk_builder_get_object (builder, "t06"));
  example_t[7] = GTK_WIDGET (gtk_builder_get_object (builder, "t07"));
  example_t[8] = GTK_WIDGET (gtk_builder_get_object (builder, "t08"));
  example_t[9] = GTK_WIDGET (gtk_builder_get_object (builder, "t09"));
  example_t[10] = GTK_WIDGET (gtk_builder_get_object (builder, "t10"));
  example_t[11] = GTK_WIDGET (gtk_builder_get_object (builder, "t11"));
  example_p = (GtkWidget *) g_malloc (NUMOF_EXAMPLE_DATA_POINTS * sizeof (GtkWidget *));
  example_p[0] = GTK_WIDGET (gtk_builder_get_object (builder, "p00"));
  example_p[1] = GTK_WIDGET (gtk_builder_get_object (builder, "p01"));
  example_p[2] = GTK_WIDGET (gtk_builder_get_object (builder, "p02"));
  example_p[3] = GTK_WIDGET (gtk_builder_get_object (builder, "p03"));
  example_p[4] = GTK_WIDGET (gtk_builder_get_object (builder, "p04"));
  example_p[5] = GTK_WIDGET (gtk_builder_get_object (builder, "p05"));
  example_p[6] = GTK_WIDGET (gtk_builder_get_object (builder, "p06"));
  example_p[7] = GTK_WIDGET (gtk_builder_get_object (builder, "p07"));
  example_p[8] = GTK_WIDGET (gtk_builder_get_object (builder, "p08"));
  example_p[9] = GTK_WIDGET (gtk_builder_get_object (builder, "p09"));
  example_p[10] = GTK_WIDGET (gtk_builder_get_object (builder, "p10"));
  example_p[11] = GTK_WIDGET (gtk_builder_get_object (builder, "p11"));
  gtk_entry_set_text (GTK_ENTRY (example_t[0]), "0");
  gtk_entry_set_text (GTK_ENTRY (example_p[0]), "0");
  models_params = (GtkWidget *) g_malloc (NUMOF_PARAMETERS * sizeof (GtkWidget *));
  models_params[0] = GTK_WIDGET (gtk_builder_get_object (builder, "k0"));
  models_params[1] = GTK_WIDGET (gtk_builder_get_object (builder, "rsq_k0"));
  models_params[2] = GTK_WIDGET (gtk_builder_get_object (builder, "k1"));
  models_params[3] = GTK_WIDGET (gtk_builder_get_object (builder, "rsq_k1"));
  models_params[4] = GTK_WIDGET (gtk_builder_get_object (builder, "kh"));
  models_params[5] = GTK_WIDGET (gtk_builder_get_object (builder, "rsq_kh"));
  models_params[6] = GTK_WIDGET (gtk_builder_get_object (builder, "k"));
  models_params[7] = GTK_WIDGET (gtk_builder_get_object (builder, "n"));
  models_params[8] = GTK_WIDGET (gtk_builder_get_object (builder, "rsq_k"));
  tick_zo = (GtkWidget *) g_malloc (sizeof (GtkWidget *));
  tick_zo = GTK_WIDGET (gtk_builder_get_object (builder, "tick_zo"));
  tick_fo = (GtkWidget *) g_malloc (sizeof (GtkWidget *));
  tick_fo = GTK_WIDGET (gtk_builder_get_object (builder, "tick_fo"));
  tick_he = (GtkWidget *) g_malloc (sizeof (GtkWidget *));
  tick_he = GTK_WIDGET (gtk_builder_get_object (builder, "tick_he"));
  tick_pe = (GtkWidget *) g_malloc (sizeof (GtkWidget *));
  tick_pe = GTK_WIDGET (gtk_builder_get_object (builder, "tick_pe"));
}

