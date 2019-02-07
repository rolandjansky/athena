/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TileCellCorrection.h"


// Atlas includes
#include "AthenaKernel/errorcheck.h"

// Calo includes
#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/IdentifierHash.h"

// Tile includes
#include "CaloIdentifier/TileID.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileEvent/TileCell.h"
#include "TileConditions/TileCondToolEmscale.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileBadChanTool.h"
#include "TileConditions/TileCondToolNoiseSample.h"
#include "TileRecUtils/TileCellNoiseFilter.h"
#include "TileRecUtils/TileCellMaskingTool.h"

CaloCell_ID::SUBCALO TileCellCorrection::m_caloIndex = CaloCell_ID::TILE;

/**
 * Constructor
 */
TileCellCorrection::TileCellCorrection(const std::string name,
    ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator)
  , m_caloCellContainerID("AllCalo")
  , m_correctTime(false)
  , m_correctEnergy(false)
  , m_maskBadChannels(false)
  , m_doTileCellNoiseFilter(false)
  , m_doTileCellMasking(false)
  , m_partScaleFactors(0)
  , m_zeroEnergy(0.5/* * MeV */)
  , m_tileBadChanTool("TileBadChanTool")
  , m_tileToolEmscale("TileCondToolEmscale")
  , m_tileToolTiming("TileCondToolTiming")
  , m_tileCellNoiseFilter("TileCellNoiseFilter")
  , m_tileCellMaskingTool("TileCellMaskingTool")
  , m_tileCellNeighborsTool("CaloCellNeighborsAverageCorr")
  , m_tileID(0)
  , m_tileHWID(0)
  , m_evtStore("StoreGateSvc/StoreGateSvc", name)
  , m_detStore("StoreGateSvc/DetectorStore", name)
{
  declareProperty("CaloCellContainer", m_caloCellContainerID);
  declareProperty("CorrectTime", m_correctTime);
  declareProperty("CorrectEnergy", m_correctEnergy);
  declareProperty("MaskBadChannels", m_maskBadChannels);
  declareProperty("TileCellNoiseFilter", m_doTileCellNoiseFilter);
  declareProperty("TileCellMasking", m_doTileCellMasking);
  declareProperty("TileCellNeighbors", m_doTileCellNeighbors);
  declareProperty("PartitionScaleFactors", m_partScaleFactors);
  declareProperty("BadChannelZeroEnergy", m_zeroEnergy);

  declareProperty("TileBadChanTool", m_tileBadChanTool);
  declareProperty("TileCondToolEmscale", m_tileToolEmscale);
  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("TileCellNoiseFilterTool", m_tileCellNoiseFilter);
  declareProperty("TileCellMaskingTool", m_tileCellMaskingTool);
  declareProperty("TileCellNeighborsTool", m_tileCellNeighborsTool);

  declareProperty("EvtStore", m_evtStore = StoreGateSvc_t("StoreGateSvc", name),
      "Handle to a StoreGateSvc instance: it will be used to "
      "retrieve data during the course of the job");

  declareProperty("DetStore",
      m_detStore = StoreGateSvc_t("StoreGateSvc/DetectorStore", name),
      "Handle to a StoreGateSvc/DetectorStore instance: it will be used to "
      "retrieve data during the course of the job");
}

/**
 * Destructor
 */
TileCellCorrection::~TileCellCorrection() {
}

/**
 * Initializer
 */
