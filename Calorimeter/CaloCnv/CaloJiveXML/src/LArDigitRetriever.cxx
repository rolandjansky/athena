/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/LArDigitRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

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
#include "CaloIdentifier/TileID.h"

using CLHEP::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  LArDigitRetriever::LArDigitRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("LArDigit")
  {

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    m_calo_id_man  = CaloIdManager::instance();
    m_calocell_id  = m_calo_id_man->getCaloCell_ID();
    m_sgKey = "AllCalo"; 
    m_sgKeyLArDigit[0] = "FREE"; // can also be: "HIGH" (for raw data)
    m_sgKeyLArDigit[1] = "LArDigitContainer_Thinned"; // used for DPD/ESD
    m_doDigit = false;
    m_inputdpd = false;
    
    declareProperty("StoreGateKey" , m_sgKey);
    declareProperty("CellThreshold", m_cellThreshold = 50.);
    declareProperty("RetrieveLAr"  , m_lar  = true);
    declareProperty("RetrieveHEC"  , m_hec  = true);
    declareProperty("RetrieveFCAL" , m_fcal = true);
    declareProperty("DoLArDigit",    m_doLArDigit = false);       
    declareProperty("DoHECDigit",    m_doHECDigit = false); 
    declareProperty("DoFCalDigit",   m_doFCalDigit = false);
    declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
    declareProperty("CellTimePrec", m_cellTimePrec = 3);
    
// Check the cell conditions. Not present in MC data, so false by default. Switch to 'true'
// for real (commissioning) cells.
    declareProperty("CellConditionCut", m_cellConditionCut = false);
    declareProperty("LArChannelsToIgnoreM5",  m_LArChannelsToIgnoreM5);
    declareProperty("DoMaskLArChannelsM5", m_doMaskLArChannelsM5 = false);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode LArDigitRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endmsg;

    ATH_CHECK( m_cablingKey.initialize() );

    return StatusCode::SUCCESS;        
  }
   
  /**
   * Tile data retrieval from chosen collection
   */
  StatusCode LArDigitRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    if (m_doLArDigit || m_doHECDigit || m_doFCalDigit){ 
      m_doDigit = true; 
    }

    const CaloCellContainer* cellContainer = nullptr;
    if (!evtStore()->retrieve(cellContainer,m_sgKey))
      {
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << 
          "Could not retrieve Calorimeter Cells" << endmsg;
//        return StatusCode::SUCCESS;
      }

   if(m_fcal){
      DataMap data = getLArDigitData(cellContainer,"FCAL",CaloCell_ID::LARFCAL);
      if ( FormatTool->AddToEvent("FCAL", m_sgKey, &data).isFailure()){
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Error reading FCAL data" << endmsg;
//        return StatusCode::SUCCESS;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "FCAL retrieved" << endmsg;
      }
    }

    if(m_lar){
      DataMap data = getLArDigitData(cellContainer,"LAr",CaloCell_ID::LAREM);
      if ( FormatTool->AddToEvent("LAr", m_sgKey, &data).isFailure()){
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Error reading LAr data" << endmsg;
//        return StatusCode::SUCCESS;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "LAr retrieved" << endmsg;
      }
    }

    if(m_hec){
      DataMap data = getLArDigitData(cellContainer,"HEC",CaloCell_ID::LARHEC);
      if ( FormatTool->AddToEvent("HEC", m_sgKey, &data).isFailure()){
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Error reading HEC data" << endmsg;
//        return StatusCode::SUCCESS;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "HEC retrieved" << endmsg;
      }
    }
    
    //Tile cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve Tile cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap LArDigitRetriever::getLArDigitData(const CaloCellContainer* cellContainer,
                                                   std::string datatype,
                                                   CaloCell_ID::SUBCALO calotype)
  {
    
    //be verbose
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getLArDigitData()" << endmsg;
    
    char rndStr[30];
    DataMap DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect x; x.reserve(cellContainer->size());
    DataVect y; y.reserve(cellContainer->size());
    DataVect dx; dx.reserve(cellContainer->size());
    DataVect dy; dy.reserve(cellContainer->size());

    DataVect energy; energy.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect channel; channel.reserve(cellContainer->size());
    DataVect feedThrough; feedThrough.reserve(cellContainer->size());
    DataVect slotVec; slotVec.reserve(cellContainer->size());

    DataVect cellTimeVec; cellTimeVec.reserve(cellContainer->size());
    DataVect cellGain; cellGain.reserve(cellContainer->size());
    DataVect cellPedestal; cellPedestal.reserve(cellContainer->size());
    DataVect adc2Mev; adc2Mev.reserve(cellContainer->size());
     
//    m_sub; m_sub.reserve(cellContainer->size());
    m_sub.clear(); // need to clear before each event, otherwise appended

    DataVect LArSampleIndexVec; LArSampleIndexVec.reserve(cellContainer->size());

    std::string LArSampleIndexStr="adcCounts multiple=\"0\"";
             
    const LArDigitContainer* LArDigitCnt;
    StatusCode scLArDigit;
    scLArDigit = evtStore()->retrieve(LArDigitCnt, m_sgKeyLArDigit[0]);
    if (scLArDigit.isFailure()) {
      scLArDigit = evtStore()->retrieve(LArDigitCnt, m_sgKeyLArDigit[1]);
      m_inputdpd = true;
    }

    if (scLArDigit.isFailure()){
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Could not retrieve LArDigits" << endmsg;
    }


    const ILArPedestal* larPedestal;
    if ( detStore()->retrieve(larPedestal).isFailure()){
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getLArDigitData(), Could not retrieve LAr Pedestal" << endmsg;
    }
    
    const LArOnlineID* onlineId;
    if ( detStore()->retrieve(onlineId, "LArOnlineID").isFailure()) {
     if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getLArDigitData(),Could not get LArOnlineID!" << endmsg;
    }
     
    IAlgTool* algtool;
    ILArADC2MeVTool* adc2mevTool=0;

    if ( toolSvc()->retrieveTool("LArADC2MeVTool", algtool).isFailure()){
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getLArDigitData(), Could not retrieve LAr ADC2MeV Tool" <<endmsg;
    } else {
      adc2mevTool=dynamic_cast<ILArADC2MeVTool*>(algtool);
    }
   
//------------------------------------------------------
//Loop over the digits and find Cell (LAr,HEC, FCAL)
//------------------------------------------------------

    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
    const LArOnOffIdMapping* cabling{*cablingHdl};
    if(!cabling) {
       ATH_MSG_ERROR( "Do not have cabling mapping from key " << m_cablingKey.key() );
       return DataMap;
    }
      
    
    if (!scLArDigit.isFailure() && m_doDigit==true) {

      int cellIndex[200000] = {0};
      int nLArSamples = 0;
      LArDigitContainer::const_iterator itLAr=LArDigitCnt->begin();
      LArDigitContainer::const_iterator itLArEnd=LArDigitCnt->end();
      HWIdentifier LArHardwareId;
      Identifier LArId;

      double energyGeV,cellTime;
      
      for (;itLAr!=itLArEnd;itLAr++){
        
        LArHardwareId = (*itLAr)->hardwareID();
        if (!cabling->isOnlineConnected(LArHardwareId))continue;
        
        LArId = cabling->cnvToIdentifier(LArHardwareId); //converter
        const IdentifierHash cellhash=m_calocell_id->calo_cell_hash(LArId); //fast method to find cell         

        int Index = cellContainer->findIndex(cellhash);  //find Cell Index
        if (Index >= 0)
          cellIndex[Index] = Index;
        
        nLArSamples = (*itLAr)->nsamples();
        std::vector<short> LArSamples = (*itLAr)->samples(); 
        int largain = (*itLAr)->gain();
        int FT = onlineId->feedthrough(LArHardwareId);
        int slot = onlineId->slot(LArHardwareId);
        int larchan = onlineId->channel(LArHardwareId);
        float pedestal=larPedestal->pedestal(LArHardwareId,largain);
        float pedvalue=0;
        if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
          else pedvalue = LArSamples[0];

        const std::vector<float>* polynom_adc2mev = 0;
        if ( adc2mevTool ){
          polynom_adc2mev = &(adc2mevTool->ADC2MEV(LArId,largain));
	}else{
	  if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getLArDigitData(), Could not access LAr ADC2MeV Tool" <<endmsg;
	}
        
        if ( Index >= 0 ){ // can be -1
          if ( (*cellContainer)[Index]->energy() >m_cellThreshold) {

            if (((((*cellContainer)[Index]->provenance())&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full conition for LAr Cells

            //ignore LAr, HEC, and/or FCAL cells that are to be masked
            if (m_doMaskLArChannelsM5){
              bool maskChannel = false;
              for (size_t i = 0; i < m_LArChannelsToIgnoreM5.size(); i++){
                  if ( (*cellContainer)[Index]->ID().get_compact() == m_LArChannelsToIgnoreM5[i]){
                  maskChannel = true; 
                  break;  // exit loop over bad channels
                }              
              }
              if (maskChannel) continue;  // continue loop over all channels
            }


            if (datatype == "LAr" && m_calocell_id->is_em(LArId)) {
            
              calcEMLayerSub(LArId);
              
              energyGeV = (*cellContainer)[Index]->energy() * (1./GeV);
              energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
              idVec.push_back(DataType((*cellContainer)[Index]->ID().get_compact() ));
              phi.push_back(DataType((*cellContainer)[Index]->phi()));
              eta.push_back(DataType((*cellContainer)[Index]->eta()));
              
              cellTime = (*cellContainer)[Index]->time();
              cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
              cellPedestal.push_back(DataType(pedvalue));
              cellGain.push_back(DataType(largain)); 
              channel.push_back(DataType(larchan)); 
                    feedThrough.push_back(DataType(FT)); 
                     slotVec.push_back(DataType(slot)); 
              if (!polynom_adc2mev || polynom_adc2mev->size()==0){
		adc2Mev.push_back(DataType(-1));
              }else{ 
		adc2Mev.push_back(DataType((*polynom_adc2mev)[1]));	
	      }              
              if ( m_doLArDigit ) { 
                LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
                for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(LArSamples[i]));
              }
            } //match datatype LAr

            else if(datatype == "HEC" && m_calocell_id->is_hec(LArId)) {
            
              calcHECLayerSub(LArId);  
              energyGeV = (*cellContainer)[Index]->energy() * (1./GeV);
              energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
              idVec.push_back(DataType((*cellContainer)[Index]->ID().get_compact() ));
              phi.push_back(DataType((*cellContainer)[Index]->phi()));
              eta.push_back(DataType((*cellContainer)[Index]->eta()));
              
              
              cellTime = (*cellContainer)[Index]->time();
              cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
              cellPedestal.push_back(DataType(pedvalue));
              cellGain.push_back(DataType(largain)); 
              channel.push_back(DataType(larchan)); 
              feedThrough.push_back(DataType(FT)); 
              slotVec.push_back(DataType(slot)); 
              if (!polynom_adc2mev || polynom_adc2mev->size()==0)
                adc2Mev.push_back(DataType(-1));
              else
                adc2Mev.push_back(DataType((*polynom_adc2mev)[1]));
              if (m_doHECDigit){
                LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
                for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(LArSamples[i]));
              }
            }//match datatype HEC

            else if(datatype == "FCAL" && m_calocell_id->is_fcal(LArId)) {
          
              energyGeV = (*cellContainer)[Index]->energy() * (1./GeV);
              energy.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
              idVec.push_back(DataType((*cellContainer)[Index]->ID().get_compact() ));
              x.push_back(DataType((*cellContainer)[Index]->x() *0.1));
              y.push_back(DataType((*cellContainer)[Index]->y() *0.1));
              
              const CaloDetDescrElement* elt = (*cellContainer)[Index]->caloDDE();
              
              dx.push_back(DataType(elt->dx()*0.1));
              dy.push_back(DataType(elt->dy()*0.1));
              
              if (m_calocell_id->pos_neg(LArId)==2) m_sub.push_back(DataType(1));
                else  m_sub.push_back(DataType(0));


              cellTime = (*cellContainer)[Index]->time();
              cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
              cellPedestal.push_back(DataType(pedvalue));
              cellGain.push_back(DataType(largain)); 
              channel.push_back(DataType(larchan)); 
              feedThrough.push_back(DataType(FT)); 
              slotVec.push_back(DataType(slot)); 
              if (!polynom_adc2mev || polynom_adc2mev->size()==0)
                adc2Mev.push_back(DataType(-1));
              else
                adc2Mev.push_back(DataType((*polynom_adc2mev)[1]));
              if (m_doFCalDigit){
                LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
                for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(LArSamples[i]));
              }
            }//match datatype FCAL
          } //if cellThreshold
        } // if Index >0
      } //math for itLar
    

// If the digits are retrived from DPD, retrieve the other cells which do not have the digits avaliable

      if (m_inputdpd) {
      
        CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(calotype);
        CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(calotype);


        for (;it1!=it2;it1++) {
        //---------------------------------------------

          if ( (*it1)->energy() < m_cellThreshold ) continue;   

          Identifier cellid = (*it1)->ID();           
          
          const IdentifierHash cellhash=m_calocell_id->calo_cell_hash(cellid); //fast method to find cell         
          int Index = cellContainer->findIndex(cellhash);  //find Cell Index
          if (Index >= 0 && cellIndex[Index] == Index)
            continue; //test whether this cell was already retrieved

          HWIdentifier LArhwid = cabling->createSignalChannelIDFromHash((*it1)->caloDDE()->calo_hash());
      
          if (m_calocell_id->is_tile(cellid) ) continue;
          if (((((*it1)->provenance())&0xFF)!=0xA5)&&m_cellConditionCut) continue; // check full condition for LAr cells 

          //ignore LAr, HEC, and/or FCAL cells that are to be masked
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
          idVec.push_back(DataType((*it1)->ID().get_compact() ));
          channel.push_back(DataType(onlineId->channel(LArhwid))); 
          feedThrough.push_back(DataType(onlineId->feedthrough(LArhwid))); 
                 slotVec.push_back(DataType(onlineId->slot(LArhwid))); 

          cellTime = (*it1)->time();
          cellTimeVec.push_back(DataType( gcvt( cellTime, m_cellTimePrec, rndStr) ) );
          cellGain.push_back(DataType((*it1)->gain())); 
          int largain = (*it1)->gain();
          float pedvalue=0; 
          float pedestal=larPedestal->pedestal(LArhwid,largain);
          if (pedestal >= (1.0+LArElecCalib::ERRORCODE)) pedvalue = pedestal;
            else pedvalue = 0;
          cellPedestal.push_back(DataType(pedvalue));

          if (!adc2mevTool)
            adc2Mev.push_back(DataType(-1));
          else {
            const std::vector<float>* polynom_adc2mev = &(adc2mevTool->ADC2MEV(cellid,largain));
            if (!polynom_adc2mev || polynom_adc2mev->size()==0){
	      adc2Mev.push_back(DataType(-1));
            }else{
	      adc2Mev.push_back(DataType((*polynom_adc2mev)[1]));
	    }
          }
            

          if (datatype == "LAr") { 
            calcEMLayerSub(cellid);
            phi.push_back(DataType((*it1)->phi()));
            eta.push_back(DataType((*it1)->eta()));
          if ( m_doLArDigit ) { 
            LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
            for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(0)); }
          }

          else if (datatype == "HEC") { 
            calcHECLayerSub(cellid);
            phi.push_back(DataType((*it1)->phi()));
            eta.push_back(DataType((*it1)->eta()));
          if (m_doHECDigit){
            LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
            for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(0)); }
          }

          else if (datatype == "FCAL") {

            x.push_back(DataType( (*it1)->x()*0.1 ));
            y.push_back(DataType( (*it1)->y()*0.1 ));

            const CaloDetDescrElement* elt = (*it1)->caloDDE();
            dx.push_back(DataType( elt->dx()*0.1 ));
            dy.push_back(DataType( elt->dy()*0.1 ));
              
            if (m_calocell_id->pos_neg(cellid)==2) m_sub.push_back(DataType(1));
              else m_sub.push_back(DataType(0));
            if (m_doFCalDigit){
              LArSampleIndexStr="adcCounts multiple=\""+DataType(nLArSamples).toString()+"\"";
              for(int i=0; i<nLArSamples; i++) LArSampleIndexVec.push_back(DataType(0));
            }
          }

        }//for(;it1!=it2;it1++)
      }// if (m_inputdpd)
    }// if (!scLArDigit)

