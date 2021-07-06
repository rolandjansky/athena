/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include <iomanip>
#include <set>
#include <sstream>

#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "LWHists/LWHist.h"
#include "LWHists/LWHist1D.h"
#include "LWHists/LWHist2D.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TProfile_LW.h"
#include "LWHists/TProfile2D_LW.h"

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
//#include "TrigConfigSvc/ILVL1ConfigSvc.h"
#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloUtils/QuadLinear.h"

#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramToolV1.h"
// ============================================================================
namespace LVL1 {
// ============================================================================
// Constructor

TrigT1CaloLWHistogramToolV1::TrigT1CaloLWHistogramToolV1(const std::string& name)
        : AsgTool(name),
	  m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
	  m_monGroup(0), m_phiScaleTT(32./M_PI), m_phiScaleJE(16./M_PI)
{
  declareInterface<TrigT1CaloLWHistogramToolV1>(this);
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

  declareProperty("EventSamples", m_eventSamples = 10,
                  "Number of Error Event Number Samples");
  declareProperty("ShrinkEtaBins", m_shrinkEtaBins = true,
                  "Make all eta bins the same size in eta/phi plots");
}

// Destructor

TrigT1CaloLWHistogramToolV1::~TrigT1CaloLWHistogramToolV1()
{
}

// Initialize

StatusCode TrigT1CaloLWHistogramToolV1::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

  // Connect to the TrigConfigSvc for the trigger configuration:

  if (m_configSvc.typeAndName() != "") {
    StatusCode sc = m_configSvc.retrieve();
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                      << endmsg;
      return sc;
    } else msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName()
                          << endmsg;
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode TrigT1CaloLWHistogramToolV1::finalize()
{
  return StatusCode::SUCCESS;
}

//===========================================================================
//  Labelling Utilities - General
//===========================================================================

// Return int/double pair as a string

std::string TrigT1CaloLWHistogramToolV1::intDoubleString(int num, double val,
                                                              int precision)
{
  std::ostringstream cnum;
  cnum << num << "/" 
       << std::setiosflags(std::ios::fixed | std::ios::showpoint)
       << std::setprecision(precision) << val;
  return cnum.str();
}

// Label bins with number pairs

void TrigT1CaloLWHistogramToolV1::numberPairs(LWHist* hist, int firstMin,
                                int firstMax, int secondMin, int secondMax,
	                        int step, int offset, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  const int numSecond = secondMax - secondMin + 1;
  for (int first = firstMin; first <= firstMax; ++first) {
    int bin = 1 + offset + (first-firstMin)*numSecond;
    for (int second = secondMin; second <= secondMax; second += step) {
      std::ostringstream cnum;
      cnum << first << "/" << second;
      axis->SetBinLabel(bin, cnum.str().c_str());
      bin += step;
    }
  }
}

// Label bins with number pairs without skipping bins when stepping

void TrigT1CaloLWHistogramToolV1::numberPairs2(LWHist* hist,
               int firstMin, int firstMax,
               int secondMin, int secondMax, int step, int offset, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  int bin = 1 + offset;
  for (int first = firstMin; first <= firstMax; ++first) {
    for (int second = secondMin; second <= secondMax; second += step) {
      std::ostringstream cnum;
      cnum << first << "/" << second;
      axis->SetBinLabel(bin, cnum.str().c_str());
      bin++;
    }
  }
}

// Label bins with numbers

void TrigT1CaloLWHistogramToolV1::numbers(LWHist* hist, int min, int max,
                                        int step, int offset, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  int bin = 1 + offset;
  for (int num = min; num <= max; num += step) {
    std::ostringstream cnum;
    cnum << num;
    axis->SetBinLabel(bin, cnum.str().c_str());
    bin += step;
  }
}

// Split long names for Y axis

std::string TrigT1CaloLWHistogramToolV1::splitLine(const std::string& word,
                                                                  bool xAxis)
{
  std::string newWord(word);
  if (!xAxis && word.length() > 6) {
    // split at last capital
    std::string::size_type idx =
                               word.find_last_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (idx != std::string::npos && idx != 0 && idx != word.length()-1) {
      newWord = "#splitline{" + word.substr(0, idx) + "}{"
                              + word.substr(idx) + "}";
    }
  }
  return newWord;
}

// Label bins with sub-status error bit names

void TrigT1CaloLWHistogramToolV1::subStatus(LWHist* hist, int offset, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  const LVL1::DataError err(0);
  for (int bit = 0; bit < 8; ++bit) {
    std::string label(splitLine(err.bitName(bit +
                                LVL1::DataError::GLinkParity), xAxis));
    axis->SetBinLabel(bit + 1 + offset, label.c_str());
  }
}

// Get list of threshold names for given type
// NB. Treats EM/TAU as one

bool TrigT1CaloLWHistogramToolV1::thresholdNames(const std::string& type,
                                             std::vector<std::string>& names)
{
  bool found = false;
  names.clear();
  TrigConf::L1DataDef def;
//   int nthresh = 0;
//   if (type == def.emType() || type == def.tauType())
//                                   nthresh = def.max_EM_Threshold_Number();
//   else if (type == def.jetType()) nthresh = def.max_J_Threshold_Number();
//   else if (type == def.jfType())  nthresh = def.max_JF_Threshold_Number();
//   else if (type == def.jbType())  nthresh = def.max_JB_Threshold_Number();
//   else if (type == def.xeType())  nthresh = def.max_XE_Threshold_Number();
//   else if (type == def.jeType())  nthresh = def.max_JE_Threshold_Number();
//   else if (type == def.teType())  nthresh = def.max_TE_Threshold_Number();
//   else if (type == def.xsType())  nthresh = def.max_XS_Threshold_Number();
//   else return found;

  TrigConf::L1DataDef::TriggerType tt = TrigConf::L1DataDef::stringAsType(type);
  if(tt == TrigConf::L1DataDef::UNDEF) return found;

  TrigConf::L1DataDef::TriggerTypeConfig ttc = TrigConf::L1DataDef::typeConfig(tt);
  int nthresh = ttc.max;

  for (int thr = 0; thr < nthresh; ++thr) {
    std::ostringstream cnum;
    cnum << type << " " << thr;
    names.push_back(cnum.str());
  }

  if (m_configSvc) {
     const TrigConf::CTPConfig* ctpConf = m_configSvc->ctpConfig();
     if (ctpConf) {
        const std::vector<TrigConf::TriggerThreshold*>& thresholds = ctpConf->menu().thresholdVector();

        std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
        for (it = thresholds.begin(); it != thresholds.end(); ++it) {
           const std::string thrType((*it)->type());
           if (type == def.emType() || type == def.tauType()) {
              if (thrType != def.emType() && thrType != def.tauType()) continue;
           } else if (thrType != type) continue;
           const int threshNum = (*it)->thresholdNumber();
           if (threshNum >= 0 && threshNum < nthresh) {
              names[threshNum] = (*it)->name();
              found = true;
           }
        }
      
     }
  }
  return found;
}

// Label bins with threshold names

void TrigT1CaloLWHistogramToolV1::thresholds(LWHist* hist,
                              const std::string& type, int offset, bool xAxis)
{
   LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
   std::vector<std::string> names;
   //const bool found = thresholdNames(type, names);
   thresholdNames(type, names);
   std::vector<std::string>::const_iterator it = names.begin();
   for (int bin = 1; it != names.end(); ++it, ++bin) {
      axis->SetBinLabel(bin+offset, (*it).c_str());
   }
   //if (!found) axis->SetTitle("Threshold Number");
}

// Put threshold hit values into a string suitable for printing

std::string TrigT1CaloLWHistogramToolV1::thresholdString(int val, int nThresh,
                                                              int nBits)
{
  std::ostringstream cval;
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit = (val >> (nBits*thr)) & mask;
    cval << hit;
    if (thr != nThresh-1) cval << " ";
  }
  return cval.str();
}

// Flag which threshold hit values are non-zero and the same

int TrigT1CaloLWHistogramToolV1::thresholdsSame(int val1, int val2,
                                                      int nThresh, int nBits)
{
  int result = 0;
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit1 = (val1 >> (nBits*thr)) & mask;
    const int hit2 = (val2 >> (nBits*thr)) & mask;
    if (hit1 && (hit1 == hit2)) result |= (1 << thr);
  }
  return result;
}

// Flag which threshold hit values are different

int TrigT1CaloLWHistogramToolV1::thresholdsDiff(int val1, int val2,
                                                      int nThresh, int nBits)
{
  int result = 0;
  const int mask = (1 << nBits) - 1;
  for (int thr = 0; thr < nThresh; ++thr) {
    const int hit1 = (val1 >> (nBits*thr)) & mask;
    const int hit2 = (val2 >> (nBits*thr)) & mask;
    if (hit1 != hit2) result |= (1 << thr);
  }
  return result;
}

