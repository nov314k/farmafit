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
 * @file data_types.h
 * @author Novak Petrovic
 * @date 2019
 * @brief Contains struct definitions for data types used by Farmafit.
 *
 * @see README (or README.md) for more details.
 */
#ifndef DATA_TYPES_H
#define DATA_TYPES_H

/**
 * @brief Data structure for linked list of data points (dp).
 *
 * @param mins Elapsed minutes (independent variable).
 * @param perc Percentage of the substance dissolved (dependent variable).
 */
struct dp {
	float mins;
	float perc;
	struct dp *next;
};

/**
 * @brief Data structure to represent the results of linear regression (lr).
 *
 * @param a Slope of the line.
 * @param b Intercept point.
 */
struct lr {
	float a;
	float b;
};

/**
 * @brief Data structure for the calculated parameters of all models.
 *
 * @param k0 Parameter for zero-order kinetics model
 * @param k1 Parameter for first-order kinetics model
 * @param kh Parameter for Higuchi's model
 * @param k  Parameter (first) for Peppas' model
 * @param tn Parameter (second) for Peppas' model
 * @param rsq_k0 r-squared value for zero-order kinetics model
 * @param rsq_k1 r-squared value for first-order kinetics model
 * @param rsq_kh r-squared value for Higuchi's model
 * @param rsq_k  r-squared value for Peppas' model
 */
struct models_params {
	float k0;
	float k1;
	float kh;
	float k;
	float tn;
	float rsq_k0;
	float rsq_k1;
	float rsq_kh;
	float rsq_k;
};

#endif							/* #ifndef DATA_TYPES_H  */
