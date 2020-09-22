/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MultipleScatteringUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_MULTIPLESCATTERINGUPDATOR_H
#define TRKEXTOOLS_MULTIPLESCATTERINGUPDATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkExInterfaces/IMultipleScatteringUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "CLHEP/Random/RandFlat.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "EventPrimitives/EventPrimitives.h"


namespace Trk {

  class MaterialProperties;
  
  /**@class MultipleScatteringUpdator
    
    The Formula used is the highland formula for the projected scattering angle :

      @f$ \theta_{ms} = \frac{13.6MeV}{p}\cdot\sqrt{t/X_{0}}[1 + 0.038\ln(t/X_{0})] @f$
      
      What is returned is the square of the expectation value of the deflection 
      @f$ < (\theta_ms)^2 > = \sigma_ms^2 @f$
     
      - For electrons the 
     @author Andreas.Salzburger@cern.ch
    */
  class MultipleScatteringUpdator : public AthAlgTool,
                                    virtual public IMultipleScatteringUpdator {
      
    public:
      /** AlgTool like constructor */
      MultipleScatteringUpdator(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~MultipleScatteringUpdator();
       
      /** AlgTool initailize method.*/
      virtual StatusCode initialize() override;
      
      /** Calculate the sigma on theta introduced by multiple scattering,
          according to the RutherFord-Scott Formula           
      */
      virtual double sigmaSquare(const MaterialProperties& mat,
                         double p,
                         double pathcorrection,
                         ParticleHypothesis particle=pion,
                         double deltaE=0.) const override;
    
    private:

      bool                  m_useTrkUtils;           //!< use eloss parametrisation from TrkUtils MaterialInterAction.h
      bool                  m_log_include;           //!< boolean switch to include log term  
      bool                  m_gaussianMixture;       //!< mainly for Fatras
      bool                  m_optGaussianMixtureG4;  //!< modifies the Fruehwirth/Regler model to fit with G4

      //========== used for Gaussian mixture model =================================================

      /** Random Generator service  */
      ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
      /** Random engine  */
      CLHEP::HepRandomEngine*                      m_randomEngine;
      std::string                                  m_randomEngineName;                   //!< Name of the random number stream
                                                      
  };


} // end of namespace


#endif // TRKEXTOOLS_MULTIPLESCATTERING_H


