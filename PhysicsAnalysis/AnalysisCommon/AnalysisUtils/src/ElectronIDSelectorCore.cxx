/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*****************************************************************************
 * @Project: AnalysisUtils
 *
 * @class ElectronIDSelectorCore
 *
 * @author Karsten Koeneke <karsten.koeneke@cernSPAMNOT.ch>
 *
 * Based on C implementation by:
 * @author John Alison <johnda@sas.upenn.edu>
 *
 * @date August 2010
 *
 * @brief Dual use tool (athena/ARA) for specialized electron identification cuts.
 *
 *****************************************************************************/

/**
   Note:
   - Currently the MC mismodels the Reta and w2 distributions. 
     We redefine the isEM definitions such that the Reta and w2 cuts are loosened to recover data/MC 
     agreement in electron efficiency. 
     isRobustLoose, isRobustMedium, and isRobusterTight, address this problem.    

     The changes to the cut values are:

     eT < 5 GeV:
       - w2: 0.014 |eta| > 1.81
       - REta: 0.7 for |eta| < 1.37,   
               {0.848, 0.876, 0.870, 0.880} for |eta| bins {1.52,1.81,2.01,2.37,2.47}  

     5 <= eT < 10 GeV:
       - w2: 0.014 |eta| > 1.81
       - REta: 0.7 for |eta| < 1.37,   
               {0.860, 0.880} for |eta| bins {1.52,1.81,2.47}  

     10 <= eT < 20 GeV:
       - w2: 0.014 |eta| > 1.81
       - REta: 0.86 for |eta| < 1.37,   
               {0.860, 0.880} for |eta| bins {1.52,1.81,2.47}  
     
     eT >= 20 GeV:
       - REta 0.9 for all |eta|
       - w2 0.013 for |eta| > 1.81, default otherwise

             
   - In addition, if a standard-isEM tight electron candidate crosses a disabled BLayer module 
     the candidate is kept tight,
     despite having no BLayer hit.  
     The conversion flag is set if the electron is recovered as a conversion. 
     This recovery procedure however does not yet check the status of the crossed BLayer. 
     Thus tight electrons crossing disabled BLayers will be recovered as single track conversions
     and the isConv bit will be set, which means they will not satisfy the standard-isEM tight criteria.
     isRobusterTight resolves this problem as well.
   
  Usage:
     The isRobustLoose, isRobustMedium, isRobusterTight methods take:
        - the electron isEM bit-mask
	- a bool indicating if we expect a BLayer hit. (isRobusterTight Only)
	- the electron cluster eta
	- the electron eT (IN MEV)
	- the electron REta 
	- the electron w2 

     They return true if:
       isRobustLoose
       - electron satisfies the Loose cuts with new REta and w2 cut values 
       isRobustMedium
       - electron satisfies the isRobustLoose and Medium cuts 
       isRobusterTight
       - electron satisfies the isRobustMedium and tight cuts EXCEPT DPhi DEta.
          or
       - electron satisfies the isRobustMedium and tight cuts EXCEPT DPhi DEta, and the isConv flag
          AND no BLayer hit is expected

     They return false otherwise.
*/


// This class's header
#include "AnalysisUtils/ElectronIDSelectorCore.h"

// STL includes
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <cfloat>

// Atlas includes
#include "GaudiKernel/ToolHandle.h"
#include "EventKernel/INavigable4Momentum.h"
#include "egammaEvent/Electron.h"
#include "egammaEvent/egamma.h"
#include "egammaEvent/EMShower.h"
#include "egammaEvent/egammaPIDdefsObs.h"
#include "Particle/TrackParticle.h"
#include "CLHEP/Units/SystemOfUnits.h"


using CLHEP::GeV;



//=============================================================================
// The contructor
//=============================================================================
ElectronIDSelectorCore::ElectronIDSelectorCore( PropertyMgr *pmgr ):  
  AraToolBase(pmgr),
  ISelectorCore(pmgr),
  m_isEM(0),
  m_isEM_NoConvCut(0)
{

  //Need a mechanism to turn on/off a cut
  // The default values are such that the cut is effectefly turned off.
  // Name of the EMShower container
  declareProperty( "EMShowerContainerName", m_showerContainerName="egDetailAOD", "Name of the EMShower container" );
  declareProperty( "isEM",                  m_isEMCut="",                        "IsEM cut selection");

}



