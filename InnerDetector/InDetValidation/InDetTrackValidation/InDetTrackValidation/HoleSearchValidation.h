/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// HoleSearchValidation.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRK_HOLESEARCHVALIDATION_H
#define TRK_HOLESEARCHVALIDATION_H

// Gaudi includes
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

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

  class HoleSearchValidation : public AthAlgorithm {
  public:
    /** Standard Athena-Algorithm Constructor */
    HoleSearchValidation(const std::string& name, ISvcLocator* pSvcLocator);
    /** Default Destructor */
    ~HoleSearchValidation();
    
    /** standard Athena-Algorithm method */
    StatusCode          initialize();
    /** standard Athena-Algorithm method */
    StatusCode          execute();
    /** standard Athena-Algorithm method */
    StatusCode          finalize();

  private:

    void printInfoTSoS( const Trk::TrackStateOnSurface* tsos);
    unsigned int doHoleSearch( const Trk::Track* track);

    const AtlasDetectorID*                m_idHelper;
    const PixelID*                        m_pixelID;
    const SCT_ID*                         m_sctID; 
    const TRT_ID*                         m_trtID; 
    const SiliconID*                      m_siliconID; 

    ToolHandle<Trk::ITrackHoleSearchTool>    m_holeSearchTool;
    std::string         m_trackCollectionName;     //!< jobOption: name of the TrackCollection
    //unsigned int   m_numberHitsToRemove;  //!< jobOption: number of hits to remove from tracks
    //unsigned int   m_numberHitsToKeep;    //!< jobOption: number of hits to remove 
    bool           m_saveNewTracksInSG;   //!< jobOption: save new tracks to SG ?
   

    bool           m_removeSctSide0;
    bool           m_removeSctSide1;

    bool           m_removePix0;
    bool           m_removePix1;
    bool           m_removePix2;

    bool           m_removeSct0;
    bool           m_removeSct1;
    bool           m_removeSct2;
    bool           m_removeSct3;
    bool           m_removeSct4;
    bool           m_removeSct5;
    bool           m_removeSct6;
    bool           m_removeSct7;
    bool           m_removeSct8;

    bool           m_removeOverlapHitsOnly;
    bool           m_ignoreTrackEnds;

    bool           m_randomRemovalMode;
    unsigned int   m_maxNumberOfHoles;   

    ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;       //!< Random number generator
    CLHEP::HepRandomEngine*      m_randomEngine;
    std::string                  m_randomEngineName; //!< Name of the random number stream

    // some information for statistics -> printout in finalize
    std::vector< std::vector< unsigned int > >  m_trackStats; //!< # tracks as function of [#generated holes, #found holes]

  }; // end of class

} // close of namespace

#endif // TRK_HOLESEARCHVALIDATION_H

