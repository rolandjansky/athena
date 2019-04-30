/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCalibSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelCalibSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "StoreGate/StoreGate.h" 
#include "Identifier/Identifier.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"

#include "InDetIdentifier/PixelID.h"
//#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsTools/IPixelCalibDbTool.h"
#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"
//Includes related to determining presence of ITK
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

// static unsigned int columnsPerFEI3 = 18;   // number of columns per FEI3 (18x160)

/*
static unsigned int rowsPerFEI3    = 164;   // number of rows per FEI3

static unsigned int rowsFGangedFEI3  =153;   // first ganged pixel row for FEI3
static unsigned int rowsLGangedFEI3  =159;   // last ganged pixel row for FEI3

static unsigned int columnsPerFEI4 = 80;   // number of columns per FEI4 (80x336)
static unsigned int rowsPerFEI4 = 336; // number of rows per FEI4
*/

static unsigned int columnsPerFEI50 = 132; // number of columns per FEI5-type 0 (132x672) for barrel layer 0
// static unsigned int rowsPerFEI50 = 672; // number of rows per FEI50

static unsigned int columnsPerFEI52 = 132; // number of columns per FEI5-type 2 (132x672) for barrel layer 1 
// static unsigned int rowsPerFEI52 = 678; // number of rows per FEI52
static unsigned int rowsFGangedFEI52  =661;   // first ganged pixel row for FEI52
static unsigned int rowsLGangedFEI52  =671;   // last ganged pixel row for FEI52

static unsigned int columnsPerFEI51 = 80; // number of columns per FEI5-type 1 (80x336) for the rest 
// static unsigned int rowsPerFEI51 = 339; // number of rows per FEI51
static unsigned int rowsFGangedFEI51  =331;   // first ganged pixel row for FEI51
static unsigned int rowsLGangedFEI51  =335;   // last ganged pixel row for FEI51

static bool isITK(false);

//================ Constructor =================================================

PixelCalibSvc::PixelCalibSvc(const std::string& name, ISvcLocator* sl):AthService(name, sl),
  m_sgSvc("StoreGateSvc",name),	
  m_detStore("DetectorStore",name),
  m_dbTool("PixelCalibDbTool"),
  m_pixid(0),
  m_pat(0),
  m_wafer_id(0),
  m_detManager(0),
  m_totparA(70.2),
  m_totparE(-3561.25),
  m_totparC(26000),
  m_totparP1(-0.68),
  m_totparP2(0.17),
  m_discrThresh(3500),
  m_discrThreshIBL(1500),
  m_discrThreshSigma(300),
  m_intimeThresh(5000),
  m_intimeThreshIBL(1900),
  m_noiseThresh(200),
  m_IBLabsent(true),
  m_disableDb(false),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_geoModelSvc("GeoModelSvc",name) ,
  m_offlineCalibSvc("PixelOfflineCalibSvc", name),
  m_specialIBL_correction(false),
  m_specialIBL_chargescale(0.985)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
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
  declareProperty("IBLParameterService", m_IBLParameterSvc); 
  declareProperty("GeoModelService",     m_geoModelSvc);
  declareProperty("PixelOfflineCalibSvc",m_offlineCalibSvc);
  declareProperty("SpecialIBLCorrection", m_specialIBL_correction);
  declareProperty("SpecialIBLChargeScale",m_specialIBL_chargescale);
}

//================ Destructor =================================================

PixelCalibSvc::~PixelCalibSvc()
{}


//=============== queryInterface ==================================================
StatusCode PixelCalibSvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if( IID_IPixelCalibSvc == riid ) {
    *ppvIF = dynamic_cast< IPixelCalibSvc* >(this); 
  } else{ 
    return AthService::queryInterface(riid, ppvIF); 
  }
  addRef();
  return StatusCode::SUCCESS; 
}

//================ Initialisation =================================================

