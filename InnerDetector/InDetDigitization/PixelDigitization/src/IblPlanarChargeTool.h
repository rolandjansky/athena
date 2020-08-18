/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IblPlanarChargeTool.h
//   Header file for class IblPlanarChargeTool
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

#ifndef PIXELDIGITIZATION_IblPlanarChargeTool_H
#define PIXELDIGITIZATION_IblPlanarChargeTool_H

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

class IblPlanarChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  IblPlanarChargeTool( const std::string& type, const std::string& name,const IInterface* parent);

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~IblPlanarChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

 
  
private:
  /** empty constructor, make private */
  IblPlanarChargeTool();

void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
private:

   //Constants that can be set by user  
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
  int m_numberOfCharges;  
  double m_diffusionConstant;
  bool m_doSlimEdges;
  
	};


#endif // PIXELDIGITIZATION_IblPlanarChargeTool_H
