/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: RecEmTauRoI.cxx 796120 2017-02-08 03:20:47Z ssnyder $
/***************************************************************************
                        RecEmTauRoI.cxx  -  description
                           -------------------
  begin                : Mon Jan 22 2001
  email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/


// STL include(s):
#include <cmath>

// Trigger config includes:
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfData/L1Menu.h"

// Local include(s):
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"

using namespace std;
using namespace TrigConf;

/** The constructor doesn't do much - all the work is in the "construct" method
 */
LVL1::RecEmTauRoI::RecEmTauRoI( unsigned int RoIWord, 
                                const std::vector<TriggerThreshold *> *caloThresholds)
    : m_roiWord(RoIWord),
      m_decoder(new CPRoIDecoder()),
      m_type(m_decoder->roiType(m_roiWord)),
      m_coordRange(m_decoder->coordinate(m_roiWord))
{
  if (m_type == TrigT1CaloDefs::CpRoIWordType) {
    this->constructRun1(caloThresholds);
  }
  else if (m_type == TrigT1CaloDefs::EMRoIWordType ||
           m_type == TrigT1CaloDefs::TauRoIWordType)
  {
    this->constructRun2(caloThresholds);
  }
  else
  {
    m_thresholdMask = 0;
    m_triggerThresholdValue.clear();
    m_isolationMask.clear();
    m_emIsolation.clear();
    m_hadIsolation.clear();
    m_hadCoreIsolation.clear();
    m_thresholdType.clear();
  }
}

LVL1::RecEmTauRoI::RecEmTauRoI(unsigned int RoIWord,
                               const TrigConf::L1Menu *const l1menu) :
  m_roiWord(RoIWord),
  m_decoder(new CPRoIDecoder()),
  m_type(m_decoder->roiType(m_roiWord)),
  m_coordRange(m_decoder->coordinate(m_roiWord))
{
  constructRun3(l1menu);
}

// written copy constructor
LVL1::RecEmTauRoI::RecEmTauRoI(const RecEmTauRoI &obj) :
  m_roiWord{obj.m_roiWord},
  m_decoder{new CPRoIDecoder()},
  m_type{obj.m_type},
  m_coordRange{obj.m_coordRange},
  m_thresholdMask{obj.m_thresholdMask},
  m_triggerThresholdValue{obj.m_triggerThresholdValue},
  m_isolationMask{obj.m_isolationMask},
  m_emIsolation{obj.m_emIsolation},
  m_hadIsolation{obj.m_hadIsolation},
  m_hadCoreIsolation{obj.m_hadCoreIsolation},
  m_thresholdType{obj.m_thresholdType}
{}

// written assignment operator
LVL1::RecEmTauRoI &
LVL1::RecEmTauRoI::operator=(const RecEmTauRoI & rhs)
{
  if(this != &rhs) {
    m_roiWord = rhs.m_roiWord;
    delete m_decoder;
    m_decoder = new CPRoIDecoder();
    m_type = rhs.m_type;
    m_coordRange = rhs.m_coordRange;
    m_thresholdMask = rhs.m_thresholdMask;
    m_triggerThresholdValue = rhs.m_triggerThresholdValue;
    m_isolationMask = rhs.m_isolationMask;
    m_emIsolation = rhs.m_emIsolation;
    m_hadIsolation = rhs.m_hadIsolation;
    m_hadCoreIsolation = rhs.m_hadCoreIsolation;
    m_thresholdType = rhs.m_thresholdType;
  }
  return *this;
}

LVL1::RecEmTauRoI::~RecEmTauRoI() {
  delete m_decoder;
}

/** The "construct" call unpacks the RoIWord and copies information about the
 passed thresholds
  from the vector of TriggerThresholds into maps for later retrieval.

  One warning here: the methods of the RecRoI classes assume threshold
  numbering starts at 1,
  rather than 0 in the calorimeter trigger simulation and Lvl1ConfigSvc. Hence
  the odd "-1"
  in a couple of places in this class */

