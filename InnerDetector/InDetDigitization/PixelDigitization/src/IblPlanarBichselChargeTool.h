/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IblPlanarBichselChargeTool.h
//   Header file for class IblPlanarBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters
//
// -numberOfSteps	Geant4:number of steps for IblPlanar
// -numberOfCharges	Geant4:number of charges for IblPlanar
// -diffusionConstant	Geant4:Diffusion Constant for IblPlanar
//
//////////////////////////////////////////////////////////////////

#ifndef PIXELDIGITIZATION_IblPlanarBichselChargeTool_H
#define PIXELDIGITIZATION_IblPlanarBichselChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"

// Base class
class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

class BichselSimTool;

class IblPlanarBichselChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  IblPlanarBichselChargeTool( const std::string& type, const std::string& name,const IInterface* parent);

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~IblPlanarBichselChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  
  
private:
  /** empty constructor, make private */
  IblPlanarBichselChargeTool();

void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
private:

 //Constants that can be set by user  
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
  int m_numberOfCharges;  
  double m_diffusionConstant;

  bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model
  double m_doBichselBetaGammaCut;                      // Momentum cut on beta-gamma
  bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
                                                       // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
  bool   m_doPU;                                       // whether we apply Bichsel model on PU
  ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here
  bool m_doSlimEdges;
  
	};


#endif // PIXELDIGITIZATION_IblPlanarBichselChargeTool_H
