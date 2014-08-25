/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IModuleDistortionsTool_H
#define IModuleDistortionsTool_H


#include "GaudiKernel/IAlgTool.h"
#include "AthenaKernel/IOVSvcDefs.h"

//Amg (Eigen) vectors
#include "GeoPrimitives/GeoPrimitives.h"

//forward declarations
class Identifier;

static const InterfaceID IID_IModuleDistortionsTool ("IModuleDistortionsTool",1,0);

/// Interface class to correct for module distortions 
/// Has two methods, one for simulation and one for reconstruction (since the correction is opposite)
/// Alternatively one can get just the delta.

class IModuleDistortionsTool : virtual public IAlgTool 
{

public:
  
  virtual ~IModuleDistortionsTool() {}
  
  static const InterfaceID& interfaceID() 
    {return IID_IModuleDistortionsTool;}   /// The AlgTool InterfaceID
  virtual StatusCode initialize()=0;     /// standard AlgTool method
  virtual StatusCode finalize()=0;       /// standard AlgTool method

  /// abstract base method to correct module distortions.
  

  /// Correct the rescontruction position 
  virtual Amg::Vector2D correctReconstruction(Identifier id,
						    const Amg::Vector2D & locpos, 
						    const Amg::Vector3D & direction) const = 0; 
  /// Correct the simulation position 
  virtual Amg::Vector2D correctSimulation(Identifier id, 
					       const Amg::Vector2D & locpos, 
					       const Amg::Vector3D & direction) const = 0; 

  /// Get just the correction. Add to reconstruction position or subtract from the simulation position. 
  virtual Amg::Vector2D correction(Identifier id, 
				const Amg::Vector2D & locpos, 
				const Amg::Vector3D & direction) const = 0; 
  
  /// Call back to load distortions from database
  virtual StatusCode fillDataCallBack(IOVSVC_CALLBACK_ARGS) = 0;


};

#endif //IModuleDistortionsTool_H
