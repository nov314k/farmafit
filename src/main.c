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
#include <stdbool.h>

/*
 * TODO Change variable names to suit standard names
 * TODO Check if all proper Gtk types are used, to reduce type casting
 * TODO Ensure all floating point values for percentages are to three decimal
 * places as it seems to add other values that it shouldn't
 * TODO Change _Boolean to gboolean
 * TODO Check all places where creation of new objects is done
 * TODO
 **/

#define NUMOF_MODELS_PARAMS 9
#define MAX_NUMOF_DATA_PTS 12
#define NUMOF_PLOT_PTS 101
#define NUMOF_MODELS 4
#define MSG_NOT_CALCED "Not calculated"
#define ERR_INCORR_ENTRY "ERROR: Incorrect entry."
#define ERR_INSUFF_DATA_PTS "ERROR: Enter at least 3 valid data points."
#define ERR_CALC_FIRST "ERROR: Please calculate models parameters first."
#define STR_LEN_FOR_CONVERSION 6
#define LBL_MEASURED_DATA "Measured data"
#define LBL_ZERO_ORDER_KINETICS "Zero-order kinetics"
#define LBL_FIRST_ORDER_KINETICS "First-order kinetics"
#define LBL_HIGUCHIS_EQUATION "Higuchi's equation"
#define LBL_PEPPAS_EQUATION "Peppas' equation"
#define GLADE_FILE_NAME "farmafit.glade"
#define KEY_BOX "key_box"
static const char *time_entries_names[] = {
	"t00", "t01", "t02", "t03", "t04", "t05", "t06", "t07", "t08", "t09", "t10", "t11"
};
static const char *perc_entries_names[] = {
	"p00", "p01", "p02", "p03", "p04", "p05", "p06", "p07", "p08", "p09", "p10", "p11"
};
static const char *models_params_names[] = {
	"k0", "rsq_k0", "k1", "rsq_k1", "kh", "rsq_kh", "k", "n", "rsq_k"
};
static const char *ticks_names[] ={
	"tick_zo", "tick_fo", "tick_he", "tick_pe"
};
static const double eg_data_time_vals[] = {
	0, 15, 30, 45, 60, 90, 120, 150, 180, 240, 300, 360
};
static const double eg_data_perc_vals[] = {
	0, 8.803, 11.935, 15.316, 19.195, 28.426,
	35.830, 41.672, 49.569, 62.338, 72.883, 79.588
};

struct app {
	SlopeScale *scale;
	SlopeFigure *figure;
	SlopeItem *series_md;
	SlopeItem *series_pe;
	SlopeItem *series_zo;
	SlopeItem *series_fo;
	SlopeItem *series_he;
	GtkWidget *view;
	GtkWidget **time_entries;
	GtkWidget **perc_entries;
	GtkWidget **models_params;
	GtkWindow *window;
	GtkWidget *ticks[NUMOF_MODELS];
	struct models_params models_params_vals;
};

static struct dp *data_set;
static _Bool done_calculating_and_plotting = false;

void help_version();
void G_MODULE_EXPORT on_btn_clear_clicked(GtkWidget *widget, struct app *app);
void G_MODULE_EXPORT on_btn_load_eg_clicked(GtkWidget *widget, struct app *app);
void G_MODULE_EXPORT on_tick_zo_toggled(GtkWidget *widget, struct app *app);
void G_MODULE_EXPORT on_tick_fo_toggled(GtkWidget *widget, struct app *app);
void G_MODULE_EXPORT on_tick_he_toggled(GtkWidget *widget, struct app *app);
void G_MODULE_EXPORT on_tick_pe_toggled(GtkWidget *widget, struct app *app);

void destroy (GtkWidget *window);
gboolean delete_event(GtkWidget *window, GdkEvent *event);

void generate_plots(struct app *app);
void fill_out_labels(struct models_params models_params_vals, struct app *app);

void G_MODULE_EXPORT show_msg_box(char *, GtkWindow *window);
int digits_or_single_pt_only(char *);
int digits_only(char *entry);

