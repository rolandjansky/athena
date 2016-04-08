// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigElectron.cxx

Description: To be created by FEX algorithms in Level 2 chains. Encodes 
             electron candidate. Should contain parameters that are used 
	     for cuts in L2 and point to original objects (track and EM 
	     cluster) so that the latter can be separated off for EDM slimming.
  
Design: Must be light and easy to serialize AND to store in ESD/AOD; 
        since pointers don't work (copied by POOL), use ElementLinks 
	instead (now -- May 08 -- being made serializable)

	The idea is that the variables used for cuts should be kept in
	the TrigElectron class for redundancy. The remaining
	quantities, not used for cuts, are accessible from the
	ElementLinks to TrigEMCluster and TrigInDetTrack. This allows
	for EDM slimming (or thinning, whatever, removing unnecessary
	information) by not storing the cluster and track.  

	In practice, some calorimeter variables would have to be
	recalculated from the actual variables stored in
	TrigEMCluster. To avoid the time overhead, the original
	variables are copied.

Authour: R.Goncalo - Tue Sep 20 22:02:43 BST 2005

Modified: RG Mar 8 2006: replaced EMShowerMinimal with TrigEMCluster 
          RG Mar 20 2006: added bool m_valid for hypos to validate elec. 
          RG Nov  7 2006: to inherit from INav4M and 4-mom base
	  RG May 22 2008: ElementLinks to track and cluster;change data members
	  RG May 18 2009: add TRT variables
*/
#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigParticle/TrigElectron.h"

/** Default constructor */
TrigElectron::TrigElectron() : 
  P4PtEtaPhiM(0, 0, 0, 0.511*CLHEP::MeV), 
  NavigableTerminalNode(), 
  m_roiWord(0), 
  m_valid(false),
  m_tr_Algo(TrigInDetTrack::NULLID), 
  m_tr_Zvtx(0), 
  m_tr_nr_trt_hits(0),
  m_tr_nr_trt_hithresh_hits(0),
  m_tr_eta_at_calo(0), 
  m_tr_phi_at_calo(0), 
  m_etoverpt(0), 
  m_cl_eta(0), 
  m_cl_phi(0), 
  m_cl_Rcore(0), 
  m_cl_Eratio(0), 
  m_cl_EThad(0),
  //  m_cl_energy(0),
  m_cl_e_frac_S0(0),
  m_cl_e_frac_S1(0),
  m_cl_e_frac_S2(0),
  m_cl_e_frac_S3(0)
{
  m_cluster.reset();
  m_track.reset();
}

