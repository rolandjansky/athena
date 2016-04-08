// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**

File: TrigElectron.h

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
	  RG May 22 2008: ElementLinks to track and cluster; change data members
	  RG Apr 07 2009: add comparison and << operators
	  RG May 18 2009: add TRT variables; remove obsolete validate() accessor
	  RG Jun 22 2009: add some calorimeter vars. changed TRT accessor names
*/

#ifndef TRIG_ELECTRON
#define TRIG_ELECTRON

#include <vector>
#include <iostream>
#include <string>
#include <cmath> 

#include "FourMom/P4PtEtaPhiM.h"
#include "Navigation/Navigable.h"
#include "EventKernel/INavigable4Momentum.h"

#include "DataModel/ElementLink.h"

#include "TrigCaloEvent/TrigEMClusterContainer.h"
#include "TrigInDetEvent/TrigInDetTrackCollection.h"

class TrigElectron : public P4PtEtaPhiM,
		     public NavigableTerminalNode ,
		     virtual public INavigable4Momentum {

 public:
  
  /** constructors; note: default constructor needed by POOL */
  TrigElectron();
  
  /** constructor used to initialize TrigElectrons (there are no "set()" methods)*/
  TrigElectron(unsigned int roi, 
	       float trkEtaAtCalo, float trkPhiAtCalo, float EToverPT,
	       const TrigEMClusterContainer* cluster, unsigned int cl_index,
	       const TrigInDetTrackCollection* track, unsigned int tr_index);

  /** Initialize without accessing cluster/track objects. */
  TrigElectron(float pt,
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
               int nTRTHiThresholdHits) ;

  /** copy constructor */
  TrigElectron(const TrigElectron& te);

  ~TrigElectron();
  
  /** accessor methods: */

  /** accessor to get RoI Word (unique to each L1 RoI) */
  int   roiId()    const { return m_roiWord;    } /** OBSOLETE: should disappear in next iteration */
  int   roiWord()  const { return m_roiWord;    }

  /** accessor to get TrigElectron valid flag */
  bool  isValid()  const { return m_valid;    }

  /** accessors to return the tracking quantities */
  TrigInDetTrack::AlgoId trackAlgo()const { return m_tr_Algo;}
  float Zvtx()                 const { return m_tr_Zvtx;                 }
  int   nTRTHits()             const { return m_tr_nr_trt_hits;          }
  int   nTRTHiThresholdHits()  const { return m_tr_nr_trt_hithresh_hits; }
  float nTRTHiTHitsRatio()     const { return (m_tr_nr_trt_hits>0 ? (float)m_tr_nr_trt_hithresh_hits/(float)m_tr_nr_trt_hits : -1); }

  /** accessor to return the track index in the collection and a
      reference to the TrigInDetTrackCollection itself */
  int trackIndx()  const { return (m_track.isValid() ? m_track.index() : 0); } 
  const TrigInDetTrackCollection* trackColl() const { return (m_track.isValid() ? m_track.getStorableObjectPointer() : NULL); } 

  /** accessor to get pT of electron candidate track */
  float trkPt()        const { return (m_track.isValid() ? fabs((*m_track)->param()->pT()) : 0 ); }

  /** accessor to return the track charge (sign of TrigInDetTrack pT) */
  int charge()     const { return (m_track.isValid() ? (int)((*m_track)->param()->pT()/fabs((*m_track)->param()->pT())) : 0); } 

  /** accessors to get uncertainty in track parameters: Pt, eta, phi and Z of track perigee (Zvtx) */
  float err_Pt()    const { return err_trkPt(); } // OBSOLETE: to be removed in next iteration 
  float err_trkPt() const { return (m_track.isValid() ? (*m_track)->param()->epT()   : -1); } 
  float err_eta()   const { return (m_track.isValid() ? (*m_track)->param()->eeta()  : -1); }
  float err_phi()   const { return (m_track.isValid() ? (*m_track)->param()->ephi0() : -1); } 
  float err_Zvtx()  const { return (m_track.isValid() ? (*m_track)->param()->ez0()   : -1); } 

  /** accessor to get eta and phi of electron candidate track extrapolated the calorimeter */
  float trkEtaAtCalo() const { return m_tr_eta_at_calo;  }
  float trkPhiAtCalo() const { return m_tr_phi_at_calo;  }

  /** accessors to get eta and phi difference between cluster and
      track extrapolated the calorimeter face (with -Pi,Pi limits) */ 
  float trkClusDeta()  const { return (DeltaEta1()); } // old method name - RG 15/6/09
  float trkClusDphi()  const { return (DeltaPhi2()); } // old method name - RG 15/6/09
  float DeltaEta1()    const { return fabs(m_tr_eta_at_calo - m_cl_eta);  }
  float DeltaPhi2()    const { return (fabs(m_tr_phi_at_calo-m_cl_phi)<M_PI ? 
				       fabs(m_tr_phi_at_calo-m_cl_phi) : 
				       2*M_PI - fabs(m_tr_phi_at_calo-m_cl_phi)); }

  /** accessor to ratio of cluster Et to track Pt */
  float EtOverPt() const { return m_etoverpt; }

  /** accessors to get cluster eta and phi (note baseclass eta and phi come from track) */
  float caloEta() const { return m_cl_eta;    }
  float caloPhi() const { return m_cl_phi;    }

  /** accessors for shower shape quantities */
  float Rcore()   const { return m_cl_Rcore;  }    // OBSOLETE name; to be removed in next iteration
  float Reta()    const { return m_cl_Rcore;  }
  float Eratio()  const { return m_cl_Eratio; }
  float EThad()   const { return m_cl_EThad;  }    // OBSOLETE name; to be removed in next iteration
  float Ethad1()  const { return m_cl_EThad;  }

  /** accessors for calibrated energy fractions in each sampling - to reject noise */ 
  float F0()      const { return m_cl_e_frac_S0; } // sampling 0 
  float F1()      const { return m_cl_e_frac_S1; } // sampling 1 
  float F2()      const { return m_cl_e_frac_S2; } // sampling 2 
  float F3()      const { return m_cl_e_frac_S3; } // sampling 3 

  /** accessor to get pointer to cluster (TrigEMCluster) */
  const TrigEMCluster* cluster() const;
  const ElementLink< TrigEMClusterContainer >& clusterLink() const
  { return m_cluster; }

  /** accessor to get pointer to track (TrigInDetTrack) */
  const TrigInDetTrack* track() const;
  const ElementLink< TrigInDetTrackCollection >& trackLink() const
  { return m_track; }

 private:
  // private data members
  /** roi word unique to L1 RoI (note: different from HLT RoI id)*/
  unsigned int m_roiWord; 

  /** To certify "good" TrigElectrons: used to be necessary for
      studies; now basically always true for electrons produced by
      TrigL2IDCaloFex if not in AcceptAll mode (in which case this
      variable is always false). Can also be set to false in case of
      errors found in constructor (see TrigElectron.cxx). */
  bool  m_valid; 

  /** Tracking quantities:
      ID of the tracking algorithm that produced the track in this
      electron. Given by the enum:
      AlgoId{NULLID=0,SITRACKID=1,IDSCANID=2,TRTLUTID=3,TRTXKID=4}; */
  TrigInDetTrack::AlgoId m_tr_Algo; 

  /** Z of the track at closest approach (perigee) to the Z axis
      (maybe later to the beam?) given by the track finder. This is
      important to check for compatibility with a common vertex in the
      case of Z->ee signatures, for example */
  float m_tr_Zvtx; 

  /** number of TRT hits and high-threshold hits */  
  int m_tr_nr_trt_hits;
  int m_tr_nr_trt_hithresh_hits;

  /** Track/Cluster matching quantities: eta and phi of track
      extrapolated to the calorimeter face */
  float m_tr_eta_at_calo;  
  float m_tr_phi_at_calo;

  /** Ratio of cluster transverse energy to track transv.momentum */ 
  float m_etoverpt;

  /** Calorimeter quantities:  
      Cluster eta and phi  */
  float m_cl_eta; 
  float m_cl_phi;

  /** Energy ratio in 3x7 and 7x7 cells in 2nd LAr sampling; Rcore =
      e237/e277 */
  float m_cl_Rcore;  

  /** Energy ratio in highest and 2nd highest strip pairs in 1st LAr
      sampling; Eratio = EmaxS1-E2ndS1 / EmaxS1+E2ndS1 */
  float m_cl_Eratio; 

  /** ET in first hadronic sampling: EThad=Ehad1/cosh(fabs(eta) */
  float m_cl_EThad;

  /** Calibrated overall and per-sampling energy - to be used for cuts*/
  //  float m_cl_energy; // removed 
  float m_cl_e_frac_S0;
  float m_cl_e_frac_S1;
  float m_cl_e_frac_S2;
  float m_cl_e_frac_S3;

  /** ElementLinks to the track and calorimeter cluster in electron candidate */
  ElementLink< TrigEMClusterContainer >    m_cluster;
  ElementLink< TrigInDetTrackCollection >  m_track;

};

// comparison operators
bool operator==( const TrigElectron& te1,  const TrigElectron& te2 );
inline bool operator!=( const TrigElectron& te1,  const TrigElectron& te2 ) {
  return !( te1 == te2 );
}
void diff( const TrigElectron& te1,  const TrigElectron& te2, std::map<std::string, double>& v_diff );

// print TrigElectron quantities
MsgStream& operator<< ( MsgStream& msg, const TrigElectron& te );


CLASS_DEF( TrigElectron , 144277868 , 1 )

#endif // TRIG_ELECTRON
