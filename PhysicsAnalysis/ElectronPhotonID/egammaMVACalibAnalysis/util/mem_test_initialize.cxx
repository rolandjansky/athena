/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "egammaMVACalibAnalysis/egammaMVATool.h"


int main()
{
  egammaMVATool myMVATool("myMVATool");
  myMVATool.setProperty("folder", "egammaMVACalib/online/v3").ignore();
  myMVATool.initialize().ignore();

  return 0;
}
