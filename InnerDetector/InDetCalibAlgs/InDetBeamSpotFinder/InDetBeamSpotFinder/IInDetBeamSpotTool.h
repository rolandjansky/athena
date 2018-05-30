/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_IINDETBEAMSPOTTOOL_H
#define INDET_IINDETBEAMSPOTTOOL_H

///////////////////////////////////////////////////////////
// Beamspot Abstract base class for beamspot determination
// Authors btamadio@lbl.gov, jwalder@cern.ch
// Beamspot fit tools are derived from IInDetBeamSpotTool
////////////////////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include "xAODTracking/TrackingPrimitives.h" //for xAOD::VxType
#include <vector>
#include <map>
#include <string>
static const InterfaceID IID_IInDetBeamSpotTool("IInDetBeamSpotTool", 1 , 0); 

namespace BeamSpot {
  struct VrtHolder {
  VrtHolder():x(0.),y(0.),z(0.),vxx(0.),vxy(0.), vyy(0.), vzz(0.), vertexType(xAOD::VxType::NotSpecified),nTracks(0),passed(false),valid(false){}
    double x,y,z;  
    double vxx, vxy, vyy, vzz; 
    xAOD::VxType::VertexType vertexType;
    unsigned int nTracks;
    bool passed, valid;
  };
}

namespace BeamSpot{
  struct Event {
    unsigned int pileup, runNumber, lumiBlock, bcid;
    unsigned long long eventNumber, eventTime, eventTime_NS; 
    std::vector<BeamSpot::VrtHolder> vertices;
  };
}

namespace InDet {
  /** @class IInDetBeamSpotTool
      Abstract class for all beamspot determination algorithms.
      Any class for finding the position of the beamspot needs to derive itself
      from this class. */ 
  class IInDetBeamSpotTool : virtual public IAlgTool {
  public:
    virtual ~IInDetBeamSpotTool(){}
    /** Internally used enum for fit status. Useful for quick checks.*/
    enum FitStatus {unsolved=0, problems, failed, successful};
    /** Beamspot determination type.*/
    enum FitID {trackChi2=1, trackChi2Widths=2, vertexLL=3, vertexChi2=4, RooFitLL = 6, unknown = 99};
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IInDetBeamSpotTool; }
    
    //General methods for all beamspot determination methods
    virtual StatusCode initialize() = 0; //!< Standard initialize
    virtual StatusCode finalize()   = 0; //!< Standard finalize. Will attempt a solution if not already done
    virtual FitStatus fit(std::vector< BeamSpot::VrtHolder >& ) = 0; //!< Attempt a to find a solution of the beamspot.
    virtual FitID getFitID() const = 0; //!< A unique ID for the specific fit type.

    virtual IInDetBeamSpotTool *Clone() = 0;
    virtual std::map<std::string,double> getCovMap() const  = 0;
    virtual std::map<std::string,double> getParamMap() const = 0;
  };
}
#endif