// Get threshold bit mask for given type
// NB. Treats EM and TAU separately

unsigned int TrigT1CaloLWHistogramToolV1::thresholdMask(const std::string& type)
{
   unsigned int mask = 0;
   //TrigConf::L1DataDef def;
   //   int nthresh = 0;
   //   if (type == def.emType() || type == def.tauType())
   //                                   nthresh = def.max_EM_Threshold_Number();
   //   else if (type == def.jetType()) nthresh = def.max_J_Threshold_Number();
   //   else if (type == def.jfType())  nthresh = def.max_JF_Threshold_Number();
   //   else if (type == def.jbType())  nthresh = def.max_JB_Threshold_Number();
   //   else if (type == def.xeType())  nthresh = def.max_XE_Threshold_Number();
   //   else if (type == def.jeType())  nthresh = def.max_JE_Threshold_Number();
   //   else if (type == def.teType())  nthresh = def.max_TE_Threshold_Number();
   //   else if (type == def.xsType())  nthresh = def.max_XS_Threshold_Number();

   TrigConf::L1DataDef::TriggerType tt = TrigConf::L1DataDef::stringAsType(type);
   TrigConf::L1DataDef::TriggerTypeConfig ttc = TrigConf::L1DataDef::typeConfig(tt);
   int nthresh = ttc.max;



   if (m_configSvc) {
      const TrigConf::CTPConfig* ctpConf = m_configSvc->ctpConfig();
      if (ctpConf) {
         const std::vector<TrigConf::TriggerThreshold*>& thresholds = ctpConf->menu().thresholdVector();
         std::vector<TrigConf::TriggerThreshold*>::const_iterator it;
         for (it = thresholds.begin(); it != thresholds.end(); ++it) {
            const std::string thrType((*it)->type());
            if (thrType != type) continue;
            const int threshNum = (*it)->thresholdNumber();
            if (threshNum >= 0 && threshNum < nthresh) {
               mask |= (1<<threshNum);
            }
         }
      }
   }
   return mask;
}

// Get threshold bit mask for EM

unsigned int TrigT1CaloLWHistogramToolV1::thresholdMaskEm()
{
  return thresholdMask(TrigConf::L1DataDef::emType());
}

// Get threshold bit mask for Tau

unsigned int TrigT1CaloLWHistogramToolV1::thresholdMaskTau()
{
  return thresholdMask(TrigConf::L1DataDef::tauType());
}

//===========================================================================
//  Labelling Utilities - CPM
//===========================================================================

// Label bins with CPM chip/local coordinate

void TrigT1CaloLWHistogramToolV1::cpmChipLocalCoord(LWHist* hist, int offset,
                                                                    bool xAxis)
{
  const int nChips = 8;
  const int nLoc   = 8;
  numberPairs(hist, 0, nChips-1, 0, nLoc-1, 4, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Chip/Local Coord");
}

// Label bins with CPM/CMM crate/module

void TrigT1CaloLWHistogramToolV1::cpmCMMCrateModule(LWHist* hist, int offset,
                                                                    bool xAxis)
{
  cpmCrateModule(hist, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+offset,  "CPM");
  axis->SetBinLabel(57+offset, "CMM");
  axis->SetBinLabel(59+offset, "1/0");
  axis->SetBinLabel(61+offset, "2/0");
  axis->SetBinLabel(63+offset, "3/0");
}

// Label bins with CPM crate/CMM

void TrigT1CaloLWHistogramToolV1::cpmCrateCMM(LWHist* hist, int offset,
                                                          bool xAxis)
{
  const int nCrates = 4;
  const int nCMMs   = 2;
  numberPairs(hist, 0, nCrates-1, 0, nCMMs-1, 1, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Crate/CMM");
}

// Label bins with CPM crate/module

void TrigT1CaloLWHistogramToolV1::cpmCrateModule(LWHist* hist, int offset,
                                                             bool xAxis)
{
  const int nCrates = 4;
  const int nCPMs = 14;
  numberPairs(hist, 0, nCrates-1, 1, nCPMs, nCPMs/2, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Crate/Module");
}

// Label bins with CPM RoI threshold names

void TrigT1CaloLWHistogramToolV1::cpmThresholds(LWHist* hist, int offset,
                                                            bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::emType(), offset, xAxis);
}

//===========================================================================
//  Labelling Utilities - JEM
//===========================================================================

// Label bins with JEM crate/module

void TrigT1CaloLWHistogramToolV1::jemCrateModule(LWHist* hist, int offset,
                                                             bool xAxis)
{
  const int nCrates = 2;
  const int nJEMs = 16;
  numberPairs(hist, 0, nCrates-1, 0, nJEMs-1, 2, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Crate/Module");
}

// Label bins with JEM/CMM crate/module

void TrigT1CaloLWHistogramToolV1::jemCMMCrateModule(LWHist* hist, int offset,
                                                                    bool xAxis)
{
  jemCrateModule(hist, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+offset,  "JEM");
  axis->SetBinLabel(33+offset, "CMM");
  axis->SetBinLabel(35+offset, "1/0");
}

// Label bins with JEM crate/CMM

void TrigT1CaloLWHistogramToolV1::jemCrateCMM(LWHist* hist, int offset,
                                                          bool xAxis)
{
  const int nCrates = 2;
  const int nCMMs   = 2;
  numberPairs(hist, 0, nCrates-1, 0, nCMMs-1, 1, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Crate/CMM");
}

// Label bins with JEM frame/local coord

void TrigT1CaloLWHistogramToolV1::jemFrameLoc(LWHist* hist, int offset,
                                                              bool xAxis)
{
  const int nFrame = 8;
  const int nLoc   = 4;
  numberPairs(hist, 0, nFrame-1, 0, nLoc-1, 2, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Frame/Local Coord");
}

// Label bins with JEM RoI threshold names

void TrigT1CaloLWHistogramToolV1::jemThresholds(LWHist* hist, int offset,
                                                                bool xAxis)
{
  int newOffset = offset;
  thresholds(hist, TrigConf::L1DataDef::jetType(), newOffset, xAxis);
  newOffset += TrigConf::L1DataDef::max_JET_Threshold_Number();
  thresholds(hist, TrigConf::L1DataDef::jbType(), newOffset, xAxis);
  newOffset += TrigConf::L1DataDef::max_JB_Threshold_Number();
  thresholds(hist, TrigConf::L1DataDef::jfType(), newOffset, xAxis);
}

// Label bins with Main Jet threshold names

void TrigT1CaloLWHistogramToolV1::mainJetThresholds(LWHist* hist, int offset,
                                                                    bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::jetType(), offset, xAxis);
}

// Label bins with Backward Jet threshold names

void TrigT1CaloLWHistogramToolV1::backwardJetThresholds(LWHist* hist, int offset,
                                                                    bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::jbType(), offset, xAxis);
}

// Label bins with Forward Jet threshold names

void TrigT1CaloLWHistogramToolV1::forwardJetThresholds(LWHist* hist, int offset,
                                                                   bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::jfType(), offset, xAxis);
}

// Label bins with JetEt threshold names

void TrigT1CaloLWHistogramToolV1::jetEtThresholds(LWHist* hist, int offset,
                                                              bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::jeType(), offset, xAxis);
}

// Label bins with MissingEt threshold names

void TrigT1CaloLWHistogramToolV1::missingEtThresholds(LWHist* hist, int offset,
                                                                  bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::xeType(), offset, xAxis);
}

// Label bins with SumEt threshold names

void TrigT1CaloLWHistogramToolV1::sumEtThresholds(LWHist* hist, int offset,
                                                              bool xAxis)
{
  thresholds(hist, TrigConf::L1DataDef::teType(), offset, xAxis);
}

// Label bins with MissingEtSig threshold names

void TrigT1CaloLWHistogramToolV1::missingEtSigThresholds(LWHist* hist, int offset,
                                                                     bool xAxis)
{
  //thresholds(hist, TrigConf::L1DataDef::xsType(), offset, xAxis);
  thresholds(hist, "XS", offset, xAxis);
}

//===========================================================================
//  Labelling Utilities - PPM
//===========================================================================

// Label bins with PPM crate/module

