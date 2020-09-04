/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-
#ifndef D2PDMAKER_D2PDPHOTONSELECTOR_H
#define D2PDMAKER_D2PDPHOTONSELECTOR_H

//============================================================================
// Name: D2PDPhotonSelector.h
//
/**
   @class D2PDPhotonSelector

   @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>

   @date April 2009

   @brief Select photons and write out good ones as ElementLinks

   Class to select good photons from an input container (which can either
   be an photon container or a container of ElementLinks poining to an
   photon container) and write out the good ones as a new container of
   ElementLinks pointing to the good photons in the original input container.
*/
//=============================================================================

// Include the header of the base class
#include "D2PDMaker/D2PDSelectorBase.h"

// STL includes
#include <string>
#include <vector>

// Gaudi includes
#include "GaudiKernel/ServiceHandle.h"

// egamma includes
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/PhotonContainer.h"
#include "egammaEvent/Photon.h"
#include "CaloEvent/CaloCluster.h"



class D2PDPhotonSelector : public D2PDSelectorBase< Analysis::Photon, PhotonContainer >
{
public:

  /** Default constructor */
  D2PDPhotonSelector(const std::string& name, ISvcLocator* pSvcLocator);

  /** Default destructor */
  virtual ~D2PDPhotonSelector() ;
  

private:
  /** This method is executed once at the end of the 
      standard athena initialize() method. */
  StatusCode initializeMore();

  /** This method is executed ONCE PER EVENT, right before
      the loop over the electrons begins. */
  //StatusCode executeMore();

  /** This method is executed ONCE FOR EVERY PROCESSED PHOTON. */
  StatusCode processObject( const Analysis::Photon* photon,
                            bool &isPassed );

  /** This method is executed once at the end of the 
      standard athena finalize() method. */
  StatusCode finalizeMore();


private:

  /** Get a handle on the kinematic selector tool for clusters */
  ToolHandle<KinematicSelector> m_clusterKinSelectorTool;

  /** Get a handle on the kinematic selector tool for a certain EM calorimeter layer of the clusters */
  ToolHandle<KinematicSelector> m_clusterCaloLayerKinSelectorTool;

  /** Get a handle on the kinematic selector tool for tracks */
  ToolHandle<KinematicSelector> m_trackKinSelectorTool;



  //------------------------------------------------------
  // Call-back methods
  //------------------------------------------------------