void LVL1::RecEmTauRoI::constructRun1(
    const std::vector<TriggerThreshold *> *caloThresholds)
{

  m_thresholdMask = m_roiWord & 0xffff;

  m_triggerThresholdValue.clear();
  m_isolationMask.clear();
  m_emIsolation.clear();
  m_hadIsolation.clear();
  m_hadCoreIsolation.clear();
  m_thresholdType.clear();

  const vector<unsigned int> passed_thresholds =
      m_decoder->thresholdsPassed(m_roiWord);

  // Copy the thresholds into a map - makes matching up the passed ones very easy
  std::map<int, TriggerThreshold *> thrMap;

  for (std::vector<TriggerThreshold *>::const_iterator it =
           caloThresholds->begin();
       it != caloThresholds->end(); ++it)
  {
    if ((*it)->type() == L1DataDef::emType() ||
        (*it)->type() == L1DataDef::tauType())
    {
      int num = (*it)->thresholdNumber();
      thrMap.insert(std::map<int, TriggerThreshold *>::value_type(num, (*it)));
    }
  }

  //
  // Extract the properties of the passed thresholds:
  //

  int ieta = int((m_coordRange.etaRange().min() + 0.025) / 0.1) +
             ((m_coordRange.etaRange().min() + 0.025 > 0) ? 0 : -1);
  int iphi = int((m_coordRange.phiRange().min() + 0.025) * 32 / M_PI);

  for (vector<unsigned int>::const_iterator itp = passed_thresholds.begin();
       itp != passed_thresholds.end(); ++itp)
  {
    std::map<int, TriggerThreshold *>::const_iterator thr =
        thrMap.find(*itp - 1);
    if (thr != thrMap.end())
    {
      TriggerThresholdValue *ttv =
          thr->second->triggerThresholdValue(ieta, iphi);
      ClusterThresholdValue *ctv = dynamic_cast<ClusterThresholdValue *>(ttv);
      if (ctv)
      {
        m_triggerThresholdValue.insert(std::map<int, unsigned int>::value_type(
            *itp, ctv->thresholdValueCount()));
        m_emIsolation.insert(std::map<int, unsigned int>::value_type(
            *itp, ctv->emIsolationCount()));
        m_hadIsolation.insert(std::map<int, unsigned int>::value_type(
            *itp, ctv->hadIsolationCount()));
        m_hadCoreIsolation.insert(
            std::map<int, unsigned int>::value_type(*itp, ctv->hadVetoCount()));
        TrigT1CaloDefs::ClusterAlgorithm type =
            ((ctv->type() == L1DataDef::emType()) ? TrigT1CaloDefs::EMAlg
                                                  : TrigT1CaloDefs::TauAlg);
        m_thresholdType.insert(
            std::map<int, TrigT1CaloDefs::ClusterAlgorithm>::value_type(*itp,
                                                                        type));
      }
    } // end "found threshold in map"
  }   // end loop through passed thresholds

  return;
}

/** Content of RoI word different in Run 2. To obtain thresholds passed need to
 * simulate
 *  thresholding.
 *  Hence have an alternate constructor here */
