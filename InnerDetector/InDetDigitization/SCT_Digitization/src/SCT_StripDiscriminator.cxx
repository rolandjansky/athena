/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_StripDiscriminator.h"

// Base class
#include "SiDigitization/SiHelper.h"

// Random number
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat

// Athena
#include "InDetSimEvent/SiTotalCharge.h"
#include "SiDigitization/SiChargedDiode.h"  // for SiChargedDiode
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SCT_Digitization/ISCT_TimeWalkGenerator.h"

using namespace InDetDD;

// constructor
SCT_StripDiscriminator::SCT_StripDiscriminator(const std::string& type, const std::string& name,const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_rndmEngine(nullptr),
    m_sct_TimeWalkGen("SCT_TimeWalkGenerator", this)
{
  declareInterface< ISCT_StripDiscriminator >( this );
  declareProperty("DiscrThresh",        m_discrThresh=4100,       "Discriminator threshold");
  declareProperty("DiscrThreshVar",     m_discrThreshSigma=300,   "Discriminator threshold sigma");
  declareProperty("IntimeThresh",       m_intimeThresh=5000,      "Discriminator in-time threshold");
  declareProperty("TimeBCN",            m_timeBCN=2,              "Number of BCID");
}

// Destructor:
SCT_StripDiscriminator::~SCT_StripDiscriminator() {}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode SCT_StripDiscriminator::initialize() {
	/** not needed now?
  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  **/
  ATH_MSG_DEBUG ( "SCT_StripDiscriminator::initialize()");

  //++ Get  the Time Walk generator tool  
  ATH_CHECK( m_sct_TimeWalkGen.retrieve()) ;
  ATH_MSG_DEBUG ( "\tRetrieved and initialised tool " <<m_sct_TimeWalkGen );
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Finalize
//----------------------------------------------------------------------
StatusCode SCT_StripDiscriminator::finalize() {
  /**not needed?
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "SCT_StripDiscriminator::finalize() failed");
    return sc ;
  }
  **/
  ATH_MSG_DEBUG ( "SCT_StripDiscriminator::finalize()");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// Set the random Engine
//----------------------------------------------------------------------
void SCT_StripDiscriminator::setRandomEngine(CLHEP::HepRandomEngine *rndmEngine) {
  m_rndmEngine = rndmEngine;                                                                                                                                                    
  m_sct_TimeWalkGen->setRandomEngine(rndmEngine) ;                                                                                                                              
}    

//----------------------------------------------------------------------
// process the collection of charged diodes
//----------------------------------------------------------------------
void SCT_StripDiscriminator::process(SiChargedDiodeCollection &collection) const {   

  // discriminator is applied to all cells, even unconnected ones to be able to use the unconnected cells as well
  SiChargedDiodeIterator i_chargedDiode=collection.begin(); 
  SiChargedDiodeIterator i_chargedDiode_end=collection.begin(); 
  for(; i_chargedDiode!=i_chargedDiode_end; ++i_chargedDiode) {

    int BCN = m_timeBCN ;

    /**
     * calculate the threshold: 
     * th0       : mean value
     * th0 sigma : variation of whatever sample the mean was obtained
     * noise     : actually a noise level that logically should be added to the charge <- TODO!
     */

    // At some point, things should be done by retrieving the calib data from data base via SCT_Conditions
    // double noise       = (m_useCalibCDB ? m_calibSvc->getNoise(id):0) ;
    // double thres       = (m_useCalibCDB ? m_calibSvc->getThreshold(diodeID):m_discrThresh);
    // double thresSigma  = (m_useCalibCDB ? m_calibSvc->getThresholdSigma(diodeID):m_discrThreshSigma);
    // double intimethres = (m_useCalibCDB ? m_calibSvc->getTimeWalk(diodeID):m_intimeThresh);

    // For now just for having something in place we do it that way !!!!
    double noise       = 0 ;
    double thres       = m_discrThresh;
    double thresSigma  = m_discrThreshSigma ;
    double intimeThres = m_intimeThresh ;

    double threshold        = thres + thresSigma*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine)+ noise*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
    double intimethreshold  = (intimeThres/thres)*threshold;
    double overcharge       = (*i_chargedDiode).second.charge()-threshold ;

    //    ATH_MSG_INFO ( "BCN="<<BCN<<" threshold="<<threshold<<" intimethreshold="<<intimethreshold<<" overcharge="<<overcharge);

    // if the discriminator fires
    if (overcharge>0) {
      /**
       * compute the relative bunch number if hit comes from track or Xtalk calculates Bunch Crossing 
       * with TimeWalk (timing algoritm) else (i.e. is a noise hit or ...) assigns a random BC
       */

      int bunch ;
      if((*i_chargedDiode).second.totalCharge().fromTrack()) bunch=m_sct_TimeWalkGen->relativeBunch(threshold, intimethreshold, (*i_chargedDiode).second.totalCharge()) ;
      else bunch=CLHEP::RandFlat::shootInt(m_rndmEngine,BCN) ;
      if(bunch < 0 || bunch > BCN-1) SiHelper::belowThreshold((*i_chargedDiode).second,true,true) ;
      else SiHelper::SetBunch((*i_chargedDiode).second,bunch) ;
    }
    else SiHelper::belowThreshold((*i_chargedDiode).second,true,true) ;
  }
}

