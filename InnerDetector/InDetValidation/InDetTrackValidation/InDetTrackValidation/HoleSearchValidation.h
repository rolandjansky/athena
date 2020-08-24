/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HoleSearchValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_HOLESEARCHVALIDATION_H
#define TRK_HOLESEARCHVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "TrkTrack/TrackCollection.h"
#include <mutex>

// forward declarations
class AtlasDetectorID;
class PixelID;
class SCT_ID;
class TRT_ID; 
class SiliconID;

namespace CLHEP {
  class HepRandomEngine; 
}

namespace Trk {
    class ITrackHoleSearchTool;
    class TrackStateOnSurface;
    class Track;
}

/** @class HoleSearchValidation

   Test algorithm that can run after the ID reco.
   It takes a track collection, runs the hole search, dumps the holes found, 
   then removes some hits from the track by making an incomplete copy of the track
   (no refit !!!), dumps into the logfile which hits are removed, 
   then runs the hole search again and dumps the result again.

   @author Till.Eifert@cern.ch
*/

namespace InDet {

  class HoleSearchValidation : public AthReentrantAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    HoleSearchValidation(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~HoleSearchValidation();

    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute(const EventContext& ctx) const;
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

  private:

    void printInfoTSoS( const Trk::TrackStateOnSurface* tsos) const;
    unsigned int doHoleSearch( const Trk::Track* track) const;

    const AtlasDetectorID*                m_idHelper;
    const PixelID*                        m_pixelID;
    const SCT_ID*                         m_sctID; 
    const TRT_ID*                         m_trtID; 
    const SiliconID*                      m_siliconID; 

    ToolHandle<Trk::ITrackHoleSearchTool>    m_holeSearchTool;
    SG::ReadHandleKey<TrackCollection> m_trackCollectionKey;     //!< jobOption: name of the TrackCollection
    SG::WriteHandleKey<TrackCollection> m_trackCollectionOutputKey;     //!< jobOption: name of the TrackCollection
    //unsigned int   m_numberHitsToRemove;  //!< jobOption: number of hits to remove from tracks
    //unsigned int   m_numberHitsToKeep;    //!< jobOption: number of hits to remove 
    bool           m_saveNewTracksInSG;   //!< jobOption: save new tracks to SG ?
   

    struct Parts {
      enum EParts {
        kPix0,
        kPix1,
        kPix2,

        kSct0,
        kSct1,
        kSct2,
        kSct3,
        kSct4,
        kSct5,
        kSct6,
        kSct7,
        kSct8,
        kSctSide0,
        kSctSide1,
        kNParts
      };
    };

    bool m_removeParts[Parts::kNParts];
    bool           m_removeOverlapHitsOnly;
    bool           m_ignoreTrackEnds;

    bool           m_randomRemovalMode;
    unsigned int   m_maxNumberOfHoles;   

    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;       //!< Random number generator
    CLHEP::HepRandomEngine*      m_randomEngine;
    std::string                  m_randomEngineName; //!< Name of the random number stream

    // some information for statistics -> printout in finalize
    mutable std::mutex                                  m_trackStatsMutex;
    mutable std::vector< std::vector< unsigned int > >  m_trackStats ATLAS_THREAD_SAFE; //!< # tracks as function of [#generated holes, #found holes]. Guarded by m_trackStatsMutex

  }; // end of class

} // close of namespace

#endif // TRK_HOLESEARCHVALIDATION_H

