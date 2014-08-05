/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUON_MUONTRACKTOSEGMENTTOOL_H
#define MUON_MUONTRACKTOSEGMENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "TrkParameters/TrackParameters.h"

#include "Identifier/Identifier.h"


#include <vector>

class MuonStationIntersectSvc;
class MsgStream;

namespace MuonGM {
  class MuonDetectorManager;
}

namespace Trk {
  class IPropagator;
  class Track;
  class MeasurementBase;
}

namespace Muon {
  class MuonSegment;
  class MuonIdHelperTool;
  class MuonEDMHelperTool;
  class MuonEDMPrinterTool;
}

namespace Muon {

  /** 
      @class MuonTrackToSegmentTool

      Tool to combine two segments. It appoximates the combined segment by a straight line. 
      The tool assumes that this is a valid assumption.
      
      For more details look at the mainpage of this package.
  */
  class MuonTrackToSegmentTool :  virtual public IMuonTrackToSegmentTool, public AthAlgTool   {
  public:
    typedef std::vector<const Trk::MeasurementBase*> MeasVec;
    typedef MeasVec::iterator                        MeasIt;
    typedef MeasVec::const_iterator                  MeasCit;

  public:
    /** default AlgTool constructor */
    MuonTrackToSegmentTool(const std::string&, const std::string&, const IInterface*);
    
    /** destructor */
    ~MuonTrackToSegmentTool();
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();

    /** finialize method, method taken from bass-class AlgTool */
    StatusCode finalize();
    
    /** @brief convert track to segment */
    MuonSegment* convert( const Trk::Track& track ) const;

  private:
    /** @brief calculate holes */
    std::vector<Identifier> calculateHoles( const Identifier& chid, const Trk::TrackParameters& pars, const MeasVec& measurements ) const;

    
    ServiceHandle<MuonStationIntersectSvc> m_intersectSvc;     //<! pointer to hole search service
    ToolHandle<Trk::IPropagator>        m_propagator;       //<! propagator
    ToolHandle<MuonIdHelperTool>        m_idHelperTool;     //<! tool to assist with Identifiers
    ToolHandle<MuonEDMHelperTool>       m_helperTool;       //<! multipurpose helper tool
    ToolHandle<MuonEDMPrinterTool>      m_printer;          //<! tool to print out EDM objects

  };


}


#endif
