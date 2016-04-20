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
#include "InDetIdentifier/PixelID.h"
//#include "PixelCabling/IPixelCablingSvc.h"
#include "PixelConditionsTools/IPixelCalibDbTool.h"

static unsigned int columnsPerFEI3 = 18;   // number of columns per FEI3 (18x160)
static unsigned int rowsPerFEI3    = 164;   // number of rows per FEI3
static unsigned int rowsFGangedFEI3  =153;   // first ganged pixel row for FEI3
static unsigned int rowsLGangedFEI3  =159;   // last ganged pixel row for FEI3

static unsigned int columnsPerFEI4 = 80;   // number of columns per FEI4 (80x336)
static unsigned int rowsPerFEI4 = 336; // number of rows per FEI4

static unsigned int columnsPerFEI50 = 132; // number of columns per FEI5-type 0 (132x672) for barrel layer 0
static unsigned int rowsPerFEI50 = 672; // number of rows per FEI50

static unsigned int columnsPerFEI52 = 132; // number of columns per FEI5-type 2 (132x672) for barrel layer 1 
static unsigned int rowsPerFEI52 = 678; // number of rows per FEI52
static unsigned int rowsFGangedFEI52  =661;   // first ganged pixel row for FEI52
static unsigned int rowsLGangedFEI52  =671;   // last ganged pixel row for FEI52

static unsigned int columnsPerFEI51 = 80; // number of columns per FEI5-type 1 (80x336) for the rest 
static unsigned int rowsPerFEI51 = 339; // number of rows per FEI51
static unsigned int rowsFGangedFEI51  =331;   // first ganged pixel row for FEI51
static unsigned int rowsLGangedFEI51  =335;   // last ganged pixel row for FEI51

static bool isITK(false);

//static unsigned int rowsPerFEI4    = 336;   // number of rows per FEI4

//================ Constructor =================================================

