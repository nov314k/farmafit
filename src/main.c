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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define BASIC_ERROR_MESSAGE "Error: Incorrect program usage."

void help_version ();
void auxf (char *filename);

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
  while ((c = getopt (argc, argv, "hvf:")) != -1)
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
	case '?':
	  help_version ();
	  exit (EXIT_FAILURE);
	  break;
	case ':':
	  help_version ();
	  exit (EXIT_FAILURE);
	  break;
	case 'f':
	  if (argc == 3)
	    {
	      fmf_calc_params (optarg);
	      exit (EXIT_SUCCESS);
	    }
	  else
	    {
	      fprintf (stderr, BASIC_ERROR_MESSAGE);
	      help_version ();
	      exit (EXIT_FAILURE);
	    }
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
