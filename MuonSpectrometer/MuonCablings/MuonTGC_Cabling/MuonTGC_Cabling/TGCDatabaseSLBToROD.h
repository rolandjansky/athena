/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCDATABASESLBTOROD_HH
#define MUONTGC_CABLING_TGCDATABASESLBTOROD_HH

#include "MuonTGC_Cabling/TGCDatabase.h"

namespace MuonTGC_Cabling
{
 
class TGCDatabaseSLBToROD : public TGCDatabase
{
public:
  // Constructor & Destructor
  TGCDatabaseSLBToROD(const std::string& filename,
                      const std::string& blockname);

  TGCDatabaseSLBToROD(const TGCDatabaseSLBToROD& );

  virtual ~TGCDatabaseSLBToROD(void);

  virtual bool update(const std::vector<int>& );
 
  virtual int  find(const std::vector<int>&) const;
  
  
private:
  virtual void readDB(void);
  TGCDatabaseSLBToROD(void) {}

  /** hidden assignment operator */
  TGCDatabaseSLBToROD & operator=(const TGCDatabaseSLBToROD &right);
};
  
} // end of namespace
 
#endif
