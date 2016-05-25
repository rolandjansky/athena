/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IToolSvc.h"

#include "StoreGate/DataHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloDM_ID.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloIdentifier/LArID_Exception.h"


#include "Identifier/Range.h" 
#include "Identifier/IdentifierHash.h"
#include "Identifier/HWIdentifier.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "LArIdentifier/LArElectrodeID.h"

#include "LArTools/LArCablingService.h"
#include "LArTools/LArHVCablingTool.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetectorElements.h"
#include "LArReadoutGeometry/EMBCell.h"
#include "LArHV/EMBHVElectrodeConstLink.h"
#include "LArHV/EMBHVElectrode.h"
#include "LArHV/EMBPresamplerHVModuleConstLink.h"
#include "LArHV/EMBPresamplerHVModule.h"
#include "LArReadoutGeometry/EMECCell.h"
#include "LArHV/EMECHVElectrodeConstLink.h"
#include "LArHV/EMECHVElectrode.h"
#include "LArHV/EMECPresamplerHVModuleConstLink.h"
#include "LArHV/EMECPresamplerHVModule.h"
#include "LArReadoutGeometry/HECCell.h"
#include "LArHV/HECHVSubgapConstLink.h"
#include "LArHV/HECHVSubgap.h"
#include "LArReadoutGeometry/FCALTile.h"
#include "LArHV/FCALHVLineConstLink.h"
#include "LArHV/FCALHVLine.h"
#include "LArHV/LArHVManager.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

///////////////////////////////////////////////////////////////////

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"


#include "LArMonitoring/LArHVMapTool.h"
#include "LArMonitoring/LArTesting.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <iostream>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor


LArTesting::LArTesting(const std::string& name,
			 ISvcLocator* pSvcLocator) 
  : AthAlgorithm(name, pSvcLocator),
    m_LArCablingService("LArCablingService"),
    m_LArHVCablingTool("LArHVCablingTool"),
    m_LArHVMapTool("LArHVMapTool/LArHVMapTool"),
    m_LArOnlineIDHelper(0),
    m_LArHVLineIDHelper(0),
    m_LArElectrodeIDHelper(0),
    m_LArEM_IDHelper(0),
    m_LArFCAL_IDHelper(0),
    m_LArHEC_IDHelper(0),
    m_caloIdMgr(0),
    m_calodetdescrmgr(0)
 {

   /** switches to control the analysis through job options */
   declareProperty("LArCablingService", m_LArCablingService);
   declareProperty("LArHVCablingTool",  m_LArHVCablingTool);
   
   // NEW
   declareProperty( "HVTool", m_LArHVMapTool );

 }

LArTesting::~LArTesting() {}


