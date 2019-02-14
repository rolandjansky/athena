/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArConditionsTest/TestLArConditionsTools.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloEvent/CaloCellContainer.h"

#include "LArElecCalib/ILArOFCTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArAutoCorrTotalTool.h"

TestLArConditionsTools::TestLArConditionsTools(const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name,pSvcLocator),
  m_ofcTool("LArOFCTool"),
  m_acTotalTool("LArAutoCorrTotalTool"),
  m_acNoiseTool("LArAutoCorrNoiseTool"),
  m_adc2MeVTool("LArADC2MeVTool"),
  m_noiseTool("CaloNoiseTool/calonoisetool"),
  m_roiMap("LArRoI_Map"),
  m_idHelper(0)
  { 
    declareProperty ("RetrieveToolInInit", m_retrieveInInit=true);
    //std::cout<<" in TestLArConditionsTools c-tor"<<std::endl;
  }

TestLArConditionsTools::~TestLArConditionsTools()
{ 
  //std::cout<<" in TestLArConditionsTools d-tor"<<std::endl;
} 

StatusCode TestLArConditionsTools::retrieveTools() {
  ATH_CHECK( m_noiseTool.retrieve() );

  ATH_MSG_DEBUG(" CaloNoiseTool typename "<<m_noiseTool.typeAndName());

  ATH_CHECK( m_acNoiseTool.retrieve() );
  ATH_CHECK( m_adc2MeVTool.retrieve() );
  ATH_CHECK( m_acTotalTool.retrieve() );
  ATH_CHECK( m_ofcTool.retrieve() );
  ATH_CHECK( m_roiMap.retrieve() );
  return StatusCode::SUCCESS;
}


StatusCode TestLArConditionsTools::initialize() {

  ATH_MSG_DEBUG ( " in initialize  ");

  ATH_CHECK( retrieveTools() );
  ATH_CHECK( detStore()->retrieve( m_idHelper) );
  ATH_CHECK( m_cablingKey.initialize() );

  /*
  log<<MSG::INFO<<" testing LArRoI_Map in init "<<endmsg;
  sc = testLArRoI_Map(); 
  if(!sc.isSuccess()) {
    log<<MSG::ERROR<<" failed LArRoI_Map test "<<endmsg;
  }

  */

  ATH_MSG_INFO ( " done initialize " );
  return StatusCode::SUCCESS;
}

StatusCode TestLArConditionsTools::testCaloCellNoise() {
  ATH_MSG_INFO ( " in testCaloCellNoise" );

  const CaloCellContainer* cont = nullptr;
  ATH_CHECK( evtStore()->retrieve(cont,"AllCalo") );

  CaloCellContainer::const_iterator cell_it = cont->begin();
  CaloCellContainer::const_iterator cell_it_e = cont->end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      const CaloCell* cell = *cell_it;
      Identifier id = cell->ID() ;
      const CaloDetDescrElement* dde=cell->caloDDE();

      if(dde==0) {
	ATH_MSG_ERROR ( " DDE null " <<  m_idHelper->print_to_string(id));
	continue;
      }

      ATH_MSG_DEBUG( " channel " << m_idHelper->print_to_string(id));

      float f2 = m_noiseTool->elecNoiseRMSHighestGain(dde);
      float f1 = m_noiseTool->elecNoiseRMS(cell);
      // float f2 = m_noiseTool->elecNoiseRMSHighestGain(dde,1);
      // float f1 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,-1);
      //      float f2 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,1);
      // float f2 = 0;

      /*
      std::vector<float> totalNoises = m_noiseTool->elecNoiseRMS3gains(dde,1);
      if(totalNoises.size()==0){
	log << MSG::ERROR << " fail to get noise for this channel" << 
	  m_idHelper->print_to_string(id)<<endmsg;
      }
      */
      ATH_MSG_DEBUG( " channel " << m_idHelper->print_to_string(id)
                     <<" noise "<<f1<<" "<<f2);

      ++n;
    }

    ATH_MSG_INFO(" number of cells tested "<<n);
    return StatusCode::SUCCESS;

}

