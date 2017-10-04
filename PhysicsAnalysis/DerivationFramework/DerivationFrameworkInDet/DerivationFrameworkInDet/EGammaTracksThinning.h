/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H
#define DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"

#include "TLorentzVector.h"

#include "TrkTrack/TrackCollection.h" 
#include "TrkTrack/Track.h" 

namespace DerivationFramework {

  class EGammaTracksThinning:public AthAlgTool, public IThinningTool{

  public:

    EGammaTracksThinning(const std::string& type, 
                   const std::string& name, 
                   const IInterface* parent);
    
    ~EGammaTracksThinning(){}
    StatusCode initialize();
    StatusCode finalize();
    StatusCode doThinning() const ;
    
  private:
    
    ServiceHandle<IThinningSvc> m_thinningSvc;
    std::string m_tracksCollectionName;
    std::string m_electronsContainerName;
    std::string m_photonsContainerName;
    double m_dr;
    double m_minEtEg;

    std::set<int> findGoodTracks(const TrackCollection* trackCont,
                                 const TLorentzVector& candHepLorentz, 
                              double maxDeltaR) const ;
    

    StatusCode thinTracks( const TrackCollection * trackCont , const std::set<int>& goodTracks ) const ;
        
  };  
}

#endif // DERIVATIONFRAMEWORK_EGAMMATRACKSTHINNING_H

