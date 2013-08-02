/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASESLBTOROD_HH
#define TGCDATABASESLBTOROD_HH

#include "TGCcabling12/TGCDatabase.h"

namespace LVL1TGCCabling12
{
 
class TGCDatabaseSLBToROD : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseSLBToROD (std::string filename, std::string blockname);

  TGCDatabaseSLBToROD (const TGCDatabaseSLBToROD& );

  virtual ~TGCDatabaseSLBToROD (void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;
  
  
private:
  virtual void readDB (void);
  TGCDatabaseSLBToROD (void) {}
};
  
} // end of namespace
 
#endif
