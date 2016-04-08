// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigPhoton.cxx

Description: To be created by hypothesis algorithms at end of Level 2 
             chains. To be used as a way to communicate L2 decisions and 
             to keep information available only online so far.
  
Design: Must be light and easy to serialize AND to store in ESD; should 
             avoid ElementLinks and try to persistify pointers instead; 
             should avoid inheritance to be easy to serialize.

Authour: R.Goncalo - Tue Jun 06 22:02:43 BST 2005

Modified: RG Nov  7 2006: to inherit from INav4M and 4-mom base
Modified: VD Oct 13 2008: change the photon constructor (see .h)

*/

#include "CaloGeoHelpers/CaloSampling.h"
#include "TrigParticle/TrigPhoton.h"

/** Default Constructor */
TrigPhoton::TrigPhoton() : 
  P4PtEtaPhiM(0, 0, 0, 0), 
  NavigableTerminalNode(),
  m_roiID(-1),  
  m_HadEt(0), 
  m_energyRatio(0),
  m_rCore(0),
  m_dPhi(0),
  m_dEta(0),
  //  m_cl_energy(0),
  m_cl_e_frac_S0(0),
  m_cl_e_frac_S1(0),
  m_cl_e_frac_S2(0),
  m_cl_e_frac_S3(0),
  m_Fside(0),
  m_Weta2(0),
  m_valid(false)

{
  m_cluster.reset();
}

/** Constructor:
    Pass TrigEMClusterContainer pointer and index to cluster 
    to initialize ElementLink to cluster quickly.     
    Design choice: data member values are passed the constructor through 
    TrigEMCluster parameters, rather than providing accessors to update 
    these values later; 4-momentum variables can still be updated through 
    the P4PtEtaPhiM baseclass accessors
*/
TrigPhoton::TrigPhoton(int roi, float dphi, float deta,
		       const TrigEMClusterContainer* cluster_cont,
		       unsigned int cluster_index        ) :
  P4PtEtaPhiM(0, 0, 0, 0), 
  NavigableTerminalNode(),
  m_roiID(roi), 
  m_HadEt(0), 
  m_energyRatio(0), 
  m_rCore(0),
  m_dPhi(dphi),
  m_dEta(deta),
  m_cl_e_frac_S0(0),
  m_cl_e_frac_S1(0),
  m_cl_e_frac_S2(0),
  m_cl_e_frac_S3(0),
  m_Fside(0),
  m_Weta2(0),
  m_valid(true)
{

  /** Set data members from calorimeter quantities. The
      idea is that the variables used for cuts should be kept in the
      TrigPhoton class for redundancy. Some remaining quantities, 
      not used for cuts, are accessible from the ElementLink to
      TrigEMCluster. This allows for EDM slimming by not storing the 
      cluster. */
  
  if (cluster_cont != NULL && cluster_cont->size() > cluster_index) {
    
    /** Set ElementLinks to TrigEMCluster : use fast
	method with reference to collection + index (O(1)), 
	Notes: 
	1. the cluster container is held by steering;
	2. if e-links not valid, just set data member links to default value   */
    m_cluster.toIndexedElement(*cluster_cont, cluster_index);
    

    /** retrieve TrigEMCluster from TrigEMClusterContainer */
    const TrigEMCluster* cl_ptr = (*cluster_cont)[cluster_index];


    /** Initialize 4-momentum base-class from cluster data */
    this->setPt(cl_ptr->et());           
    this->setEta(cl_ptr->eta());
    this->setPhi(cl_ptr->phi());

    /** set data members from cluster quantities */

    // EThad
    if (cl_ptr->eta() != 0) {
      m_HadEt  = cl_ptr->ehad1()/cosh(fabs(cl_ptr->eta())); // Ehad1/cosh(fabs(eta) 
    } else {
      m_HadEt  = cl_ptr->ehad1();                           // Ehad1 (=EThad1)
    }

    // Rcore (test for positive energy to avoid zeros)
    if (cl_ptr->e277() != 0) { // (VD) before it was '>'
      m_rCore  =  cl_ptr->e237()/cl_ptr->e277();
    } else {
      m_rCore  = 0;
      m_valid     = false;
    }    
    
    // Eratio (test for positive energy to avoid zeros)
    if (cl_ptr->emaxs1()+cl_ptr->e2tsts1() > 0) {
      m_energyRatio = (cl_ptr->emaxs1()-cl_ptr->e2tsts1()) / (cl_ptr->emaxs1()+cl_ptr->e2tsts1());
    } else {
      m_energyRatio = 0;
      m_valid     = false;
    }
   
    // energy and energy fraction per sample
    float tmp_cl_energy = cl_ptr->energy();
    if (fabs(tmp_cl_energy) > 0.00001) { // avoid floating-point exceptions
      m_cl_e_frac_S0 = ( cl_ptr->energy(CaloSampling::PreSamplerB) + cl_ptr->energy(CaloSampling::PreSamplerE)) / tmp_cl_energy;
      m_cl_e_frac_S1 = ( cl_ptr->energy(CaloSampling::EMB1) + cl_ptr->energy(CaloSampling::EME1)) / tmp_cl_energy;
      m_cl_e_frac_S2 = ( cl_ptr->energy(CaloSampling::EMB2) + cl_ptr->energy(CaloSampling::EME2)) / tmp_cl_energy;
      m_cl_e_frac_S3 = ( cl_ptr->energy(CaloSampling::EMB3) + cl_ptr->energy(CaloSampling::EME3)) / tmp_cl_energy;
      m_Fside        = cl_ptr->fracs1();
      m_Weta2        = cl_ptr->weta2();
    }
  } 
  else {
    m_cluster.reset();
    m_valid  = false;
    //    m_cl_energy = 0;
    m_cl_e_frac_S0 = 0;
    m_cl_e_frac_S1 = 0;
    m_cl_e_frac_S2 = 0;
    m_cl_e_frac_S3 = 0;
  }    
  

  /** set TrigPhoton invalid if e-link could not be made */
  if (!m_cluster.isValid()) {
    m_cluster.reset();
    m_valid  = false;
  }    

}


