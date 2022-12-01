/* DejaGnu unit testing header.
   Copyright (C) 2000-2016, 2022 Free Software Foundation, Inc.

This file is part of DejaGnu.

DejaGnu is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

DejaGnu is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with DejaGnu; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.  */

#ifndef __DEJAGNU_H__
#define __DEJAGNU_H__

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* If you have problems with DejaGnu dropping failed, untested, or
 * unresolved messages generated by a unit testcase, then see the section
 * "Priority of Expect Patterns" in *note (dejagnu)Writing a test case. or
 * use the DejaGnu built-in unit testing support in your testsuite, which
 * has been improved to resolve this issue in DejaGnu 1.6.3.  */

static struct {
  int pass;
  int fail;
  int xpass;
  int xfail;
  int untested;
  int unresolved;
  int unsupported;
  /**/
  int endmsg_registered;
  int TestState_count;	/* number of live TestState objects in C++ */
} DG__status = { 0 };

static inline void
DG__endmsg (void)
{ puts ("\tEND: done"); }

static inline void
DG__init (void)
{
  if (DG__status.endmsg_registered) return;

  if (atexit (DG__endmsg) == 0)
    DG__status.endmsg_registered = 1;
}

static inline void
pass (const char* fmt, ...)
{
  va_list ap;

  DG__status.pass++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tPASSED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
xpass (const char* fmt, ...)
{
  va_list ap;

  DG__status.xpass++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tXPASSED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
fail (const char* fmt, ...)
{
  va_list ap;

  DG__status.fail++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tFAILED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
xfail (const char* fmt, ...)
{
  va_list ap;

  DG__status.xfail++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tXFAILED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
untested (const char* fmt, ...)
{
  va_list ap;

  DG__status.untested++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tUNTESTED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
unresolved (const char* fmt, ...)
{
  va_list ap;

  DG__status.unresolved++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tUNRESOLVED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
unsupported (const char* fmt, ...)
{
  va_list ap;

  DG__status.unsupported++;
  DG__init ();

  flockfile (stdout);
  fputs ("\tUNSUPPORTED: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
note (const char* fmt, ...)
{
  va_list ap;

  DG__init ();

  flockfile (stdout);
  fputs ("\tNOTE: ", stdout);
  va_start (ap, fmt); vfprintf (stdout, fmt, ap); va_end (ap);
  fputc ('\n', stdout);
  funlockfile (stdout);
}

static inline void
totals (void)
{
  printf ("\nTotals:\n");
  printf ("\t#passed:\t\t%d\n", DG__status.pass);
  printf ("\t#real failed:\t\t%d\n", DG__status.fail);
  if (DG__status.xfail)
    printf ("\t#expected failures:\t\t%d\n", DG__status.xfail);
  if (DG__status.xpass)
    printf ("\t#unexpected passes:\t\t%d\n", DG__status.xpass);
  if (DG__status.untested)
    printf ("\t#untested:\t\t%d\n", DG__status.untested);
  if (DG__status.unresolved)
    printf ("\t#unresolved:\t\t%d\n", DG__status.unresolved);
  if (DG__status.unsupported)
    printf ("\t#unsupported:\t\t%d\n", DG__status.unsupported);
}

#ifdef __cplusplus

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

const char * DG__outstate_list[] = {
  "\tFAILED: ", "\tPASSED: ",
  "\tUNTESTED: ", "\tUNRESOLVED: ", "\tUNSUPPORTED: ",
  "\tXFAILED: ", "\tXPASSED: "
};

enum DG_teststate { FAILED, PASSED,
		    UNTESTED, UNRESOLVED, UNSUPPORTED,
		    XFAILED, XPASSED };

class TestState {
 private:
  DG_teststate laststate;
  std::string lastmsg;
 public:
  TestState (void)
    {
      DG__status.pass = 0;
      DG__status.fail = 0;
      DG__status.xpass = 0;
      DG__status.xfail = 0;
      DG__status.untested = 0;
      DG__status.unresolved = 0;
      DG__status.unsupported = 0;

      /* C++ object destruction will substitute for atexit(). */
      DG__status.endmsg_registered = 1;
      DG__status.TestState_count++;
    }

  ~TestState (void)
    {
      DG__status.TestState_count--;

      if (DG__status.TestState_count > 0) return;

      /* The last TestState object is being destroyed. */
      totals ();
      std::cout << "\tEND: done" << std::endl;
    }

  void testrun (bool b, std::string s)
    {
      if (b)
	pass (s);
      else
	fail (s);
    }

    void pass (std::string s)
      {
	DG__status.pass++;
	laststate = PASSED;
	lastmsg = s;
	std::cout << DG__outstate_list[PASSED] << s << std::endl;
      }

    void xpass (std::string s)
      {
	DG__status.xpass++;
	laststate = PASSED;
	lastmsg = s;
	std::cout << DG__outstate_list[XPASSED] << s << std::endl;
      }

    void fail (std::string s)
      {
	DG__status.fail++;
	laststate = FAILED;
	lastmsg = s;
	std::cout << DG__outstate_list[FAILED] << s << std::endl;
      }

    void xfail (std::string s)
      {
	DG__status.xfail++;
	laststate = XFAILED;
	lastmsg = s;
	std::cout << DG__outstate_list[XFAILED] << s << std::endl;
      }

    void untested (std::string s)
      {
	DG__status.untested++;
	laststate = UNTESTED;
	lastmsg = s;
	std::cout << DG__outstate_list[UNTESTED] << s << std::endl;
      }

    void unresolved (std::string s)
      {
	DG__status.unresolved++;
	laststate = UNRESOLVED;
	lastmsg = s;
	std::cout << DG__outstate_list[UNRESOLVED] << s << std::endl;
      }

    void unsupported (std::string s)
      {
	DG__status.unsupported++;
	laststate = UNSUPPORTED;
	lastmsg = s;
	std::cout << DG__outstate_list[UNSUPPORTED] << s << std::endl;
      }

    void note (std::string s)
      {
	std::cout << "\t" << "NOTE: " << s << std::endl;
      }

    void totals (void)
      {
	std::cout << std::endl << "Totals:" << std::endl;

	std::cout << "\t#passed:\t\t"
		  << DG__status.pass << std::endl;
	std::cout << "\t#real failed:\t\t"
		  << DG__status.fail << std::endl;

	if (DG__status.xfail)
	  std::cout << "\t#expected failures:\t\t"
		    << DG__status.xfail << std::endl;
	if (DG__status.xpass)
	  std::cout << "\t#unexpected passes:\t\t"
		    << DG__status.xpass << std::endl;
	if (DG__status.untested)
	  std::cout << "\t#untested:\t\t"
		    << DG__status.untested << std::endl;
	if (DG__status.unresolved)
	  std::cout << "\t#unresolved:\t\t"
		    << DG__status.unresolved << std::endl;
	if (DG__status.unsupported)
	  std::cout << "\t#unsupported:\t\t"
		    << DG__status.unsupported << std::endl;
      }

    // This is so this class can be printed in an ostream.
    friend std::ostream & operator << (std::ostream &os, TestState& t)
      {
	return os << DG__outstate_list[t.laststate] << t.lastmsg ;
      }

    int GetState (void) { return laststate; }
    std::string GetMsg (void) { return lastmsg; }
};

#endif /* __cplusplus */
#endif /* _DEJAGNU_H_ */
