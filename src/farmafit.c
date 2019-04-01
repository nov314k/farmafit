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
 * @file farmafit.c
 * @author Novak Petrovic
 * @date 2019
 * @brief Fitting and other functions used by Farmafit.
 *
 * @see farmafit.h for descriptions of parameters and return values.
 * @see README (or README.md) for more details.
 */
#include "data_types.h"
#include "farmafit.h"
#include "../lib/cJSON/cJSON.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

float fmf_armean(float *data, int size)
{
	float total = 0.0;
	for (int i = 0; i < size; total += data[i], i++) ;
	return total / size;
}

float fmf_moprods(float *data1, float *data2, int size)
{
	float total = 0.0;
	for (int i = 0; i < size; total += (data1[i] * data2[i]), i++) ;
	return total / size;
}

float fmf_variance(float *data, int size)
{
	float squares[size];
	for (int i = 0; i < size; i++) {
		squares[i] = pow(data[i], 2);
	}
	float mean_of_squares = fmf_armean(squares, size);
	float mean = fmf_armean(data, size);
	float square_of_mean = pow(mean, 2);
	float variance = mean_of_squares - square_of_mean;
	return variance;
}

void
fmf_calc_linreg(float *independent, float *dependent, int size,
				struct lr *linreg)
{
	float independent_armean = fmf_armean(independent, size);
	float dependent_armean = fmf_armean(dependent, size);
	float mean_of_products = fmf_moprods(independent, dependent, size);
	float independent_variance = fmf_variance(independent, size);
	linreg->a =
		(mean_of_products -
		 independent_armean * dependent_armean) / independent_variance;
	linreg->b = dependent_armean - (linreg->a * independent_armean);
}

float fmf_calc_rsq(float *x, float *y, int size)
{
	float x_mean = fmf_armean(x, size);
	float y_mean = fmf_armean(y, size);
	float top = 0.0;
	float bottom2x = 0.0;
	float bottom2y = 0.0;
	for (int i = 0; i < size; i++) {
		top += (x[i] - x_mean) * (y[i] - y_mean);
		bottom2x += pow(x[i] - x_mean, 2);
		bottom2y += pow(y[i] - y_mean, 2);
	}
	float r = top / sqrt(bottom2x * bottom2y);
	return pow(r, 2);
}

int fmf_gtdpts(const char *const data_str, struct dp *head)
{
	const cJSON *data_point = NULL;
	const cJSON *data_points = NULL;
	const cJSON *experiment_name = NULL;
	cJSON *data_json = cJSON_Parse(data_str);
	if (data_json == NULL) {
		const char *error_ptr = cJSON_GetErrorPtr();
		if (error_ptr != NULL) {
			fprintf(stderr, "Error before: %s\n", error_ptr);
		}
		cJSON_Delete(data_json);
		return EXIT_FAILURE;
	}
	experiment_name = cJSON_GetObjectItemCaseSensitive
		(data_json, "experiment_name");
	if (cJSON_IsString(experiment_name)
		&& (experiment_name->valuestring != NULL)) {
		printf("\nResults of processing \"%s\"\n\n",
			   experiment_name->valuestring);
	}
	data_points = cJSON_GetObjectItemCaseSensitive(data_json, "data_points");
	cJSON_ArrayForEach(data_point, data_points) {
		cJSON *mins = cJSON_GetObjectItemCaseSensitive(data_point, "minutes");
		cJSON *percentage = cJSON_GetObjectItemCaseSensitive
			(data_point, "percentage");
		if (cJSON_IsNumber(mins) && cJSON_IsNumber(percentage)) {
			if (head->mins == VALUE_NOT_SET && head->perc == VALUE_NOT_SET) {
				head->mins = mins->valuedouble;
				head->perc = percentage->valuedouble;
				head->next = NULL;
			} else {
				struct dp *last = head;
				while (last->next != NULL)
					last = last->next;
				last->next = (struct dp *)malloc(sizeof(struct dp));
				last->next->mins = mins->valuedouble;
				last->next->perc = percentage->valuedouble;
				last->next->next = NULL;
		}}
	} cJSON_Delete(data_json);
	return EXIT_SUCCESS;
}