/** Constructor: 
    Faster to pass TrigEMClusterContainer and
    TrigInDetTrackCollection pointers and indices to track and
    cluster, so that ElementLinks to track and EM cluster can be
    initialized quickly     
    Design choice: pass data member values as constructor parameters,
    rather than providing accessors to update these values later;
    4-momentum variables can still be updated through the P4PtEtaPhiM
    maseclass accessors
*/
TrigElectron::TrigElectron(unsigned int roi,                                              // roi word
			   float trkEtaAtCalo, float trkPhiAtCalo, float EToverPT,        // track-cluster match variables
			   const TrigEMClusterContainer* cl_coll, unsigned int cl_index,  // links to track and cluster
			   const TrigInDetTrackCollection* tr_coll, unsigned int tr_index) 
  :   P4PtEtaPhiM(0,0,0,0.511*CLHEP::MeV), 
      NavigableTerminalNode(), 
      m_roiWord(roi), 
      m_valid(true), 
      m_tr_Zvtx(0),
      m_tr_nr_trt_hits(0),
      m_tr_nr_trt_hithresh_hits(0),
      m_tr_eta_at_calo(trkEtaAtCalo), 
      m_tr_phi_at_calo(trkPhiAtCalo), 
      m_etoverpt(EToverPT),
      m_cl_eta(0), 
      m_cl_phi(0), 
      m_cl_Rcore(0), 
      m_cl_Eratio(0), 
      m_cl_EThad(0),
      //      m_cl_energy(0),
      m_cl_e_frac_S0(0),
      m_cl_e_frac_S1(0),
      m_cl_e_frac_S2(0),
      m_cl_e_frac_S3(0)
{

  /** Set data members from tracking and calorimeter quantities. The
      idea is that the variables used for cuts should be kept in the
      TrigElectron class for redundancy. The remaining quantities, not
      used for cuts, are accessible from the ElementLinks to
      TrigEMCluster and TrigInDetTrack. This allows for EDM slimming
      (or thinning, whatever, removing unnecessary information) by not
      storing the cluster and track.  In practice, some calorimeter
      variables would have to be recalculated from the actual
      variables stored in TrigEMCluster. To avoid the time overhead,
      the original variables are copied. */
  
  // track variables
  if (tr_coll != NULL && tr_coll->size() > tr_index) {

    /** Set ElementLinks to TrigEMCluster and TriginDetTrack: use fast
	method with reference to collection + index (O(1)), instead of
	reference to track (O(n) for n elements in collection).
	Notes: 
	1. the cluster container is held by steering; track container is per RoI 
	2. if e-links not valid, just set data member links to default value   */
    m_track.toIndexedElement(*tr_coll, tr_index);
    if (!m_track.isValid()) {
      m_track.reset();
      m_valid  = false;
    }

    /** set data members from track quantities */
    const TrigInDetTrack* tr_ptr = (*tr_coll)[tr_index];

    // get track author and nr. trt hits from track
    m_tr_Algo                 = tr_ptr->algorithmId();    
    m_tr_nr_trt_hits          = tr_ptr->NStrawHits();
    m_tr_nr_trt_hithresh_hits = tr_ptr->NTRHits();      

    /** Initialize 4-momentum base-class angular quantities from track
	data: assume that track parameters at perigee give better
	estimates of angular quantities near vertex  */
    if ( tr_ptr->param() != NULL ) {
      m_tr_Zvtx = tr_ptr->param()->z0();
      this->setEta(tr_ptr->param()->eta());  // set to track eta
      this->setPhi(tr_ptr->param()->phi0()); // set to track phi
    } else {
      m_valid     = false;
    }

  } else {
    m_tr_Algo   = TrigInDetTrack::NULLID;
    m_tr_Zvtx   = 0;
    m_valid     = false;
  }

  // calorimeter variables
  if (cl_coll != NULL && cl_coll->size() > cl_index) {

    /** Set ElementLinks to TrigEMCluster and TriginDetTrack: use fast
	method with reference to collection + index (O(1)), instead of
	reference to track (O(n) for n elements in collection).
	Notes: 
	1. the cluster container is held by steering; track container is per RoI 
	2. if e-links not valid, just set data member links to default value   */
    m_cluster.toIndexedElement(*cl_coll, cl_index);
    if (!m_cluster.isValid()) {
      m_cluster.reset();
      m_valid  = false;
    }

    /** set data members from cluster quantities */
    const TrigEMCluster* cl_ptr = (*cl_coll)[cl_index];

    /** Initialize 4-momentum base-class ET from cluster data: assume
	cluster quantities give better estimate of transverse energy */
    this->setPt(cl_ptr->et());             // set to cluster ET

    /** set other cluster-type data members */
    // eta-phi of cluster
    m_cl_eta    = cl_ptr->eta();
    m_cl_phi    = cl_ptr->phi();

    // EThad
    if (m_cl_eta != 0) {
      m_cl_EThad  = cl_ptr->ehad1()/cosh(fabs(m_cl_eta)); // Ehad1/cosh(fabs(eta) */
    } else {
      m_cl_EThad  = cl_ptr->ehad1();                      // Ehad1 (=EThad1)
    }

    // Rcore (test for positive energy to avoid zeros)
    if (cl_ptr->e277() != 0) { // (VD) before it was '>'
      m_cl_Rcore  =  cl_ptr->e237()/cl_ptr->e277();
    } else {
      m_cl_Rcore  = 0;
      m_valid     = false;
    }    

    // Eratio (test for positive energy to avoid zeros)
    if (cl_ptr->emaxs1()+cl_ptr->e2tsts1() > 0) {
      m_cl_Eratio = (cl_ptr->emaxs1()-cl_ptr->e2tsts1()) / (cl_ptr->emaxs1()+cl_ptr->e2tsts1());
    } else {
      m_cl_Eratio = 0;
      m_valid     = false;
    }

    // energy and energy fraction per sample
    float tmp_cl_energy = cl_ptr->energy();
    if (fabs(tmp_cl_energy) > 0.00001) { // avoid floating-point exceptions
      m_cl_e_frac_S0 = ( cl_ptr->energy(CaloSampling::PreSamplerB) + cl_ptr->energy(CaloSampling::PreSamplerE)) / tmp_cl_energy;
      m_cl_e_frac_S1 = ( cl_ptr->energy(CaloSampling::EMB1) + cl_ptr->energy(CaloSampling::EME1)) / tmp_cl_energy;
      m_cl_e_frac_S2 = ( cl_ptr->energy(CaloSampling::EMB2) + cl_ptr->energy(CaloSampling::EME2)) / tmp_cl_energy;
      m_cl_e_frac_S3 = ( cl_ptr->energy(CaloSampling::EMB3) + cl_ptr->energy(CaloSampling::EME3)) / tmp_cl_energy;
    }

  } else {
    m_cl_eta       = 0;
    m_cl_phi       = 0;
    m_cl_Rcore     = 0;
    m_cl_Eratio    = 0;
    m_cl_EThad     = 0;
    m_valid        = false;
    //    m_cl_energy    = 0;
    m_cl_e_frac_S0 = 0;
    m_cl_e_frac_S1 = 0;
    m_cl_e_frac_S2 = 0;
    m_cl_e_frac_S3 = 0;
  }
}