void LVL1::RecEmTauRoI::constructRun2(const std::vector<TriggerThreshold *> *caloThresholds)
{

  m_thresholdMask = 0;
  m_triggerThresholdValue.clear();
  m_isolationMask.clear();
  m_emIsolation.clear();
  m_hadIsolation.clear();
  m_hadCoreIsolation.clear();
  m_thresholdType.clear();

  std::string triggerType =
      (m_type == TrigT1CaloDefs::EMRoIWordType ? L1DataDef::emType()
                                                : L1DataDef::tauType());

  int ieta = int((m_coordRange.etaRange().min() + 0.025) / 0.1) +
              ((m_coordRange.etaRange().min() + 0.025 > 0) ? 0 : -1);
  int iphi = int((m_coordRange.phiRange().min() + 0.025) * 32 / M_PI);

  unsigned int isolWord = isolation();

  // Iterate through vector of thresholds and see which ones this RoI satisfies
  for (std::vector<TriggerThreshold *>::const_iterator it =
            caloThresholds->begin();
        it != caloThresholds->end(); ++it) {
    if ((*it)->type() == triggerType) {
      // Does it satisfy this one?
      // Start by extracting threshold values
      TriggerThresholdValue *ttv = (*it)->triggerThresholdValue(ieta, iphi);
      ClusterThresholdValue *ctv = dynamic_cast<ClusterThresholdValue *>(ttv);
      if (ctv) {
        float scale = ctv->caloInfo().globalEmScale();
        unsigned int etCut = ctv->ptcut();
        unsigned int threshold = etCut * scale;
        unsigned int isolMask = ctv->isolationMask();

        bool isolationPassed = true;
        for (unsigned int bit = 0; bit < TrigT1CaloDefs::numOfIsolationBits;
              ++bit)
          if ((isolMask & (1 << bit)) && !(isolWord & (1 << bit)))
            isolationPassed = false;

        if (et() > threshold && isolationPassed) {
          int num = (*it)->thresholdNumber();
          m_triggerThresholdValue.insert(
              std::map<int, unsigned int>::value_type(num, etCut));
          m_isolationMask.insert(
              std::map<int, unsigned int>::value_type(num, isolMask));
          TrigT1CaloDefs::ClusterAlgorithm type =
              ((ctv->type() == L1DataDef::emType()) ? TrigT1CaloDefs::EMAlg
                                                    : TrigT1CaloDefs::TauAlg);
          m_thresholdType.insert(
              std::map<int, TrigT1CaloDefs::ClusterAlgorithm>::value_type(
                  num, type));
          m_thresholdMask |= (1 << num);
        } // passes cuts
      }   // ClusterThresholdValue pointer valid
    }     // correct threshold type
  }       // Loop over thresholds
}

void LVL1::RecEmTauRoI::constructRun3(const TrigConf::L1Menu *const l1menu)
{
  m_thresholdMask = 0;
  m_triggerThresholdValue.clear();
  m_isolationMask.clear();
  m_emIsolation.clear();
  m_hadIsolation.clear();
  m_hadCoreIsolation.clear();
  m_thresholdType.clear();

  std::string triggerType = (m_type == TrigT1CaloDefs::EMRoIWordType ? "EM" : "TAU");

  // get the integer eta value
  // eta value of ROIs goes in steps of 0.1 and on the negative side 1 is subtracted to avoid overlap at 0
  // (copied from constructRun2)
  int ieta = int((m_coordRange.etaRange().min() + 0.025) / 0.1) +
              ((m_coordRange.etaRange().min() + 0.025 > 0) ? 0 : -1);

  unsigned int isolWord = isolation();

  // Iterate through the configured thresholds and see which ones this RoI satisfies
  for (shared_ptr<TrigConf::L1Threshold> thr : l1menu->thresholds(triggerType))
  {
    // Start by extracting threshold values
    auto caloThr = std::dynamic_pointer_cast<TrigConf::L1Threshold_Calo>(thr);
    if (!caloThr)
    {
      continue;
    }

    // test eT
    unsigned int etCut = std::round(caloThr->thrValue(ieta)); // threshold value in GeV (integer for legacy thr)
    unsigned int etCounts = caloThr->thrValueCounts(ieta);    // threshold counts (threshold value * emscale)
    bool eTPassed = (et() > etCounts);

    // test isolation
    unsigned int isolMask = 0;
    if (triggerType == "EM")
    {
      auto emThr = std::dynamic_pointer_cast<TrigConf::L1Threshold_EM>(thr);
      if (emThr)
      {
        isolMask = emThr->isolationMask(ieta);
      }
    }
    else if (triggerType == "TAU")
    {
      auto tauThr = std::dynamic_pointer_cast<TrigConf::L1Threshold_TAU>(thr);
      if (tauThr)
      {
        isolMask = tauThr->isolationMask();
      }
    }
    bool isolationPassed = true;
    for (unsigned int bit = 0; bit < TrigT1CaloDefs::numOfIsolationBits; ++bit)
    {
      if ((isolMask & (1 << bit)) && !(isolWord & (1 << bit)))
      {
        isolationPassed = false;
      }
    }

    if (eTPassed && isolationPassed)
    {
      int num = thr->mapping();
      m_triggerThresholdValue[num] = etCut;
      m_isolationMask[num] = isolMask;
      m_thresholdType[num] = (m_type == TrigT1CaloDefs::EMRoIWordType) ? TrigT1CaloDefs::EMAlg : TrigT1CaloDefs::TauAlg;
      m_thresholdMask |= (1 << num);
    } // passes cuts
  }   // Loop over thresholds
}

