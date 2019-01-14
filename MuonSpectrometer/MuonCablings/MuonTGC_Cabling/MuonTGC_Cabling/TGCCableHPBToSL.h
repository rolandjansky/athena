/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTGC_CABLING_TGCCABLEHPBTOSL_HH
#define MUONTGC_CABLING_TGCCABLEHPBTOSL_HH
 
#include "MuonTGC_Cabling/TGCCable.h"

#include <string>

#include "TGCcablingInterface/TGCIdBase.h"

namespace MuonTGC_Cabling
{

class TGCDatabase;
  
class TGCCableHPBToSL : public TGCCable
{
public:
  // Constructor & Destructor
  TGCCableHPBToSL(std::string filename);

  virtual ~TGCCableHPBToSL(void);
  
  virtual TGCModuleMap* getModule(const TGCModuleId* moduleId) const;

private:
  TGCCableHPBToSL(void) {}
  virtual TGCModuleMap* getModuleIn(const TGCModuleId* sl) const;
  virtual TGCModuleMap* getModuleOut(const TGCModuleId* hpt) const;
  TGCDatabase* m_database[TGCIdBase::MaxRegionType][TGCIdBase::MaxSignalType];
};
  
} // end of namespace
 
#endif
