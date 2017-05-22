/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDIGITIZATION_FrontEndSimTool_H
#define PIXELDIGITIZATION_FrontEndSimTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include "SiDigitization/SiChargedDiodeCollection.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetConditionsSummaryService/IInDetConditionsSvc.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "InDetSimEvent/SiTotalCharge.h"

#include "CommissionEvent/ComTime.h"

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
      m_timeBCN(1),
      m_timeZero(5.0),
      m_timePerBCO(25.0),
      m_useComTime(false),
      m_ComputedTime(nullptr),
      m_comTime(0.0),
      m_timeJitter(0.0),
      m_eventStore("StoreGateSvc", name),
      m_BarrelAnalogthreshold({-1,-1,-1,-1,-1,-1,-1}),
      m_EndcapAnalogthreshold({-1,-1,-1,-1,-1,-1,-1}),
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
	  declareProperty("TimeBCN",                   m_timeBCN,        "Number of BCID");	
	  declareProperty("TimeZero",                  m_timeZero,       "Time zero...?");
	  declareProperty("TimePerBCO",                m_timePerBCO,     "Time per BCO - should be 25ns");
    declareProperty("UseComTime",                m_useComTime,     "Use ComTime for timing");
	  declareProperty("TimeJitter",                m_timeJitter,     "Time jitter");
    declareProperty("BarrelAnalogthreshold",     m_BarrelAnalogthreshold);
    declareProperty("EndcapAnalogthreshold",     m_EndcapAnalogthreshold);
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

      m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
      if (!m_rndmEngine) {
        ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
        return StatusCode::FAILURE;
      }
      else {
        ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);
      }

      if (m_useComTime) {
        CHECK(m_eventStore.retrieve());
        if (StatusCode::SUCCESS==m_eventStore->retrieve(m_ComputedTime,"ComTime")) {
          m_comTime = m_ComputedTime->getTime();
          ATH_MSG_DEBUG("Found tool for cosmic/commissioning timing: ComTime");
        } 
        else {
          ATH_MSG_WARNING("Did not find tool needed for cosmic/commissioning timing: ComTime");
        }
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

    double m_timeBCN;
    double m_timeZero;
    double m_timePerBCO;
    bool   m_useComTime;    /**< use ComTime for timing */
    ComTime  *m_ComputedTime;
    double m_comTime;       /**< cosmics timing ofs */
    double m_timeJitter; 
    ServiceHandle<StoreGateSvc>  m_eventStore;
    std::vector<int> m_BarrelAnalogthreshold;
    std::vector<int> m_EndcapAnalogthreshold;
    std::vector<int> m_BarrelToTthreshold;
    std::vector<int> m_EndcapToTthreshold;
    std::vector<int> m_BarrelLatency;
    std::vector<int> m_EndcapLatency;

    double getG4Time(const SiTotalCharge &totalCharge) const {
      // If there is one single charge, return its time:
      if (totalCharge.chargeComposition().empty()) { return totalCharge.time(); }

      std::list<SiCharge>::const_iterator p_charge=totalCharge.chargeComposition().begin();
      int findfirst = 0;
      SiCharge first = *p_charge;

      // Look for first charge which is not noise
      for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
        if (p_charge->processType()!=SiCharge::noise) { findfirst=1; break; }
      }

      // if all charges were noise, return the time of the highest charge
      if (findfirst == 0) { return totalCharge.time(); }

      // look for the earlist charge among the remaining non-noise charges:
      first = *p_charge;
      p_charge++;

      for ( ; p_charge!=totalCharge.chargeComposition().end() ; p_charge++) {
        if (p_charge->time()<first.time() && p_charge->processType()!=SiCharge::noise) { first=*p_charge; }
      }
      return first.time();
    }

};

#endif // PIXELDIGITIZATION_FrontEndSimTool_H