void
G_MODULE_EXPORT on_btn_calc_and_plot_clicked(GtkWidget *widget, struct app *app)
{
	if (done_calculating_and_plotting) {
		slope_scale_remove_item(app->scale, app->series_md);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[0]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[0]), FALSE);
			slope_scale_remove_item(app->scale, app->series_zo);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[1]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[1]), FALSE);
			slope_scale_remove_item(app->scale, app->series_fo);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[2]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[2]), FALSE);
			slope_scale_remove_item(app->scale, app->series_he);
		}
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[3]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[3]), FALSE);
			slope_scale_remove_item(app->scale, app->series_pe);
		}
		/* Four -1's are key to clearing the graph properly!  */
		slope_scale_set_layout_rect(app->scale, -1, -1, -1, -1);
		slope_view_redraw(SLOPE_VIEW(app->view));
		g_free(data_set);
	}
	data_set = (struct dp *)g_malloc(sizeof(struct dp));
	fmf_init_data_set(data_set);
	struct dp *current_point = data_set;
	unsigned int numof_valid_pts = 0;
	char *aux_t = (char *)g_malloc(STR_LEN_FOR_CONVERSION * sizeof(char));
	char *aux_p = (char *)g_malloc(STR_LEN_FOR_CONVERSION * sizeof(char));
	/* Cast from const char * to avoid compiler warnings  */
	aux_t = (char *)gtk_entry_get_text(GTK_ENTRY(app->time_entries[0]));
	if (!digits_only(aux_t)) {
		show_msg_box(ERR_INCORR_ENTRY, app->window);
		gtk_widget_grab_focus(app->time_entries[0]);
		return;
	}
	aux_p = (char *)gtk_entry_get_text(GTK_ENTRY(app->perc_entries[0]));
	if (!digits_or_single_pt_only(aux_p)) {
		show_msg_box(ERR_INCORR_ENTRY, app->window);
		gtk_widget_grab_focus(app->perc_entries[0]);
		return;
	}
	current_point->mins = atof(aux_t);
	current_point->perc = atof(aux_p);
	++numof_valid_pts;
	_Bool found_the_end = false;
	for (unsigned int i = 1; i < MAX_NUMOF_DATA_PTS && !found_the_end; ++i) {
		aux_t = (char *)gtk_entry_get_text(GTK_ENTRY(app->time_entries[i]));
		if (!digits_only(aux_t)) {
			show_msg_box(ERR_INCORR_ENTRY, app->window);
			gtk_widget_grab_focus(app->time_entries[i]);
			return;
		}
		aux_p = (char *)gtk_entry_get_text(GTK_ENTRY(app->perc_entries[i]));
		if (!digits_or_single_pt_only(aux_p)) {
			show_msg_box(ERR_INCORR_ENTRY, app->window);
			gtk_widget_grab_focus(app->perc_entries[i]);
			return;
		}
		if (atof(aux_t) == 0 || atof(aux_p) == 0 || strlen(aux_t) == 0
			|| strlen(aux_p) == 0) {
			found_the_end = true;
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
		app->models_params_vals = fmf_calc_params(data_set);
		/*Change this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		fill_out_labels(app->models_params_vals, app);
		generate_plots(app);
		done_calculating_and_plotting = true;
	} else {
		show_msg_box(ERR_INSUFF_DATA_PTS, app->window);
		gtk_widget_grab_focus(app->time_entries[numof_valid_pts]);
	}
	return;
}

void load_and_start_gui(int argc, char *argv[])
{
	struct app app;
	gtk_init(&argc, &argv);
	/*******/
	GtkBuilder *builder;
	builder = gtk_builder_new();
	gtk_builder_add_from_file(builder, GLADE_FILE_NAME, NULL);
	app.window = GTK_WINDOW(gtk_builder_get_object(builder, "top_window"));
	/*******/
	GtkWidget *box;
	box = GTK_WIDGET(gtk_builder_get_object(builder, KEY_BOX));
	/*******/
	app.view = slope_view_new();
	app.figure = slope_figure_new();
	slope_view_set_figure(SLOPE_VIEW(app.view), app.figure);
	gtk_box_pack_start(GTK_BOX(box), app.view, TRUE, TRUE, 0);
	/*******/
	app.time_entries = (GtkWidget **) g_malloc(MAX_NUMOF_DATA_PTS * sizeof(GtkWidget *));
	app.perc_entries = (GtkWidget **) g_malloc(MAX_NUMOF_DATA_PTS * sizeof(GtkWidget *));
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		app.time_entries[i] = GTK_WIDGET(gtk_builder_get_object(builder, time_entries_names[i]));
		app.perc_entries[i] = GTK_WIDGET(gtk_builder_get_object(builder, perc_entries_names[i]));
	}
	app.models_params = (GtkWidget **) g_malloc(NUMOF_MODELS_PARAMS * sizeof(GtkWidget *));
	gtk_entry_set_text(GTK_ENTRY(app.time_entries[0]), "0");
	gtk_entry_set_text(GTK_ENTRY(app.perc_entries[0]), "0");
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		app.models_params[i] = GTK_WIDGET(gtk_builder_get_object(builder, models_params_names[i]));
	}
	for (int i = 0; i < NUMOF_MODELS; ++i) {
		app.ticks[i] = gtk_check_button_new();
		app.ticks[i] = GTK_WIDGET(gtk_builder_get_object(builder, ticks_names[i]));
	}
	/*******/
	GtkWidget *btn_load_eg;
	btn_load_eg = GTK_WIDGET(gtk_builder_get_object(builder, "btn_load_example"));
	GtkWidget *btn_calc_and_plot;
	btn_calc_and_plot = GTK_WIDGET(gtk_builder_get_object(builder, "btn_calculate_and_plot"));
	GtkWidget *btn_clear;
	btn_clear = GTK_WIDGET(gtk_builder_get_object(builder, "btn_clear"));
	GtkWidget *btn_close;
	btn_close = GTK_WIDGET(gtk_builder_get_object(builder, "btn_close"));
	/*******/
	/* TODO Consider removing the line below  */
	//gtk_builder_connect_signals(builder, NULL);
	g_signal_connect(btn_load_eg, "clicked", G_CALLBACK(on_btn_load_eg_clicked), &app);
	g_signal_connect(btn_calc_and_plot, "clicked", G_CALLBACK(on_btn_calc_and_plot_clicked), &app);
	g_signal_connect(btn_clear, "clicked", G_CALLBACK(on_btn_clear_clicked), &app);
	/* TODO Check the best way to exit the application  */
	g_signal_connect(btn_close, "clicked", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect (app.window, "destroy", G_CALLBACK(destroy), NULL);
	g_signal_connect (app.window, "delete_event", G_CALLBACK (delete_event), NULL);


	g_signal_connect(app.ticks[0], "toggled", G_CALLBACK(on_tick_zo_toggled), &app);
	g_signal_connect(app.ticks[1], "toggled", G_CALLBACK(on_tick_fo_toggled), &app);
	g_signal_connect(app.ticks[2], "toggled", G_CALLBACK(on_tick_he_toggled), &app);
	g_signal_connect(app.ticks[3], "toggled", G_CALLBACK(on_tick_pe_toggled), &app);
	/*******/
	g_object_unref(builder);
	gtk_widget_show_all(GTK_WIDGET(app.window));
	gtk_main();
	return;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		//fprintf(stderr, BASIC_ERROR_MESSAGE);
		//help_version();
		//exit(EXIT_FAILURE);
		load_and_start_gui(argc, argv);
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
			load_and_start_gui(argc, argv);
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
G_MODULE_EXPORT on_btn_clear_clicked(GtkWidget *widget, struct app *app)
{
	for (int i = 0; i < NUMOF_MODELS; ++i) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[i]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[i]), FALSE);
		}
	}
	/*******/
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		gtk_entry_set_text(GTK_ENTRY(app->time_entries[i]), "");
		gtk_entry_set_text(GTK_ENTRY(app->perc_entries[i]), "");
	}
	/*******/
	gtk_entry_set_text(GTK_ENTRY(app->time_entries[0]), "0");
	gtk_entry_set_text(GTK_ENTRY(app->perc_entries[0]), "0");
	/*******/
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		gtk_label_set_text(GTK_LABEL(app->models_params[i]), MSG_NOT_CALCED);
	}
	/*******/
	if (done_calculating_and_plotting == 1) {
		slope_scale_remove_item(app->scale, app->series_md);
		/* Four -1's are key to clearing the graph properly!  */
		slope_scale_set_layout_rect(app->scale, -1, -1, -1, -1);
		slope_view_redraw(SLOPE_VIEW(app->view));
		/* TODO Check the need for this  */
		free(data_set);
	}
	/*******/
	done_calculating_and_plotting = false;
	return;
}


