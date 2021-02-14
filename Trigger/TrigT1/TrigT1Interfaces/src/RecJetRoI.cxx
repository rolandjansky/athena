// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

/***************************************************************************
                         RecJetRoI.cxx  -  description
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
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfData/L1Menu.h"

// Local include(s):
#include "TrigT1Interfaces/RecJetRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

using namespace std;
using namespace TrigConf;

/** The constructor doesn't do much - all the work is in the "construct" methods */
LVL1::RecJetRoI::RecJetRoI( unsigned int RoIWord,
                            const std::vector<TriggerThreshold *> *caloThresholds) {

  m_roiWord = RoIWord;
  m_decoder = new JEPRoIDecoder();
  m_version = m_decoder->jetRoIVersion(m_roiWord);
  m_coordRange = m_decoder->coordinate(m_roiWord);

  if (m_version > 1)
    this->constructRun2(caloThresholds);
  else
    this->constructRun1(caloThresholds);
}

LVL1::RecJetRoI::RecJetRoI( unsigned int RoIWord,
                            const TrigConf::L1Menu * const l1menu) :
  m_roiWord(RoIWord),
  m_decoder(new JEPRoIDecoder()),
  m_version(m_decoder->jetRoIVersion(m_roiWord)),
  m_coordRange(m_decoder->coordinate(m_roiWord))
{
  constructRun3(l1menu);
}

// copy constructor
LVL1::RecJetRoI::RecJetRoI( const RecJetRoI &obj ) :
   m_roiWord(obj.m_roiWord),
   m_decoder(new JEPRoIDecoder()),
   m_version(obj.m_version),
   m_coordRange(obj.m_coordRange),
   m_triggerThresholdValue(obj.m_triggerThresholdValue),
   m_windowSize(obj.m_windowSize)
{}

// assignment operator
LVL1::RecJetRoI&
LVL1::RecJetRoI::operator=( const RecJetRoI & rhs ) {
   if(this != &rhs) {
      m_roiWord = rhs.m_roiWord;
      delete m_decoder;
      m_decoder = new JEPRoIDecoder();
      m_version = rhs.m_version;
      m_coordRange = rhs.m_coordRange;
      m_thresholdMask = rhs.m_thresholdMask;
      m_triggerThresholdValue = rhs.m_triggerThresholdValue;
      m_windowSize = rhs.m_windowSize;
   }
   return *this;
}

LVL1::RecJetRoI::~RecJetRoI() { delete m_decoder; }

/** The "construct" calls unpack the RoIWord and copy information about the
 *  passed thresholds from the vector of TriggerThresholds into maps for later retrieval.
 *
 *  One warning here: the methods of the RecRoI classes assume threshold numbering starts at 1,
 *  rather than 0 in the calorimeter trigger simulation and Lvl1ConfigSvc. Hence
 *  the odd "-1" in a couple of places in these classes
 **/

/** construct call for Run 1 Jet RoIs */
void LVL1::RecJetRoI::constructRun1(const std::vector<TriggerThreshold *> *caloThresholds)
{

   m_thresholdMask = m_roiWord & 0xfff;

   std::string jetTriggerType = L1DataDef::jetType();
   if (this->isForwardJet())
      jetTriggerType =
          ((this->eta() > 0) ? L1DataDef::jfType() : L1DataDef::jbType());

   //
   // Copy the thresholds into a map - makes matching up the passed ones very
   // easy
   //

   std::map<int, TriggerThreshold *> thrMap;
   for (std::vector<TriggerThreshold *>::const_iterator it =
            caloThresholds->begin();
        it != caloThresholds->end(); ++it)
   {
      if ((*it)->type() == jetTriggerType)
      {
         int num = (*it)->thresholdNumber();
         thrMap.insert(std::map<int, TriggerThreshold *>::value_type(num, (*it)));
      }
   }

   //
   // Extract the properties of the passed thresholds:
   //
   const vector<unsigned int> passed_thresholds =
       (this->isForwardJet() ? m_decoder->fwdThresholdsPassed(m_roiWord)
                             : m_decoder->thresholdsPassed(m_roiWord));

   int iphi = int((m_coordRange.phiRange().min() + 0.025) * 32 / M_PI);
   int ieta =
       int((this->eta() + ((this->eta() > 0.01) ? 0.025 : -0.025)) / 0.1) - 1;
   // Adjustment due to irregular geometries
   if (ieta > 24)
      ieta += 2;

   for (vector<unsigned int>::const_iterator itp = passed_thresholds.begin();
        itp != passed_thresholds.end(); ++itp)
   {
      std::map<int, TriggerThreshold *>::const_iterator thr =
          thrMap.find(*itp - 1);
      if (thr != thrMap.end())
      {
         TriggerThresholdValue *ttv =
             thr->second->triggerThresholdValue(ieta, iphi);
         JetThresholdValue *jtv = dynamic_cast<JetThresholdValue *>(ttv);
         if (jtv)
         {
            m_triggerThresholdValue.insert(std::map<int, unsigned int>::value_type(
                *itp, jtv->thresholdValueCount()));
            m_windowSize.insert(
                std::map<int, unsigned int>::value_type(*itp, jtv->window()));
         }
      } // end "found threshold in map"
   }    // end loop through passed thresholds

   return;
}

