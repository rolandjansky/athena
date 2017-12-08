/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRAWCONDITIONS_LARAUTOCORRMC_H
#define LARRAWCONDITIONS_LARAUTOCORRMC_H

#include "LArRawConditions/LArAutoCorrComplete.h"
#include "LArElecCalib/ILArMCSymTool.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;

/** Implementation of the interface ILArAutoCorr for MC
 *  Derives from LArAutoCorrComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 * History:
 *   - 08/02/2004, S. Laplace: new online ID and cablingSvc
 */

class LArAutoCorrMC: public LArAutoCorrComplete {
  
 public: 
  LArAutoCorrMC();
  
  virtual ~LArAutoCorrMC( );
  virtual StatusCode initialize( ) ;
  
  // retrieving AutoCorr using online ID
  
  virtual AutoCorrRef_t autoCorr(const HWIdentifier&  CellID, int gain) const ;

  virtual AutoCorrRef_t autoCorr(const Identifier&  CellID, int gain) const ;
  
 protected:

  // helper for MC z-phi symmetry 
  //  LArMCSym m_larmc;
  ToolHandle<ILArMCSymTool> m_larmcsym;


};


CLASS_DEF( LArAutoCorrMC, 217258659,1)
#endif 