void
G_MODULE_EXPORT on_tick_zo_toggled(GtkWidget *widget, struct app *app)
{
	if (done_calculating_and_plotting) {
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			slope_scale_remove_item(app->scale, app->series_zo);
		} else {
			slope_scale_add_item(app->scale, app->series_zo);
		}
		slope_view_redraw(SLOPE_VIEW(app->view));
	/* Condition (if) below is to prevent message box popping up twice.  */
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
	} else {
		show_msg_box(ERR_CALC_FIRST, app->window);
	}
	return;
}

void
G_MODULE_EXPORT on_tick_fo_toggled(GtkWidget *widget, struct app *app)
{
	if (done_calculating_and_plotting) {
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			slope_scale_remove_item(app->scale, app->series_fo);
		} else {
			slope_scale_add_item(app->scale, app->series_fo);
		}
		slope_view_redraw(SLOPE_VIEW(app->view));
	/* Condition (if) below is to prevent message box popping up twice.  */
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
	} else {
		show_msg_box(ERR_CALC_FIRST, app->window);
	}
	return;
}

void
G_MODULE_EXPORT on_tick_he_toggled(GtkWidget *widget, struct app *app)
{
	if (done_calculating_and_plotting) {
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			slope_scale_remove_item(app->scale, app->series_he);
		} else {
			slope_scale_add_item(app->scale, app->series_he);
		}
		slope_view_redraw(SLOPE_VIEW(app->view));
	/* Condition (if) below is to prevent message box popping up twice.  */
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
	} else {
		show_msg_box(ERR_CALC_FIRST, app->window);
	}
	return;
}

