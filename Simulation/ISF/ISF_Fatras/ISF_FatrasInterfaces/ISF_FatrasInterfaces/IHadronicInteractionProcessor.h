/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IHadronicInteractionProcessor.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_IHADRONICINTERACTIONPROCESSOR_H
#define ISF_FATRASINTERFACES_IHADRONICINTERACTIONPROCESSOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Event primitives
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
// ISF
#include "ISF_Event/ISFParticleVector.h"

namespace Trk {
  class Material;
  class MaterialProperties;
}

/*-------------------------------------------------------------------------
 *  Interface definition
 *-------------------------------------------------------------------------*/
namespace iFatras
{
  static const InterfaceID IID_IHadronicInteractionProcessor("IHadronicInteractionProcessor", 1, 0);
}

/*-------------------------------------------------------------------------
 *  class IHadronicInteractionProcessor
 *-------------------------------------------------------------------------*/
namespace iFatras
{
  /** 
      @class IHadronicInteractionProcessor
      
      Interface definition for the handling of nuclear/hadronic interactions,
      to be used by the MC based material effects updater
      
      @author Andreas.Salzburger@cern.ch
  */
    class IHadronicInteractionProcessor : virtual public IAlgTool {

  public:
    
    /** Virtual destructor */    virtual ~IHadronicInteractionProcessor(){}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_IHadronicInteractionProcessor; }
    
    /** interface for processing of the nuclear interactions */
    virtual bool hadronicInteraction(const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
				     double p, double E, double charge,
				     const Trk::MaterialProperties& mprop, double pathCorrection,
                                     Trk::ParticleHypothesis particle=Trk::pion) const = 0;

    /** interface for processing of the presampled nuclear interactions */
    virtual bool recordHadState(double time, double p, 
                                const Amg::Vector3D& vertex, 
				const Amg::Vector3D& momDir,
                                Trk::ParticleHypothesis particle=Trk::pion) const ;

    /** interface for processing of the presampled nuclear interactions */
    virtual bool doHadronicInteraction(double time, const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
				       const Trk::Material *ematprop,
				       Trk::ParticleHypothesis particle=Trk::pion,
				       bool  processSecondaries=true) const ;

    /** interface for processing of the presampled nuclear interactions on layer*/
    virtual ISF::ISFParticleVector doHadIntOnLayer(const ISF::ISFParticle* parent, double time, 
						   const Amg::Vector3D& position, const Amg::Vector3D& momentum, 
						   const Trk::Material *mat,
						   Trk::ParticleHypothesis particle=Trk::pion) const ;

  };

  inline bool IHadronicInteractionProcessor::recordHadState(double /*time*/, double /*p*/,
							    const Amg::Vector3D& /*vertex*/, 
							    const Amg::Vector3D& /*momDir*/,
							    Trk::ParticleHypothesis /*particle*/) const {
    return false;
  }

  inline bool IHadronicInteractionProcessor::doHadronicInteraction(double /*time*/, 
								   const Amg::Vector3D& /* position */, 
								   const Amg::Vector3D& /* momentum */, 
								   const Trk::Material* /*ematprop*/,
								   Trk::ParticleHypothesis /*particle*/,
								   bool  /*processSecondaries*/) const {
    return false;
  }

  inline ISF::ISFParticleVector IHadronicInteractionProcessor::doHadIntOnLayer(const ISF::ISFParticle* /*parent*/, double /*time*/,
									       const Amg::Vector3D& /* position */, 
									       const Amg::Vector3D& /* momentum */, 
									       const Trk::Material* /*matprop*/,
									       Trk::ParticleHypothesis /*particle=Trk::pion*/) const {
    ISF::ISFParticleVector vec(0); return vec;
  }

}

#endif // ISF_FATRASINTERFACES_IHADRONICINTERACTIONPROCESSOR_H

