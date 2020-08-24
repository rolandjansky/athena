/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
///////////////////////////////////////////////////////////////////
// L1JetEtTools.cxx,  
///////////////////////////////////////////////////////////////////

#include "L1JetEtTools.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/CaloInfo.h"
#include "TrigConfL1Data/L1DataDef.h"
#include <cmath>

using namespace TrigConf;


namespace LVL1 {

//================ Constructor =================================================

L1JetEtTools::L1JetEtTools(const std::string& t,
			  const std::string& n,
			  const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", n)
{
  declareInterface<IL1JetEtTools>(this);

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

//================ Destructor =================================================

L1JetEtTools::~L1JetEtTools()
{
}


//================ Initialisation =================================================

StatusCode L1JetEtTools::initialize()
{
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode L1JetEtTools::finalize()
{
  return StatusCode::SUCCESS;
}


/** Compute JetEtSum value from vector of JetROI objects */

unsigned int L1JetEtTools::jetEtSum(const DataVector<JetROI>* rois)
{

  /** Fill multiplicity vectors */
  std::vector<unsigned int> multiplicities;
  fillMultiplicities(rois, multiplicities);
    
  /** Compute JetEt sum */
  const std::vector<int> weights = m_configSvc->thresholdConfig()->caloInfo().jetWeights();
  return jetEtSum(multiplicities, weights);
  
}


/** Compute JetEtSum value from a CMMHitsCollection */

unsigned int L1JetEtTools::jetEtSum(const CMMJetHitsCollection* sums)
{

  /** Fill multiplicity vectors */
  std::vector<unsigned int> multiplicities;
  fillMultiplicities(sums, multiplicities);
    
  /** Compute JetEt sum */  
  const std::vector<int> weights = m_configSvc->thresholdConfig()->caloInfo().jetWeights();
  return jetEtSum(multiplicities, weights);
  
}


/** Compute JetEtSum value from vector of Jet_ROI objects */

unsigned int L1JetEtTools::jetEtSum(const std::vector<Jet_ROI>& rois)
{

  /** Fill multiplicity vectors */
  std::vector<unsigned int> multiplicities;
  fillMultiplicities(rois, multiplicities);
    
  /** Compute JetEt sum */
  const std::vector<int> weights = m_configSvc->thresholdConfig()->caloInfo().jetWeights();
  return jetEtSum(multiplicities, weights);
  
}


/** Form JetEt sum from user-supplied multiplicities and weights */

unsigned int L1JetEtTools::jetEtSum(const std::vector< unsigned int >& multiplicities, const std::vector< int >& weights)
{
 
  unsigned int jetEt = 0;

  for (unsigned int thresh = 0; thresh < multiplicities.size(); ++thresh) 
     if (thresh < weights.size()) jetEt += multiplicities[thresh] * weights[thresh];

  ATH_MSG_DEBUG( "JetEt value = " << jetEt );
  
  return jetEt;
}


/** Apply thresholds and fill RoI word */
unsigned int L1JetEtTools::jetEtRoIWord(unsigned int jetEt)
{
 
  /** Test ET estimator against thresholds */
  unsigned int hits = 0;
  L1DataDef def;
  std::vector<TriggerThreshold*> thresholds = m_configSvc->ctpConfig()->menu().thresholdVector();
  std::vector<TriggerThreshold*>::const_iterator it;
  for (it = thresholds.begin(); it != thresholds.end(); ++it) {
    if ( (*it)->type() == def.jeType() ) {
      TriggerThresholdValue* tv = (*it)->triggerThresholdValue(0,0); // ET trigger thresholds can only have one global value
      unsigned int threshNum = (*it)->thresholdNumber();
      unsigned int thresholdValue = static_cast<unsigned>((*tv).ptcut());
      // set bit if relevant threshold is passed
      if (jetEt > thresholdValue) hits=hits|(1<<(threshNum));
      ATH_MSG_DEBUG("Passed threshold "  << threshNum << " (" << thresholdValue << "*GeV)");
    }
  }

  /** Form RoIWord */
  unsigned int roiWord = (TrigT1CaloDefs::jetRoIType<<30) + (TrigT1CaloDefs::jetEtRoI<<29);
  roiWord += hits;
  ATH_MSG_DEBUG( "JetEt RoIWord = " << std::hex << roiWord << std::dec );

  return roiWord;
 
}


/** Compute threshold multiplicities from a vector of JetROI objects */

void L1JetEtTools::fillMultiplicities(const DataVector<JetROI>* rois, std::vector< unsigned int >& multiplicities)
{

  // Reset jet multiplicity sums to zero
  multiplicities.assign(TrigT1CaloDefs::numOfJetThresholds+TrigT1CaloDefs::numOfFwdJetThresholds,0);

  // Create temporary arrays for left, right FJ sums
  unsigned int  leftFwdMultiplicities[TrigT1CaloDefs::numOfFwdJetThresholds] = {0};
  unsigned int rightFwdMultiplicities[TrigT1CaloDefs::numOfFwdJetThresholds] = {0};
  
  /** Now step through input vector and count jets passing each threshold */

  DataVector<JetROI>::const_iterator roi = rois->begin();
  for ( ; roi != rois->end(); ++roi) {
    unsigned int roiWord = (*roi)->roiWord();
    
    if (!(*roi)->isForward()) {
      for (unsigned int ithresh = 0; ithresh < TrigT1CaloDefs::numOfJetThresholds; ++ithresh)
         if ( roiWord&(1<<ithresh) && multiplicities[ithresh] < 7 ) multiplicities[ithresh]++;
    }
    else {
      for (unsigned int ithresh = 0; ithresh < TrigT1CaloDefs::numOfFwdJetThresholds; ++ithresh) {
        double eta = (*roi)->eta();
        if ( roiWord&(1<<(ithresh+TrigT1CaloDefs::numOfJetThresholds)) ) {
          if (eta < 0 && leftFwdMultiplicities[ithresh] < 3)        leftFwdMultiplicities[ithresh]++;
          else if  (eta > 0 && rightFwdMultiplicities[ithresh] < 3) rightFwdMultiplicities[ithresh]++;
        }
      }
    }
  }

  /** Sum forward mults */
  for (unsigned int i = 0; i < TrigT1CaloDefs::numOfFwdJetThresholds; ++i) 
    multiplicities[i+TrigT1CaloDefs::numOfJetThresholds] = leftFwdMultiplicities[i] + rightFwdMultiplicities[i];
      
}


/** Compute threshold multiplicities from a CMMJetHitsCollection */

void L1JetEtTools::fillMultiplicities(const CMMJetHitsCollection* sums, std::vector< unsigned int >& multiplicities)
{

  // Reset jet multiplicity sums to zero
  multiplicities.assign(TrigT1CaloDefs::numOfJetThresholds+TrigT1CaloDefs::numOfFwdJetThresholds,0);
  
  /** Now step through input collection and count jets passing each threshold */

  CMMJetHitsCollection::const_iterator sum = sums->begin();
  for ( ; sum != sums->end(); ++sum) {
    if ((*sum)->dataID() == CMMJetHits::TOTAL_MAIN) {
      unsigned int hits = (*sum)->Hits();
      for (unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfJetThresholds; ++thresh) 
         multiplicities[thresh] = (hits>>(3*thresh))&0x7;
    }
    else if ((*sum)->dataID() == CMMJetHits::TOTAL_FORWARD) {
      unsigned int hits = (*sum)->Hits();
      for (unsigned int thresh = 0; thresh < TrigT1CaloDefs::numOfFwdJetThresholds; ++thresh) 
         multiplicities[thresh+TrigT1CaloDefs::numOfJetThresholds]
           = ( (hits>>(2*thresh)) & 0x3 )
           + ( (hits>>(2*(thresh+TrigT1CaloDefs::numOfFwdJetThresholds))) & 0x3 );
    }
  }
    
}



/** Compute threshold multiplicities from a vector of Jet_ROI objects */

void L1JetEtTools::fillMultiplicities(const std::vector<Jet_ROI>& rois, std::vector< unsigned int >& multiplicities)
{

  // Reset jet multiplicity sums to zero
  multiplicities.assign(TrigT1CaloDefs::numOfJetThresholds+TrigT1CaloDefs::numOfFwdJetThresholds,0);

  // Create temporary arrays for left, right FJ sums
  unsigned int  leftFwdMultiplicities[TrigT1CaloDefs::numOfFwdJetThresholds] = {0};
  unsigned int rightFwdMultiplicities[TrigT1CaloDefs::numOfFwdJetThresholds] = {0};
  
  /** Now step through input vector and count jets passing each threshold */

  std::vector<Jet_ROI>::const_iterator roi = rois.begin();
  for ( ; roi != rois.end(); ++roi) {
    unsigned int roiWord = (*roi).getROIWord();
    double eta = (*roi).getEta();
    
    if (fabs(eta)<3.2) {
      for (unsigned int ithresh = 0; ithresh < TrigT1CaloDefs::numOfJetThresholds; ++ithresh)
         if ( roiWord&(1<<ithresh) && multiplicities[ithresh] < 7 ) multiplicities[ithresh]++;
    }
    else {
      for (unsigned int ithresh = 0; ithresh < TrigT1CaloDefs::numOfFwdJetThresholds; ++ithresh) {
        if ( roiWord&(1<<(ithresh+TrigT1CaloDefs::numOfJetThresholds)) ) {
          if (eta < 0 && leftFwdMultiplicities[ithresh] < 3)        leftFwdMultiplicities[ithresh]++;
          else if  (eta > 0 && rightFwdMultiplicities[ithresh] < 3) rightFwdMultiplicities[ithresh]++;
        }
      }
    }
  }

  /** Sum forward mults */
  for (unsigned int i = 0; i < TrigT1CaloDefs::numOfFwdJetThresholds; ++i) 
    multiplicities[i+TrigT1CaloDefs::numOfJetThresholds] = leftFwdMultiplicities[i] + rightFwdMultiplicities[i];
      
}

//============================================================================================

} // end of namespace
