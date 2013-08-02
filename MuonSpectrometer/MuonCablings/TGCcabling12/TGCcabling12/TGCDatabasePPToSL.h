/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDATABASEPPTOSL_HH
#define TGCDATABASEPPTOSL_HH

#include "TGCcabling12/TGCDatabase.h"

namespace LVL1TGCCabling12
{
 
class TGCDatabasePPToSL : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabasePPToSL (std::string filename, std::string blockname);

  TGCDatabasePPToSL (const TGCDatabasePPToSL&);

  virtual ~TGCDatabasePPToSL (void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;
    
private:
  virtual void readDB (void);
  TGCDatabasePPToSL (void) {}
};
  
} // end of namespace
 
#endif
