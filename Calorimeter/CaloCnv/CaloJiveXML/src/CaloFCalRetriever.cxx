/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloFCalRetriever.h"

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
  CaloFCalRetriever::CaloFCalRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("FCAL"),
    m_larCablingSvc("LArCablingService")
  {

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 

    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("FCallCellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveFCal" , m_fcal = true);
    declareProperty("DoBadFCal",     m_doBadFCal = false);
    declareProperty("DoFCalCellDetails",  m_doFCalCellDetails = false); 
    declareProperty("CellConditionCut", m_cellConditionCut = false);
    declareProperty("LArChannelsToIgnoreM5",  m_LArChannelsToIgnoreM5);
    declareProperty("DoMaskLArChannelsM5", m_doMaskLArChannelsM5 = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
    declareProperty("CellTimePrec", m_cellTimePrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloFCalRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endmsg;

    return StatusCode::SUCCESS;	
  }
   
  /**
   * FCal data retrieval from default collection
   */
  StatusCode CaloFCalRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Could not retrieve Calorimeter Cells " << endmsg;
	return StatusCode::FAILURE;
      }

   if(m_fcal){
      DataMap data = getFCalData(cellContainer);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data).isFailure()){
        return StatusCode::FAILURE;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "FCal retrieved" << endmsg;
      }
    }
    //FCal cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve FCal cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloFCalRetriever::getFCalData(const CaloCellContainer* cellContainer) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getFCalData()" << endmsg;

    DataMap DataMap;

    DataVect x; x.reserve(cellContainer->size());
    DataVect y; y.reserve(cellContainer->size());
    DataVect dx; dx.reserve(cellContainer->size());
    DataVect dy; dy.reserve(cellContainer->size());

    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slot; slot.reserve(cellContainer->size());
    DataVect sub; sub.reserve(cellContainer->size());

    DataVect cellTimeVec; cellTimeVec.reserve(cellContainer->size());
    DataVect cellGain; cellGain.reserve(cellContainer->size());
    DataVect cellPedestal; cellPedestal.reserve(cellContainer->size());
    DataVect adc2Mev; adc2Mev.reserve(cellContainer->size());
    DataVect BadCell; BadCell.reserve(cellContainer->size());

    char rndStr[30]; // for rounding (3 digit precision)

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LARFCAL);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LARFCAL);

    
    if(m_larCablingSvc.retrieve().isFailure())
      ATH_MSG_ERROR ("Could not retrieve LArCablingService");

    const ILArPedestal* larPedestal = nullptr;
    if(m_doFCalCellDetails){
	if( detStore()->retrieve(larPedestal).isFailure() ){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getFCalData(), Could not retrieve LAr Pedestal" << endmsg;
	}
      }
      
    const LArOnlineID* onlineId;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getFCalData(),Could not get LArOnlineID!" << endmsg;
     }
    
      IAlgTool* algtool;
      ILArADC2MeVTool* adc2mevTool=0;
      if(m_doFCalCellDetails){
	if( toolSvc()->retrieveTool("LArADC2MeVTool", algtool).isFailure()){
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getFCalData(), Could not retrieve LAr ADC2MeV Tool" <<endmsg;
	} else {
	  adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
	}
      }

      double energyGeV, xmm, ymm, dxmm, dymm, cellTime;
      double energyAllLArFcal = 0.;

      for(;it1!=it2;it1++){

      if((*it1)->badcell()) BadCell.push_back(1);
      else if((*it1)->energy()>= m_cellThreshold) BadCell.push_back(0);
      else BadCell.push_back(-1);

	  if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for FCal
	  Identifier cellid = (*it1)->ID(); 

          HWIdentifier LArhwid = m_larCablingSvc->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	  
	  //ignore FCal cells that are to be masked
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
          energyAllLArFcal += energyGeV;
          idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
        
	  xmm = (*it1)->x()*0.1;
	  ymm = (*it1)->y()*0.1;
	  x.push_back(DataType( gcvt( xmm, 4, rndStr)  ));
	  y.push_back(DataType( gcvt( ymm, 4, rndStr)  ));
	  
          channel.push_back(DataType(onlineId->channel(LArhwid))); 
          feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
       	  slot.push_back(DataType(onlineId->slot(LArhwid))); 

	  if ( m_doFCalCellDetails){
	    cellTime = (*it1)->time();
	    cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
	    cellGain.push_back(DataType( (*it1)->gain() ) ); 
      	
	    int fcalgain = (*it1)->gain();
	    float pedestal=larPedestal->pedestal(cellid,fcalgain);
	    float pedvalue=0;
	    if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
	    else pedvalue = 0;
	    cellPedestal.push_back(DataType(pedvalue));
	         
            if ( adc2mevTool ){
	       const std::vector<float>* polynom_adc2mev = &(adc2mevTool->ADC2MEV(cellid,fcalgain));
	       if (polynom_adc2mev->size()==0){ adc2Mev.push_back(DataType(-1)); }
 	       else{ adc2Mev.push_back(DataType((*polynom_adc2mev)[1])); }
            }else{
               adc2Mev.push_back(DataType(-1)); // write placeholder
	       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) 
 		<< "LArADC2MeVTool dynamic cast failed" << endmsg;
	    }
	  }

	  const CaloDetDescrElement* elt = (*it1)->caloDDE();

	  dxmm = elt->dx()*0.1;
	  dymm = elt->dy()*0.1;
	  dx.push_back(DataType( gcvt( dxmm, 4, rndStr)  ));
	  dy.push_back(DataType( gcvt( dymm, 4, rndStr)  ));
	    
	  if(m_calocell_id->pos_neg(cellid)==2)
	    sub.push_back(DataType(1));
	  else
	    sub.push_back(DataType(0));
      }

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Total energy in FCAL (LAr) in GeV : " <<  energyAllLArFcal << endmsg;

    // write values into DataMap
    DataMap["x"] = x;
    DataMap["y"] = y;
    DataMap["dx"] = dx;
    DataMap["dy"] = dy;
    DataMap["energy"] = energy;
    DataMap["id"] = idVec;
    DataMap["channel"] = channel;
    DataMap["feedThrough"] = feedThrough;
    DataMap["slot"] = slot;
    //Bad Cells
    if (m_doBadFCal==true) {
      DataMap["BadCell"] = BadCell;
    }    DataMap["sub"] = sub;

    // adc counts
    if ( m_doFCalCellDetails){
       DataMap["cellTime"] = cellTimeVec;
       DataMap["cellGain"] = cellGain;
       DataMap["cellPedestal"] = cellPedestal;
       DataMap["adc2Mev"] = adc2Mev;
    }
    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << x.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getFCalData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