/** Initialize without accessing cluster objects. */
TrigPhoton::TrigPhoton(float pt,
                       float eta,
                       float phi,

                       // roi word
                       unsigned int roi,
                       bool valid,

                       // Cluster
                       const ElementLink< TrigEMClusterContainer >& cluster,
                       float HadEt,
                       float energyRatio,
                       float rCore,
                       float deta,
                       float dphi,
                       float Fside,
                       float Weta2,
                       float F0,
                       float F1,
                       float F2,
                       float F3)
  : P4PtEtaPhiM(pt, eta, phi, 0),
    m_roiID(roi),
    m_HadEt(HadEt),
    m_energyRatio(energyRatio),
    m_rCore(rCore),
    m_dPhi(dphi),
    m_dEta(deta),
    m_cl_e_frac_S0(F0),
    m_cl_e_frac_S1(F1),
    m_cl_e_frac_S2(F2),
    m_cl_e_frac_S3(F3),
    m_Fside(Fside),
    m_Weta2(Weta2),
    m_valid(valid),
    m_cluster(cluster)
{
}
    
/** Copy Constructor */
TrigPhoton::TrigPhoton(const TrigPhoton& tp) : 
  I4Momentum(tp),
  P4PtEtaPhiMBase(tp),
  INavigable(tp),
  IAthenaBarCode(tp),
  INavigable4Momentum(tp),
  P4PtEtaPhiM(tp),
  NavigableTerminalNode()
{
  m_roiID        = tp.m_roiID;
  m_HadEt        = tp.m_HadEt; 
  m_energyRatio  = tp.m_energyRatio; 
  m_rCore        = tp.m_rCore;
  m_dPhi         = tp.m_dPhi;
  m_dEta         = tp.m_dEta;
  //    m_cl_energy    = tp.m_cl_energy;
  m_cl_e_frac_S0 = tp.m_cl_e_frac_S0;
  m_cl_e_frac_S1 = tp.m_cl_e_frac_S1;
  m_cl_e_frac_S2 = tp.m_cl_e_frac_S2;
  m_cl_e_frac_S3 = tp.m_cl_e_frac_S3;
  m_Fside        = tp.m_Fside;
  m_Weta2        = tp.m_Weta2;
  m_valid        = tp.m_valid;

  //set ElementLink to cluster
  if( m_cluster.isValid() ){
    m_cluster.toIndexedElement( tp.m_cluster.getStorableObjectRef(), tp.m_cluster.index() );
  }
  else {
    m_cluster.reset();
  }
}

