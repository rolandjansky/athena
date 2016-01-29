/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelBarrelBichselChargeTool.h
//   Header file for class PixelBarrelBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for PixelBarrel
// -numberOfCharges	Geant4:number of charges for PixelBarrel
// -diffusionConstant	Geant4:Diffusion Constant for PixelBarrel
// -doBichsel : re-do charge deposition following Bichsel model
// -BichselMomentumCut : minimum MOMENTUM for particle to be re-simulated through Bichsel Model
// -BichselSimTool : tool that implements Bichsel model
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_PixelBarrelBichselChargeTool_H
#define PIXELDIGITIZATION_PixelBarrelBichselChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"

// Base class
class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

class BichselSimTool; 

class PixelBarrelBichselChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  PixelBarrelBichselChargeTool( const std::string& type, const std::string& name,const IInterface* parent);


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~PixelBarrelBichselChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  //Constants that can be set by user  
  int    m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
  int    m_numberOfCharges;  
  double m_diffusionConstant;
  bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model ?
  //double m_doBichselMomentumCut;                       // minimum MOMENTUM for particle to be re-simulated through Bichsel Model. Unit in MeV
  double m_doBichselBetaGammaCut;                      // replace momentum cut
  bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
                                                       // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
  ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here
  std::string m_OutputFileName;                        // name of output file for customized study during digitization
  bool   m_doHITPlots;                                 // validation plots on HIT level

private:
  /** empty constructor, make private */
  PixelBarrelBichselChargeTool();

  TFile* f_output;
  TH1D*  h_Length;
  TH1D*  h_EnergyDepositionBichsel;
  TH1D*  h_EnergyDepositionNominal;
  TH1D*  h_EnergyDepositionDeltaRay;
  TH1D*  h_EnergyDepositionDeltaRay_Bichsel;
  TH1D*  h_EnergyDepositionDeltaRay_Nominal;
  TH1D*  h_isRealBichsel_DeltaRay;
  TH1D*  h_isRealBichsel_Primary;        // check if it is a real Bichsel model implementation
  std::map<std::string, TH2D*> h_dEdxBetaGammaMap_Bichsel;    // x in log-scale
  std::map<std::string, TH2D*> h_dEdxBetaGammaMap_Nominal;
  std::map<std::string, TH2D*> h_dEdxMomentumMap_Bichsel;
  std::map<std::string, TH2D*> h_dEdxMomentumMap_Nominal;
  TH1D* h_largestEnergyDeposition;
  TH1D* h_twolargestEnergyDeposition;


void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
private:
  
	};


#endif // PIXELDIGITIZATION_PixelBarrelBichselChargeTool_H
