/*******************************************************************************
 * Copyright (C) 2019 Novak Petrovic                                           * 
 * dev2 <@> novak5 <.> 33mail <.> com                                          *
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "farmafit.h"
#include "globdefs.h"
#include "data_types.h"
#include "../lib/cJSON.h"

float
fmf_armean (float *data, int size)
{
  float total = 0.0;
  for (int i = 0; i < size; total += data[i], i++);
  return total / size;
}

float
fmf_moprods (float *data1, float *data2, int size)
{
  float total = 0.0;
  for (int i = 0; i < size; total += (data1[i] * data2[i]), i++);
  return total / size;
}

float
fmf_variance (float *data, int size)
{
  float squares[size];
  for (int i = 0; i < size; i++)
    {
      squares[i] = pow (data[i], 2);
    }
  float mean_of_squares = fmf_armean (squares, size);
  float mean = fmf_armean (data, size);
  float square_of_mean = pow (mean, 2);
  float variance = mean_of_squares - square_of_mean;
  return variance;
}

void
fmf_linreg (float *independent, float *dependent, int size,
		   struct lr *linreg)
{
  float independent_mean = fmf_armean (independent, size);
  float dependent_mean = fmf_armean (dependent, size);
  float products_mean = fmf_moprods (independent, dependent, size);
  float independent_variance = fmf_variance (independent, size);
  linreg->a =
    (products_mean -
     (independent_mean * dependent_mean)) / independent_variance;
  linreg->b = dependent_mean - (linreg->a * independent_mean);
}

float
fmf_rsq (float *x, float *y, int size)
{
  float x_mean = fmf_armean (x, size);
  float y_mean = fmf_armean (y, size);
  float top = 0.0;
  float bottom2x = 0.0;
  float bottom2y = 0.0;
  for (int i = 0; i < size; i++)
    {
      top += (x[i] - x_mean) * (y[i] - y_mean);
      bottom2x += pow (x[i] - x_mean, 2);
      bottom2y += pow (y[i] - y_mean, 2);
    }
  float r = top / sqrt (bottom2x * bottom2y);
  return pow (r, 2);
}

int
fmf_gtdpts (const char *const data_str, struct dp *head)
{
  const cJSON *data_point = NULL;
  const cJSON *data_points = NULL;
  const cJSON *experiment_name = NULL;
  cJSON *data_json = cJSON_Parse (data_str);
  if (data_json == NULL)
    {
      const char *error_ptr = cJSON_GetErrorPtr ();
      if (error_ptr != NULL)
	{
	  fprintf (stderr, "Error before: %s\n", error_ptr);
	}
      cJSON_Delete (data_json);
      return EXIT_FAILURE;
    }
  experiment_name = cJSON_GetObjectItemCaseSensitive
    (data_json, "experiment_name");
  if (cJSON_IsString (experiment_name)
      && (experiment_name->valuestring != NULL))
    {
      printf ("Analysis of \"%s\":\n", experiment_name->valuestring);
    }
  data_points = cJSON_GetObjectItemCaseSensitive (data_json, "data_points");
  cJSON_ArrayForEach (data_point, data_points)
  {
    cJSON *mins = cJSON_GetObjectItemCaseSensitive (data_point, "mins");
    cJSON *percentage = cJSON_GetObjectItemCaseSensitive
      (data_point, "percentage");
    if (cJSON_IsNumber (mins) && cJSON_IsNumber (percentage))
      {
	if (head->mins == VALUE_NOT_SET && head->perc == VALUE_NOT_SET)
	  {
	    head->mins = mins->valuedouble;
	    head->perc = percentage->valuedouble;
	    head->next = NULL;
	  }
	else
	  {
	    struct dp *last = head;
	    while (last->next != NULL)
	      last = last->next;
	    last->next = (struct dp *) malloc (sizeof (struct dp));
	    last->next->mins = mins->valuedouble;
	    last->next->perc = percentage->valuedouble;
	    last->next->next = NULL;
      }}
  } cJSON_Delete (data_json);
  return EXIT_SUCCESS;
}

char *
fmf_file2str (char *file_name)
{
  char *buffer = 0;
  long length;
  FILE *f = fopen (file_name, "rb");
  if (f)
    {
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      int magic_value = 3; /* Be very careful about this!  */
      buffer = malloc (length) + 3;
      if (buffer)
	{
	  fread (buffer, 1, length, f);
	}
      fclose (f);
    }
  return buffer;
}
