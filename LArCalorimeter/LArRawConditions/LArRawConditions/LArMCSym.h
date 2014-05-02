/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARMCSYM_H
#define LARRAWCONDITIONS_LARMCSYM_H


/** Helper class to handle z-phi symmetry of calibration constants in MC
 *
 * @author S. Laplace
 * @version  0-0-1 , 02/02/2004
 */

#include "Identifier/HWIdentifier.h"

class StoreGateSvc; 
class LArCablingService ;
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;

class LArMCSym {

 public:

  LArMCSym();
  virtual ~LArMCSym() ;
  
  // method to handle z-phi symmetry
  HWIdentifier ZPhiSym(const HWIdentifier& NotSymOnId) const;
  
  Identifier ZPhiSym(const Identifier& NotSymOffId) const;
  
 private:

  const LArEM_ID*           m_lar_em_id; 
  const LArHEC_ID*          m_lar_hec_id;   
  const LArFCAL_ID*         m_lar_fcal_id; 
  LArCablingService*        m_cablingService;
  StoreGateSvc*             m_detStore;

};

#endif 
