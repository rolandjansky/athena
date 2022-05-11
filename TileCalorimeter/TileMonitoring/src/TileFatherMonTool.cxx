/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************//
//
// NAME:     TileFatherMonTool.cxx
// PACKAGE:  TileMonitoring
//
// AUTHOR:   Luca Fiorini (Luca.Fiorini@cern.ch)
//
// July 2006
// ********************************************************************//

#include "TileFatherMonTool.h"

#include "xAODEventInfo/EventInfo.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/TileID.h"


#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"
#include "TileEvent/TileContainer.h"
#include "TileEvent/TileCell.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TH1C.h"
#include "TH2C.h"
#include "TH1S.h"
#include "TH2S.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TString.h"
#include "TDirectory.h"
#include "TAxis.h"

#include "LWHists/LWHist.h"


///Base class for TileCal monitoring tools
/*---------------------------------------------------------*/
TileFatherMonTool::TileFatherMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : TilePaterMonTool(type, name, parent)
  , m_lvl1info(0)
  , m_evtNum(0)
  , m_lumiBlock(0)
  , m_evtBCID(0)
  , m_runNum(0)
  , m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
  , m_fillHistogramsForL1Triggers{0,1,2,3,4,5,6,7}
  , m_allowedL1TriggerBits(8, false)

/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  declareProperty("MBTSCellContainerID", m_MBTSCellContainerID = "MBTSContainer");
  declareProperty("CellsContainerID" , m_cellsContainerID = "AllCalo"); //SG Cell Container
  declareProperty("FillHistogramsForL1Triggers" , m_fillHistogramsForL1Triggers);

  // conversion from ROS index to partition index
  m_ros2partition[TileHWID::BEAM_ROS] = NumPart;
  m_ros2partition[TileHWID::BARREL_POS] = PartLBA;
  m_ros2partition[TileHWID::BARREL_NEG] = PartLBC;
  m_ros2partition[TileHWID::EXTBAR_POS] = PartEBA;
  m_ros2partition[TileHWID::EXTBAR_NEG] = PartEBC;

  // conversion from partition index to ROS index
  m_partition2ros[PartEBA] = TileHWID::EXTBAR_POS;
  m_partition2ros[PartLBA] = TileHWID::BARREL_POS;
  m_partition2ros[PartLBC] = TileHWID::BARREL_NEG;
  m_partition2ros[PartEBC] = TileHWID::EXTBAR_NEG;
  m_partition2ros[NumPart] = TileHWID::BEAM_ROS;

  m_path = "/Tile";
}

/*---------------------------------------------------------*/
TileFatherMonTool::~TileFatherMonTool() {
/*---------------------------------------------------------*/

}

/// The initialize will set some navigation variables
/// and retrieve basic services. It is called by every montool
/*---------------------------------------------------------*/
StatusCode TileFatherMonTool::initialize() {
/*---------------------------------------------------------*/

  m_SampStrNames[SampA] = "SampA";
  m_SampStrNames[SampB] = "SampB";
  m_SampStrNames[SampD] = "SampD";
  m_SampStrNames[SampE] = "SampE";
  m_SampStrNames[AllSamp] = "AllSamp";

  m_PartNames[PartEBA] = "EBA";
  m_PartNames[PartLBA] = "LBA";
  m_PartNames[PartLBC] = "LBC";
  m_PartNames[PartEBC] = "EBC";
  m_PartNames[NumPart] = "AllPart";

  m_GainNames[0] = "LG";
  m_GainNames[1] = "HG";

  m_TrigNames[Trig_b0] = "bit0_RNDM";
  m_TrigNames[Trig_b1] = "bit1_ZeroBias";
  m_TrigNames[Trig_b2] = "bit2_L1CAL";
  m_TrigNames[Trig_b3] = "bit3_Muon";
  m_TrigNames[Trig_b4] = "bit4_RPC";
  m_TrigNames[Trig_b5] = "bit5_FTK";
  m_TrigNames[Trig_b6] = "bit6_CTP";
  m_TrigNames[Trig_b7] = "bit7_Calib";
  m_TrigNames[AnyTrig] = "AnyPhysTrig";

  m_activeTrigs[Trig_b0] = -1;
  m_activeTrigs[Trig_b1] = -1;
  m_activeTrigs[Trig_b2] = -1;
  m_activeTrigs[Trig_b3] = -1;
  m_activeTrigs[Trig_b4] = -1;
  m_activeTrigs[Trig_b5] = -1;
  m_activeTrigs[Trig_b6] = -1;
  m_activeTrigs[Trig_b7] = -1;
  m_activeTrigs[AnyTrig] = -1;

  //done explicitly
  //ToolRootHistSvc();

  for (unsigned int bit : m_fillHistogramsForL1Triggers) {
    if (bit < 8) m_allowedL1TriggerBits[bit] = true;
  }


  //SetBookStatus(false);
  CHECK(TilePaterMonTool::initialize());

  return StatusCode::SUCCESS;
}


