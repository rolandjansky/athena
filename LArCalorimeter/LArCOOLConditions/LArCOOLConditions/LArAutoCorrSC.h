/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARCOOLCONDITIONS_LARAUTOCORRSUPERCELL_H
#define LARCOOLCONDITIONS_LARAUTOCORRSUPERCELL_H

#include "LArElecCalib/ILArAutoCorr.h" 
#include "LArCOOLConditions/LArCondSuperCellBase.h"
#include "LArCOOLConditions/LArAutoCorrBlob.h"

class CondAttrListCollection;

class LArAutoCorrSC: 
  public ILArAutoCorr, public LArCondSuperCellBase, public LArAutoCorrBlob
{  
 private:
  LArAutoCorrSC();

 public:  
  LArAutoCorrSC(const CondAttrListCollection* attrList);
  virtual ~LArAutoCorrSC( );

  bool good() const { return m_isInitialized && m_nChannels>0; }

  virtual  AutoCorrRef_t autoCorr(const HWIdentifier&  CellID, int gain) const ;  
  virtual  AutoCorrRef_t autoCorr(const Identifier&  CellID, int gain) const;
  
};
#include "AthenaKernel/CondCont.h"
CONDCONT_BASE(LArAutoCorrSC,ILArAutoCorr);
CLASS_DEF( LArAutoCorrSC , 219258685, 1 )
CLASS_DEF( CondCont<LArAutoCorrSC> , 169266589 , 1 )


#endif 
