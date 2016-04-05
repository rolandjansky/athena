/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_Ibl3DBichselChargeTool_H
#define PIXELDIGITIZATION_Ibl3DBichselChargeTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SubChargesTool.h"
#include "IChargeCollProbSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1.h"
#include "TH2.h"


// Base class
class PixelID;

namespace InDetDD {
  class SiDetectorElement;
}

namespace CLHEP {
  class HepRandomEngine;
}

class BichselSimTool;

class Ibl3DBichselChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  Ibl3DBichselChargeTool( const std::string& type, const std::string& name,const IInterface* parent);


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~Ibl3DBichselChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  //Constants that can be set by user 
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element

  bool   m_doBichsel;                                  // re-do charge deposition following Bichsel model
  double m_doBichselBetaGammaCut;                      // Momentum cut on beta-gamma
  bool   m_doDeltaRay;                                 // implement Bichsel Model into delta-ray, which does not have truth particle link. 
                                                       // We will assume all delta-ray is electron, with all energy deposited in silicon layer. So the 4-momentum can be reconstructed using energy and direction
  ToolHandle<BichselSimTool> m_BichselSimTool;         // if yes, you need to load related tool here
 
private:
  /** empty constructor, make private */
  Ibl3DBichselChargeTool();
private:
	ServiceHandle<IChargeCollProbSvc> m_chargeCollSvc;

	};


#endif // PIXELDIGITIZATION_Ibl3DBichselChargeTool_H