void TrigT1CaloLWHistogramToolV1::ppmCrateModule(LWHist* hist, int firstCrate,
                                      int lastCrate, int offset, bool xAxis)
{
  int step = 2;
  if (lastCrate-firstCrate > 1) step = 4;
  numberPairs(hist, firstCrate, lastCrate, 0, 15, step, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Crate/Module");
}

// Label bins with PPM error bit names

void TrigT1CaloLWHistogramToolV1::ppmErrors(LWHist* hist, int offset, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  const LVL1::DataError err(0);
  for (int bit = 0; bit < 8; ++bit) {
    std::string label(splitLine(err.bitName(bit +
                                LVL1::DataError::ChannelDisabled), xAxis));
    axis->SetBinLabel(bit + 1 + offset, label.c_str());
  }
}

// Label bins with PPM submodule/channel

void TrigT1CaloLWHistogramToolV1::ppmSubmoduleChannel(LWHist* hist, int offset,
                                                                  bool xAxis)
{
  numberPairs(hist, 0, 15, 0, 3, 4, offset, xAxis);
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetTitle("Submodule/Channel");
}

//===========================================================================
//  Booking Utilities - General
//===========================================================================

// Book and register a 1D histogram

TH1F_LW* TrigT1CaloLWHistogramToolV1::book1F(const std::string& name,
                                           const std::string& title,
                                           int nx, double xmin, double xmax)
{
  TH1F_LW *hist = TH1F_LW::create(name.c_str(), title.c_str(), nx, xmin, xmax);
  registerHist(hist);
  return hist;
}

// Book and register a 1D histogram with variable width bins

TH1F_LW* TrigT1CaloLWHistogramToolV1::book1F(const std::string& name,
                                           const std::string& title,
                                           int nx, const double* xbins)
{
  TH1F_LW *hist = TH1F_LW::create(name.c_str(), title.c_str(), nx, xbins);
  registerHist(hist);
  return hist;
}

// Book and register a 1D profile histogram

TProfile_LW* TrigT1CaloLWHistogramToolV1::bookProfile(const std::string& name,
                    const std::string& title, int nx, double xmin, double xmax)
{
  TProfile_LW *hist = TProfile_LW::create(name.c_str(), title.c_str(),
                                                              nx, xmin, xmax);
  registerHist(hist);
  return hist;
}

// Book and register a 1D profile histogram with variable width bins

TProfile_LW* TrigT1CaloLWHistogramToolV1::bookProfile(const std::string& name,
                         const std::string& title, int nx, const double* xbins)
{
  TProfile_LW *hist = TProfile_LW::create(name.c_str(), title.c_str(),
                                                                  nx, xbins);
  registerHist(hist);
  return hist;
}

// Book and register a 2D histogram

TH2F_LW* TrigT1CaloLWHistogramToolV1::book2F(const std::string& name,
                                           const std::string& title,
                                           int nx, double xmin, double xmax,  
	                                   int ny, double ymin, double ymax)
{		
  TH2F_LW *hist = TH2F_LW::create(name.c_str(), title.c_str(), nx, xmin, xmax,
                                                               ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D histogram with variable width bins

TH2F_LW* TrigT1CaloLWHistogramToolV1::book2F(const std::string& name,
                                           const std::string& title,
                                           int nx, const double* xbins,
                                           int ny, double ymin, double ymax)
{
  TH2F_LW *hist = TH2F_LW::create(name.c_str(), title.c_str(), nx, xbins,
                                                               ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D histogram with variable width bins both axes

TH2F_LW* TrigT1CaloLWHistogramToolV1::book2F(const std::string& name,
                                           const std::string& title,
                                           int nx, const double* xbins,
                                           int ny, const double* ybins)
{
  TH2F_LW *hist = TH2F_LW::create(name.c_str(), title.c_str(), nx, xbins,
                                                               ny, ybins);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D profile histogram

TProfile2D_LW* TrigT1CaloLWHistogramToolV1::bookProfile2D(
           const std::string& name, const std::string& title,
           int nx, double xmin, double xmax, int ny, double ymin, double ymax)
{		
  TProfile2D_LW *hist = TProfile2D_LW::create(name.c_str(), title.c_str(),
                                              nx, xmin, xmax, ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D profile histogram with variable width bins

TProfile2D_LW* TrigT1CaloLWHistogramToolV1::bookProfile2D(
                const std::string& name, const std::string& title,
                int nx, const double* xbins, int ny, double ymin, double ymax)
{
  TProfile2D_LW *hist = TProfile2D_LW::create(name.c_str(), title.c_str(),
                                                   nx, xbins, ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D profile histogram with variable width bins both axes

TProfile2D_LW* TrigT1CaloLWHistogramToolV1::bookProfile2D(
                const std::string& name, const std::string& title,
                int nx, const double* xbins, int ny, const double* ybins)
{
  TProfile2D_LW *hist = TProfile2D_LW::create(name.c_str(), title.c_str(),
                                                   nx, xbins, ny, ybins);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Book and register a 2D histogram of integers displayed as text

TH2I_LW* TrigT1CaloLWHistogramToolV1::book2I(const std::string& name,
                                           const std::string& title,
                                           int nx, double xmin, double xmax,
	                                   int ny, double ymin, double ymax)
{
  TH2I_LW *hist = TH2I_LW::create(name.c_str(), title.c_str(), nx, xmin, xmax,
                                                               ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("text");
  return hist;
}

// Book and register a 2D histogram containing event numbers as bin contents

TH2I_LW* TrigT1CaloLWHistogramToolV1::bookEventNumbers(
                const std::string& name, const std::string& title,
		int ny, double ymin, double ymax)
{
  TH2I_LW* hist = book2I(name, title, m_eventSamples, 0, m_eventSamples,
                                   ny, ymin, ymax);
  if (m_eventSamples <= 10) numbers(hist, 1, m_eventSamples);
  hist->GetXaxis()->SetTitle("Events with Error/Mismatch");
  return hist;
}

// Register a histogram

void TrigT1CaloLWHistogramToolV1::registerHist(LWHist* hist)
{
  if (m_monGroup && m_monGroup->regHist(hist) != StatusCode::SUCCESS) {
    msg(MSG::WARNING) << "Could not register histogram : "
                      << hist->GetName() << endmsg;
  }
}

// ROOT versions

// Book and register a 1D histogram

TH1F* TrigT1CaloLWHistogramToolV1::bookTH1F(const std::string& name,
                                          const std::string& title,
                                          int nx, double xmin, double xmax)
{
  TH1F* hist = new TH1F(name.c_str(), title.c_str(), nx, xmin, xmax);
  registerHist(hist);
  return hist;
}

// Book and register a 2D histogram

TH2F* TrigT1CaloLWHistogramToolV1::bookTH2F(const std::string& name,
                                          const std::string& title,
                                          int nx, double xmin, double xmax,  
	                                  int ny, double ymin, double ymax)
{		
  TH2F* hist = new TH2F(name.c_str(), title.c_str(), nx, xmin, xmax,
                                                     ny, ymin, ymax);
  registerHist(hist);
  hist->SetOption("colz");
  return hist;
}

// Register a histogram

void TrigT1CaloLWHistogramToolV1::registerHist(TH1* hist)
{
  if (m_monGroup && m_monGroup->regHist(hist) != StatusCode::SUCCESS) {
    msg(MSG::WARNING) << "Could not register histogram : "
                      << hist->GetName() << endmsg;
  }
  if (!m_monGroup) hist->SetDirectory(0);
}

//===========================================================================
//  Booking Utilities - CPM
//===========================================================================

// Book CPM crate/module vs chip/local coordinate

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMCrateModuleVsChipLocalCoord(
                     const std::string& name, const std::string& title)
{
  TH2F_LW *hist = book2F(name, title, 56, 0., 56., 64, 0., 64.);
  cpmCrateModule(hist);
  cpmChipLocalCoord(hist, 0, false);
  return hist;
}

// Book CPM crate/module vs FPGA

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMCrateModuleVsFPGA(
                     const std::string& name, const std::string& title)
{
  TH2F_LW *hist = book2F(name, title, 56, 0., 56., 20, 0., 20.);
  cpmCrateModule(hist);
  numbers(hist, 2, 17, 1, 2, false);
  // colour overlap fpga bins
  LWHist::LWHistAxis* axis = hist->GetYaxis();
  axis->SetBinLabel(1,  "#color[4]{0}");
  axis->SetBinLabel(2,  "#color[4]{1}");
  axis->SetBinLabel(19, "#color[4]{18}");
  axis->SetBinLabel(20, "#color[4]{19}");
  axis->SetTitle("Serialiser FPGA");
  return hist;
}

// Book CPM crate/module vs thresholds

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMCrateModuleVsThreshold(
                     const std::string& name, const std::string& title)
{
  TH2F_LW *hist = book2F(name, title, 56, 0., 56., 16, 0., 16.);
  cpmCrateModule(hist);
  cpmThresholds(hist, 0, false);
  return hist;
}

// Book CPM eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMEtaVsPhi(const std::string& name,
                                                    const std::string& title)
{
  TH2F_LW*  hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = bookPPMEmEtaVsPhi(name, title);
    axis = hist->GetXaxis();
    for (int bin = 1; bin <= 8; ++bin) {
      axis->SetBinLabel(bin,    "+");
      axis->SetBinLabel(bin+58, "+");
    }
  } else {
    hist = book2F(name, title, 50, -2.5, 2.5, 64, 0., 64.);
    hist->SetXTitle("eta");
    const double phiBin     = M_PI/32.;
    const double halfPhiBin = M_PI/64.;
    axis = hist->GetYaxis();
    for (int chan = 0; chan < 64; chan += 4 ) {
      const double rad = chan*phiBin + halfPhiBin;
      axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
    }
    axis->SetBinLabel(64, "phi");
  }
  return hist;
}

// Book CPM RoI eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMRoIEtaVsPhi(const std::string& name,
                                                       const std::string& title)
{
  TH2F_LW*  hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = book2F(name, title, 66, -3.3, 3.3, 64, 0., 64.);
    axis = hist->GetXaxis();
    for (int chan = -24; chan < 26; chan+=4) {
      const double eta = (chan/10.);
      axis->SetBinLabel(chan+33, intDoubleString(chan, eta).c_str());
    }
    for (int bin = 1; bin <= 8; ++bin) {
      axis->SetBinLabel(bin,    "+");
      axis->SetBinLabel(bin+58, "+");
    }
  } else {
    hist = book2F(name, title, 50, -2.45, 2.55, 64, 0., 64.);
    hist->SetXTitle("eta");
  }
  const double phiBin = M_PI/32.;
  axis = hist->GetYaxis();
  for (int chan = 0; chan < 64; chan += 4 ) {
    const double rad = (chan + 1)*phiBin;
    axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
  }
  if (m_shrinkEtaBins) axis->SetBinLabel(64, "etaVphi");
  else                 axis->SetBinLabel(64, "phi");
  return hist;
}

// Book CPM events with error/mismatch vs crate/module

TH2I_LW* TrigT1CaloLWHistogramToolV1::bookCPMEventVsCrateModule(
                          const std::string& name, const std::string& title)
{
  TH2I_LW* hist = bookEventNumbers(name, title, 56, 0., 56.);
  cpmCrateModule(hist, 0, false);
  return hist;
}

// Book CPM module vs crate

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMModuleVsCrate(
                     const std::string& name, const std::string& title)
{
  TH2F_LW *hist = book2F(name, title, 14, 1., 15., 4, 0., 4.);
  numbers(hist, 1, 14);
  numbers(hist, 0, 3, 1, 0, false);
  hist->GetXaxis()->SetTitle("Module");
  hist->GetYaxis()->SetTitle("Crate");
  return hist;
}

// Book CPM module vs crate/CMM

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMModuleVsCrateCMM(
                     const std::string& name, const std::string& title)
{
  TH2F_LW *hist = book2F(name, title, 14, 1., 15., 8, 0., 8.);
  numbers(hist, 1, 14);
  hist->GetXaxis()->SetTitle("Module");
  cpmCrateCMM(hist, 0, false);
  return hist;
}

// Book CPM sub-status errors vs crate/module

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMSubStatusVsCrateModule(
                            const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., 56, 0., 56.);
  subStatus(hist);
  cpmCrateModule(hist, 0, false);
  return hist;
}

// Book CPM Sum/CMM

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookCPMSumCMM(const std::string& name,
                                                  const std::string& title)
{
  TH1F_LW* hist = book1F(name, title, 16, 0., 16.);
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "L0/0");
  axis->SetBinLabel(2, "L0/1");
  axis->SetBinLabel(3, "L1/0");
  axis->SetBinLabel(4, "L1/1");
  axis->SetBinLabel(5, "L2/0");
  axis->SetBinLabel(6, "L2/1");
  axis->SetBinLabel(7, "L3/0");
  axis->SetBinLabel(8, "L3/1");
  axis->SetBinLabel(9, "R0/0");
  axis->SetBinLabel(10, "R0/1");
  axis->SetBinLabel(11, "R1/0");
  axis->SetBinLabel(12, "R1/1");
  axis->SetBinLabel(13, "R2/0");
  axis->SetBinLabel(14, "R2/1");
  axis->SetBinLabel(15, "T/0");
  axis->SetBinLabel(16, "T/1");
  axis->SetTitle("Sum/CMM");
  return hist;
}

// Book CPM Sum vs Threshold

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookCPMSumVsThreshold(
                            const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., 16, 0., 16.);
  LWHist::LWHistAxis* axis = hist->GetXaxis();
  axis->SetBinLabel(1, "L0");
  axis->SetBinLabel(2, "L1");
  axis->SetBinLabel(3, "L2");
  axis->SetBinLabel(4, "L3");
  axis->SetBinLabel(5, "R0");
  axis->SetBinLabel(6, "R1");
  axis->SetBinLabel(7, "R2");
  axis->SetBinLabel(8, "T");
  axis->SetTitle("Sum (Local/Remote/Total)");
  cpmThresholds(hist, 0, false);
  return hist;
}

//===========================================================================
//  Booking Utilities - JEM
//===========================================================================

// Book JEM crate/module vs EX,Ey,Et

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMCrateModuleVsExEyEt(
                             const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 32, 0., 32., 3, 0., 3.);
  jemCrateModule(hist);
  LWHist::LWHistAxis* axis = hist->GetYaxis();
  axis->SetBinLabel(1, "Ex");
  axis->SetBinLabel(2, "Ey");
  axis->SetBinLabel(3, "Et");
  return hist;
}

// Book JEM crate/module vs frame/local coord

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMCrateModuleVsFrameLoc(
                             const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 32, 0., 32., 32, 0., 32.);
  jemCrateModule(hist);
  jemFrameLoc(hist, 0, false);
  return hist;
}

// Book JEM crate/module vs thresholds

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMCrateModuleVsThresholds(
                             const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 32, 0., 32., 16, 0., 16.);
  jemCrateModule(hist);
  jemThresholds(hist, 0, false);
  return hist;
}

// Book JEM events with error/mismatch vs crate/module

TH2I_LW* TrigT1CaloLWHistogramToolV1::bookJEMEventVsCrateModule(
                             const std::string& name, const std::string& title)
{
  TH2I_LW* hist = bookEventNumbers(name, title, 32, 0., 32.);
  jemCrateModule(hist, 0, false);
  return hist;
}

// Book JEM module Vs crate

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMModuleVsCrate(
                             const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 16, 0., 16., 2, 0., 2.);
  numbers(hist, 0, 15);
  numbers(hist, 0, 1, 1, 0, false);
  hist->SetXTitle("Module");
  hist->SetYTitle("Crate");
  return hist;
}

// Book JEM eta

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookJEMEta(const std::string& name,
                                               const std::string& title)
{
  const int nxbins = 32;
  const double xbins[nxbins+1] = {-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,-1.6,
                                  -1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,0.0,0.2,
				  0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,2.0,2.2,2.4,
				  2.7,2.9,3.2,4.9};
  TH1F_LW* hist = book1F(name, title, nxbins, xbins);
  hist->SetXTitle("eta");
  return hist;
}

// Book JEM eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMEtaVsPhi(const std::string& name,
                                                    const std::string& title)
{
  TH2F_LW*  hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = book2F(name, title, 32, -3.2, 3.2, 32, 0., 32.);
    axis = hist->GetXaxis();
    for (int ch = -11; ch < 12; ch+=2) {
      int chan = ch;
      if (chan >= -1) ++chan;
      const double eta = chan/5. + 0.1;
      axis->SetBinLabel(chan+17, intDoubleString(chan, eta).c_str());
    }
    axis->SetBinLabel(2, "-15/-3.05");
    axis->SetBinLabel(4, "-13/-2.55");
    axis->SetBinLabel(29, "12/2.55");
    axis->SetBinLabel(31, "14/3.05");
  } else {
    const int nxbins = 32;
    const double xbins[nxbins+1] = {-4.9,-3.2,-2.9,-2.7,-2.4,-2.2,-2.0,-1.8,
                                    -1.6,-1.4,-1.2,-1.0,-0.8,-0.6,-0.4,-0.2,
				    0.0,0.2,0.4,0.6,0.8,1.0,1.2,1.4,1.6,1.8,
				    2.0,2.2,2.4,2.7,2.9,3.2,4.9};
    hist = book2F(name, title, nxbins, xbins, 32, 0., 32.);
    hist->SetXTitle("eta");
  }
  axis = hist->GetYaxis();
  const double phiBin     = M_PI/16.;
  const double halfPhiBin = M_PI/32.;
  for (int chan = 0; chan < 32; chan += 2 ) {
    const double rad = chan*phiBin + halfPhiBin;
    axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
  }
  if (m_shrinkEtaBins) axis->SetBinLabel(32, "etaVphi");
  else                 axis->SetBinLabel(32, "phi");
  return hist;
}