PixelCalibSvc::PixelCalibSvc(const std::string& name, ISvcLocator* sl)
  : 
  AthService(name, sl),
  m_sgSvc("StoreGateSvc",name),	
  m_detStore("DetectorStore",name),
  m_dbTool("PixelCalibDbTool"),
  m_pixid(0),
  //m_pixidmap("PixelCablingSvc",name),
  m_pat(0),
  m_wafer_id(0),
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
  m_IBLParameterSvc("IBLParameterSvc",name)
{
  //  template for property decalration
  //declareProperty("PropertyName", m_propertyName);
  declareProperty("CalibrationDbTool",m_dbTool);
  //declareProperty("CablingService"   ,m_pixidmap);
  declareProperty("ToTParA",            m_totparA,              "TOT parameter A");
  declareProperty("ToTParE",            m_totparE,              "TOT parameter E");
  declareProperty("ToTParC",            m_totparC,              "TOT parameter C");
  declareProperty("ToTParP1",           m_totparP1,             "TOT smearing parameter p1");
  declareProperty("ToTParP2",           m_totparP2,             "TOT smearing parameter p2");
  declareProperty("DiscrThresh",        m_discrThresh,          "Discriminator threshold");
  declareProperty("DiscrThreshVar",     m_discrThreshSigma,     "Discriminator threshold sigma");
  declareProperty("IntimeThresh",       m_intimeThresh,         "Discriminator in-time threshold");
  declareProperty("NoiseThresh",        m_noiseThresh,          "Discriminator noise");
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

StatusCode PixelCalibSvc::initialize()
{

 // retrieve the StoreGate Service (delete if not needed)
 // sc= service("StoreGateSvc",m_sgSvc);
  if (m_sgSvc.retrieve().isFailure()){
    msg(MSG::FATAL) << "Could not retrieve StoreGateSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  else
    msg(MSG::INFO) << "StoreGateSvc retrieved!" << endreq;

  // locate the conditions store ptr to it.
  // sc = service("DetectorStore", m_detStore);

  if (m_detStore.retrieve().isFailure())  {
    msg(MSG::FATAL) <<"Could not find DetStore" <<endreq;
    return StatusCode::FAILURE;
  }

  // Get the geometry 
  if (StatusCode::SUCCESS!= m_detStore->retrieve(m_pixid,"PixelID") ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }
  // locate PixelID service
  if(m_pixid->wafer_hash_max()>3000)isITK = true;
  // locate PixelCalibTools  
  if(m_dbTool.retrieve().isFailure()){
    msg(MSG::FATAL)<< "Could not find tool "<< m_dbTool.name() << ". Existing."<<endreq;
    return StatusCode::FAILURE;
  } else {
    msg(MSG::INFO) <<" Database tool:"<< m_dbTool.name() << " retrieved."<<endreq;
  }

  /*
  StatusCode sc;
  sc = m_pixidmap.retrieve();
  if(sc.isFailure()){
    msg(MSG::FATAL) << " Can't get " << m_pixidmap.name() << endreq;
      return sc;
  }
  */
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
  msg(MSG::INFO)<<" PixelCalibSvc Finalizing " <<endreq; 
  return StatusCode::SUCCESS; 
}

//============================================================================

int PixelCalibSvc::PixelType(const Identifier& wafer_id, int row, int col) const
{
  // local row and col of ith circuit(0-15) 
  // each circuit contains 160 rows and 18 columns 
  if(isFEI4(wafer_id)){ 
    if(getNFE(wafer_id)>1){ // planar modules
      if(isITK){ 
	if(col==0||col==(static_cast<int>(columnsPerFEI50)-1))return 1; // long pixel
      }
      else{
	if(col==0||col==(static_cast<int>(columnsPerFEI4)-1))return 1; // long pixel
      }
    }
    return 0;
  }
  else{
    if(isITK){ 
      int iec = m_pixid->barrel_ec(wafer_id);
      int il = m_pixid->layer_disk(wafer_id); 
      if(iec==0&&il==1)return gangedType(row, col, columnsPerFEI52,rowsFGangedFEI52,rowsLGangedFEI52);
      else return gangedType(row, col, columnsPerFEI51,rowsFGangedFEI51,rowsLGangedFEI51);
    }
    else return gangedType(row, col, columnsPerFEI3,rowsFGangedFEI3,rowsLGangedFEI3);
  }
}

int PixelCalibSvc::PixelType(const Identifier& pix_id, const Identifier& wafer_id, int &circ) const
{
  int phi_index = m_pixid->phi_index(pix_id);
  int eta_index = m_pixid->eta_index(pix_id);
  int barrel_ec = m_pixid->barrel_ec(wafer_id);
  int layer_disk = m_pixid->layer_disk(wafer_id);
  int phi_module = m_pixid->phi_module(wafer_id);
  int FEIXsPerHalfModule = getNFE(wafer_id)/2;
  int col;
  int row;
  circ = -1;
  if(isFEI4(wafer_id)){ // FEI4 chips, need check ? 
    int columnsPerFE =isITK?columnsPerFEI50:columnsPerFEI4;
    col =  (columnsPerFE-1) -eta_index%columnsPerFE; // check the col order in FEI4 ?
    int rowsPerFE =isITK?rowsPerFEI50:rowsPerFEI4;
    row = (rowsPerFE-1)-phi_index; // check this ?
    circ = getNFE(wafer_id)==1 ? 0: 1-eta_index/columnsPerFE;
  }
  else{ // FEI3 chips, FEI51, FEI52
    int rowsPerFE =isITK?rowsPerFEI51:rowsPerFEI3; // check ?
    int columnsPerFE =isITK?columnsPerFEI51:columnsPerFEI3;
    if (barrel_ec == 2 || barrel_ec == -2) {
      if (phi_module%2 == 0) {
	phi_index = 2*rowsPerFE-phi_index-1;
      }
    }
    if(isITK&&barrel_ec==0&&layer_disk==1){
      rowsPerFE = rowsPerFEI52;
      columnsPerFE = columnsPerFEI52;
    } 
    if (phi_index >= rowsPerFE) {
      circ = (int)((FEIXsPerHalfModule-1)-(eta_index/columnsPerFE));
      col=(columnsPerFE-1)-(eta_index%columnsPerFE);
      row = ((2*rowsPerFE)-1)-phi_index;
    } else {
      circ = (int)(eta_index/columnsPerFEI3)+FEIXsPerHalfModule;
      col=eta_index%columnsPerFE;
      row=phi_index;
    }
  }
  return PixelType(wafer_id, row, col);
}

int PixelCalibSvc::PixelCirc(const Identifier& pix_id, const Identifier& wafer_id) const
{
  int phi_index = m_pixid->phi_index(pix_id);
  int eta_index = m_pixid->eta_index(pix_id);
  int barrel_ec = m_pixid->barrel_ec(wafer_id);
  int layer_disk = m_pixid->layer_disk(wafer_id);
  int phi_module = m_pixid->phi_module(wafer_id);
  int FEIXsPerHalfModule = getNFE(wafer_id)/2;
  int circ(-1);
  if(isFEI4(wafer_id)){ // FEI4 chips, need check ?
    int columnsPerFE =isITK?columnsPerFEI50:columnsPerFEI4;
    circ = getNFE(wafer_id)==1 ? 0 : 1-eta_index/columnsPerFE;
  }
  else{ // FEI3 chips or FEI51, FEI52
    int rowsPerFE =isITK?rowsPerFEI51:rowsPerFEI3; // check ?
    int columnsPerFE =isITK?columnsPerFEI51:columnsPerFEI3;
    if (barrel_ec == 2 || barrel_ec == -2) {
      if (phi_module%2 == 0) {
        phi_index = 2*rowsPerFE-phi_index-1;
      }
    }
    if(isITK&&barrel_ec==0&&layer_disk==1){
      rowsPerFE = rowsPerFEI52;
      columnsPerFE = columnsPerFEI52;
    } 
    if (phi_index >= rowsPerFE) {
      circ = (int)((FEIXsPerHalfModule-1)-(eta_index/columnsPerFE));
    } else {
      circ = (int)(eta_index/columnsPerFE)+FEIXsPerHalfModule;
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
int PixelCalibSvc::getNFE(const Identifier& wafer_id) const
{
  if(hasCalibData(wafer_id))return m_pat->size(); 
  else {return 0;}
}

int PixelCalibSvc::gangedType(int row, int col, const unsigned int& columnsPerFE, const unsigned int& rowsFGanged, const unsigned int& rowsLGanged) const
{  
  if (( col > 0) && ( col <(static_cast<int>(columnsPerFE)-1) )){
    if(row>=(static_cast<int>(rowsFGanged)-1)&&row<=static_cast<int>(rowsLGanged)){
      return (row-rowsFGanged+1)%2+1; // 1 inter ganged pixel; 2 ganged pixel
    }
    else return 0; 
  } 
  else if (col==0||col==(static_cast<int>(columnsPerFE)-1)){
    if (row >=(static_cast<int>(rowsFGanged)-1))
      return 2; // treat long ganged (3) as ganged 
    else
      return 1; // long
  }
  else {
    msg(MSG::WARNING)<<" Pixel Type : the col number should be 0-"<<columnsPerFE<<", not "<<col<<endreq;
    return 0; // treat it as normal
  }
}

// is FEI4 chip
bool PixelCalibSvc::isFEI4(const Identifier& wafer_id) const
{
   if(hasCalibData(wafer_id))return (m_pat->size())<3;
   else {return false;}
}

// getThreshold 
int PixelCalibSvc::getThreshold(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol);
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
  return m_pat->getPixelChipSummaryData(circ)->getThreshold(type);}
  else return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL;
}
  
// getThreshold 
int PixelCalibSvc::getThreshold(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getThreshold(type);}
  else return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_discrThresh : m_discrThreshIBL;
}


// getThresholdSigma
int PixelCalibSvc::getThresholdSigma(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getThresholdSigma(type);}
  else{ return m_discrThreshSigma;}
}

// getThresholdSigma
int PixelCalibSvc::getThresholdSigma(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id, wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getThresholdSigma(type);}
  else{ return m_discrThreshSigma;}
}
  