/** construct call for Run 2 Jet RoIs */
void 
LVL1::RecJetRoI::constructRun2(const std::vector<TriggerThreshold *> *caloThresholds)
{

  m_thresholdMask = 0;
  m_triggerThresholdValue.clear();
  m_windowSize.clear();

  int ieta =
      int((this->eta() + ((this->eta() > 0.01) ? 0.025 : -0.025)) / 0.1) - 1;
  // Adjustment due to irregular geometries
  if (ieta > 24)
    ieta += 2;
  int iphi = int((m_coordRange.phiRange().min() + 0.025) * 32 / M_PI);
  //
  // Iterate through vector of thresholds and see which ones this RoI satisfies
  //
  for (std::vector<TriggerThreshold *>::const_iterator it =
           caloThresholds->begin();
       it != caloThresholds->end(); ++it) {
    if ((*it)->type() == L1DataDef::jetType()) {
      // Does it satisfy this one?
      // Start by extracting threshold values
      TriggerThresholdValue *ttv = (*it)->triggerThresholdValue(ieta, iphi);
      JetThresholdValue *jtv = dynamic_cast<JetThresholdValue *>(ttv);
      if (jtv) {
        float scale = jtv->caloInfo().globalJetScale();
        unsigned int etCut = jtv->ptcut();
        unsigned int threshold = etCut * scale;
        JetWindowSize::Size window = jtv->windowSize();
        unsigned int roiET =
            (window == JetWindowSize::LARGE ? etLarge() : etSmall());
        if (roiET > threshold) {
          int num = (*it)->thresholdNumber();
          m_triggerThresholdValue.insert(
              std::map<int, unsigned int>::value_type(num, etCut));
          m_windowSize.insert(
              std::map<int, unsigned int>::value_type(num, window));
          m_thresholdMask |= (1 << num);
        } // passes cuts
      }   // JetThresholdValue pointer valid
    }     // correct threshold type
  }       // Loop over thresholds
}

void
LVL1::RecJetRoI::constructRun3(const TrigConf::L1Menu * const l1menu)
{
   int ieta = int((this->eta() + ((this->eta() > 0.01) ? 0.025 : -0.025)) / 0.1) - 1;
   // Adjustment due to irregular geometries
   if (ieta > 24)
   {
      ieta += 2;
   }

   // Iterate through vector of thresholds and see which ones this RoI satisfies
   for (shared_ptr<TrigConf::L1Threshold> thr : l1menu->thresholds("JET"))
   {
      // Does it satisfy this one?
      // Start by extracting threshold values
      auto jetThr = std::dynamic_pointer_cast<TrigConf::L1Threshold_JET>(thr);
      if (!jetThr)
      {
         continue;
      }

      // test eT
      unsigned int etCut = std::round(jetThr->thrValue(ieta)); // threshold value in GeV (integer for legacy thr)
      unsigned int etCounts = jetThr->thrValueCounts(ieta);    // threshold counts (threshold value * emscale)
      TrigConf::JetWindowSize::Size window = (jetThr->window() == 8) ? TrigConf::JetWindowSize::LARGE : TrigConf::JetWindowSize::SMALL;
      unsigned int roiET = (window == TrigConf::JetWindowSize::LARGE) ? etLarge() : etSmall();
      bool eTPassed = (roiET > etCounts);

      if (eTPassed)
      {
         int num = thr->mapping();
         m_triggerThresholdValue[num] = etCut;
         m_windowSize[num] = window;
         m_thresholdMask |= (1 << num);
      } // passes cuts
   }    // Loop over thresholds
}

unsigned int
LVL1::RecJetRoI::roiWord() const { return m_roiWord; }

/** returns phi coord of ROI */
double
LVL1::RecJetRoI::phi() const
{
  double roiPhi = m_coordRange.phi();
  if (roiPhi > M_PI)
    roiPhi -= 2 * M_PI;
  return roiPhi;
}

/** returns eta coord of ROI */
double
LVL1::RecJetRoI::eta() const { return m_coordRange.eta(); }

/** returns crate number of ROI */
unsigned int
LVL1::RecJetRoI::crate() const { return m_decoder->crate(m_roiWord); }

/** returns module number of ROI */
unsigned int 
LVL1::RecJetRoI::module() const { return m_decoder->module(m_roiWord); }

/** returns frame number of ROI */
unsigned int 
LVL1::RecJetRoI::frame() const {
  unsigned int row = m_decoder->row(m_roiWord);
  unsigned int col = m_decoder->column(m_roiWord);
  return ((col & 2) << 1) + (row >> 1);
}