//=============================================================================
// Athena initialize method
//=============================================================================
StatusCode ElectronIDSelectorCore::initialize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  // Set the IsEM bitmask to the wanted criteria
  if ( m_isEMCut == "RobustLoose" )
    {
      // Does it pass the loose isEM with Reta and w2 removed?
      const unsigned int CALOMIDDLE_ELECTRON_NoReta_NoW2 = 0x1 << egammaPIDObs::ClusterMiddleEnergy_Electron;
      const unsigned int ElectronLoose_NoReta_NoW2 = CALOMIDDLE_ELECTRON_NoReta_NoW2 | egammaPIDObs::HADLEAKETA_ELECTRON;
      m_isEM = ElectronLoose_NoReta_NoW2;
    }
  else if ( m_isEMCut == "RobustMedium" )
    {
      const unsigned int CALOMIDDLE_ELECTRON_NoReta_NoW2 = 0x1 << egammaPIDObs::ClusterMiddleEnergy_Electron;
  
      const unsigned int CALO_ELECTRON_NoReta_NoW2 = 
        egammaPIDObs::HADLEAKETA_ELECTRON | 
        egammaPIDObs::CALOSTRIPS_ELECTRON | 
        CALOMIDDLE_ELECTRON_NoReta_NoW2 ;

      const unsigned int ElectronMedium_NOReta_NoW2 = 
        CALO_ELECTRON_NoReta_NoW2 | 
        egammaPIDObs::TRACKINGNOBLAYER_ELECTRON | 
        egammaPIDObs::TRACKMATCHDETA_ELECTRON;
  
      m_isEM = ElectronMedium_NOReta_NoW2;
    }
  else if ( m_isEMCut == "RobustTight" || m_isEMCut == "RobusterTight" )
    {
      const unsigned int CALOMIDDLE_ELECTRON_NoReta_NoW2 = 0x1 << egammaPIDObs::ClusterMiddleEnergy_Electron;
   
      const unsigned int CALO_ELECTRON_NoReta_NoW2 = 
        egammaPIDObs::HADLEAKETA_ELECTRON | 
        egammaPIDObs::CALOSTRIPS_ELECTRON | 
        CALOMIDDLE_ELECTRON_NoReta_NoW2 ;  
   
      const unsigned int ElectronTightRobust_NoReta_NoW2 = 
        CALO_ELECTRON_NoReta_NoW2 | 
        egammaPIDObs::TRACKING_ELECTRON | 
        0x1 << egammaPIDObs::TrackMatchEta_Electron | 
        0x1 << egammaPIDObs::TrackMatchEoverP_Electron | 
        0x1 << egammaPIDObs::TrackA0Tight_Electron |
        0x1 << egammaPIDObs::ConversionMatch_Electron |
        egammaPIDObs::TRT_ELECTRON ;

      m_isEM = ElectronTightRobust_NoReta_NoW2;

      // Tight without the conversion requirement 
      const unsigned int ElectronTightRobust_NoReta_NoW2_NoConvCut = 
        CALO_ELECTRON_NoReta_NoW2 | 
        egammaPIDObs::TRACKING_ELECTRON | 
        0x1 << egammaPIDObs::TrackMatchEta_Electron | 
        0x1 << egammaPIDObs::TrackMatchEoverP_Electron | 
        0x1 << egammaPIDObs::TrackA0Tight_Electron |
        egammaPIDObs::TRT_ELECTRON ;
      
      m_isEM_NoConvCut = ElectronTightRobust_NoReta_NoW2_NoConvCut;
    }
  else
    {
      std::cerr << "IsEM is not recognized! Please configure it properly"
                << std::endl;
      sc = StatusCode::FAILURE;
    }


  return sc ;
}





//=============================================================================
// Athena finalize method
//=============================================================================
StatusCode ElectronIDSelectorCore::finalize()
{
  // The standard status code
  StatusCode sc = StatusCode::SUCCESS ;
  
  return sc ;
}




//=============================================================================
// The main accept method for INavigable4Momentum
//=============================================================================
bool ElectronIDSelectorCore::accept( const INavigable4Momentum *part ) const
{
  // Cast to an electron and if succesfull, perform selection
  const Analysis::Electron* electron = dynamic_cast< const Analysis::Electron* >( part );
  if ( !electron )
    {
      std::cerr << "Couldn't cast to an electron! Are you sure you are using this right?"
                << std::endl;
      return true;
    }


  // Does it pass the isEM with Reta and w2 removed?
  if ( m_isEM_NoConvCut == 0 && electron->isem(m_isEM) != 0 )
    {
      return false;
    }
  
  // Check for the b-layer requirement if RobusterTight is requested
  if ( m_isEM_NoConvCut != 0 )
    {
      bool expectInnermostPixelLayerHit = electron->detailValue( egammaParameters::expectHitInBLayer );
      if ( expectInnermostPixelLayerHit )
        {
          if ( electron->isem(m_isEM) != 0 )
            {
              return false;
            }
        }
      else // no b-layer hit is expected; thus don't use the conversion cut
        {
          if ( electron->isem(m_isEM_NoConvCut) != 0 )
            {
              return false;
            }
        }
    }


  // Get the CaloCluster for this electron and make sure that we got it
  const CaloCluster* cluster = electron->cluster();
  if ( !cluster )
    {
      std::cerr << "Couldn't get the CaloCluster for this electron! Passing this electron."
                << std::endl;
      return true;
    }
      
  // eta position in second sampling
  double eta2 = fabs(cluster->etaBE(2)); 
  // transverse energy in calorimeter (using eta position in second sampling)
  double et = 0.;
  if (fabs(eta2)<999.)
    { 
      et = cosh(eta2)!=0. ? cluster->energy()/cosh(eta2) : 0.;
    }

  // Get the absolute value of eta2
  double absEta2 = fabs(eta2);
  

  // retrieve shower object
  const EMShower* shower = electron->detail<EMShower>(m_showerContainerName);
  if ( !shower )
    {
      std::cerr << "Couldn't get the EMShower object for this electron! Passing this electron."
                << std::endl;
      return true;
    }


  // Get the W2 for this electron
  double weta2 = shower->weta2();

  // Do the modified W2 cut
  double w2CutValue = getW2Cut( et, absEta2 );
  if ( weta2 > w2CutValue )
    {
      return false; 
    }
  

  // E(3*7) in 2nd sampling
  double e237   = shower->e237(); 
  // E(7*7) in 2nd sampling
  double e277   = shower->e277(); 
  // Get the REta for this electron
  double reta = e277 != 0.0 ? e237/e277 : 0.0;

  // Do the modified REta cut
  double REtaCutValue = getREtaCut( et, absEta2 );
  if ( reta <= REtaCutValue )
    {
      return false;
    }

  return true;
}








