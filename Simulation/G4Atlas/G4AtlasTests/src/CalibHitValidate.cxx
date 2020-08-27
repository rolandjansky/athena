/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
//
// Description: see CalibHitValidate.h
//
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
//-----------------------------------------------------------------------

#include "CalibHitValidate.h"

#include "McParticleEvent/TruthParticle.h"

#include "CaloSimEvent/CaloCalibrationHit.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"

#include "CLHEP/Units/SystemOfUnits.h"

#include <iterator>
#include <sstream>
#include <set>

#include <math.h>


//###############################################################################

CalibHitValidate::CalibHitValidate(const std::string& name,
                                   ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
{

  declareProperty("CalibrationHitContainerNames",m_CalibrationHitContainerNames);
  declareProperty("DMCalibrationHitContainerNames",m_DMCalibrationHitContainerNames);
}


CalibHitValidate::~CalibHitValidate() {
}


//###############################################################################

StatusCode CalibHitValidate::initialize()
{
  ATH_MSG_VERBOSE ( "Initializing " << name() );

  return StatusCode::SUCCESS;
}

//###############################################################################

StatusCode CalibHitValidate::execute()
{
  // get truth containers
  const McEventCollection* truthEvent=0;
  StatusCode sc0 = evtStore()->retrieve(truthEvent, "TruthEvent");
  if (sc0.isFailure()||!truthEvent){
    ATH_MSG_ERROR ( "No  McEventCollection found");
    return StatusCode::FAILURE;
  }
  HepMC::GenEvent::particle_const_iterator pit  = truthEvent->at(0)->particles_begin();
  HepMC::ConstGenParticlePtr   gen  = *pit;


  std::vector<std::string>::iterator containerNameIter;

  std::vector<double> hitEnergies;

  double totalEnergyAllContainers=0.;

  // calibration hit containers
  const DataHandle<CaloCalibrationHitContainer> cchc;
  std::vector<const CaloCalibrationHitContainer *> v_cchc;
  for (containerNameIter=m_CalibrationHitContainerNames.begin();containerNameIter!=m_CalibrationHitContainerNames.end();containerNameIter++) {


    double totEnergy=0.;

    if ( evtStore()->contains<CaloCalibrationHitContainer>(*containerNameIter)) {
      StatusCode sc = evtStore()->retrieve(cchc,*containerNameIter);
      if (sc.isFailure() ) {
        ATH_MSG_ERROR ( "Cannot retrieve calibration hit container " << *containerNameIter );
        exit(3);
      }

      //loop over cells in calibration container
      CaloCalibrationHitContainer::const_iterator chIter  = cchc->begin();
      CaloCalibrationHitContainer::const_iterator chIterE = cchc->end();

      for(;chIter!=chIterE;chIter++)
        totEnergy+=(*chIter)->energyTotal();
    }

    ATH_MSG_INFO ("container name: "<<*containerNameIter<<", energy="<<totEnergy);

    totalEnergyAllContainers += totEnergy;

    hitEnergies.push_back(totEnergy);
  }

  // dark material calibration hit containers

  std::vector<double> hitEnergiesDM;

  std::vector<const CaloCalibrationHitContainer *> v_dmcchc;
  for (containerNameIter=m_DMCalibrationHitContainerNames.begin();containerNameIter!=m_DMCalibrationHitContainerNames.end();containerNameIter++) {


    double totEnergy=0.;
    if ( evtStore()->contains<CaloCalibrationHitContainer>(*containerNameIter)) {
      StatusCode sc = evtStore()->retrieve(cchc,*containerNameIter);
      if (sc.isFailure() ) {
        ATH_MSG_ERROR ( "Cannot retrieve DM calibration hit container " << *containerNameIter );
        exit(3);
      }

      //loop over cells in calibration container
      CaloCalibrationHitContainer::const_iterator chIter  = cchc->begin();
      CaloCalibrationHitContainer::const_iterator chIterE = cchc->end();

      for(;chIter!=chIterE;chIter++)
        totEnergy+=(*chIter)->energyTotal();
    }

    ATH_MSG_INFO ("DM container name: "<<*containerNameIter<<", energy="<<totEnergy);

    totalEnergyAllContainers += totEnergy;

    hitEnergiesDM.push_back(totEnergy);
  }

  ATH_MSG_INFO ("total energy in hits: "<<totalEnergyAllContainers<<", in primary particle: "<<gen->momentum().e());

  return StatusCode::SUCCESS;
}