// Book JEM RoI eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMRoIEtaVsPhi(const std::string& name,
                                                       const std::string& title)
{
  TH2F_LW*  hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = book2F(name, title, 32, -3.2, 3.2, 32, 0., 32.);
    axis = hist->GetXaxis();
    for (int chan = -10; chan < 12; chan+=2) {
      const double eta = chan/5.;
      axis->SetBinLabel(chan+16, intDoubleString(chan, eta).c_str());
    }
    axis->SetBinLabel(2, "-14/-2.95");
    axis->SetBinLabel(4, "-12/-2.45");
    axis->SetBinLabel(28, "12/2.45");
    axis->SetBinLabel(30, "14/2.95");
    axis->SetBinLabel(32, "16/4.05");
  } else {
    const int nxbins = 32;
    const double xbins[nxbins+1] = {-4.0,-3.05,-2.8,-2.55,-2.3,-2.1,-1.9,-1.7,
                                    -1.5,-1.3,-1.1,-0.9,-0.7,-0.5,-0.3,-0.1,
  				    0.1,0.3,0.5,0.7,0.9,1.1,1.3,1.5,1.7,1.9,
				    2.1,2.3,2.55,2.8,3.05,4.0,4.95};
    hist = book2F(name, title, nxbins, xbins, 32, 0., 32.);
    hist->SetXTitle("eta");
  }
  axis = hist->GetYaxis();
  const double phiBin = M_PI/16.;
  for (int chan = 0; chan < 32; chan += 2 ) {
    const double rad = (chan + 1)*phiBin;
    axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
  }
  if (m_shrinkEtaBins) axis->SetBinLabel(32, "etaVphi");
  else                 axis->SetBinLabel(32, "phi");
  return hist;
}

