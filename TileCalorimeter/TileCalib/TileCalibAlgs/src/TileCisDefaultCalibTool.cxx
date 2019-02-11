/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "TileCalibAlgs/TileCisDefaultCalibTool.h"
#include "TileEvent/TileRawChannelContainer.h"
#include "TileEvent/TileDigitsContainer.h"
#include "TileIdentifier/TileHWID.h"
#include "TileConditions/TileCablingSvc.h"

#include "TileMonitoring/ITileStuckBitsProbsTool.h"

#include "TFile.h"
#include "TTree.h"
#include "TObjString.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TMap.h"
#include "TMath.h"
#include <cmath>

static const double c_dac2ChargeSmall(2.0 * 4.096 * 5.2 / 1023.0);
static const double c_dac2ChargeLarge(2.0 * 4.096 * 100.0 / 1023.0);
static const double c_defaultHiCalib(81.8);
static const double c_defaultLoCalib(1.29);

TileCisDefaultCalibTool::TileCisDefaultCalibTool(const std::string& type, const std::string& name,
    const IInterface* pParent)
    : AthAlgTool(type, name, pParent)
  , m_tileHWID(0)
  , m_cabling(0)
  , m_cablingSvc("TileCablingSvc", name)
  , m_stuckBitsProbs("")
  , m_scanMap(0)
  , m_scanMapRMS(0)
{
  declareInterface<ITileCalibTool>(this);

  declareProperty("NtupleID", m_ntupleID = "h3000");

  declareProperty("removePed", m_removePed = false);
  declareProperty("useSmallCap", m_useSmallCap = false);
  declareProperty("phaseMin", m_phaseMin = -10);
  declareProperty("phaseMax", m_phaseMax = 300);
  declareProperty("chargeMaxHi", m_chargeMaxHi = 12.5);
  declareProperty("chargeMaxLo", m_chargeMaxLo = 700);

  declareProperty("linfitMaxHi", m_linfitMaxHi = 10.0);
  declareProperty("linfitMinHi", m_linfitMinHi = 3.0);
  declareProperty("linfitMaxLo", m_linfitMaxLo = 700.0);
  declareProperty("linfitMinLo", m_linfitMinLo = 300.0);

  declareProperty("doSampleChecking", m_doSampleChecking = true); // do sample checking by default
  declareProperty("StuckBitsProbsTool", m_stuckBitsProbs);
  declareProperty("TileDQstatus", m_dqStatusKey = "TileDQstatus");

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

  CHECK( m_dqStatusKey.initialize() );

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

  // Find event's charge (for checking later that charge is in useful range)
  double charge = (cispar[7] > 10) ? (double) dac * c_dac2ChargeLarge
                                   : (double) dac * c_dac2ChargeSmall;

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
        if (NEvtDacMap == 0) {
          NEvtDacMap = (m_NEvtMap)[hwid] = new TDACIntMap;
          NDigitalErrorsDacMap = (m_NDigitalErrorsMap)[hwid] = new TDACIntMap;
          MeanDacMap = (m_MeanMap)[hwid] = new TDACDoubleMap;
          MeanSqDacMap = (m_MeanSqMap)[hwid] = new TDACDoubleMap;
        }

        // check that charge is less than chargeMax (depends on gain)
        if ((gain == 0 && charge < m_chargeMaxLo) || charge < m_chargeMaxHi) {

          // Hack to get rid of pedestal events: need more sophisticated method!          
          if (!m_removePed || (gain == 0 && (*it)->amplitude() > 6)
              || (gain == 1 && (*it)->amplitude() > 40)) {

            // Digital error check
            if (!(theDQstatus->isAdcDQgood(ros, drawer, chan, gain))) {
              ATH_MSG_DEBUG(  "Skipping Module: " << ros << drawer + 1
                            << " channel: " << chan
                            << " ADC: " << gain
                            << " due to DQ error found." );

              int ndigerr = (*NDigitalErrorsDacMap)[dac];
              ndigerr += 1;
              (*NDigitalErrorsDacMap)[dac] = ndigerr;
              continue;
            } else {
              // increment entries for current dac value
              int nevt = (*NEvtDacMap)[dac];
              double mean = (*MeanDacMap)[dac];
              double meansq = (*MeanSqDacMap)[dac];

              nevt++;
              mean += (*it)->amplitude();
              meansq += pow((*it)->amplitude(), 2);

              (*NEvtDacMap)[dac] = nevt;
              (*MeanDacMap)[dac] = mean;
              (*MeanSqDacMap)[dac] = meansq;
            }

          }

        } // end if charge < maxCharge
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
               
          if (pass) {
            //MM - only consider fit range
            if (gain == 0) {
              if ((charge < m_linfitMinLo) || (charge > m_linfitMaxLo)) {
                continue;
              }
            } else if (gain == 1) {
              if ((charge < m_linfitMinHi) || (charge > m_linfitMaxHi)) {
                continue;
              }
            }

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

  // hardware id (key to maps)
  HWIdentifier hwid;

  // dac maps
  TDACDoubleMap* MeanDacMap;
  TDACDoubleMap* MeanSqDacMap;
  TDACIntMap* NEvtDacMap;
  TDACIntMap* NDigitalErrorsDacMap;

  // count number of points in dac map (for TGraph)
  int npt, pt;

  // count number of adcs (length of vectors in ntuple)
  //int nadc;

  // temporary objects for loop
  TGraphErrors* gr;
  TGraphErrors* grrms;
  uint32_t dac;
  double charge, mean, meansq, rms, ratio;
  int nevt, ndigerr = 0;
  int badPts;
  double maxRMS;
  float maxPointInFitRange;

  // linear fit for the calibration factor
  TF1 *fslope = new TF1("fslope", "[0]*x", 0, 1000);

  //  scanList = new TList();
  m_scanMap = new TMap(20000, 1);
  m_scanMapRMS = new TMap(20000, 1);

  // iterators over adc maps
  TAdcDoubleMapIter adcIter(m_MeanMap.begin());
  TAdcDoubleMapIter adcIterE(m_MeanMap.end());

  // initialize number of adcs
  //nadc = 0;

  // loop over all adcs
  for (; adcIter != adcIterE; adcIter++) {
    hwid = (adcIter)->first;
    MeanDacMap = (adcIter)->second;
    MeanSqDacMap = m_MeanSqMap[hwid];
    NEvtDacMap = m_NEvtMap[hwid];
    NDigitalErrorsDacMap = m_NDigitalErrorsMap[hwid];

    int ros = m_tileHWID->ros(hwid);    // LBA=1  LBC=2  EBA=3  EBC=4
    int drawer = m_tileHWID->drawer(hwid); // 0 to 63
    int chan = m_tileHWID->channel(hwid);  // 0 to 47 channel not PMT
    int gain = m_tileHWID->adc(hwid);      // low=0 high=1

    // find number of points in graph for this adc
    npt = MeanDacMap->size();
    m_nDAC[ros][drawer][chan][gain] = npt;
    gr = new TGraphErrors(npt);
    grrms = new TGraphErrors(npt);

    if (npt == 0) {
      // Arms of conditional identical; prevent coverity warning.
#if 0
      if (gain == 0) {
        m_calib[ros][drawer][chan][gain] = 0; //c_defaultLoCalib;
      } else
#endif
      {
        m_calib[ros][drawer][chan][gain] = 0; //c_defaultHiCalib;
      }
      m_chi2[ros][drawer][chan][gain] = 0.0;
      ATH_MSG_DEBUG( "npt==0 for adc channel "
                    << ros << "   " << drawer << "   " << chan << "   " << gain );
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
      for (; dacIter != dacIterE; dacIter++) {
        dac = (dacIter)->first;
        mean = (dacIter)->second;
        meansq = (*MeanSqDacMap)[dac];
        nevt = (*NEvtDacMap)[dac];
        ndigerr = (*NDigitalErrorsDacMap)[dac];

        mean = mean / (double) nevt;
        meansq = meansq / (double) nevt;

        if (meansq <= pow(mean, 2))
          rms = 0;
        else
          rms = sqrt(meansq - pow(mean, 2));

        // find charge for this dac
        if (m_useSmallCap)
          charge = (double) dac * c_dac2ChargeSmall;
        else
          charge = (double) dac * c_dac2ChargeLarge;

        // check for problems in calibration range
        if (gain == 0) {
          if (charge > m_linfitMinLo && charge < m_linfitMaxLo && rms < 0.01) {
            badPts++;
          }
          if (charge > m_linfitMinLo && charge < m_linfitMaxLo && mean > maxPointInFitRange) {
            maxPointInFitRange = mean;
          }
          if (charge > m_linfitMinLo && charge < m_linfitMaxLo && rms > maxRMS) {
            maxRMS = rms;
          }

        } else {
          if (charge > m_linfitMinHi && charge < m_linfitMaxHi && rms < 0.01) {
            badPts++;
          }
          if (charge > m_linfitMinHi && charge < m_linfitMaxHi && mean > maxPointInFitRange) {
            maxPointInFitRange = mean;
          }
          if (charge > m_linfitMinHi && charge < m_linfitMaxHi && rms > maxRMS) {
            maxRMS = rms;
          }

        }

        // set point and errors in tgraph
        gr->SetPoint(pt, charge, mean);
        gr->SetPointError(pt, 0.0, sqrt((rms / sqrt(nevt)) * (rms / sqrt(nevt)) + 0.5 * 0.5)); // 0.5 is the absolute systematic uncertainty on the measurement

        grrms->SetPoint(pt, charge, mean);
        grrms->SetPointError(pt, 0.0, rms);

        pt++;
      } // end of for all DAC values

      if (gain == 0) {
        fslope->SetParameter(0, c_defaultLoCalib);
        gr->Fit("fslope", "q", "", m_linfitMinLo, m_linfitMaxLo);
      } else {
        fslope->SetParameter(0, c_defaultHiCalib);
        gr->Fit("fslope", "q", "", m_linfitMinHi, m_linfitMaxHi);
      }

      m_nDigitalErrors[ros][drawer][chan][gain] = ndigerr;

      // Set this bit if there aren't any digital errors
      if (ndigerr == 0) {
        setBit(digiErrorBit, m_qflag[ros][drawer][chan][gain]);
      }

      m_calib[ros][drawer][chan][gain] = fslope->GetParameter(0);
      if (fslope->GetNDF() == 0)
        m_chi2[ros][drawer][chan][gain] = 0.0;
      else
        m_chi2[ros][drawer][chan][gain] = fslope->GetChisquare() / fslope->GetNDF();

      // Set this bit if there is a good Chi2 probability
      if (TMath::Prob(fslope->GetChisquare(), fslope->GetNDF()) > 2 * pow(10, -6)) {
        setBit(probChi2Bit, m_qflag[ros][drawer][chan][gain]);
      }

      // update quality flag if calibration is successful
      if (!badPts && fslope->GetNDF() > 0) setBit(calibratedBit, m_qflag[ros][drawer][chan][gain]);

      // update quality flag if calibration is within 5% of nominal
      // saved for legacy support
      if (gain == 0)
        ratio = (fslope->GetParameter(0) / c_defaultLoCalib);
      else
        ratio = (fslope->GetParameter(0) / c_defaultHiCalib);
      if (ratio > 0.95 && ratio < 1.05) setBit(rangeBit, m_qflag[ros][drawer][chan][gain]);

      // update quality flag if calibration if the probability of this calibration
      // constant, given a 1.6% gaussian-sigma of the calibration constants, is greater
      // than 1/10000 (number of channels)
      //
      // Mathemica code:  NSolve[Erf[x/(1.6*Sqrt[2])] == 0.9999, x]
      // x -> 6.22495
      //
      // The values of 81.454 and 1.295 are derived from using runs:
      // 72652 73305 72653 72661 79259 78023 79781 78026
      // to calibrate the detector, and looking at the mean good calibration value 
      if (gain == 0)
        ratio = (fslope->GetParameter(0) / 1.295);
      else
        ratio = (fslope->GetParameter(0) / 81.454);
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
          m_bitStatus[ros][drawer][chan][gain][0] += pow(2, i);
          NoStuckBit = 0;
          ATH_MSG_DEBUG( "\n\nBIT STUCK AT ZERO: "
                  << ros << "   " << drawer << "   " << chan << "   " << gain <<  " " << i << "\n");

        }
       // Same for a bit stuck at one
        else if (m_sampleBit[ros][drawer][chan][gain][i] == m_numSamp[ros][drawer][chan][gain] && (m_numSamp[ros][drawer][chan][gain] != 0)) {
          m_bitStatus[ros][drawer][chan][gain][1] += pow(2, i);
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

      //scanList->Add(gr);
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
  //  scanList->Write("cisScans",TObject::kSingleKey);
  m_scanMap->Write("cisScans", TObject::kSingleKey);
  m_scanMapRMS->Write("cisScansRMS", TObject::kSingleKey);

  return StatusCode::SUCCESS;
}

StatusCode TileCisDefaultCalibTool::finalize() {

  ATH_MSG_INFO( "finalize()" );

  return StatusCode::SUCCESS;
}
