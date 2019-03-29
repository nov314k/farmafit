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
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * TODO Change variable names to suit standard names
 * TODO Process data from the input table, instead of reading it from the file
 * TODO Check if all proper Gtk types are used, to reduce type casting
 * TODO Ensure all floating point values for percentages are to three decimal
 * places as it seems to add other values that it shouldn't
 * Right at the very beginning pushing "calculate and plot" crashes the app
 * */

/* TODO Change tick_zo to GtkToggleButton*/

#define NUMOF_MODELS_PARAMS 9
#define MAX_NUMOF_DATA_PTS 12
#define NUMOF_PLOT_POINTS 101
#define NUMOF_LINES 4
#define MSG_NOT_CALCULATED "Not calculated"
#define MSG_INCORRECT_ENTRY "ERROR: Incorrect entry!"
#define MSG_INSUFF_DATA_POINTS "ERROR: Enter at least 3 valid data points."
#define BASE_10 10
#define STR_LEN_FOR_CONVERSION 6
#define LBL_MEASURED_DATA "Measured data"
#define LBL_ZERO_ORDER_KINETICS "Zero-order kinetics"
#define LBL_FIRST_ORDER_KINETICS "First-order kinetics"
#define LBL_HIGUCHIS_EQUATION "Higuchi's equation"
#define LBL_PEPPAS_EQUATION "Peppas' equation"
#define GLADE_FILE_NAME "farmafit.glade"
#define KEY_BOX "key_box"
static const char *time_entry_name[] = {
	"t00", "t01", "t02", "t03", "t04", "t05", "t06", "t07", "t08", "t09", "t10", "t11"
};
static const char *perc_entry_name[] = {
	"p00", "p01", "p02", "p03", "p04", "p05", "p06", "p07", "p08", "p09", "p10", "p11"
};
static const char *models_params_names[] = {
	"k0", "rsq_k0", "k1", "rsq_k1", "kh", "rsq_kh", "k", "n", "rsq_k"
};







static const double example_data_time_values[] = {

	0, 15, 30, 45, 60, 90, 120, 150, 180, 240, 300, 360
};

static const double example_data_percentage_values[] = {
	0, 8.803, 11.935, 15.316, 19.195, 28.426,
	35.830, 41.672, 49.569, 62.338, 72.883, 79.588
};

//static const SlopeSample example_data_time_ticks[] =
//{
//  {0.0, "0"},   {15.0, "15"},   {30.0, "30"},   {45.0, "45"},
//  {60.0, "60"},   {90.0, "90"},   {120.0, "120"}, {150.0, "150"},
//  {180.0, "180"}, {240.0, "240"}, {300.0, "300"}, {360.0, "360"}
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
static GtkWidget *tick_zo;
static GtkWidget *tick_fo;
static GtkWidget *tick_he;
static GtkWidget *tick_pe;
static GtkWidget **time_entries;
static GtkWidget **perc_entries;
static GtkWidget **models_params;
static GtkWindow *window;
static GtkBuilder *builder;
static struct models_params models_params_struct;
static struct dp *data_set;
static int done_calculating_and_plotting = 0;

void help_version();
void G_MODULE_EXPORT on_btn_clear_clicked();
void G_MODULE_EXPORT on_tick_zo_toggled(GtkWidget * widget, gpointer data);
void G_MODULE_EXPORT on_tick_fo_toggled(GtkWidget * widget, gpointer data);
void G_MODULE_EXPORT on_tick_he_toggled(GtkWidget * widget, gpointer data);
void G_MODULE_EXPORT on_tick_pe_toggled(GtkWidget * widget, gpointer data);
void G_MODULE_EXPORT on_btn_load_example_clicked();
void generate_plots();
void fill_out_labels();
void G_MODULE_EXPORT show_msg_box(char *);
void load_io_widget_handles();
int digits_or_single_pt_only(char *);
int digits_only(char *entry);