// Book JEM energy with bins matching QuadLinear encoding

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookJEMQuadLinear(const std::string& name,
                                                      const std::string& title,
     						      int scale)
{
  if (scale < 1) scale = 1;
  const int eRange = 256;        //range of encoded value
  const int dRange = 4096*scale; //range of decoded value
  std::set<int> sorted;
  for (int i = 0; i < eRange; ++i) {
    const int val = LVL1::QuadLinear::Expand(i);
    if (val != 0) sorted.insert(val);
  }
  double binedges[eRange+2];
  int nbins = (scale > 1) ? 1 : 0;
  std::set<int>::const_iterator iter  = sorted.begin();
  std::set<int>::const_iterator iterE = sorted.end();
  for (; iter != iterE; ++iter) {
    binedges[nbins] = (*iter)*scale;
    ++nbins;
  }
  binedges[0] = 1;
  //if (scale > 1) binedges[1] = scale;
  binedges[nbins] = dRange;
  TH1F_LW* hist = book1F(name, title, nbins, binedges);
  return hist;
}

// Book JEM main jet thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookMainJetThresholds(
                            const std::string& name, const std::string& title)
{
  const int nbins = TrigConf::L1DataDef::max_JET_Threshold_Number();
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  mainJetThresholds(hist);
  return hist;
}

// Book JEM backward jet thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookBackwardJetThresholds(
                            const std::string& name, const std::string& title)
{
  const int nbins = TrigConf::L1DataDef::max_JB_Threshold_Number();
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  backwardJetThresholds(hist);
  return hist;
}

// Book JEM forward jet thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookForwardJetThresholds(
                            const std::string& name, const std::string& title)
{
  const int nbins = TrigConf::L1DataDef::max_JF_Threshold_Number();
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  forwardJetThresholds(hist);
  return hist;
}

// Book JEM JetEt thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookJetEtThresholds(
                            const std::string& name, const std::string& title)
{
  const int nbins = TrigConf::L1DataDef::max_JE_Threshold_Number();
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  jetEtThresholds(hist);
  return hist;
}

// Book JEM MissingEt thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookMissingEtThresholds(
                            const std::string& name, const std::string& title)
{
  const int nbins = TrigConf::L1DataDef::max_XE_Threshold_Number();
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  missingEtThresholds(hist);
  return hist;
}

// Book JEM SumEt thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookSumEtThresholds(
                            const std::string& name, const std::string& title)
{
  //const int nbins = TrigConf::L1DataDef::max_TE_Threshold_Number();
  const int nbins = 8;
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  sumEtThresholds(hist);
  return hist;
}

// Book JEM MissingEtSig thresholds

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookMissingEtSigThresholds(
                            const std::string& name, const std::string& title)
{
  //const int nbins = TrigConf::L1DataDef::max_XS_Threshold_Number();
  const int nbins = 8;
  TH1F_LW* hist = book1F(name, title, nbins, 0, nbins);
  missingEtSigThresholds(hist);
  return hist;
}

// Book JEM sub-status errors vs crate/module

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMSubStatusVsCrateModule(
                            const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., 32, 0., 32.);
  subStatus(hist);
  jemCrateModule(hist, 0, false);
  return hist;
}

// Book JEM sub-status errors vs crate

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookJEMSubStatusVsCrate(
                            const std::string& name, const std::string& title)
{
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., 2, 0., 2.);
  subStatus(hist);
  numbers(hist, 0, 1, 1, 0, false);
  hist->SetYTitle("Crate");
  return hist;
}

//===========================================================================
//  Booking Utilities - PPM
//===========================================================================

// Book PPM Em eta

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookPPMEmEta(const std::string name,
                                                 const std::string title)
{
  const int nxbins = 66;
  const double xbins[nxbins+1] = {-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                                  -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
      		                  -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
				  -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
				  -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
				  0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
				  1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
				  3.1,3.2,3.625,4.050,4.475,4.9};
  TH1F_LW* hist = book1F(name, title, nxbins, xbins);
  hist->SetXTitle("eta");
  return hist;
}

// Book PPM Had eta

TH1F_LW* TrigT1CaloLWHistogramToolV1::bookPPMHadEta(const std::string name,
                                                  const std::string title)
{
  const int nxbins = 62;
  const double xbins[nxbins+1] = {-4.9,-4.050,-3.2,-3.1,-2.9,
                                  -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
      		                  -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
				  -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
				  -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
				  0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
				  1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
				  3.1,3.2,4.050,4.9};
  TH1F_LW* hist = book1F(name, title, nxbins, xbins);
  hist->SetXTitle("eta");
  return hist;
}
 
// Book PPM Em eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookPPMEmEtaVsPhi(const std::string name,
                                                      const std::string title)
{
  TH2F_LW* hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = book2F(name, title, 66, -3.3, 3.3, 64, 0., 64.);
    axis = hist->GetXaxis();
    for (int ch = -25; ch < 25; ch+=4) {
      int chan = ch;
      if (chan >= -1) ++chan;
      const double eta = (chan/10.)+0.05;
      axis->SetBinLabel(chan+34, intDoubleString(chan, eta).c_str());
    }
    axis->SetBinLabel(1, "-49/-4.41");
    axis->SetBinLabel(5, "-32/-3.15");
    axis->SetBinLabel(62, "31/3.15");
    axis->SetBinLabel(66, "44/4.41");
  } else {
    const int nxbins = 66;
    const double xbins[nxbins+1] = {-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                                    -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
  				    -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
				    -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
				    -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
				    0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
				    1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
				    3.1,3.2,3.625,4.050,4.475,4.9};
    hist = book2F(name, title, nxbins, xbins, 64, 0., 64.);
    hist->SetXTitle("eta");
  }

  axis = hist->GetYaxis();
  const double phiBin     = M_PI/32.;
  const double halfPhiBin = M_PI/64.;
  for (int chan = 0; chan < 64; chan += 4 ) {
    const double rad = chan*phiBin + halfPhiBin;
    axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
  }
  if (m_shrinkEtaBins) axis->SetBinLabel(64, "etaVphi");
  else                 axis->SetBinLabel(64, "phi");
  return hist;

}

// Book PPM Had eta vs phi

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookPPMHadEtaVsPhi(const std::string name,
                                                       const std::string title)
{
  TH2F_LW* hist = bookPPMEmEtaVsPhi(name, title);
  if (m_shrinkEtaBins) {
    LWHist::LWHistAxis* axis = hist->GetXaxis();
    axis->SetBinLabel(1, "-49/-4.17");
    axis->SetBinLabel(66, "44/4.19");
  }
  return hist;

}

// Book PPM Em eta vs phi profile

