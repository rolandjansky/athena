/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_FrontEndSimTool_H
#define PIXELDIGITIZATION_FrontEndSimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "TimeSvc.h"

static const InterfaceID IID_IFrontEndSimTool("FrontEndSimTool", 1, 0);

class FrontEndSimTool:public AthAlgTool,virtual public IAlgTool {

  public:
    FrontEndSimTool( const std::string& type, const std::string& name,const IInterface* parent):
      AthAlgTool(type,name,parent),
      m_rndmSvc("AtRndmGenSvc",name),
      m_rndmEngineName("PixelDigitization"),
      m_rndmEngine(nullptr),
      m_pixelConditionsSvc("PixelConditionsSummarySvc",name),
      m_pixelCalibSvc("PixelCalibSvc",name),
      m_TimeSvc("TimeSvc",name),
      m_BarrelToTthreshold({-1,-1,-1,-1,-1,-1,-1}),
      m_EndcapToTthreshold({-1,-1,-1,-1,-1,-1,-1}),
      m_BarrelLatency({256,256,256,256,256,256,256}),
      m_EndcapLatency({256,256,256,256,256,256,256})
  {
    declareInterface<FrontEndSimTool>(this);
    declareProperty("RndmSvc",                   m_rndmSvc,        "Random number service used in FE simulation");
    declareProperty("RndmEngine",                m_rndmEngineName, "Random engine name");
    declareProperty("PixelConditionsSummarySvc", m_pixelConditionsSvc);
    declareProperty("PixelCalibSvc",             m_pixelCalibSvc);
    declareProperty("TimeSvc",                   m_TimeSvc);
    declareProperty("BarrelToTthreshold",        m_BarrelToTthreshold);
    declareProperty("EndcapToTthreshold",        m_EndcapToTthreshold);
    declareProperty("BarrelLatency",             m_BarrelLatency);
    declareProperty("EndcapLatency",             m_EndcapLatency);
  }

    static const InterfaceID& interfaceID() { return IID_IFrontEndSimTool; }

    virtual StatusCode initialize() {
      CHECK(AthAlgTool::initialize()); 

      CHECK(m_rndmSvc.retrieve());

      CHECK(m_pixelConditionsSvc.retrieve());

      CHECK(m_pixelCalibSvc.retrieve());

      CHECK(m_TimeSvc.retrieve());

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

    virtual StatusCode finalize() { return StatusCode::FAILURE; }
    virtual ~FrontEndSimTool() {}
    virtual void process(SiChargedDiodeCollection &chargedDiodes,PixelRDO_Collection &rdoCollection) = 0;

  private:
    FrontEndSimTool();

  protected:
    ServiceHandle<IAtRndmGenSvc> m_rndmSvc;
    std::string                  m_rndmEngineName;
    CLHEP::HepRandomEngine      *m_rndmEngine;	

    ServiceHandle<IInDetConditionsSvc>   m_pixelConditionsSvc;
    ServiceHandle<IPixelCalibSvc>        m_pixelCalibSvc;
    ServiceHandle<TimeSvc>               m_TimeSvc;

    std::vector<int> m_BarrelToTthreshold;
    std::vector<int> m_EndcapToTthreshold;
    std::vector<int> m_BarrelLatency;
    std::vector<int> m_EndcapLatency;

};

#endif // PIXELDIGITIZATION_FrontEndSimTool_H