StatusCode LArTesting::initialize() {

  ATH_MSG_DEBUG ( "Initializing LArTesting" );
 
  ATH_CHECK( m_LArCablingService.retrieve() );

  ATH_CHECK( detStore()->retrieve(m_calodetdescrmgr) );

  // Retrieve online ID helper
  const DataHandle<LArOnlineID> LArOnlineIDHelper;
  ATH_CHECK( detStore()->retrieve(LArOnlineIDHelper, "LArOnlineID") );
  m_LArOnlineIDHelper = LArOnlineIDHelper;
  ATH_MSG_DEBUG( " Found LArOnline Helper");

  // Retrieve HV line ID helper
  const DataHandle<LArHVLineID> LArHVLineIDHelper;
  ATH_CHECK( detStore()->retrieve(LArHVLineIDHelper, "LArHVLineID") );
  m_LArHVLineIDHelper = LArHVLineIDHelper;
  ATH_MSG_DEBUG( " Found LArOnlineIDHelper Helper");

  // Retrieve HV electrode ID helper
  const DataHandle<LArElectrodeID> LArElectrodeIDHelper;
  ATH_CHECK( detStore()->retrieve(LArElectrodeIDHelper, "LArElectrodeID") );
  m_LArElectrodeIDHelper = LArElectrodeIDHelper;
  ATH_MSG_DEBUG( " Found LArElectrodeIDHelper Helper");

  // Retrieve ID helpers
  ATH_CHECK( detStore()->retrieve(m_caloIdMgr) );
  m_LArEM_IDHelper   = m_caloIdMgr->getEM_ID();
  m_LArHEC_IDHelper  = m_caloIdMgr->getHEC_ID();
  m_LArFCAL_IDHelper = m_caloIdMgr->getFCAL_ID();
  
  if(m_LArHVMapTool.retrieve().isFailure()){
    ATH_MSG_WARNING ( "Failed to retrieve tool " << m_LArHVMapTool );
  }else{
    ATH_MSG_INFO ( "Retrieved tool " << m_LArHVMapTool );
  }


  ATH_MSG_DEBUG ( "End of Initializing LArTesting" );
 
  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

StatusCode LArTesting::finalize() {
  ATH_MSG_DEBUG ( "in finalize()" );
  return StatusCode::SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////
/// Execute - on event by event

StatusCode LArTesting::execute() {
  ATH_MSG_DEBUG ( "in execute()" );

  std::vector<HWIdentifier>::const_iterator ch_b = m_LArOnlineIDHelper->channel_begin();
  std::vector<HWIdentifier>::const_iterator ch_e = m_LArOnlineIDHelper->channel_end();

  for(; ch_b != ch_e; ++ch_b) {
    if (! m_LArCablingService->isOnlineConnected(*ch_b)) continue;

    Identifier id=m_LArCablingService->cnvToIdentifier(*ch_b);

    std::vector<int>* hv=GetHVLines(id);
    if(hv) {
      std::cout<<id.get_compact()<<" "<<hv->size()<<std::endl;
      for (unsigned i=0; i<hv->size(); ++i) std::cout<<hv->at(i)<<" ";
      std::cout<<std::endl;
    }
  }

  return StatusCode::SUCCESS;
}


std::vector<int>* LArTesting::GetHVLines(const Identifier& id)
{
  ATH_MSG_DEBUG ( "in GetHVLines function " );

  std::vector<int>  tmplines;
  unsigned int nelec = 0;
  //unsigned int ngap = 0;
  unsigned int nsubgaps = 0;
  unsigned int nlines = 0;
  unsigned int i=0,j=0,igap=0;

  // LAr EMB
  if(m_LArEM_IDHelper->is_lar_em(id) && m_LArEM_IDHelper->sampling(id)>0){
    ATH_MSG_DEBUG ( "LAr EMB");
    if(abs(m_LArEM_IDHelper->barrel_ec(id))==1) {
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement)
        return 0;
      const EMBCellConstLink cell = embElement->getEMBCell();
      nelec = cell->getNumElectrodes();
      //ngap = 2*nelec;
      for(i=0;i<nelec;i++) {
	      const EMBHVElectrodeConstLink electrode = cell->getElectrode(i);
	      for(igap=0;igap<2;igap++) tmplines.push_back(electrode->hvLineNo(igap));
      }        
    } else { // LAr EMEC
      ATH_MSG_DEBUG ( "LAr EMEC");
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement)
        return 0;
      const EMECCellConstLink cell = emecElement->getEMECCell();
      nelec = cell->getNumElectrodes();
      //ngap = 2*nelec;
      for(i=0;i<nelec;i++) {
	      const EMECHVElectrodeConstLink electrode = cell->getElectrode(i);
	      for(igap=0;igap<2;igap++) tmplines.push_back(electrode->hvLineNo(igap));
      }      
    }
  } else if(m_LArHEC_IDHelper->is_lar_hec(id)) { // LAr HEC
    ATH_MSG_DEBUG ( "LAr HEC");
    const HECDetectorElement* hecElement = dynamic_cast<const HECDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!hecElement)
      return 0;
    const HECCellConstLink cell = hecElement->getHECCell();
    nsubgaps = cell->getNumSubgaps();
    for(i=0;i<nsubgaps;i++) {
      const HECHVSubgapConstLink subgap = cell->getSubgap(i);
      tmplines.push_back(subgap->hvLineNo());
    }
  } else if(m_LArFCAL_IDHelper->is_lar_fcal(id)) { // LAr FCAL
    ATH_MSG_DEBUG ( "LAr HEC");
    const FCALDetectorElement* fcalElement = dynamic_cast<const FCALDetectorElement*>(m_calodetdescrmgr->get_element(id));
    if (!fcalElement)
      return 0;
    const FCALTile* tile = fcalElement->getFCALTile();
    nlines = tile->getNumHVLines();
    for(i=0;i<nlines;i++) {
      const FCALHVLineConstLink line = tile->getHVLine(i);
      if(line) tmplines.push_back(line->hvLineNo());
    }   
  } else if(m_LArEM_IDHelper->is_lar_em(id) && m_LArEM_IDHelper->sampling(id)==0) { // Presamplers
    ATH_MSG_DEBUG ( "LAr PRESAMPLES");
    
    if(abs(m_LArEM_IDHelper->barrel_ec(id))==1){
      
      const EMBDetectorElement* embElement = dynamic_cast<const EMBDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!embElement) {
        return 0;
      }
      const EMBCellConstLink cell = embElement->getEMBCell();
      const EMBPresamplerHVModuleConstLink hvmodule =  cell->getPresamplerHVModule();
      for(igap=0;igap<2;igap++) tmplines.push_back(hvmodule->hvLineNo(igap));

    } else {
      
      const EMECDetectorElement* emecElement = dynamic_cast<const EMECDetectorElement*>(m_calodetdescrmgr->get_element(id));
      if (!emecElement)
        return 0;
      const EMECCellConstLink cell = emecElement->getEMECCell();
      const EMECPresamplerHVModuleConstLink hvmodule = cell->getPresamplerHVModule();
      for(igap=0;igap<2;igap++) tmplines.push_back(hvmodule->hvLineNo(igap));

    }
  } else {
    ATH_MSG_WARNING ( " cell neither in EM nor HEC nor FCAL !!!!!  return empty HV " );
    ATH_MSG_WARNING ( id.get_identifier32().get_compact() );

    return (vector<int>*)0;
  }

  // optimization of storage: replace by std:set
  std::vector<int>* hvlines = new std::vector<int>;
  bool found = 0;
  for(i=0;i<tmplines.size();i++){
    found = 0;
    for(j=0;j<hvlines->size();j++){
      if(hvlines->at(j)==tmplines.at(i)){
        found=1;
        break;
      }
    }
    if(!found) hvlines->push_back(tmplines.at(i));
  }

  return hvlines;
}


 