TProfile2D_LW* TrigT1CaloLWHistogramToolV1::bookProfilePPMEmEtaVsPhi(
                              const std::string name, const std::string title)
{
  // todo - remove duplication with above
  TProfile2D_LW* hist = 0;
  LWHist::LWHistAxis* axis = 0;
  if (m_shrinkEtaBins) {
    hist = bookProfile2D(name, title, 66, -3.3, 3.3, 64, 0., 64.);
    axis = hist->GetXaxis();
    for (int ch = -25; ch < 25; ch+=4) {
      int chan = ch;
      if (chan >= -1) ++chan;
      const double eta = (chan/10.)+0.05;
      axis->SetBinLabel(chan+34, intDoubleString(chan, eta).c_str());
    }
    axis->SetBinLabel(1, "-49/-4.41");
    axis->SetBinLabel(5, "-32/-3.15");
    axis->SetBinLabel(62, "31/3.15");
    axis->SetBinLabel(66, "44/4.41");
  } else {
    const int nxbins = 66;
    const double xbins[nxbins+1] = {-4.9,-4.475,-4.050,-3.625,-3.2,-3.1,-2.9,
                                    -2.7,-2.5,-2.4,-2.3,-2.2,-2.1,-2.0,-1.9,
  				    -1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,
				    -1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,
				    -0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,
				    0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,
				    1.8,1.9,2.0,2.1,2.2,2.3,2.4,2.5,2.7,2.9,
				    3.1,3.2,3.625,4.050,4.475,4.9};
    hist = bookProfile2D(name, title, nxbins, xbins, 64, 0., 64.);
    hist->SetXTitle("eta");
  }

  axis = hist->GetYaxis();
  const double phiBin     = M_PI/32.;
  const double halfPhiBin = M_PI/64.;
  for (int chan = 0; chan < 64; chan += 4 ) {
    const double rad = chan*phiBin + halfPhiBin;
    axis->SetBinLabel(chan+1, intDoubleString(chan, rad).c_str());
  }
  if (m_shrinkEtaBins) axis->SetBinLabel(64, "etaVphi");
  else                 axis->SetBinLabel(64, "phi");
  return hist;

}

// Book PPM Had eta vs phi profile

TProfile2D_LW* TrigT1CaloLWHistogramToolV1::bookProfilePPMHadEtaVsPhi(
                              const std::string name, const std::string title)
{
  TProfile2D_LW* hist = bookProfilePPMEmEtaVsPhi(name, title);
  if (m_shrinkEtaBins) {
    LWHist::LWHistAxis* axis = hist->GetXaxis();
    axis->SetBinLabel(1, "-49/-4.17");
    axis->SetBinLabel(66, "44/4.19");
  }
  return hist;

}

// Book PPM events with error/mismatch vs crate/module

TH2I_LW* TrigT1CaloLWHistogramToolV1::bookPPMEventVsCrateModule(
                     const std::string& name, const std::string& title,
		     int firstCrate, int lastCrate)
{
  const int nbins = (lastCrate-firstCrate+1)*16;
  TH2I_LW* hist = bookEventNumbers(name, title, nbins, 0, nbins);
  ppmCrateModule(hist, firstCrate, lastCrate, 0, false);
  return hist;
}

// Book PPM Crate/Module vs Submodule/Channel

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookPPMCrateModuleVsSubmoduleChannel(
                     const std::string& name, const std::string& title,
                     int firstCrate, int lastCrate)
{
  const int nbins = (lastCrate-firstCrate+1)*16;
  TH2F_LW* hist = book2F(name, title, nbins, 0., nbins, 64, 0., 64.);
  ppmCrateModule(hist, firstCrate, lastCrate);
  ppmSubmoduleChannel(hist, 0, false);
  return hist;
}

// Book PPM Crate/Module vs Submodule/Channel profile

TProfile2D_LW*
  TrigT1CaloLWHistogramToolV1::bookProfilePPMCrateModuleVsSubmoduleChannel(
                     const std::string& name, const std::string& title,
                     int firstCrate, int lastCrate)
{
  const int nbins = (lastCrate-firstCrate+1)*16;
  TProfile2D_LW* hist = bookProfile2D(name, title, nbins, 0., nbins,
                                                              64, 0., 64.);
  ppmCrateModule(hist, firstCrate, lastCrate);
  ppmSubmoduleChannel(hist, 0, false);
  return hist;
}

// Book PPM SubStatus vs crate/module

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookPPMSubStatusVsCrateModule(
                     const std::string& name, const std::string& title,
		     int firstCrate, int lastCrate)
{
  const int nbins = (lastCrate-firstCrate+1)*16;
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., nbins, 0., nbins);
  subStatus(hist);
  ppmCrateModule(hist, firstCrate, lastCrate, 0, false);
  return hist;
}

// Book PPM ASIC errors vs crate/module

TH2F_LW* TrigT1CaloLWHistogramToolV1::bookPPMErrorsVsCrateModule(
                     const std::string& name, const std::string& title,
		     int firstCrate, int lastCrate)
{
  const int nbins = (lastCrate-firstCrate+1)*16;
  TH2F_LW* hist = book2F(name, title, 8, 0., 8., nbins, 0., nbins);
  ppmErrors(hist);
  ppmCrateModule(hist, firstCrate, lastCrate, 0, false);
  return hist;
}

//===========================================================================
//  Filling Utilities - General
//===========================================================================

// Get Minimum and Maximun bins and contents of 2D histogram

bool TrigT1CaloLWHistogramToolV1::getMinMaxBin(LWHist2D* hist,
                       int& minBinx, int& minBiny, int& maxBinx, int& maxBiny,
		       double& minValue, double& maxValue)
{
  //NB. this ignores bins with content & error = 0
  minBinx = 1;
  minBiny = 1;
  maxBinx = 1;
  maxBiny = 1;
  minValue = 0.;
  maxValue = 0.;
  bool found = false;
  unsigned int ix = 0;
  unsigned int iy = 0;
  double content = 0.;
  double error   = 0.;
  hist->resetActiveBinLoop();
  while(hist->getNextActiveBin(ix, iy, content, error)) {
    if (!found) {
      minBinx = ix;
      minBiny = iy;
      maxBinx = ix;
      maxBiny = iy;
      minValue = content;
      maxValue = content;
      found = true;
    } else {
      if (content < minValue) {
        minValue = content;
	minBinx = ix;
	minBiny = iy;
      } else if (content > maxValue) {
        maxValue = content;
	maxBinx = ix;
	maxBiny = iy;
      }
    }
  }
  // return false if no active bins found
  return found;
}

// Get Minimum and Maximun bins and contents of 2D Profile histogram

bool TrigT1CaloLWHistogramToolV1::getMinMaxBin(TProfile2D_LW* hist,
                       int& minBinx, int& minBiny, int& maxBinx, int& maxBiny,
		       double& minValue, double& maxValue)
{
  //NB. this ignores bins with no entries
  minBinx = 1;
  minBiny = 1;
  maxBinx = 1;
  maxBiny = 1;
  minValue = 0.;
  maxValue = 0.;
  bool found = false;
  const int xbins = hist->GetNbinsX();
  const int ybins = hist->GetNbinsY();
  for (int ix = 1; ix <= xbins; ++ix) {
    for (int iy = 1; iy <= ybins; ++iy) {
      double entries = 0.;
      double content = 0.;
      double error   = 0.;
      hist->GetBinInfo(ix, iy, entries, content, error);
      if (entries > 0.) {
        if (!found) {
          minBinx = ix;
          minBiny = iy;
          maxBinx = ix;
          maxBiny = iy;
          minValue = content;
          maxValue = content;
          found = true;
        } else {
          if (content < minValue) {
            minValue = content;
	    minBinx = ix;
	    minBiny = iy;
          } else if (content > maxValue) {
            maxValue = content;
	    maxBinx = ix;
	    maxBiny = iy;
	  }
        }
      }
    }
  }
  // return false if no entries found
  return found;
}

// Fill Error/Mismatch event number

void TrigT1CaloLWHistogramToolV1::fillEventNumber(LWHist2D* hist, double y)
{
  int eventNumber = 0;
  const EventInfo* evInfo = 0;
  StatusCode sc = evtStore()->retrieve(evInfo);
  if (sc.isFailure()) {
    msg(MSG::DEBUG) << "No EventInfo found" << endmsg;
  } else {
    const EventID* evID = evInfo->event_ID();
    if (evID) eventNumber = evID->event_number();
  }
  if (eventNumber <= 0) return;
  const int biny  = hist->GetYaxis()->FindBin(y);
  const int nbins = hist->GetNbinsX();
  double entries = hist->GetEntries();
  for (int binx = 1; binx <= nbins; ++binx) {
    const int val = hist->GetBinContent(binx, biny);
    if (val == eventNumber) break;
    else if (val == 0) {
      hist->SetBinContent(binx, biny, eventNumber);
      entries++;
      break;
    } else if (eventNumber < val) {
      for (int b = nbins; b > binx; --b) {
        const int v1 = hist->GetBinContent(b-1, biny);
	if (v1 == 0) continue;
        const int v2 = hist->GetBinContent(b, biny);
	if (v2 == 0) entries++;
        hist->SetBinContent(b, biny, v1);
      }
      hist->SetBinContent(binx, biny, eventNumber);
      break;
    }
  }
  hist->SetEntries(entries);
}

// Fill weighted thresholds 1D

