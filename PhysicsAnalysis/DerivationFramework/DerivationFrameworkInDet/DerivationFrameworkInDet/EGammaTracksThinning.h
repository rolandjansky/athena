/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"

#include "TLorentzVector.h"

#include "TrkTrack/TrackCollection.h" 
#include "TrkTrack/Track.h" 
#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ThinningHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/ElectronContainer.h"


namespace DerivationFramework {

  class EGammaTracksThinning:public extends<AthAlgTool, IThinningTool> {

  public:

    EGammaTracksThinning(const std::string& type, 
                   const std::string& name, 
                   const IInterface* parent);
    
    virtual  ~EGammaTracksThinning(){}
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    virtual StatusCode doThinning() const override;
    
  private:
    
    StringProperty m_streamName
      { this, "StreamName", "", "Name of the stream being thinned" };
    SG::ReadHandleKey<xAOD::ElectronContainer> m_electronsContainerKey
      { this, "electronContainerName", "Electrons", "" };
    SG::ReadHandleKey<xAOD::PhotonContainer>   m_photonsContainerKey
      { this, "photonContainerName", "Photons", ""};
    SG::ThinningHandleKey<TrackCollection> m_tracksCollectionName
      { this, "tracksCollectionName", "Tracks", "" };
    Gaudi::Property<double> m_dr
      { this, "deltaR", 0.5, "" };
    Gaudi::Property<double> m_minEtEg
      { this, "minEtEg", 0,""};

    std::set<int> findGoodTracks(const TrackCollection* trackCont,
                                 const TLorentzVector& candHepLorentz, 
                              double maxDeltaR) const ;
    

    StatusCode thinTracks( SG::ThinningHandle<TrackCollection>& trackCont ,
                           const std::set<int>& goodTracks ) const ;
        
  };  
}

#endif // DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H