StatusCode PixelCalibSvc::initialize() {

  // StoreGateSvc
  CHECK(m_sgSvc.retrieve());

  // Detector store
  CHECK(m_detStore.retrieve());

  // Get the detector manager
  CHECK(m_detStore->retrieve(m_detManager,"Pixel"));

  // Pixel ID
  CHECK(m_detStore->retrieve(m_pixid,"PixelID"));

  // GeoModelSvc
  CHECK(m_geoModelSvc.retrieve()); 
  if(m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN4) { isITK = true; }

  // OfflineCalibSvc
  CHECK(m_offlineCalibSvc.retrieve());

  // locate PixelCalibTools  
  CHECK(m_dbTool.retrieve());

  if (m_IBLParameterSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING( "Could not retrieve IBLParameterSvc");
  }
  else {
    m_IBLParameterSvc->setBoolParameters(m_IBLabsent,"IBLAbsent");
  }

  return StatusCode::SUCCESS;

}

//================ Finalisation =================================================

StatusCode PixelCalibSvc::finalize()
{
  ATH_MSG_INFO("PixelCalibSvc Finalizing.");
  return StatusCode::SUCCESS; 
}

//============================================================================
int PixelCalibSvc::PixelType(const Identifier wafer_id, int row, int col) const {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  // local row and col of ith circuit(0-15) 
  // each circuit contains 160 rows and 18 columns 
  if(isITK){ 
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (p_design->numberOfCircuits()==2) {     // IBL planar
        if(col==0||col==(static_cast<int>(columnsPerFEI50)-1))return 1; // long pixel
      }
      return 0;
    }
    else{
      int iec = m_pixid->barrel_ec(wafer_id);
      int il = m_pixid->layer_disk(wafer_id); 
      if(iec==0&&il==1)return gangedType(row, col, columnsPerFEI52,rowsFGangedFEI52,rowsLGangedFEI52);
      else return gangedType(row, col, columnsPerFEI51,rowsFGangedFEI51,rowsLGangedFEI51);
    }
  }
  else {
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (p_design->numberOfCircuits()==2) {       // IBL planar
        if (col==0 || col==p_design->columnsPerCircuit()-1) {   // column edge =0,79
          return 1; // long pixel
        }
      }
      return 0;
    }
    else{
      /* Obsolete Method  return gangedType(row, col, columnsPerFEI3,rowsFGangedFEI3,rowsLGangedFEI3); */
      return gangedType(row, col, p_design->columnsPerCircuit(),p_design->rowsPerCircuit()/2-1-6,p_design->rowsPerCircuit()/2-1);
    }
  }
}

int PixelCalibSvc::PixelType(const Identifier pix_id, const Identifier wafer_id, int &circ) const {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  int phi_index = m_pixid->phi_index(pix_id);
  int eta_index = m_pixid->eta_index(pix_id);
  int barrel_ec = m_pixid->barrel_ec(wafer_id);
  // int layer_disk = m_pixid->layer_disk(wafer_id);
  int phi_module = m_pixid->phi_module(wafer_id);

  // int FEIXsPerHalfModule = getNFE(wafer_id)/2;
// int FEIXsPerHalfModule = p_design->numberOfCircuits();   // can be...
  int col;
  int row;
  circ = -1;

  if (isITK) {

/*  So far comment them until the module specification is established.

    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      int columnsPerFE =columnsPerFEI50;
      col =  (columnsPerFE-1) -eta_index%columnsPerFE; // check the col order in FEI4 ?
      int rowsPerFE =rowsPerFEI50;
      row = (rowsPerFE-1)-phi_index; // check this ?
      circ = getNFE(wafer_id)==1 ? 0: 1-eta_index/columnsPerFE;
    }
    else{ // FEI51, FEI52
      int rowsPerFE =rowsPerFEI51; // check ?
      int columnsPerFE =columnsPerFEI51;
      if (barrel_ec == 2 || barrel_ec == -2) {
        if (phi_module%2 == 0) {
          phi_index = 2*rowsPerFE-phi_index-1;
        }
      }
      if(barrel_ec==0&&layer_disk==1){
        rowsPerFE = rowsPerFEI52;
        columnsPerFE = columnsPerFEI52;
      } 
      if (phi_index >= rowsPerFE) {
        circ = (int)((FEIXsPerHalfModule-1)-(eta_index/columnsPerFE));
        col=(columnsPerFE-1)-(eta_index%columnsPerFE);
        row = ((2*rowsPerFE)-1)-phi_index;
      } else {
        circ = (int)(eta_index/columnsPerFE)+FEIXsPerHalfModule;
        col=eta_index%columnsPerFE;
        row=phi_index;
      }
    }
*/

    col  = p_design->columnsPerCircuit();
    row  = 0;
    circ = (int)eta_index/p_design->columnsPerCircuit();
  }
  else {
    int columnsPerFE = p_design->columnsPerCircuit();
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (m_pixid->is_dbm(wafer_id)) {   // DBM
        columnsPerFE = p_design->rowsPerCircuit();
      }
      col = columnsPerFE-1-eta_index%columnsPerFE; // check the col order in FEI4 ?
      row = 0;
      circ = p_design->numberOfCircuits()==1 ? 0: eta_index/columnsPerFE;
    }
    else { // FEI3
      if (barrel_ec==2 || barrel_ec==-2) {
        if (phi_module%2 == 0) {
          phi_index = p_design->rowsPerCircuit()-phi_index+7;
        }
      }
      if (phi_index>=p_design->rowsPerCircuit()/2+4) {
        circ = (int)((p_design->numberOfCircuits()-1)-(eta_index/columnsPerFE));
        col = columnsPerFE-1-eta_index%columnsPerFE;
        row = p_design->rowsPerCircuit()-phi_index+7;
      } 
      else {
        circ = (int)(eta_index/columnsPerFE)+p_design->numberOfCircuits();
        col = eta_index%columnsPerFE;
        row = phi_index;
      }
    }
  }
  return PixelType(wafer_id, row, col);
}

