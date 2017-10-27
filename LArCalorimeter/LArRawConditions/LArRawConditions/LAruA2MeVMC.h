/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARUA2MEVMC_H
#define LARRAWCONDITIONS_LARUA2MEVMC_H

#include "LArRawConditions/LAruA2MeVComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

/** Implementation of the interface ILAruA2MeV for MC
 *  Derives from LAruA2MeVComplete, and implements the phi-z symmetry
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 *
 * History:
 *  - 08/02/2004, S. Laplace: new online ID
 */


class LAruA2MeVMC: public LAruA2MeVComplete {
  
 public: 
  
  LAruA2MeVMC();
  
  virtual ~LAruA2MeVMC();
  virtual StatusCode initialize() ;
  
  // retrieving uA2MeV using online ID
  
  virtual const float& UA2MEV(const HWIdentifier&  CellID) const ;
  
  virtual const float& UA2MEV(const Identifier&  CellID) const;
  
  
 private: 

  ToolHandle<ILArMCSymTool> m_larmcsym;


};

#include "AthenaKernel/CondCont.h"
CLASS_DEF( LAruA2MeVMC, 172345330,1)
CONDCONT_DEF( LAruA2MeVMC, 245266716, ILAruA2MeV );
#endif 
