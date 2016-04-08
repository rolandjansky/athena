/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// McEnergyLossUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////


#ifndef ISF_Fatras_McEnergyLossUpdator_H
#define ISF_Fatras_McEnergyLossUpdator_H

// GaudiKernel & Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

// Trk
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

namespace Trk{
  class MaterialProperties;
  class EnergyLoss;
}

namespace iFatras{

      
  /** @class McEnergyLossUpdator
  
      Updator for a eloss of a track on a Trk::Layer, 
      it extends the IEnergyLossUpdtor interface  
      
      @author Tom.Atkinson@cern.ch, Andreas.Salzburger@cern.ch
   */

 class McEnergyLossUpdator : public AthAlgTool, 
                             virtual public Trk::IEnergyLossUpdator {

  public:

    /** Constructor with AlgTool parameters */
    McEnergyLossUpdator( const std::string&, const std::string&, const IInterface* );

    /** Destructor */
    ~McEnergyLossUpdator();

    /** AlgTool initialise method */
    StatusCode initialize();

    /** AlgTool finalise method */
    StatusCode finalize();

    /** IEnergyLossUpdator public method to compute dEdX */
    double dEdX( const Trk::MaterialProperties& materialProperties,
	               double momentum,
	               Trk::ParticleHypothesis particleHypothesis = Trk::pion ) const override;

    /** IEnergyLossUpdator public method to compute the mean and variance of the energy loss */
    Trk::EnergyLoss* energyLoss( const Trk::MaterialProperties& materialProperties,
                                 double momentum,
                                 double pathCorrection,
                                 Trk::PropDirection direction = Trk::alongMomentum,
                                 Trk::ParticleHypothesis particleHypothesis = Trk::pion,
                                 bool mpv=true, bool usePDGformula = false) const override;

    /** Dummy methodes imposed by public interface - cleanup */
    /** Method to recalculate Eloss values for the fit setting an elossFlag using as an input
        the detailed Eloss information Calorimeter energy, error momentum and momentum error */ 
    Trk::EnergyLoss* updateEnergyLoss( Trk::EnergyLoss*, double, double, double, double, int&) const override { return 0; }
  
    /** Routine to calculate X0 and Eloss scale factors for the Calorimeter and Muon System */
    void getX0ElossScales(int, double, double, double&, double& ) const override {}
    /** Dummy methods end here */

  private:
    
    ToolHandle<IEnergyLossUpdator> m_energyLossUpdator;            //!< Pointer to the energy loss updator
   int                            m_energyLossDistribution;       //!< include energy loss straggling or not ( 0 == none, 1 == gauss, 2 == landau)

   /** Random Generator service  */
   ServiceHandle<IAtRndmGenSvc>                 m_rndGenSvc;
   /** Random engine  */
   CLHEP::HepRandomEngine*                      m_randomEngine;
   std::string                                  m_randomEngineName;         //!< Name of the random number stream
   bool                                         m_usePDGformula;

   static Trk::ParticleMasses    s_particleMasses;         //!< struct of Particle masses                                                        


};

} // end iFatras namespace

#endif
