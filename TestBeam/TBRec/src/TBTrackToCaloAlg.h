/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackToCaloAlg.h
//   Header file for class TrackToCaloAlg
///////////////////////////////////////////////////////////////////
// Top algorithm class for extrapolating tracks to Calo
///////////////////////////////////////////////////////////////////


#ifndef TBREC_TBTRACKTOCALOALG_H
#define TBREC_TBTRACKTOCALOALG_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
//#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"   // typedef
//#include "Particle/TrackParticleContainer.h"
//#include "Particle/TrackParticle.h"
//#include "TrackToCalo/ImpactInCalo.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
//class IExtrapolTrackToCaloTool;
class IExtrapolateToCaloTool;
class TBExtrapolTrackToCaloTool;
class ICaloCoordinateTool;
class CaloCellList;
class CaloDepthTool;
class ImpactInCalo;

namespace Trk {
    class Track;
}
namespace Rec {
    class TrackParticle;
    class TrackParticleContainer;
}

#include "CaloIdentifier/CaloCell_ID.h"

/**

This Algorithm is meant to be an example of use of the TrackToCalo tools :

 - It loops on a given Track collection ( choosen by jobOption ) and extrapolates all tracks
   to the layers of the calorimeter. 

 - Impacts are stored in an ImpactInCaloCollection, which is then used to fill CBNT

 - An example of primitive Track to Cluster matching is provided, as well as an example of how
   one can retreive all the CaloCell's around a track extrapolation, using the CaloCellList
   class ( these 2 are off by default )

 - One can also find an example of usage of the CaloDepthTool methods to define if one
   should extrapolate to the barrel or to the endcap.

*/

class TBTrackToCaloAlg : public AthAlgorithm  {

public:

  // Constructor with parameters:
  TBTrackToCaloAlg(const std::string &name,ISvcLocator *pSvcLocator);

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // Basic algorithm methods:
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  // Loop on Trk::Tracks and create ImpactInCaloCollection
  bool CreateTrkImpactInCalo();
  ImpactInCalo* GetImpactsInCalo(const Trk::Track* track, bool& got_a_track);

  // Retreive ImpactInCaloCollection and compare to CaloClusters
  void CompareImpactWithCluster();
  void PrintImpact();

  // List of cells crossed by Trk::Tracks 
  // in sampling_or_module = 0,1,2,3
  // ( neta and nphi are the total window width, e.g .5x5 )
  CaloCellList* CellsCrossedByTrack(const Trk::Track* trk , 
				    const CaloCell_ID::CaloSample sam, 
				    int neta, int nphi);
  bool PrintCellsCrossed();


  // A little check : where is the beam ?
  void PrintBeamPosition();

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:
  TBTrackToCaloAlg();
  TBTrackToCaloAlg(const TBTrackToCaloAlg&);
  TBTrackToCaloAlg &operator=(const TBTrackToCaloAlg&);
  
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  // Tracking variables :
  std::string m_TrackName;
  const TrackCollection *m_tracks; 
  std::string m_TrackParticleName;
  const Rec::TrackParticleContainer *m_particle;
  std::string m_trkinput;

  // Calo variables :
  std::string m_cluster_container;
  std::string m_cell_container;
  const CaloCell_ID* m_calo_id;

  // Between the 2 :
  std::string m_ImpactInCalosOutputName;
  TBExtrapolTrackToCaloTool* m_toCalo;
  CaloDepthTool* m_calodepth;
  CaloPhiRange m_phiRange;

  // these ones are only here for debugging, not for general use !
  ICaloCoordinateTool* m_calo_tb_coord;

  SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey{this,"CaloDetDescrManager", "CaloDetDescrManager"};

}; 

#endif //TESTLARDETDESCR_TESTTRACKTOCALO_H

