/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////// 
// PixelCellDiscriminator.cxx 
//   Implementation file for class PixelCellDiscriminator 
/////////////////////////////////////////////////////////////////// 
// (c) ATLAS Detector software 
/////////////////////////////////////////////////////////////////// 

#include "PixelCellDiscriminator.h"
#include "SiDigitization/SiChargedDiodeCollection.h"
#include "SiDigitization/SiHelper.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "TimeSvc.h"
// random number service
#include "CLHEP/Random/RandomEngine.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"
////#include "InDetIdentifier/PixelID.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

using namespace InDetDD;

static const InterfaceID IID_IPixelCellDiscriminator("PixelCellDiscriminator", 1, 0);
const InterfaceID& PixelCellDiscriminator::interfaceID( ){ return IID_IPixelCellDiscriminator; }

// Constructor with parameters:
PixelCellDiscriminator::PixelCellDiscriminator(const std::string& type, const std::string& name,const IInterface* parent):
  AthAlgTool(type,name,parent),
  m_TimeSvc("TimeSvc",name),
  m_pixelCalibSvc("PixelCalibSvc", name),
  m_rndmSvc("AtDSFMTGenSvc",name),
  m_rndmEngineName("PixelDigitization"),
  m_rndmEngine(0),
  m_timingTune(2015),
  m_disableTimeSim(false)
{  
	declareInterface< PixelCellDiscriminator >( this );
	declareProperty("RndmSvc",m_rndmSvc,"Random Number Service used in Pixel digitization");
	declareProperty("RndmEngine",m_rndmEngineName,"Random engine name");
	declareProperty("TimeSvc",m_TimeSvc);
	declareProperty("TimingTune",m_timingTune,"Version of the timing tune");	
  declareProperty("DisableTimeSim",m_disableTimeSim);
}

// Destructor:
PixelCellDiscriminator::~PixelCellDiscriminator()
{}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelCellDiscriminator::initialize() {

  CHECK(m_TimeSvc.retrieve());

  CHECK(m_pixelCalibSvc.retrieve());

  CHECK(m_rndmSvc.retrieve());
  m_rndmEngine = m_rndmSvc->GetEngine(m_rndmEngineName);
  if (!m_rndmEngine) {
    ATH_MSG_ERROR("Could not find RndmEngine : " << m_rndmEngineName);
    return StatusCode::FAILURE;
  } 
  else { 
    ATH_MSG_DEBUG("Found RndmEngine : " << m_rndmEngineName);  
  }

  ATH_MSG_DEBUG ( "PixelCellDiscriminator::initialize()");
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelCellDiscriminator::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelCellDiscriminator::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelCellDiscriminator::finalize()");
  return sc ;
}

// process the collection of charged diodes
void PixelCellDiscriminator::process(SiChargedDiodeCollection &collection) const
{   
  bool ComputeTW = false;
  const PixelModuleDesign *p_design = dynamic_cast<const PixelModuleDesign*>(&(collection.element()->design()));
  if (p_design->getReadoutTechnology()==PixelModuleDesign::FEI3) {
    ComputeTW = true;
  }

  const PixelID* pixelId = static_cast<const PixelID *>((collection.element())->getIdHelper());

  for(SiChargedDiodeIterator i_chargedDiode=collection.begin() ;
      i_chargedDiode!=collection.end() ; ++i_chargedDiode) {

    int BCN = m_TimeSvc->getTimeBCN();

    Identifier diodeID = collection.getId((*i_chargedDiode).first);

    //
    // calculate the threshold:
    //    th0      : mean value
    //    th0 sigma: variation of whatever sample the mean was obtained
    //    noise    : actually a noise level that logically should be added to
    //               the charge <- TODO!
    //
    double th0  = m_pixelCalibSvc->getThreshold(diodeID);
    double ith0 = m_pixelCalibSvc->getTimeWalk(diodeID);
    // Flers: here I rely on CalibSvc providing correct values for th0, ith0
    // if that's not true, need to figure out if we are in dbm and set
    // th0, ith0, e.g.
    //                      if (dbm) { th0=1200.; ith0=1500.; }
    double threshold = th0 +
      m_pixelCalibSvc->getThresholdSigma(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine) +
      m_pixelCalibSvc->getNoise(diodeID)*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
     
    double intimethreshold  =  (ith0/th0)*threshold;
    // double overdrive        =  intimethreshold - threshold ;
    // compute overcharge for this diode
    double overcharge=(*i_chargedDiode).second.charge()-threshold;
    // if the discriminator fires
    if (overcharge>0) {
      // compute the relative bunch number 
      // if hit comes from track or Xtalk calculates Bunch Crossing with TimeWalk (timing algoritm)
      // else (i.e. is a noise hit or ...) assigns a random BC
      int bunch = 0;
      if (!m_disableTimeSim) {
        if( (*i_chargedDiode).second.totalCharge().fromTrack()){
          if (m_timingTune==2015) {
            // Timing tune from 2015 collision data.
            int bec      = pixelId->barrel_ec(collection.element()->identify());
            int layerID  = pixelId->layer_disk(collection.element()->identify());
            int moduleID = pixelId->eta_module(collection.element()->identify());
            bunch = m_TimeSvc->relativeBunch2015((*i_chargedDiode).second.totalCharge(),bec,layerID,moduleID);
          }
          else {
            // Old tune from 2010 cosmic data.
            bunch=m_TimeSvc->relativeBunch(threshold, intimethreshold, (*i_chargedDiode).second.totalCharge(), ComputeTW);
          }
        } else {
          bunch=CLHEP::RandFlat::shootInt(m_rndmEngine,BCN);
        }
      }
      if (bunch<0 || bunch>BCN) { //If bunch is out of range, set below threshold (keep hits with +1 BC, for hit duplication). 
        SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
      } else {  
        SiHelper::SetBunch((*i_chargedDiode).second,bunch);
      }
      //ATH_MSG_DEBUG ( (*i_chargedDiode).second.totalCharge().fromTrack() <<" Charge = " << (*i_chargedDiode).second.charge()
      //<< " , bunch = " << bunch << " , BCN = " << BCN);
    } else {
      //Set the Below threshold flag if charge < threshold
      SiHelper::belowThreshold((*i_chargedDiode).second,true,true);
    }
  }
}
