/* sqlbunl.c
   Copyright (C) 2012 Erik Smit <zoiah@zoiah.net>

   based on example: http://www.unify.com/onlinedocs/sb/books/api/api_using_the_api.htm#api_using_the_api_6046_7615

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

/* example usage 1: # sqlbunl -f backup.sql DATABASE/USERNAME/PASSWORD
   example usage 2: # sqlbunl DATABASE/USERNAME/PASSWORD | gzip > backup.sql.gz */

#include "sqlbase.h"
#include "errsql.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

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

SQLTAPI
unloadx (SQLTCUR cur, int fd)
{
  SQLTDAL len;			/* length indicator     */
  SQLTRCD rcd;			/* return code  */
  char buf[BUFFER_SIZE];	/* unload data buffer   */

  if (rcd = sqlcom (cur, "UNLOAD DATABASE foo ON CLIENT", 0))   // 
    failure ("sqlcom", cur, rcd);  

  if (rcd = sqlexe (cur))
    failure ("sqlexe", cur, rcd);

  while (!(rcd = sqlfet (cur)))
    {
      while (1)
	{
	  if (rcd = sqlrlo (cur, (SQLTSLC) 1, buf,
			    (SQLTDAL) BUFFER_SIZE, &len))
	    failure ("sqlrlo", cur, rcd);
	  if (len)
	    {
	      write (fd, buf, len);
	    }
	  else
	    break;
	}
      if (rcd = sqlelo (cur))
	failure ("sqlelo: ", cur, rcd);
    }
  if (rcd > 1)
    failure ("still data: ", cur, rcd);

  close(fd);
}

void
printusage (char *argv[])
{
  fprintf (stderr, "Usage: %s [-f filename] dbname\n", argv[0]);
  exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
  SQLTCUR cur = 0;		/* SQLBase cursor number         */
  SQLTRCD rcd = 0;		/* return code */

  char *dbname;
  char *filename = 0;
  int fd;


  int flags = 0, opt;
  int nsecs, tfnd;

  nsecs = 0;
  tfnd = 0;
  flags = 0;
  while ((opt = getopt (argc, argv, "f:")) != -1)
    {

      switch (opt)
	{
	case 'f':
	  filename = optarg;
	  break;
	case '?':
	  if (optopt == 'f')
	    fprintf (stderr, " %c missing argument. \n", optopt);
	  else if (isprint (optopt))
	    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	  else
	    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
	  printusage (argv);
	  break;
	default:		/* '?' */
	  printusage (argv);
	}
    }

  printf ("flags=%d; tfnd=%d; optind=%d\n", flags, tfnd, optind);

  if (optind >= argc)
    {
      printusage (argv);
    }

  dbname = argv[optind];
  if (!filename)
    fd = fileno(stdout); 
  else 
  if (!strcmp(filename,"-"))
    fd = fileno(stdout); 
        else    
  if ((fd = open (filename, "w")) < 0)
    { fprintf (stderr, "File already exists"); exit(1); }



  if (rcd = sqlcnc (&cur, dbname, 0))
    failure ("sqlcnc", cur, rcd);
  else
    fprintf (stderr, "Connected\n");

  unloadx (cur, fd);

  if (rcd = sqldis (cur))
    failure ("Cannot disconnect: ", cur, rcd);
  else
    fprintf (stderr, "Disconnected\n");
}
