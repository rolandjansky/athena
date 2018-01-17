/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringSamplerGaussianMixture.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGAUSSIANMIXTURE_H
#define ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGAUSSIANMIXTURE_H
 
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "ISF_FatrasInterfaces/IMultipleScatteringSampler.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CLHEP/Random/RandFlat.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"

namespace Trk {
  class MaterialProperties;
}

namespace iFatras {
  
  /**@class MultipleScatteringSamplerGaussianMixture
     
     @author Noemi.Calace@cern.ch , Andreas.Salzburger@cern.ch
  */
  
  class MultipleScatteringSamplerGaussianMixture : public extends<AthAlgTool, IMultipleScatteringSampler> {
      
  public:
      /** AlgTool like constructor */
      MultipleScatteringSamplerGaussianMixture(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~MultipleScatteringSamplerGaussianMixture();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Calculate the sigma on theta introduced by multiple scattering,
	  according to the RutherFord-Scott Formula           
      */
      double simTheta(const Trk::MaterialProperties& mat,
		      double p,
		      double pathcorrection,
		      Trk::ParticleHypothesis particle=Trk::pion,
		      double deltaE=0.) const;
      
  private:
      bool                  m_log_include;           //!< boolean switch to include log term  
      
      bool                  m_optGaussianMixtureG4;  //!< modifies the Fruehwirth/Regler model to fit with G4
      
      //========== used for Gaussian mixture model =================================================
      
      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      /** Random engine  */
      CLHEP::HepRandomEngine*                      m_randomEngine;
      std::string                                  m_randomEngineName;                   //!< Name of the random number stream
       
      static Trk::ParticleMasses s_particleMasses;        //!< struct of Particle masses
      
      static double         s_main_RutherfordScott;  //!< main factor of Rutherford-Scott formula
      static double         s_log_RutherfordScott;   //!< log factor of Rutherford-Scott formula
       
      static double         s_main_RossiGreisen;     //!< main factor for Rossi-Greisen formula
      static double         s_log_RossiGreisen;      //!< main factor for Rossi-Greisen formula
      
      
      // ========= Gaussian mixture model Fruehwirth, Regler Nucl. Inst. Methods A 456(2001) =========
      static double         s_gausMixSigma1_a0;     //!< Gaussian mixture model: Sigma parameter a0
      static double         s_gausMixSigma1_a1;     //!< Gaussian mixture model: Sigma parameter a1
      static double         s_gausMixSigma1_a2;     //!< Gaussian mixture model: Sigma parameter a2
      
      static double         s_gausMixEpsilon_a0;     //!< Gaussian mixture model: Epsilon parameter a0
      static double         s_gausMixEpsilon_a1;     //!< Gaussian mixture model: Epsilon parameter a1
      static double         s_gausMixEpsilon_a2;     //!< Gaussian mixture model: Epsilon parameter a2
      
      static double         s_gausMixEpsilon_b0;     //!< Gaussian mixture model: Epsilon parameter b0
      static double         s_gausMixEpsilon_b1;     //!< Gaussian mixture model: Epsilon parameter b1
      static double         s_gausMixEpsilon_b2;     //!< Gaussian mixture model: Epsilon parameter b2
       
      static double 	    s_projectionFactor;	     //!< projection factor to scale the projected angle out of the plane
      
    };
  
  
} // end of namespace


#endif // ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGAUSSIANMIXTURE_H
