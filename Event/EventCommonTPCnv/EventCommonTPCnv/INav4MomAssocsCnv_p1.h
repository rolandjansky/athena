///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv_p1.h 
// Header file for class INav4MomAssocsCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P1_H 
#define EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P1_H 

// STL includes

// HepMC / CLHEP includes

// Gaudi includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// NavFourMom includes
#define private public
#define protected public
#include "NavFourMom/INav4MomAssocs.h"
#undef private
#undef protected

// EventCommonTPCnv includes
#include "EventCommonTPCnv/INav4MomAssocs_p1.h"

// Forward declaration
class MsgStream;
class StoreGateSvc;

class INav4MomAssocsCnv_p1 : public T_AthenaPoolTPCnvBase<
                                              INav4MomAssocs, 
                                              INav4MomAssocs_p1
                                            >  
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  INav4MomAssocsCnv_p1();

  /** Constructor with parameters: 
   */
  INav4MomAssocsCnv_p1( StoreGateSvc* storeGate );

  /** Destructor: 
   */
  virtual ~INav4MomAssocsCnv_p1();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c INav4MomAssocs
   *  from its persistent representation @c INav4MomAssocs_p1
   */
  virtual void persToTrans( const INav4MomAssocs_p1* persObj, 
                            INav4MomAssocs* transObj, 
                            MsgStream &log );

  /** Method creating the persistent representation @c INav4MomAssocs_p1
   *  from its transient representation @c INav4MomAssocs
   */
  virtual void transToPers( const INav4MomAssocs* transObj, 
                            INav4MomAssocs_p1* persObj, 
                            MsgStream &log );

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /////////////////////////////////////////////////////////////////// 
  // Protected data: 
  /////////////////////////////////////////////////////////////////// 
 protected: 

  /** Pointer to the StoreGateSvc: we need this as we'll have to fetch
   *  the @c INav4MomAssocs which is the parent collection (the
   *  collection from it has been made) of the transient 
   *  @c TruthParticleContainer has been created.
   */
  StoreGateSvc* m_storeGate;

}; 

/////////////////////////////////////////////////////////////////// 
/// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline INav4MomAssocsCnv_p1::INav4MomAssocsCnv_p1() :
  m_storeGate ( 0 )
{}

inline INav4MomAssocsCnv_p1::INav4MomAssocsCnv_p1( StoreGateSvc* storeGate ) :
  m_storeGate ( storeGate )
{}

inline INav4MomAssocsCnv_p1::~INav4MomAssocsCnv_p1()
{}

#endif //> EVENTCOMMONTPCNV_INAV4MOMASSOCSCNV_P1_H