unsigned int LVL1::RecEmTauRoI::roiWord() const { return m_roiWord; }

/** returns phi coord of ROI */
double LVL1::RecEmTauRoI::phi() const
{

  double roiPhi = m_coordRange.phi();
  if (roiPhi > M_PI)
    roiPhi -= 2 * M_PI;
  return roiPhi;
}

/** returns eta coord of ROI */
double LVL1::RecEmTauRoI::eta() const { return m_coordRange.eta(); }

/** returns crate number of ROI */
unsigned int LVL1::RecEmTauRoI::crate() const { return m_decoder->crate(m_roiWord); }

/** returns module number of ROI */
unsigned int LVL1::RecEmTauRoI::module() const
{
  return m_decoder->module(m_roiWord);
}

/** returns FPGA number of ROI */
unsigned int LVL1::RecEmTauRoI::fpga() const { return m_decoder->chip(m_roiWord); }

/** returns local coordinate of ROI */
unsigned int LVL1::RecEmTauRoI::localcoord() const
{
  return m_decoder->localcoord(m_roiWord);
}

/** returns cluster ET of Run 2 RoI */
unsigned int LVL1::RecEmTauRoI::et() const { return m_decoder->et(m_roiWord); }

/** returns isolation word of Run 2 RoI */
unsigned int LVL1::RecEmTauRoI::isolation() const
{

  return m_decoder->isolationWord(m_roiWord);
}

/** returns bitmask of thresholds passed by RoI */
unsigned int LVL1::RecEmTauRoI::thresholdPattern() const { return m_thresholdMask; }

/** returns type of RoI */
LVL1::TrigT1CaloDefs::RoIType
LVL1::RecEmTauRoI::roiType() const { return m_type; }

/** returns TRUE if threshold number <em>threshold_number</em> has been passed
 * by this ROI. */
bool LVL1::RecEmTauRoI::passedThreshold(int threshold_number) const
{
  if (this->isValidThreshold(threshold_number)) {
    return ((1 << threshold_number) & m_thresholdMask) != 0;
  }
  return false;
}

/** returns the CoordinateRange. This is worked out from the RoIWord's hardware
 coords
  (i.e. crate number, CPM number etc.) by the RoIDecoder class.  */
LVL1::CoordinateRange LVL1::RecEmTauRoI::coord() const { return m_coordRange; }

/** returns a vector of thresholds passed. */
std::vector<unsigned int> *
LVL1::RecEmTauRoI::thresholdsPassed() const
{
  std::vector<unsigned int> *newVec = new std::vector<unsigned int>;

  unsigned int nThresh = TrigT1CaloDefs::numOfCPThresholds;
  if (m_type == TrigT1CaloDefs::EMRoIWordType)
    nThresh = TrigT1CaloDefs::numOfEMCMXThresholds;
  else if (m_type == TrigT1CaloDefs::TauRoIWordType)
    nThresh = TrigT1CaloDefs::numOfTauCMXThresholds;

  for (unsigned int iTh = 0; iTh <= nThresh; ++iTh) {
    std::map<int, unsigned int>::const_iterator it =
        m_triggerThresholdValue.find(iTh);
    if (it != m_triggerThresholdValue.end())
      newVec->push_back(iTh);
  }

  return newVec;
}

