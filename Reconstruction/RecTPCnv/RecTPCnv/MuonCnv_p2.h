///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MuonCnv_p2.h 
// Header file for class MuonCnv_p2
// Author: Ketevi A. Assamagan <ketevi@bnl.gov>
/////////////////////////////////////////////////////////////////// 
#ifndef RECTPCNV_MUONCNV_P2_H 
#define RECTPCNV_MUONCNV_P2_H 

// STL includes
#include <string>

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// RecTPCnv includes
#include "RecTPCnv/Muon_p2.h"

// Forward declaration
class MsgStream;
namespace Analysis { class Muon; }

class MuonCnv_p2 : public T_AthenaPoolTPCnvConstBase<Analysis::Muon, Muon_p2>
{ 
 public: 
  using base_class::transToPers;
  using base_class::persToTrans;


  typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;

  /** Default constructor: 
   */
  MuonCnv_p2();


  /** Method creating the transient representation of @c Analysis::Muon
   *  from its persistent representation @c Muon_p1
   */
  virtual void persToTrans( const Muon_p2* persObj, 
                            Analysis::Muon* transObj, 
                            MsgStream& msg ) const override;

  /** Method creating the persistent representation @c Muon_p1
   *  from its transient representation @c Analysis::Muon
   */
  virtual void transToPers( const Analysis::Muon* transObj, 
                            Muon_p2* persObj, 
                            MsgStream& msg ) const override;

  void setKey ( const std::string& key ) { m_muonCaloEnergyContainerName=key; }
  void setEventStore( const StoreGateSvc_t storeGate ) { m_storeGate=storeGate; }

 protected: 

  StoreGateSvc_t m_storeGate;
  std::string m_muonCaloEnergyContainerName;
}; 


inline MuonCnv_p2::MuonCnv_p2()
  : m_storeGate( "StoreGateSvc", "MuonContainerCnv" ) 
{}

#endif //> RECTPCNV_MUONCNV_P2_H
