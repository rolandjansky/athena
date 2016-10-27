/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_Ibl3DChargeTool_H
#define PIXELDIGITIZATION_Ibl3DChargeTool_H

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

class Ibl3DChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  Ibl3DChargeTool( const std::string& type, const std::string& name,const IInterface* parent);


  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~Ibl3DChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  //Constants that can be set by user 
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
 
private:
  /** empty constructor, make private */
  Ibl3DChargeTool();
private:
	ServiceHandle<IChargeCollProbSvc> m_chargeCollSvc;

	};


#endif // PIXELDIGITIZATION_Ibl3DChargeTool_H