int PixelCalibSvc::PixelCirc(const Identifier& pix_id, const Identifier& wafer_id) const {

  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());

  int phi_index = m_pixid->phi_index(pix_id);
  int eta_index = m_pixid->eta_index(pix_id);
  int barrel_ec = m_pixid->barrel_ec(wafer_id);
  // int layer_disk = m_pixid->layer_disk(wafer_id);
  int phi_module = m_pixid->phi_module(wafer_id);
  // int FEIXsPerHalfModule = getNFE(wafer_id)/2;
// int FEIXsPerHalfModule = p_design->numberOfCircuits();   // can be...
  int circ(-1);
  if (isITK) {

/*  So far comment them until the module specification is established.

    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      int columnsPerFE = columnsPerFEI50;
      circ = getNFE(wafer_id)==1 ? 0: 1-eta_index/columnsPerFE;
    }
    else{ // FEI51, FEI52
      int rowsPerFE = rowsPerFEI51; // check ?
      int columnsPerFE = columnsPerFEI51;
      if (barrel_ec == 2 || barrel_ec == -2) {
        if (phi_module%2 == 0) {
          phi_index = 2*rowsPerFE-phi_index-1;
        }
      }
      if (barrel_ec==0 && layer_disk==1){
        rowsPerFE = rowsPerFEI52;
        columnsPerFE = columnsPerFEI52;
      } 
      if (phi_index >= rowsPerFE) {
        circ = (int)((FEIXsPerHalfModule-1)-(eta_index/columnsPerFE));
      } else {
        circ = (int)(eta_index/columnsPerFE)+FEIXsPerHalfModule;
      }
    }
*/

    circ = (int)eta_index/p_design->columnsPerCircuit();
  }
  else {
    int columnsPerFE = p_design->columnsPerCircuit();
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      circ = p_design->numberOfCircuits()==1 ? 0: eta_index/columnsPerFE;
    }
    else{ // FEI3 chips
      if (barrel_ec==2 || barrel_ec==-2) {
        if (phi_module%2 == 0) {
          phi_index = p_design->rowsPerCircuit()-phi_index+7;
        }
      }
      if (phi_index>=p_design->rowsPerCircuit()/2+4) {
        circ = (int)((p_design->numberOfCircuits()-1)-(eta_index/columnsPerFE));
      } 
      else {
        circ = (int)(eta_index/columnsPerFE)+p_design->numberOfCircuits();
      }
    }
  }
  return circ;
}

