/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			McBetheHeitlerEnergyLossUpdator.h  -  description
			-------------------------------------------------
begin                : Wednesday 28th June 2006
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for updating energy loss based on the Bethe-Heitler
		       distribution
*********************************************************************************/

#ifndef ISF_Fatras_McBetheHeitlerEnergyLossUpdator_H
#define ISF_Fatras_McBetheHeitlerEnergyLossUpdator_H

// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
// Trk
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkEventPrimitives/PropDirection.h"

namespace Trk{
  class MaterialProperties;
  class EnergyLoss;
}

namespace iFatras{
	
class McBetheHeitlerEnergyLossUpdator : public AthAlgTool, virtual public Trk::IEnergyLossUpdator {
   
 public:	
	
  /** Constructor with AlgTool parameters */
  McBetheHeitlerEnergyLossUpdator( const std::string&, const std::string&, const IInterface* );
  
  /** Destructor */
  ~McBetheHeitlerEnergyLossUpdator();
  
  /** AlgTool initialise method */
  StatusCode initialize();
  
  /** AlgTool finalize method */
  StatusCode finalize();
  
  /** dEdx method - not used */
  double dEdX( const Trk::MaterialProperties& materialProperties, 
	       double momentum, 
	       Trk::ParticleHypothesis particleHypothesis = Trk::electron ) const;
  
  /** Method to determine the change in q/p and the variance */
  Trk::EnergyLoss* energyLoss( const Trk::MaterialProperties& materialProperties,
                               double momentum,
                               double pathCorrection,
                               Trk::PropDirection direction = Trk::alongMomentum,
                               Trk::ParticleHypothesis particleHypothesis = Trk::electron,
                               bool mpv = false) const;

  /** Method to recalculate Eloss values for the fit setting an elossFlag using as an input
      the detailed Eloss information Calorimeter energy, error momentum and momentum error */
  
  Trk::EnergyLoss* updateEnergyLoss( Trk::EnergyLoss*, double, double, double, double, int&) const override { return 0; }
  
  /** Routine to calculate X0 and Eloss scale factors for the Calorimeter and Muon System */
  void getX0ElossScales(int, double, double, double&, double& ) const override {}
  
 private:
  
  /** Private method to compute the Bethe-Heitler PDF */
  std::vector<double> betheHeitlerPDF( double pathLength ) const;
  
  ServiceHandle<IAtRndmGenSvc> m_rndmGenSvc;       //!< Random number generator
  CLHEP::HepRandomEngine*             m_randomEngine;
  std::string                  m_randomEngineName; //!< Name of the random number stream

  mutable std::vector<double>          m_pdf;
  mutable double                       m_cashedPathLength;
  int                                  m_numberPointsInPDF;

  double                               m_scaleFactor;    //!< the one free parameter to scale
     
  static Trk::ParticleMasses    s_particleMasses;         //!< struct of Particle masses   

};
 
} // end iFatras namespace

inline double iFatras::McBetheHeitlerEnergyLossUpdator::dEdX( const Trk::MaterialProperties&, double, Trk::ParticleHypothesis ) const
{ return 0.; }

#endif
