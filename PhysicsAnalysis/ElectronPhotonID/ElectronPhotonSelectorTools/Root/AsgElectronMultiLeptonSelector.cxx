/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AsgElectronMultiLeptonSelector
   @brief Electron selector tool to select objects in Asgena using an underlying pure ROOT tool.

   @author Karsten Koeneke
   @date   October 2012

   12-MAR-2014 convert to ASG tool (Jovan Mitrevski)

*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgElectronMultiLeptonSelector.h"
#include "TElectronMultiLeptonSelector.h"
#include "xAODEgamma/Electron.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODCaloEvent/CaloCluster.h"

#include <cmath>
#include <cstdint>

//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronMultiLeptonSelector::AsgElectronMultiLeptonSelector(const std::string& myname) :
  AsgTool(myname),
  m_rootTool(nullptr)
{
 
  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronMultiLeptonSelector(myname.c_str());

}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronMultiLeptonSelector::~AsgElectronMultiLeptonSelector()
{
  if(finalize().isFailure()){
    ATH_MSG_ERROR ( "Failure in AsgElectronMultiLeptonSelector finalize()");
  }
  delete m_rootTool;
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronMultiLeptonSelector::initialize()
{

  ATH_MSG_INFO ("Initialize ");
    

    // We need to initialize the underlying ROOT TSelectorTool
  m_rootTool->msg().setLevel(this->msg().level());
  if ( m_rootTool->initialize().isFailure() )
    {
      ATH_MSG_ERROR ( "ERROR! Could not initialize the TElectronMultiLeptonSelector!" );
      return StatusCode::FAILURE;
    }

  ATH_MSG_INFO ("Initialize end ");
  
  return StatusCode::SUCCESS ;
}



//=============================================================================
// finalize method (now called by destructor)
//=============================================================================
StatusCode AsgElectronMultiLeptonSelector::finalize()
{
  return StatusCode::SUCCESS;
}


//=============================================================================
// return the accept info object
//=============================================================================

const asg::AcceptInfo& AsgElectronMultiLeptonSelector::getAcceptInfo() const
{
    return m_rootTool->getAcceptInfo();
}


//=============================================================================
// The main accept method: the actual cuts are applied here 
//=============================================================================
asg::AcceptData AsgElectronMultiLeptonSelector::accept( const xAOD::Electron* eg ) const
{
  if ( !eg )
    {
      ATH_MSG_DEBUG ("Failed, no egamma object.");
      return m_rootTool->accept();
    }
  
  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if ( !cluster )
    {
      ATH_MSG_DEBUG ("Failed, no cluster.");
      return m_rootTool->accept();
    }  
  
  float eta = fabsf(cluster->etaBE(2)); 
  if ( eta > 100.0 )
    {
      ATH_MSG_DEBUG ("Failed, eta range.");
      return m_rootTool->accept();
    }
  
  // transverse energy of the electron (using the track eta) 
  double et = eg->pt();
  
  bool allFound = true;

  float Reta(0);
  float Rhad1(0);
  float Rhad(0);
  float w2(0);
  float f1(0);
  float f3(0);
  float wstot(0);
  float Eratio(0);
  float deltaEta(0);
  float deltaPhiRescaled(0);

  // Reta
  allFound = allFound && eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta);
  allFound = allFound && eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1);
  allFound = allFound && eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad);
  // shower width in 2nd sampling
  allFound = allFound && eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2);
  // fraction of energy reconstructed in the 1st sampling
  allFound = allFound && eg->showerShapeValue(f1, xAOD::EgammaParameters::f1);
  // E of 1st max in strips
  allFound = allFound && eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio);
  // fraction of energy reconstructed in the 3rd sampling
  allFound = allFound && eg->showerShapeValue(f3, xAOD::EgammaParameters::f3);
  // total shower width in 1st sampling
  allFound = allFound && eg->showerShapeValue(wstot, xAOD::EgammaParameters::wtots1);
      
  

  // delta eta
  allFound = allFound && eg->trackCaloMatchValue(deltaEta, xAOD::EgammaParameters::deltaEta1);
  
  // difference between the cluster phi (sampling 2) and the eta of the track extrapolated from the last measurement point.
  allFound = allFound && eg->trackCaloMatchValue(deltaPhiRescaled, xAOD::EgammaParameters::deltaPhiRescaled2);
  
  // number of track hits
  uint8_t nSCT(0);
  uint8_t nSCTDeadSensors(0);
  uint8_t nSi(0);
  uint8_t nSiDeadSensors(0);
  uint8_t nPix(0);
  uint8_t nPixDeadSensors(0); 
  uint8_t nTRThigh(0); 
  uint8_t nTRThighOutliers(0); 
  uint8_t nTRT(0); 
  uint8_t nTRTOutliers(0);
  uint8_t expectBlayer(1);
  uint8_t nBlayerHits(0); 
  double trackqoverp(0.0);
  double dpOverp(0.0);
  // retrieve associated track
  const xAOD::TrackParticle* t  = eg->trackParticle();    

  if (t) {
    trackqoverp = t->qOverP();
    
    allFound = allFound && t->summaryValue(nBlayerHits, xAOD::numberOfInnermostPixelLayerHits);
    allFound = allFound && t->summaryValue(nPix, xAOD::numberOfPixelHits);
    allFound = allFound && t->summaryValue(nSCT, xAOD::numberOfSCTHits);
    allFound = allFound && t->summaryValue(nPixDeadSensors, xAOD::numberOfPixelDeadSensors);
    allFound = allFound && t->summaryValue(nSCTDeadSensors, xAOD::numberOfSCTDeadSensors);
    
    allFound = allFound && t->summaryValue(nTRThigh, xAOD::numberOfTRTHighThresholdHits);
    allFound = allFound && t->summaryValue(nTRThighOutliers, xAOD::numberOfTRTHighThresholdOutliers);
    allFound = allFound && t->summaryValue(nTRT, xAOD::numberOfTRTHits);
    allFound = allFound && t->summaryValue(nTRTOutliers, xAOD::numberOfTRTOutliers);
    
    allFound = allFound && t->summaryValue(expectBlayer, xAOD::expectInnermostPixelLayerHit);
    
    
    nSi = nPix+nSCT;
    nSiDeadSensors = nPixDeadSensors+nSCTDeadSensors;

    unsigned int index;
    if( t->indexOfParameterAtPosition(index, xAOD::LastMeasurement) ) {
      
      double refittedTrack_LMqoverp  = 
	t->charge() / sqrt(std::pow(t->parameterPX(index), 2) +
			   std::pow(t->parameterPY(index), 2) +
			   std::pow(t->parameterPZ(index), 2));

      dpOverp = 1 - trackqoverp/(refittedTrack_LMqoverp);
    }
  } else {
    ATH_MSG_DEBUG ( "Failed, no track particle: et= " << et << "eta= " << eta );
    allFound = false;
  }
  
  // TRT high-to-low threshold hits ratio
  double rTRT(0.0);
  rTRT = (nTRT+nTRTOutliers) > 0 ?  ((double) ((double)(nTRThigh+nTRThighOutliers))/((double)(nTRT+nTRTOutliers)) ) : 0.;
  int nTRTTotal = nTRT+nTRTOutliers;

  if (!allFound) {
     // if object is bad then use the bit for "bad eta"
    ATH_MSG_WARNING("Have some variables missing.");
    return m_rootTool->accept();
  }

  // Get the answer from the underlying ROOT tool
  return m_rootTool->accept(eta, et,
                            Rhad, Rhad1,
                            Reta, w2,
                            f1, f3,
                            wstot, Eratio, 
                            deltaEta,
                            nSi, nSiDeadSensors, nPix, nPixDeadSensors,
                            deltaPhiRescaled,
                            dpOverp,
                            rTRT, nTRTTotal,
                            nBlayerHits, expectBlayer);
}


asg::AcceptData AsgElectronMultiLeptonSelector::accept(const xAOD::IParticle* part) const
{
  ATH_MSG_DEBUG("Entering accept( const IParticle* part )");
  if(part->type()==xAOD::Type::Electron){
    const xAOD::Electron* eg =static_cast<const xAOD::Electron*> (part);
    return accept(eg);
  }
  
    ATH_MSG_ERROR("AsgElectronMultiLeptonSelector::could not convert argument to accept");
    return m_rootTool->accept();
  
}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgElectronMultiLeptonSelector::getOperatingPointName() const{
  return "MultiLepton";
}