/** returns the value of the trigger threshold for the threshold passed.
  The trigger threshold is also sometimes called the cluster threshold.*/
unsigned int LVL1::RecEmTauRoI::triggerThreshold(const unsigned int thresh) const
{

  std::map<int, unsigned int>::const_iterator it =
      m_triggerThresholdValue.find(thresh);
  if (it != m_triggerThresholdValue.end())
  {
    return (it->second);
  }
  return TrigT1CaloDefs::Error;
}

/** returns the value of the isolation mask for the threshold passed. (Run 2
 * only) */
unsigned int LVL1::RecEmTauRoI::isolationMask(const unsigned int thresh) const
{

  if (m_type == TrigT1CaloDefs::CpRoIWordType)
    return 0;

  std::map<int, unsigned int>::const_iterator it = m_isolationMask.find(thresh);
  if (it != m_isolationMask.end())
  {
    return (it->second);
  }
  return TrigT1CaloDefs::Error;
}

/** returns the value of the em isolation ring for the threshold passed. (Run 1
 * only) */
unsigned int LVL1::RecEmTauRoI::emIsolation(const unsigned int thresh) const
{

  if (m_type != TrigT1CaloDefs::CpRoIWordType)
    return 0;

  std::map<int, unsigned int>::const_iterator it = m_emIsolation.find(thresh);
  if (it != m_emIsolation.end())
  {
    return (it->second);
  }
  return TrigT1CaloDefs::Error;
}

/** returns the value of the hadronic layer isolation ring for the threshold
 * passed. (Run 1 only) */
unsigned int LVL1::RecEmTauRoI::hadIsolation(const unsigned int thresh) const
{
  if (m_type != TrigT1CaloDefs::CpRoIWordType)
    return 0;

  map<int, unsigned int>::const_iterator it = m_hadIsolation.find(thresh);
  if (it != m_hadIsolation.end())
  {
    return (it->second);
  }
  return TrigT1CaloDefs::Error;
}

/** returns the value of the hadronic layer core isolation for the threshold
 * passed. (Run 1 only) */
unsigned int LVL1::RecEmTauRoI::hadCoreIsolation(const unsigned int thresh) const
{

  if (m_type != TrigT1CaloDefs::CpRoIWordType)
    return 0;

  map<int, unsigned int>::const_iterator it = m_hadCoreIsolation.find(thresh);
  if (it != m_hadCoreIsolation.end())
  {
    return (it->second);
  }
  return TrigT1CaloDefs::Error;
}

/**returns true if thresh is a valid threshold number */
bool LVL1::RecEmTauRoI::isValidThreshold(const unsigned int thresh) const
{
  if (m_type == TrigT1CaloDefs::CpRoIWordType)
  {
    return (thresh <= TrigT1CaloDefs::numOfCPThresholds);
  }
  else if (m_type == TrigT1CaloDefs::EMRoIWordType)
  {
    return (thresh <= TrigT1CaloDefs::numOfEMCMXThresholds);
  }
  else if (m_type == TrigT1CaloDefs::TauRoIWordType)
  {
    return (thresh <= TrigT1CaloDefs::numOfTauCMXThresholds);
  }
  return false;
}

/** returns the type of the threshold, which is either EMAlg or TauAlg.
  If there is a problem, ClustAlgError is returned */
LVL1::TrigT1CaloDefs::ClusterAlgorithm
LVL1::RecEmTauRoI::thresholdType(const unsigned int thresh) const
{

  map<int, TrigT1CaloDefs::ClusterAlgorithm>::const_iterator it =
      m_thresholdType.find(thresh);
  if (it != m_thresholdType.end())
  {
    return it->second;
  }
  return TrigT1CaloDefs::ClustAlgError;
}