void TrigT1CaloLWHistogramToolV1::fillThresholds(LWHist1D* hist, int val,
                                          int nThresh, int nBits, int offset)
{
  if (val) {
    const int mask = (1 << nBits) - 1;
    for (int thr = 0; thr < nThresh; ++thr) {
      const int hit = (val >> (nBits*thr)) & mask;
      if (hit) hist->Fill(thr + offset, hit);
    }
  }
}

// Fill weighted thresholds 2D, X axis

void TrigT1CaloLWHistogramToolV1::fillThresholdsVsY(LWHist2D* hist, int val,
                                    int y, int nThresh, int nBits, int offset)
{
  if (val) {
    const int mask = (1 << nBits) - 1;
    for (int thr = 0; thr < nThresh; ++thr) {
      const int hit = (val >> (nBits*thr)) & mask;
      if (hit) hist->Fill(thr + offset, y, hit);
    }
  }
} 

// Fill weighted thresholds 2D, Y axis

void TrigT1CaloLWHistogramToolV1::fillXVsThresholds(LWHist2D* hist, int x,
                                  int val, int nThresh, int nBits, int offset)
{
  if (val) {
    const int mask = (1 << nBits) - 1;
    for (int thr = 0; thr < nThresh; ++thr) {
      const int hit = (val >> (nBits*thr)) & mask;
      if (hit) hist->Fill(x, thr + offset, hit);
    }
  }
}

// Replace the contents of one 2D histogram with those of another

void TrigT1CaloLWHistogramToolV1::replaceContents(LWHist2D* histTo,
                                                LWHist2D* histFrom)
{
  histTo->Reset();
  unsigned int ix = 0;
  unsigned int iy = 0;
  double content = 0.;
  double error   = 0.;
  histFrom->resetActiveBinLoop();
  while (histFrom->getNextActiveBin(ix, iy, content, error)) {
    histTo->SetBinContentAndError(ix, iy, content, error);
  }
  double sumW   = 0.;
  double sumW2  = 0.;
  double sumWX  = 0.;
  double sumWX2 = 0.;
  double sumWY  = 0.;
  double sumWY2 = 0.;
  double sumWXY = 0.;
  histFrom->getSums(sumW, sumW2, sumWX, sumWX2, sumWY, sumWY2, sumWXY);
  histTo->setSums(sumW, sumW2, sumWX, sumWX2, sumWY, sumWY2, sumWXY);
  histTo->SetEntries(histFrom->GetEntries());
}

// Replace the contents of one 2D Profile histogram with those of another

void TrigT1CaloLWHistogramToolV1::replaceContents(TProfile2D_LW* histTo,
                                                TProfile2D_LW* histFrom)
{
  histTo->Reset();
  const int xbins = histFrom->GetNbinsX();
  const int ybins = histFrom->GetNbinsY();
  for (int ix = 1; ix <= xbins; ++ix) {
    for (int iy = 1; iy <= ybins; ++iy) {
      double entries    = 0.;
      double contentOut = 0.;
      double errorOut   = 0.;
      histFrom->GetBinInfo(ix, iy, entries, contentOut, errorOut);
      if (entries > 0.) {
	const double contentIn = contentOut*entries;
	const double errorIn   = sqrt(errorOut*errorOut*entries*entries
	                                      + contentOut*contentOut*entries);
        histTo->SetBinInfo(ix, iy, entries, contentIn, errorIn);
      }
    }
  }
  double sumW   = 0.;
  double sumW2  = 0.;
  double sumWX  = 0.;
  double sumWX2 = 0.;
  double sumWY  = 0.;
  double sumWY2 = 0.;
  double sumWXY = 0.;
  double sumWZ  = 0.;
  double sumWZ2 = 0.;
  histFrom->getSums(sumW, sumW2, sumWX, sumWX2, sumWY, sumWY2, sumWXY,
                                                               sumWZ, sumWZ2);
  histTo->setSums(sumW, sumW2, sumWX, sumWX2, sumWY, sumWY2, sumWXY,
                                                             sumWZ, sumWZ2);
  histTo->SetEntries(histFrom->GetEntries());
}

//===========================================================================
//  Filling Utilities - CPM
//===========================================================================

// Fill CPM eta vs phi

void TrigT1CaloLWHistogramToolV1::fillCPMEtaVsPhi(LWHist2D* hist, double eta,
                                                double phi, double weight)
{
  const double phiMod = phi * m_phiScaleTT;
  hist->Fill(eta, phiMod, weight);
}

// Fill CPM RoI eta vs phi

void TrigT1CaloLWHistogramToolV1::fillCPMRoIEtaVsPhi(LWHist2D* hist, double eta,
                                                   double phi, double weight)
{
  const double phiMod = phi * m_phiScaleTT - 0.5;
  double etaMod = eta;
  if (m_shrinkEtaBins) etaMod -= 0.05;
  hist->Fill(etaMod, phiMod, weight);
}

//===========================================================================
//  Filling Utilities - JEM
//===========================================================================

// Fill JEM eta vs phi