/** Destructor */
TrigPhoton::~TrigPhoton() {}

const TrigEMCluster* TrigPhoton::cluster() const 
{ 
  if( m_cluster.isValid() ){
    return (*m_cluster);
  }
  else{
    return NULL;
  }
}


// comparison operators
/** Two TrigPhoton are the same if they have the same cluster (and if valid) */
bool operator==( const TrigPhoton& tp1,  const TrigPhoton& tp2 ) {
  return ( tp1.isValid() && tp2.isValid() && tp1.cluster() == tp2.cluster());
}


/**
 * Function compares two objects and returns "semi verbose" output 
 * in the form of map v_diff where varaible names and _relative_
 * differences between two objects are returned
 */
void diff( const TrigPhoton& tp1,  const TrigPhoton& tp2, std::map< std::string, double >& v_diff ) {

  v_diff["RoIword"]     = tp1.roiId() - tp2.roiId();
  v_diff["Et"]          = 0;
  if(fabs(tp1.Et())+fabs(tp2.Et()) !=0)
	  v_diff["Et"]      = 2 * fabs( ( fabs(tp1.Et()) - fabs(tp2.Et() )) / ( fabs(tp1.Et())+fabs(tp2.Et()) ) );
  v_diff["HadEt1"]      = 0;
  if(fabs(tp1.HadEt1())+fabs(tp2.HadEt1()) !=0)
	  v_diff["HadEt1"]  = 2 * fabs( ( fabs(tp1.HadEt1()) - fabs(tp2.HadEt1() )) / ( fabs(tp1.HadEt1())+fabs(tp2.HadEt1()) ) );
  v_diff["Eratio"]      = fabs(tp1.Eratio() - tp2.Eratio());
  v_diff["rCore"]       = fabs(tp1.Reta() - tp2.Reta());
  v_diff["Eta"]         = fabs( tp1.eta() - tp2.eta() );
  double d_phi          = fabs( tp1.phi() - tp2.phi() );
  v_diff["Phi"]         = (d_phi < M_PI ? d_phi : 2*M_PI - d_phi);
  //  v_diff["E"]           = fabs(tp1.E() - tp2.E());
  v_diff["F0"]          = fabs(tp1.F0() - tp2.F0());
  v_diff["F1"]          = fabs(tp1.F1() - tp2.F1());
  v_diff["F2"]          = fabs(tp1.F2() - tp2.F2());
  v_diff["F3"]          = fabs(tp1.F3() - tp2.F3());
  return;
}


/** print TrigPhoton quantities */
MsgStream& operator<< ( MsgStream& msg, const TrigPhoton& tp ) {
  return ( msg << "TrigPhoton " << (tp.isValid() ? "(valid)" : "(not valid)") 
	   << ": RoI = " << tp.roiWord() 
	   << "; Et = "  << tp.Et() 
	   << "; eta = " << tp.eta()
	   << "; phi = " << tp.phi()
	   << "; Delta_eta = " << tp.dEta() << "; Delta_phi = " << tp.dPhi() 
	   << "; Eratio = " << tp.Eratio() << "; Rcore = " << tp.Reta() 
	   //	   << "; E = "<<tp.E()
	   <<"; F0 = "<<tp.F0()<<"; F1 = "<<tp.F1()<<"; F2 = "<<tp.F2()<<"; F3 = "<<tp.F3() );
}