void
G_MODULE_EXPORT on_btn_calculate_and_plot_clicked()
{
	if (done_calculating_and_plotting == 1) {
		slope_scale_remove_item(scale, series_md);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tick_zo))) {
			slope_scale_remove_item(scale, series_zo);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_zo), FALSE);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tick_fo))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_fo), FALSE);
			slope_scale_remove_item(scale, series_fo);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tick_he))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_he), FALSE);
			slope_scale_remove_item(scale, series_he);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tick_pe))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_pe), FALSE);
			slope_scale_remove_item(scale, series_pe);
		}
		/* Four -1's are key to clearing the graph properly!  */
		slope_scale_set_layout_rect(scale, -1, -1, -1, -1);
		slope_view_redraw(SLOPE_VIEW(view));
		g_free(data_set);
	}
	data_set = (struct dp *)g_malloc(sizeof(struct dp));
	fmf_init_data_set(data_set);
	struct dp *current_point = data_set;
	unsigned int numof_valid_pts = 0;
	char *aux_t = (char *)g_malloc(STR_LEN_FOR_CONVERSION * sizeof(char));
	char *aux_p = (char *)g_malloc(STR_LEN_FOR_CONVERSION * sizeof(char));
	/* Cast from const char * to avoid compiler warnings  */
	aux_t = (char *)gtk_entry_get_text(GTK_ENTRY(time_entries[0]));
	if (!digits_only(aux_t)) {
		show_msg_box(MSG_INCORRECT_ENTRY);
		gtk_widget_grab_focus(time_entries[0]);
		return;
	}
	aux_p = (char *)gtk_entry_get_text(GTK_ENTRY(perc_entries[0]));
	if (!digits_or_single_pt_only(aux_p)) {
		show_msg_box(MSG_INCORRECT_ENTRY);
		gtk_widget_grab_focus(perc_entries[0]);
		return;
	}
	current_point->mins = atof(aux_t);
	current_point->perc = atof(aux_p);
	++numof_valid_pts;
	unsigned int found_the_end = 0;
	for (unsigned int i = 1; i < 12 && !found_the_end; ++i) {
		aux_t = (char *)gtk_entry_get_text(GTK_ENTRY(time_entries[i]));
		if (!digits_only(aux_t)) {
			show_msg_box(MSG_INCORRECT_ENTRY);
			gtk_widget_grab_focus(time_entries[i]);
			return;
		}
		aux_p = (char *)gtk_entry_get_text(GTK_ENTRY(perc_entries[i]));
		if (!digits_or_single_pt_only(aux_p)) {
			show_msg_box(MSG_INCORRECT_ENTRY);
			gtk_widget_grab_focus(perc_entries[i]);
			return;
		}
		if (atof(aux_t) == 0 || atof(aux_p) == 0 || strlen(aux_t) == 0
			|| strlen(aux_p) == 0) {
			found_the_end = 1;
		} else {
			struct dp *dp_ = (struct dp *)g_malloc(sizeof(struct dp));
			dp_->mins = atof(aux_t);
			dp_->perc = atof(aux_p);
			current_point->next = dp_;
			current_point = dp_;
			current_point->next = NULL;
			++numof_valid_pts;
		}
	}
	//fmf_form_data_set ("example.json", data_set);
	if (numof_valid_pts >= 3) {
		models_params_struct = fmf_calc_params(data_set);
		fill_out_labels();
		generate_plots();
	} else {
		show_msg_box(MSG_INSUFF_DATA_POINTS);
		gtk_widget_grab_focus(time_entries[numof_valid_pts]);
	}
	done_calculating_and_plotting = 1;
	return;
}