void fmf_file2str(char *file_name, char *data_str)
{
	*data_str = 0;
	long length;
	FILE *f = fopen(file_name, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		int magic_value = 3;	/* Be very careful about this!  */
		data_str = malloc(length + magic_value);
		if (data_str) {
			fread(data_str, 1, length, f);
		}
		fclose(f);
	}
	puts("aeiti");
	return;
}

struct models_params fmf_calc_params(struct dp *data_set)
{
	int numof_data_points = 0;
	struct dp *cur = data_set;
	while (cur != NULL) {
		numof_data_points++;
		cur = cur->next;
	}
	float independent[numof_data_points];
	float dependent[numof_data_points];
	int i = 0;
	cur = data_set;
	while (cur != NULL) {
		independent[i] = cur->mins;
		dependent[i] = cur->perc;
		i++;
		cur = cur->next;
	}
	struct lr *linreg = (struct lr *)malloc(sizeof(struct lr));
	fmf_calc_linreg(independent, dependent, numof_data_points, linreg);
	struct models_params models_params;
	models_params.k0 = linreg->a;
	models_params.rsq_k0 =
		fmf_calc_rsq(independent, dependent, numof_data_points);
	float x_axis_shorter[numof_data_points - 1];
	float y_axis_shorter[numof_data_points - 1];
	for (int i = 0; i < numof_data_points - 1; i++) {
		x_axis_shorter[i] = independent[i + 1];
		y_axis_shorter[i] = log(dependent[i + 1]);
	}
	fmf_calc_linreg(x_axis_shorter, y_axis_shorter, numof_data_points - 1,
					linreg);
	models_params.k1 = linreg->a;
	models_params.rsq_k1 = fmf_calc_rsq(x_axis_shorter, y_axis_shorter,
										numof_data_points - 1);
	float x_axis[numof_data_points];
	float y_axis[numof_data_points];
	for (int i = 0; i < numof_data_points; i++) {
		x_axis[i] = sqrt(independent[i]);
		y_axis[i] = dependent[i];
	}
	fmf_calc_linreg(x_axis, y_axis, numof_data_points, linreg);
	models_params.kh = linreg->a;
	models_params.rsq_kh = fmf_calc_rsq(x_axis, y_axis, numof_data_points);
	for (int i = 0; i < numof_data_points - 1; i++) {
		x_axis_shorter[i] = log(independent[i + 1]);
		y_axis_shorter[i] = log(dependent[i + 1]);
	}
	fmf_calc_linreg(x_axis_shorter, y_axis_shorter, numof_data_points - 1,
					linreg);
	models_params.k = exp(linreg->b);
	models_params.tn = linreg->a;
	models_params.rsq_k = fmf_calc_rsq(x_axis_shorter, y_axis_shorter,
									   numof_data_points - 1);
	return models_params;
}

void fmf_print_params(char *file_name)
{
	struct dp *data_set = (struct dp *)malloc(sizeof(struct dp));
	fmf_init_data_set(data_set);
	fmf_form_data_set(file_name, data_set);
	struct models_params models_params = fmf_calc_params(data_set);
	printf("Zero-order kinetics");
	printf("\tk0 = %.4f", models_params.k0);
	printf("\trsq = %.4f\n", models_params.rsq_k0);
	printf("First-order kinetics");
	printf("\tk1 = %.4f", models_params.k1);
	printf("\trsq = %.4f\n", models_params.rsq_k1);
	printf("Higuchi's equation");
	printf("\tkh = %.4f", models_params.kh);
	printf("\trsq = %.4f\n", models_params.rsq_kh);
	printf("Peppas' equation");
	printf("\tk  = %.4f", models_params.k);
	printf("\trsq = %.4f\n", models_params.rsq_k);
	printf("\t\t\tn  = %.4f\n\n", models_params.tn);
}

void fmf_init_data_set(struct dp *data_set)
{
	data_set->mins = VALUE_NOT_SET;
	data_set->perc = VALUE_NOT_SET;
	data_set->next = NULL;
}

void fmf_form_data_set(char *file_name, struct dp *data_set)
{
	char *data_str = 0;
	long length;
	FILE *f = fopen(file_name, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		int magic_value = 0;	/* Be very careful about this!  */
		data_str = malloc(length + magic_value);
		if (data_str) {
			fread(data_str, 1, length, f);
		}
		fclose(f);
	}
	fmf_gtdpts(data_str, data_set);
}
