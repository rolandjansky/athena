/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlapCnv_p1 - class to do T<->P conversion for TopOverlap

#ifndef TOPINPUTSTPCNV_TOPOVERLAPCNV_P1_H
#define TOPINPUTSTPCNV_TOPOVERLAPCNV_P1_H

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopInputs/TopOverlap.h"
#include "TopInputsTPCnv/TopOverlap_p1.h"

class TopOverlapCnv_p1 :
  public T_AthenaPoolTPCnvBase< TopRec::TopOverlap, TopOverlap_p1 >
{
  
public:

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  
  TopOverlapCnv_p1() {;}
  virtual ~TopOverlapCnv_p1() {;}

  /////////////////////////////////////////////////////////////////// 
  // Conversion methods: 
  ///////////////////////////////////////////////////////////////////
  
  virtual void persToTrans( const TopOverlap_p1* persObj,
			    TopRec::TopOverlap* transObj,
			    MsgStream &msg );

  virtual void transToPers( const TopRec::TopOverlap* transObj,
			    TopOverlap_p1* persObj,
			    MsgStream &msg);
  
private:
  ElementLinkVectorCnv_p1< ElementLinkVector<Analysis::MuonContainer> >     m_muon_cnv;
  ElementLinkVectorCnv_p1< ElementLinkVector<ElectronContainer> >           m_electron_cnv;
  ElementLinkVectorCnv_p1< ElementLinkVector<PhotonContainer> >             m_photon_cnv;
  ElementLinkVectorCnv_p1< ElementLinkVector<Analysis::TauJetContainer> >   m_tauJet_cnv;
  ElementLinkVectorCnv_p1< ElementLinkVector<JetCollection> >               m_jet_cnv;
  ElementLinkVectorCnv_p1< ElementLinkVector<Rec::TrackParticleContainer> > m_track_cnv;
};


#endif // not TOPINPUTSTPCNV_TOPOVERLAPCNV_P1_H
