/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARRAWCONDITIONS_LARAUTOCORRMC_H
#define LARRAWCONDITIONS_LARAUTOCORRMC_H

#include "LArRawConditions/LArAutoCorrComplete.h"
#include "AthenaKernel/BaseInfo.h"
#include <vector>


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

};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArAutoCorrMC, 217258659,1)
CONDCONT_DEF(LArAutoCorrMC ,139308879 , ILArAutoCorr );
#endif 
