/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelCalibSvc.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

PixelCalibSvc::PixelCalibSvc(const std::string& name, ISvcLocator* sl):AthService(name, sl),
  m_detStore("DetectorStore",name),
  m_dbTool("PixelCalibDbTool"),
  m_pixelCabling("PixelCablingSvc",name),
  m_pixid(0),
  m_wafer_id(0),
  m_detManager(0),
  m_totparA(70.2),
  m_totparE(-3561.25),
  m_totparC(26000),
  m_totparP1(-0.68),
  m_totparP2(0.17),
  m_discrThresh(3500),
  m_discrThreshSigma(300),
  m_intimeThresh(5000),
  m_noiseThresh(200),
  m_disableDb(false)
{
  declareProperty("CalibrationDbTool",m_dbTool);
  declareProperty("ToTParA",            m_totparA,              "TOT parameter A");
  declareProperty("ToTParE",            m_totparE,              "TOT parameter E");
  declareProperty("ToTParC",            m_totparC,              "TOT parameter C");
  declareProperty("ToTParP1",           m_totparP1,             "TOT smearing parameter p1");
  declareProperty("ToTParP2",           m_totparP2,             "TOT smearing parameter p2");
  declareProperty("DiscrThresh",        m_discrThresh,          "Discriminator threshold");
  declareProperty("DiscrThreshVar",     m_discrThreshSigma,     "Discriminator threshold sigma");
  declareProperty("IntimeThresh",       m_intimeThresh,         "Discriminator in-time threshold");
  declareProperty("NoiseThresh",        m_noiseThresh,          "Discriminator noise");
  declareProperty("DisableDB",          m_disableDb,            "Disable DB");
}

PixelCalibSvc::~PixelCalibSvc() {}

StatusCode PixelCalibSvc::queryInterface(const InterfaceID& riid, void** ppvIF) {
  if (IID_IPixelCalibSvc==riid) {
    *ppvIF = dynamic_cast<IPixelCalibSvc*>(this); 
  } 
  else { 
    return AthService::queryInterface(riid,ppvIF); 
  }
  addRef();
  return StatusCode::SUCCESS; 
}

StatusCode PixelCalibSvc::initialize() {
  ATH_MSG_INFO("PixelCalibSvc::initialize()");

  CHECK(m_detStore.retrieve());

  CHECK(m_detStore->retrieve(m_detManager,"Pixel"));

  CHECK(m_detStore->retrieve(m_pixid,"PixelID"));

  CHECK(m_pixelCabling.retrieve());

  if (!m_disableDb) { CHECK(m_dbTool.retrieve()); }

  return StatusCode::SUCCESS;
}

StatusCode PixelCalibSvc::finalize() {
  ATH_MSG_INFO("PixelCalibSvc::finalize()");
  return StatusCode::SUCCESS; 
}

bool PixelCalibSvc::hasCalibData(const Identifier& wafer_id) const {
  if (m_disableDb) { return false; }
  if (wafer_id!=m_wafer_id) {
    m_wafer_id = wafer_id;
  }
  return true;
}

int PixelCalibSvc::getThreshold(Identifier pix_id) const {
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  if (m_disableDb) { return m_discrThresh; }
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThreshold(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_discrThresh;
  }
}

int PixelCalibSvc::getThresholdSigma(Identifier pix_id) const {
  if (m_disableDb) { return m_discrThreshSigma; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThresholdSigma(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_discrThreshSigma;
  }
}
  
int PixelCalibSvc::getNoise(Identifier pix_id) const {
  if (m_disableDb) { return m_noiseThresh; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getNoise(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_noiseThresh;
  }
}
  
int PixelCalibSvc::getTimeWalk(Identifier pix_id) const {
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  if (m_disableDb) { return m_intimeThresh; }
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTimeWalk(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_intimeThresh;
  }
}
  
float PixelCalibSvc::getQ2TotA(Identifier pix_id) const {
  if (m_disableDb) { return m_totparA; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotA(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparA;
  }
}

float PixelCalibSvc::getQ2TotE(Identifier pix_id) const {
  if (m_disableDb) { return m_totparE; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotE(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparE;
  }
}
  
float PixelCalibSvc::getQ2TotC(Identifier pix_id) const {
  if (m_disableDb) { return m_totparE; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotC(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparC;
  }
}

float PixelCalibSvc::getTotRes(Identifier pix_id, float Q) const {
  if (m_disableDb) { return m_totparP1+m_totparP2*Q; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotRes(Q); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP1+m_totparP2*Q;
  }
}

float PixelCalibSvc::getTotMean(Identifier pix_id, float Q) const {
  if (m_disableDb) { return m_totparA*(m_totparE+Q)/(m_totparC+Q); }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = m_pixelCabling->getFE(&pix_id,wafer_id);
  int type = m_pixelCabling->getPixelType(pix_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2Tot(type,Q); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparA*(m_totparE+Q)/(m_totparC+Q);
  }
}

float PixelCalibSvc::getCharge(Identifier pix_id, float ToT) const {
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  float charge = 0.0;
  if (m_pixid->is_dbm(wafer_id)) {
    charge = ToT/8.0*(8000.0-1200.0)+1200.0;
  }
  else {
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (ToT>=m_pixelCabling->getIBLOverflowToT(&pix_id)) { ToT=m_pixelCabling->getIBLOverflowToT(&pix_id); }
    }
    float termA = getQ2TotA(pix_id);
    float termE = getQ2TotE(pix_id);
    float termC = getQ2TotC(pix_id);
    if (fabs(termA)>0.0 && fabs(ToT/termA-1.0)>0.0) {
      charge = (termC*ToT/termA-termE)/(1.0-ToT/termA);
    }
  }
  return charge;
}

