/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEPPTOSL_HH
#define TGCDATABASEPPTOSL_HH

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{
 
class TGCDatabasePPToSL : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabasePPToSL (std::string filename, std::string blockname)
    : TGCDatabase(TGCDatabase::PPToSL, filename, blockname )
  {}

  virtual ~TGCDatabasePPToSL (void){}
  
private:
  virtual void readDB (void);
  TGCDatabasePPToSL (void) {}
};
  
} // end of namespace
 
#endif