void gui_driver(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, GLADE_FILE_NAME, NULL);
	box = GTK_WIDGET(gtk_builder_get_object(builder, KEY_BOX));
	view = slope_view_new();
	figure = slope_figure_new();
	slope_view_set_figure(SLOPE_VIEW(view), figure);
	gtk_box_pack_start(GTK_BOX(box), view, TRUE, TRUE, 0);
	window = GTK_WINDOW(gtk_builder_get_object(builder, "top_window"));
	//
	//
	//

	time_entries =
		(GtkWidget **) g_malloc(MAX_NUMOF_DATA_PTS *
								sizeof(GtkWidget *));

	perc_entries =
		(GtkWidget **) g_malloc(MAX_NUMOF_DATA_PTS *
								sizeof(GtkWidget *));
	models_params =
		(GtkWidget **) g_malloc(NUMOF_MODELS_PARAMS * sizeof(GtkWidget *));
	load_io_widget_handles(time_entries, perc_entries, models_params);
	//
	//
	//
	gtk_builder_connect_signals(builder, NULL);
	g_object_unref(builder);
	g_signal_connect(window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);
	gtk_widget_show_all(GTK_WIDGET(window));
	gtk_main();
	return;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		//fprintf(stderr, BASIC_ERROR_MESSAGE);
		//help_version();
		//exit(EXIT_FAILURE);
		gui_driver(argc, argv);
		exit(EXIT_SUCCESS);
	}
	int c;
	while ((c = getopt(argc, argv, ":hvgf:?")) != -1) {
		switch (c) {
		case 'h':
			help_version();
			exit(EXIT_SUCCESS);
			break;
		case 'v':
			help_version();
			exit(EXIT_SUCCESS);
			break;
		case 'g':
			gui_driver(argc, argv);
			exit(EXIT_SUCCESS);
			break;
		case 'f':
			if (argc == 3) {
				fmf_print_params(optarg);
				exit(EXIT_SUCCESS);
			} else {
				fprintf(stderr, BASIC_ERROR_MESSAGE);
				help_version();
				exit(EXIT_FAILURE);
			}
			break;
		case '?':
			fprintf(stderr, BASIC_ERROR_MESSAGE);
			help_version();
			exit(EXIT_FAILURE);
			break;
		case ':':
			fprintf(stderr, BASIC_ERROR_MESSAGE);
			help_version();
			exit(EXIT_FAILURE);
			break;
		default:
			fprintf(stderr, BASIC_ERROR_MESSAGE);
			help_version();
			exit(EXIT_FAILURE);
			break;
		}
	}
	return EXIT_SUCCESS;
}

void help_version()
{
	puts("Basic usage: ./farmafit -f example.json,");
	puts("where example.json contains experimental measurement data.");
	puts("For further help and version information see README.md");
	return;
}

void
G_MODULE_EXPORT on_btn_clear_clicked()
{
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		gtk_entry_set_text(GTK_ENTRY(time_entries[i]), "");
		gtk_entry_set_text(GTK_ENTRY(perc_entries[i]), "");
	}
	gtk_entry_set_text(GTK_ENTRY(time_entries[0]), "0");
	gtk_entry_set_text(GTK_ENTRY(perc_entries[0]), "0");
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		gtk_label_set_text(GTK_LABEL(models_params[i]), MSG_NOT_CALCULATED);
	}
	if (done_calculating_and_plotting == 1) {
		slope_scale_remove_item(scale, series_md);
		/* Four -1's are key to clearing the graph properly!  */
		slope_scale_set_layout_rect(scale, -1, -1, -1, -1);
		slope_view_redraw(SLOPE_VIEW(view));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_zo), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_fo), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_he), FALSE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tick_pe), FALSE);
		free(data_set);
	}
	done_calculating_and_plotting = 0;
	return;
}

void
G_MODULE_EXPORT on_tick_zo_toggled(GtkWidget * widget, gpointer data)
{
	if (data) {
		;
	}
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		slope_scale_remove_item(scale, series_zo);
	else
		slope_scale_add_item(scale, series_zo);
	slope_view_redraw(SLOPE_VIEW(view));
	return;
}

void
G_MODULE_EXPORT on_tick_fo_toggled(GtkWidget * widget, gpointer data)
{
	if (data) {
		;
	}
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		slope_scale_remove_item(scale, series_fo);
	else
		slope_scale_add_item(scale, series_fo);
	slope_view_redraw(SLOPE_VIEW(view));
	return;
}

void
G_MODULE_EXPORT on_tick_he_toggled(GtkWidget * widget, gpointer data)
{
	if (data) {
		;
	}
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		slope_scale_remove_item(scale, series_he);
	else
		slope_scale_add_item(scale, series_he);
	slope_view_redraw(SLOPE_VIEW(view));
	return;
}

void
G_MODULE_EXPORT on_tick_pe_toggled(GtkWidget * widget, gpointer data)
{
	if (data) {
		;
	}
	if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
		slope_scale_remove_item(scale, series_pe);
	else
		slope_scale_add_item(scale, series_pe);
	slope_view_redraw(SLOPE_VIEW(view));
	return;
}