/// Method to navigate from a cell to its Tile partition
/// EBA, LBA, LBC, EBC. Other kind of cell or non Tile cells
/// will return a conventional number NumPart
/// Argument cell is a pointer to a CaloCell
/*---------------------------------------------------------*/
int TileFatherMonTool::getPartition(const CaloCell* cell) {
  /*---------------------------------------------------------*/

  if (cell == 0) {
    return NumPart;
  }

  Identifier id = cell->ID();
  int section = m_tileID->section(id);
  int side = m_tileID->side(id);
  int partition = NumPart; // default is gap, ITC or non Tile cell

  if ((section == 2 || section == 3) && side == 1) {
    partition = PartEBA;
  } else if (section == 1 && side == 1) {
    partition = PartLBA;
  } else if (section == 1 && side == -1) {
    partition = PartLBC;
  } else if ((section == 2 || section == 3) && side == -1) {
    partition = PartEBC;
  } else {
    partition = NumPart;
  }  //what are you? get off!

  return partition;
}




/// Method to get the Level1 Trigger word:
/// 32bit long, but only 8 bit are used
/// If no trigger word or empty it will return 0
/*---------------------------------------------------------*/
void TileFatherMonTool::fillEvtInfo()
/*---------------------------------------------------------*/
{

  uint32_t lvl1info = 0;

  const DataHandle<xAOD::EventInfo> eventInfo;

  if (evtStore()->retrieve(eventInfo).isFailure()) {

    m_lvl1info = lvl1info;
    m_evtNum = 0;
    m_lumiBlock = 0;
    m_evtBCID = 0;
    m_runNum = 0;
    m_tileFlag = 0;
    m_tileError_error = 0;

  } else {

    m_lvl1info = eventInfo->level1TriggerType();
    m_evtNum = eventInfo->eventNumber();
    m_lumiBlock = eventInfo->lumiBlock();
    m_evtBCID = eventInfo->bcid();
    m_runNum = eventInfo->runNumber();

    m_tileFlag = eventInfo->eventFlags(xAOD::EventInfo::Tile);
    m_tileError_error = eventInfo->errorState(xAOD::EventInfo::Tile) == xAOD::EventInfo::Error;

//    const std::vector< xAOD::EventInfo::StreamTag >& evtStreamTags = eventInfo->streamTags();
//    for (const auto& evtStreamTag : evtStreamTags) {
//      m_streamTagTypes.push_back(evtStreamTag.type());
//      m_streamTagNames.push_back(evtStreamTag.name());
//    }

  }

  if (m_evtNum != TileFatherMonTool::m_lastevent) {
    TileFatherMonTool::m_lastevent = m_evtNum;
    checkIsCollision();
  }

}


/// Method to get the run Number in words:
/*---------------------------------------------------------*/
  std::string  TileFatherMonTool::getRunNumStr() {
/*---------------------------------------------------------*/


  std::ostringstream ss; ss.str("");
  ss << m_runNum;
  return ss.str();
}



/// Method to get the list of all Level1 trigger passed
/// by the event.
/// Results are stored in m_eventTrigs
/// Beware: vector contains always at least 1 element: AnyPhystrig or calib
/// Argument lvl1info is the 32 bit Level 1 trigger word
/*---------------------------------------------------------*/
void TileFatherMonTool::get_eventTrigs(uint32_t lvl1info) {
  /*---------------------------------------------------------*/

  m_eventTrigs.clear(); // clear vector

  if (lvl1info != 0) {

    if ((lvl1info >> Trig_b7) & 1) { // first bit tells if physics (=1) or calibration (=0) event
                                     // format is 0x1aaaaaaa
      m_eventTrigs.push_back(AnyTrig); //adding the AnyPhysTrig trigger
      for (int exp = 0; exp < Trig_b7; exp++) { // adding the phys triggers one by one

        if (((lvl1info >> exp) & 1) and (m_allowedL1TriggerBits[exp])) m_eventTrigs.push_back(exp); // store active trigger

      }
    } else { //calibration event foramt is 0x0aaaaaaa
      m_eventTrigs.push_back(Trig_b7);
    }

  } else {//close if lvl1 word not empty

    m_eventTrigs.push_back(AnyTrig); //always store at least AnyPhysTrig (=8)
  }
}

