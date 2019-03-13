/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloLArRetriever.h"

#include "AthenaKernel/Units.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArADC2MeVTool.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArRawChannel.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "Identifier/HWIdentifier.h"

using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CaloLArRetriever::CaloLArRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("LAr"),
    m_calocell_id(nullptr)
  {

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_sgKey = "AllCalo"; 

    declareInterface<IDataRetriever>(this);
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("LArlCellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveLAr" , m_lar = true);
    declareProperty("DoBadLAr",     m_doBadLAr = false);
    declareProperty("DoLArCellDetails",  m_doLArCellDetails = false); 
    declareProperty("CellConditionCut", m_cellConditionCut = false);
    declareProperty("LArChannelsToIgnoreM5",  m_LArChannelsToIgnoreM5);
    declareProperty("DoMaskLArChannelsM5", m_doMaskLArChannelsM5 = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
    declareProperty("CellTimePrec", m_cellTimePrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloLArRetriever::initialize() {

    ATH_MSG_DEBUG( "Initialising Tool"  );
    ATH_CHECK( detStore()->retrieve (m_calocell_id, "CaloCell_ID") );

    ATH_CHECK( m_cablingKey.initialize() );

    return StatusCode::SUCCESS;	
  }
   
  /**
   * LAr data retrieval from default collection
   */
  StatusCode CaloLArRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    ATH_MSG_DEBUG( "in retrieve()"  );

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	ATH_MSG_WARNING( "Could not retrieve Calorimeter Cells "  );
	return StatusCode::FAILURE;
      }

   if(m_lar){
      DataMap data = getLArData(cellContainer);
      ATH_CHECK( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data) );
      ATH_MSG_DEBUG( "LAr retrieved"  );
    }
    //LAr cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve LAr cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloLArRetriever::getLArData(const CaloCellContainer* cellContainer) {
    
    ATH_MSG_DEBUG( "getLArData()"  );

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

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LAREM);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LAREM);

    
    const ILArPedestal* larPedestal = nullptr;
    if(m_doLArCellDetails){
      if( detStore()->retrieve(larPedestal).isFailure() ){
        ATH_MSG_ERROR( "in getLArData(), Could not retrieve LAr Pedestal"  );
      }
    }
      
    const LArOnlineID* onlineId = nullptr;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
      ATH_MSG_ERROR( "in getLArData(),Could not get LArOnlineID!"  );
    }

      IAlgTool* algtool;
      ILArADC2MeVTool* adc2mevTool=0;
      if(m_doLArCellDetails){
	if( toolSvc()->retrieveTool("LArADC2MeVTool", algtool).isFailure()){
	  ATH_MSG_ERROR( "in getLArData(), Could not retrieve LAr ADC2MeV Tool"  );
	} else {
	  adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
	}
      }

      double energyGeV,cellTime;
      double energyAllLArBarrel = 0.;

      ATH_MSG_DEBUG( "Start iterator loop over cells"  );
      
      SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
      const LArOnOffIdMapping* cabling{*cablingHdl};
      if(!cabling) {
         ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
         return DataMap;
      }
     

      for(;it1!=it2;it1++){
	if ((*it1)->energy() < m_cellThreshold) continue; // skip to next cell if threshold not passed

	if((*it1)->badcell()){ BadCell.push_back(1); }
        else{ BadCell.push_back(-1); }

	  if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for LAr
	  Identifier cellid = (*it1)->ID(); 

          HWIdentifier LArhwid = cabling->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	  
	  //ignore LAr cells that are to be masked
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
	  if (energyGeV == 0) energyGeV = 0.001; // 1 MeV due to LegoCut > 0.0 (couldn't be >= 0.0) 
	  energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
          energyAllLArBarrel += energyGeV;

          idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
          phi.push_back(DataType((*it1)->phi()));
          eta.push_back(DataType((*it1)->eta()));
          channel.push_back(DataType(onlineId->channel(LArhwid))); 
          feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
       	  slot.push_back(DataType(onlineId->slot(LArhwid))); 

	  if ( m_doLArCellDetails){
	    cellTime = (*it1)->time();
	    cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
	    cellGain.push_back(DataType( (*it1)->gain() ) ); 
      	
	    int largain = (*it1)->gain();
	    float pedestal=larPedestal->pedestal(LArhwid,largain);
	    float pedvalue=0;
	    if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
	    else pedvalue = 0;
	    cellPedestal.push_back(DataType(pedvalue));

            if (!adc2mevTool)
              adc2Mev.push_back(DataType(-1));
            else {
              const std::vector<float>* polynom_adc2mev = &(adc2mevTool->ADC2MEV(cellid,largain));
              if (polynom_adc2mev->size()==0){ adc2Mev.push_back(DataType(-1)); }
              else{ adc2Mev.push_back(DataType((*polynom_adc2mev)[1])); }
            }
	  }
      }

    ATH_MSG_DEBUG( " Total energy in LAr barrel in GeV : " <<  energyAllLArBarrel  );

    // write values into DataMap
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["energy"] = energy;
    DataMap["id"] = idVec;
    DataMap["channel"] = channel;
    DataMap["feedThrough"] = feedThrough;
    DataMap["slot"] = slot;

    //Bad Cells
    if (m_doBadLAr==true) {
      DataMap["BadCell"] = BadCell;
    }

   // adc counts
    if ( m_doLArCellDetails){
       DataMap["cellTime"] = cellTimeVec;
       DataMap["cellGain"] = cellGain;
       DataMap["cellPedestal"] = cellPedestal;
       DataMap["adc2Mev"] = adc2Mev;
    }
    //Be verbose
    ATH_MSG_DEBUG( dataTypeName() << " , collection: " << dataTypeName()
                   << " retrieved with " << phi.size() << " entries" );

    //All collections retrieved okay
    return DataMap;

  } // getLArData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
