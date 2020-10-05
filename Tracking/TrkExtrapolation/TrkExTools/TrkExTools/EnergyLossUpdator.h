/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossUpdator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKEXTOOLS_ENERGYLOSSUPDATOR_H
#define TRKEXTOOLS_ENERGYLOSSUPDATOR_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
// Trk
#include "TrkExInterfaces/IEnergyLossUpdator.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkExUtils/MaterialInteraction.h"
// STL
#include <utility>

namespace Trk {

  class MaterialProperties;
  class EnergyLoss;
  
  /**@class EnergyLossUpdator

    The mean Energy loss per unit length of a particle is calculated as
    
    @f$ (dE/dx) = (dE/dX)_{io} + (dE/dX)_{rad} @f$
    
    Formulas used:
      - <b>For the ionization :</b>
        Bethe-Bloch for minimum ionizing particles (modified for electrons)
        
        @f$ (dE/dX)_{io,mip} = K \cdot \frac{Z}{A} \frac{1/\beta^2} \cdot [\frac{1}{2}]
        \cdot \ln \frac{2 m_e c^2 \beta^2 \gamma^2 T_max}{I^2} - \beta^2] @f$, (1)
        
        respectively
        
        @f$ (dE/dX)_{io,e}  = \frac{1/2} \cdot K \cdot \frac{Z}{A} 
        [2\ln\frac{2m_e c^2}{I} + 3 \ln{\gamma} - 1.95] @f$ (2)
    
        Alternatively, the most probable value instead of the mean value can be taken (steared by boolean in
        the method signature), taken from (4)

         @f$ \_{L}\Delta_p = \xi \left[ln\frac{2mc^2\beta^2\gamma^2}{I} + ln\frac{\xi}{I} - 0.8 + 4.447 \right] @f$

        with @f$ \xi = Z N_a \frac{k}{\beta^2}\codt t @f$

        The with of both distributions is given as the (crude) approximation, 
        that the full width at half maximum of the landau distribution is :  @f$ 4 \xi @f$

        For the transformation into a parameterized sigma the factor @f$ 2\cdot\sqrt{2.\cdot ln(2.) } @f$ can be used


        Sigma can be multiplied by a form factor (given through job options)
    
     - <b>For radiation corrections: </b>

        The Bethe-Heitler parameterisation is in terms of

	     @f$ Z = \frac{E_f}{E_i} @f$ (2)

	     With this parameterisation the expected energy loss

      @f$ \langle Z \rangle = e^{-t} @f$ (2)

	    where @f$ t = \frac{x}{X_0} @f$ or the fraction of a radiation length of material traversed by the particle.

	    This formula can be transposed in terms of @f$ \Delta (E) @f$. The transposition however depends on the direction of propagation.

	    For forwards propagation we get: @f$ \langle \Delta (E) \rangle = E_i ( 1 - \langle Z \rangle ) @f$ (2)

	    For the propagation against momentum we get: @f$ \langle \Delta (E) \rangle = E_i ( \frac{1}{\langle Z \rangle} - 1 ) @f$ (2)

    	The variance of Z is also defined (2): @f$ Var(Z) = e^{-t ln(3) / ln(2)} - e^{-2t} @f$

	    Transposing this one dervies the following relations for 

	    1. Forwards propagation @f$ Var ( \Delta \frac{q}{p} ) = \frac{Var(Z)}{\langle Z \rangle ^2 p^2} @f$

    	2. Backwards propagation @f$ Var ( \Delta \frac{q}{p} ) = \frac{Var(Z)}{p^2} @f$

    	Where p is the global momentum.

        Bethe-Heitler formula
        
        @f$ (dE/dX)_{rad} = \frac{E_{initial}}{X_o}\cdot (\frac{m}{m_e})^2 @f$ (3)
        
      References:
      (1)      Review of Particle Physics, ELSEVIR, ISSN 0370-2639 (2004)
      (2),(3)  D. Stampfer et al., Track fitting with energy loss, Comp. Phys. Comm 79 (1994) 
      (4)      H. Bichsel, Straggling in thin silicon detectors, Rev. Mod. Phys., Vol. 60., No. 3, July 1988
        
             
     @author Andreas.Salzburger@cern.ch, Tom.Atkinson@cern.ch
    */
    
