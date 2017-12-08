/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPhysicsValidationTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IPHYSICSVALIDATIONTOOL_H
#define ISF_FATRASINTERFACES_IPHYSICSVALIDATIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// ISF
#include "ISF_Event/ISFParticleVector.h"
// Trk
#include "TrkParameters/TrackParameters.h"
#include "TrkNeutralParameters/NeutralParameters.h"
#include "TrkExUtils/ExtrapolationCell.h"

//namespace ISF {
//    class ISFParticle;
//    class ISFParticleVector;
//}

namespace iFatras {
  
  static const InterfaceID IID_IPhysicsValidationTool("IPhysicsValidationTool", 1, 0);
  
  /** 
      @class IPhysicsValidationTool
      
      physics validation ntuple
      
      @author Sarka.Todorova -at- cern.ch
      
  */
      
  class IPhysicsValidationTool : virtual public IAlgTool {
  public:
    
    /** Virtual destructor */
    virtual ~IPhysicsValidationTool(){}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IPhysicsValidationTool; }
    
    /** ISFParticle info: old transport tool */
    virtual void saveISFParticleInfo(const ISF::ISFParticle& isp, int endProcess, const Trk::TrackParameters* ePar, double time, double dX0 ) const = 0;

    /** ISFParticle info: new transport tool */
    virtual void saveISFParticleInfo(const ISF::ISFParticle& isp, const Trk::ExtrapolationCell<Trk::TrackParameters>& ec,
				     Trk::ExtrapolationCode ecode ) const = 0;

    /** ISFParticle info: new transport tool */
    virtual void saveISFParticleInfo(const ISF::ISFParticle& isp, const Trk::ExtrapolationCell<Trk::NeutralParameters>& ec,
				     Trk::ExtrapolationCode ecode ) const = 0;

    /** Interaction vertex info */
    virtual void saveISFVertexInfo(int process, Amg::Vector3D vertex,const ISF::ISFParticle& isp, Amg::Vector3D primIn,
				   Amg::Vector3D* primOut, const ISF::ISFParticleVector children) const = 0;
  };
  
} // end of namespace

#endif 