StatusCode TileCellCorrection::initialize() {
  ATH_MSG_INFO( "in initialize()");
  ATH_MSG_INFO( "Correct time from DB: " << ((m_correctTime) ? "true" : "false") );
  ATH_MSG_INFO( "Correct energy from DB: " << ((m_correctEnergy) ? "true" : "false") );
  ATH_MSG_INFO( "Mask bad channels: " << ((m_maskBadChannels) ? "true" : "false") );
  ATH_MSG_INFO( "Tile cell masking: " << ((m_doTileCellMasking) ? "true" : "false") );
  ATH_MSG_INFO( "Tile cell NeighborsAverageCorr: " << ((m_doTileCellNeighbors) ? "true" : "false") );
  ATH_MSG_INFO( "Apply noise filter: " << ((m_doTileCellNoiseFilter) ? "true" : "false") );
  ATH_MSG_INFO( "Energy scale factors from jobOptions: ");

  for (size_t i = 0; i < m_partScaleFactors.size(); ++i)
    msg(MSG::INFO) << m_partScaleFactors[i];

  msg(MSG::INFO) << endmsg;

  if (m_correctEnergy && m_partScaleFactors.size() > 0) {
    ATH_MSG_WARNING( "Energy correction from DB requested, energy scale factors from JO will be ignored");
    m_partScaleFactors.resize(0);
  }

  if (m_partScaleFactors.size() > 0) {
    // to avoid calculation of (ros-1) in event loop add one more element in front
    m_partScaleFactors.insert(m_partScaleFactors.begin(), 0, 0.0);
    if (m_partScaleFactors.size() < 5) {
      m_partScaleFactors.resize(5, m_partScaleFactors[1]);
      ATH_MSG_WARNING( "Setting missing scale factors -"
                      << " LBA: " << m_partScaleFactors[1]
                      << " LBC: " << m_partScaleFactors[2]
                      << " EBA: " << m_partScaleFactors[3]
                      << " EBC: " << m_partScaleFactors[4]);
    }
  }

  if (m_caloCellContainerID.length() > 0
      && (m_correctTime || m_correctEnergy || m_maskBadChannels
          || m_doTileCellNoiseFilter || m_doTileCellMasking
          || m_doTileCellNeighbors || m_partScaleFactors.size() > 0)) {

    // Listen for begin of event
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
    CHECK(incSvc.retrieve());
    incSvc->addListener(this, "BeginEvent", 100);

    ATH_MSG_INFO( "TileCellCorrection created " << "and BeginOfEvent activated " );

  } else {
    ATH_MSG_INFO( "TileCellCorrection created, but BeginOfEvent incident NOT activated " );
  }

  CHECK( m_detStore->retrieve(m_tileID));
  CHECK( m_detStore->retrieve(m_tileHWID));

  //=== get TileBadChanTool
  CHECK( m_tileBadChanTool.retrieve());

  //=== get TileCondToolEmscale
  CHECK( m_tileToolEmscale.retrieve());

  //=== get TileCondToolTiming
  CHECK( m_tileToolTiming.retrieve());

  //=== get TileCellNoiseFilter
  if (m_doTileCellNoiseFilter) {
    CHECK( m_tileCellNoiseFilter.retrieve());
  }

  //=== get TileCellMaskingTool
  if (m_doTileCellMasking) {
    CHECK( m_tileCellMaskingTool.retrieve());
  }

  //=== get CaloCellNeighborsAverageCorr
  if (m_doTileCellNeighbors) {
    CHECK( m_tileCellNeighborsTool.retrieve());
  }

  ATH_MSG_INFO( "GeoInit initialization completed");

  return StatusCode::SUCCESS;
}

StatusCode TileCellCorrection::finalize() {
  ATH_MSG_INFO( "Finalizing");
  return StatusCode::SUCCESS;
}

/****************************************************************************
 * Implementation of Incident listener
 ****************************************************************************/
