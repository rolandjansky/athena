/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi includes
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ThreadLocalContext.h"

#include "Identifier/HWIdentifier.h"

// Athena includes
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadHandle.h"

// Tile includes
#include "TileCisDefaultCalibTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingSvc.h"
#include "TileConditions/TileInfo.h"


#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TMap.h"
#include "TMath.h"
#include <cmath>

TileCisDefaultCalibTool::TileCisDefaultCalibTool(const std::string& type, const std::string& name,
    const IInterface* pParent)
    : AthAlgTool(type, name, pParent)
  , m_tileHWID(nullptr)
  , m_cabling(nullptr)
  , m_cablingSvc("TileCablingSvc", name)
  , m_scanMap(nullptr)
  , m_scanMapRMS(nullptr)
  , m_tileInfo(nullptr)
{
  declareInterface<ITileCalibTool>(this);

  declareProperty("NtupleID", m_ntupleID = "h3000");

  declareProperty("removePed", m_removePed = true);
  declareProperty("useSmallCap", m_useSmallCap = false);
  declareProperty("phaseMin", m_phaseMin = -10);
  declareProperty("phaseMax", m_phaseMax = 300);

  declareProperty("maxPed", m_maxPed = 45.);

  declareProperty("chargeMinHi", m_chargeMinHi = 0.5);
  declareProperty("chargeMaxHi", m_chargeMaxHi = 12.5);
  declareProperty("chargeMinLo", m_chargeMinLo = 31.5);
  declareProperty("chargeMaxLo", m_chargeMaxLo = 800.0);

  declareProperty("linfitMinHi", m_linfitMinHi = 3.0);
  declareProperty("linfitMaxHi", m_linfitMaxHi = 10.0);
  declareProperty("linfitMinLo", m_linfitMinLo = 300.0);
  declareProperty("linfitMaxLo", m_linfitMaxLo = 700.0);

  declareProperty("linfitMinHiDemo", m_linfitMinHiDemo = 6.0);
  declareProperty("linfitMaxHiDemo", m_linfitMaxHiDemo = 20.0);
  declareProperty("linfitMinLoDemo", m_linfitMinLoDemo = 300.0);
  declareProperty("linfitMaxLoDemo", m_linfitMaxLoDemo = 700.0);

  declareProperty("doSampleChecking", m_doSampleChecking = true); // do sample checking by default
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");
  declareProperty("TileInfoName", m_infoName = "TileInfo");

  declareProperty("FragIDsDemonstrators", m_fragIDsDemonstrators, "List of Tile frag IDs of demonstrators, which have different CIS circuits than the legacy ones");

  // Initialize arrays for results
  m_calib = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_qflag = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_nDAC = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_nDigitalErrors = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_chi2 = new float[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();

  // Initialize sample check arrays
  m_edgeSample = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
  m_nextToEdgeSample = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();

  m_sampleBit = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NBITS]();
  m_bitStatus = new unsigned short[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN][NBSTATUS]();
  m_numSamp = new int[Tile::MAX_ROS][Tile::MAX_DRAWER][Tile::MAX_CHAN][Tile::MAX_GAIN]();
}

TileCisDefaultCalibTool::~TileCisDefaultCalibTool() {

  delete[] m_calib;
  delete[] m_qflag;
  delete[] m_nDAC;
  delete[] m_nDigitalErrors;
  delete[] m_chi2;
  delete[] m_edgeSample;
  delete[] m_nextToEdgeSample;
  delete[] m_sampleBit;
  delete[] m_bitStatus;
  delete[] m_numSamp;

}