/** returns local coordinate within frame */
unsigned int 
LVL1::RecJetRoI::localcoord() const {
  unsigned int row = m_decoder->row(m_roiWord);
  unsigned int col = m_decoder->column(m_roiWord);
  return (col & 1) + ((row & 1) << 1);
}

/** returns ET of ROI large cluster.
    For Run 1 will return highest threshold passed irrespective of size */
unsigned int 
LVL1::RecJetRoI::etLarge() const {
  if (m_version > 1) {
    return m_decoder->etLarge(m_roiWord);
  } else {
    float highest = 0;
    std::map<int, unsigned int>::const_iterator it =
        m_triggerThresholdValue.begin();
    for (; it != m_triggerThresholdValue.end(); ++it)
      if (it->second > highest)
        highest = it->second;
    return highest;
  }
}

/** returns ET of ROI small cluster.
    For Run 1 will return highest threshold passed irrespective of size */
unsigned int
LVL1::RecJetRoI::etSmall() const {
  if (m_version > 1) {
    return m_decoder->etSmall(m_roiWord);
  } else {
    float highest = 0;
    std::map<int, unsigned int>::const_iterator it =
        m_triggerThresholdValue.begin();
    for (; it != m_triggerThresholdValue.end(); ++it)
      if (it->second > highest)
        highest = it->second;
    return highest;
  }
}

/** returns mask of thresholds passed */
unsigned int
LVL1::RecJetRoI::thresholdPattern() const { return m_thresholdMask; }

/** returns TRUE if threshold number <em>threshold_number</em> has been pass
    ed by this ROI. */
bool
LVL1::RecJetRoI::passedThreshold(int thresholdNumber) const {

  bool value = false;

  std::map<int, unsigned int>::const_iterator it =
      m_triggerThresholdValue.find(thresholdNumber);
  if (it != m_triggerThresholdValue.end())
    value = true;

  return value;
}

/** returns TRUE if threshold number <em>threshold_number</em> has been pass
    ed by this ROI.
    Returns FALSE if used in Run 2, where forward jet is not defined */
bool
LVL1::RecJetRoI::passedFwdThreshold(int thresholdNumber) const {

  if ((m_version < 2) && this->isValidThreshold(thresholdNumber)) {
    return (((1 << (thresholdNumber + 7)) & m_roiWord) > 0);
  }
  return false;
}

/** returns the CoordinateRange. This is worked out from the RoIWord's hardware
 *  coords (i.e. crate number, CPM number etc.) by the RoIDecoder class.
 **/
LVL1::CoordinateRange
LVL1::RecJetRoI::coord() const { return m_coordRange; }

/** returns a vector of thresholds passed. */
std::vector<unsigned int>
LVL1::RecJetRoI::thresholdsPassed() const {

  std::vector<unsigned int> newVec;// = new std::vector<unsigned int>;

  unsigned int nThresh = TrigT1CaloDefs::numOfJetCMXThresholds;
  if (m_version == 1) {
    if (this->isForwardJet())
      nThresh = TrigT1CaloDefs::numOfFwdJetThresholds;
    else
      nThresh = TrigT1CaloDefs::numOfJetThresholds;
  }

  for (unsigned int iTh = 0; iTh <= nThresh; ++iTh) {
    std::map<int, unsigned int>::const_iterator it =
        m_triggerThresholdValue.find(iTh);
    if (it != m_triggerThresholdValue.end())
      newVec.push_back(iTh);
  }

  return newVec;
}


/** returns the value of the trigger threshold for the threshold passed.
 *  The trigger threshold is also sometimes called the cluster threshold.
 **/
unsigned int
LVL1::RecJetRoI::triggerThreshold(const unsigned int thresh) const {
  map<int, unsigned int>::const_iterator it =
      m_triggerThresholdValue.find(thresh);
  if (it != m_triggerThresholdValue.end()) {
    return it->second;
  }
  return TrigT1CaloDefs::Error;
}

/** returns the value of the jet cluster for the threshold passed.*/
unsigned int
LVL1::RecJetRoI::windowSize(const unsigned int thresh) const {
  map<int, unsigned int>::const_iterator it = m_windowSize.find(thresh);
  if (it != m_windowSize.end()) {
    return it->second;
  }
  return TrigT1CaloDefs::Error;
}

/** returns true if thresh is a valid threshold number */
bool
LVL1::RecJetRoI::isValidThreshold(const unsigned int thresh) const
{
  if (!this->isForwardJet()) {
    return (thresh <= TrigT1CaloDefs::numOfJetThresholds);
  } else {
    return (thresh <= TrigT1CaloDefs::numOfFwdJetThresholds);
  }
}

/** returns true if the RoI is a forward jet RoI. A forward jet is defined as
 *  one that passes ANY forward jet threshold. Always false in Run 2 + 3
 **/
bool
LVL1::RecJetRoI::isForwardJet() const
{
  if (m_version <= 1 && ((1 << 18) & m_roiWord))
    return true;
  return false;
}

