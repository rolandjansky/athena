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

// Athena includes
#include "xAODEventInfo/EventInfo.h"
#include "AthenaKernel/errorcheck.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/TileID.h"

#include "TileEvent/TileContainer.h"
#include "TileEvent/TileCell.h"
#include "TileMonitoring/TileCellMonTool.h"
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingService.h"

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

#include "TileMonitoring/TileFatherMonTool.h"

/*---------------------------------------------------------*/
/// Methods registering historgrams
/// Ownership passed to Gaudi
template <typename T>
void TileFatherMonTool::regHist(const std::string path, T * hist, Interval_t interval, MgmtAttr_t attribute,  std::string trigChain, std::string mergeAlgo)
{
  if(ManagedMonitorToolBase::regHist(hist, path, interval, attribute, trigChain, mergeAlgo).isFailure()) {
    ATH_MSG_WARNING( "Could not register histogram : "  << "/" + path + "/" + hist->GetName() );
  }
 
}

template <typename T>
void TileFatherMonTool::regGraph(const std::string path, T * graph, Interval_t interval, MgmtAttr_t attribute,  std::string trigChain, std::string mergeAlgo)
{
  if(ManagedMonitorToolBase::regGraph(graph, path, interval, attribute, trigChain, mergeAlgo) != StatusCode::SUCCESS) {
    ATH_MSG_WARNING( "Could not register Graph : " << path );
  }	 
}

///Base class for TileCal monitoring tools
/*---------------------------------------------------------*/
TileFatherMonTool::TileFatherMonTool(const std::string & type, const std::string & name, const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent)
  , m_lvl1info(0)
  , m_evtNum(0)
  , m_lumiBlock(0)
  , m_evtBCID(0)
  , m_runNum(0)
  , m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool")
  , m_tileID(0)
  , m_tileHWID(0)
  , m_cabling(0)
