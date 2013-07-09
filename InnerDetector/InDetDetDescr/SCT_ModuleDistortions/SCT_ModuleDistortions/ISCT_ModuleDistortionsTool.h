/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_ModuleDistortionsTool_H
#define ISCT_ModuleDistortionsTool_H

#include "GaudiKernel/IAlgTool.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

namespace Trk { class LocalParameters; }
namespace InDetDD { class SiDetectorElement; }

static const InterfaceID IID_ISCT_ModuleDistortionsTool ("ISCT_ModuleDistortionsTool",1,0);
/// Based on IModuleDistortionsTool
/// Interface class to correct for module distortions 
/// Has two methods, one for simulation and one for reconstruction (since the correction is opposite)
/// Alternatively one can get just the delta.

class ISCT_ModuleDistortionsTool : virtual public IAlgTool 
{

public:
  
  virtual ~ISCT_ModuleDistortionsTool() {}
  
  static const InterfaceID& interfaceID() 
    {return IID_ISCT_ModuleDistortionsTool;}   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// Find Z shift for x,y hit
  virtual float zShift( const double xhit, const double yhit, const std::vector<float>* ZVec ) const = 0;
  /// Identify assembly region
  virtual int identifyRegion(IdentifierHash id) const =0;
  /// Load distortions
  virtual const std::vector<float>* readDistortions(int RegionID, int Side) const = 0;
  /// Correct the rescontruction position 
  virtual double correctReconstruction(const Trk::TrackParameters & trackPar, const InDetDD::SiDetectorElement & EL, Trk::LocalParameters & locpar, const Amg::Vector2D & loct) const = 0; 
  /// Correct the simulation position 
  virtual Amg::Vector2D correctSimulation(IdentifierHash id, double xhit, double yhit, double cEta, double cPhi, double cDep ) const = 0; 
  /// Get just the correction
  virtual Amg::Vector2D correction(IdentifierHash id, const Amg::Vector2D & locpos, const Amg::Vector3D & direction) const = 0;   
  /// Call back to load distortions from database
  //virtual StatusCode fillDataCallBack(IOVSVC_CALLBACK_ARGS) = 0;


};



#endif //ISCT_ModuleDistortionsTool_H
