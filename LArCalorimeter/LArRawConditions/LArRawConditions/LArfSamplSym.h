/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARFSAMPLSYM_H
#define LARRAWCONDITIONS_LARFSAMPLSYM_H

#include "LArElecCalib/ILArfSampl.h"

class LArMCSym;
class LArfSamplComplete ;
class LArfSamplMC;

/** Implementation of the interface ILArfSampl for MC
    using the z-phi symmetry
 */

class LArfSamplSym: virtual public ILArfSampl {
  
 public: 
  
  LArfSamplSym()=delete;

  LArfSamplSym(const LArMCSym* mcSym, const LArfSamplMC* fSamplMC);
  
  virtual ~LArfSamplSym();
  
  // retrieving fSampl using online ID
  
  virtual const float& FSAMPL(const HWIdentifier&  CellID) const override;
  
  virtual const float& FSAMPL(const Identifier&  CellID) const override;
    
 private: 
  
  const LArMCSym* m_mcSym;
  const LArfSamplComplete* m_fSamplComplete;

};

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArfSamplSym, 169816659, 1)
CONDCONT_DEF( LArfSamplSym, 75658929, ILArfSampl );


#endif 