StatusCode TestLArConditionsTools::testCaloNoiseDDE() {

    const CaloDetDescrManager*  dd_man = nullptr;
    ATH_CHECK( detStore()->retrieve( dd_man) );
    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    const LArOnOffIdMapping* cabling = *cablingHdl;

    std::vector<Identifier>::const_iterator cell_it  = m_idHelper->cell_begin();
    std::vector<Identifier>::const_iterator cell_it_e  = m_idHelper->cell_end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      Identifier id = *cell_it;
      bool isLAr = m_idHelper->is_lar(id);
      const CaloDetDescrElement* dde=dd_man->get_element(id); 
      if(dde==0) {
	ATH_MSG_ERROR ( " DDE null " <<  m_idHelper->print_to_string(id));
	continue;
      }

      ATH_MSG_DEBUG( " channel " << m_idHelper->print_to_string(id));

      float f1 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,-1);
      float f2 = m_noiseTool->elecNoiseRMS(dde,CaloGain::LARHIGHGAIN,1);

      std::vector<float> totalNoises = m_noiseTool->elecNoiseRMS3gains(dde,1);
      if(totalNoises.size()==0){
	ATH_MSG_ERROR ( " fail to get noise for this channel" << 
                        m_idHelper->print_to_string(id));
      }

      ATH_MSG_DEBUG( " noise =  " << f1<<" "<<f2);


      if(isLAr){
        ILArOFCTool::OFCRef_t ofc_a = m_ofcTool->OFC_a(id,0); 
	if(ofc_a.size()==0){
	  ATH_MSG_ERROR ( " fail to get OFC " << 
                          m_idHelper->print_to_string(id));
      }
	
	const std::vector<float>& adc2mev  =  m_adc2MeVTool->ADC2MEV(id,0); 
	if(adc2mev.size()==0){
          ATH_MSG_ERROR ( " fail to get ADC2MEV " << 
                          m_idHelper->print_to_string(id));
	}
	
	const std::vector<double> acTotal  =  m_acTotalTool->autoCorrTotal(cabling->createSignalChannelID(id),0); 
	if(acTotal.size()==0){
	  ATH_MSG_ERROR ( " fail to get AutoCorrTotal " << 
                          m_idHelper->print_to_string(id));
	}
      }

      ++n;

    }

    ATH_MSG_INFO(" number of cells tested "<<n);
    return StatusCode::SUCCESS;

}

StatusCode TestLArConditionsTools::testLArRoI_Map() {

    std::vector<Identifier>::const_iterator cell_it  = m_idHelper->cell_begin();
    std::vector<Identifier>::const_iterator cell_it_e  = m_idHelper->cell_end();

    int n=0;
    for( ; cell_it!=cell_it_e ; ++cell_it){

      Identifier id = *cell_it;
      bool isLAr = m_idHelper->is_lar(id);
      int ttid = m_roiMap->TrigTowerID(id);
      ATH_MSG_DEBUG ( " n " << n
                      << " id " << id.get_compact()
                      << " isLAr " << isLAr
                      << " ttid " << ttid );
      ++n;
    }

    ATH_MSG_INFO(" number of cells tested for LArRoI_Map "<<n);
    return StatusCode::SUCCESS;

}
StatusCode TestLArConditionsTools::execute() {
  
  ATH_MSG_DEBUG(" in execute");
  //StatusCode sc = testCaloCellNoise(); 
  StatusCode sc = testCaloNoiseDDE(); 
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR(" failed CaloNoise Test");
  }


  sc = testLArRoI_Map(); 
  if(!sc.isSuccess()) {
    ATH_MSG_ERROR(" failed LArRoI_Map test ");
  }
  return StatusCode::SUCCESS;
}

StatusCode TestLArConditionsTools::finalize() {
  return StatusCode::SUCCESS;
}

