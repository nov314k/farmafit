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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "globdefs.h"
#include "data_types.h"
#include "farmafit.h"

int
main (int argc, char **argv)
{
  struct dp *head = (struct dp *) malloc (sizeof (struct dp));
  head->mins = VALUE_NOT_SET;
  head->perc = VALUE_NOT_SET;
  head->next = NULL;
  const char *const data_str = fmf_file2str (argv[1]);
  int exit_status = fmf_gtdpts (data_str, head);
  if (exit_status == EXIT_SUCCESS)
    {
      int data_points = 0;
      struct dp *cur = head;
      while (cur != NULL)
	{
	  data_points++;
	  cur = cur->next;
	}
      float dependent[data_points];
      float independent[data_points];
      int i = 0;
      cur = head;
      while (cur != NULL)
	{
	  dependent[i] = cur->perc;
	  independent[i] = cur->mins;
	  i++;
	  cur = cur->next;
	}
      struct lr *linreg = (struct lr *) malloc (sizeof (struct lr));
      fmf_linreg (independent, dependent, data_points, linreg);
      printf ("Zero-order kinetics");
      printf ("\tk0 = %.4f", linreg->a);
      printf ("\trsq = %.4f\n",
	      fmf_rsq (independent, dependent, data_points));
      float x[data_points - 1];
      float y[data_points - 1];
      for (int i = 0; i < data_points - 1; i++)
	{
	  x[i] = independent[i + 1];
	  y[i] = log (dependent[i + 1]);
	}
      fmf_linreg (x, y, data_points - 1, linreg);
      printf ("First-order kinetics");      
      printf ("\tk1 = %.4f", linreg->a);
      printf ("\trsq = %.4f\n", fmf_rsq (x, y, data_points - 1));
      float xx[data_points];
      float yy[data_points];
      for (int i = 0; i < data_points; i++)
	{
	  xx[i] = sqrt (independent[i]);
	  yy[i] = dependent[i];
	}
      fmf_linreg (xx, yy, data_points, linreg);
      printf ("Higuchi's equation");
      printf ("\tkh = %.4f", linreg->a);
      printf ("\trsq = %.4f\n", fmf_rsq (xx, yy, data_points));
      for (int i = 0; i < data_points - 1; i++)
	{
	  x[i] = log (independent[i + 1]);
	  y[i] = log (dependent[i + 1]);
	}
      fmf_linreg (x, y, data_points - 1, linreg);
      printf ("Peppas' equation");
      printf ("\tk  = %.4f", exp (linreg->b));
      printf ("\trsq = %.4f\n", fmf_rsq (x, y, data_points - 1));
      printf ("\t\t\tn  = %.4f\n\n", linreg->a);
    }
  return 0;
}

