/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossSamplerBetheHeitler.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASTOOLS_ENERGYLOSSSAMPLERBETHEHEITLER_H
#define ISF_FATRASTOOLS_ENERGYLOSSSAMPLERBETHEHEITLER_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Trk
#include "ISF_FatrasInterfaces/IEnergyLossSampler.h"
#include "TrkEventPrimitives/PropDirection.h"

namespace Trk{
  class MaterialProperties;
  class EnergyLoss;
}

namespace iFatras{

  /** @class EnergyLossSamplerBetheHeitler

      Sampler for the Eloss of a track
      It extends the IEnetgyLossSampler interface      
      
      @author Noemi.Calace@cern.ch, Andreas.Salzburger@cern.ch
   */

  class EnergyLossSamplerBetheHeitler : public extends<AthAlgTool, iFatras::IEnergyLossSampler> {

 public:
    
    /** Constructor with AlgTool parameters */
    EnergyLossSamplerBetheHeitler( const std::string&, const std::string&, const IInterface* );
    
    /** Destructor */
    virtual ~EnergyLossSamplerBetheHeitler();
    
    /** AlgTool initialise method */
    virtual StatusCode initialize() override;
    
    /** AlgTool finalise method */
    virtual StatusCode finalize() override;
    
    /** IEnergyLossSampler public method to compute dEdX */
    virtual
    double dEdX( const Trk::MaterialProperties& materialProperties,
		 double momentum,
		 Trk::ParticleHypothesis particleHypothesis = Trk::pion ) const override;
    
    /** IEnergyLossSampler public method to compute the mean and variance of the energy loss */
    virtual
    Trk::EnergyLoss* energyLoss( const Trk::MaterialProperties& materialProperties,
                                 double momentum,
                                 double pathCorrection,
                                 Trk::PropDirection direction = Trk::alongMomentum,
                                 Trk::ParticleHypothesis particleHypothesis = Trk::pion,
                                 bool mpv=true) const override;
              
  private:
    
    /** Private method to compute the Bethe-Heitler PDF */
  std::vector<double> betheHeitlerPDF( double pathLength ) const;
  
  /** Random number generator */
  ServiceHandle<IAtRndmGenSvc>         m_rndmGenSvc;       
  /** Random engine  */
  CLHEP::HepRandomEngine*              m_randomEngine;
  /** Name of the random number stream */
  std::string                          m_randomEngineName; 

  mutable std::vector<double>          m_pdf;

  /** the one free parameter to scale */
  double                               m_scaleFactor;   
    
        
};

} // end iFatras namespace

inline double iFatras::EnergyLossSamplerBetheHeitler::dEdX( const Trk::MaterialProperties&, double, Trk::ParticleHypothesis ) const
{ return 0.; }

#endif //  ISF_FATRASTOOLS_ENERGYLOSSSAMPLERBETHEHEITLER_H

    
