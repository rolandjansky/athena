/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalib/egammaMVATool.h"

#include <iostream>

int main()
{
  egammaMVATool myMVATool("myMVATool");
  myMVATool.setProperty("folder", "egammaMVACalib/online/v3");
  myMVATool.initialize();

  int a;
  std::cin >> a;

  return 0;
}
