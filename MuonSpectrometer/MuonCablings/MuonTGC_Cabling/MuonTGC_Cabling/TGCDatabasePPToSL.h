/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCDATABASEPPTOSL_HH
#define MUONTGC_CABLING_TGCDATABASEPPTOSL_HH

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{
 
class TGCDatabasePPToSL : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabasePPToSL(const std::string& filename, const std::string& blockname);

  TGCDatabasePPToSL(const TGCDatabasePPToSL&);

  virtual ~TGCDatabasePPToSL(void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;
    
private:
  virtual void readDB(void);
  TGCDatabasePPToSL(void) {}

  /** hidden assignment operator */
  TGCDatabasePPToSL & operator=(const TGCDatabasePPToSL &right);
};
  
} // end of namespace
 
#endif
