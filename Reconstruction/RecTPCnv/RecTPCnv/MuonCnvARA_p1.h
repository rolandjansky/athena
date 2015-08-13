///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnvARA_p1.h 
// Header file for class MuonCnvARA_p1
// Version of the converter that works with ARA.
// Author: snyder@bnl.gov
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCNVARA_P1_H 
#define RECTPCNV_MUONCNVARA_P1_H 

// STL includes
#include <string>

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/Muon_p1.h"

#include "muonEvent/CaloEnergy.h"


// Forward declaration
class MsgStream;
namespace Analysis { class Muon; }

class MuonCnvARA_p1 : public T_AthenaPoolTPCnvBase<Analysis::Muon, Muon_p1>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c Analysis::Muon
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const Muon_p1* persObj, 
                            Analysis::Muon* transObj, 
                            MsgStream& msg );

  /** Method creating the persistent representation @c Muon_p1
   *  from its transient representation @c Analysis::Muon
   */
  virtual void transToPers( const Analysis::Muon* transObj, 
                            Muon_p1* persObj, 
                            MsgStream& msg );


  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

#endif //> RECTPCNV_MUONCNV_P1_H
