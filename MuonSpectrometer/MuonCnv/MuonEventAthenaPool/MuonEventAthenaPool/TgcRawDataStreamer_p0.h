/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATA_STREAMER_P0_H
#define TGCRAWDATA_STREAMER_P0_H

#include "AthenaPoolServices/T_AthenaRootConverter.h"
#include "TgcRawData_p0.h"

// This streamer is not needed until TgcRawData is changed

//#include <iostream>

class TgcRawDataStreamer_p0
   : public T_AthenaRootConverter<TgcRawData, TgcRawData_p0 >
{
public:
  TgcRawDataStreamer_p0() {
     SetClassNameAndChecksum("TgcRawData",  0U);
  }          

  void CopyOldToNew(const TgcRawData_p0 &old_obj, TgcRawData &new_obj )
  {
     // somewhat inefficient data filling, can be sped up using some tricks

     TgcRawData	obj();
     new_obj = obj;
  }
  
};



#endif
