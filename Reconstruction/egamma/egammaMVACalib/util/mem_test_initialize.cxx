/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalib/egammaMVATool.h"


int main()
{
  egammaMVATool myMVATool("myMVATool");
  myMVATool.setProperty("folder", "egammaMVACalib/online/v3");
  myMVATool.initialize();

  return 0;
}
