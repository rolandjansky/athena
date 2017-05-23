/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ABBAMAPPING_H
#define ABBAMAPPING_H
#include <map>
#include "LArByteStream/LArABBADecoder.h"

class ABBAMapping
{
public:
  static void fill(std::map<int, HWIdentifier> *toFill, int iphi);

};

#endif // ABBAMAPPING_H

