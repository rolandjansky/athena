/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/BadLArRetriever.h"

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
  BadLArRetriever::BadLArRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("BadLAr"),
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
    declareProperty("CellConditionCut", m_cellConditionCut = false);

    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode BadLArRetriever::initialize() {

    ATH_MSG_DEBUG( "Initialising Tool"  );
    ATH_CHECK( detStore()->retrieve (m_calocell_id, "CaloCell_ID") );

    ATH_CHECK( m_cablingKey.initialize() );

    return StatusCode::SUCCESS;	
  }
  
  /**
   * LAr data retrieval from default collection
   */
  StatusCode BadLArRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    ATH_MSG_DEBUG( "in retrieve()"  );

    const CaloCellContainer* cellContainer;
    if ( !evtStore()->retrieve(cellContainer,m_sgKey))
      {
	ATH_MSG_WARNING( "Could not retrieve Calorimeter Cells "  );
	return StatusCode::FAILURE;
      }

    if(m_lar){
      DataMap data = getBadLArData(cellContainer);
      ATH_CHECK( FormatTool->AddToEvent(dataTypeName(), m_sgKey, &data) );
      ATH_MSG_DEBUG( "Bad cell retrieved"  );
    }
    //LAr cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve LAr bad cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap BadLArRetriever::getBadLArData(const CaloCellContainer* cellContainer) {
    
    ATH_MSG_DEBUG( "getBadLArData()"  );

    DataMap DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slot; slot.reserve(cellContainer->size());

    char rndStr[30]; // for rounding (3 digit precision)

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::LAREM);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::LAREM);


    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    const LArOnOffIdMapping* cabling{*cablingHdl};
      
    const LArOnlineID* onlineId = nullptr;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
      ATH_MSG_ERROR( "in getBadLArData(),Could not get LArOnlineID!"  );
    }

    if (m_doBadLAr && cabling) {

      double energyGeV;

      ATH_MSG_DEBUG( "Start iterator loop over cells"  );
      
      for(;it1!=it2;it1++){

	if( !(*it1)->badcell() ) continue;
	//if( (*it1)->energy() < m_cellThreshold) continue;
	
	if ((((*it1)->provenance()&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conditions for LAr
	//Identifier cellid = (*it1)->ID(); 
	
	HWIdentifier LArhwid = cabling->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
	
	energyGeV = (*it1)->energy()*(1./GeV);
        if (energyGeV == 0) energyGeV = 0.001; // 1 MeV due to LegoCut > 0.0 (couldn't be >= 0.0) 
	energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
	
	idVec.push_back(DataType((Identifier::value_type)(*it1)->ID().get_compact() ));
	phi.push_back(DataType((*it1)->phi()));
	eta.push_back(DataType((*it1)->eta()));
	channel.push_back(DataType(onlineId->channel(LArhwid))); 
	feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
	slot.push_back(DataType(onlineId->slot(LArhwid))); 

      } // end cell iterator

    } // doBadLAr

    // write values into DataMap
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["energy"] = energy;
    DataMap["id"] = idVec;
    DataMap["channel"] = channel;
    DataMap["feedThrough"] = feedThrough;
    DataMap["slot"] = slot;
    //Be verbose
    ATH_MSG_DEBUG( dataTypeName() << " , collection: " << dataTypeName()
                   << " retrieved with " << phi.size() << " entries" );

    //All collections retrieved okay
    return DataMap;

  } // getBadLArData

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
