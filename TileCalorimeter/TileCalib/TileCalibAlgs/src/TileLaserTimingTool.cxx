/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Athena includes
#include "xAODEventInfo/EventInfo.h"
#include "CoralBase/Blob.h"
#include "CoralBase/AttributeListSpecification.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Tile includes
#include "TileCalibAlgs/TileLaserTimingTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"
#include "TileCalibBlobObjs/TileCalibDrawerFlt.h"

#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TObjArray.h"
#include <TROOT.h>

#include <cmath>

//const unsigned NPmts = 48;

template<class _T> inline _T sqr(const _T &data) { return data * data; }

inline int imax(int i, int j) { return i < j ? j : i; }

TileLaserTimingTool::PMTData::PMTData(const std::string &id)
 : TimeDiffHisto(("pmt_dtime"+id).c_str(),("pmt_dtime"+id).c_str(),1200,-60,60)
 , TimeDiffNoCFCorrHisto(("dtimeNoCorr"+id).c_str(),("dtimeNoCorr"+id).c_str(),1200,-60,60)

#ifdef TileLaserTimingMon
  , timeHisto(("time" + id).c_str(), ("time" + id).c_str(), 200, -100, 100)
  , eneHisto(("energy" + id).c_str(), ("energy" + id).c_str(), 300, 0., 300.)
  , gainHisto(("gain" + id).c_str(), ("gain" + id).c_str(), 2, 0, 2)
#endif

#ifdef TileLaserTimingPMT0Mon
  , TimeDiffHistoHigh(("dtime" + id + "hi").c_str(), ("dtime" + id + "hi").c_str(), 200, -40, 40)
  , TimeDiffHistoLow(("dtime" + id + "lo").c_str(), ("dtime" + id + "lo").c_str(), 200, -60, 60)
#endif

#ifdef TileLaserTimingMonPulse
  , pulseShapeHigh(("pulseShape_" + id + "hi").c_str(), ("pulseShape_" + id + "hi").c_str(), 400, -200., 300., 0, 100000.)
  , pulseShapeLow(("pulseShape_" + id + "lo").c_str(), ("pulseShape_" + id + "lo").c_str(), 400, -200., 300., 0, 100000.)
#endif

  , TimeDiffResHis(("dtimeRes"+id).c_str(),("dtimeRes"+id).c_str(),1000,-40.,40.)
  , time(0.0)

#ifdef TileLaserTimingPMT0
  , dtime(0.0)
  , dtimeNoCFCorr(0.0)
#endif

  , energy(0.0)
{
}

TileLaserTimingTool::DigitizerData::DigitizerData()
  : m_N_active(0)
  , m_mean_time(0.)
  , m_mean_rms(0.)
{
  memset(m_ch_time, 0, sizeof(m_ch_time));
}

void TileLaserTimingTool::DigitizerData::Add(float time) {
  m_ch_time[m_N_active] = time;
  ++m_N_active;
}

void TileLaserTimingTool::DigitizerData::Freeze() {
  m_mean_time = 0;
  m_mean_rms = 0;

  for (int i = 0; i < m_N_active; ++i) m_mean_time += m_ch_time[i];

  m_mean_time = m_mean_time / ((float) m_N_active);

  for (int i = 0; i < m_N_active; ++i) m_mean_rms += sqr(m_ch_time[i] - m_mean_time);

  m_mean_rms = m_mean_rms / ((float) m_N_active);
  m_mean_rms = sqrt(m_mean_rms);
}

TileLaserTimingTool::DrawerData::DrawerData(const std::string &id)
 : Digi0TimeDiffHisto(("TimeDiff" + id).c_str(), ("TimeDiff" + id).c_str(), 2000, -100., 100.)
 , Digi0Time(("Digi0Time" + id).c_str(), ("Digi0Time" + id).c_str(), 2000, -100., 100.)
 , FarDigitizerMeanTime(0.0)
{

  for (int i = 0; i < 48; ++i) {
    std::ostringstream sout;
    sout << id << '_' << i;
    pmtd[i] = new PMTData(sout.str());
  }
}


// LBA | LBC
// EBA | EBC
float TileLaserTimingTool::s_fiberLength[2][48] = {{167.1,167.1,178.7,178.7,190.3,190.3,201.9,201.9,213.5,213.5,
						    225.1,225.1,236.7,236.7,248.3,248.3,259.9,259.9,271.5,271.5,
						    283.1,283.1,294.7,294.7,201.9,201.9,213.5,213.5,225.1,225.1,
						    236.7,236.7,248.3,248.3,259.9,259.9,271.5,271.5,283.1,283.1,
						    294.7,294.7,306.3,306.3,317.9,317.9,329.6,329.6},
						   
						   { 51.1, 51.1, 62.7, 62.7, 74.3, 74.3, 85.9, 85.9, 97.5, 97.5,
						    109.1,109.1,120.7,120.7,132.3,132.3,143.9,143.9,143.9,143.9,
						    167.1,167.1,178.1,178.1,178.1,178.1,178.1,178.1,213.5,213.5,
						    213.5,213.5,236.7,236.7,236.7,236.7,259.9,259.9,259.9,259.9,
						    283.1,283.1,294.7,294.7,294.7,294.7,294.7,294.7}};

