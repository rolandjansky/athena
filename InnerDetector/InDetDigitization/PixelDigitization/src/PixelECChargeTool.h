/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelECChargeTool.cxx
//   Implementation file for class PixelECChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
//
// Configurable Parameters:
// -numberOfSteps	Geant4:number of steps for PixelEC
// -numberOfCharges	Geant4:number of charges for PixelEC
// -diffusionConstant	Geant4:Diffusion Constant for PixelEC
//
//////////////////////////////////////////////////////////////////



#ifndef PIXELDIGITIZATION_PixelECChargeTool_H
#define PIXELDIGITIZATION_PixelECChargeTool_H

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

class PixelECChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  PixelECChargeTool( const std::string& type, const std::string& name,const IInterface* parent);


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~PixelECChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  //Constants that can be set by user  
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
  int m_numberOfCharges;  
  double m_diffusionConstant;
 
private:
  /** empty constructor, make private */
  PixelECChargeTool();

void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
private:
  
	};


#endif // PIXELDIGITIZATION_PixelECChargeTool_H