StatusCode TileCisDefaultCalibTool::initialize() {
  ATH_MSG_INFO( "initialize()" );

  // get TileHWID helper
  CHECK( detStore()->retrieve(m_tileHWID) );

  // get TileCabling Service
  CHECK( m_cablingSvc.retrieve() );
  m_cabling = m_cablingSvc->cablingService();
  int runPeriod = m_cabling->runPeriod();

  if (runPeriod==3) {
    std::vector<int> v = { 0x10d }; // LBA14 is demonstrator in RUN3

    if ( m_fragIDsDemonstrators.size() == 0) {
      m_fragIDsDemonstrators = v;
    }
  }

  if ( m_fragIDsDemonstrators.size() != 0) {

    std::sort(m_fragIDsDemonstrators.begin(),m_fragIDsDemonstrators.end());

    std::ostringstream os;
    for (int fragID : m_fragIDsDemonstrators) {
      os << " 0x" << std::hex << fragID << std::dec;
    }

    ATH_MSG_INFO("Special settings in histograms for demonstrator modules (frag IDs):" << os.str());
  }

  CHECK( m_dqStatusKey.initialize() );

  // get TileInfo
  CHECK( detStore()->retrieve(m_tileInfo, m_infoName) );

  // set important constants
  m_dac2Charge[0] = 100.* 2.0 * 4.096 / double(m_tileInfo->ADCmax()); // 100 pF * 2 for legacy
  m_dac2Charge[1] = 5.2 * 2.0 * 4.096 / double(m_tileInfo->ADCmax()); // effective 5.2 pF * 2 for 5 pF capacitor
  m_dac2Charge[2] = 200.* 4.096 / double(m_tileInfo->ADCmax()); // 200 pF for demonstrator - similar to legacy
  m_dac2Charge[3] = 5.2 * 4.096 / double(m_tileInfo->ADCmax()); // effective value of small capacitor is twice smaller for demonstrator

  // the same overflow limit for the moment, can be changed later
  m_maxAmp[0] = m_tileInfo->ADCmax() - m_maxPed;
  m_maxAmp[1] = m_tileInfo->ADCmax() - m_maxPed;
  m_maxAmp[2] = m_tileInfo->ADCmax() - m_maxPed;
  m_maxAmp[3] = m_tileInfo->ADCmax() - m_maxPed;

  m_defaultCalib[0] = 1.29;
  m_defaultCalib[1] = 81.8;
  m_defaultCalib[2] = m_defaultCalib[0];
  m_defaultCalib[3] = m_defaultCalib[1] / 2;

  if (!m_useSmallCap) {
    m_chargeMin[0] = m_chargeMinLo;
    m_chargeMin[1] = m_chargeMinHi;
    m_chargeMin[2] = m_chargeMinLo;
    m_chargeMin[3] = m_chargeMinHi * 2;
  }

  m_chargeMax[0] = m_chargeMaxLo;
  m_chargeMax[1] = m_chargeMaxHi;
  m_chargeMax[2] = m_chargeMaxLo;
  m_chargeMax[3] = m_chargeMaxHi * 2; // high gain in demo is up to 25 pC istead of 12.5 pC

  m_linfitMin[0] = m_linfitMinLo;
  m_linfitMin[1] = m_linfitMinHi;
  m_linfitMin[2] = m_linfitMinLoDemo;
  m_linfitMin[3] = m_linfitMinHiDemo;

  m_linfitMax[0] = m_linfitMaxLo;
  m_linfitMax[1] = m_linfitMaxHi;
  m_linfitMax[2] = m_linfitMaxLoDemo;
  m_linfitMax[3] = m_linfitMaxHiDemo;

  ATH_CHECK( m_rawChannelContainerKey.initialize() );
  ATH_CHECK( m_digitsContainerKey.initialize(m_doSampleChecking) );

  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::initNtuple(int runNumber, int runType, TFile * rootFile) {
  ATH_MSG_INFO( "initialize(" << runNumber << "," << runType << "," << rootFile << ")" );

  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::execute() {

  ATH_MSG_DEBUG( "execute()" );

  // Get the DQ digital check information
  const EventContext& ctx = Gaudi::Hive::currentContext();
  const TileDQstatus* theDQstatus = SG::makeHandle (m_dqStatusKey, ctx).get();

  // Get event's CIS parameters
  const uint32_t *cispar = theDQstatus->cispar();
  uint32_t dac = cispar[6];
  uint32_t phase = cispar[5];
  uint32_t cap = cispar[7];
  int cap_ind = (cap > 10) ? 0 : 1; // 100 pF or 5 pF
  double chargeAll[4];
  for (int i=0; i<4; ++i)
    chargeAll[i] = dac * m_dac2Charge[i];

  // Check if event should be used in calibration
  bool pass = true;
  if (cap == 100 && m_useSmallCap)
    pass = false;
  else if (cap == 5 && !m_useSmallCap) pass = false;
  if (phase > m_phaseMax) pass = false;
  if (phase < m_phaseMin) pass = false;
  if (cispar[6] == 120) {  // Reject garbage events at the beginning of files. This DAQ
    pass = false;         // setting isn't used during a normal CIS scan FYI.
  }

  // Get TileRawChannelContainer
  SG::ReadHandle<TileRawChannelContainer> container(m_rawChannelContainerKey);
  ATH_CHECK( container.isValid() );

  // Create iterator over RawChannelContainer
  TileRawChannelContainer::const_iterator itColl = (*container).begin();
  TileRawChannelContainer::const_iterator itCollEnd = (*container).end();
  TileRawChannelCollection::const_iterator it, itEnd;

  if (pass) {

    // Go through all TileRawChannelCollections
    for (; itColl != itCollEnd; ++itColl) {

      int fragId = (*itColl)->identify();
      bool demonstrator = (std::binary_search(m_fragIDsDemonstrators.begin(), m_fragIDsDemonstrators.end(), fragId));
      int gain_offset = (demonstrator) ? 2 : 0;
      double charge = chargeAll[cap_ind+gain_offset];

      // go through all TileRawChannels in collection
      it = (*itColl)->begin();
      itEnd = (*itColl)->end();

      for (; it != itEnd; ++it) {

        // get hardware id to identify adc channel
        HWIdentifier hwid = (*it)->adc_HWID();
        int ros = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4
        int drawer = m_tileHWID->drawer(hwid); // 0 to 63
        int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
        int gain = m_tileHWID->adc(hwid);      // low=0 high=1

        // check if channel is connected
        // if( !chanIsConnected(ros,chan) ) continue;

        // Is channel empty? DQ version
        if (theDQstatus->isChEmpty(ros, drawer, chan)) continue;

        // find dac maps for adc channel
        TDACIntMap *NEvtDacMap = (m_NEvtMap)[hwid];
        TDACIntMap *NDigitalErrorsDacMap = (m_NDigitalErrorsMap)[hwid];
        TDACDoubleMap *MeanDacMap = (m_MeanMap)[hwid];
        TDACDoubleMap *MeanSqDacMap = (m_MeanSqMap)[hwid];

        // create new dac maps if they don't exist
        if (NEvtDacMap == nullptr) {
          NEvtDacMap = (m_NEvtMap)[hwid] = new TDACIntMap;
          NDigitalErrorsDacMap = (m_NDigitalErrorsMap)[hwid] = new TDACIntMap;
          MeanDacMap = (m_MeanMap)[hwid] = new TDACDoubleMap;
          MeanSqDacMap = (m_MeanSqMap)[hwid] = new TDACDoubleMap;
        }

        // check that charge is less than chargeMax (depends on gain)
        if (charge > m_chargeMin[gain+gain_offset] && charge < m_chargeMax[gain+gain_offset]) {

          double amp = (*it)->amplitude();

          // Hack to get rid of pedestal events: need more sophisticated method!          
          if ( (amp > 40.) || (gain == 0 && amp > 6.) || (!m_removePed) ) {

            // Digital error check
            if (!(theDQstatus->isAdcDQgood(ros, drawer, chan, gain))) {
              ATH_MSG_DEBUG(  "Skipping Module: " << ros << drawer + 1
                            << " channel: " << chan
                            << " ADC: " << gain
                            << " due to DQ error found." );

              (*NDigitalErrorsDacMap)[dac] += 1;
              continue;

            } else {

              // increment entries for current dac value
              (*NEvtDacMap)[dac] += 1;
              (*MeanDacMap)[dac] += amp;
              (*MeanSqDacMap)[dac] += amp*amp;
            }

          }

        } // end if (min < charge < max)
      }
    }
  } // end if pass

  if (m_doSampleChecking) {
    // Get TileDigitsContainer
    SG::ReadHandle<TileDigitsContainer> digContainer(m_digitsContainerKey);
    ATH_CHECK( digContainer.isValid() );

    // Create iterator over RawDigitsContainer
    TileDigitsContainer::const_iterator digItColl = digContainer->begin();
    TileDigitsContainer::const_iterator digItCollEnd = digContainer->end();

     for (; digItColl != digItCollEnd; ++digItColl) {

      TileDigitsCollection::const_iterator digIt = (*digItColl)->begin();
      TileDigitsCollection::const_iterator digItEnd = (*digItColl)->end();

      if (digIt != digItEnd) {

        int fragId = (*digItColl)->identify();
        bool demonstrator = (std::binary_search(m_fragIDsDemonstrators.begin(), m_fragIDsDemonstrators.end(), fragId));
        int gain_offset = (demonstrator) ? 2 : 0;
        double charge = chargeAll[cap_ind+gain_offset];

        HWIdentifier adc_id = (*digIt)->adc_HWID();
        int ros = m_tileHWID->ros(adc_id);    // LBA=1  LBC=2  EBA=3  EBC=4
        int drawer = m_tileHWID->drawer(adc_id); // 0 to 63

        // not clear how to handle this. if not 7 get off? MM - 4 June 2009
        int numSamples = (*digIt)->NtimeSamples();
        if (numSamples != 7) {
          m_doSampleChecking = false;
          break;
        }

        for (; digIt != digItEnd; ++digIt) {

            adc_id = (*digIt)->adc_HWID();
            int chan = m_tileHWID->channel(adc_id);  // 0 to 47 channel not PMT
            int gain = m_tileHWID->adc(adc_id);      // low=0 high=1

            std::vector<float> theDigits = (*digIt)->samples();

            //MM - skip channels with digital errors
            if (!(theDQstatus->isAdcDQgood(ros, drawer, chan, gain))) {
              continue;
            }

            // Loop over samples for bit analysis
            // We don't need to use the same cuts as the "Edge Sample" analysis           
            for(unsigned int sampNum = 0; sampNum < theDigits.size(); sampNum++) {

              // Count the total number of samples taken by an ADC
              m_numSamp[ros][drawer][chan][gain] += 1;
              int k = 0;
              int quotient = theDigits[sampNum];

              // convert sample to binary number
              while(quotient!=0) {
                if((quotient % 2) == 1) {
                  // If the bit is one, store info in the array
                  m_sampleBit[ros][drawer][chan][gain][k] += 1;
                }

                quotient = quotient / 2;
                k += 1;
              } // end binary conversion
            } //end sample loop

          if (pass && charge > m_linfitMin[gain+gain_offset] && charge < m_linfitMax[gain+gain_offset]) {

            std::vector<float> theDigits = (*digIt)->samples();
            float maxSampVal = -1.;
            int maxSampNum = -1;

            for (unsigned int sampNum = 0; sampNum < theDigits.size(); sampNum++) {
              if (theDigits[sampNum] > maxSampVal) {
                maxSampVal = theDigits[sampNum];
                maxSampNum = sampNum + 1;
              }
            }

            if (maxSampNum == 1 || maxSampNum == 7) {
              m_edgeSample[ros][drawer][chan][gain] = 1;
            } else if (maxSampNum == 2 || maxSampNum == 6) {
              m_nextToEdgeSample[ros][drawer][chan][gain] = 1;
            }

          } // end digits iterator
        }
      } // end digits collections

    } // end if pass

  } // end m_doSampleChecking

  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::finalizeCalculations() {

  ATH_MSG_INFO( "finalizeCalculations()" );

  // The values of 81.454 and 1.295 were derived from runs:
  // 72652 73305 72653 72661 79259 78023 79781 78026
  // to calibrate the detector, and looking at the mean good calibration value
  double meanCalib[4];
  meanCalib[0] = 1.295;
  meanCalib[1] = 81.454;
  // values for demonstrator were taken from run 416970
  meanCalib[2] = 1.26282;
  meanCalib[3] = 40.9303;

  // hardware id (key to maps)
  HWIdentifier hwid;

  // dac maps
  TDACDoubleMap* MeanDacMap;
  TDACDoubleMap* MeanSqDacMap;
  TDACIntMap* NEvtDacMap;
  TDACIntMap* NDigitalErrorsDacMap;

  // count number of points in dac map (for TGraph)
  int npt, pt;

  // temporary objects for loop
  TGraphErrors* gr;
  TGraphErrors* grrms;
  uint32_t dac;
  double charge, mean, mean2, meansq, rms, err, ratio;
  int nevt, ndigerr = 0;
  int badPts;
  double maxRMS;
  double maxPointInFitRange;

  // linear fit for the calibration factor
  TF1 *fslope = new TF1("fslope", "[0]*x", 0, 1000);

  m_scanMap = new TMap(20000, 1);
  m_scanMapRMS = new TMap(20000, 1);

  // iterators over adc maps
  TAdcDoubleMapIter adcIter(m_MeanMap.begin());
  TAdcDoubleMapIter adcIterE(m_MeanMap.end());

  // loop over all adcs
  for (; adcIter != adcIterE; ++adcIter) {
    hwid = (adcIter)->first;
    MeanDacMap = (adcIter)->second;
    MeanSqDacMap = m_MeanSqMap[hwid];
    NEvtDacMap = m_NEvtMap[hwid];
    NDigitalErrorsDacMap = m_NDigitalErrorsMap[hwid];

    int ros = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4
    int drawer = m_tileHWID->drawer(hwid); // 0 to 63
    int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
    int gain = m_tileHWID->adc(hwid);      // low=0 high=1

    int fragId = (ros << 8) | drawer;
    bool demonstrator = (std::binary_search(m_fragIDsDemonstrators.begin(), m_fragIDsDemonstrators.end(), fragId));
    int gain_offset = (demonstrator) ? 2 : 0;
    int gain_ind = gain + gain_offset;
    int cap_ind = (m_useSmallCap) ? 1 : 0;
    double dac2Charge = m_dac2Charge[cap_ind+gain_offset];
    double prevMean = 0.0, deltaMean = 0.0, averSlope = 0.0;
    double prevCharge = 0.0, deltaCharge = 0.0, deltaChargeMin = 999999.;
    double prevSlope = 0.0, slope = 0.0, maxGoodAmp=0.0, minGoodAmp = 999999.;
    double maxGoodCharge = m_linfitMax[gain_ind], minGoodCharge = 999999.;
    double maxCharge = 0.0, minCharge = 999999., maxAmp = 0.0, minAmp = 999999.;
    bool signalInRange = true;
    int nptGood = 0, nptBefore = 0;

    // find number of points in graph for this adc
    npt = MeanDacMap->size();
    m_nDAC[ros][drawer][chan][gain] = npt;
    gr = new TGraphErrors(npt);
    grrms = new TGraphErrors(npt);

    if (npt == 0) {
      m_calib[ros][drawer][chan][gain] = 0;
      m_chi2[ros][drawer][chan][gain] = 0.0;
      ATH_MSG_DEBUG( "npt==0 for adc channel "
                    << ros << "/" << drawer << "/" << chan << "/" << gain );
    } else {

      // update quality flag: adc channel is included in run
      setBit(includedBit, m_qflag[ros][drawer][chan][gain]);

      // iterator over dacs
      TDACDoubleMapIter dacIter((*MeanDacMap).begin());
      TDACDoubleMapIter dacIterE((*MeanDacMap).end());

      // initialize current point
      pt = 0;
      badPts = 0;
      maxPointInFitRange = 0.0;
      maxRMS = 0.0;
      for (; dacIter != dacIterE; ++dacIter) {
        dac = (dacIter)->first;
        mean = (dacIter)->second;
        meansq = (*MeanSqDacMap)[dac];
        nevt = (*NEvtDacMap)[dac];
        ndigerr = (*NDigitalErrorsDacMap)[dac];

        mean = mean / nevt;
        mean2 = mean * mean;
        meansq = meansq / nevt;

        rms = (meansq <= mean2) ? 0. : sqrt(meansq - mean2);
        err = sqrt(rms * rms / nevt + 0.5 * 0.5); // 0.5 is the absolute systematic uncertainty on the measurement

        // find charge for this dac
        charge = dac * dac2Charge;

        if (mean < m_maxAmp[gain_ind] ) {
          if (charge>maxCharge) maxCharge = charge;
          if (charge<minCharge) minCharge = charge;
          if (mean>maxAmp) maxAmp = mean;
          if (mean<minAmp) minAmp = mean;
        }

        deltaMean = mean - prevMean;
        deltaCharge = charge - prevCharge;
        if (deltaCharge != 0) {
          slope = deltaMean / deltaCharge;
          if (deltaCharge<deltaChargeMin) deltaChargeMin = deltaCharge;
        }

        // check for problems in calibration range
        if (charge > m_linfitMin[gain_ind] && charge < m_linfitMax[gain_ind]) {
          if (mean > m_maxAmp[gain_ind] ) {
            signalInRange = false;
            ATH_MSG_DEBUG( "Too high amp in "
                           << ros << "/" << drawer
                           << "/" << chan << "/" << gain
                           << " charge " << charge
                           << " amp " << mean
                           << " => Ignoring this point ");
          } else {
            if (prevSlope != 0) {
              double R = std::abs(slope/prevSlope);
              double R1 = (nptGood>1) ? (slope / averSlope) : 1.;
              if (R<0.025 || (nptGood>2 && R1>2.5 && R>2.5) || (nptGood==2 && R1>4.1) ) {
                // amplitude not changed (within +/- 2.5%) or changed too much, ignore this point
                ATH_MSG_WARNING( "Wrong amp in "
                                 << ros << "/" << drawer
                                 << "/" << chan << "/" << gain
                                 << " charge " << charge
                                 << " amp " << mean
                                 << " dC " << deltaCharge
                                 << " dA " << deltaMean
                                 << " expected dA " << prevSlope*deltaCharge
                                 << " aver dA " << averSlope*deltaCharge
                                 << " rms " << rms
                                 << " => Removing this point ");
                continue;
              }
            }
            if (signalInRange) {
              maxGoodCharge = charge;
              if (charge<minGoodCharge) minGoodCharge = charge;
              if (mean>maxGoodAmp) maxGoodAmp = mean;
              if (mean<minGoodAmp) minGoodAmp = mean;
            }
            prevMean = mean;
            prevCharge = charge;
            if (slope > 0) {
              prevSlope = slope;
            }
            ++nptGood;
            averSlope += (slope - averSlope) / nptGood;
            if (slope < 0) {
              --nptGood;
            }
            if (averSlope < 0) {
              nptGood = 0;
              averSlope = 0.;
            }
          }

          if (rms < 0.01) badPts++;
          if (mean > maxPointInFitRange) maxPointInFitRange = mean;
          if (rms > maxRMS) maxRMS = rms;

        } else if (charge <= m_linfitMin[gain_ind] && charge > 0) {

          slope = mean/charge;
          if (prevSlope !=0 ) {
            double R = slope / prevSlope;
            if (R>0.4 && R<2.5) {
              prevMean = mean;
              prevCharge = charge;
              //prevSlope = slope;
              prevSlope = (prevSlope + slope)/2.;
              //prevSlope = (prevSlope*nptBefore + slope)/(nptBefore+1);
              ++nptBefore;
            }
          } else {
            double R = slope / m_defaultCalib[gain_ind];
            if (R>0.4 && R<2.5) {
              prevSlope = slope;
              ++nptBefore;
            }
          }
        } else if (nptGood < 3 && signalInRange && charge >= m_linfitMax[gain_ind] && mean < m_maxAmp[gain_ind] && prevSlope != 0) {
          double R = slope/prevSlope;
          if (R>0.9 && R<1.1) {
            maxGoodCharge = charge;
            if (charge<minGoodCharge) minGoodCharge = charge;
            ++nptGood;
          }
        }

        // set point and errors in tgraph
        gr->SetPoint(pt, charge, mean);
        gr->SetPointError(pt, 0.0, err);

        grrms->SetPoint(pt, charge, mean);
        grrms->SetPointError(pt, 0.0, rms);

        pt++;
      } // end of for all DAC values

      // remove empty points at the end
      for (int i=npt-1; i>=pt; --i)
        gr->RemovePoint(i);

      slope = (prevMean > 0 && prevCharge > 0) ? prevMean/prevCharge : m_defaultCalib[gain_ind];
      fslope->SetParameter(0, slope);
      if (maxGoodCharge > m_linfitMax[gain_ind]) {
        ATH_MSG_WARNING( "Extending fit range for "
                         << ros << "/" << drawer
                         << "/" << chan << "/" << gain
                         << " up to " << maxGoodCharge
                         << " pC; N good points " << nptGood);
      }

      if (deltaChargeMin>999.) deltaChargeMin = 0.1;
      else deltaChargeMin *= 0.5;
      gr->Fit("fslope", "q", "", minGoodCharge-deltaChargeMin, maxGoodCharge+deltaChargeMin);
      ATH_MSG_VERBOSE( "Fit for "
                       << ros << "/" << drawer
                       << "/" << chan << "/" << gain
                       << " minC " << minGoodCharge
                       << " maxC " << maxGoodCharge
                       << " deltaC " << deltaChargeMin*2
                       << " minA " << minGoodAmp
                       << " maxA " << maxGoodAmp
                       << " nptG " << nptGood
                       << " slope " << slope
                       << " fit " << fslope->GetParameter(0));
      if (nptGood < 3) {
        const char * bms[3] = {"No good points to fit ",
                               "Only one point to fit ",
                               "Only two points to fit "};
        const char * ems[3] = {" => Put zero slope",
                               "",
                               ""};
        ATH_MSG_WARNING( bms[nptGood]
                         << ros << "/" << drawer
                         << "/" << chan << "/" << gain
                         << " charge " << charge
                         << " amp " << mean
                         << " slope " << slope
                         << " fit " << fslope->GetParameter(0)
                         << ems[nptGood]);
      }
      slope = (nptGood > 0) ? fslope->GetParameter(0) : 0.;

      if (slope > 0.) {
        if (maxGoodCharge > minGoodCharge) {
          averSlope = (maxGoodAmp-minGoodAmp)/(maxGoodCharge-minGoodCharge);
          if (slope > 2.5 * averSlope) {
            ATH_MSG_WARNING( "Average slope and fit slope do not match "
                             << ros << "/" << drawer
                             << "/" << chan << "/" << gain
                             << " charge " << charge
                             << " amp " << mean
                             << " avslope " << averSlope
                             << " fit " << slope
                             << " => Put zero slope");
            slope = 0.;
          }
        } else if (maxCharge > minCharge) {
          averSlope = (maxAmp-minAmp)/(maxCharge-minCharge);
          if (slope > 10. * averSlope) {
            ATH_MSG_WARNING( "AVERAGE slope and fit slope do not match "
                             << ros << "/" << drawer
                             << "/" << chan << "/" << gain
                             << " charge " << charge
                             << " amp " << mean
                             << " AVslope " << averSlope
                             << " fit " << slope
                             << " => Put zero slope");
            slope = 0.;
          }
        }
      }

      m_nDigitalErrors[ros][drawer][chan][gain] = ndigerr;

      // Set this bit if there aren't any digital errors
      if (ndigerr == 0) {
        setBit(digiErrorBit, m_qflag[ros][drawer][chan][gain]);
      }

      m_calib[ros][drawer][chan][gain] = slope;
      if (fslope->GetNDF() == 0)
        m_chi2[ros][drawer][chan][gain] = 0.0;
      else
        m_chi2[ros][drawer][chan][gain] = fslope->GetChisquare() / fslope->GetNDF();

      // Set this bit if there is a good Chi2 probability
      if (TMath::Prob(fslope->GetChisquare(), fslope->GetNDF()) > 2.e-6) {
        setBit(probChi2Bit, m_qflag[ros][drawer][chan][gain]);
      }

      // update quality flag if calibration is successful
      if (!badPts && fslope->GetNDF() > 0) setBit(calibratedBit, m_qflag[ros][drawer][chan][gain]);

      // update quality flag if calibration is within 5% of nominal
      // saved for legacy support
      ratio = (slope / m_defaultCalib[gain_ind]);
      if (ratio > 0.95 && ratio < 1.05) setBit(rangeBit, m_qflag[ros][drawer][chan][gain]);

      // update quality flag if calibration if the probability of this calibration
      // constant, given a 1.6% gaussian-sigma of the calibration constants, is greater
      // than 1/10000 (number of channels)
      //
      // Mathemica code:  NSolve[Erf[x/(1.6*Sqrt[2])] == 0.9999, x]
      // x -> 6.22495
      ratio = (slope / meanCalib[gain_ind]);
      if (ratio > 0.9378 && ratio < 1.0623) setBit(probBit, m_qflag[ros][drawer][chan][gain]);

      // If the maximum response in the fit range is less than 600 ADC counts, then
      // all the response in most likely noise
      if (maxPointInFitRange > 600) {
        setBit(noiseBit, m_qflag[ros][drawer][chan][gain]);
      }

      // RMS criteria.  If any collection of injections at a fixed-charge has
      // an RMS less than 5 ADC counts, then set this bit.
      if (maxRMS < 5.0) {
        setBit(injRMSBit, m_qflag[ros][drawer][chan][gain]);
      }

      // set the sample check bits

      // this bit is set if there were no events found in the fit range  
      // with the maximum sample value in the first or last sample 

      if (m_edgeSample[ros][drawer][chan][gain] == 0) {
        setBit(edgeSamp, m_qflag[ros][drawer][chan][gain]);
      }
      // this bit is set if there were no events found in the fit range 
      // with the maximum sample value in the second or sixth sample 
      if (m_nextToEdgeSample[ros][drawer][chan][gain] == 0) {
        setBit(nextToEdgeSamp, m_qflag[ros][drawer][chan][gain]);
      }

      // Determine failure/passing of StuckBit quality flag
      // And store information about bits in an array
      // which will be written to the ntuple
      int NoStuckBit = 1;
      for(int i = 0; i < NBITS; i++) {
        // If a bit is stuck at zero...
        if(m_sampleBit[ros][drawer][chan][gain][i] == 0  && (m_numSamp[ros][drawer][chan][gain] != 0)) {
          // write information to m_bitStatus array of shorts
          // each bit in short corresponds to a bit in an adc
          // with 6 short bits left over
          m_bitStatus[ros][drawer][chan][gain][0] += (1<<i);
          NoStuckBit = 0;
          ATH_MSG_DEBUG( "\n\nBIT STUCK AT ZERO: "
                  << ros << "   " << drawer << "   " << chan << "   " << gain <<  " " << i << "\n");

        }
       // Same for a bit stuck at one
        else if (m_sampleBit[ros][drawer][chan][gain][i] == m_numSamp[ros][drawer][chan][gain] && (m_numSamp[ros][drawer][chan][gain] != 0)) {
          m_bitStatus[ros][drawer][chan][gain][1] += (1<<i);
          NoStuckBit = 0;
          ATH_MSG_DEBUG( "\n\nBIT STUCK AT ONE: "
                  << ros << "   " << drawer << "   " << chan << "   " << gain <<  " " << i << "\n");
        }
      } //end bit loop

      // If no stuck bits are found, this adc passes StuckBit m_qflag
      if(NoStuckBit) {
        setBit(stuckbitBit, m_qflag[ros][drawer][chan][gain]);
      }

      gr->SetName("scan_" + arrayString(ros, drawer, chan, gain));
      grrms->SetName("scan_" + arrayString(ros, drawer, chan, gain));

      m_scanMap->Add(new TObjString("scan" + arrayString(ros, drawer, chan, gain)), gr);
      m_scanMapRMS->Add(new TObjString("scan" + arrayString(ros, drawer, chan, gain)), grrms);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::writeNtuple(int runNumber, int runType, TFile* rootFile) {

  ATH_MSG_INFO( "writeNtuple(" << runNumber << "," << runType << "," << rootFile << ")" );

  TTree *t = new TTree(m_ntupleID.c_str(), "TileCalib-Ntuple");
  t->Branch("RunNumber", &runNumber, "runNo/I");
  t->Branch("calib", *m_calib, "calib[5][64][48][2]/F");
  t->Branch("qflag", *m_qflag, "qflag[5][64][48][2]/I");
  t->Branch("nDAC", *m_nDAC, "nDAC[5][64][48][2]/I");
  t->Branch("nDigitalErrors", *m_nDigitalErrors, "nDigitalErrors[5][64][48][2]/I");
  t->Branch("chi2", *m_chi2, "chi2[5][64][48][2]/F");
  t->Branch("BitStatus", *m_bitStatus, "BitStatus[5][64][48][2][4]/s");

  if (!m_stuckBitsProbs.empty()) {
    if (m_stuckBitsProbs.retrieve().isFailure()) {
      ATH_MSG_WARNING("Impossible to get ITileStuckBitsProbsTool and stuck bits probabilities!");
    } else {
      m_stuckBitsProbs->saveStuckBitsProbabilities(t);
    }
  }

  // Fill with current values (i.e. tree will have only one entry for this whole run)
  t->Fill();
  t->Write();

  // Save graphs for all calibrated adc channels
  m_scanMap->Write("cisScans", TObject::kSingleKey);
  m_scanMapRMS->Write("cisScansRMS", TObject::kSingleKey);

  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::finalize() {

  ATH_MSG_INFO( "finalize()" );

  return StatusCode::SUCCESS;
}