void TrigT1CaloLWHistogramToolV1::fillJEMEtaVsPhi(LWHist2D* hist, double eta,
                                                double phi, double weight)
{
  double phiMod = phi * m_phiScaleJE;
  double etaMod = eta;
  const double absEta = fabs(eta);
  if (m_shrinkEtaBins && absEta > 2.4) {
    int offset = 1;
    if      (absEta > 3.2) offset = 4;
    else if (absEta > 2.9) offset = 3;
    else if (absEta > 2.7) offset = 2;
    etaMod = 2.3 + 0.2*offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  if (eta < -3.2 || eta > 3.2) {
    // Fill two bins for FCAL
    phiMod = floor(phiMod/2)*2. + 1.;
    hist->Fill(etaMod, phiMod + 0.5, weight);
    hist->Fill(etaMod, phiMod - 0.5, weight);
  } else hist->Fill(etaMod, phiMod, weight);
}

// Fill JEM RoI eta vs phi

void TrigT1CaloLWHistogramToolV1::fillJEMRoIEtaVsPhi(LWHist2D* hist, double eta,
                                                   double phi, double weight)
{
  const double phiMod = phi * m_phiScaleJE - 0.5;
  double etaMod = eta;
  const double absEta = fabs(eta);
  if (m_shrinkEtaBins && absEta > 2.3) {
    int offset = 1;
    if      (absEta > 4.0)  offset = 5;
    else if (absEta > 3.05) offset = 4;
    else if (absEta > 2.8)  offset = 3;
    else if (absEta > 2.55) offset = 2;
    etaMod = 2.2 + 0.2*offset;
    if (eta < 0.) etaMod = -etaMod;
  }
  const double etaShift = (m_shrinkEtaBins) ? 0.1 : 0.;
  // JEPRoIDecoder returns eta=3.9 for both of the two forwardmost bins
  if (eta > 3.8 && eta < 4.0) {
    const double eta2 = (m_shrinkEtaBins) ? 3.2 : 4.05;
    hist->Fill(eta2 - etaShift, phiMod, weight);
  }
  hist->Fill(etaMod - etaShift, phiMod, weight);
}

// Fill JEM phi allowing for granularity varying with eta

void TrigT1CaloLWHistogramToolV1::fillJEMPhi(LWHist1D* hist,
                                       double eta, double phi, double weight)
{
  const double halfBin = 1./(2.*m_phiScaleJE);
  if (eta < -3.2 || eta > 3.2) {
    // Fill two bins for FCAL
    hist->Fill(phi + halfBin, weight);
    hist->Fill(phi - halfBin, weight);
  } else hist->Fill(phi, weight);
}

//===========================================================================
//  Filling Utilities - PPM
//===========================================================================

// Fill PPM Em eta vs phi

void TrigT1CaloLWHistogramToolV1::fillPPMEmEtaVsPhi(LWHist2D* hist, double eta,
                                                  double phi, double weight)
{
  double phiMod = phi * m_phiScaleTT;
  double etaMod = eta;
  const double absEta = fabs(eta);
  if (absEta > 3.2) {
    if (m_shrinkEtaBins) {
      etaMod = 2.9 + 0.1*(absEta-3.2)/0.425;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill four bins in phi
    phiMod = floor(phiMod/4)*4. + 2.;
    hist->Fill(etaMod, phiMod + 1.5, weight);
    hist->Fill(etaMod, phiMod + 0.5, weight);
    hist->Fill(etaMod, phiMod - 0.5, weight);
    hist->Fill(etaMod, phiMod - 1.5, weight);
  } else if (absEta > 2.5) {
    if (m_shrinkEtaBins) {
      etaMod = (absEta > 3.1) ? 2.85 : 2.5 + (absEta-2.5)/2.;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill two bins in phi
    phiMod = floor(phiMod/2)*2. + 1.;
    hist->Fill(etaMod, phiMod + 0.5, weight);
    hist->Fill(etaMod, phiMod - 0.5, weight);
  } else hist->Fill(eta, phiMod, weight);
}

// Fill PPM Had eta vs phi

void TrigT1CaloLWHistogramToolV1::fillPPMHadEtaVsPhi(LWHist2D* hist, double eta,
                                                   double phi, double weight)
{
  // Use EM mapping - puts FCAL2 in left half of bin and FCAL3 in right half
  fillPPMEmEtaVsPhi(hist, eta, phi, weight);
}

// Fill PPM Em eta vs phi profile

void TrigT1CaloLWHistogramToolV1::fillPPMEmEtaVsPhi(TProfile2D_LW* hist,
                              double eta, double phi, double z, double weight)
{
  double phiMod = phi * m_phiScaleTT;
  double etaMod = eta;
  const double absEta = fabs(eta);
  if (absEta > 3.2) {
    if (m_shrinkEtaBins) {
      etaMod = 2.9 + 0.1*(absEta-3.2)/0.425;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill four bins in phi
    phiMod = floor(phiMod/4)*4. + 2.;
    hist->Fill(etaMod, phiMod + 1.5, z, weight);
    hist->Fill(etaMod, phiMod + 0.5, z, weight);
    hist->Fill(etaMod, phiMod - 0.5, z, weight);
    hist->Fill(etaMod, phiMod - 1.5, z, weight);
  } else if (absEta > 2.5) {
    if (m_shrinkEtaBins) {
      etaMod = (absEta > 3.1) ? 2.85 : 2.5 + (absEta-2.5)/2.;
      if (eta < 0.) etaMod = -etaMod;
    }
    // Fill two bins in phi
    phiMod = floor(phiMod/2)*2. + 1.;
    hist->Fill(etaMod, phiMod + 0.5, z, weight);
    hist->Fill(etaMod, phiMod - 0.5, z, weight);
  } else hist->Fill(eta, phiMod, z, weight);
}

// Fill PPM Had eta vs phi profile

void TrigT1CaloLWHistogramToolV1::fillPPMHadEtaVsPhi(TProfile2D_LW* hist,
                              double eta, double phi, double z, double weight)
{
  // Use EM mapping - puts FCAL2 in left half of bin and FCAL3 in right half
  fillPPMEmEtaVsPhi(hist, eta, phi, z, weight);
}

// Fill PPM phi allowing for granularity varying with eta

void TrigT1CaloLWHistogramToolV1::fillPPMPhi(LWHist1D* hist,
                                       double eta, double phi, double weight)
{
  const double halfBin = 1./(2.*m_phiScaleTT);
  const double absEta  = fabs(eta);
  if (absEta > 3.2) {
    // Fill four bins in phi
    hist->Fill(phi + 3.*halfBin, weight);
    hist->Fill(phi + halfBin,    weight);
    hist->Fill(phi - halfBin,    weight);
    hist->Fill(phi - 3.*halfBin, weight);
  } else if (absEta > 2.5) {
    // Fill two bins in phi
    hist->Fill(phi + halfBin, weight);
    hist->Fill(phi - halfBin, weight);
  } else hist->Fill(phi, weight);
}

// Fill PPM phi profile allowing for granularity varying with eta

void TrigT1CaloLWHistogramToolV1::fillPPMPhi(TProfile_LW* hist,
                             double eta, double phi, double z, double weight)
{
  const double halfBin = 1./(2.*m_phiScaleTT);
  const double absEta  = fabs(eta);
  if (absEta > 3.2) {
    // Fill four bins in phi
    hist->Fill(phi + 3.*halfBin, z, weight);
    hist->Fill(phi + halfBin,    z, weight);
    hist->Fill(phi - halfBin,    z, weight);
    hist->Fill(phi - 3.*halfBin, z, weight);
  } else if (absEta > 2.5) {
    // Fill two bins in phi
    hist->Fill(phi + halfBin, z, weight);
    hist->Fill(phi - halfBin, z, weight);
  } else hist->Fill(phi, z, weight);
}

// Find bin in Em eta vs phi

void TrigT1CaloLWHistogramToolV1::findBinPPMEmEtaVsPhi(LWHist* hist,
                                double eta, double phi, int& binx, int& biny)
{
  double phiMod = phi * m_phiScaleTT;
  if (eta < -2.5 || eta > 2.5) phiMod += 0.5;
  double etaMod = eta;
  if (m_shrinkEtaBins) {
    const double absEta = fabs(eta);
    if (absEta > 3.2) {
      etaMod = 2.9 + 0.1*(absEta-3.2)/0.425;
      if (eta < 0.) etaMod = -etaMod;
    } else if (absEta > 2.5) {
      etaMod = (absEta > 3.1) ? 2.85 : 2.5 + (absEta-2.5)/2.;
      if (eta < 0.) etaMod = -etaMod;
    }
  }
  binx = hist->GetXaxis()->FindBin(etaMod);
  biny = hist->GetYaxis()->FindBin(phiMod);
}

// Find bin in Had eta vs phi

void TrigT1CaloLWHistogramToolV1::findBinPPMHadEtaVsPhi(LWHist* hist,
                                double eta, double phi, int& binx, int& biny)
{
  findBinPPMEmEtaVsPhi(hist, eta, phi, binx, biny);
}

// Set bin content and optionally error in Em eta vs phi

void TrigT1CaloLWHistogramToolV1::setBinPPMEmEtaVsPhi(LWHist2D* hist,
                             int binx, int biny, double content, double error)
{
  int nbin = 1;
  if      (binx <= 4 || binx >= 63) nbin = 4; // |eta|>3.2
  else if (binx <= 8 || binx >= 59) nbin = 2; // 2.5<|eta|<3.2
  const int binyBase = ((biny-1)/nbin)*nbin+1;
  for (int i = 0; i < nbin; ++i) {
    if (error >= 0.) {
      hist->SetBinContentAndError(binx, binyBase+i, content, error);
    } else hist->SetBinContent(binx, binyBase+i, content);
  }
}

// Set bin content and optionally error in Had eta vs phi

void TrigT1CaloLWHistogramToolV1::setBinPPMHadEtaVsPhi(LWHist2D* hist,
                             int binx, int biny, double content, double error)
{
  setBinPPMEmEtaVsPhi(hist, binx, biny, content, error);
}

//===========================================================================
//  Merge Utilities
//===========================================================================

// Scale histogram quantities by number of events for weightedAverage merge
// Used for eg. DB status plots which are just filled on the first event
// Call from procHistograms.

// Obsolete - superceded by lowerLB merge method.

void TrigT1CaloLWHistogramToolV1::scaleForMerge(LWHist2D* hist, int nevents)
{
  unsigned int entries = hist->GetEntries();
  if (entries > 0 && nevents > 0) {
    unsigned int ix = 0;
    unsigned int iy = 0;
    double content = 0.;
    double error = 0.;
    hist->resetActiveBinLoop();
    while (hist->getNextActiveBin(ix, iy, content, error)) {
      hist->SetBinError(ix, iy, 1./sqrt(nevents));
    }
  } else hist->SetBinError(1, 1, 0.); // merge always creates sumw2 array
  hist->setSums(0., 0., 0., 0., 0., 0., 0.); // to force recomputation
  hist->SetEntries(nevents*entries);
}

// Compute efficiencies and errors for perBinEffPerCent merge method
// Call from procHistograms.

void TrigT1CaloLWHistogramToolV1::efficienciesForMerge(LWHist* lw1, LWHist* lw2,
		                                     LWHist* lw3) {
  TH1* hist1 = lw1->getROOTHistBase(); // denominator
  TH1* hist2 = lw2->getROOTHistBase(); // numerator
  TH1* hist3 = lw3->getROOTHistBase(); // % efficiency
  hist3->Sumw2();
  const double OneSigOneSided = 0.159;
  int nbins = hist3->GetNbinsX() + 2;
  if (hist3->GetDimension() == 2) nbins *= (hist3->GetNbinsY() + 2);
  for (int bin = 0; bin < nbins; ++bin) {
    double denom = hist1->GetBinContent(bin);
    if (denom == 0.) continue;
    float eff = hist2->GetBinContent(bin)/denom;
    float err = 0.;
    if (eff == 0. || eff > 0.99) {
      err = 1. - std::pow(OneSigOneSided, 1. / denom);
    } else {
      err = std::sqrt(eff * (1. - eff) / denom);
    }
    hist3->SetBinContent(bin, eff*100.);
    hist3->SetBinError(bin, err*100.);
  }
  hist3->SetEntries(hist1->GetEntries());
  if (hist3->GetDimension() == 1) {
    hist3->SetMinimum(0.);
    hist3->SetMaximum(110.);
  }
}
// ============================================================================
} // end namespace
// ============================================================================
