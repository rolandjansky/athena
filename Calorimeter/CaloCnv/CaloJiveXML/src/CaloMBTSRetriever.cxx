/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloJiveXML/CaloMBTSRetriever.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include "EventContainers/SelectAllObject.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "TileEvent/TileCell.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileEvent/TileCellContainer.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileIdentifier/TileTBID.h"
#include "TileConditions/TileInfo.h"
#include "TileConditions/TileCablingService.h"
#include "TileConditions/ITileBadChanTool.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"

namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  CaloMBTSRetriever::CaloMBTSRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("MBTS"),
    m_tileTBID(nullptr)
  {

    //Only declare the interface
    declareInterface<IDataRetriever>(this);

    m_sgKeyMBTS = "MBTSContainer";

    declareProperty("StoreGateKeyMBTS" , m_sgKeyMBTS);
    declareProperty("MBTSThreshold", m_mbtsThreshold = 0.05);
    declareProperty("RetrieveMBTS" , m_mbts = true);
    declareProperty("DoMBTSDigits",  m_mbtsdigit=false);
  }

  /**
   * Initialise the ToolSvc
   */

  StatusCode CaloMBTSRetriever::initialize() {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialising Tool" << endmsg;

    return StatusCode::SUCCESS;
  }

  /**
   * MBTS data retrieval from chosen collection
   */
  StatusCode CaloMBTSRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "in retrieve()" << endmsg;

    const TileCellContainer* cellContainerMBTS;
    if ( !evtStore()->retrieve(cellContainerMBTS,m_sgKeyMBTS))
    {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Could not retrieve Calorimeter Cells " << endmsg;
      return StatusCode::FAILURE;
    }

    if (m_mbts) {
      DataMap data = getMBTSData(cellContainerMBTS);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyMBTS, &data).isFailure()) {
        return StatusCode::FAILURE;
      } else {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "MBTS retrieved" << endmsg;
      }
    }
    //MBTS cells retrieved okay
    return StatusCode::SUCCESS;
  }


  /**
   * Retrieve MBTS cell location and details
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  const DataMap CaloMBTSRetriever::getMBTSData(const TileCellContainer* tileMBTSCellContainer) {

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "getMBTSData()" << endmsg;

    DataMap DataMap;

    DataVect energy; energy.reserve(tileMBTSCellContainer->size());
    DataVect label; label.reserve(tileMBTSCellContainer->size());
    DataVect phi; phi.reserve(tileMBTSCellContainer->size());
    DataVect eta; eta.reserve(tileMBTSCellContainer->size());
    DataVect sampling; sampling.reserve(tileMBTSCellContainer->size());
    DataVect timeVec; timeVec.reserve(tileMBTSCellContainer->size());
    DataVect quality; quality.reserve(tileMBTSCellContainer->size());
    DataVect type; type.reserve(tileMBTSCellContainer->size());
    DataVect channel; channel.reserve(tileMBTSCellContainer->size());
    DataVect module; module.reserve(tileMBTSCellContainer->size());
    DataVect cellPedestal; cellPedestal.reserve(tileMBTSCellContainer->size());
    DataVect cellRawAmplitude; cellRawAmplitude.reserve(tileMBTSCellContainer->size());
    DataVect cellRawTime; cellRawTime.reserve(tileMBTSCellContainer->size());
    DataVect adcCounts; adcCounts.reserve(tileMBTSCellContainer->size() * 10);

    std::string adcCountsStr="adcCounts multiple=\"0\"";
    StatusCode scTileDigit = StatusCode::FAILURE;
    StatusCode scTileRawChannel = StatusCode::FAILURE;
    const TileDigitsContainer *tileDigits;
    const TileRawChannelContainer* RawChannelCnt = 0;
    const TileHWID* tileHWID;
    const TileInfo* tileInfo;
    const TileCablingService* cabling=0;
    ToolHandle<TileCondToolEmscale> tileToolEmscale("TileCondToolEmscale"); //!< main Tile Calibration tool
    TileRawChannelUnit::UNIT RChUnit = TileRawChannelUnit::ADCcounts;  //!< Unit for TileRawChannels (ADC, pCb, etc.)
    cabling = TileCablingService::getInstance();
    bool offlineRch = false;

    if ( detStore()->retrieve(m_tileTBID).isFailure() )
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<< "in getMBTSData(), Could not retrieve m_tileTBID" <<endmsg;

    if ( detStore()->retrieve(tileHWID).isFailure() )
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<< "in getMBTSData(), Could not retrieve TileHWID" <<endmsg;

    //=== get TileInfo
    if ( detStore()->retrieve(tileInfo, "TileInfo").isFailure() )
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "in getMBTSData(), Could not retrieve TileInfo"<< endmsg;

    //=== get TileCondToolEmscale
    if ( tileToolEmscale.retrieve().isFailure())
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR)<< "in getMBTSData(), Could not retrieve " << tileToolEmscale << endmsg;

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
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)<< "in getMBTSData(), Could not retrieve TileRawchannel" << endmsg;

    std::string DigiName[2] = {"TileDigitsCnt","TileDigitsFlt"};
    int icntd=2;
    for (icntd=0; icntd<2; ++icntd) {
      if (evtStore()->contains<TileDigitsContainer>(DigiName[icntd])) break;
    }
    if (icntd<2) {
      scTileDigit = evtStore()->retrieve(tileDigits,DigiName[icntd]);
    }

    if (scTileDigit.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)<< "in getMBTSData(), Could not retrieve TileDigits" << endmsg;
    }
    // from: TileCalorimeter/TileRec/src/TileCellToNtuple.cxx

    std::string MBTS_ID;
    int nchan =0;
    int nTileSamples=0;
    const int max_chan=5216;
    double energyMeV = 0.;
    double phiMBTS = 0.;
    double amplitude = 0.;
    unsigned long int cellid;
    std::map<unsigned long int,double> theMbtspedestal;
    std::map<unsigned long int,double> theMbtsrawamp;
    std::map<unsigned long int,double> theMbtsrawtime;
    std::map<unsigned long int,std::vector<float> > theMbtsdigit;
    std::string myCellRawTimeStr = "0.";

    //Loop over TileRawChannel to get Pedestal and raw amplitude and time

    if (scTileRawChannel.isSuccess()) { // separate "if" just to make sure that status is always checked
     if (offlineRch) {

      TileRawChannelContainer::const_iterator collItr=RawChannelCnt->begin();
      TileRawChannelContainer::const_iterator lastColl=RawChannelCnt->end();

      for (; collItr!=lastColl; ++collItr) {

        TileRawChannelCollection::const_iterator chItr=(*collItr)->begin();
        TileRawChannelCollection::const_iterator lastCh=(*collItr)->end();

        if (chItr!=lastCh) {

          for (; chItr!=lastCh; ++chItr) {

            Identifier pmt_id = (*chItr)->pmt_ID();
            if (!m_tileTBID->is_tiletb(pmt_id)) continue;

            Identifier id = (*chItr)->cell_ID();
            cellid = id.get_identifier32().get_compact();
            HWIdentifier hwid=(*chItr)->adc_HWID();
            int adc       = tileHWID->adc(hwid);
            int channel   = tileHWID->channel(hwid);
            int drawer    = tileHWID->drawer(hwid);
            int ros       = tileHWID->ros(hwid);
            int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);

            amplitude = (*chItr)->amplitude();
            //Change amplitude units to ADC counts
            if (TileRawChannelUnit::ADCcounts < RChUnit && RChUnit < TileRawChannelUnit::OnlineADCcounts) {
              amplitude /= tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0, TileRawChannelUnit::ADCcounts, RChUnit);
            } else if (RChUnit > TileRawChannelUnit::OnlineADCcounts) {
              amplitude = tileToolEmscale->undoOnlCalib(drawerIdx, channel, adc, amplitude, RChUnit);
            }

            theMbtspedestal.insert(std::make_pair( cellid, (*chItr)->pedestal() ) );
            theMbtsrawamp.insert(std::make_pair( cellid, amplitude ));
            theMbtsrawtime.insert(std::make_pair( cellid, (*chItr)->time((*chItr)->uncorrTime()) ));
            break;

          }
        }
      }
     }
    }

    //Loop over TileDigits to retrieve MBTS digits

    if (m_mbtsdigit) {

      if (scTileDigit.isSuccess()) {

        //----- get tile digits--------------------------
        TileDigitsContainer::const_iterator itColl = tileDigits->begin();
        TileDigitsContainer::const_iterator itCollEnd = tileDigits->end();

        // tile digits loop
        for (; itColl != itCollEnd; ++itColl) {
          TileDigitsCollection::const_iterator it = (*itColl)->begin();
          TileDigitsCollection::const_iterator itEnd = (*itColl)->end();

          for (; it!=itEnd; ++it) {

            Identifier pmt_id = (*it)->pmt_ID();
            if (!m_tileTBID->is_tiletb(pmt_id)) continue;

            Identifier id = (*it)->cell_ID();
            cellid = id.get_identifier32().get_compact();

            nTileSamples = (*it)->NtimeSamples();
            std::vector<float> tileSamples = (*it)->samples();
            theMbtsdigit.insert(std::make_pair( cellid, tileSamples));
            break;

          }
        }//for TileDigitContainer loop
      } // if scTileDigit.isSuccess
    } //if (m_mbtsdigit)

    //Loop Over TileCellContainer to retrieve MBTSCell information

    TileCellContainer::const_iterator it = tileMBTSCellContainer->begin();
    TileCellContainer::const_iterator end = tileMBTSCellContainer->end();
    for (; it != end; ++it) {

      const TileCell *cell = (const TileCell *)(*it);

      int qual = cell->quality();
      if (cell->badcell()) qual = -qual;

      timeVec.push_back(DataType( cell->time() ));
      quality.push_back(DataType( qual ));

      if (  cell->energy() >= m_mbtsThreshold ) {
        energyMeV = cell->energy(); // roughly correct: energy unit is pC, which is 95% MeV
      }else{
        energyMeV = 0.;
      }
      energy.push_back(DataType( energyMeV )); // write in MeV

      Identifier id=cell->ID();

      //TileCell/type is  "side"  +/- 1
      //TileCell/module is "phi"  0-7
      //TileCell/channel is "eta"  0-1   zero is closer to beam pipe

      type.push_back(DataType( m_tileTBID->type(id) ));
      channel.push_back(DataType( m_tileTBID->channel(id) ));
      module.push_back(DataType( m_tileTBID->module(id) ));

      MBTS_ID = "type_" + DataType( m_tileTBID->type(id) ).toString() + "_ch_" +
          DataType( m_tileTBID->channel(id) ).toString() + "_mod_" +
          DataType( m_tileTBID->module(id) ).toString();

      label.push_back(DataType( MBTS_ID ));

      eta.push_back(DataType( 5.0*m_tileTBID->type(id) ));

      phiMBTS = (M_PI/8)+(2*M_PI/8)*int(m_tileTBID->module(id));
      phi.push_back(DataType( phiMBTS ));
      sampling.push_back(DataType( m_tileTBID->channel(id) ));

      if (scTileRawChannel.isSuccess() ) {

        cellPedestal.push_back(DataType( theMbtspedestal[id.get_identifier32().get_compact()] ));
        cellRawAmplitude.push_back(DataType( theMbtsrawamp[id.get_identifier32().get_compact()] ));
        myCellRawTimeStr =  DataType(theMbtsrawtime[id.get_identifier32().get_compact()]).toString();

        //if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << " Capture nan for cellRawTime: "  
	//	<< theMbtsrawtime[id.get_identifier32().get_compact()] 
	//	<< " " << myCellRawTimeStr << " " << myCellRawTimeStr.find("n") << endmsg;

        if ( myCellRawTimeStr.find("n") == 1 )  myCellRawTimeStr="0."; 
        cellRawTime.push_back( myCellRawTimeStr );

          // this can rarely be '-nan', but checking this each time may make code slow ?
      }
      else { // don't have TileRawChannel container (for DPF input)

        float maxTime = (tileInfo->NdigitSamples()/2) * 25;
        int gain = cell->gain();

        if (gain<0 || gain>1) { //invalid gain - channel missing
          cellRawAmplitude.push_back(DataType(0));
          cellRawTime.push_back(DataType(0));
          cellPedestal.push_back(DataType(0));  //There is no pedestal in DPD .
        }
        else {

          HWIdentifier hwid = tileHWID->adc_id(cabling->s2h_channel_id(id),gain);
          int adc       = tileHWID->adc(hwid);
          int channel   = tileHWID->channel(hwid);
          int drawer    = tileHWID->drawer(hwid);
          int ros       = tileHWID->ros(hwid);
          int drawerIdx = TileCalibUtils::getDrawerIdx(ros,drawer);
          float scale = tileToolEmscale->channelCalib(drawerIdx, channel, adc, 1.0,TileRawChannelUnit::ADCcounts, TileRawChannelUnit::MegaElectronVolts);
          float amp;

          if (  cell->energy() >= m_mbtsThreshold ) amp = cell->energy()/scale;
          else amp = 0.0;
          float time = cell->time();

          if ((qual != 0 || amp != 0.0) && (fabs(time) < maxTime && time != 0.0)) time -= tileInfo->TimeCalib(hwid,0.0);

          cellRawAmplitude.push_back(DataType(amp));
          cellRawTime.push_back(DataType(time));
          cellPedestal.push_back(DataType(0));  //There is no pedestal in DPD . This line is temporary.
        }
      }


      if (m_mbtsdigit && scTileDigit.isSuccess()) {

        if ( !theMbtsdigit[id.get_identifier32().get_compact()].empty() ) {
          for (int i=0; i<nTileSamples; i++) {
            adcCountsStr="adcCounts multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts.push_back(DataType( int(theMbtsdigit[id.get_identifier32().get_compact()][i]) ));
          }
        }
        else {
          for (int i=0; i<nTileSamples; i++) {
            adcCountsStr="adcCounts multiple=\""+DataType(nTileSamples).toString()+"\"";
            adcCounts.push_back(DataType(0));
          }
        }
      }


      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "MBTS no: " << nchan << ", type_chan_mod: " << MBTS_ID
                                               << ", energy MeV pC: " << energyMeV << endmsg;

      nchan++;

      if (nchan >= max_chan) break;

    }//TileCell Loop

    if (!theMbtspedestal.empty()) theMbtspedestal.clear();
    if (!theMbtsrawamp.empty() )  theMbtsrawamp.clear();
    if (!theMbtsrawtime.empty())  theMbtsrawtime.clear();

    // write values into DataMap
    DataMap["energy"] = energy;
    DataMap["label"] = label;
    DataMap["phi"] = phi;
    DataMap["eta"] = eta;
    DataMap["sampling"] = sampling;
    DataMap["time"] = timeVec;
    DataMap["quality"] = quality;
    DataMap["type"] = type;
    DataMap["channel"] = channel;
    DataMap["module"] = module;
    DataMap["cellPedestal"] = cellPedestal;
    DataMap["cellRawAmplitude"] = cellRawAmplitude;
    DataMap["cellRawTime"] = cellRawTime;
    DataMap[adcCountsStr] = adcCounts;

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return DataMap;

  } // getMBTSData

  //--------------------------------------------------------------------------

} // JiveXML namespace
