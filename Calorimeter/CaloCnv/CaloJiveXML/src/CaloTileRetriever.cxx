/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloTileRetriever.h"

#include "AthenaKernel/Units.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileCellContainer.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

using Athena::Units::GeV;

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CaloTileRetriever::CaloTileRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("TileDigit") {

   //Only declare the interface
   declareInterface<IDataRetriever>(this);

   m_calo_id_man  = CaloIdManager::instance();
   m_calocell_id  = m_calo_id_man->getCaloCell_ID();
   m_sgKey = "AllCalo";

   declareProperty("StoreGateKey" , m_sgKey);
   declareProperty("CellThreshold", m_cellThreshold = 50.);
   declareProperty("RetrieveTILE" , m_tile = true);
   declareProperty("DoTileDigit",   m_doTileDigit = false);
   declareProperty("DoBadTile",     m_doBadTile = false);
   declareProperty("DoTileCellDetails",  m_doTileCellDetails = false);
   declareProperty("CellEnergyPrec", m_cellEnergyPrec = 3);
   declareProperty("CellTimePrec", m_cellTimePrec = 3);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloTileRetriever::initialize() {

    ATH_MSG_DEBUG( "Initialising Tool" );

    //=== get TileCondToolTiming
    ATH_CHECK( m_tileToolTiming.retrieve() );

    //=== get TileCondToolEmscale
    ATH_CHECK( m_tileToolEmscale.retrieve() );

    //=== get TileBadChanTool
    ATH_CHECK( m_tileBadChanTool.retrieve() );

    return StatusCode::SUCCESS;
  }

  /**
   * Tile data retrieval from chosen collection
   */
  StatusCode CaloTileRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    const CaloCellContainer* cellContainer;
    if (!evtStore()->retrieve(cellContainer,m_sgKey)) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  <<
        "Could not retrieve Calorimeter Cells for Tile " << endmsg;
      return StatusCode::SUCCESS;
    }

    if (m_tile) {
      DataMap data = getCaloTileData(cellContainer);
      if ( FormatTool->AddToEvent("TILE", m_sgKey, &data).isFailure()) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Error reading Tile data" << endmsg;
        return StatusCode::SUCCESS;
      } else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Tile retrieved" << endmsg;
      }
    }
    //Tile cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve Tile cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloTileRetriever::getCaloTileData(const CaloCellContainer* cellContainer) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getTileData()" << endmsg;
    char rndStr[30];
    DataMap DataMap;

    DataVect phi; phi.reserve(cellContainer->size());
    DataVect eta; eta.reserve(cellContainer->size());
    DataVect idVec; idVec.reserve(cellContainer->size());
    DataVect energyVec; energyVec.reserve(cellContainer->size());
    DataVect adcCounts1Vec; adcCounts1Vec.reserve(cellContainer->size() * 10);
    DataVect adcCounts2Vec; adcCounts2Vec.reserve(cellContainer->size() * 10);
    DataVect BadCell; BadCell.reserve(cellContainer->size());

    DataVect pmt1Energy; pmt1Energy.reserve(cellContainer->size());
    DataVect pmt1Time; pmt1Time.reserve(cellContainer->size());
    DataVect pmt1Chi2; pmt1Chi2.reserve(cellContainer->size());
    DataVect pmt1Gain; pmt1Gain.reserve(cellContainer->size());
    DataVect pmt2Energy; pmt2Energy.reserve(cellContainer->size());
    DataVect pmt2Time; pmt2Time.reserve(cellContainer->size());
    DataVect pmt2Chi2; pmt2Chi2.reserve(cellContainer->size());
    DataVect pmt2Gain; pmt2Gain.reserve(cellContainer->size());

    DataVect pmt1RawAmplitude; pmt1RawAmplitude.reserve(cellContainer->size());
    DataVect pmt1RawTime; pmt1RawTime.reserve(cellContainer->size());
    DataVect pmt1Pedestal; pmt1Pedestal.reserve(cellContainer->size());
    DataVect pmt1Number; pmt1Number.reserve(cellContainer->size());
    DataVect pmt1ADCStatus; pmt1ADCStatus.reserve(cellContainer->size());
    DataVect pmt2RawAmplitude; pmt2RawAmplitude.reserve(cellContainer->size());
    DataVect pmt2RawTime; pmt2RawTime.reserve(cellContainer->size());
    DataVect pmt2Pedestal; pmt2Pedestal.reserve(cellContainer->size());
    DataVect pmt2Number; pmt2Number.reserve(cellContainer->size());
    DataVect pmt2ADCStatus; pmt2ADCStatus.reserve(cellContainer->size());

