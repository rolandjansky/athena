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
    SG::ThinningHandleKey<TrackCollection> m_tracksCollectionName
      { this, "tracksCollectionName", "Tracks", "" };
    std::string m_electronsContainerName;
    std::string m_photonsContainerName;
    double m_dr;
    double m_minEtEg;

    std::set<int> findGoodTracks(const TrackCollection* trackCont,
                                 const TLorentzVector& candHepLorentz, 
                              double maxDeltaR) const ;
    

    StatusCode thinTracks( SG::ThinningHandle<TrackCollection>& trackCont ,
                           const std::set<int>& goodTracks ) const ;
        
  };  
}

#endif // DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H