  /// Call-back method to configure the cut properties (min/max): IsEM
  void setupIsEM( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): passID
  void setupPassID( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): Author
  void setupAuthor( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined et
  void setupUncombinedEt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined e
  void setupUncombinedE( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined eta
  void setupUncombinedEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined |eta|
  void setupUncombinedAbsEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined phi
  void setupUncombinedPhi( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): uncombined mass
  void setupUncombinedMass( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster et
  void setupClusterEt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster e
  void setupClusterE( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster eta
  void setupClusterEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster |eta|
  void setupClusterAbsEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster eta veto
  void setupClusterEtaVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster phi
  void setupClusterPhi( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster phi veto
  void setupClusterPhiVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster mass
  void setupClusterMass( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer e
  void setupClusterLayerE( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer eta
  void setupClusterLayerEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer |eta|
  void setupClusterLayerAbsEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer eta veto
  void setupClusterLayerEtaVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer phi
  void setupClusterLayerPhi( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): cluster layer phi veto
  void setupClusterLayerPhiVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track pt
  void setupTrackPt( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track p
  void setupTrackP( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track eta
  void setupTrackEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track |eta|
  void setupTrackAbsEta( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track eta veto
  void setupTrackEtaVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track phi
  void setupTrackPhi( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track phi veto
  void setupTrackPhiVeto( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track mass
  void setupTrackMass( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): track pt iso
  void setupTrackPtIso( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): |track pt iso|
  void setupAbsTrackPtIso( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): calo et iso
  void setupCaloEtIso( Gaudi::Details::PropertyBase& prop );

  /// Call-back method to configure the cut properties (min/max): |calo et iso|
  void setupAbsCaloEtIso( Gaudi::Details::PropertyBase& prop );




  //------------------------------------------------------
  // Selections
  //------------------------------------------------------

  //-----------------------
  // Photon quantities
  //-----------------------


  /// do isEM cut
  bool m_doIsEM;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxIsEM;

  /** isEM selection */
  LongProperty m_isEM;


  /// do passID cut
  bool m_doPassID;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxPassID;

  /** identification selection */
  LongProperty m_passID;


  /// do author cut
  bool m_doAuthor;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAuthor;

  /** author selection */
  LongProperty m_author;


  /// do  cut
  bool m_doAcceptAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAcceptAuthorList;

  /** author selection list; logical OR */
  std::vector<long> m_acceptAuthorList;


  /// do  cut
  bool m_doRequireAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxRequireAuthorList;

  /** author selection list; logical AND */
  std::vector<long> m_requireAuthorList;


  /// do  cut
  bool m_doVetoAuthorList;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxVetoAuthorList;

  /** author selection list; logical NOT */
  std::vector<long> m_vetoAuthorList;



  //-----------------------
  // Uncombined 4-momenta quantities
  //-----------------------


  /// do  cut
  bool m_doUncombinedEt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedEt;

  /** uncombined minimum transverse energy */
  DoubleProperty m_uncombinedEtMin;

  /** uncombined maximum transverse energy */
  DoubleProperty m_uncombinedEtMax;


  /// do  cut
  bool m_doUncombinedE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedE;

  /** uncombined minimum energy */
  DoubleProperty m_uncombinedEMin;

  /** uncombined maximum energy */
  DoubleProperty m_uncombinedEMax;


  /// do  cut
  bool m_doUncombinedEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedEta;

  /** uncombined minimum eta */
  DoubleProperty m_uncombinedEtaMin;

  /** uncombined maximum eta */
  DoubleProperty m_uncombinedEtaMax;


  /// do  cut
  bool m_doUncombinedAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedAbsEta;

  /** uncombined minimum |eta| */
  DoubleProperty m_uncombinedAbsEtaMin;

  /** uncombined maximum |eta| */
  DoubleProperty m_uncombinedAbsEtaMax;

  
  /// do  cut
  bool m_doUncombinedPhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedPhi;

  /** uncombined minimum phi */
  DoubleProperty m_uncombinedPhiMin;

  /** uncombined maximum phi */
  DoubleProperty m_uncombinedPhiMax;


  /// do  cut
  bool m_doUncombinedMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxUncombinedMass;

  /** uncombined minimum mass */
  DoubleProperty m_uncombinedMassMin;

  /** uncombined maximum mass */
  DoubleProperty m_uncombinedMassMax;


  //-----------------------
  // Cluster quantities
  //-----------------------

  /// do  cut
  bool m_doClusterEt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterEt;

  /** cluster minimum transverse energy */
  DoubleProperty m_clusterEtMin;

  /** cluster maximum transverse energy */
  DoubleProperty m_clusterEtMax;


  /// do  cut
  bool m_doClusterE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterE;

  /** cluster minimum energy */
  DoubleProperty m_clusterEMin;

  /** cluster maximum energy */
  DoubleProperty m_clusterEMax;


  /// do  cut
  bool m_doClusterEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterEta;

  /** cluster minimum eta */
  DoubleProperty m_clusterEtaMin;

  /** cluster maximum eta */
  DoubleProperty m_clusterEtaMax;


  /// do  cut
  bool m_doClusterAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterAbsEta;

  /** cluster minimum |eta| */
  DoubleProperty m_clusterAbsEtaMin;

  /** cluster maximum |eta| */
  DoubleProperty m_clusterAbsEtaMax;


  /// do  cut
  bool m_doClusterEtaVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterEtaVeto;

  /** cluster eta ranges in which the particle should NOT be */
  StringProperty  m_clusterEtaVetoRanges;

  
  /// do  cut
  bool m_doClusterPhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterPhi;

  /** cluster minimum phi */
  DoubleProperty m_clusterPhiMin;

  /** cluster maximum phi */
  DoubleProperty m_clusterPhiMax;


  /// do  cut
  bool m_doClusterPhiVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterPhiVeto;

  /** cluster phi ranges in which the particle should NOT be */
  StringProperty  m_clusterPhiVetoRanges;
  

  /// do  cut
  bool m_doClusterMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterMass;

  /** cluster minimum mass */
  DoubleProperty m_clusterMassMin;

  /** cluster maximum mass */
  DoubleProperty m_clusterMassMax;


  //-----------------------
  // Cluster in EM Calo layer quantities
  //-----------------------

  /// do  cut
  bool m_doClusterLayerE;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerE;

  /** Layer in the EM calorimeter to look at */
  int m_emCaloLayer;

  /** cluster in EM calorimeter layer minimum energy */
  DoubleProperty m_clusterCaloLayerEMin;

  /** cluster in EM calorimeter layer maximum energy */
  DoubleProperty m_clusterCaloLayerEMax;


  /// do  cut
  bool m_doClusterLayerEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerEta;

  /** cluster in EM calorimeter layer minimum eta */
  DoubleProperty m_clusterCaloLayerEtaMin;

  /** cluster in EM calorimeter layer maximum eta */
  DoubleProperty m_clusterCaloLayerEtaMax;


  /// do  cut
  bool m_doClusterLayerAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerAbsEta;

  /** cluster in EM calorimeter layer minimum |eta| */
  DoubleProperty m_clusterCaloLayerAbsEtaMin;

  /** cluster in EM calorimeter layer maximum |eta| */
  DoubleProperty m_clusterCaloLayerAbsEtaMax;


  /// do  cut
  bool m_doClusterLayerEtaVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerEtaVeto;

  /** cluster in EM calorimeter layer eta ranges in which the particle should NOT be */
  StringProperty  m_clusterCaloLayerEtaVetoRanges;

  
  /// do  cut
  bool m_doClusterLayerPhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerPhi;

  /** cluster in EM calorimeter layer minimum phi */
  DoubleProperty m_clusterCaloLayerPhiMin;

  /** cluster in EM calorimeter layer maximum phi */
  DoubleProperty m_clusterCaloLayerPhiMax;


  /// do  cut
  bool m_doClusterLayerPhiVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxClusterLayerPhiVeto;

  /** cluster in EM calorimeter layer phi ranges in which the particle should NOT be */
  StringProperty  m_clusterCaloLayerPhiVetoRanges;


  //-----------------------
  // Track quantities
  //-----------------------

  /// do  cut
  bool m_doTrackPt;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackPt;

  /** track minimum transverse momentum */
  DoubleProperty m_trackPtMin;

  /** track maximum transverse momentum */
  DoubleProperty m_trackPtMax;


  /// do  cut
  bool m_doTrackP;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackP;

  /** track minimum momentum */
  DoubleProperty m_trackPMin;

  /** track maximum momentum */
  DoubleProperty m_trackPMax;


  /// do  cut
  bool m_doTrackEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackEta;

  /** track minimum eta */
  DoubleProperty m_trackEtaMin;

  /** track maximum eta */
  DoubleProperty m_trackEtaMax;


  /// do  cut
  bool m_doTrackAbsEta;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackAbsEta;

  /** track minimum |eta| */
  DoubleProperty m_trackAbsEtaMin;

  /** track maximum |eta| */
  DoubleProperty m_trackAbsEtaMax;


  /// do  cut
  bool m_doTrackEtaVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackEtaVeto;

  /** track eta ranges in which the particle should NOT be */
  StringProperty  m_trackEtaVetoRanges;

  
  /// do  cut
  bool m_doTrackPhi;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackPhi;

  /** track minimum phi */
  DoubleProperty m_trackPhiMin;

  /** track maximum phi */
  DoubleProperty m_trackPhiMax;


  /// do  cut
  bool m_doTrackPhiVeto;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackPhiVeto;

  /** track phi ranges in which the particle should NOT be */
  StringProperty  m_trackPhiVetoRanges;
  

  /// do  cut
  bool m_doTrackMass;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackMass;

  /** track minimum mass */
  DoubleProperty m_trackMassMin;

  /** track maximum mass */
  DoubleProperty m_trackMassMax;



  //-----------------------
  // Isolation quantities
  //-----------------------

  /** egammaParameter for track isolation */
  std::string m_egammaDetailKey;

  /** egammaParameter for track isolation */
  int m_egammaTrackIsoParam;

  /** egammaParameter for calorimeter isolation */
  int m_egammaCaloIsoParam;


  /// do  cut
  bool m_doTrackPtIso;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxTrackPtIso;

  /** Track isolation minimum pt / object pt */
  DoubleProperty m_relTrackPtIsoMin;

  /** Track isolation maximum pt / object pt */
  DoubleProperty m_relTrackPtIsoMax;


  /// do  cut
  bool m_doAbsTrackPtIso;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAbsTrackPtIso;

  /** Track isolation minimum pt */
  DoubleProperty m_absTrackPtIsoMin;

  /** Track isolation maximum pt */
  DoubleProperty m_absTrackPtIsoMax;


  /// do  cut
  bool m_doCaloEtIso;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxCaloEtIso;

  /** Calorimeter isolation minimum et / object et */
  DoubleProperty m_relCaloEtIsoMin;

  /** Calorimeter isolation maximum et / object et */
  DoubleProperty m_relCaloEtIsoMax;


  /// do  cut
  bool m_doAbsCaloEtIso;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxAbsCaloEtIso;

  /** Calorimeter isolation minimum et */
  DoubleProperty m_absCaloEtIsoMin;

  /** Calorimeter isolation maximum et */
  DoubleProperty m_absCaloEtIsoMax;



  /// do  cut
  bool m_doEGammaParam;

  /// Index of where in the internal cutflow the cut is bookkept
  unsigned int m_idxEGammaParam;

  /** egammaParameter list for selection */
  std::vector<int> m_egammaParameterList;

  /** egammaParameter lower cut list */
  std::vector<double> m_egammaParameterCutMinList;

  /** egammaParameter upper cut list */
  std::vector<double> m_egammaParameterCutMaxList;


private:
  //-----------------------
  // Private member variables
  //-----------------------
  CaloCluster* m_tmp4Mom;


}; // End class D2PDPhotonSelector



///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline void D2PDPhotonSelector::setupIsEM( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doIsEM = true;
  return;
}

inline void D2PDPhotonSelector::setupPassID( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doPassID = true;
  return;
}

inline void D2PDPhotonSelector::setupAuthor( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doAuthor = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedEt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedEt = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedE( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedE = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedEta = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedAbsEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedAbsEta = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedPhi( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedPhi = true;
  return;
}

inline void D2PDPhotonSelector::setupUncombinedMass( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doUncombinedMass = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterEt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterEt = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterE( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterE = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterEta = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterAbsEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterAbsEta = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterEtaVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterEtaVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterPhi( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterPhi = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterPhiVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterPhiVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterMass( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterMass = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerE( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerE = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerEta = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerAbsEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerAbsEta = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerEtaVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerEtaVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerPhi( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerPhi = true;
  return;
}

inline void D2PDPhotonSelector::setupClusterLayerPhiVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doClusterLayerPhiVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackPt( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackPt = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackP( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackP = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackEta = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackAbsEta( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackAbsEta = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackEtaVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackEtaVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackPhi( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackPhi = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackPhiVeto( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackPhiVeto = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackMass( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackMass = true;
  return;
}

inline void D2PDPhotonSelector::setupTrackPtIso( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doTrackPtIso = true;
  return;
}

inline void D2PDPhotonSelector::setupAbsTrackPtIso( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doAbsTrackPtIso = true;
  return;
}

inline void D2PDPhotonSelector::setupCaloEtIso( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doCaloEtIso = true;
  return;
}

inline void D2PDPhotonSelector::setupAbsCaloEtIso( Gaudi::Details::PropertyBase& /*prop*/ )
{
  m_doAbsCaloEtIso = true;
  return;
}


#endif