/** Initialize without accessing cluster/track objects. */
TrigElectron::TrigElectron(float pt,
                           float eta,
                           float phi,

                           // roi word
                           unsigned int roi,
                           bool valid,

                           // track-cluster match variables
			   float trkEtaAtCalo,
                           float trkPhiAtCalo,
                           float EToverPT,

                           // Cluster
                           const ElementLink< TrigEMClusterContainer >& cluster,
                           float caloEta,
                           float caloPhi,
                           float Rcore,
                           float Eratio,
                           float EThad,
                           float F0,
                           float F1,
                           float F2,
                           float F3,

                           // Track
                           const ElementLink< TrigInDetTrackCollection >& track,
                           TrigInDetTrack::AlgoId trackAlgo,
                           float Zvtx,
                           int nTRTHits,
                           int nTRTHiThresholdHits) 
  :   P4PtEtaPhiM(pt,eta,phi,0.511*CLHEP::MeV),
      m_roiWord(roi),
      m_valid(valid),
      m_tr_Algo(trackAlgo),
      m_tr_Zvtx(Zvtx),
      m_tr_nr_trt_hits(nTRTHits),
      m_tr_nr_trt_hithresh_hits(nTRTHiThresholdHits),
      m_tr_eta_at_calo(trkEtaAtCalo),
      m_tr_phi_at_calo(trkPhiAtCalo),
      m_etoverpt(EToverPT),
      m_cl_eta(caloEta),
      m_cl_phi(caloPhi),
      m_cl_Rcore(Rcore),
      m_cl_Eratio(Eratio),
      m_cl_EThad(EThad),
      m_cl_e_frac_S0(F0),
      m_cl_e_frac_S1(F1),
      m_cl_e_frac_S2(F2),
      m_cl_e_frac_S3(F3),
      m_cluster(cluster),
      m_track(track)
{
}


/** Copy constructor: Copy ElementLinks only if valid. Otherwise
    should leave as invalid in this object.
 */
TrigElectron::TrigElectron(const TrigElectron& te) :
  I4Momentum(te),
  P4PtEtaPhiMBase(te),
  INavigable(te),
  IAthenaBarCode(te),
  INavigable4Momentum(te),
  P4PtEtaPhiM(te),
  NavigableTerminalNode()
{
  m_roiWord  = te.m_roiWord;
  m_valid    = te.m_valid;
  // track variables
  m_tr_Algo  = te.m_tr_Algo; 
  m_tr_Zvtx  = te.m_tr_Zvtx;
  m_tr_nr_trt_hits = te.m_tr_nr_trt_hits;
  m_tr_nr_trt_hithresh_hits = te.m_tr_nr_trt_hithresh_hits;
  m_tr_eta_at_calo = te.m_tr_eta_at_calo;
  m_tr_phi_at_calo = te.m_tr_phi_at_calo;
  // track-cluster match
  m_etoverpt = te.m_etoverpt;
  // calorimeter variables
  m_cl_eta   = te.m_cl_eta;  
  m_cl_phi   = te.m_cl_phi;
  m_cl_Rcore = te.m_cl_Rcore; 
  m_cl_Eratio= te.m_cl_Eratio;
  m_cl_EThad = te.m_cl_EThad;
  //    m_cl_energy= te.m_cl_energy;
  m_cl_e_frac_S0 = te.m_cl_e_frac_S0;
  m_cl_e_frac_S1 = te.m_cl_e_frac_S1;
  m_cl_e_frac_S2 = te.m_cl_e_frac_S2;
  m_cl_e_frac_S3 = te.m_cl_e_frac_S3;


  // set ElementLink to cluster
  if (m_cluster.isValid()) {
    m_cluster.toIndexedElement( te.m_cluster.getStorableObjectRef(), te.m_cluster.index() );
  } else {
    m_cluster.reset();
  }

  // set ElementLink to track
  if (m_track.isValid()) {
    m_track.toIndexedElement( te.m_track.getStorableObjectRef(), te.m_track.index() );
  } else {
    m_track.reset();
  }
}