/*---------------------------------------------------------*/
void TileFatherMonTool::checkIsCollision() {
  /*---------------------------------------------------------*/

  TileFatherMonTool::m_time_difference = -999.;
  TileFatherMonTool::m_is_collision = false;

  if (m_MBTSCellContainerID.size() > 0) {

    const TileCellContainer* mbts_container;

    if (evtStore()->retrieve(mbts_container, m_MBTSCellContainerID).isFailure()) {
      ATH_MSG_INFO( "Can't find TileCellContainer with name " << m_MBTSCellContainerID
                    << " in TES. No checking if it is collision possible!" );

      ATH_MSG_INFO( "Will not try to read this container anymore!" );

      m_MBTSCellContainerID = "";

    } else {

      int nHitsA = 0;
      int nHitsC = 0;
      double timeA = 0.;
      double timeC = 0.;

      for (const TileCell* mbts_cell : *mbts_container) {
        if (mbts_cell->energy() < 0.27 || TMath::Abs(mbts_cell->time()) < 1.e-5) continue; //threshold approx 60./222.

        if (m_tileTBID->side(mbts_cell->ID()) > 0) {
          ++nHitsA;
          timeA += mbts_cell->time();
        } else {
          ++nHitsC;
          timeC += mbts_cell->time();
        }

      } // end Cell loop

      if (nHitsA > 1 && nHitsC > 1) {
        TileFatherMonTool::m_time_difference = timeA / nHitsA - timeC / nHitsC;
        if (TMath::Abs(TileFatherMonTool::m_time_difference) < 7.5) {
          TileFatherMonTool::m_is_collision = true;
        }
      }
    }
  }

  if (!TileFatherMonTool::m_is_collision && !m_cellsContainerID.empty()) {

    const CaloCellContainer* cell_container;

    if (evtStore()->retrieve(cell_container, m_cellsContainerID).isFailure()) {
      ATH_MSG_INFO( "TileCellMonTool: Retrieval of Tile cells from container " << m_cellsContainerID
                    << " failed! No checking if it is collision possible!" );
      ATH_MSG_INFO( "Will not try to read this container anymore!" );

      m_cellsContainerID = "";

    } else {
      int nHitsA = 0;
      int nHitsC = 0;
      double timeA = 0.0;
      double timeC = 0.0;

      for (const CaloCell* cell : *cell_container) {

        Identifier id = cell->ID();

        if (m_tileID->is_tile(id)) {

          const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell);
          if (!tile_cell) continue;

          // get the cell energy, time
          double time = tile_cell->time();
          double energy = tile_cell->energy();

          if (energy < 500.0 || TMath::Abs(time) < 1.e-5) continue;

          if (m_tileID->sample(id) == TileID::SAMP_E) {
            if (m_tileID->side(id) == TileID::POSITIVE) {
              ++nHitsA;
              timeA += time;
            } else {
              ++nHitsC;
              timeC += time;
            }
          } //if partition
        } // end if tile_cell
      } // end of loop over the Cells

      if (nHitsA > 1 && nHitsC > 1) {
        TileFatherMonTool::m_time_difference = timeA / nHitsA - timeC / nHitsC;

        if (TMath::Abs(TileFatherMonTool::m_time_difference) < 7.5) {
          TileFatherMonTool::m_is_collision = true;
        }
      }
    } //sc1 else

  }
}

//Generic Method to set the bin labels of an axis
template<class T>
void TileFatherMonTool::SetBinLabel(T* axis, const std::vector<std::string>& labels) {
  unsigned int bin(1);
  for (const std::string& label : labels) {
    axis->SetBinLabel(bin, label.c_str());
    ++bin;
  }
}

//Generic Method to set the bin labels of an axis
template<class T>
void TileFatherMonTool::SetBinLabel(T* axis, const std::string* labels, int nLabels) {
  if (nLabels < 0) return;
  for (unsigned int bin = 1; bin <= (unsigned int) nLabels; ++bin) {
    axis->SetBinLabel(bin, (labels[bin - 1]).c_str());
  }
}

// Method to move bins of TProfile histogra,
// Used for online to display only latest 100 LB
void TileFatherMonTool::ShiftTprofile(TProfile* histo, int delta_lb) {

  if (delta_lb < 1) return;
  int Nbins = histo->GetNbinsX();
  double total_entries = 0.;
  int i;
  for (i = 1; i <= Nbins - delta_lb; i++) {
    double entries = histo->GetBinEntries(i + delta_lb);
    double content = histo->GetBinContent(i + delta_lb);
    total_entries += entries;
    histo->SetBinEntries(i, entries);
    histo->SetBinContent(i, content * entries);
    histo->SetBinError(i, content * sqrt(entries));
  }

  for (; i <= 100; i++) { // set rest to zero

    histo->SetBinEntries(i, 0.);
    histo->SetBinContent(i, 0.);
    histo->SetBinError(i, 0.);
  }

  histo->ResetStats();
  histo->SetEntries(total_entries);
}


bool TileFatherMonTool::m_is_collision = false;
unsigned int TileFatherMonTool::m_lastevent = 0;
float TileFatherMonTool::m_time_difference = 999.;

template void TileFatherMonTool::SetBinLabel<TAxis>(TAxis*, const std::vector<std::string>&);
template void TileFatherMonTool::SetBinLabel<TAxis>(TAxis*, const std::string*, int);

template void TileFatherMonTool::SetBinLabel<LWHist::LWHistAxis>(LWHist::LWHistAxis*, const std::vector<std::string>&);
template void TileFatherMonTool::SetBinLabel<LWHist::LWHistAxis>(LWHist::LWHistAxis*, const std::string*, int);
