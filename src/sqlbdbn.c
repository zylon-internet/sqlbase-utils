/* sqlbdbn.c
   Copyright (C) 2012 Erik Smit <zoiah@zoiah.net>

   based on example: http://www.unify.com/onlinedocs/sb/books/api/api_function_reference.htm#api_function_reference_6977_8267

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA. */

/* example usage 1: 
# sqlbdbn SQLBSERV
DB1
DB2
DB3
DB4
# 
*/

#include "sqlbase.h"
#include "errsql.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#define BUFFER_SIZE 1024	/* read 1k buffers  */

void
failure (char *ep, SQLTCUR cur, SQLTRCD rcd)
{
  SQLTEPO epo;			/* error position */
  char errmsg[SQLMERR];		/* error msg text buffer */

  fprintf (stderr, "Failure on %s: ", ep);

  if (rcd)
    {
      sqlerr (rcd, errmsg);
      sqlepo (cur, &epo);
    }

  sqldis (cur);

  fprintf (stderr, "%s(error: %u, position: %u) \n", errmsg, rcd, epo);
  exit (1);
}

void
printusage (char *argv[])
{
  fprintf (stderr, "Usage: %s [SERVERNAME]\n", argv[0]);
  exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
  SQLTRCD rcd = 0;		/* return code */
  char buffer[BUFFER_SIZE];
  int bufferptr;

  char *servername = "";

  int flags = 0, opt;
  int nsecs, tfnd;

  nsecs = 0;
  tfnd = 0;
  flags = 0;
  while ((opt = getopt (argc, argv, ":")) != -1)
    {
              switch (opt)
                          {


        case '?':
          if (isprint (optopt))
            fprintf (stderr, "Unknown option `-%c'.\n", optopt);
          else
            fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
          printusage (argv);
          break;
        default:                /* '?' */
          printusage (argv);
          break;
        }

    }

  if (optind < argc)
    {
      servername = argv[optind];
    }

  if (rcd = sqldbn ((SQLTDAP)servername, (SQLTDAP)buffer, sizeof(buffer)))
    failure ("sqldbn", 0, rcd);
  else
    {
      bufferptr=0;
      while (bufferptr < BUFFER_SIZE)
	{
	  if (strlen(&buffer[bufferptr]) > 0)
	    {
	      printf ("%s\n", &buffer[bufferptr]);
	    }
	  else
	    {
                break;
	    }
	  bufferptr += strnlen(&buffer[bufferptr], BUFFER_SIZE-bufferptr)+1;
	}
    }
  exit(0);
}