TileLaserTimingTool::TileLaserTimingTool(const std::string& type, const std::string& name, const IInterface* pParent)
  : AthAlgTool(type, name, pParent)
  , m_tileHWID(0)
  , m_cabling(0)
  , m_tileToolTiming("TileCondToolTiming")
  , m_nevts(0)
  , m_gaussf(0)
{

  declareInterface<ITileCalibTool>( this );

  declareProperty("TileCondToolTiming", m_tileToolTiming);
  declareProperty("NtupleID", m_ntupleID = "h3000");
  declareProperty("FiberLightSpeed", m_fiberLightSpeed);
  declareProperty("NSamples", m_nSamples = 9);
  declareProperty("EneLowLimitPulseShape", m_eneLowLimitPulseShape = 0.000001);
  declareProperty("ReferenceDrawer", m_refDrawer = 39);

  declareProperty("UseMeanChannelOffset", m_useMeanChannelOffset = true);
  declareProperty("MaxTimeDiff", m_maxTimeDiff = 10000.);
  declareProperty("UsePMT0AsDskewRef", m_usePMT0AsDskewRef = true);

#ifdef TileLaserTimingMonPulse
  // create mon histos
  m_h1_time_check = new TH1F("h1_time_check", "Time(Tfit based) - Time(cispar based)", 1000, -200., 200.);
  m_h2_time_check = new TH2F("h2_time_check", "Time(Tfit based) vs Time(cispar based)", 1000, -200., 200., 1000, -200., 200.);

  m_PulseShapeHigh = new TProfile("pulse_shape_HG", "pulse shape HG", 400, -200., 300., 0, 100000.);
  m_PulseShapeLow = new TProfile("pulse_shape_LG", "pulse shape LG", 400, -200., 300., 0, 100000.);
#endif

  // creating multi-dim arrays on the heap and initialize all elements to zeros
  m_drawerData = new DrawerData*[4][64]();

  m_DrawerOffset = new float[NROS][64]();
  m_DrawerOffsetError = new float[NROS][64]();

  m_ChannelOffset = new float[NROS][64][48]();
  m_ChannelOffsetError = new float[NROS][64][48]();

  m_ADCAmplitude = new float[NROS][64][48]();

  m_PedestalMean = new float[NROS][64][48]();
  m_PedestalSigma = new float[NROS][64][48]();

  m_TimeDiffMean = new float[NROS][64][48]();
  m_TimeDiffMeanError = new float[NROS][64][48]();
  m_TimeDiffSigma = new float[NROS][64][48]();

  m_MeanOddPmtTdiffPMT0 = new float[NROS][64]();
  m_OddPmtCounterPMT0 = new int[NROS][64]();
  m_MeanEvenPmtTdiffPMT0 = new float[NROS][64]();
  m_EvenPmtCounterPMT0 = new int[NROS][64]();
  m_EvenOddTimeDiffPMT0 = new float[NROS][64]();

#ifdef TileLaserTimingPMT0Mon
  m_TimeDiffHighMean = new float[NROS][64][48]();
  m_TimeDiffHighMeanError = new float[NROS][64][48]();
  m_TimeDiffHighSigma = new float[NROS][64][48]();

  m_TimeDiffLowMean = new float[NROS][64][48]();
  m_TimeDiffLowMeanError = new float[NROS][64][48]();
  m_TimeDiffLowSigma = new float[NROS][64][48]();

  m_TimeDiffNoCFCorrMean = new float[NROS][64][48]();
  m_TimeDiffNoCFCorrMeanError = new float[NROS][64][48]();
  m_TimeDiffNoCFCorrSigma = new float[NROS][64][48]();
#endif

  m_FiberLength = new float[NROS][64][48]();

#ifdef TileLaserTimingMon
  m_TimeDiffPMTDigi0 = new float[NROS][64][48]();
  m_FiberCorrection = new float[NROS][64][48]();
  m_IsConnected = new int[NROS][64][48]();

  m_MeanOddPmtTdiff = new float[NROS][64]();
  m_OddPmtCounter = new int[NROS][64]();
  m_MeanEvenPmtTdiff = new float[NROS][64]();
  m_EvenPmtCounter = new int[NROS][64]();

  m_EvenOddTimeDiff = new float[NROS][64]();
#endif

  m_DSkewSet = new float[NROS][64][8]();
  m_DigiMean = new float[NROS][64][8]();
}

TileLaserTimingTool::~TileLaserTimingTool() {

  delete[] m_drawerData;
  delete[] m_DrawerOffset;
  delete[] m_DrawerOffsetError;
  delete[] m_ChannelOffset;
  delete[] m_ChannelOffsetError;
  delete[] m_ADCAmplitude;
  delete[] m_PedestalMean;
  delete[] m_PedestalSigma;
  delete[] m_TimeDiffMean;
  delete[] m_TimeDiffMeanError;
  delete[] m_TimeDiffSigma;
  delete[] m_MeanOddPmtTdiffPMT0;
  delete[] m_OddPmtCounterPMT0;
  delete[] m_MeanEvenPmtTdiffPMT0;
  delete[] m_EvenPmtCounterPMT0;
  delete[] m_EvenOddTimeDiffPMT0;
#ifdef TileLaserTimingPMT0Mon
  delete[] m_TimeDiffHighMean;
  delete[] m_TimeDiffHighMeanError;
  delete[] m_TimeDiffHighSigma;
  delete[] m_TimeDiffLowMean;
  delete[] m_TimeDiffLowMeanError;
  delete[] m_TimeDiffLowSigma;
  delete[] m_TimeDiffNoCFCorrMean;
  delete[] m_TimeDiffNoCFCorrMeanError;
  delete[] m_TimeDiffNoCFCorrSigma;
#endif

  delete[] m_FiberLength;

#ifdef TileLaserTimingMon
  delete[] m_TimeDiffPMTDigi0;
  delete[] m_FiberCorrection;
  delete[] m_IsConnected;
  delete[] m_MeanOddPmtTdiff;
  delete[] m_OddPmtCounter;
  delete[] m_MeanEvenPmtTdiff;
  delete[] m_EvenPmtCounter;
  delete[] m_EvenOddTimeDiff;
#endif
  delete[] m_DSkewSet;
  delete[] m_DigiMean;
}



StatusCode TileLaserTimingTool::initialize() {

  ATH_MSG_INFO( "initialize()" );

  // get TileHWID helper
  CHECK( detStore()->retrieve(m_tileHWID) );

  // get the timing tool
  CHECK( m_tileToolTiming.retrieve() );

  // get TileCabling Service
  m_cabling = TileCablingService::getInstance();

  m_nevts = 0;

  ATH_CHECK( m_eventInfoKey.initialize() );
  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_digitsContainerKey.initialize() );

  // gauss fit function
  m_gaussf = new TF1("GainGauss", "[0]*exp(- (x-[1])*(x-[1])/(2*[2]*[2]))", -60, 60);

  ATH_MSG_DEBUG( "initialize() ready " );

  /**
   * Use timing tool to get cell fiber lengths, when implemented!
  float athfl, offlfl;
  const int drawerIdx = TileCalibUtils::getDrawerIdx(1, 0);
  for (int pmt = 0; pmt < 48; pmt++) {
    athfl = m_tileToolTiming->getCellFiberLength(drawerIdx, pmt);
    offlfl = fiberLength(1, pmt);
    ATH_MSG_INFO( " " << pmt << " : " << athfl << " , " << offlfl );
  }
   */

  return StatusCode::SUCCESS;
}

