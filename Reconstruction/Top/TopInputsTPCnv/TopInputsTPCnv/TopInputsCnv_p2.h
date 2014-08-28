/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// Does TP conversion for TopInputs class

#ifndef TOPINPUTSTPCNV_TOPINPUTSCNV_P2_H
#define TOPINPUTSTPCNV_TOPINPUTSCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TopInputs/TopInputs.h"
#include "TopInputsTPCnv/TopInputs_p2.h"
#include "TopInputsTPCnv/TopMuonMapCnv_p2.h"
#include "TopInputsTPCnv/TopElectronMapCnv_p2.h"
#include "TopInputsTPCnv/TopPhotonMapCnv_p1.h"
#include "TopInputsTPCnv/TopTauJetMapCnv_p1.h"
#include "TopInputsTPCnv/TopJetMapCnv_p1.h"
#include "TopInputsTPCnv/TopTrackMapCnv_p1.h"

class TopInputsCnv_p2 :
  public T_AthenaPoolTPCnvBase< TopRec::TopInputs, TopInputs_p2 >
{
  
public:
  
  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
  
  TopInputsCnv_p2() {;}
  virtual ~TopInputsCnv_p2() {;}

  /////////////////////////////////////////////////////////////////// 
  // Conversion methods: 
  ///////////////////////////////////////////////////////////////////
  
  virtual void persToTrans( const TopInputs_p2* persObj,
			    TopRec::TopInputs* transObj,
			    MsgStream &msg );

  virtual void transToPers( const TopRec::TopInputs* transObj,
			    TopInputs_p2* persObj,
			    MsgStream &msg);
 
private:
  TopInputMapTriggerCnv_p2<Analysis::MuonContainer> m_mu_cnv;
  TopInputMapTriggerCnv_p2<ElectronContainer> m_el_cnv;
  TopInputMapCnv_p1<PhotonContainer> m_ph_cnv;
  TopInputMapCnv_p1<Analysis::TauJetContainer> m_tau_cnv;
  TopInputMapCnv_p1<JetCollection> m_jet_cnv;
  TopInputMapCnv_p1<Rec::TrackParticleContainer> m_track_cnv;
  DataLinkCnv_p1<DataLink<MissingET> > m_dl_cnv;
};

#endif // not TOPINPUTSTPCNV_TOPINPUTSCNV_P2_H
