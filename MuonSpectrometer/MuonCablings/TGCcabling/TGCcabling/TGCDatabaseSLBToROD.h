/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASESLBTOROD_HH
#define TGCDATABASESLBTOROD_HH

#include "TGCcabling/TGCDatabase.h"

namespace LVL1TGCCabling8
{
 
class TGCDatabaseSLBToROD : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseSLBToROD (const std::string& filename, const std::string& blockname)
    : TGCDatabase(TGCDatabase::SLBToROD, filename, blockname )
  {}

  virtual ~TGCDatabaseSLBToROD (void){}
  
private:
  virtual void readDB (void);
  TGCDatabaseSLBToROD (void) {}
};
  
} // end of namespace
 
#endif
