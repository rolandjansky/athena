/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class LWHistAthMonWrapper                 //
//                                                            //
//  Description: Utility methods to associate a few custom    //
//               fields (key and streamName) with LWHists.    //
//               This makes book-keeping simpler, and saves   //
//               needless string copies and map lookups.      //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2009                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef LWHISTWRAPPER_H
#define LWHISTWRAPPER_H

#include "LWHists/LWHist.h"
#include "LWHists/LWHistControls.h"

class LWHistAthMonWrapper {
public:

  static void setStreamName(LWHist*, const std::string& streamName);
  static void setKey(LWHist*, const std::string& key);
  static const std::string& streamName(LWHist*);
  static const std::string& key(LWHist*);
  static void deleteLWHist(LWHist*);//always use this to delete LWHists in this package
  static void removeCustomData(LWHist*);

private:
  struct LWHistAthenaMonCustomData;
  LWHistAthMonWrapper(){}
  ~LWHistAthMonWrapper(){}
};

#include "LWHistAthMonWrapper.icc"

#endif

