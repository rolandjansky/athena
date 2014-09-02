/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_IINDETBEAMSPOTTOOL_H
#define INDET_IINDETBEAMSPOTTOOL_H

///////////////////////////////////////////////////////////
// Beamspot Abstract base class for beamspot determination
// Author jwalder@cern.ch
// All classes for beamspot determination should be derived from 
// this class.
////////////////////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include "CLHEP/Matrix/SymMatrix.h"
//#include "TrkEventPrimitives/VertexType.h"
#include "xAODTracking/TrackingPrimitives.h"

//Abstract class

static const InterfaceID IID_IInDetBeamSpotTool("IInDetBeamSpotTool", 1 , 0); 

namespace BeamSpot {
  /** A simple struct for holding vertex info.
   */
  struct VrtHolder {
    //simple container of vertex info
    VrtHolder():x(0.),y(0.),z(0.),vxx(0.), vxy(0.), vyy(0.),
	 vxz(0.),vyz(0.),vzz(0.),
      chi2(0.),ndf(0.), vertexType(xAOD::VxType::NotSpecified),valid(true)
	 ,run(0),evt(0),lb(0),tck(0),idx(0),outlierRemoved(false)
    {}
    double x,y,z;  //vertex position
    double vxx, vxy, vyy,vxz,vyz,vzz; //covariance parameters
    double chi2, ndf;
    xAOD::VxType::VertexType vertexType;
    bool valid;
    unsigned int  run,evt,lb,tck;
    int idx; // idx is an index of the pv position in the container
    bool outlierRemoved; // Was this vertex removed during outlier removal
  };
}

namespace InDet {
  /** @class IInDetBeamSpotTool
      Abstract class for all beamspot determination algorithms.
      Any class for finding the position of the beamspot needs to derive itself
      from this class. */ 
  class IInDetBeamSpotTool : virtual public IAlgTool {
  public:
    IInDetBeamSpotTool() {} //!< Default Constructor
    IInDetBeamSpotTool( const IInDetBeamSpotTool&  ){}//Copy constructor

    /** Internaly used enum for fit status. Useful for quick checks.*/
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
    virtual std::map<std::string,double> getCovMap() const = 0;
    virtual std::map<std::string,double> getParamMap() const = 0;
  };
}
#endif

