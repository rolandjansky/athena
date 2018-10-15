/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLSYM_H
#define LARRAWCONDITIONS_LARFSAMPLSYM_H

#include "LArElecCalib/ILArfSampl.h"
#include "LArRawConditions/LArfSamplMC.h"

class LArMCSym;
class LArfSamplMC;
class LArfSamplP;
template<typename LArfSamplP> class LArConditionsContainer;

/** Implementation of the interface ILArfSampl for MC
    using the z-phi symmetry
 */

class LArfSamplSym: virtual public ILArfSampl {  
 public: 
  LArfSamplSym()=delete;
  LArfSamplSym(const LArMCSym* mcSym, const LArfSamplMC* fSamplMC);
  virtual ~LArfSamplSym();
  
  virtual const float& FSAMPL(const HWIdentifier&  CellID) const override;
  virtual const float& FSAMPL(const Identifier&  CellID) const override;    
 private: 
  const LArMCSym* m_mcSym;
  const LArConditionsContainer<LArfSamplP>* m_fSamplCont;
};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArfSamplSym, 169816659, 1)
CONDCONT_DEF( LArfSamplSym, 75658929, ILArfSampl );

#endif 