void
G_MODULE_EXPORT on_tick_pe_toggled(GtkWidget *widget, struct app *app)
{
	if (done_calculating_and_plotting) {
		if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
			slope_scale_remove_item(app->scale, app->series_pe);
		} else {
			slope_scale_add_item(app->scale, app->series_pe);
		}
		slope_view_redraw(SLOPE_VIEW(app->view));
	/* Condition (if) below is to prevent message box popping up twice.  */
	} else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), FALSE);
	} else {
		show_msg_box(ERR_CALC_FIRST, app->window);
	}
	return;
}

void
G_MODULE_EXPORT on_btn_load_eg_clicked(GtkWidget *widget, struct app *app)
{
	for (int i = 0; i < NUMOF_MODELS; ++i) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(app->ticks[i]))) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(app->ticks[i]), FALSE);
		}
	}
	/*******/
	char number_as_string[STR_LEN_FOR_CONVERSION];
	for (int i = 0; i < MAX_NUMOF_DATA_PTS; ++i) {
		sprintf(number_as_string, "%.0f", eg_data_time_vals[i]);
		gtk_entry_set_text(GTK_ENTRY(app->time_entries[i]), number_as_string);
		sprintf(number_as_string, "%.3f", eg_data_perc_vals[i]);
		gtk_entry_set_text(GTK_ENTRY(app->perc_entries[i]), number_as_string);
	}
	/*******/
	for (int i = 0; i < NUMOF_MODELS_PARAMS; ++i) {
		gtk_label_set_text(GTK_LABEL(app->models_params[i]), MSG_NOT_CALCED);
	}
	/*******/
	done_calculating_and_plotting = false;
	return;
}

