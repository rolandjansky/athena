/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARDAC2UAMC_H
#define LARRAWCONDITIONS_LARDAC2UAMC_H

#include "LArRawConditions/LArDAC2uAComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

/** Implementation of the interface ILArDAC2uA for MC
 *  Derives from LArDAC2uAComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 *  - 29/04/2004, S. Laplace: added ERRORCODE
 */

class LArDAC2uAMC: public LArDAC2uAComplete {
  
 public: 
  
  LArDAC2uAMC();
  
  virtual ~LArDAC2uAMC( );
  virtual StatusCode initialize() ;
  
  // retrieving DAC2uA using online ID
  
  virtual const float& DAC2UA(const HWIdentifier&  CellID) const ;
  
  virtual const float& DAC2UA(const Identifier&  CellID) const;
  
  static float dummy;
  
 private: 
  
  // helper for MC z-phi symmetry 
//  LArMCSym m_larmc;
   ToolHandle<ILArMCSymTool> m_larmcsym;

};


#include "AthenaKernel/CondCont.h"
CLASS_DEF( LArDAC2uAMC, 48944014,1)
CONDCONT_DEF( LArDAC2uAMC, 218623664, ILArDAC2uA );
#endif 
