/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCellHVCorr.h" 
#include "CaloEvent/CaloCell.h"


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

  ATH_CHECK( m_hvCorrTool.retrieve() );

  ATH_CHECK( m_cablingKey.initialize() );

  // if (m_undoHVonline) {
  //   sc = detStore()->regHandle(m_dd_HVScaleCorr,m_keyHVScaleCorr);
  //   if (sc.isFailure()) {
  //     msg(MSG::ERROR) << "Unable to register handle to HVScaleCorr " << endmsg;
  //     return StatusCode::FAILURE;
  //   }
  // }

  ATH_CHECK(detStore()->regFcn(&ILArHVCorrTool::LoadCalibration,&(*m_hvCorrTool),&ILArCellHVCorrTool::LoadCalibration,(ILArCellHVCorrTool*)this));

  ATH_MSG_INFO( "Registered callback on ILArHVCorrTool"  );

  ATH_CHECK(detStore()->regFcn(&LArCellHVCorr::LoadCalibration,this,m_dd_HVScaleCorr,m_keyHVScaleCorr));

  ATH_MSG_INFO( "Registered callback on DataHandle<ILArHVScaleCorr>"  );

  return StatusCode::SUCCESS;
}


StatusCode  LArCellHVCorr::LoadCalibration(IOVSVC_CALLBACK_ARGS) {
  //Dummy callback method to forward callback on HV update to CaloNoiseTool
  ATH_MSG_DEBUG( "LArCellHVCorr::LoadCalibration callback invoked"  );
  m_updateOnLastCallback=m_hvCorrTool->updateOnLastCallback();
  if (!m_updateOnLastCallback) 
    ATH_MSG_DEBUG( "No real HV change, chaches remain valid"  );
  return StatusCode::SUCCESS;
}

float LArCellHVCorr::getCorrection(const Identifier id)
{
  return const_cast<const LArCellHVCorr*>(this)->getCorrection (id);
}

float LArCellHVCorr::getCorrection(const Identifier id) const
{
  // this is highly ineffective, but this tool will be soon decommissioned, so could live with this for some time...
  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling){
      ATH_MSG_WARNING("Do not have mapping object " << m_cablingKey.key() << " returning 1.");
      return 1.;
  }
 float hvcorr = m_hvCorrTool->Scale(cabling->createSignalChannelID(id));

 if (m_undoHVonline) {
    float hvonline = m_dd_HVScaleCorr->HVScaleCorr(cabling->createSignalChannelID(id));
    if (hvonline>0. && hvonline<100.) hvcorr = hvcorr/hvonline;
  }

 if (hvcorr<0.9 ) {
   if (hvcorr<0.4) ATH_MSG_WARNING( "HV corr for cell with id " << id.get_identifier32().get_compact() << " = " << hvcorr  );
   else ATH_MSG_DEBUG( "HV corr for cell with id " << id.get_identifier32().get_compact() << " = " << hvcorr  );
 }


 //hvcorr might be zero in case of problems with the DCS database
 if (hvcorr<0.01) hvcorr=1.0;
 
 return hvcorr;
}


void LArCellHVCorr::MakeCorrection (CaloCell* theCell,
                                    const EventContext& /*ctx*/) const
{
  const Identifier id=theCell->ID();
  const float hvcorr=getCorrection(id);
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