void generate_plots(struct app *app)
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
	app->scale = slope_xyscale_new();
	slope_figure_add_scale(SLOPE_FIGURE(app->figure), app->scale);
	slope_scale_set_layout_rect(app->scale, 0, 0, 1, 1);
	/* Beware: Changing from 12 to 14 below does funny things!  */
	//series_md = slope_xyseries_new_filled (LBL_MEASURED_DATA, example_data_time_values, example_data_percentage_values, 12, "kor");
	app->series_md =
		slope_xyseries_new_filled(LBL_MEASURED_DATA, data_time_values,
								  data_percentage_values, 12, "kor");
	slope_scale_add_item(app->scale, app->series_md);
	/* Zero-order kinetics  */
	double *x_zo, *y_zo;
	x_zo = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	y_zo = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_PTS; ++i) {
		x_zo[i] = (double)i *max_minutes / (NUMOF_PLOT_PTS - 1);
		y_zo[i] = app->models_params_vals.k0 * x_zo[i];
	}
	app->series_zo =
		slope_xyseries_new_filled(LBL_ZERO_ORDER_KINETICS, x_zo, y_zo,
								  NUMOF_PLOT_PTS, "r-");
	/* First-order kinetics  */
	double *x_fo, *y_fo;
	x_fo = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	y_fo = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_PTS; ++i) {
		x_fo[i] = (double)i *max_minutes / (NUMOF_PLOT_PTS - 1);
		y_fo[i] = (1 - exp(-1 * app->models_params_vals.k1 * x_fo[i])) * 100;
	}
	app->series_fo =
		slope_xyseries_new_filled(LBL_FIRST_ORDER_KINETICS, x_fo, y_fo,
								  NUMOF_PLOT_PTS, "m-");
	/* Higuchi's equation  */
	double *x_he, *y_he;
	x_he = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	y_he = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_PTS; ++i) {
		x_he[i] = (double)i *max_minutes / (NUMOF_PLOT_PTS - 1);
		y_he[i] = app->models_params_vals.kh * sqrt(x_he[i]);
	}
	app->series_he =
		slope_xyseries_new_filled(LBL_HIGUCHIS_EQUATION, x_he, y_he,
								  NUMOF_PLOT_PTS, "g-");
	/* Peppas' equation  */
	double *x_pe, *y_pe;
	x_pe = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	y_pe = g_malloc(NUMOF_PLOT_PTS * sizeof(double));
	for (int i = 0; i < NUMOF_PLOT_PTS; ++i) {
		x_pe[i] = (double)i *max_minutes / (NUMOF_PLOT_PTS - 1);
		y_pe[i] =
			app->models_params_vals.k * pow(x_pe[i], app->models_params_vals.tn);
	}
	app->series_pe =
		slope_xyseries_new_filled(LBL_PEPPAS_EQUATION, x_pe, y_pe,
								  NUMOF_PLOT_PTS, "b-");
	/* Setting up the graph  */
	SlopeItem *x_axis;
	x_axis =
		slope_xyscale_get_axis(SLOPE_XYSCALE(app->scale), SLOPE_XYSCALE_AXIS_BOTTOM);
	SlopeSampler *x_sampler;
	x_sampler = slope_xyaxis_get_sampler(SLOPE_XYAXIS(x_axis));
	slope_sampler_set_samples(x_sampler, data_time_ticks, numof_data_pts);
	slope_view_redraw(SLOPE_VIEW(app->view));
	return;
}

void fill_out_labels(struct models_params models_params_vals, struct app *app)
{
	char number[STR_LEN_FOR_CONVERSION];
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.k0);
	gtk_label_set_text(GTK_LABEL(app->models_params[0]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.rsq_k0);
	gtk_label_set_text(GTK_LABEL(app->models_params[1]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.k1);
	gtk_label_set_text(GTK_LABEL(app->models_params[2]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.rsq_k1);
	gtk_label_set_text(GTK_LABEL(app->models_params[3]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.kh);
	gtk_label_set_text(GTK_LABEL(app->models_params[4]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.rsq_kh);
	gtk_label_set_text(GTK_LABEL(app->models_params[5]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.k);
	gtk_label_set_text(GTK_LABEL(app->models_params[6]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.tn);
	gtk_label_set_text(GTK_LABEL(app->models_params[7]), number);
	snprintf(number, STR_LEN_FOR_CONVERSION, "%.4f", models_params_vals.rsq_k);
	gtk_label_set_text(GTK_LABEL(app->models_params[8]), number);
	return;
}

void
G_MODULE_EXPORT show_msg_box(char *msg_text, GtkWindow *window)
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


void destroy (GtkWidget *window)
{
	gtk_main_quit ();
}

gboolean delete_event(GtkWidget *window, GdkEvent *event)
{
	return FALSE;
}
