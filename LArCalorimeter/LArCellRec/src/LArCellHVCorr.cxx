/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellRec/LArCellHVCorr.h" 
//#include "GaudiKernel/MsgStream.h"
//#include "StoreGate/StoreGateSvc.h"
#include "CaloEvent/CaloCell.h"
//#include "LArRecUtils/LArHVCorrTool.h"


LArCellHVCorr::LArCellHVCorr (const std::string& type, 
				  const std::string& name, 
				  const IInterface* parent) :
  CaloCellCorrection(type, name, parent),
  m_hvCorrTool("LArHVCorrTool"),
  m_keyHVScaleCorr("LArHVScaleCorr"),
  m_undoHVonline(true),
  m_updateOnLastCallback(true)
{ 
  declareInterface<CaloCellCorrection>(this); 
  declareInterface<ILArCellHVCorrTool>(this);
  declareProperty("HVCorrTool",m_hvCorrTool);
  declareProperty("keyHVScaleCorr",m_keyHVScaleCorr);
  declareProperty("undoHVOnline",m_undoHVonline);
}


LArCellHVCorr::~LArCellHVCorr() {}

/*
//The following is necessary b/c we are implementing a double interface
StatusCode LArCellHVCorr::queryInterface( const InterfaceID& riid, void** ppvIf ) {
  if (riid==ILArCellHVCorrTool::interfaceID()) {
    *ppvIf=(ILArCellHVCorrTool*)this;
    addRef();
    std::cout << "LArCellHVCorr queried as ILArCellHVCorrTool" << std::endl;
    return StatusCode::SUCCESS;
  }

  if (riid==CaloCellCorrection::interfaceID()) {
    *ppvIf=(CaloCellCorrection*)this;
    addRef();
    std::cout << "LArCellHVCorr queried as CaloCellCorrection" << std::endl;
    return StatusCode::SUCCESS;
  }
  std::cout << "LArCellHVCorr queried with unknown interface id" << std::endl;
  return AthAlgTool::queryInterface( riid, ppvIf );
}
*/


StatusCode LArCellHVCorr::initialize() {

  StatusCode  sc = m_hvCorrTool.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to find tool for LArHVCorrTool" << endreq; 
    return StatusCode::FAILURE;
  }

  // if (m_undoHVonline) {
  //   sc = detStore()->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr);
  //   if (sc.isFailure()) {
  //     msg(MSG::ERROR) << "Unable to register handle to HVScaleCorr " << endreq;
  //     return StatusCode::FAILURE;
  //   }
  // }

  ATH_CHECK(detStore()->regFcn(&ILArHVCorrTool::LoadCalibration,&(*m_hvCorrTool),&ILArCellHVCorrTool::LoadCalibration,(ILArCellHVCorrTool*)this));

  msg(MSG::INFO) << "Registered callback on ILArHVCorrTool" << endreq;

  ATH_CHECK(detStore()->regFcn(&LArCellHVCorr::LoadCalibration,this,m_dd_HVScaleCorr,m_keyHVScaleCorr));

  msg(MSG::INFO) << "Registered callback on DataHandle<ILArHVScaleCorr>" << endreq;

  return StatusCode::SUCCESS;
}


StatusCode  LArCellHVCorr::LoadCalibration(IOVSVC_CALLBACK_ARGS) {
  //Dummy callback method to forward callback on HV update to CaloNoiseTool
  msg(MSG::DEBUG) << "LArCellHVCorr::LoadCalibration callback invoked" << endreq;
  m_updateOnLastCallback=m_hvCorrTool->updateOnLastCallback();
  if (!m_updateOnLastCallback) 
    msg(MSG::DEBUG) << "No real HV change, chaches remain valid" << endreq;
  return StatusCode::SUCCESS;
}

float LArCellHVCorr::getCorrection(const Identifier id) {

 float hvcorr = m_hvCorrTool->Scale(id);

 if (m_undoHVonline) {
    float hvonline = m_dd_HVScaleCorr->HVScaleCorr(id);
    if (hvonline>0. && hvonline<100.) hvcorr = hvcorr/hvonline;
  }

 if (hvcorr<0.9) {
   msg(MSG::WARNING) << "HV corr for cell with id " << id.get_identifier32().get_compact() << " = " << hvcorr << endreq; 
 }


 //hvcorr might be zero in case of problems with the DCS database
 if (hvcorr<0.01) hvcorr=1.0;
 
 return hvcorr;
}


void LArCellHVCorr::MakeCorrection(CaloCell* theCell) {
  
  const float hvcorr=getCorrection(theCell->ID());
  theCell->setEnergy(theCell->energy()*hvcorr);

}  

  /*
  Identifier id = theCell->ID();
  float hvcorr = m_hvCorrTool->Scale(id);

  if (m_undoHVonline) {
    float hvonline = m_dd_HVScaleCorr->HVScaleCorr(id);
    if (hvonline>0. && hvonline<100.) hvcorr = hvcorr/hvonline;
  }

  if (hvcorr>0.01) {
    //hvcorr might be zero in case of problems with the DCS database
    theCell->setEnergy(theCell->energy()*hvcorr);
  }
  // else
  //   msg(MSG::WARNING) << "HVScaleCorr is zero for cell with id 0x" 
  // 		      << std::hex << theCell->ID().get_identifier32().get_compact() << std::dec 
  // 		      << " Correction NOT applied" << std::endl;
  return;
  
}
  */
