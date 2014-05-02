/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCMC_H
#define LARRAWCONDITIONS_LAROFCMC_H

#include "LArRawConditions/LArOFCComplete.h"
#include "GaudiKernel/ToolHandle.h"

#include <vector>

class LArCablingService ;
class ILArMCSymTool;

/** Implementation of the interface ILArOFC for MC
 *  Derives from LArOFCComplete, and implements the phi-z symmetry
 *
 * @author H. Ma 
 * @version  0-0-1 , Feb 17, 2007
 *
 */

class LArOFCMC: public LArOFCComplete {
  
 public: 

  typedef LArOFCComplete::CONTAINER  CONTAINER; 
  
  LArOFCMC();
  
  virtual ~LArOFCMC( );
  
  virtual StatusCode initialize () ; 

  // retrieving OFC using online ID

  virtual OFCRef_t OFC_a(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  virtual OFCRef_t OFC_b(const HWIdentifier&  CellID,
                         int gain,
                         int tbin=0) const ;

  virtual OFCRef_t OFC_a(const Identifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  virtual OFCRef_t OFC_b(const Identifier&  CellID,
                         int gain,
                         int tbin=0) const ;
  
 private: 

  // helper for MC z-phi symmetry 
  ToolHandle<ILArMCSymTool> m_larmcsym;

};


CLASS_DEF( LArOFCMC, 56306025,1)
#endif 