StatusCode TileLaserTimingTool::initNtuple(int runNumber, int runType, TFile * rootFile) {

  ATH_MSG_INFO( "initNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );

  return StatusCode::SUCCESS;
}

// conv factor from low gain to high gain
//const float LG2HG = 64.;

/**
 * TODO: determine event type, select laser?
 */
StatusCode TileLaserTimingTool::execute() {

  ATH_MSG_DEBUG( "execute()" );

  gROOT->cd();

  // Get event's CIS parameters
  // QQQ: const uint32_t *cispar = m_beamInfo->cispar();
  // QQQ: const uint32_t dac = cispar[6];
  // QQQ: const uint32_t phase = cispar[5];
  // QQQ: const uint32_t cap = cispar[7];

  // Get EventInfo
  SG::ReadHandle<xAOD::EventInfo> eventInfo(m_eventInfoKey);
  ATH_CHECK( eventInfo.isValid() );


  // QQQ: const unsigned runNumber = eventInfo->runNumber();
  // QQQ: const unsigned eventNumber = eventInfo->eventNumber();

  // Check if event should be used in calibration
  bool pass = true;

  // Get TileRawChannelContainer
  SG::ReadHandle<TileRawChannelContainer> container(m_rawChannelContainerKey);
  ATH_CHECK( container.isValid() );

  // Get TileDigitsContainer
  SG::ReadHandle<TileDigitsContainer> digitsCnt(m_digitsContainerKey);
  ATH_CHECK( digitsCnt.isValid() );

  // Create iterator over RawChannelContainer
  TileRawChannelContainer::const_iterator itColl = (*container).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*container).end();
  TileRawChannelCollection::const_iterator it, itEnd;

  // tmp vars
  const TileDigitsCollection *digiColl = 0;
  const TileDigits* digits;
  const TileRawChannel *rawch = 0;
  float dtime, dtimeNoCFCorr;

#ifdef TileLaserTimingMonPulse	
  float f_SampleTime, f_phase1, f_phase2, f_time, f_tfittime;
#endif

  size_t colls;
  int Nconnected;
  float ChannelOffset;

  // accumulated vals
  float totene = 0.0;

  DrawerData *ddata = 0;

  unsigned frag;
  if (pass) {
    m_nevts++;

    // Go through all TileRawChannelCollections
    for (; itColl != itCollEnd; ++itColl) {
      // Get the digits collection for the drawer corresponding to the raw channel
      frag = (*itColl)->identify();
      TileDigitsContainer::const_iterator digiCollIt = digitsCnt->indexFind(
          digitsCnt->hashFunc()(frag));
      if (digiCollIt == digitsCnt->end()) {
        ATH_MSG_ERROR( " unable to get TileDigitsCollection for drawer:" << drawerIdStr(frag) );

      } else {
        digiColl = *digiCollIt;
        colls = (*itColl)->size();
        ATH_MSG_VERBOSE( "execute() colls:" << colls
                         << " digiColl->size:" << digiColl->size()
                         << " drawer:" << drawerIdStr((*itColl)->identify()) );

        if (digiColl->size() != colls) {
          ATH_MSG_ERROR( " size of TileDigitsCollection and TileRawChannelCollection differ for drawer:" << drawerIdStr((*itColl)->identify()) );

        } else {
          // ZZZ: Assume the first RawChannel in the collection corresponds to PMT 0
          // ZZZ: if this is not the case (for some reason) there will be a nasty segflt

          // get drawer HWID
          HWIdentifier dhwid = drawerId((*itColl)->identify());
          // LBA=1  LBC=2  EBA=3  EBC=4
          int ros = m_tileHWID->ros(dhwid);
          // 0 to 63
          int drawer = m_tileHWID->drawer(dhwid);
          // get and create DrawerData if needed, cannot be null
          ddata = drawerData(ros, drawer);
          // store ref to first PMT
          PMTData &pmt0data = *ddata->pmtd[0];

          for (size_t i = 0; i < colls; ++i) {
            digits = digiColl->at(i);
            rawch = (*itColl)->at(i);

            // require both digits and raw channels with equal ids
            if (!digits || !rawch || (digits->identify() != rawch->identify())) {
              if (!digits) {
                ATH_MSG_ERROR( " no TileDigits for drawer:" << drawerIdStr((*itColl)->identify())  );
              }

              if (!rawch) {
                ATH_MSG_ERROR( " no TileRawChannel for drawer:" << drawerIdStr((*itColl)->identify()) );
              }

              if (rawch && digits) {
                ATH_MSG_ERROR( " no id mismatch for drawer:" << drawerIdStr((*itColl)->identify()) );
              }

            } else {
              // get hardware id to identify adc
              HWIdentifier hwid = rawch->adc_HWID();
              // int ros    = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4
              // int drawer = m_tileHWID->drawer(hwid); // 0 to 63
              const int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
              const int gain = m_tileHWID->adc(hwid);      // low=0 high=1
              const int ipmt = channel2PMT(ros, chan);

              // store Fit data
              PMTData &pmtdata = *ddata->pmtd[ipmt];
              pmtdata.time = rawch->time();
              pmtdata.energy = rawch->amplitude();
              pmtdata.pedestal.add(rawch->pedestal());

              // in case of the first PMT (i.e. 0) this will be 0 as per definition
              dtimeNoCFCorr = pmtdata.time - pmt0data.time;
              dtime = dtimeNoCFCorr - fiberCorrection(ros, ipmt);
              pmtdata.TimeDiffHisto.Fill(dtime);

#ifdef TileLaserTimingPMT0	      
              pmtdata.TimeDiffNoCFCorrHisto.Fill(dtimeNoCFCorr);

              if(gain == 0) pmtdata.TimeDiffHistoLow.Fill(dtime);
              else pmtdata.TimeDiffHistoHigh.Fill(dtime);
#endif

#ifdef TileLaserTimingMon
              pmtdata.timeHisto.Fill(pmtdata.time);
              pmtdata.eneHisto.Fill(pmtdata.energy);
              pmtdata.gainHisto.Fill(gain);
#endif
              if (pmtdata.energy > 0.1) totene += pmtdata.energy;

              // pulse data-->
#ifdef TileLaserTimingMonPulse
              if(isConnectedChan(ros, chan)) {
                std::vector<float> sample = digits->samples();
                if(sample.size() != m_nSamples) {
                  ATH_MSG_ERROR( " sample size mismatch for drawer:" << drawerIdStr(frag)
                                 <<" in:" << sample.size() << " expected:" << m_nSamples );
                } else {
                  for (unsigned jsample = 0; jsample < m_nSamples; ++jsample) {
                    f_SampleTime = ((float) jsample) * 25.;
                    f_phase1 = (25. * ((float) phase)) / (30. * 8);
                    f_phase2 = -pmtdata.time;
                    f_time = f_SampleTime + f_phase1 - 100;
                    f_tfittime = f_SampleTime + f_phase2 - 100;
                    if (phase >= 160) {
                      f_time -= 25;
                      f_phase1 -= 25;
                    }

                    if (phase < 140 || phase > 160) {
                      // if Low gain convert to the HG scale
                      if (gain == 0) {
                        m_h1_time_check->Fill(f_phase1 - f_phase2);
                        m_h2_time_check->Fill(f_phase1, f_phase2);

                        if (fabs(pmtdata.energy) > m_eneLowLimitPulseShape) {
                          pmtdata.pulseShapeLow.Fill(f_tfittime, (LG2HG * (sample[jsample] - rawch->pedestal())));
                        }
                      }
                      //HG
                      else if (fabs(pmtdata.energy) > m_eneLowLimitPulseShape) {
                        pmtdata.pulseShapeHigh.Fill(f_tfittime, (sample[jsample] - rawch->pedestal()));
                      }
                    } // if Cispar not in 140->160 (range with time jitter,
                    //difficult to handle for smooth pulse shape
                  }
                } // end loop over jsample
              } // end if PMT is connected
#endif
              // <--pulse data

            } // else: ok digits and raw channels
          } // end collection loop

          //=====================================================================================
          //=== Compute mean Tfit (corrected for laser fiber length) over the first digitizer ===
          //=====================================================================================
          Nconnected = 0;
          ddata->FarDigitizerMeanTime = 0.;
          for (int ipmt = 0; ipmt < 6; ++ipmt) {
            if (isConnectedPMT(ros, ipmt)) {
              ddata->FarDigitizerMeanTime += (ddata->pmtd[ipmt]->time - fiberCorrection(ros, ipmt));
              Nconnected++;
            }
          }
          if (Nconnected) 
            ddata->FarDigitizerMeanTime /= (float) Nconnected;

#ifdef TileLaserTimingMon
          ddata->Digi0Time.Fill(ddata->FarDigitizerMeanTime);
#endif

          if (msgLvl(MSG::VERBOSE) && (ros == 1) && (drawer == 1)) {
            ATH_MSG_VERBOSE( " ddata->FarDigitizerMeanTime:" << ddata->FarDigitizerMeanTime );
          }

          //============================================================================
          //=== Calculate residuals. Fill histos with time diff b/w PMT i and digi 1 ===
          //============================================================================
          for (int ipmt = 0; ipmt < 48; ipmt++) {
            if (isConnectedPMT(ros, ipmt)) {
              ChannelOffset = -(ddata->pmtd[ipmt]->time - fiberCorrection(ros, ipmt) - ddata->FarDigitizerMeanTime);
              ddata->pmtd[ipmt]->TimeDiffResHis.Fill(ChannelOffset);
              if (msgLvl(MSG::VERBOSE) && (ros == 1) && (drawer == 1)) {
                ATH_MSG_VERBOSE(  " ipmt:" << ipmt << " ChannelOffset:" << ChannelOffset );
              }
            }
          }

        } // else: matching digits and raw channel collections
      }
    } // end container loop

    //=====================================================================================
    //=== this histogram contains the difference between first digitizer of drawer 1 and drawer 2: drawer offset
    //=====================================================================================
    for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
      if (drawerData(ros, m_refDrawer)) {
        DrawerData &drawer0data = *drawerData(ros, m_refDrawer);
        for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
          if ((drawer != m_refDrawer) && drawerData(ros, drawer)) {
            DrawerData &ddata = *drawerData(ros, drawer);
            ddata.Digi0TimeDiffHisto.Fill( ddata.FarDigitizerMeanTime - drawer0data.FarDigitizerMeanTime);

            if (msgLvl(MSG::VERBOSE) && (ros == 1) && (drawer == 1)) {
              ATH_MSG_VERBOSE( " drawerdiff:" << ddata.FarDigitizerMeanTime - drawer0data.FarDigitizerMeanTime );
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileLaserTimingTool::finalizeCalculations() {

  ATH_MSG_INFO( "finalizeCalculations()" );

  gROOT->cd();

  float fmean, fdummy, fchi2, fmeanerr;

  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      DrawerData &drawerd = *drawerData(ros, drawer);

      for (int ipmt = 0; ipmt < 48; ++ipmt) {
        PMTData &pmtd = *drawerd.pmtd[ipmt];
        // to simplify the root analysis, store the fiber length, not really neccessary
        m_FiberLength[ros][drawer][ipmt] = fiberLength(ros, ipmt);
#ifdef TileLaserTimingMon
        m_FiberCorrection[ros][drawer][ipmt] = fiberCorrection(ros, ipmt);
        m_IsConnected[ros][drawer][ipmt] = isConnectedPMT(ros, ipmt);
#endif
        //==========================================================================================
        //==== Pulse shapes ====

        // use the number of entries in the histos as the number of passed events for high/low

#ifdef TileLaserTimingMonPulse
        if(pmtd.TimeDiffHistoLow.GetEntries() > 0.99 * m_nevts) {
          m_PulseShapeLow->Add(&pmtd.pulseShapeLow);
        }

        if(pmtd.TimeDiffHistoHigh.GetEntries() > 0.99 * m_nevts) {
          m_PulseShapeHigh->Add(&pmtd.pulseShapeHigh);
        }
#endif

        if (ipmt > 0) {
          //==================================================================
          // Fit time difference regardless of gain, to derive standard Dskews
          //==================================================================
          if (isConnectedPMT(ros, ipmt)) {
            fitGauss(pmtd.TimeDiffHisto, m_TimeDiffMean[ros][drawer][ipmt], m_TimeDiffSigma[ros][drawer][ipmt], m_TimeDiffMeanError[ros][drawer][ipmt]);
          }

#ifdef TileLaserTimingPMT0
          //===========================================================
          //=== Fit time difference without clear fiber corrections ===
          //===========================================================
          if(isConnectedPMT(ros, ipmt)) {
            fitGauss(pmtd.TimeDiffNoCFCorrHisto, m_TimeDiffNoCFCorrMean[ros][drawer][ipmt], m_TimeDiffNoCFCorrSigma[ros][drawer][ipmt], m_TimeDiffNoCFCorrMeanError[ros][drawer][ipmt]);
          }

          //================================================================================
          //==== Fit gauss to time differences in all gains/ low gain/ high gain in all PMTs
          //================================================================================

          fitGauss(pmtd.TimeDiffHistoLow, m_TimeDiffLowMean[ros][drawer][ipmt], m_TimeDiffLowSigma[ros][drawer][ipmt], m_TimeDiffLowMeanError[ros][drawer][ipmt]);

          //------------------------------------- For high gain --------------------------------------

          fitGauss(pmtd.TimeDiffHistoHigh, m_TimeDiffHighMean[ros][drawer][ipmt], m_TimeDiffHighSigma[ros][drawer][ipmt], m_TimeDiffHighMeanError[ros][drawer][ipmt]);
#endif
        }

        //==================================================================================
        //=== Fit mean value of time difference b/w drawer_i(pmt_j) and drawer_i(digi_1) ===
        //==================================================================================

        // TODO: previous values on error
        if (isConnectedPMT(ros, ipmt)) {
          if (m_useMeanChannelOffset) {
            m_TimeDiffPMTDigi0[ros][drawer][ipmt] = m_ChannelOffset[ros][drawer][ipmt] =  pmtd.TimeDiffResHis.GetMean();
            m_ChannelOffsetError[ros][drawer][ipmt] = pmtd.TimeDiffResHis.GetMeanError();
          } else {
            fitGauss2(pmtd.TimeDiffResHis, fmean, fdummy, fchi2, fmeanerr, 1.0, 3.0);

            if (fchi2 > 500) {
              ATH_MSG_WARNING( "pmt # " << ipmt + 1
                               << " has chi2 value " << fchi2
                               << " force timing residual same as PMT-1 " );

              // FittedMean=PreviousFittedMean;
              fchi2 = -fchi2; // just to make Coverity happt
              fmean = -4711;
              fmeanerr = 0.0;
            }
            if (fabs(fmean) > 100) {
              ATH_MSG_WARNING( " pmt # " << ipmt + 1
                               << " has ChannelOffset " << fmean
                               << " force timing residual same as PMT-1" );

              // FittedMean=PreviousFittedMean;
              fmean = -4711;
              fmeanerr = 0.0;
            }
            m_ChannelOffset[ros][drawer][ipmt] = fmean;
            m_ChannelOffsetError[ros][drawer][ipmt] = fmeanerr;
          }
        } else {
          m_ChannelOffset[ros][drawer][ipmt] = 0.0;
          m_ChannelOffsetError[ros][drawer][ipmt] = 0.0;
        }

#ifdef TileLaserTimingMonPulse
        //===========================================================
        //=== Pedestal and ADC Amplitudes                         ===
        //===========================================================
        pmtd.pedestal.getStat(m_PedestalMean[ros][drawer][ipmt], m_PedestalSigma[ros][drawer][ipmt]);
        if (isConnectedPMT(ros, ipmt))
          m_ADCAmplitude[ros][drawer][ipmt] = fmax(pmtd.pulseShapeHigh.GetMaximum(), pmtd.pulseShapeLow.GetMaximum());
        else
          m_ADCAmplitude[ros][drawer][ipmt] = 0.;

        if (msgLvl(MSG::VERBOSE) && (ros == 1) && (drawer == 1)) {
          msg(MSG::VERBOSE) << "ipmt: " << ipmt
                            << " TimeDiffLow: " << m_TimeDiffLowMean[ros][drawer][ipmt] << "(" << m_TimeDiffLowSigma[ros][drawer][ipmt] << ") +-" << m_TimeDiffLowMeanError[ros][drawer][ipmt]
                            << " TimeDiffHigh:" << m_TimeDiffHighMean[ros][drawer][ipmt] << "(" << m_TimeDiffHighSigma[ros][drawer][ipmt] << ") +-" << m_TimeDiffHighMeanError[ros][drawer][ipmt] << endmsg;

          msg(MSG::VERBOSE) << "     TimeDiff:" << m_TimeDiffMean[ros][drawer][ipmt] << "(" << m_TimeDiffSigma[ros][drawer][ipmt] << ") +-" << m_TimeDiffMeanError[ros][drawer][ipmt]
                            << " TomeDiffNoCF:" << m_TimeDiffNoCFCorrMean[ros][drawer][ipmt] << "(" << m_TimeDiffNoCFCorrSigma[ros][drawer][ipmt] << ") +-" << m_TimeDiffNoCFCorrMeanError[ros][drawer][ipmt] << endmsg;

          msg(MSG::VERBOSE) << "     ped:" << m_PedestalMean[ros][drawer][ipmt] << "(" << m_PedestalSigma[ros][drawer][ipmt]
                            << ") amp:" << m_ADCAmplitude[ros][drawer][ipmt] << endmsg;
        }
#endif

      } // end PMT loop

      //============================================
      //========= Fit mean DrawerOffset ============
      //============================================

      fitGauss2(drawerd.Digi0TimeDiffHisto, m_DrawerOffset[ros][drawer], fdummy, fdummy, m_DrawerOffsetError[ros][drawer], 0.5, 1.0);

      ATH_MSG_DEBUG( "finalizeCalculations()"
                     << " ros: " << ros
                     << " drawer:" << drawer
                     << " DrawerOffset:" << m_DrawerOffset[ros][drawer] << "("
                     << m_DrawerOffsetError[ros][drawer] << ")" );

      //==============================================================
      //=== correct for overall offset between odd and even fibers ===
      //
      // Better would be to meassure the laser fiber lengths and 
      // implement this as a correction. Meanwhile, use this as 
      // a compromise.
      //==============================================================
      // CCC: MeanEvenPmtTdiff= 0;
      // CCC: MeanOddPmtTdiff = 0;
      // CCC: DeltaParity     = 0;
      // CCC: OddPmtCounter =0;
      // CCC: EvenPmtCounter =0;

      correctEvenOddPMT(ros, drawer, m_ChannelOffset, m_MeanOddPmtTdiff, m_OddPmtCounter, m_MeanEvenPmtTdiff, m_EvenPmtCounter, m_EvenOddTimeDiff);

      correctEvenOddPMT(ros, drawer, m_TimeDiffMean, m_MeanOddPmtTdiffPMT0, m_OddPmtCounterPMT0, m_MeanEvenPmtTdiffPMT0, m_EvenPmtCounterPMT0, m_EvenOddTimeDiffPMT0);

      TPMTArray& timeDiffs = m_usePMT0AsDskewRef ? m_TimeDiffMean : m_ChannelOffset;

      //==================================================================
      //=== Calculate Dskews                                           ===
      //==================================================================
      for (int ipmt = 0; ipmt < 48; ++ipmt) {
        // LBA and LBC
        if (isBarrel(ros)) {
          if (isConnectedPMT(ros, ipmt) && (timeDiffs[ros][drawer][ipmt] < 300)) {
            drawerd.digid[ipmt / 6].Add(timeDiffs[ros][drawer][ipmt]);
          }
          if (ipmt % 6 == 5) {
            drawerd.digid[ipmt / 6].Freeze();
          }
        } // end isBarrel
        // EBA and EBC%
        else if (isExtBarrel(ros)) {
          if (isConnectedPMT(ros, ipmt) && (timeDiffs[ros][drawer][ipmt] < 300)) {
            if (ipmt < 6)                    drawerd.digid[0].Add(timeDiffs[ros][drawer][ipmt]);
            else if (ipmt > 5 && ipmt < 12)  drawerd.digid[1].Add(timeDiffs[ros][drawer][ipmt]);
            else if (ipmt > 11 && ipmt < 18) drawerd.digid[2].Add(timeDiffs[ros][drawer][ipmt]);
            else if (ipmt > 17 && ipmt < 24) drawerd.digid[3].Add(timeDiffs[ros][drawer][ipmt]);
            else if (ipmt > 27 && ipmt < 34) {
              drawerd.digid[4].Add(timeDiffs[ros][drawer][ipmt]);
              drawerd.digid[5].Add(timeDiffs[ros][drawer][ipmt]);
            } else if (ipmt > 35) {
              drawerd.digid[6].Add(timeDiffs[ros][drawer][ipmt]);
              drawerd.digid[7].Add(timeDiffs[ros][drawer][ipmt]);
            }
          }

          if (ipmt == 5)       drawerd.digid[0].Freeze();
          else if (ipmt == 11) drawerd.digid[1].Freeze();
          else if (ipmt == 17) drawerd.digid[2].Freeze();
          else if (ipmt == 23) drawerd.digid[3].Freeze();
          else if (ipmt == 33) {
            drawerd.digid[4].Freeze();
            drawerd.digid[5].Freeze();
          } else if (ipmt == 47) {
            drawerd.digid[6].Freeze();
            drawerd.digid[7].Freeze();
          }
        } // end isExtBarrel
      } // end PMT loop

      for (int digi = 0; digi < 8; ++digi) {
        m_DigiMean[ros][drawer][digi] = drawerd.digid[digi].GetDigiTime();
        m_DSkewSet[ros][drawer][digi] = (m_DigiMean[ros][drawer][digi] - m_DigiMean[ros][drawer][0]) * 240. / 25.;

        ATH_MSG_DEBUG( "finalizeCalculations()"
                       << " ros: " << ros
                       << " drawer: " << drawer
                       << " digi: " << digi
                       << " digi_time: " << drawerd.digid[digi].GetDigiTime()
                       << " #active: " << drawerd.digid[digi].GetTotActive()
                       << " DSkewSet: " << m_DSkewSet[ros][drawer][digi] );

      }
    } // end drawer loop
  } // end ros loop

  return StatusCode::SUCCESS;
}

StatusCode TileLaserTimingTool::writeNtuple(int runNumber, int runType, TFile* rootFile) {

  ATH_MSG_INFO( "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );

  // Write a tree with all arrays
  TTree *t = new TTree(m_ntupleID.c_str(), "TileLaserTimingNtuple");
  t->Branch("DrawerOffset", m_DrawerOffset, "DrawerOffset[5][64]/F"); // float m_DrawerOffset[s_NRos][64];
  t->Branch("DrawerOffsetError", m_DrawerOffsetError, "DrawerOffsetError[5][64]/F"); // float m_DrawerOffsetError[NROS][64];

  t->Branch("ChannelOffset", m_ChannelOffset, "ChannelOffset[5][64][48]/F"); // float m_ChannelOffset[NROS][64][48];
  t->Branch("ChannelOffsetError", m_ChannelOffsetError, "ChannelOffsetError[5][64][48]/F"); // float m_ChannelOffsetError[NROS][64][48];

  t->Branch("ADCAmplitude", m_ADCAmplitude, "ADCAmplitude[5][64][48]/F"); // float m_ADCAmplitude[NROS][64][48];

  t->Branch("PedestalMean", m_PedestalMean, "PedestalMean[5][64][48]/F"); // float m_PedestalMean[NROS][64][48];
  t->Branch("PedestalSigma", m_PedestalSigma, "PedestalSigma[5][64][48]/F"); // float m_PedestalSigma[NROS][64][48];

  t->Branch("TimeDiffMean", m_TimeDiffMean, "TimeDiffMean[5][64][48]/F"); // float m_TimeDiffMean[NROS][64][48];
  t->Branch("TimeDiffMeanError", m_TimeDiffMeanError, "TimeDiffMeanError[5][64][48]/F"); // float m_TimeDiffMeanError[NROS][64][48];
  t->Branch("TimeDiffSigma", m_TimeDiffSigma, "TimeDiffSigma[5][64][48]/F"); // float m_TimeDiffSigma[NROS][64][48];

#ifdef TileLaserTimingPMT0Mon
  t->Branch("TimeDiffHighMean", m_TimeDiffHighMean, "TimeDiffHighMean[5][64][48]/F"); // float m_TimeDiffHighMean[NROS][64][48];
  t->Branch("TimeDiffHighMeanError", m_TimeDiffHighMeanError, "TimeDiffHighMeanError[5][64][48]/F"); // float m_TimeDiffHighMeanError[NROS][64][48];
  t->Branch("TimeDiffHighSigma", m_TimeDiffHighSigma, "TimeDiffHighSigma[5][64][48]/F"); // float m_TimeDiffHighSigma[NROS][64][48];
  t->Branch("TimeDiffLowMean", m_TimeDiffLowMean, "TimeDiffLowMean[5][64][48]/F"); // float m_TimeDiffLowMean[NROS][64][48];
  t->Branch("TimeDiffLowMeanError", m_TimeDiffLowMeanError, "TimeDiffLowMeanError[5][64][48]/F"); // float m_TimeDiffLowMeanError[NROS][64][48];
  t->Branch("TimeDiffLowSigma", m_TimeDiffLowSigma, "TimeDiffLowSigma[5][64][48]/F"); // float m_TimeDiffLowSigma[NROS][64][48];
  t->Branch("TimeDiffNoCFCorrMean", m_TimeDiffNoCFCorrMean, "TimeDiffNoCFCorrMean[5][64][48]/F"); // float m_TimeDiffNoCFCorrMean[NROS][64][48];
  t->Branch("TimeDiffNoCFCorrMeanError", m_TimeDiffNoCFCorrMeanError, "TimeDiffNoCFCorrMeanError[5][64][48]/F"); // float m_TimeDiffNoCFCorrMeanError[NROS][64][48];
  t->Branch("TimeDiffNoCFCorrSigma", m_TimeDiffNoCFCorrSigma, "TimeDiffNoCFCorrSigma[5][64][48]/F"); // float m_TimeDiffNoCFCorrSigma[NROS][64][48];
#endif
  t->Branch("FiberLength", m_FiberLength, "FiberLength[5][64][48]/F"); // float m_FiberLength[NROS][64][48];

#ifdef TileLaserTimingMon
  t->Branch("EvenOddTimeDiff", m_EvenOddTimeDiff, "EvenOddTimeDiff[5][64]/F"); // float m_EvenOddTimeDiff[NROS][64];

  t->Branch("FiberCorrection", m_FiberCorrection, "FiberCorrection[5][64][48]/F"); // float m_FiberCorrection[NROS][64][48];
  t->Branch("IsConnected", m_IsConnected, "IsConnected[5][64][48]/F"); // float m_FiberCorrection[NROS][64][48];

  t->Branch("MeanOddPmtTdiff", m_MeanOddPmtTdiff, "MeanOddPmtTdiff[5][64]/F");
  t->Branch("OddPmtCounter", m_OddPmtCounter, "OddPmtCounter[5][64]/I");
  t->Branch("MeanEvenPmtTdiff", m_MeanEvenPmtTdiff, "MeanEvenPmtTdiff[5][64]/F");
  t->Branch("EvenPmtCounter", m_EvenPmtCounter, "EvenPmtCounter[5][64]/I");

  t->Branch("TimeDiffPMTDigi0", m_TimeDiffPMTDigi0, "TimeDiffPMTDigi0[5][64][48]/F");
#endif

  // mon pmt0-->
  t->Branch("EvenOddTimeDiffPMT0", m_EvenOddTimeDiffPMT0, "EvenOddTimeDiffPMT0[5][64]/F");
  t->Branch("MeanOddPmtTdiffPMT0", m_MeanOddPmtTdiffPMT0, "MeanOddPmtTdiffPMT0[5][64]/F");
  t->Branch("OddPmtCounterPMT0", m_OddPmtCounterPMT0, "OddPmtCounterPMT0[5][64]/I");
  t->Branch("MeanEvenPmtTdiffPMT0", m_MeanEvenPmtTdiffPMT0, "MeanEvenPmtTdiffPMT0[5][64]/F");
  t->Branch("EvenPmtCounterPMT0", m_EvenPmtCounterPMT0, "EvenPmtCounterPMT0[5][64]/I");
  // <-- mon pmt0

  t->Branch("DSkewSet", m_DSkewSet, "DSkewSet[5][64][8]/F");       // float m_DSkewSet[NROS][64][8];
  t->Branch("DigiMean", m_DigiMean, "DigiMean[5][64][8]/F");       // float m_DigiMean[NROS][64][8];
  t->Fill();
  rootFile->cd();
  t->Write();

  ATH_MSG_INFO( "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ") data written" );

  rootFile->ls();

  // store histos 
  for (unsigned int ros = 1; ros < TileCalibUtils::MAX_ROS; ++ros) {
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER; ++drawer) {
      ATH_MSG_DEBUG( "writeNtuple() ros:" << ros << " drawer:" << drawer );

      DrawerData* dd = drawerData(ros, drawer);
      if (dd) {
        dd->Digi0TimeDiffHisto.Write();
#ifdef TileLaserTimingMon
        dd->Digi0Time.Write();
#endif
        for (int ipmt = 0; ipmt < 48; ++ipmt) {
          if (isConnectedPMT(ros, ipmt)) {
            /*dd->pmtd[ipmt]->TimeDiffHisto.Write();
             dd->pmtd[ipmt]->TimeDiffNoCFCorrHisto.Write();*/
#ifdef TileLaserTimingMon
            dd->pmtd[ipmt]->timeHisto.Write();
            dd->pmtd[ipmt]->eneHisto.Write();
            dd->pmtd[ipmt]->gainHisto.Write();
#endif
            /*dd->pmtd[ipmt]->TimeDiffHistoHigh.Write();
             dd->pmtd[ipmt]->pulseShapeHigh.Write();
             dd->pmtd[ipmt]->TimeDiffHistoLow.Write();
             dd->pmtd[ipmt]->pulseShapeLow.Write();*/
            dd->pmtd[ipmt]->TimeDiffResHis.Write();
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TileLaserTimingTool::finalize() {

  ATH_MSG_INFO( "finalize()" );

  return StatusCode::SUCCESS;
} 



bool TileLaserTimingTool::isConnectedPMT(int ros, int ipmt) {
  if (isBarrel(ros)) {
    return !(ipmt == 31 || ipmt == 32 || ipmt == 43);
  } else if (isExtBarrel(ros)) {
    return !(   ipmt == 18 || ipmt == 19 || ipmt == 24 || ipmt == 25
             || ipmt == 26 || ipmt == 27 || ipmt == 30 || ipmt == 31
             || ipmt == 34 || ipmt == 35 || ipmt == 38 || ipmt == 39
             || ipmt == 44 || ipmt == 45 || ipmt == 46 || ipmt == 47);
  }

  return false;
}


void TileLaserTimingTool::fitGauss(TH1F &hi, float &p1, float &p2, float &p1_err) {

  if (hi.GetEntries() == 0) {
    p1 = p2 = p1_err = 0.0;
    return;
  }
  double MaxAmplitude = hi.GetMaximum();
  double MeanTimeDiff = hi.GetMean();
  double RMSTimeDiff = hi.GetRMS();
  m_gaussf->SetParameter(0, MaxAmplitude);
  m_gaussf->SetParameter(1, MeanTimeDiff);
  m_gaussf->SetParameter(2, RMSTimeDiff);
  m_gaussf->SetRange(MeanTimeDiff - 2.0 * RMSTimeDiff, MeanTimeDiff + 2.0 * RMSTimeDiff);

  hi.Fit(m_gaussf, "QR");

  p1 = m_gaussf->GetParameter(1);
  p1_err = m_gaussf->GetParError(1);
  p2 = m_gaussf->GetParameter(2);
}

void TileLaserTimingTool::fitGauss2(TH1F &hi, float &p1, float &p2, float &chi2, float &p1_err, float w1, float w2) {

  if (hi.GetEntries() == 0) {
    p1 = p2 = chi2 = p1_err = w1 = w2 = 0.0;
    return;
  }

  double MaxAmplitude = hi.GetMaximum();
  double MeanTimeDiff = hi.GetMean();
  double RMSTimeDiff = hi.GetRMS();

  m_gaussf->SetParameter(0, MaxAmplitude);
  m_gaussf->SetParameter(1, MeanTimeDiff);
  m_gaussf->SetParameter(2, RMSTimeDiff);
  m_gaussf->SetRange(MeanTimeDiff - w1 * RMSTimeDiff, MeanTimeDiff + w1 * RMSTimeDiff);

  hi.Fit(m_gaussf, "QR");
  double FittedAmplitude = m_gaussf->GetParameter(0);
  double FittedMean = m_gaussf->GetParameter(1);
  double FittedSigma = m_gaussf->GetParameter(2);

  //=== Iterate once ===

  m_gaussf->SetParameter(0, FittedAmplitude);
  m_gaussf->SetParameter(1, FittedMean);
  m_gaussf->SetParameter(2, FittedSigma);
  m_gaussf->SetRange(FittedMean - w2 * FittedSigma, FittedMean + w2 * FittedSigma);

  hi.Fit(m_gaussf, "QR");
  p1 = m_gaussf->GetParameter(1);
  p2 = m_gaussf->GetParameter(2);
  p1_err = m_gaussf->GetParError(1);
  chi2 = m_gaussf->GetChisquare();
}

void TileLaserTimingTool::correctEvenOddPMT(int ros, int drawer, TPMTArray& ChannelOffset
                                            , TDrawerArrayF& MeanOddPmtTdiff, TDrawerArrayI& OddPmtCounter
                                            , TDrawerArrayF& MeanEvenPmtTdiff, TDrawerArrayI& EvenPmtCounter
                                            , TDrawerArrayF& EvenOddTimeDiff) {


  float DeltaParity = 0;

  //=== determine overall shift between odd and even fibers ===
  //=== for this take only into account channels with connected PMT
  //=== don't take into the mean the value of tdiff when it is too far from zero
  for (int ipmt = 0; ipmt < 48; ++ipmt) {
    if (isConnectedPMT(ros, ipmt)) {
      if (ipmt % 2 == 1) {
        // odd PMTs
        if (fabs(ChannelOffset[ros][drawer][ipmt]) < m_maxTimeDiff) {
          MeanOddPmtTdiff[ros][drawer] += ChannelOffset[ros][drawer][ipmt];
          ++OddPmtCounter[ros][drawer];
        } else {
          ATH_MSG_WARNING( "correctEvenOddPMT()"
                           << " time overflow (" << ChannelOffset[ros][drawer][ipmt]
                           << ") ros:" << ros
                           << " dr:" << drawer
                           << " pmt:" << ipmt );

        }
      } else if (ipmt % 2 == 0) {
        // even PMTs
        if (fabs(ChannelOffset[ros][drawer][ipmt]) < m_maxTimeDiff) {
          MeanEvenPmtTdiff[ros][drawer] += ChannelOffset[ros][drawer][ipmt];
          ++EvenPmtCounter[ros][drawer];
        } else {
          ATH_MSG_WARNING( "correctEvenOddPMT()"
                           << " time overflow (" << ChannelOffset[ros][drawer][ipmt]
                           << ") ros:" << ros
                           << " dr:" << drawer
                           << " pmt:" << ipmt );
        }
      }
    }
  }

  // Save Time difference b/w odd/even pmt's
  MeanOddPmtTdiff[ros][drawer] /= (float) OddPmtCounter[ros][drawer];
  MeanEvenPmtTdiff[ros][drawer] /= (float) EvenPmtCounter[ros][drawer];
  DeltaParity = 0.5 * (MeanOddPmtTdiff[ros][drawer] - MeanEvenPmtTdiff[ros][drawer]);

  EvenOddTimeDiff[ros][drawer] = 2 * DeltaParity;

  ATH_MSG_DEBUG( "correctEvenOddPMT()"
                << "  ros: " << ros
                << " drawer:" << drawer
                << " EvenOddTimeDiff:" << EvenOddTimeDiff[ros][drawer]
                << " | MeanOddPmtTdiff:" << MeanOddPmtTdiff[ros][drawer]
                << " MeanEvenPmtTdiff:" << MeanEvenPmtTdiff[ros][drawer]
                << " DeltaParity:" << DeltaParity );

  //==================================================================
  //=== Correct for overall shift b/w odd and even fibers          ===
  //==================================================================
  for (int ipmt = 0; ipmt < 48; ++ipmt) {
    if (isConnectedPMT(ros, ipmt)) {
      if (ipmt % 2 == 1)
        ChannelOffset[ros][drawer][ipmt] -= DeltaParity;
      else if (ipmt % 2 == 0) ChannelOffset[ros][drawer][ipmt] += DeltaParity;
    }
  } // end pmt loop
}
