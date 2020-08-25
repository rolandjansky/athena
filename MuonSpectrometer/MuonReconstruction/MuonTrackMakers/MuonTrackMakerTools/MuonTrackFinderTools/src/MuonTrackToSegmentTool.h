/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#ifndef MUON_MUONTRACKTOSEGMENTTOOL_H
#define MUON_MUONTRACKTOSEGMENTTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"

#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"
#include "MuonCondData/MdtCondDbData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkExInterfaces/IPropagator.h"
#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"

#include "TrkParameters/TrackParameters.h"

#include <vector>

namespace Trk {
  class Track;
  class MeasurementBase;
}

namespace Muon {
  class MuonSegment;
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
    ~MuonTrackToSegmentTool()=default;
    
    /** initialize method, method taken from bass-class AlgTool */
    StatusCode initialize();
    
    /** @brief convert track to segment */
    MuonSegment* convert( const Trk::Track& track ) const;

  private:
    /** @brief calculate holes */
    std::vector<Identifier> calculateHoles( const Identifier& chid, const Trk::TrackParameters& pars, const MeasVec& measurements ) const;

    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey {this, "DetectorManagerKey", 
	"MuonDetectorManager", 
	"Key of input MuonDetectorManager condition data"};    

    ServiceHandle<MuonStationIntersectSvc> m_intersectSvc {this, "MuonStationIntersectSvc", "MuonStationIntersectSvc", "pointer to hole search service"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
    ServiceHandle<IMuonEDMHelperSvc>    m_edmHelperSvc {this, "edmHelper", 
      "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
      "Handle to the service providing the IMuonEDMHelperSvc interface" };       //<! multipurpose helper tool
    
    ToolHandle<MuonEDMPrinterTool> m_printer {this, "EDMPrinter", "Muon::MuonEDMPrinterTool/MuonEDMPrinterTool", "helper to nicely print out tracks"};
    ToolHandle<Trk::IPropagator> m_propagator {this, "Propagator", "Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"};

    SG::ReadCondHandleKey<MdtCondDbData> m_condKey{this, "MdtCondKey", "MdtCondDbData", "Key of MdtCondDbData"};

  };


}


#endif
