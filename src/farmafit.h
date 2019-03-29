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
 * @file farmafit.h
 * @author Novak Petrovic
 * @date 2019
 * @brief Header file for farmafit.c.
 * 
 * @see README (or README.md) for more details.
 */
#ifndef FARMAFIT_H
#define FARMAFIT_H

struct dp;
struct lr;

/**                                                                            
 * @brief Calculates arithmetic mean.
 * 
 * @param[in] data Values for which to calculate arithmetic mean
 * @param[in] size Number of data points
 * @return Arithmetic mean of data
 */
float fmf_armean(float *data, int size);

/**
 * @brief Calculates mean of products.
 * 
 * @param[in] data1 Data set 1
 * @param[in] data2 Data set 2
 * @param[in] size Number of data points
 * @return Mean of products
 * @note Number of data points in data1 and data2 must be the same.
 */
float fmf_moprods(float *data1, float *data2, int size);

/**
 * @brief Calculates variance.
 * 
 * @param[in] data Data set for which to calculate variance
 * @param[in] size Number of data points
 * @return Variance
 */
float fmf_variance(float *data, int size);

/**
 * @brief Uses linear regression to calculate slope and intercept.
 *
 * @param[in] independent Independent (x) coordinate of each data point
 * @param[in] dependent Dependent (y) coordinate of each data point
 * @param[in] size Number of data points
 * @param[out] linreg Pointer to lr (linear regression) struct to hold the result
 * @return void
 */
void fmf_calc_linreg(float *independent, float *dependent, int size,
					 struct lr *linreg);

/**
 * @brief Calculates r squared (r^2) value.
 *
 * @param[in] x Independent (x) coordinate of each data point
 * @param[in] y Dependent (y) coordinate of each data point
 * @param[in] size Number of data points
 * @return Value of r squared
 */
float fmf_calc_rsq(float *x, float *y, int size);

/**
 * @brief Gets data points.
 *
 * @param[in] data_str All data from JSON file as a string
 * @param[in,out] head Pointer to the first element of the linked list 
 *                     which contains measurement points (from JSON file)
 * @return Error code (to be documented)
 */
int fmf_gtdpts(const char *const data_str, struct dp *head);

/**
 * @brief Calculates all model parameters, including r-squared values
 * @param[in] file_name Name of the experimental measurements file
 * @return Structure containing parameters for all models 
 */
struct models_params fmf_calc_params(struct dp *data_set);

/*
 * @brief Prints all model parameters, includin r-squared values
 * @param[in] file_name Name of the experimental measurements file
 * @return void 
 */
void fmf_print_params(char *file_name);

void fmf_init_data_set(struct dp *);

void fmf_form_data_set(char *, struct dp *);

#endif							/* #ifndef FARMAFIT_H  */