//=============================================================================
// Gets the Eta bin [0-9] given the eta
//=============================================================================
unsigned int ElectronIDSelectorCore::getEtaBin(double eta) const
{
  const unsigned int nEtaBins = 10;
  const double etaBins[nEtaBins] = {0.1,0.6,0.8,1.15,1.37,1.52,1.81,2.01,2.37,2.47};
  
  for ( unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin )
    {
      if ( eta < etaBins[etaBin] )
        {
          return etaBin;
        }
    }
  
  return 9;
}


//=============================================================================
// Gets the Et bin [0-10] given the et (MeV)
//=============================================================================
unsigned int ElectronIDSelectorCore::getEtBin(double eT) const
{
  const unsigned int nEtBins = 11;
  const double eTBins[nEtBins] = {5*GeV,10*GeV,15*GeV,20*GeV,30*GeV,40*GeV,50*GeV,60*GeV,70*GeV,80*GeV};
  
  for ( unsigned int eTBin = 0; eTBin < nEtBins; ++eTBin )
    {
      if ( eT < eTBins[eTBin] )
        {
          return eTBin;
        }
    }
  
  return 10;
}




//=============================================================================
// Get the REta cut value for a given et and eta
//=============================================================================
double ElectronIDSelectorCore::getREtaCut( double eT, double eta ) const
{
  // New values cut on ratio e237/e277 (rows are eT bins, columns are eta bins)
  const double cutReta37[11][10] = {{ 0.700, 0.700, 0.798, 0.700, 0.700, 0.690, 0.848, 0.876, 0.870, 0.894}  // < 5
                                    ,{0.700, 0.700, 0.700, 0.700, 0.700, 0.715, 0.860, 0.880, 0.880, 0.880} // 5-10
                                    ,{0.860, 0.860, 0.860, 0.860, 0.860, 0.730, 0.860, 0.880, 0.880, 0.880}// 10-15
                                    ,{0.860, 0.860, 0.860, 0.860, 0.860, 0.740, 0.860, 0.880, 0.880, 0.880}// 15-20
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 20-30
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 30-40
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 40-50
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 50-60
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 60-70
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}// 70-80
                                    ,{0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900, 0.900}};// 80<

  unsigned int eTBin = getEtBin(eT);
  unsigned int etaBin = getEtaBin(eta);

  return cutReta37[eTBin][etaBin];
}





//=============================================================================
// Get te W2 cut value for a given et and eta
//=============================================================================
double ElectronIDSelectorCore::getW2Cut( double eT, double eta ) const
{
  //New values for cut on shower width in 2nd sampling (rows are eT bins, columns are eta bins)
  const double cutWeta2[11][10] = {{ 0.014, 0.014, 0.014, 0.014, 0.014, 0.028, 0.017, 0.014, 0.014, 0.014}   // < 5 
                                   ,{0.013, 0.013, 0.014, 0.014, 0.014, 0.026, 0.017, 0.014, 0.014, 0.014}  // 5-10
                                   ,{0.013, 0.013, 0.014, 0.014, 0.014, 0.025, 0.017, 0.014, 0.014, 0.014} // 10-15
                                   ,{0.012, 0.012, 0.013, 0.013, 0.013, 0.025, 0.017, 0.014, 0.014, 0.014} // 15-20
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 20-30
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 30-40
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 40-50
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 50-60
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 60-70
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013} // 70-80
                                   ,{0.012, 0.012, 0.012, 0.013, 0.015, 0.025, 0.015, 0.013, 0.013, 0.013}};// 80<;

  unsigned int eTBin = getEtBin(eT);
  unsigned int etaBin = getEtaBin(eta);
    
  return  cutWeta2[eTBin][etaBin];
}