// getNoise
int PixelCalibSvc::getNoise(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getNoise(type);}
  else{return m_noiseThresh;}
}

// getNoise
int PixelCalibSvc::getNoise(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id, wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getNoise(type);}
  else{return m_noiseThresh;}
}

  
// getTimeWalk 
int PixelCalibSvc::getTimeWalk(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getTimeWalk(type);}
  else return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL;
}

// getTimeWalk 
int PixelCalibSvc::getTimeWalk(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id, wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getTimeWalk(type);}
  else return m_IBLabsent || m_pixid->layer_disk(wafer_id)>0 ? m_intimeThresh : m_intimeThreshIBL;
}

  
//getQ2TotA 
float PixelCalibSvc::getQ2TotA(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotA(type);}
  else{return m_totparA;}
}

//getQ2TotA 
float PixelCalibSvc::getQ2TotA(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id, wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotA(type);}
  else{return m_totparA;}
}

  
//getQ2TotB
float PixelCalibSvc::getQ2TotE(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotE(type);}
  else{return m_totparE;}
}

//getQ2TotB
float PixelCalibSvc::getQ2TotE(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotE(type);}
  else{return m_totparE;}
}
  
//getQ2TotC
float PixelCalibSvc::getQ2TotC(const Identifier& wafer_id, int irow, int icol, int circ) const
{
  int type = PixelType(wafer_id,irow,icol); 
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotC(type);}
  else{return m_totparC;}
}

