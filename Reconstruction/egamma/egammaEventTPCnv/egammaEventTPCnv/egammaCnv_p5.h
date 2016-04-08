///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// egammaCnv_p5.h 
// Header file for class egammaCnv_p5
// New cnv to include Ringer: W.S.Freund<wsfreund@mail.cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef EGAMMAEVENTTPCNV_EGAMMACNV_P5_H 
#define EGAMMAEVENTTPCNV_EGAMMACNV_P5_H 1

// STL includes

// AthenaPoolCnvSvc includes
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

// egammaEventTPCnv includes
#include "egammaEventTPCnv/egamma_p5.h"

// egammaEvent includes
#include "egammaEvent/egamma.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplEEtaPhiMCnv_p2.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"

// Forward declaration
class MsgStream;

class egammaCnv_p5 : public T_AthenaPoolTPCnvBase<egamma, egamma_p5>
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  /** Default constructor: 
   */
  egammaCnv_p5();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /** Method creating the transient representation of @c egamma
   *  from its persistent representation @c egamma_p5
   */
  virtual void persToTrans( const egamma_p5* persObj, 
                            egamma* transObj, 
                            MsgStream& msg ) override final;

  /** Method creating the persistent representation @c egamma_p5
   *  from its transient representation @c egamma
   */
  virtual void transToPers( const egamma* transObj, 
                            egamma_p5* persObj, 
                            MsgStream& msg ) override final;

  /////////////////////////////////////////////////////////////////// 
  // Protected method: 
  /////////////////////////////////////////////////////////////////// 
 protected: 
  
 private:
  typedef ElementLinkCnv_p3<ElementLink<CaloClusterContainer> > ClusterLinkCnv_t;
  typedef ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> > TrackLinkCnv_t;
  typedef ElementLinkVectorCnv_p1<ElementLinkVector<VxContainer> > ConversionLinkCnv_t;
  typedef ElementLinkVectorCnv_p1<ElementLinkVector<egDetailContainer> > EgDetailsCnv_t;
  typedef ElementLinkCnv_p3<ElementLink<CaloRingsContainer> > RingsLinkCnv_t;

  // pre-allocate converters
  P4ImplEEtaPhiMCnv_p2 m_momCnv;
  ParticleBaseCnv_p2 m_partBaseCnv;
  ClusterLinkCnv_t   m_clusterCnv;
  TrackLinkCnv_t     m_trackCnv;
  ConversionLinkCnv_t     m_conversionCnv;
  EgDetailsCnv_t     m_egDetailsCnv;
  RingsLinkCnv_t     m_ringsCnv;

}; 

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 

inline egammaCnv_p5::egammaCnv_p5()
{}

#endif //> EGAMMAEVENTTPCNV_EGAMMACNV_P5_H
