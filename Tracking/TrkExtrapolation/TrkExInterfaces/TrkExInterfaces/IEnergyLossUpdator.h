/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IEnergyLossUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXINTERFACES_IENERGYLOSSUPDATOR_H
#define TRKEXINTERFACES_IENERGYLOSSUPDATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
// STL
#include <utility>

namespace Trk {

  class MaterialProperties;
  class EnergyLoss;

  /** Interface ID for IEnergyLossUpdator*/  
  static const InterfaceID IID_IEnergyLossUpdator("IEnergyLossUpdator", 1, 0);
  
  /**@class IEnergyLossUpdator
     
     Interface class IEnergyLossUpdator
     
     @author Andreas.Salzburger@cern.ch
    */
  class IEnergyLossUpdator : virtual public IAlgTool {
      
     public:
       /**Virtual destructor*/
       virtual ~IEnergyLossUpdator(){}
       
       /** AlgTool and IAlgTool interface methods */
       static const InterfaceID& interfaceID() { return IID_IEnergyLossUpdator; }
       
      /** dEdX calculation when providing MaterialProperties,
        a momentum, a pathlength, and a ParicleHypothesis:
        
        Units: [Mev/mm]
        */
      virtual double dEdX(const MaterialProperties& mat,
                          double p,
                          ParticleHypothesis particle=pion) const = 0;
      /** deltaE calculation
        using dEdX and integrating along pathlength,
        assuming constant dEdX during for the path.
        The sign depends on the given propagation direction 
        
        mpv steers the most probable energy loss

        Units: [MeV]
        */
      virtual EnergyLoss* energyLoss(const MaterialProperties& mat,
				                     double p,
				                     double pathcorrection,
				                     PropDirection dir=alongMomentum,
				                     ParticleHypothesis particle=pion,
				                     bool mpv = false,
				                     bool usePDGformula = false) const = 0;  

    /** Method to recalculate Eloss values for the fit setting an elossFlag using as an input
          the detailed Eloss information Calorimeter energy, error momentum and momentum error */

     virtual EnergyLoss* updateEnergyLoss(EnergyLoss* eLoss, double caloEnergy, double caloEnergyError,
                          double pCaloEntry, double momentumError, int & elossFlag) const = 0;

     /** Routine to calculate X0 and Eloss scale factors for the Calorimeter and Muon System */
     virtual void getX0ElossScales(int icalo, double eta, double phi, double & X0Scale, double & ElossScale ) const = 0;


     /**
      Calculate dg/dlambda, for non-electrons (g=dEdX and lambda=q/p).
      This is used by the analytical error propagation in STEP
      */
      double dgdlambda(double,
                       const MaterialProperties&,
                       ParticleHypothesis) const { return 0; }

       /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
       virtual void validationAction() const {}

  };

} // end of namespace

#endif // TRKEXINTERFACES_IENERGYLOSSUPDATOR_H