void TileCellCorrection::handle(const Incident& inc ) {

  bool verbose = msgLvl(MSG::VERBOSE);

  static bool first = true;
  if (first) {
    first = false;
    ATH_MSG_INFO( "Handling First BeginEvent incident");
  } else {
    ATH_MSG_INFO( "Handling BeginEvent incident");
  }

  const CaloCellContainer * caloCellContainer;

  if (m_evtStore->retrieve(caloCellContainer, m_caloCellContainerID).isFailure()) {
    ATH_MSG_DEBUG( " Could not find container " << m_caloCellContainerID);
    return;
  }

  int nCells = caloCellContainer->nCellsCalo(m_caloIndex);
  ATH_MSG_DEBUG( "Container " << m_caloCellContainerID << " with "
      << nCells << " TileCells found " );

  if (nCells <= 0) {
    ATH_MSG_DEBUG( " No TileCells in the container - nothing to do ");
    return;
  }

  if (m_correctTime || m_correctEnergy || m_maskBadChannels
      || m_partScaleFactors.size() > 0) {

    if (verbose) {
      double energy = totalEnergy(caloCellContainer);
      msg(MSG::VERBOSE) << "Before any correction total energy = " << energy << endmsg;
    }

    size_t cellItr = caloCellContainer->indexFirstCellCalo(m_caloIndex);
    size_t lastCell = caloCellContainer->indexLastCellCalo(m_caloIndex);

    int nCorrected = 0;
    for (; cellItr <= lastCell; ++cellItr) {
      const CaloCell* CCell = (*caloCellContainer)[cellItr];
      const TileCell* TCell = dynamic_cast<const TileCell*>(CCell);
      if (TCell == 0) continue;

      TileCell* cell = (TileCell*) TCell;
      ++nCorrected;

      const CaloDetDescrElement * caloDDE = CCell->caloDDE();

      IdentifierHash hash1 = caloDDE->onl1();
      IdentifierHash hash2 = caloDDE->onl2();

      bool twoChan = (hash2 != TileHWID::NOT_VALID_HASH);

      int gain1 = cell->gain1();
      HWIdentifier adc_id1 = m_tileHWID->adc_id(hash1, gain1);
      int ros1 = m_tileHWID->ros(adc_id1);
      int drawer1 = m_tileHWID->drawer(adc_id1);
      int chan1 = m_tileHWID->channel(adc_id1);
      int drawerIdx1 = TileCalibUtils::getDrawerIdx(ros1, drawer1);
      bool timePresent1 = (cell->qbit1() >= TileCell::KEEP_TIME);
      bool bad1 = m_tileBadChanTool->getAdcStatus(adc_id1).isBad();

      if (twoChan) {
        int gain2 = cell->gain2();
        HWIdentifier adc_id2 = m_tileHWID->adc_id(hash2, gain2);
        int ros2 = m_tileHWID->ros(adc_id2);
        int drawer2 = m_tileHWID->drawer(adc_id2);
        int chan2 = m_tileHWID->channel(adc_id2);
        int drawerIdx2 = TileCalibUtils::getDrawerIdx(ros2, drawer2);
        bool timePresent2 = (cell->qbit2() >= TileCell::KEEP_TIME);
        bool bad2 = m_tileBadChanTool->getAdcStatus(adc_id2).isBad();

        ATH_MSG_VERBOSE( "Cell ID = "
            << m_tileID->to_string(caloDDE->identify(),-2)
            << "  ADC1 = " << m_tileHWID->to_string(adc_id1,0)
            << "  ADC2 = " << m_tileHWID->to_string(adc_id2,0) );

        if (!(m_maskBadChannels && bad1 && bad2)) {
          if (m_correctTime) {// assume that in database we have correction to old time
            if (timePresent1 || timePresent2) {
              if (verbose) {
                msg(MSG::VERBOSE) << "Time before:"
                    << ((timePresent1) ? " (" : " (Z_") << cell->time1()
                    << ((timePresent2) ? ", " : ", Z_") << cell->time2() << ")";
              }

              float time1 = ((timePresent1)
                  ? cell->time1() + m_tileToolTiming->getSignalPhase(drawerIdx1, chan1, gain1)
                  : 0.0);

              float time2 = ((timePresent2)
                  ? cell->time2() + m_tileToolTiming->getSignalPhase(drawerIdx2, chan2, gain2)
                  : 0.0);

              cell->setTime(time1);   // overwrite time completely
              cell->setTime(time2, 1); // calculate average time and timeDiff

              if (verbose)
                msg(MSG::VERBOSE) << " after: (" << cell->time1() << ", " << cell->time2() << ")" << endmsg;

            } else {
              ATH_MSG_VERBOSE( "Time before: (Z_" << cell->time1() << ", Z_"
                  << cell->time2() << ") - should be zero and not corrected");
            }
          }

          if (m_correctEnergy) {// assume that in online folder we have old scale in offline folder - new scale
            /*
             // one way to change energy scale is to go from Online to Offline MeV
             // but in this case we apply non-linear CIS corrections twice, which is not nice
             float ene1 = m_tileToolEmscale->channelCalib(drawerIdx1, chan1, gain1, cell->ene1(),
             TileRawChannelUnit::OnlineMegaElectronVolts,
             TileRawChannelUnit::MegaElectronVolts);
             float ene2 = m_tileToolEmscale->channelCalib(drawerIdx2, chan2, gain2, cell->ene2(),
             TileRawChannelUnit::OnlineMegaElectronVolts,
             TileRawChannelUnit::MegaElectronVolts);
             */
            // another way to change energy scale is to use constant which contains correction itself
            // in our case it's laser correction factor (it was set to new value for reporcessing)
            if (verbose)
              msg(MSG::VERBOSE) << "Ene  before: (" << cell->ene1() << ", " << cell->ene2() << ")";

            float ene1 = m_tileToolEmscale->doCalibLas(drawerIdx1, chan1, cell->ene1());
            float ene2 = m_tileToolEmscale->doCalibLas(drawerIdx2, chan2, cell->ene2());
            cell->setEnergy(ene1, ene2); // set two energies simultaneously
            if (verbose)
              msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ", "
                  << cell->ene2() << ")" << endmsg;
          }

          if (m_partScaleFactors.size() > 0) {
            if (verbose)
              msg(MSG::VERBOSE) << "Ene  before: (" << cell->ene1() << ", " << cell->ene2() << ")";

            float ene1 = cell->ene1() * m_partScaleFactors[ros1];
            float ene2 = cell->ene1() * m_partScaleFactors[ros2];
            cell->setEnergy(ene1, ene2); // set two energies simultaneously
            if (verbose)
              msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ", "
                  << cell->ene2() << ")" << endmsg;
          }
        }

        if (m_maskBadChannels) {
          if (verbose && (bad1 || bad2)) {
            msg(MSG::VERBOSE) << "Bad  status: (" << ((bad1) ? "B" : "G") << ","
                << ((bad2) ? "B" : "G") << ")" << endmsg;

            msg(MSG::VERBOSE) << "E,t  before mask: (" << cell->ene1() << ","
                << cell->ene2() << ") (" << cell->time1() << ","
                << cell->time2() << ")";
          }

          if (bad1 && bad2) {
            // both channels are bad
            cell->setEnergy(m_zeroEnergy, m_zeroEnergy, TileID::LOWGAIN,
                TileID::LOWGAIN); // reset energy completely, indicate problem putting low gain
            cell->setTime(0.0);  // reset time completely
            cell->setQuality(255, TileCell::MASK_BADCH, 0); // reset quality flag for first pmt
            cell->setQuality(255, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt
          } else if (bad1 && !bad2) {
            float ene2 = cell->ene2();

            cell->setEnergy(ene2, ene2, gain2, gain2); // use energy/gain from second pmt for both pmts
            cell->setTime(cell->time2()); // use time from second pmt as cell time
            cell->setQuality(cell->qual2(), (cell->qbit2() | TileCell::MASK_BADCH), 0); // change quality flag for first pmt

          } else if (!bad1 && bad2) {
            float ene1 = cell->ene1();

            cell->setEnergy(ene1, ene1, gain1, gain1); // use energy/gain from first pmt for both pmts
            cell->setTime(cell->time1()); // use time from first pmt as cell time
            cell->setQuality(cell->qual1(), (cell->qbit1() | TileCell::MASK_BADCH), 1); // change quality flag for second pmt
          }

          if (verbose && (bad1 || bad2)) {
            msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ","
                << cell->ene2() << ") (" << cell->time1() << ","
                << cell->time2() << ")" << endmsg;
          }

        }
      } else { // one channel
        if (verbose)
          msg(MSG::VERBOSE) << "Cell ID = "
              << m_tileID->to_string(caloDDE->identify(), -2) << "  ADC1 = "
              << m_tileHWID->to_string(adc_id1, 0) << "  ADC2 - doesn't exist "
              << endmsg;

        if (!(m_maskBadChannels && bad1)) {
          if (m_correctTime) {// assume that in database we have correction to old time
            if (timePresent1) {
              if (verbose)
                msg(MSG::VERBOSE) << "Time before: (" << cell->time1() << ", X_"
                    << cell->time2() << ")";

              float time = cell->time1() + m_tileToolTiming->getSignalPhase(drawerIdx1, chan1, gain1);
              cell->setTime(time); // overwrite time completely
              if (verbose)
                msg(MSG::VERBOSE) << " after: (" << cell->time1() << ", X_"
                    << cell->time2() << ")" << " one channel only " << endmsg;

            } else {
              if (verbose)
                msg(MSG::VERBOSE) << "Time before: (Z_" << cell->time1()
                    << ", X_" << cell->time2()
                    << ") - should be zero and not corrected, one channel only"
                    << endmsg;
            }
          }

          if (m_correctEnergy) { // assume that in online folder we have old scale in online folder - new scale

            /*
             // one way to change energy scale is to go from Online to Offline MeV
             // but in this case we apply non-linear CIS corrections twice, which is not nice
             float ene = m_tileToolEmscale->channelCalib(drawerIdx1, chan1, gain1, cell->ene1(),
             TileRawChannelUnit::OnlineMegaElectronVolts,
             TileRawChannelUnit::MegaElectronVolts);
             */
            // another way to change energy scale is to use constant which contains correction itself
            // in our case it's laser correction factor (it was set to new value for reporcessing)
            if (verbose)
              msg(MSG::VERBOSE) << "Ene  before: (" << cell->ene1() << ", X_" << cell->ene2() << ")";

            float ene = m_tileToolEmscale->doCalibLas(drawerIdx1, chan1, cell->ene1());
            cell->setEnergy(ene, 0.0); // set two energies simultaneously
            if (verbose)
              msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ", X_"
                  << cell->ene2() << ")" << " one channel only " << endmsg;
          }

          if (m_partScaleFactors.size() > 0) {
            if (verbose)
              msg(MSG::VERBOSE) << "Ene  before: (" << cell->ene1() << ", X_" << cell->ene2() << ")";

            float ene = cell->ene1() * m_partScaleFactors[ros1];
            cell->setEnergy(ene, 0.0); // set two energies simultaneously
            if (verbose)
              msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ", X_"
                  << cell->ene2() << ")" << " one channel only " << endmsg;
          }
        }

        if (m_maskBadChannels) {
          if (bad1) {
            if (verbose) {
              msg(MSG::VERBOSE) << "Bad  status: (B,X)" << endmsg;
              msg(MSG::VERBOSE) << "E,t  before mask: (" << cell->ene1()
                  << ",X_" << cell->ene2() << ") (" << cell->time1() << ",X_"
                  << cell->time2() << ")";
            }
            cell->setEnergy(m_zeroEnergy, 0.0, TileID::LOWGAIN,
                CaloGain::INVALIDGAIN); // reset energy completely, indicate problem putting low gain
            cell->setTime(0.0); // reset time completely
            cell->setQuality(255, TileCell::MASK_BADCH, 0); // reset quality flag for first pmt
            cell->setQuality(0, TileCell::MASK_BADCH, 1); // reset quality flag for second pmt

            if (verbose) {
              msg(MSG::VERBOSE) << " after: (" << cell->ene1() << ",X_"
                  << cell->ene2() << ") (" << cell->time1() << ",X_"
                  << cell->time2() << ")" << " one channel only " << endmsg;
            }
          }
        }
      }
    }

    ATH_MSG_DEBUG( "After corrections " << nCorrected << " cells out of "
        << nCells << "corrected" );
  }

  // masking additional drawers
  if (m_doTileCellMasking) {
    if (verbose) {
      double energy = totalEnergy(caloCellContainer);
      msg(MSG::VERBOSE) << "Before cell masking total energy = " << energy << endmsg;
    }

    if (m_tileCellMaskingTool->process((CaloCellContainer *) caloCellContainer, inc.context()).isFailure()) {
      ATH_MSG_ERROR( " failed to run TileCellMaskingTool " );
    }
  }

  // apply noise filter
  if (m_doTileCellNoiseFilter) {
    if (verbose) {
      double energy = totalEnergy(caloCellContainer);
      msg(MSG::VERBOSE) << "Before noise filter total energy = " << energy << endmsg;
    }

    if (m_tileCellNoiseFilter->process((CaloCellContainer*) caloCellContainer, inc.context()).isFailure()) {
      ATH_MSG_ERROR( " failed to run TileCellNoiseFilter " );
    }
  }

  // apply average neighbor cell correction
  if (m_doTileCellNeighbors) {
    if (verbose) {
      double energy = totalEnergy(caloCellContainer);
      msg(MSG::VERBOSE) << "Before average neighbor cell correction"
                        << " total energy = " << energy << endmsg;
    }

    if (m_tileCellNeighborsTool->process((CaloCellContainer *) caloCellContainer, inc.context()).isFailure()) {
      ATH_MSG_ERROR( " failed to run CaloCellNeighborsAverageCorr " );
    }
  }

  if (verbose) {
    double energy = totalEnergy(caloCellContainer);
    msg(MSG::VERBOSE) << "After all corrections total energy = " << energy << endmsg;
  }

  ATH_MSG_DEBUG( "End of handling BeginEvent incident " );
}

double TileCellCorrection::totalEnergy(const CaloCellContainer * caloCellContainer) {

  size_t cellItr = caloCellContainer->indexFirstCellCalo(m_caloIndex);
  size_t lastCell = caloCellContainer->indexLastCellCalo(m_caloIndex);

  double energy = 0.0;
  for (; cellItr <= lastCell; ++cellItr) {
    energy += (*caloCellContainer)[cellItr]->energy();
  }

  return energy;
}

