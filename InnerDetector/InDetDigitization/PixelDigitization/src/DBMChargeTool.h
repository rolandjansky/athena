/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_DBMChargeTool_H
#define PIXELDIGITIZATION_DBMChargeTool_H

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

class DBMChargeTool : public SubChargesTool {

public:
  
  // Constructor:
  DBMChargeTool( const std::string& type, const std::string& name,const IInterface* parent);

  /** AlgTool initialize */
  virtual StatusCode initialize();

  /** AlgTool finalize */
  virtual StatusCode finalize();

  /** Destructor */
  virtual ~DBMChargeTool();

  virtual StatusCode charge(const TimedHitPtr<SiHit> &phit,
			    SiChargedDiodeCollection& chargedDiodes,
			    const InDetDD::SiDetectorElement &Module);  

  virtual StatusCode LoadPixelParameters();
  //Constants that can be set by user  
  int m_numberOfSteps;    //number of steps for particle traveling perpendicular to detector element
  int m_numberOfCharges;  
  double m_diffusionConstant;
 
private:
  /** empty constructor, make private */
  DBMChargeTool();

void simulateBow(const InDetDD::SiDetectorElement * element,double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const;
private:

	};


#endif // PIXELDIGITIZATION_DBMChargeTool_H