//    m_sub; m_sub.reserve(cellContainer->size());
    m_sub.clear();
//    msg(MSG::INFO)  << "Size of CellC =  " << cellContainer->size() << endmsg;

    std::string adcCounts1Str = "adcCounts1 multiple=\"0\"";
    std::string adcCounts2Str = "adcCounts2 multiple=\"0\"";

    StatusCode scTileDigit = StatusCode::FAILURE;
    StatusCode scTileRawChannel = StatusCode::FAILURE;
    const TileID* tileID;
    const TileHWID* tileHWID;
    const TileInfo* tileInfo;
    const TileCablingService* cabling=nullptr;
    const TileDigitsContainer *tileDigits = nullptr;
    const TileRawChannelContainer* RawChannelCnt;
    TileRawChannelUnit::UNIT RChUnit = TileRawChannelUnit::ADCcounts;  //!< Unit for TileRawChannels (ADC, pCb, etc.)
    cabling = TileCablingService::getInstance();
    double energyGeV;
    double amplitude = 0.;
    const int vsize = cellContainer->size();  //5184;
    int Index,pmtInd, cellInd, nTileSamples=0;
    std::vector<double> pmt1ped(vsize,0.0);
    std::vector<double> pmt2ped(vsize,0.0);
    std::vector<double> pmt1rawamp(vsize,0.0);
    std::vector<double> pmt2rawamp(vsize,0.0);
    std::vector<double> pmt1rawtime(vsize,0.0);
    std::vector<double> pmt2rawtime(vsize,0.0);
    std::vector<int> pmt1number(vsize,0);
    std::vector<int> pmt2number(vsize,0);
    std::vector<unsigned long int> pmt1status(vsize,0);
    std::vector<unsigned long int> pmt2status(vsize,0);
    std::map<int,std::vector<float> > pmt1digit;
    std::map<int,std::vector<float> > pmt2digit;
    bool offlineRch = false;

    //===== retrieving everything which is needed for Tile

    StatusCode sc = detStore()->retrieve(tileID);
    if ( sc.isFailure() )
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getCaloTileData(), Could not retrieve TileID" <<endmsg;

    sc = detStore()->retrieve(tileHWID);
    if ( sc.isFailure() )
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getCaloTileData(), Could not retrieve TileHWID" <<endmsg;

    //=== get TileInfo
    sc = detStore()->retrieve(tileInfo, "TileInfo");
    if (sc.isFailure())
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getCaloTileData(), Could not retrieve TileInfo"<< endmsg;


    if (m_doTileDigit) {

      std::string DigiName[2] = {"TileDigitsCnt","TileDigitsFlt"};
      int icntd=2;
      for (icntd=0; icntd<2; ++icntd) {
        if (evtStore()->contains<TileDigitsContainer>(DigiName[icntd])) break;
      }
      if (icntd<2) {
        scTileDigit = evtStore()->retrieve(tileDigits,DigiName[icntd]);
      }

      if (scTileDigit.isFailure())
        if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getTileData(), Could not retrieve TileDigits" << endmsg;
    }


    if (m_doTileCellDetails) {

      std::string RchName[7] = {"TileRawChannelOpt2","TileRawChannelOpt","TileRawChannelFixed",
                                "TileRawChannelFitCool","TileRawChannelFit",
                                "TileRawChannelCnt","TileRawChannelFlt"};
      int icntr=0;
      for ( ; icntr<7; ++icntr) {
        if (evtStore()->contains<TileRawChannelContainer>(RchName[icntr])) break;
      }
      if (icntr<7) {
        scTileRawChannel = evtStore()->retrieve(RawChannelCnt,RchName[icntr]);
        if (scTileRawChannel.isSuccess()) {
          RChUnit = RawChannelCnt->get_unit();
          offlineRch = (RChUnit<TileRawChannelUnit::OnlineADCcounts && 
                        RawChannelCnt->get_type() != TileFragHash::OptFilterDsp);
        }
      }

      if (scTileRawChannel.isFailure())
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "in getTileData(), Could not retrieve TileRawchannel" << endmsg;
    }


    // Loop Over TileRawChannelContainer to retrieve raw information. Keep the values in vectors

    if (scTileRawChannel.isSuccess()) { // separate "if" just to make sure that status is always checked
     if (offlineRch) {

      TileRawChannelContainer::const_iterator collItr=RawChannelCnt->begin();
      TileRawChannelContainer::const_iterator lastColl=RawChannelCnt->end();

      for (; collItr!=lastColl; ++collItr) {

        TileRawChannelCollection::const_iterator chItr=(*collItr)->begin();
        TileRawChannelCollection::const_iterator lastCh=(*collItr)->end();

        for (; chItr!=lastCh; ++chItr) {

          /*Identifier cell_id =*/ (*chItr)->cell_ID_index(Index,pmtInd);
          if (Index <= -1 ) continue;  //disconnect channel index is -1 and MBTS is -2. They do not have an idhash
          IdentifierHash cell_hash = m_calocell_id->calo_cell_hash( (*chItr)->cell_ID() );
          cellInd = cellContainer->findIndex(cell_hash);  //find Cell Index
          if (cellInd < 0) continue;

          HWIdentifier hwid=(*chItr)->adc_HWID();
          int adc       = tileHWID->adc(hwid);
          int channel   = tileHWID->channel(hwid);
          int drawer    = tileHWID->drawer(hwid);
          int ros       = tileHWID->ros(hwid);
          int PMT = abs( cabling->channel2hole(ros,channel) );
          int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
          uint32_t tileAdcStatus = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(drawerIdx,channel,adc));

          amplitude = (*chItr)->amplitude();
          //Change amplitude units to ADC counts
          if (TileRawChannelUnit::ADCcounts < RChUnit && RChUnit < TileRawChannelUnit::OnlineADCcounts) {
            amplitude /= m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0, TileRawChannelUnit::ADCcounts, RChUnit);
          } else if (RChUnit > TileRawChannelUnit::OnlineADCcounts) {
            // Should never get here due to offlineRch test above.
            //amplitude = m_tileToolEmscale->undoOnlCalib(drawerIdx, channel, adc, amplitude, RChUnit);
            std::abort();
          }

          if ( pmtInd == 0 ) { // first PMT

            pmt1ped[cellInd] = (*chItr)->pedestal();
            pmt1rawamp[cellInd] = amplitude;
            pmt1rawtime[cellInd] = (*chItr)->uncorrTime();
            pmt1number[cellInd] = PMT;
            pmt1status[cellInd] = tileAdcStatus;
          }
          else { // second PMT

            pmt2ped[cellInd] = (*chItr)->pedestal();
            pmt2rawamp[cellInd] = amplitude;
            pmt2rawtime[cellInd] = (*chItr)->uncorrTime();
            pmt2number[cellInd] = PMT;
            pmt2status[cellInd] = tileAdcStatus;

          }
        }
      }//for TileRawChannelContainer loop
     }
    } //end TileRawChannel.sucess


    //Loop over TileDigitsContainer to retrieve digits. Keep the digits values in a map

    if (scTileDigit.isSuccess() && tileDigits) {

      //----- get tile digits--------------------------
      TileDigitsContainer::const_iterator itColl = tileDigits->begin();
      TileDigitsContainer::const_iterator itCollEnd = tileDigits->end();

      // tile digits loop
      for (; itColl != itCollEnd; ++itColl) {
        TileDigitsCollection::const_iterator it = (*itColl)->begin();
        TileDigitsCollection::const_iterator itEnd = (*itColl)->end();

        for (; it!=itEnd; ++it) {

          /*Identifier cell_id =*/ (*it)->cell_ID_index(Index,pmtInd);
          if (Index <= -1 ) continue; //disconnect channel index is -1 and MBTS is -2. They do not have an idhash
          IdentifierHash cell_hash = m_calocell_id->calo_cell_hash( (*it)->cell_ID() );
          cellInd = cellContainer->findIndex(cell_hash);  //find Cell Index
          if (cellInd < 0) continue;
          if ( (*cellContainer)[cellInd]->energy() < m_cellThreshold) continue;

          nTileSamples = (*it)->NtimeSamples();
          std::vector<float> tileSamples = (*it)->samples();

          if (pmtInd == 0 ) { // first PMT
            pmt1digit.insert(std::make_pair( cellInd, tileSamples ) );
          }
          else { // second PMT
            pmt2digit.insert(std::make_pair( cellInd, tileSamples ) );
          }

        }
      }//for TileDigitContainer loop
    } // if scTileDigit.isSuccess


    //Loop Over CaloCellContainer to retrieve TileCell information

    CaloCellContainer::const_iterator it1 = cellContainer->beginConstCalo(CaloCell_ID::TILE);
    CaloCellContainer::const_iterator it2 = cellContainer->endConstCalo(CaloCell_ID::TILE);

    double energyAllTile = 0.; 

    for (;it1!=it2;it1++) {

      if ((*it1)->badcell()) BadCell.push_back(1);
      else if ((*it1)->energy()>= m_cellThreshold) BadCell.push_back(0);
      else BadCell.push_back(-1);

      Identifier cellid = (*it1)->ID();
      IdentifierHash cell_hash = m_calocell_id->calo_cell_hash( cellid );
      cellInd = cellContainer->findIndex(cell_hash);
      calcTILELayerSub(cellid);

      energyGeV = (*it1)->energy()*(1./GeV);
      energyVec.push_back(DataType( gcvt( energyGeV, m_cellEnergyPrec, rndStr) ));
      energyAllTile += energyGeV;

      idVec.push_back(DataType( (Identifier::value_type)(*it1)->ID().get_compact() ));
      phi.push_back(DataType((*it1)->phi()));
      eta.push_back(DataType((*it1)->eta()));

      if (m_doTileDigit && scTileDigit.isSuccess()) {

        if ( !pmt1digit[cellInd].empty()) {
          for (int i=0; i<nTileSamples; i++) {
            adcCounts1Str="adcCounts1 multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts1Vec.push_back(DataType( int(pmt1digit[cellInd][i])));
          }
        }
        else {
          for (int i=0; i<nTileSamples; i++) {
            adcCounts1Str="adcCounts1 multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts1Vec.push_back(DataType(0));
          }
        }


        if ( !pmt2digit[cellInd].empty()) {
          for (int i=0; i<nTileSamples; i++) {
            adcCounts2Str="adcCounts2 multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts2Vec.push_back(DataType( int(pmt2digit[cellInd][i])));
          }
        }
        else {
          for (int i=0; i<nTileSamples; i++) {
            adcCounts2Str= "adcCounts2 multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts2Vec.push_back(DataType(0));
          }
        }
      }

      if (m_doTileCellDetails) {

        const TileCell* theTileCell = dynamic_cast<const TileCell *>(*it1);

        int  gain1  = theTileCell->gain1();
        int  qual1  = theTileCell->qual1();
        bool badch1 = theTileCell->badch1();
        bool noch1  = (gain1<0 || gain1>1);

        int  qual2  = theTileCell->qual2();
        int  gain2  = theTileCell->gain2();
        bool badch2 = theTileCell->badch2();
        bool noch2  = (gain2<0 || gain2>1);

        if (badch1 != badch2 && qual1 != qual2 && qual1 < 255 && qual2 < 255) {
          if (badch1 && !noch1) gain1 = 1 - gain1;
          if (badch2 && !noch1) gain2 = 1 - gain2;
        }
        if (badch1) qual1 = -qual1;
        if (badch2) qual2 = -qual2;

        pmt1Energy.push_back(DataType(theTileCell->ene1()*(1./GeV)));
        pmt1Time.push_back(DataType(theTileCell->time1()));
        pmt1Chi2.push_back(DataType(qual1));
        pmt1Gain.push_back(DataType(gain1));

        pmt2Energy.push_back(DataType(theTileCell->ene2()*(1./GeV)));
        pmt2Time.push_back(DataType(theTileCell->time2()));
        pmt2Chi2.push_back(DataType(qual2));
        pmt2Gain.push_back(DataType(gain2));

        if (offlineRch && scTileRawChannel.isSuccess() 
            && (noch1 || pmt1number[cellInd]!=0) && (noch2 || pmt2number[cellInd]!=0)) {

          uint32_t tileAdcStatus = pmt1status[cellInd];
          if (badch1) tileAdcStatus += 10;

          pmt1RawAmplitude.push_back(DataType(pmt1rawamp[cellInd]));
          pmt1RawTime.push_back(DataType(pmt1rawtime[cellInd]));
          pmt1Pedestal.push_back(DataType(pmt1ped[cellInd]));
          pmt1Number.push_back(DataType(pmt1number[cellInd]));
          pmt1ADCStatus.push_back(DataType(tileAdcStatus));

          tileAdcStatus = pmt2status[cellInd];
          if (badch2) tileAdcStatus += 10;

          pmt2RawAmplitude.push_back(DataType(pmt2rawamp[cellInd]));
          pmt2RawTime.push_back(DataType(pmt2rawtime[cellInd]));
          pmt2Pedestal.push_back(DataType(pmt2ped[cellInd]));
          pmt2Number.push_back(DataType(pmt2number[cellInd]));
          pmt2ADCStatus.push_back(DataType(tileAdcStatus));
        }
        else {  //// don't have TileRawChannel container, but want raw amplitude (for DPD input)

          float maxTime = (tileInfo->NdigitSamples()/2) * 25;

          if (noch1 /* || pmt1digit[cellInd].empty() */ ) { //invalid gain - channel missing or digits not include in DPD
            pmt1RawAmplitude.push_back(DataType(0));
            pmt1RawTime.push_back(DataType(0));
            pmt1ADCStatus.push_back(DataType(0));
            pmt1Number.push_back(DataType(0));
            pmt1Pedestal.push_back(DataType(0));  //The is no pedestal in DPD.
          } else {
            HWIdentifier hwid = cabling->s2h_adc_id(tileID->adc_id(cellid,0,gain1));

            int adc       = tileHWID->adc(hwid);
            int channel   = tileHWID->channel(hwid);
            int drawer    = tileHWID->drawer(hwid);
            int ros       = tileHWID->ros(hwid);
            int PMT = abs( cabling->channel2hole(ros,channel) );
            int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
            float scale = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0,
                                                          TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
            float amp = theTileCell->ene1() / scale;
            float time = theTileCell->time1();

            int qbit = (theTileCell->qbit1() & TileCell::MASK_TIME);
            if ((qual1 != 0 || qbit != 0 || amp != 0.0) && (fabs(time) < maxTime && time != 0.0)) {
              time += m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc);
            }

            uint32_t tileAdcStatus = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(drawerIdx,channel,adc));
            if (badch1) tileAdcStatus += 10;

            pmt1RawAmplitude.push_back(DataType(amp));
            pmt1RawTime.push_back(DataType(time));
            pmt1Number.push_back(DataType(PMT));
            pmt1ADCStatus.push_back(DataType( tileAdcStatus ));
            pmt1Pedestal.push_back(DataType(0));  //The is no pedestal in DPD . This line is temporary.
          }

          if (noch2 /* || pmt2digit[cellInd].empty() */ ) { //invalid gain - channel missing
            pmt2RawAmplitude.push_back(DataType(0));
            pmt2RawTime.push_back(DataType(0));
            pmt2ADCStatus.push_back(DataType(0));
            pmt2Number.push_back(DataType(0));
            pmt2Pedestal.push_back(DataType(0));  //The is no pedestal in DPD
          } else {
            HWIdentifier hwid = cabling->s2h_adc_id(tileID->adc_id(cellid,1,gain2));

            int adc       = tileHWID->adc(hwid);
            int channel   = tileHWID->channel(hwid);
            int drawer    = tileHWID->drawer(hwid);
            int ros       = tileHWID->ros(hwid);
            int PMT = abs( cabling->channel2hole(ros,channel) );
            int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
            float scale = m_tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0,
                                                          TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
            float amp = theTileCell->ene2() / scale;
            float time = theTileCell->time2();

            int qbit = (theTileCell->qbit2() & TileCell::MASK_TIME);
            if ((qual2 != 0 || qbit != 0 || amp != 0.0) && (fabs(time) < maxTime && time != 0.0)) {
              time += m_tileToolTiming->getSignalPhase(drawerIdx, channel, adc);
            }

            uint32_t tileAdcStatus = m_tileBadChanTool->encodeStatus(m_tileBadChanTool->getAdcStatus(drawerIdx,channel,adc));
            if (badch2) tileAdcStatus += 10;

            pmt2RawAmplitude.push_back(DataType(amp));
            pmt2RawTime.push_back(DataType(time));
            pmt2ADCStatus.push_back(DataType( tileAdcStatus ));
            pmt2Number.push_back(DataType(PMT));
            pmt2Pedestal.push_back(DataType(0));  //The is no pedestal in DPD .
          }
        } //don't have TileRawChannel
      } //if (m_doTileCellDetails)
    } // end cell iterator

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Total energy in Tile in GeV : " <<  energyAllTile<< endmsg;

    if ( !pmt1digit.empty() ) pmt1digit.clear();
    if ( !pmt2digit.empty() ) pmt2digit.clear();

    // write values into DataMap
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["sub"] = m_sub;
    DataMap["id"] = idVec;
    DataMap["energy"] = energyVec;


    //BadCells
    if (m_doBadTile==true) {
      DataMap["BadCell"]= BadCell;
    }

    if (m_doTileCellDetails) {
      DataMap["pmt1Energy"] = pmt1Energy;
      DataMap["pmt1Time"] = pmt1Time;
      DataMap["pmt1Chi2"] = pmt1Chi2;
      DataMap["pmt1Gain"] = pmt1Gain;

      DataMap["pmt2Energy"] = pmt2Energy;
      DataMap["pmt2Time"] = pmt2Time;
      DataMap["pmt2Chi2"] = pmt2Chi2;
      DataMap["pmt2Gain"] = pmt2Gain;

      DataMap["pmt1RawAmplitude"] = pmt1RawAmplitude;
      DataMap["pmt1RawTime"] = pmt1RawTime;
      DataMap["pmt1ADCStatus"] = pmt1ADCStatus;
      DataMap["pmt1Number"] = pmt1Number;
      DataMap["pmt1Pedestal"] = pmt1Pedestal;

      DataMap["pmt2RawAmplitude"] = pmt2RawAmplitude;
      DataMap["pmt2RawTime"] = pmt2RawTime;
      DataMap["pmt2ADCStatus"] = pmt2ADCStatus;
      DataMap["pmt2Number"] = pmt2Number;
      DataMap["pmt2Pedestal"] = pmt2Pedestal;
    }

    DataMap[adcCounts1Str] = adcCounts1Vec;
    DataMap[adcCounts2Str] = adcCounts2Vec;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getTileData

  //-----------------------------------------------------------------------------------------------------

  void CaloTileRetriever::calcTILELayerSub(Identifier& cellid)
  {
    if (m_calocell_id->is_tile_barrel(cellid))
    {
      if (m_calocell_id->is_tile_negative(cellid))
        m_sub.push_back(DataType(2));
      else
        m_sub.push_back(DataType(3));
    }
    else if (m_calocell_id->is_tile_extbarrel(cellid))
    {
      if (m_calocell_id->is_tile_negative(cellid))
        m_sub.push_back(DataType(0));
      else
        m_sub.push_back(DataType(5));
    }
    //else in ITC or scint
    else
    {
      if (m_calocell_id->is_tile_negative(cellid))
        m_sub.push_back(DataType(1));
      else
        m_sub.push_back(DataType(4));
    }
  }

  //--------------------------------------------------------------------------

} // JiveXML namespace