void
G_MODULE_EXPORT on_btn_load_example_clicked()
{
	char number_as_string[STR_LEN_FOR_CONVERSION];
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		snprintf(number_as_string, BASE_10, "%.0f",
				 example_data_time_values[i]);
		gtk_entry_set_text(GTK_ENTRY(time_entries[i]), number_as_string);
		snprintf(number_as_string, BASE_10, "%.3f",
				 example_data_percentage_values[i]);
		gtk_entry_set_text(GTK_ENTRY(perc_entries[i]), number_as_string);
	}
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		gtk_label_set_text(GTK_LABEL(models_params[i]), MSG_NOT_CALCULATED);
	}
	return;
}

void generate_plots()
{
	struct dp *current_data_pt;
	int numof_data_pts = 0;
	if (data_set != NULL) {
		++numof_data_pts;
		current_data_pt = data_set;
		while (current_data_pt->next != NULL) {
			++numof_data_pts;
			current_data_pt = current_data_pt->next;
		}
	}
	double *data_time_values =
		(double *)g_malloc(numof_data_pts * sizeof(double));
	double *data_percentage_values =
		(double *)g_malloc(numof_data_pts * sizeof(double));
	SlopeSample *data_time_ticks;
	data_time_ticks =
		(SlopeSample *) g_malloc(numof_data_pts * sizeof(SlopeSample));
	current_data_pt = data_set;
	int max_minutes = 0;
	char *aux;
	for (int i = 0; i < numof_data_pts; ++i) {
		//printf("%f\t%f\n", current_data_pt->mins, current_data_pt->perc);
		data_time_values[i] = current_data_pt->mins;
		data_percentage_values[i] = current_data_pt->perc;
		if (data_time_values[i] > max_minutes) {
			max_minutes = data_time_values[i];
		}
		data_time_ticks[i].coord = data_time_values[i];
		aux = (char *)g_malloc(STR_LEN_FOR_CONVERSION * sizeof(char));
		sprintf(aux, "%d", (int)data_time_values[i]);
		data_time_ticks[i].label = aux;
		current_data_pt = current_data_pt->next;
		//printf("%f\t%f\n", data_time_values[i], data_percentage_values[i]);
	}
	scale = slope_xyscale_new();
	slope_figure_add_scale(SLOPE_FIGURE(figure), scale);
	slope_scale_set_layout_rect(scale, 0, 0, 1, 1);
	/* Beware: Changing from 12 to 14 below does funny things!  */
	//series_md = slope_xyseries_new_filled (LBL_MEASURED_DATA, example_data_time_values, example_data_percentage_values, 12, "kor");
	series_md =
		slope_xyseries_new_filled(LBL_MEASURED_DATA, data_time_values,
								  data_percentage_values, 12, "kor");
	slope_scale_add_item(scale, series_md);
	/* Zero-order kinetics  */
	double *x_zo, *y_zo;
	x_zo = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	y_zo = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) {
		x_zo[i] = (double)i *max_minutes / (NUMOF_PLOT_POINTS - 1);
		y_zo[i] = models_params_struct.k0 * x_zo[i];
	}
	series_zo =
		slope_xyseries_new_filled(LBL_ZERO_ORDER_KINETICS, x_zo, y_zo,
								  NUMOF_PLOT_POINTS, "r-");
	/* First-order kinetics  */
	double *x_fo, *y_fo;
	x_fo = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	y_fo = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) {
		x_fo[i] = (double)i *max_minutes / (NUMOF_PLOT_POINTS - 1);
		y_fo[i] = (1 - exp(-1 * models_params_struct.k1 * x_fo[i])) * 100;
	}
	series_fo =
		slope_xyseries_new_filled(LBL_FIRST_ORDER_KINETICS, x_fo, y_fo,
								  NUMOF_PLOT_POINTS, "m-");
	/* Higuchi's equation  */
	double *x_he, *y_he;
	x_he = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	y_he = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) {
		x_he[i] = (double)i *max_minutes / (NUMOF_PLOT_POINTS - 1);
		y_he[i] = models_params_struct.kh * sqrt(x_he[i]);
	}
	series_he =
		slope_xyseries_new_filled(LBL_HIGUCHIS_EQUATION, x_he, y_he,
								  NUMOF_PLOT_POINTS, "g-");
	/* Peppas' equation  */
	double *x_pe, *y_pe;
	x_pe = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	y_pe = g_malloc(NUMOF_PLOT_POINTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_POINTS; ++i) {
		x_pe[i] = (double)i *max_minutes / (NUMOF_PLOT_POINTS - 1);
		y_pe[i] =
			models_params_struct.k * pow(x_pe[i], models_params_struct.tn);
	}
	series_pe =
		slope_xyseries_new_filled(LBL_PEPPAS_EQUATION, x_pe, y_pe,
								  NUMOF_PLOT_POINTS, "b-");
	/* Setting up the graph  */
	SlopeItem *x_axis;
	x_axis =
		slope_xyscale_get_axis(SLOPE_XYSCALE(scale), SLOPE_XYSCALE_AXIS_BOTTOM);
	SlopeSampler *x_sampler;
	x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
	slope_sampler_set_samples(x_sampler, data_time_ticks, numof_data_pts);
	slope_view_redraw(SLOPE_VIEW(view));
	return;
}

