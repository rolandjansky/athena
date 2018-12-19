/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
// $Id$
#include "RingerSelectorTools/ExtraDescriptionPatterns.h"

#ifndef RINGER_STANDALONE

//==============================================================================
MsgStream& operator<< ( MsgStream& out,  const Ringer::ExtraDescriptionPatterns& extraPat )
{
  out << "Using following extra description patterns configuration: "
      << "\n - (bool)Feed particle eta position (yes/no)                        : " << (extraPat.feedEta() ? "yes" : "no")
      << "\n - (bool)Feed particle Et position (yes/no)                         : " << (extraPat.feedEt() ? "yes" : "no")
      << "\n - (bool)Feed particle pile-up estimation (yes/no)                  : " << (extraPat.feedPileupEstimation() ? "yes" : "no");
  return out;
}
#endif // RINGER_STANDALONE

