/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file  Units_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2015
 * @brief Regression tests for Units package.
 */


#undef NDEBUG


#include "AthenaKernel/Units.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "TestTools/FLOATassert.h"
#include <cassert>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


template <class NUM>
void testunits()
{
#define TESTUNIT(NAME)                          \
  {                                             \
    using Athena::Units::NAME;                  \
    volatile NUM val = 123.5;                   \
    NUM val2 = val * NAME;                      \
    NUM val3 = val2 / NAME;                     \
    assert (std::abs (val3 - val) < 1e-6*val);  \
  }

  TESTUNIT(GeV);
  TESTUNIT(cm3);
  TESTUNIT(gram);
  TESTUNIT(micrometer);
}


void test1()
{
  std::cout << "test1\n";
  testunits<float>();
  testunits<double>();
}


void divcheck()
{
  std::cout << "divcheck\n";
  bool fail = false;
  pid_t pid = getpid();
  char cmd[128];
  snprintf (cmd, sizeof(cmd), "objdump -d /proc/%d/exe", pid);
  FILE* fdump = popen (cmd, "r");
  assert (fdump != nullptr);
  bool readingtest = false;
  char linebuf[2048];
  while (fgets (linebuf, sizeof(linebuf), fdump)) {
    size_t len = strlen (linebuf);
    if (len < 6) continue;
    if (linebuf[len-1] == '\n') {
      --len;
      linebuf[len] = '\0';
    }

    if (readingtest && (strstr (linebuf, "\tdiv") != 0 ||
                        strstr (linebuf, "tfdiv") != 0))
    {
      printf ("Unexpected div: %s\n", linebuf);
      fail = true;
    }

    if (linebuf[0] != '0') continue;
    if (linebuf[len-1] != ':' || linebuf[len-2] != '>') continue;
    char* pos = strchr (linebuf, '<');
    if (!pos) continue;
    if (strstr (linebuf, "test") != 0)
      readingtest = true;
    else
      readingtest = false;
  }

  pclose (fdump);

  if (fail) {
    printf ("%s\n", cmd);
    system (cmd);
    snprintf (cmd, sizeof(cmd), "ls -l /proc/%d/exe", pid);
    system (cmd);
  }
}


int main()
{
  test1();
  divcheck();
  return 0;
}