//----------------above lines are trying to get the LAr,HEC and FCAL tags --------------    

    // write values into DataMap

    if(!(datatype=="FCAL")){
      DataMap["phi"] = phi;
      DataMap["eta"] = eta;
    } else {
      DataMap["x"] = x;
      DataMap["y"] = y;
      DataMap["dx"] = dx;
      DataMap["dy"] = dy;
    }

    DataMap["energy"] = energy;
    DataMap["id"] = idVec;
    DataMap["channel"] = channel;
    DataMap["feedThrough"] = feedThrough;
    DataMap["slot"] = slotVec;
    
    // adc counts
    DataMap["cellTime"] = cellTimeVec;
    DataMap["cellGain"] = cellGain;
    DataMap["cellPedestal"] = cellPedestal;
    DataMap["adc2Mev"] = adc2Mev;

    DataMap[LArSampleIndexStr] = LArSampleIndexVec; // adcCounts

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getTileData


  //--------------------------------------------------------------------------
  
  void LArDigitRetriever::calcEMLayerSub(Identifier& cellid)
  {
    if(abs(m_calocell_id->pos_neg(cellid))==1)
      m_sub.push_back(DataType(1));
    if(abs(m_calocell_id->pos_neg(cellid))==2)
      m_sub.push_back(DataType(2));
    if(abs(m_calocell_id->pos_neg(cellid))==3)
      m_sub.push_back(DataType(0));
    
  }

  //--------------------------------------------------------------------------

  void LArDigitRetriever::calcHECLayerSub(Identifier& cellid)
  {
    if(m_calocell_id->pos_neg(cellid)==2)
      m_sub.push_back(DataType(1));
    else
      m_sub.push_back(DataType(0));
  }
  

  //--------------------------------------------------------------------------
  
} // JiveXML namespace
