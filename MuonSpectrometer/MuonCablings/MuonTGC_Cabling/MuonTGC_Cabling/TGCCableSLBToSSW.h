/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLESLBTOSSW_HH
#define MUONTGC_CABLING_TGCCABLESLBTOSSW_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

#include "MuonTGC_Cabling/TGCId.h"

namespace MuonTGC_Cabling
{

class TGCDatabase;
  
class TGCCableSLBToSSW : public TGCCable
{
public:
  enum {
    SL            = TGCIdBase::MaxModuleType, 
    MaxModuleType = TGCIdBase::MaxModuleType + 1
  };

  // Constructor & Destructor
  TGCCableSLBToSSW(std::string filename);

  virtual ~TGCCableSLBToSSW(void);
  
  virtual TGCModuleMap* getModule(const TGCModuleId* moduleId) const;

private:
  TGCCableSLBToSSW(void) {}
  virtual TGCModuleMap* getModuleIn(const TGCModuleId* ssw) const;
  virtual TGCModuleMap* getModuleOut(const TGCModuleId* slb) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][MaxModuleType];
};
  
} // end of namespace
 
#endif
