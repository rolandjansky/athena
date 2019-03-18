/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSimTool.h
//   Header file for class SensorSimTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef PIXELDIGITIZATION_SensorSimTool_H
#define PIXELDIGITIZATION_SensorSimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "HitManagement/TimedHitPtr.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiPropertiesTool/ISiPropertiesTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

static const InterfaceID IID_ISensorSimTool("SensorSimTool", 1, 0);

class SensorSimTool:public AthAlgTool,virtual public IAlgTool {

  public:
    SensorSimTool( const std::string& type, const std::string& name,const IInterface* parent) : 
      AthAlgTool(type,name,parent),
      m_rndmSvc("AtDSFMTGenSvc",name),
      m_rndmEngineName("PixelDigitization"),
      m_rndmEngine(nullptr)	
  {
    declareInterface<SensorSimTool>(this);
    declareProperty("RndmSvc",           m_rndmSvc,            "Random Number Service used in SCT & Pixel digitization");
    declareProperty("RndmEngine",        m_rndmEngineName,     "Random engine name");
  }

    static const InterfaceID& interfaceID() { return IID_ISensorSimTool; }

    virtual StatusCode initialize() {
      CHECK(AthAlgTool::initialize()); 

      CHECK(m_siPropertiesTool.retrieve());

      CHECK(m_rndmSvc.retrieve());

      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (!m_rndmEngine) {
        ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
      }

      return StatusCode::SUCCESS;
    }

    virtual StatusCode finalize() {return StatusCode::FAILURE;}
    virtual ~SensorSimTool() {}
    virtual StatusCode induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions) = 0;  

  private:
    SensorSimTool();

  protected:
    ToolHandle<ISiPropertiesTool>   m_siPropertiesTool{this, "SiPropertiesTool", "SiPropertiesTool", "Tool to retrieve SiProperties"};
    ServiceHandle<IAtRndmGenSvc>    m_rndmSvc;
    std::string 		                m_rndmEngineName;
    CLHEP::HepRandomEngine         *m_rndmEngine;	

  private:
    const InDetDD::SiDetectorElement *m_module;   
};


#endif // PIXELDIGITIZATION_SensorSimTool_H
