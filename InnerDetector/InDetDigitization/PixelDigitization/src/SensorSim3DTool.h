/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_SensorSim3DTool_H
#define PIXELDIGITIZATION_SensorSim3DTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "SensorSimTool.h"
#include "IChargeCollProbSvc.h"


class SensorSim3DTool : public SensorSimTool {

  public:
    SensorSim3DTool( const std::string& type, const std::string& name,const IInterface* parent);
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual ~SensorSim3DTool();

    virtual StatusCode induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine);  


  private:
    SensorSim3DTool();

    int m_numberOfSteps;
    ServiceHandle<IChargeCollProbSvc> m_chargeCollSvc;
};

#endif // PIXELDIGITIZATION_SensorSim3DTool_H
