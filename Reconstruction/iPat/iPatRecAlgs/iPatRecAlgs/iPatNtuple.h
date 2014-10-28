/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
   
NAME:     iPatNtuple.h
PACKAGE:  offline/Reconstruction/iPatRec/iPatRecAlgs

AUTHORS:  Alan Poppleton - Nektarios Benekos
CREATED:  January 31, 2003

PURPOSE:  Header file for the iPatRec Algorithm
ATLAS Collaboration 
********************************************************************/


#ifndef IPATRECALGS_IPATNTUPLE_H
# define IPATRECALGS_IPATNTUPLE_H

//<<<<<< INCLUDES >>>>>>

#include <string>
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ToolHandle.h"
#include "HepMC/GenParticle.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class ITruthParameters;
class ITruthSelector;
class iPatTrackContainer;
 
class iPatNtuple:public AthAlgorithm 
{
public:

    //-------------------------
    // Constructors/Destructors
    //
    // Athena requires that the constructor takes certain arguments
    // (and passes them directly to the constructor of the base class)
    //-------------------------
 
    // Algorithm Constructor
 
    iPatNtuple(const std::string& name, ISvcLocator* pSvcLocator);
    ~iPatNtuple(void);
 
    //------------------------------------------------------
    // Methods used by Athena to run the algorithm
    // Define Athena Algorithm HBOOKS
    //------------------------------------------------------
        
    // Gaudi algorithm hooks

    /** Standard Athena-Algorithm method */ 
    StatusCode initialize(void) ;
    /** Standard Athena-Algorithm method */ 
    StatusCode execute(void) ;
    /** Standard Athena-Algorithm method */ 
    StatusCode finalize(void) ;
              
private:

    void					initNTuple(void);

    int 					m_eventCount; 
    bool                                  m_g3;
    int					m_maxKine;	// max number of kine tracks stored in ntuple
    int					m_maxReco;	// max number of reconstructed tracks stored in ntuple
  
    // pointer to an ntuple
    NTuple::Tuple* 			m_ntuple;
    const iPatTrackContainer*		m_tracks;
    // define the name of the iPatTrack Container registered in TDS:
    std::string 			m_tracksName;
    ToolHandle<ITruthParameters>	m_truthParameters;
    ToolHandle<ITruthSelector>		m_truthSelector;
           
    // number of Track data per event objects found by the pattern recognition
    // Ntuple data member

    // long sequential event number
    long				m_iPatEvent;

    NTuple::Item<long>			m_RunNum;
    NTuple::Item<long> 			m_EventNum;
    NTuple::Item<long> 			m_iPatEventNum;
    
    // Hits
    // b-layer
    NTuple::Item<long> 			m_b_layer_clusters_unique;
    NTuple::Item<long> 			m_b_layer_clusters_spoilt;
    NTuple::Item<long> 			m_b_layer_clusters_wrong;
    NTuple::Item<long> 			m_b_layer_clusters_shared;
    NTuple::Item<long> 			m_b_layer_clusters_total;
    //outer pixel    
    NTuple::Item<long> 			m_outer_pixel_clusters_unique;
    NTuple::Item<long> 			m_outer_pixel_clusters_spoilt;
    NTuple::Item<long> 			m_outer_pixel_clusters_wrong;
    NTuple::Item<long> 			m_outer_pixel_clusters_shared;
    NTuple::Item<long> 			m_outer_pixel_clusters_total;
    // pixel
    NTuple::Item<long> 			m_pixel_clusters_unique;
    NTuple::Item<long> 			m_pixel_clusters_spoilt;
    NTuple::Item<long> 			m_pixel_clusters_wrong;
    NTuple::Item<long> 			m_pixel_clusters_shared;
    NTuple::Item<long> 			m_pixel_clusters_total;  
    // strip
    NTuple::Item<long> 			m_strip_clusters_unique;
    NTuple::Item<long> 			m_strip_clusters_spoilt;
    NTuple::Item<long> 			m_strip_clusters_wrong;
    NTuple::Item<long> 			m_strip_clusters_shared;
    NTuple::Item<long> 			m_strip_clusters_total;
    // drift  
    NTuple::Item<long> 			m_drift_unique;
    NTuple::Item<long> 			m_drift_spoilt;
    NTuple::Item<long> 			m_drift_wrong;
    NTuple::Item<long> 			m_drift_wires;  
    NTuple::Item<long> 			m_drift_shared; 
    NTuple::Item<long> 			m_drift_rejects;
    NTuple::Item<long> 			m_drift_hits;    
    NTuple::Item<long> 			m_drift_time_hits;
    NTuple::Item<long> 			m_tr_hits;        

    NTuple::Item<long> 			m_total_shared_hits;
    NTuple::Item<long> 			m_total_spoilt_hits;
    NTuple::Item<long> 			m_total_unique_hits;
    NTuple::Item<long> 			m_total_wrong_hits;  

    NTuple::Item<long> 			m_boundary_number;
    NTuple::Item<long> 			m_pending_number;
    NTuple::Item<long> 			m_planar_holes;

    // tracks        
    NTuple::Item<long>			m_nreco;          
    NTuple::Array<float>		m_fitprob;
    NTuple::Array<float> 		m_fitchi2;    
    NTuple::Array<float> 		m_pt;
    NTuple::Array<float> 		m_ipt;   
    NTuple::Array<float> 		m_phi;
    NTuple::Array<float> 		m_eta;
    NTuple::Array<float> 		m_cottheta;
    NTuple::Array<float> 		m_A0;
    NTuple::Array<float> 		m_Z0;
    NTuple::Item<float> 		m_zMax;

    // KINE variables
    NTuple::Item<long>			m_nkine;
    NTuple::Item<long> 			m_barcode;
    NTuple::Array<long> 		m_pdgpart;    
    NTuple::Array<long> 		m_statuspart;       
    NTuple::Array<float> 		m_pttrue;
    NTuple::Array<float> 		m_ipttrue;    
    NTuple::Array<float> 		m_A0true;
    NTuple::Array<float> 		m_Z0true;
    NTuple::Array<float> 		m_phitrue;
    NTuple::Array<float> 		m_cthetatrue;
    NTuple::Array<float> 		m_sthetatrue;        
    NTuple::Array<float> 		m_etatrue;
    NTuple::Array<float> 		m_absetatrue;
  
  
    NTuple::Item<float> 		m_ptptt;
    NTuple::Item<float> 		m_iptptt;
    
    NTuple::Item<float> 		m_A0_pull;
    NTuple::Item<float> 		m_Z0_pull;
    NTuple::Item<float> 		m_phi_pull;
    NTuple::Item<float> 		m_ctheta_pull;
    NTuple::Item<float> 		m_ipt_pull;
    
    NTuple::Item<float> 		m_diffA0;
    NTuple::Item<float> 		m_diffZR;
    NTuple::Item<float> 		m_diffipt;
    NTuple::Item<float> 		m_diffphi;
    NTuple::Item<float> 		m_diffcot;
    NTuple::Item<float> 		m_diffeta;
};

 
#endif // IPATRECALGS_IPATNTUPLE_H
 
 
