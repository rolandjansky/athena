/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloHECRetriever.h"

#include "AthenaKernel/Units.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/HWIdentifier.h"
#include "LArCabling/LArCablingService.h"

using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CaloHECRetriever::CaloHECRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("HEC"),
    m_larCablingSvc("LArCablingService")
  {

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 

    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("HEClCellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveHEC" , m_hec = true);
    declareProperty("DoBadHEC",     m_doBadHEC = false);
    declareProperty("DoHECCellDetails",  m_doHECCellDetails = false); 
    declareProperty("CellConditionCut", m_cellConditionCut = false);
    declareProperty("LArChannelsToIgnoreM5",  m_LArChannelsToIgnoreM5);
    declareProperty("DoMaskLArChannelsM5", m_doMaskLArChannelsM5 = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
    declareProperty("CellTimePrec", m_cellTimePrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloHECRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endmsg;

    return StatusCode::SUCCESS;	
  }
   
  /**
   * HEC data retrieval from default collection
   */
  StatusCode CaloHECRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Could not retrieve Calorimeter Cells " << endmsg;
	return false;
      }

   if(m_hec){
      DataMap data = getHECData(cellContainer);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
        return false;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "HEC retrieved" << endmsg;
      }
    }
    //HEC cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve HEC cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloHECRetriever::getHECData(const CaloCellContainer* cellContainer) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getHECData()" << endmsg;

    DataMap DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slot; slot.reserve(cellContainer->size());

    DataVect cellTimeVec; cellTimeVec.reserve(cellContainer->size());
    DataVect cellGain; cellGain.reserve(cellContainer->size());
    DataVect cellPedestal; cellPedestal.reserve(cellContainer->size());
    DataVect adc2Mev; adc2Mev.reserve(cellContainer->size());
    DataVect BadCell; BadCell.reserve(cellContainer->size());

    char rndStr[30]; // for rounding (3 digit precision)

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LARHEC);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LARHEC);

    
    if(m_larCablingSvc.retrieve().isFailure())
      ATH_MSG_ERROR ("Could not retrieve LArCablingService");

    const ILArPedestal* larPedestal = nullptr;
    if(m_doHECCellDetails){
	if( detStore()->retrieve(larPedestal).isFailure() ){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(), Could not retrieve LAr Pedestal" << endmsg;
	}
      }
      
    const LArOnlineID* onlineId = nullptr;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(),Could not get LArOnlineID!" << endmsg;
     }

      IAlgTool* algtool;
      ILArADC2MeVTool* adc2mevTool=0;
      if(m_doHECCellDetails){
	if( toolSvc()->retrieveTool("LArADC2MeVTool", algtool).isFailure()){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getHECData(), Could not retrieve LAr ADC2MeV Tool" <<endmsg;
	} else {
	  adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
	}
      }

      double energyGeV,cellTime;	
      double energyAllLArHEC = 0.;      

      for(;it1!=it2;it1++){
        if ((*it1)->energy() < m_cellThreshold) continue; // skip to next cell if threshold not passed

	if((*it1)->badcell()){ BadCell.push_back(1); }
	else{ BadCell.push_back(-1); }

	  if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for HEC
	  Identifier cellid = (*it1)->ID(); 

          HWIdentifier LArhwid = m_larCablingSvc->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	  
	  //ignore HEC cells that are to be masked
	  if (m_doMaskLArChannelsM5){
	    bool maskChannel = false;
	    for (size_t i = 0; i < m_LArChannelsToIgnoreM5.size(); i++){
              if (cellid == m_LArChannelsToIgnoreM5[i]){
		maskChannel = true; 
		break;  // exit loop over bad channels
	      }	      
	    }
	    if (maskChannel) continue;  // continue loop over all channels
	  }

	  energyGeV = (*it1)->energy()*(1./GeV);
	  energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
    	  energyAllLArHEC += energyGeV;

          idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
          phi.push_back(DataType((*it1)->phi()));
          eta.push_back(DataType((*it1)->eta()));
          channel.push_back(DataType(onlineId->channel(LArhwid))); 
          feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
       	  slot.push_back(DataType(onlineId->slot(LArhwid))); 

	  if ( m_doHECCellDetails){
	    cellTime = (*it1)->time();
	    cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
	    cellGain.push_back(DataType( (*it1)->gain() ) ); 
      	
	    int hecgain = (*it1)->gain();
	    float pedestal=larPedestal->pedestal(cellid,hecgain);
	    float pedvalue=0;
	    if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
	    else pedvalue = 0;
	    cellPedestal.push_back(DataType(pedvalue));

            if (!adc2mevTool)
              adc2Mev.push_back(DataType(-1));
            else {
              const std::vector<float>* polynom_adc2mev = &(adc2mevTool->ADC2MEV(cellid,hecgain));
              if (polynom_adc2mev->size()==0){ adc2Mev.push_back(DataType(-1)); }
              else{ adc2Mev.push_back(DataType((*polynom_adc2mev)[1])); }
            }
	  }
      }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Total energy in HEC (LAr) in GeV : " <<  energyAllLArHEC << endmsg;

    // write values into DataMap
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["energy"] = energy;
    DataMap["id"] = idVec;
    DataMap["channel"] = channel;
    DataMap["feedThrough"] = feedThrough;
    DataMap["slot"] = slot;
    //Bad Cells
    if (m_doBadHEC==true) {
      DataMap["BadCell"] = BadCell;
    }
    // adc counts
    if ( m_doHECCellDetails){
       DataMap["cellTime"] = cellTimeVec;
       DataMap["cellGain"] = cellGain;
       DataMap["cellPedestal"] = cellPedestal;
       DataMap["adc2Mev"] = adc2Mev;
    }
    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " , collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getHECData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