  class EnergyLossUpdator : public AthAlgTool,
                            virtual public IEnergyLossUpdator {
      
    public:
      /** AlgTool like constructor */
      EnergyLossUpdator(const std::string&,const std::string&,const IInterface*);

      /**Virtual destructor*/
      virtual ~EnergyLossUpdator() = default;
       
      /** dEdX calculation when providing MaterialProperties,
        a momentum, a pathlength, and a ParicleHypothesis:
        
        Units: [MeV/mm]
        */
      virtual double dEdX(const MaterialProperties& mat,
                          double p,
                          ParticleHypothesis particle=pion) const override;
                  
      /** deltaE calculation
        using dEdX and integrating along pathlength,
        assuming constant dEdX during for the path.
        The sign depends on the given propagation direction 
        
        Units: [MeV]

        mpv steers the most probable energy loss
        */
     virtual EnergyLoss* energyLoss(
			    const MaterialProperties& mat,
			    double p,
			    double pathcorrection,
			    PropDirection dir=alongMomentum,
			    ParticleHypothesis particle=pion,
			    bool mpv = false,
			    bool usePDGformula = false) const override;   
    

     /** Method to recalculate Eloss values for the fit setting an elossFlag using as an input
         the detailed Eloss information Calorimeter energy, error momentum and momentum error */
     virtual EnergyLoss* updateEnergyLoss(EnergyLoss* eLoss, double caloEnergy, double caloEnergyError, 
                          double pCaloEntry, double momentumError, int & elossFlag) const override;

     /** Routine to calculate X0 and Eloss scale factors for the Calorimeter and Muon System */
     virtual void getX0ElossScales(int icalo, double eta, double phi, double & X0Scale, double & ElossScale ) const override;


    private:
      /** Method to return the variance of the change in q/p for the Bethe-Heitler parameterisation */
      double varianceDeltaQoverP(const MaterialProperties&,
                                 double p,
                                 double pathcorrection,
                                 PropDirection direction = alongMomentum,
                                 ParticleHypothesis particleHypothesis = electron ) const ;


      /** dEdX BetheBloch calculation: 
        Units: [MeV]
        */
      double dEdXBetheBloch(const MaterialProperties& mat,
                            double& transKaz,
                            double& transTmax,
                            double beta,
                            double gamma,
                            ParticleHypothesis particle=pion) const;
      
      Trk::EnergyLoss* ionizationEnergyLoss(const MaterialProperties& mat,
					    double p,
					    double pathcorrection,
					    PropDirection dir=alongMomentum,
					    ParticleHypothesis particle = pion) const;
      
      /** dEdX BetheHeitler calculation: 
        Units: [MeV]
        */
      double dEdXBetheHeitler(const MaterialProperties& mat,
                              double initialE,
                              ParticleHypothesis particle=pion) const;

      Trk::MaterialInteraction       m_matInt;
      double                  m_stragglingErrorScale;      //!< stragglingErrorScale
      double                  m_mpvScale;                  //!< a scalor that can be introduced for the MPV 
      bool                    m_useTrkUtils;               //!< use eloss parametrisation from TrkUtils MaterialInterAction.h
      bool                    m_gaussianVavilovTheory;     //!< include energy loss straggling or not
      bool                    m_useBetheBlochForElectrons; //!< use adopted bethe bloch for electrons 
      bool                    m_mpvSigmaParametric;        //!< take the (crude) parametric mpv sigma 
      bool                    m_detailedEloss;             //!< provide extended EnergyLoss info 
      bool                    m_optimalRadiation;          //!< use calorimeter more optimal for radiation detection
  };

} // end of namespace


#endif // TRKEXTOOLS_ENERGYLOSSUPDATOR_H