// hasCalibData
bool PixelCalibSvc::hasCalibData(const Identifier& wafer_id) const
{
  if (m_disableDb) return false;
  if(wafer_id !=m_wafer_id){
    m_pat = m_dbTool->getCalibPtr(wafer_id);
    m_wafer_id = wafer_id;
  }
  if(m_pat)
    return m_pat->size()>0; 
  return false;
}

// get Number of FE chips per module 
int PixelCalibSvc::getNFE(const Identifier& wafer_id) const {
  if(hasCalibData(wafer_id))return m_pat->size(); 
  else {return 0;}
}

int PixelCalibSvc::gangedType(int row, int col, int columnsPerFE, int rowsFGanged, int rowsLGanged) const {  
  if (col>0 && col<columnsPerFE-1) {
    if (row>=rowsFGanged-1 && row<=rowsLGanged) {
      return (row-rowsFGanged+1)%2+1; // 1 inter ganged pixel; 2 ganged pixel
    }
    else {
      return 0; 
    }
  } 
  else if (col==0 || col==columnsPerFE-1) {
    if (row>=rowsFGanged-1) {
      return 2; // treat long ganged (3) as ganged 
    }
    else {
      return 1; // long
    }
  }
  else {
    ATH_MSG_WARNING("Pixel Type : the col number should be 0-" << columnsPerFE << ", not " <<col);
    return 0; // treat it as normal
  }
}

// getThreshold 
int PixelCalibSvc::getThreshold(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThreshold(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL;
  }
}
  
// getThreshold 
int PixelCalibSvc::getThreshold(const Identifier& pix_id) const {
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  if (m_disableDb) { return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL; }
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThreshold(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL;
  }
}

// getThresholdSigma
int PixelCalibSvc::getThresholdSigma(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_discrThreshSigma; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThresholdSigma(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_discrThreshSigma;
  }
}

