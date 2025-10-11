/*
Copyright (C) 1992-2025 Free Software Foundation, Inc.

This file is part of DejaGnu.

DejaGnu is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DejaGnu is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with DejaGnu.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#ifndef NO_UNISTD_H
#include <sys/unistd.h>
#endif

/* A simple glue file for embedded targets so we can get the real exit
   status from the program. This assumes we're using GNU ld and can use
   the -wrap option, and that write(1, ...) does something useful. */

/* There is a bunch of weird cruft with #ifdef UNDERSCORES. This is needed
   because currently GNU ld doesn't deal well with a.out targets and
   the -wrap option. When GNU ld is fixed, this should definitely be
   removed. Note that we actually wrap __exit, not _exit on a target
   that has UNDERSCORES defined.  On non-UNDERSCORE targets, we
   wrap _exit separately; it's actually a different function.  */

#ifdef WRAP_M68K_AOUT

#define REAL_EXIT(code) asm ( "trap %0" : : "i" (0) );
#define REAL_ABORT() REAL_EXIT(6)
#define ORIG_EXIT _exit
#define ORIG_ABORT abort

#else /* not WRAP_M68K_AOUT */

#ifdef UNDERSCORES

#define REAL_EXIT _real___exit
#define REAL_MAIN _real__main
#define REAL_ABORT _real__abort
#define ORIG_EXIT _wrap___exit
#define ORIG_ABORT _wrap__abort
#define ORIG_MAIN _wrap__main

#else /* not UNDERSCORES */

#define REAL_EXIT __real_exit
#ifndef VXWORKS
#define REAL__EXIT __real__exit
#endif /* not VXWORKS */
#define REAL_MAIN __real_main
#define REAL_ABORT __real_abort
#define ORIG_EXIT __wrap_exit
#define ORIG__EXIT __wrap__exit
#define ORIG_ABORT __wrap_abort
#define ORIG_MAIN __wrap_main

#endif /* not UNDERSCORES */

#ifdef __STDC__
extern void abort (void);
extern void exit (int);
#endif /* __STDC__ */

#endif /* not WRAP_M68K_AOUT */

#ifdef __STDC__
#ifdef REAL_ABORT
extern void REAL_ABORT (void);
#endif /* REAL_ABORT */
#ifdef REAL_EXIT
extern void REAL_EXIT (int);
#endif /* REAL_EXIT */
#ifdef REAL__EXIT
extern void REAL__EXIT (int);
#endif /* REAL__EXIT */
#ifdef REAL_MAIN
extern int REAL_MAIN (int argc, char **argv, char **envp);
#endif /* REAL_MAIN */
#endif /* __STDC__ */

static int done_exit_message = 0;
int ___constval = 1;

#ifdef VXWORKS
static void __runexit(void);
#endif /* VXWORKS */

#ifdef __STDC__
static char *
write_int(int val, char *ptr)
#else /* not __STDC__ */
static char *
write_int(val, ptr)
     int val;
     char *ptr;
#endif /* not __STDC__ */
{
  char c;
  if (val<0) {
    *(ptr++) = '-';
    val = -val;
  }
  if (val>9) {
    ptr = write_int (val/10, ptr);
  }
  c = (val%10)+'0';
  *(ptr++) = c;
  return ptr;
}

#ifdef __STDC__
void
ORIG_EXIT (int code)
#else /* not __STDC__ */
void
ORIG_EXIT (code)
     int code;
#endif /* not __STDC__ */
{
  char buf[30];
  char *ptr;

#ifdef VXWORKS
  __runexit ();
#endif /* VXWORKS */
  strcpy (buf, "\n*** EXIT code ");
  ptr = write_int (code, buf + strlen(buf));
  *(ptr++) = '\n';
  write (1, buf, ptr-buf);
  done_exit_message = 1;
  REAL_EXIT (code);
  while (___constval);
}

#ifdef ORIG__EXIT
#ifdef __STDC__
void
ORIG__EXIT (int code)
#else /* not __STDC__ */
void
ORIG__EXIT (code)
     int code;
#endif /* not __STDC__ */
{
  char buf[30];
  char *ptr;

  /* Since exit may call _exit, we need to avoid a second message.  */
  if (! done_exit_message)
    {
      strcpy (buf, "\n*** EXIT code ");
      ptr = write_int (code, buf + strlen(buf));
      *(ptr++) = '\n';
      write (1, buf, ptr-buf);
    }

  REAL__EXIT (code);
  while (___constval);
}
#endif /* ORIG__EXIT */

#ifdef __STDC__
void
ORIG_ABORT (void)
#else /* not __STDC__ */
void
ORIG_ABORT ()
#endif /* not __STDC__ */
{
  write (1, "\n*** EXIT code 4242\n", 20);
  REAL_ABORT ();
  while (___constval);
  abort ();
}

#ifdef REAL_MAIN
#ifdef __STDC__
int
ORIG_MAIN (int argc, char **argv, char **envp)
#else /* not __STDC__ */
int
ORIG_MAIN (argc, argv, envp)
     int argc;
     char **argv;
     char **envp;
#endif /* not __STDC__ */
{
#ifdef WRAP_FILE_ARGS
  extern int __argc;
  extern char *__args[];

  exit (REAL_MAIN (__argc,__args,envp));
#else /* not WRAP_FILE_ARGS */
  exit (REAL_MAIN (argc, argv, envp));
#endif /* not WRAP_FILE_ARGS */
  while (___constval);
}
#endif /* REAL_MAIN */

#ifdef VXWORKS
void
_exit (int status)
{
  REAL_EXIT (status);
}

typedef (*PFV)(void);

static PFV __list[32];
static int __listcnt = 0;
static int __running = 0;

int
atexit (PFV func)
{
  __list[__listcnt++] = func;
}

static void
__runexit ()
{
  int i;
  if (__running++)
    return;

  for (i = 0; i < __listcnt; i++)
    __list[i]();
  __running = 0;
}
#endif /* VXWORKS */
