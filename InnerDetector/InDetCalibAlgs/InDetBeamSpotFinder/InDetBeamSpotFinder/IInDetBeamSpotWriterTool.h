/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_IINDETBEAMPSPOTWRITERTOOL_H
#define INDET_IINDETBEAMPSPOTWRITERTOOL_H
////////////////////////////////////////////////////
// Author James Walder : jwalder@cern.ch
// AlgTool to handle main Beamspot writing functions
//
// Principle expectation is a croncrete implementation
// to write to a database
////////////////////////////////////////////////////

#include "GaudiKernel/IAlgTool.h"
#include <vector>
#include "InDetBeamSpotFinder/IInDetBeamSpotTool.h"
// #include "BeamSpotID.h"

static const InterfaceID IID_IInDetBeamSpotWriterTool("IInDetBeamSpotWriterTool", 1 , 0); 

namespace BeamSpot {
  /** A simple struct to hold vertex info. Defined to have the minimal information
   required in the database. Not designed to be needed externally.*/
  struct BeamSpotDB{
    BeamSpotDB():isSuccessfulFit(false),status(0),
		 posX(0.),posY(0.),posZ(0.),
		 sigmaX(0.),sigmaY(0.),sigmaZ(0.),
		 tiltX(0.),tiltY(0.),sigmaXY(0.),
		 posXErr(0.),posYErr(0.),posZErr(0.),
		 sigmaXErr(0.),sigmaYErr(0.),sigmaZErr(0.),
		 tiltXErr(0.),tiltYErr(0.),sigmaXYErr(0.)
    {}
    bool isSuccessfulFit;
    int status;
    float posX,posY,posZ,sigmaX,sigmaY,sigmaZ,tiltX,tiltY,sigmaXY;
    float posXErr,posYErr,posZErr,sigmaXErr,sigmaYErr,sigmaZErr,tiltXErr,tiltYErr,sigmaXYErr;
  };
  
class ID;
}


namespace InDet {
  /** Abstract class to control the outputing of beamspot information.
   */
  class IInDetBeamSpotWriterTool : virtual public IAlgTool {
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() {
      return IID_IInDetBeamSpotWriterTool; 
    }
    // Actual operator function 
    
    //General methods for all beamspot determination methods
    virtual StatusCode initialize() = 0;
    virtual StatusCode finalize()   = 0;
    
    /** Principle method to output the beamspot information.
	A map of beamspots is taken as input, to be output in the method defined by the
	concrete implementation of the class (probably a database of some description).
    */
    virtual StatusCode write(std::map<BeamSpot::ID, BeamSpot::BeamSpotDB> &) = 0;

    virtual void setLBfromAcceptedEvents(bool ) = 0;
    virtual void setLBfromViewedEvents(bool ) = 0; 


  };


} // namespace
#endif


