/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "RecTPCnv/MissingETComposition_p2.h"


#include "MissingETEvent/MissingETComposition.h"

Weight_p1::Weight_p1(const MissingETHandlers::Weight& we)
{
   w[0]=we.wpx(); w[1]=we.wpy(); w[2]=we.wet(); statusWord=we.statusWord();
}