/*---------------------------------------------------------*/
{
  declareInterface<IMonitorToolBase>(this);

  // the same variable is property THistSvc_OutStream in MonitorToolBase
  declareProperty("histoStreamName", m_THistSvc_streamname = "/SHIFT");

  // property histoPathBase in MonitorToolBase
  declareProperty("histoPathBase", m_path = "Tile");
  declareProperty("MBTSCellContainerID", m_MBTSCellContainerID = "MBTSContainer");
  declareProperty("savePng", m_savePng = false);
  declareProperty("savePs", m_savePs = false);

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


  //NB Changed for new ManagedMonitorMonTool 
  //m_stem=m_THistSvc_streamname+m_path;
  m_stem = "/" + m_path;

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
  m_TrigNames[Trig_b1] = "bit1_BPTX";
  m_TrigNames[Trig_b2] = "bit2_L1CAL";
  m_TrigNames[Trig_b3] = "bit3_TGC";
  m_TrigNames[Trig_b4] = "bit4_RPC";
  m_TrigNames[Trig_b5] = "bit5_MBTS";
  m_TrigNames[Trig_b6] = "bit6_COSM";
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

  CHECK(detStore()->retrieve(m_tileID));

  CHECK(detStore()->retrieve(m_tileHWID));

  m_cabling = TileCablingService::getInstance();

  //done explicitly
  //ToolRootHistSvc();

  //SetBookStatus(false);
  CHECK(ManagedMonitorToolBase::initialize());

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileFatherMonTool::bookHistograms() {
/*---------------------------------------------------------*/

  if (msgLvl(MSG::DEBUG)) {
    msg(MSG::DEBUG) << "in bookHistograms()" << endmsg;
    msg(MSG::DEBUG) << "Using base path " << m_stem << endmsg;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileFatherMonTool::fillHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in fillHistograms()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileFatherMonTool::procHistograms() {
/*---------------------------------------------------------*/

  ATH_MSG_DEBUG( "in procHistograms()" );

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode TileFatherMonTool::checkHists(bool /* fromFinalize */) {
/*---------------------------------------------------------*/


  ATH_MSG_DEBUG( "in checkHists()" );

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

  } else {

    m_lvl1info = eventInfo->level1TriggerType();
    m_evtNum = eventInfo->eventNumber();
    m_lumiBlock = eventInfo->lumiBlock();
    m_evtBCID = eventInfo->bcid();
    m_runNum = eventInfo->runNumber();

//    const std::vector< xAOD::EventInfo::StreamTag >& evtStreamTags = eventInfo->streamTags();
//    for (const auto& evtStreamTag : evtStreamTags) {
//      m_streamTagTypes.push_back(evtStreamTag.type());
//      m_streamTagNames.push_back(evtStreamTag.name());
//    }

  }

  if (m_evtNum != TileFatherMonTool::m_lastevent) {
    TileFatherMonTool::m_lastevent = m_evtNum;
    collcand();
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

        if ((lvl1info >> exp) & 1) m_eventTrigs.push_back(exp); // store active trigger

      }
    } else { //calibration event foramt is 0x0aaaaaaa
      m_eventTrigs.push_back(Trig_b7);
    }

  } else {//close if lvl1 word not empty

    m_eventTrigs.push_back(AnyTrig); //always store at least AnyPhysTrig (=8)
  }
}

/*---------------------------------------------------------*/
void TileFatherMonTool::collcand() {
  /*---------------------------------------------------------*/

  TileFatherMonTool::m_tdiff = -999.;
  TileFatherMonTool::m_iscoll = false;

//  Int_t MBTS_cont = -1;		//Lukas
//  Int_t ECells_cont = -1;		//Lukas
//  Float_t t_diff_MBTS = -999;		//Lukas
//  Float_t t_diff_ECells = -999;		//Lukas

  if (m_MBTSCellContainerID.size() > 0) {

    const TileCellContainer* theMBTScontainer;

//    MBTS_cont = 1;	//Lukas

    if (evtStore()->retrieve(theMBTScontainer, m_MBTSCellContainerID).isFailure()) {
      ATH_MSG_INFO( "Can't find TileCellContainer with name " << m_MBTSCellContainerID
                    << " in TES. No CollCand evaluation possible!" );

      ATH_MSG_INFO( "Will not try to read this container anymore!" );

      m_MBTSCellContainerID = "";

      // MBTS_cont=2;	//Lukas
    } else {
      // MBTS_cont=3;	//Lukas

      int tHitsA = 0, tHitsC = 0;
      double timeA = 0., timeC = 0.;
      int counter = -1;

      TileCellContainer::const_iterator iCell = theMBTScontainer->begin();
      TileCellContainer::const_iterator lastCell = theMBTScontainer->end();

      for (; iCell != lastCell; ++iCell) {
        Identifier id;
        const TileCell* mbtsCell = *iCell;   // pointer to cell object
        if (mbtsCell != 0) {
          id = mbtsCell->ID();

          // Calculate MBTS counter from "side", "tower" and "module"
          // Counter goes 0-31.
          // EBA-inner:0-7,EBA-outer:8-15,EBC-inner:16-23,EBC-outer:24-31
          // tower is 1 for outer counter (lower eta) and 0 for inner counter (higher eta)
          // module counts from 0-7 in increasing phi
          // side is -1 for EBC 1 for EBA
          //int counter = (m_tileID->module(id)*2+1) - m_tileID->tower(id);
          counter = (m_tileID->module(id)) + 8 * (m_tileID->tower(id));
          if (m_tileID->side(id) < 0) // EBC side
            counter += 16;

          if (mbtsCell->energy() < 0.27) continue; //threshold approx 60./222.
          if (TMath::Abs(mbtsCell->time()) < 1.e-5) continue; //

          if (counter < 0) continue;
          if (counter < 16) {
            tHitsA++;
            timeA += mbtsCell->time();
          } else {
            tHitsC++;
            timeC += mbtsCell->time();
          }

        } // end if cell not empty
      } // end Cell loop

      if (tHitsA > 1 && tHitsC > 1) {
        TileFatherMonTool::m_tdiff = timeA / tHitsA - timeC / tHitsC;
//        t_diff_MBTS = TileFatherMonTool::m_tdiff;	//Lukas testing

        if (TMath::Abs(TileFatherMonTool::m_tdiff) < 7.5) {
          TileFatherMonTool::m_iscoll = true;
          // MBTS_cont = 4;	//Lukas
        }
      }
    }
  } else {
    // MBTS_cont = 0;	//Lukas
  }

  //Lukas
  //if(MBTS_cont != 4){
  if (false) {	//only for testing

    //Lukas; code from TileCellMonTool.cxx

    // Pointer to a Tile cell container
    const CaloCellContainer* cell_container;

    std::string cellsContName = "AllCalo";

    //Retrieve Cell collection from SG

    if (evtStore()->retrieve(cell_container, cellsContName).isFailure()) {
      ATH_MSG_WARNING( "TileCellMonTool: Retrieval of Tile cells from container " << cellsContName << " failed" );

      // ECells_cont=0;
    } else {
      int tHitsA = 0, tHitsC = 0;
      double timeA = 0., timeC = 0.;

      // ECells_cont=1;

      CaloCellContainer::const_iterator iCell = cell_container->begin();
      CaloCellContainer::const_iterator lastCell = cell_container->end();

      for (; iCell != lastCell; ++iCell) {

        const CaloCell* cell_ptr = *iCell;     // pointer to cell object
        Identifier id = cell_ptr->ID();

        if (m_tileID->is_tile(id)) {

          const TileCell* tile_cell = dynamic_cast<const TileCell*>(cell_ptr);
          if (tile_cell == 0) continue;

          int samp = std::min(m_tileID->sample(id), (int) AllSamp);
          int side = m_tileID->side(id);

          // get the cell energy, time
          double time = cell_ptr->time();
          double energy = cell_ptr->energy();

          if (energy < 0.27) continue; //threshold approx 60./222. ???????????????????????????????????? is this same for ECells as for MBTS?
          if (TMath::Abs(time) < 1.e-5) continue;
          if (side > 0) {
            if (samp == 3) {
              tHitsA++;
              timeA += time;
            }

          } else if (side < 0) {
            if (samp == 3) {
              tHitsC++;
              timeC += time;
            }
          } //if partition
        } // end if tile_cell
      } // end of loop over the Cells

      if (tHitsA > 1 && tHitsC > 1) {
        TileFatherMonTool::m_tdiff = timeA / tHitsA - timeC / tHitsC;
        // t_diff_ECells = TileFatherMonTool::m_tdiff;	//Lukas testing

        if (TMath::Abs(TileFatherMonTool::m_tdiff) < 7.5) {
          TileFatherMonTool::m_iscoll = true;
        }
      }
    } //sc1 else

  } //if(MBTS)
  /*
   //++++++Testing++++++
  if (MBTS_cont < 0) ATH_MSG_WARNING( MSG::WARNING << "MBTS_cont==-1" );
  if (ECells_cont < 0) ATH_MSG_WARNING( "ECells_cont==-1" );

  // MBTSvsECells possible values> 0;1;2;3;

  if ((MBTS_cont == 3) == false && (ECells_cont == 1) == false) { //MBTS and ECells fail
    if (TileFatherMonTool::m_iscoll) {
      ATH_MSG_WARNING( "MBTSvsECells	0	is_coll	1	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    } else {
      ATH_MSG_WARNING( "MBTSvsECells	0	is_coll	0	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    }
  }

  if ((MBTS_cont == 3) == true && (ECells_cont == 1) == false) { //MBTS works and ECells fails
    if (TileFatherMonTool::m_iscoll) {
      ATH_MSG_WARNING( "MBTSvsECells	1	is_coll	1	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    } else {
      ATH_MSG_WARNING( "MBTSvsECells	1	is_coll	0	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    }
  }

  if ((MBTS_cont == 3) == false && (ECells_cont == 1) == true) { //MBTS fails and ECells works
    if (TileFatherMonTool::m_iscoll) {
      ATH_MSG_WARNING( "MBTSvsECells	2	is_coll	1	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    } else {
      ATH_MSG_WARNING( "MBTSvsECells	2	is_coll	0	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    }
  }

  if ((MBTS_cont == 3) == true && (ECells_cont == 1) == true) { //MBTS and ECells works
    if (TileFatherMonTool::m_iscoll) {
      ATH_MSG_WARNING( "MBTSvsECells	3	is_coll	1	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );

    } else {
      ATH_MSG_WARNING( "MBTSvsECells	3	is_coll	0	t_diff_ECells	" << t_diff_ECells << "	t_diff_MBTS	" << t_diff_MBTS );
    }
  }
   */
  //Lukas
}

//Generic Method to set the bin labels of an axis
void TileFatherMonTool::SetBinLabel(TAxis* ax, std::vector<std::string>& labelVec) {
  unsigned int nb = labelVec.size();
  for (unsigned int b = 0; b < nb; b++) {
    ax->SetBinLabel(b + 1, (labelVec.at(b)).c_str());
  }

}

//Generic Method to set the bin labels of an axis
void TileFatherMonTool::SetBinLabel(TAxis* ax, std::string * labelVec, int nb) {
  if (nb < 0) return;
  for (unsigned int b = 0; b < (unsigned int) nb; b++) {
    ax->SetBinLabel(b + 1, (labelVec[b]).c_str());
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
  for (; i <= 100; i++) // set rest to zero
      {
    histo->SetBinEntries(i, 0.);
    histo->SetBinContent(i, 0.);
    histo->SetBinError(i, 0.);
  }
  histo->ResetStats();
  histo->SetEntries(total_entries);
}

/*---------------------------------------------------------*/
/// Method booking 1D Histograms and storing them in THistSvc
/// The method return the pointer to the new histogram
/// Argument sudir the path to to the histograms
/// nam is the histogram name
/// tit is the histogram title
/// nx the number of bins
/// xmin the the low limit of the histogram
/// xmax is the high limit of the histogram
TH1D* TileFatherMonTool::book1D(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1D* hist = new TH1D(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1F* TileFatherMonTool::book1F(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1F* hist = new TH1F(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1C* TileFatherMonTool::book1C(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{

  TH1C* hist = new TH1C(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1S* TileFatherMonTool::book1S(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{

  TH1S* hist = new TH1S(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH1I* TileFatherMonTool::book1I(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{

  TH1I* hist = new TH1I(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

/*---------------------------------------------------------*/
/// Method booking 2D Histograms and storing them in THistSvc
/// The method return the pointer to the new histogram
TH2D* TileFatherMonTool::book2D(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, double ymin, double ymax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2D* hist = new TH2D(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2F* TileFatherMonTool::book2F(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                int ny, double ymin, double ymax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{
  TH2F* hist = new TH2F(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2F* TileFatherMonTool::book2F(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, const double* ybins,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2F* hist = new TH2F(TString(nam), TString(tit), nx, xmin, xmax, ny, ybins);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2I* TileFatherMonTool::book2I(std::string subdir, std::string nam, std::string tit,
                                 int nx, double xmin, double xmax,
                                 int ny, double ymin, double ymax,
                                 Interval_t interval, MgmtAttr_t attribute,
                                 std::string trigChain, std::string mergeAlgo)
{
  TH2I* hist = new TH2I(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2S * TileFatherMonTool::book2S(std::string subdir, std::string nam, std::string tit,
                                  int nx, double xmin, double xmax,
                                  int ny, double ymin, double ymax,
                                  Interval_t interval, MgmtAttr_t attribute,
                                  std::string trigChain, std::string mergeAlgo)
{
  TH2S *hist = new TH2S(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TH2C* TileFatherMonTool::book2C(std::string subdir, std::string nam, std::string tit,
                                int nx, double xmin, double xmax,
                                int ny, double ymin, double ymax,
                                Interval_t interval, MgmtAttr_t attribute,
                                std::string trigChain, std::string mergeAlgo)
{
  TH2C* hist = new TH2C(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile* TileFatherMonTool::bookProfile(std::string subdir, std::string nam, std::string tit,
                                          int nx, double xmin, double xmax,
                                          Interval_t interval, MgmtAttr_t attribute,
                                          std::string trigChain, std::string mergeAlgo)
{
  TProfile* hist = new TProfile(TString(nam), TString(tit), nx, xmin, xmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile* TileFatherMonTool::bookProfile(std::string subdir, std::string nam, std::string tit,
                                          int nx, double xmin, double xmax,
                                          double ymin, double ymax,
                                          Interval_t interval, MgmtAttr_t attribute,
                                          std::string trigChain, std::string mergeAlgo)
{
  TProfile* hist = new TProfile(TString(nam), TString(tit), nx, xmin, xmax, ymin, ymax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}

TProfile2D* TileFatherMonTool::bookProfile2D(std::string subdir, std::string nam, std::string tit,
                                             int nx, double xmin, double xmax,
                                             int ny, double ymin, double ymax,
                                             double zmin, double zmax,
                                             Interval_t interval, MgmtAttr_t attribute,
                                             std::string trigChain, std::string mergeAlgo)
{

  TProfile2D* hist = new TProfile2D(TString(nam), TString(tit), nx, xmin, xmax, ny, ymin, ymax, zmin, zmax);
  regHist(subdir, hist, interval, attribute, trigChain, mergeAlgo);
  return hist;
}


/*-----------------------------------------------------------*/
/// Method booking TTree and TGraph and storing them in THistSvc
/// The method return the pointer to the new histogram
TTree* TileFatherMonTool::bookTree(std::string subdir, std::string nam, std::string tit) {

  TTree* hist = new TTree(TString(nam), TString(tit));
  if (m_THistSvc_streamname.size() > 0) {
    if (m_THistSvc->regTree(m_stem + subdir + "/" + nam, hist).isFailure()) {
      ATH_MSG_WARNING( "Could not register object : " << m_stem + subdir + "/" + nam );
    }
  }
  return hist;
}

//
// Terrible hack to register TGraph in THistSvc
//

//#define private public
//#define GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.h"
//#undef GAUDISVC_THISTSVC_ICC
//#include "THistSvc/THistSvc.icc"
//#undef private

class TGraph1: public TGraph {
  public:
    TGraph1(int N, float * X, float * Y)
        : TGraph(N, X, Y), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TGraph* TileFatherMonTool::bookGraph(std::string subdir, std::string nam, std::string tit, int N, float * X, float * Y) {

  TGraph1 *hist = new TGraph1(N, X, Y);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(m_stem + subdir + "/" + nam, hist);

  return (TGraph*) hist;
}

class TGraphErrors1: public TGraphErrors {
  public:
    TGraphErrors1(int N, float * X, float * Y, float * X_errors, float * Y_errors)
        : TGraphErrors(N, X, Y, X_errors, Y_errors), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory * fDirectory;
};

TGraphErrors * TileFatherMonTool::bookGraphErrors(std::string subdir, std::string nam, std::string tit, int N, float * X, float * Y, float * X_errors, float * Y_errors) {

  TGraphErrors *hist = new TGraphErrors(N, X, Y, X_errors, Y_errors);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(m_stem + subdir + "/" + nam, hist);
  return (TGraphErrors *) hist;
}

StatusCode TileFatherMonTool::removeTObj(TObject *obj) {
  if (obj != 0) {
    if (obj->IsA()->InheritsFrom("TH1")) {
      if (deregHist((TH1*) obj).isFailure()) {
        ATH_MSG_WARNING( "Could not dereg Histogram : " << obj->GetName() );
        return StatusCode::FAILURE;
      } else {
        delete obj;
      }
    } else if (obj->IsA()->InheritsFrom("TGraph")) {
      if (deregGraph((TGraph*) obj) != StatusCode::SUCCESS) {
        ATH_MSG_WARNING( "Could not dereg Graph : " << obj->GetName() );
        return StatusCode::FAILURE;
      } else {
        delete obj;
      }
    } else {
      ATH_MSG_WARNING( "Asked to remove object " << obj->GetName() << "of unsupported type " << obj->IsA() );
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_WARNING( "Asked to remove NULL pointer" );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


class TGraphAsymmErrors1: public TGraphAsymmErrors {
  public:
    TGraphAsymmErrors1(int N, float * X, float * Y, float * X_errors1, float * X_errors2, float * Y_errors1, float * Y_errors2)
        : TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2), fDirectory(0) {
    }

    TDirectory * GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TGraphAsymmErrors* TileFatherMonTool::bookGraphAsymmErrors(std::string subdir, std::string nam, std::string tit, int N,
                                                           float* X, float* Y, float* X_errors1, float* X_errors2,
                                                           float* Y_errors1, float* Y_errors2)
{

  TGraphAsymmErrors *hist = new TGraphAsymmErrors(N, X, Y, X_errors1, X_errors2, Y_errors1, Y_errors2);
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(m_stem + subdir + "/" + nam, hist);
  return (TGraphAsymmErrors*) hist;
}

class TMultiGraph1: public TMultiGraph {
  public:
    TMultiGraph1()
        : TMultiGraph(), fDirectory(0) {
    }

    TDirectory* GetDirectory() { return fDirectory; }

    void SetDirectory(TDirectory *dir) {
      if (fDirectory == dir) return;
      if (fDirectory) fDirectory->GetList()->Remove(this);
      fDirectory = dir;
      if (fDirectory) fDirectory->GetList()->Add(this);
    }

  private:
    TDirectory* fDirectory;
};

TMultiGraph* TileFatherMonTool::bookMultiGraph(std::string subdir, std::string nam, std::string tit) {

  TMultiGraph1* hist = new TMultiGraph1();
  hist->SetName(TString(nam));
  hist->SetTitle(TString(tit));

  regGraph(m_stem + subdir + "/" + nam, (TGraph*) hist);
  return (TMultiGraph*) hist;
}

bool TileFatherMonTool::m_iscoll = false;
unsigned int TileFatherMonTool::m_lastevent = 0;
float TileFatherMonTool::m_tdiff = 999.;

