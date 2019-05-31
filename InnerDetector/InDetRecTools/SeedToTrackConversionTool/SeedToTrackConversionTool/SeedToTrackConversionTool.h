// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SeedToTrackConversionTool.h, (c) ATLAS Detector software
// Class for converting Seeds to Track Collection 
// Version 0.0 01/02/2016 Weiming Yao(LBNL)  
///////////////////////////////////////////////////////////////////

#ifndef INDETSEEDTOTRACKCONVERSIONTOOL_H
#define INDETSEEDTOTRACKCONVERSIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetRecToolInterfaces/ISeedToTrackConversionTool.h"
#include "StoreGate/WriteHandleKey.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/TrackCollection.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <list>
#include <mutex>

class MsgStream;

namespace InDet 
{

  /** @class SeedToTrackConversionTool 

      InDet::SeedToTrackConversionTool is tool that converts the seeds
      into track collection. 
      
      @author  Weiming Yao <Weiming.Yao>
  */  

  class SeedToTrackConversionTool : public extends<AthAlgTool, ISeedToTrackConversionTool>
  {
  public:
    SeedToTrackConversionTool(const std::string&,const std::string&,const IInterface*);

    /** default destructor */
    virtual ~SeedToTrackConversionTool() = default;
      
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize();
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize();

    // Main methods for seeds conversion
    virtual void executeSiSPSeedSegments(const Trk::TrackParameters*, const int&, const std::list<const Trk::SpacePoint*>&) const;
    //!<seed trackparameters, number of tracks found:m_track.size(), list of spacepoints
    virtual void newEvent(const Trk::TrackInfo&, const std::string&) const;
    virtual void endEvent() const;

    //////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
      
    virtual MsgStream&    dump(MsgStream&    out) const;
    virtual std::ostream& dump(std::ostream& out) const;
    // enter declaration of your interface-defining member functions here
      
  private:
    PublicToolHandle<Trk::IExtrapolator> m_extrapolator
      {this, "Extrapolator","Trk::Extrapolator", "extrapolator"};
    PublicToolHandle<Trk::IRIO_OnTrackCreator > m_rotcreator
      {this, "RIO_OnTrackCreator", "Trk::RIO_OnTrackCreator/InDetRotCreator", "Creator ROT"};
    SG::WriteHandleKey<TrackCollection> m_seedsegmentsOutput{this, "OutputName", "SiSPSeedSegments", "SiSpSeedSegments Output Collection"};

    mutable std::atomic_int m_nprint{0}; //!< Kind output information
    mutable std::atomic_int m_totseed{0}; //!< number of total seeds in the pass
    mutable std::atomic_int m_survived{0}; //!< number of survived seeds 

    mutable std::mutex m_mutex;
    mutable std::vector<EventContext::ContextEvt_t> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
    struct EventData { // To hold event dependent data
      std::unique_ptr<TrackCollection> m_seedSegmentsCollection; //!< output collection for seed
      Trk::TrackInfo m_trackInfo; //!< TrackInfo for seeds
      std::string m_patternName; //!< Name of the pattern recognition
    };
    mutable std::vector<EventData> m_eventData ATLAS_THREAD_SAFE; // Guarded by m_mutex

    EventData& getEventData() const;

    MsgStream& dumpconditions(MsgStream& out) const;
    MsgStream& dumpevent     (MsgStream& out) const;

  }; 
  MsgStream&    operator << (MsgStream&   , const SeedToTrackConversionTool&);
  std::ostream& operator << (std::ostream&, const SeedToTrackConversionTool&); 
} // end of namespace

#endif 
