/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARUA2MEVSYM_H
#define LARRAWCONDITIONS_LARUA2MEVSYM_H

#include "LArElecCalib/ILAruA2MeV.h"

class LArMCSym;
class LAruA2MeVMC;
class LAruA2MeVComplete;

class LAruA2MeVSym: virtual public ILAruA2MeV {
  
 public:

  LAruA2MeVSym() = delete;

  LAruA2MeVSym(const LArMCSym* mcsym,  const LAruA2MeVMC* uA2MeVComplete);
  
  virtual ~LAruA2MeVSym( );
  
  virtual const float& UA2MEV(const HWIdentifier& hwid) const override;
  
  virtual const float& UA2MEV(const Identifier& CellID) const override;
  
 private: 

  const LArMCSym* m_mcSym;
  const LAruA2MeVComplete* m_uA2MeVComplete;

};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LAruA2MeVSym,123298666,1)
CONDCONT_DEF( LAruA2MeVSym,131315182, ILAruA2MeV );
#endif 