// getThresholdSigma
int PixelCalibSvc::getThresholdSigma(const Identifier& pix_id) const {
  if (m_disableDb) { return m_discrThreshSigma; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getThresholdSigma(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_discrThreshSigma;
  }
}
  

// getNoise
int PixelCalibSvc::getNoise(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_noiseThresh; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getNoise(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_noiseThresh;
  }
}

// getNoise
int PixelCalibSvc::getNoise(const Identifier& pix_id) const {
  if (m_disableDb) { return m_noiseThresh; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getNoise(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_noiseThresh;
  }
}
  
// getTimeWalk 
int PixelCalibSvc::getTimeWalk(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTimeWalk(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL;
  }
}

// getTimeWalk 
int PixelCalibSvc::getTimeWalk(const Identifier& pix_id) const {
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  if (m_disableDb) { return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL; }
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTimeWalk(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL;
  }
}
  
//getQ2TotA 
float PixelCalibSvc::getQ2TotA(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_totparA; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotA(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparA;
  }
}

//getQ2TotA 
float PixelCalibSvc::getQ2TotA(const Identifier& pix_id) const {
  if (m_disableDb) { return m_totparA; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotA(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparA;
  }
}

//getQ2TotB
float PixelCalibSvc::getQ2TotE(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_totparE; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotE(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparE;
  }
}

//getQ2TotB
float PixelCalibSvc::getQ2TotE(const Identifier& pix_id) const {
  if (m_disableDb) { return m_totparE; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotE(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparE;
  }
}
  
//getQ2TotC
float PixelCalibSvc::getQ2TotC(const Identifier& wafer_id, int irow, int icol, int circ) const {
  if (m_disableDb) { return m_totparC; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  int type = PixelType(wafer_id,irow,icol); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotC(type);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparC;
  }
}

//getQ2TotC
float PixelCalibSvc::getQ2TotC(const Identifier& pix_id) const {
  if (m_disableDb) { return m_totparE; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2TotC(type); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparC;
  }
}

//getTotP1
float PixelCalibSvc::getTotP1(const Identifier& wafer_id, int circ) const {
  if (m_disableDb) { return m_totparP1; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotP1();
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP1;
  }
}

//getTotP1
float PixelCalibSvc::getTotP1(const Identifier& pix_id) const {
  if (m_disableDb) { return m_totparP1; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = PixelCirc(pix_id, wafer_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotP1(); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP1;
  }
}

//getTotP2
float PixelCalibSvc::getTotP2(const Identifier& wafer_id, int circ) const {
  if (m_disableDb) { return m_totparP2; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotP2();
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP2;
  }
}

//getTotP2
float PixelCalibSvc::getTotP2(const Identifier& pix_id) const {
  if (m_disableDb) { return m_totparP2; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = PixelCirc(pix_id, wafer_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotP2(); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP2;
  }
}

//getTotRes
float PixelCalibSvc::getTotRes(const Identifier& wafer_id, int circ, float Q) const {
  if (m_disableDb) { return m_totparP1+m_totparP2*Q; }
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  if (m_dbTool->getCalibPtr(wafer_id) && circ<p_design->numberOfCircuits()) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotRes(Q);
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP1+m_totparP2*Q;
  }
}

//getTotRes
float PixelCalibSvc::getTotRes(const Identifier& pix_id, float Q) const {
  if (m_disableDb) { return m_totparP1+m_totparP2*Q; }
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = PixelCirc(pix_id, wafer_id);
  if (m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    return m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getTotRes(Q); 
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    return m_totparP1+m_totparP2*Q;
  }
}

//getTotMean
float PixelCalibSvc::getTotMean(const Identifier& pix_id, float Q) const {

  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  float ToT = 0.0;
  if (!m_disableDb && m_dbTool->getCalibPtr(wafer_id) && circ>-1) { 
    if (m_specialIBL_correction) {
      //===============================================================================================================
      // Special IBL calibration
      // const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
      // const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
      if (m_pixid->barrel_ec(pix_id)==0 && m_pixid->layer_disk(pix_id)==0) {  // IBL
        double scaleC = m_specialIBL_chargescale;
        double corrQ = scaleC*1.11*(1.0-(-7.09*1000.0)/(23.72*1000.0+Q)+(-0.22*1000.0)/(-0.42*1000.0+Q));
        if (corrQ<1.0) { corrQ = 1.0; }
        ToT = m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2Tot(type,Q/corrQ); 
      }
      else {
        ToT = m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2Tot(type,Q); 
      }
      //===============================================================================================================
    }
    else {
      ToT = m_dbTool->getCalibPtr(wafer_id)->getPixelChipSummaryData(circ)->getQ2Tot(type,Q); 
    }
  }
  else {
    ATH_MSG_WARNING("Condition DB is not available. Use hardcoded value.");
    ToT = m_totparA*(m_totparE+Q)/(m_totparC+Q);
  }

/*
  // In Pixel Digitization the DBM is separately treated as the below constant ToT formation.
  // This should be revisited. 
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  if (m_pixid->is_dbm(wafer_id)) {  // DBM uses constant so far.
    ToT = 8.0*(Q-1200.0)/(8000.0-1200.0);
  }
  else {
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (ToT>=m_offlineCalibSvc->getIBLToToverflow()) { ToT=m_offlineCalibSvc->getIBLToToverflow(); }
    }
  }
*/

  return ToT;
}

//getCharge
float PixelCalibSvc::getCharge(const Identifier& pix_id, float ToT) const {

  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  const InDetDD::SiDetectorElement *element = m_detManager->getDetectorElement(wafer_id);
  const InDetDD::PixelModuleDesign *p_design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
  float charge = 0.0;
  if (m_pixid->is_dbm(wafer_id)) {
    charge = ToT/8.0*(8000.0-1200.0)+1200.0;
  }
  else {
    if (p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
      if (ToT>=m_offlineCalibSvc->getIBLToToverflow()) { ToT=m_offlineCalibSvc->getIBLToToverflow(); }
    }
    float termA = getQ2TotA(pix_id);
    float termE = getQ2TotE(pix_id);
    float termC = getQ2TotC(pix_id);
    if (termA!=0.0 && ToT/termA!=1.0) {
      charge = (termC*ToT/termA-termE)/(1.0-ToT/termA);
    }
  }
  return charge;
}