void fill_out_labels()
{
	char number[STR_LEN_FOR_CONVERSION];
	snprintf(number, BASE_10, "%.4f", models_params_struct.k0);
	gtk_label_set_text(GTK_LABEL(models_params[0]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k0);
	gtk_label_set_text(GTK_LABEL(models_params[1]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.k1);
	gtk_label_set_text(GTK_LABEL(models_params[2]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k1);
	gtk_label_set_text(GTK_LABEL(models_params[3]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.kh);
	gtk_label_set_text(GTK_LABEL(models_params[4]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_kh);
	gtk_label_set_text(GTK_LABEL(models_params[5]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.k);
	gtk_label_set_text(GTK_LABEL(models_params[6]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.tn);
	gtk_label_set_text(GTK_LABEL(models_params[7]), number);
	snprintf(number, BASE_10, "%.4f", models_params_struct.rsq_k);
	gtk_label_set_text(GTK_LABEL(models_params[8]), number);
	return;
}

void
G_MODULE_EXPORT show_msg_box(char *msg_text)
{
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new(window,
									flags,
									GTK_MESSAGE_ERROR,
									GTK_BUTTONS_CLOSE, "%s", msg_text);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void load_io_widget_handles(GtkWidget **time_entries, GtkWidget **perc_entries, GtkWidget **models_params)
{
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		time_entries[i] = GTK_WIDGET(gtk_builder_get_object(builder, time_entry_name[i]));
		perc_entries[i] = GTK_WIDGET(gtk_builder_get_object(builder, perc_entry_name[i]));
	}
	gtk_entry_set_text(GTK_ENTRY(time_entries[0]), "0");
	gtk_entry_set_text(GTK_ENTRY(perc_entries[0]), "0");
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		models_params[i] = GTK_WIDGET(gtk_builder_get_object(builder, models_params_names[i]));
	}
	/* TODO Change this into an array  */
	tick_zo = (GtkWidget *) g_malloc(sizeof(GtkWidget *));
	tick_zo = GTK_WIDGET(gtk_builder_get_object(builder, "tick_zo"));
	tick_fo = (GtkWidget *) g_malloc(sizeof(GtkWidget *));
	tick_fo = GTK_WIDGET(gtk_builder_get_object(builder, "tick_fo"));
	tick_he = (GtkWidget *) g_malloc(sizeof(GtkWidget *));
	tick_he = GTK_WIDGET(gtk_builder_get_object(builder, "tick_he"));
	tick_pe = (GtkWidget *) g_malloc(sizeof(GtkWidget *));
	tick_pe = GTK_WIDGET(gtk_builder_get_object(builder, "tick_pe"));
}

/* TODO Change to gbool?  */
int digits_or_single_pt_only(char *entry)
{
	int all_good_flag = 1;
	int numof_pts = 0;
	size_t str_len = strlen(entry);
	for (unsigned int i = 0; i < str_len; ++i) {
		if (entry[i] == '.')
			++numof_pts;
	}
	if (numof_pts > 1) {
		return 0;
	}
	for (unsigned int i = 0; i < str_len; ++i) {
		if (!(isdigit(entry[i]) || entry[i] == '.')) {
			return 0;
		}
	}
	return all_good_flag;
}

/* TODO Change to gbool?  */
int digits_only(char *entry)
{
	int all_good_flag = 1;
	size_t str_len = strlen(entry);
	for (unsigned int i = 0; i < str_len; ++i) {
		if (!(isdigit(entry[i]))) {
			return 0;
		}
	}
	return all_good_flag;
}
