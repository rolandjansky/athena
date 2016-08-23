/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNGENTOOLS_MUONTRACKCOLLECTIONPROVIDER_H
#define MUONALIGNGENTOOLS_MUONTRACKCOLLECTIONPROVIDER_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkAlignInterfaces/ITrackCollectionProvider.h"

/**
   @file MuonTrackCollectionProvider.h
   @class MuonTrackCollectionProvider
   
   @brief Tool used to get Muon collection from StoreGate and return collection of tracks

   @author Robert Harrington <roberth@bu.edu>
   @date 7/29/2010
*/

namespace MuonCombined {
  class IMuonTrackTagTool;
}

namespace Muon {

  class MuonAlignRefitTool;

  class MuonTrackCollectionProvider : virtual public Trk::ITrackCollectionProvider, public AthAlgTool {

  public:

    enum TrackType {
      
      /** Analysis::Muon */
      Muon = 0,
      
      /** MS standalone and ID tracks */
      MSStandaloneWithIDTrackMatch = 1,

      /** Combined muon track */
      CombinedMuonTrack = 2,

      /** number of track types */
      NumberOfTrackTypes = 3
    };

    MuonTrackCollectionProvider(const std::string & type, const std::string & name, const IInterface * parent);
    virtual ~MuonTrackCollectionProvider();
    
    StatusCode initialize();
    StatusCode finalize();
    
    StatusCode trackCollection(const TrackCollection*& tracks);
    StatusCode trackCollectionFromMuonContainer(const TrackCollection*& tracks);
    StatusCode trackCollectionFromTrackContainers(const TrackCollection*& tracks);
    StatusCode trackCollectionFromCombinedTrack(const TrackCollection*& tracks);
        
  private:

    ToolHandle<MuonCombined::IMuonTrackTagTool>   m_tagTool;

    ToolHandle<MuonAlignRefitTool> m_muonAlignRefitTool;

    //TrackCollection*     m_trackCollection;
    
    std::string m_inputMuonCol;    //!< Name of input Analysis::Muon collection
    
    std::string m_inputTrackCol;   //!< Name of input muon Trk::Track collection

    std::string m_inputIDTrackCol; //!< Name of input ID Trk::Track collection
 
    //int m_nRetrievalErrors;    //!< number of retrieval errors at the beginning of the job
    //int m_maxRetrievalErrors;  //!< maximum allowed number of retrieval errors at the beginning of the job (-1 for infinite)
    
    bool m_useStandaloneTracks; //!< flag to return standalone track (returns combined if false)

    int    m_inputTrackType; //!< type of input container(s)
    
    double m_momentumCut;       //!< momentum cut
    double m_trackMatchDeltaR;  //!< track match deltaR cut

    bool m_removePerigeeBeforeRefit; //!> remove perigee from track before refit

  }; // end class

} // end namespace

#endif // MUONALIGNGENTOOLS_MUONTRACKCOLLECTIONPROVIDER_H
