/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalibAnalysis/egammaMVATool.h"


int main()
{
  egammaMVATool myMVATool("myMVATool");
  myMVATool.setProperty("folder", "egammaMVACalib/online/v3");
  myMVATool.initialize();

  return 0;
}
