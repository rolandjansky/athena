/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringSamplerGeneralMixture.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGENERALMIXTURE_H
#define ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGENERALMIXTURE_H
 
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
  
  /**@class MultipleScatteringSamplerGeneralMixture
     
     @author Noemi.Calace@cern.ch , Andreas.Salzburger@cern.ch, Artem.Basalaev@cern.ch
  */
  
  class MultipleScatteringSamplerGeneralMixture : public AthAlgTool,
    virtual public IMultipleScatteringSampler {
      
  public:
      /** AlgTool like constructor */
      MultipleScatteringSamplerGeneralMixture(const std::string&,const std::string&,const IInterface*);
      
      /**Virtual destructor*/
      virtual ~MultipleScatteringSamplerGeneralMixture();
      
      /** AlgTool initailize method.*/
      StatusCode initialize();
      
      /** AlgTool finalize method */
      StatusCode finalize();
      
      /** Calculate theta introduced by multiple scattering,
	  according to the RutherFord-Scott Formula           
      */
      double simTheta(const Trk::MaterialProperties& mat,
			 double p,
			 double pathcorrection,
			 Trk::ParticleHypothesis particle=Trk::pion,
			 double deltaE=0.) const;
      
  private:
      bool                  m_log_include;           //!< boolean switch to include log term  
      static double 	    s_projectionFactor;	     //!< projection factor to scale the projected angle out of the plane
      
      //========== used for General mixture model =================================================
      
      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      /** Random engine  */
      CLHEP::HepRandomEngine*                      m_randomEngine;
      std::string                                  m_randomEngineName;                   //!< Name of the random number stream
       
      static Trk::ParticleMasses s_particleMasses;        //!< struct of Particle masses
       
      static double         s_main_RossiGreisen;     //!< main factor for Rossi-Greisen formula
      static double         s_log_RossiGreisen;      //!< main factor for Rossi-Greisen formula
      
      
      // ========= General mixture model R.Frühwirth, M. Liendl. Comp. Phys. Comm. 141 (2001) 230–246 =========
      static double	    s_genMixScale;	    //!< General mixture model: Scaling factor
      
      //!< General mixture model: get parameters for single gaussian simulation
      double 	*getGaussian(double beta, double p,double dOverX0, double scale) const;
      //!< General mixture model: get parameters for gaussian mixture
      double 	*getGaussmix(double beta, double p,double dOverX0,double Z, double scale) const;
      //!< General mixture model: get parameters for semi-gaussian mixture
      double 	*getSemigauss(double beta,double p,double dOverX0,double Z, double scale) const;
      //!< General mixture model: simulate semi-gaussian mixture
      double 	simGaussmix(double * scattering_params) const;
      //!< General mixture model: simulate gaussian mixture
      double 	simSemigauss(double * scattering_params) const;
    };
  
  
} // end of namespace


#endif // ISF_FATRASTOOLS_MULTIPLESCATTERINGSAMPLERGENERALMIXTURE_H