//getQ2TotC
float PixelCalibSvc::getQ2TotC(const Identifier& pix_id) const
{

  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id, wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getQ2TotC(type);}
  else{return m_totparC;}
}

//getTotP1
float PixelCalibSvc::getTotP1(const Identifier& wafer_id, int circ) const
{
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getTotP1(); }
  else{return m_totparP1;}
}

//getTotP1
float PixelCalibSvc::getTotP1(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ = PixelCirc(pix_id, wafer_id);
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getTotP1();}
  else{return m_totparP1;}
}


//getTotP2
float PixelCalibSvc::getTotP2(const Identifier& wafer_id, int circ) const
{
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getTotP2(); }
  else{return m_totparP2;}
}

//getTotP2
float PixelCalibSvc::getTotP2(const Identifier& pix_id) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ =  PixelCirc(pix_id, wafer_id);
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getTotP2();}
  else{return m_totparP2;}
}


//getTotRes
float PixelCalibSvc::getTotRes(const Identifier& wafer_id, int circ, float Q) const
{
  if(hasCalibData(wafer_id)&&circ<getNFE(wafer_id)){
    return m_pat->getPixelChipSummaryData(circ)->getTotRes(Q);}
  else{ return m_totparP1+m_totparP2*Q;}
}

//getTotRes
float PixelCalibSvc::getTotRes(const Identifier& pix_id, float Q) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ =  PixelCirc(pix_id, wafer_id);
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getTotRes(Q);}
  else{return m_totparP1+m_totparP2*Q;}
}

//getTotMean
float PixelCalibSvc::getTotMean(const Identifier& pix_id, float Q) const
{
  Identifier wafer_id = m_pixid->wafer_id(pix_id);
  int circ;
  int type = PixelType(pix_id,wafer_id,circ); 
  if(hasCalibData(wafer_id)&&circ>-1){
    return m_pat->getPixelChipSummaryData(circ)->getQ2Tot(type,Q);}
  else{return m_totparA*(m_totparE+Q)/(m_totparC+Q);}
}
  

  