// destructor
TrigElectron::~TrigElectron() {}


// accessor to get pointer to cluster (TrigEMCluster) 
const TrigEMCluster* TrigElectron::cluster() const
{
  if (m_cluster.isValid()) {
    return (*m_cluster);
  } else {
    return NULL;
  }
}


// accessor to get pointer to track (TrigInDetTrack) 
const TrigInDetTrack* TrigElectron::track() const
{
  if (m_track.isValid()) {
    return (*m_track); // note: TrigInDetTrackContainer is a vector<TrigInDetTrack*>
  } else {
    return NULL;
  }
}

// comparison operators
/** Two TrigElectron are the same if they have the same cluster and
    the same track (and if valid, i.e. well built) */
// TEMPORARY: should fix it to allow proper comparison after re-running
bool operator==( const TrigElectron& te1,  const TrigElectron& te2 ) {
  return ( te1.isValid() && te2.isValid() 
	   && te1.cluster() == te2.cluster() && te1.track() == te2.track());
}


/**
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map v_diff where varaible names and _relative_
 * differences between two objects are returned
 */
void diff( const TrigElectron& te1, const TrigElectron& te2, std::map< std::string, double >& v_diff ) {

  v_diff["RoI"]            = te1.roiId() - te2.roiId();
  v_diff["Charge"]         = std::abs( te1.charge() - te2.charge() );
  v_diff["Eta"]            = fabs( te1.eta() - te2.eta() );
  double d_phi             = fabs( te1.phi() - te2.phi() );
  v_diff["Phi"]            = (d_phi < M_PI ? d_phi : 2*M_PI - d_phi);
  v_diff["Pt"]             = 2 * fabs( (fabs(te1.pt()) - fabs(te2.pt())) / (fabs(te1.pt())+fabs(te2.pt())) );
  v_diff["Zvtx"]           = fabs( te1.Zvtx() - te2.Zvtx() );
  v_diff["trkEtaEtCalo"]   = fabs( te1.trkEtaAtCalo() - te2.trkEtaAtCalo() );
  double d_phi_extrapol    = fabs( te1.trkPhiAtCalo() - te2.trkPhiAtCalo() );
  v_diff["trkPhiEtCalo"]   = (d_phi_extrapol < M_PI ? d_phi_extrapol : 2*M_PI - d_phi_extrapol);
  v_diff["TRThits"]        = abs(te1.nTRTHits() - te2.nTRTHits());
  v_diff["TRThitsHiThres"] = abs(te1.nTRTHiThresholdHits() - te2.nTRTHiThresholdHits());
  //  v_diff["E"]              = fabs(te1.E() - te2.E());
  v_diff["F0"]             = fabs(te1.F0() - te2.F0());
  v_diff["F1"]             = fabs(te1.F1() - te2.F1());
  v_diff["F2"]             = fabs(te1.F2() - te2.F2());
  v_diff["F3"]             = fabs(te1.F3() - te2.F3());

  return;
}


// print TrigElectron quantities
MsgStream& operator<< ( MsgStream& msg, const TrigElectron& te ) {
  return ( msg << "TrigElectron " << (te.isValid() ? "(valid)" : "(not valid)") 
	   << ": RoI = " << te.roiId() << "; Q = " << te.charge() 
	   << "; Et/Pt = " << te.EtOverPt() << "; Pt   = " << te.pt()
	   << "; TRT hits = " << te.nTRTHits() << "; TRT HiThreshold hits = " << te.nTRTHiThresholdHits() 
	   << "; trkPt = " << te.trkPt() << " +- " << te.err_Pt()
	   << "; eta = " << te.eta()   << " +- " << te.err_eta()
	   << "; phi = " << te.phi()   << " +- " << te.err_phi()
	   << "; Zvt = " << te.Zvtx()  << " +- " << te.err_Zvtx()
	   << "; DeltaEta1 = " << te.DeltaEta1() << "; DeltaPhi2 = " << te.DeltaPhi2() 
	   //	   << "; E = "<<te.E()
	   <<"; F0 = "<<te.F0()<<"; F1 = "<<te.F1()<<"; F2 = "<<te.F2()<<"; F3 = "<<te.F3() );
}
