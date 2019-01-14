/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLESSWTOROD_HH
#define MUONTGC_CABLING_TGCCABLESSWTOROD_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

namespace MuonTGC_Cabling
{

class TGCDatabase;
  
class TGCCableSSWToROD : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableSSWToROD(std::string filename);
  TGCCableSSWToROD (const TGCCableSSWToROD&);
  TGCCableSSWToROD& operator=(const TGCCableSSWToROD&);

  virtual ~TGCCableSSWToROD(void);
  
  virtual TGCModuleMap* getModule(const TGCModuleId* moduleId) const;

private:
  TGCCableSSWToROD(void);
  virtual TGCModuleMap* getModuleIn(const TGCModuleId* rod) const;
  virtual TGCModuleMap* getModuleOut(const TGCModuleId* ssw) const;
  TGCDatabase* m_database;
};
  
} // end of namespace
 
#endif
