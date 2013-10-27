/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//************************************************************
//
// Class MinBiasScintillatorSD
// Sensitive detector for the Minimum Bias Scintillator
//
// Author: A. Solodkov
// April 01, 2006.
//
//************************************************************

#include "MinBiasScintillator/MinBiasScintillatorSD.hh"
#include "CaloIdentifier/TileTBID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "TileSimUtils/TileG4SimOptions.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"

//#include "DetectorDescriptionStore.h"
#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "FadsSensitiveDetector/SensitiveDetectorCatalog.h"

#include "G4HCofThisEvent.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4Trd.hh"

#include "G4UImanager.hh"
#include "G4ios.hh"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include <cassert>

static SensitiveDetectorEntryT<MinBiasScintillatorSD> minbiasscintillatorSD("MinBiasScintillatorSD");

MinBiasScintillatorSD::MinBiasScintillatorSD(G4String name)
  : FadsSensitiveDetector(name),
    m_msg("MinBiasScintillatorSD"),
    m_containerName("MBTSHits"),
    m_iamowner(true)
{  
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("StoreGateSvc", m_sgSvc);
  if(status.isFailure())
    {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "StoreGateSvc not found!" << endreq;
    }
  else
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "StoreGateSvc initialized" << endreq;
    }

  status = svcLocator->service("DetectorStore", m_detStore);
  if(status.isFailure())
    {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "DetectorStoreSvc not found!" << endreq;
    }
  else
    {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "DetectorStoreSvc initialized" << endreq;
    }


 const TileG4SimOptions* tileG4SimOptions;
  status = m_detStore->retrieve(tileG4SimOptions);
  if (status.isFailure()) {
    m_deltaT = 5.0 * CLHEP::ns;
    m_deltaTvec.resize(1); m_deltaTvec[0]=m_deltaT;
    m_timeCut = 402.5 * CLHEP::ns;
    m_doBirk = true;
    m_doTOFCorr = true;
    if (m_deltaT>0.0) m_timeCut = ((int)(m_timeCut/m_deltaT) + 0.5) * m_deltaT;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TileG4SimOptions was not found in DetectorStore" << endreq;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Setting deltaTHit to "<< m_deltaT/CLHEP::ns << " ns. " << endreq;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Setting timeCut to " << m_timeCut/CLHEP::ns << " ns. " << endreq;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Setting doBirk to "    << (m_doBirk    ? "true" : "false") << endreq;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Setting doTOFCorr to " << (m_doTOFCorr ? "true" : "false") << endreq;
  }
  else {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TileG4SimOptions retrieved" << endreq;
    m_deltaT = tileG4SimOptions->GetDeltaTHit() * CLHEP::ns;
    m_deltaTvec = tileG4SimOptions->GetDeltaTHitVector();
    for (int i=m_deltaTvec.size()-1; i>-1; --i) m_deltaTvec[i] *= CLHEP::ns;
    if (m_deltaTvec.size()%3 != 1 || m_deltaT<=0.0) { m_deltaTvec.resize(1); m_deltaTvec[0]=m_deltaT; } // protection agaist wrong vector size
    m_timeCut = tileG4SimOptions->GetTimeCut() * CLHEP::ns;
    m_doBirk = tileG4SimOptions->GetDoBirk();
    m_doTOFCorr = tileG4SimOptions->GetDoTOFCorrection();
    if (m_deltaT>0.0) m_timeCut = ((int)(m_timeCut/m_deltaT) + 0.5) * m_deltaT;
    if (m_deltaTvec.size()>1) {
      // m_deltaTvec.size() is 3*Nwindows+1, therefore iisize/3 is number of windows (extra element ignored)
      unsigned int ilast = m_deltaTvec.size()-1;  // accounting for extra element, ilast is multiple of 3
      unsigned int nWindows = ilast/3;            // exact integer division, remainder=0
      if (msgLvl(MSG::INFO)) 
	{
	  msg(MSG::INFO) << "Using deltaTHit = ";
	  for(unsigned int ii=0; ii<nWindows; ++ii) {
	    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << m_deltaTvec[3*ii]/CLHEP::ns << " ns for window [" << m_deltaTvec[3*ii+1] << "," << m_deltaTvec[3*ii+2] << "] ";
	  }
	  msg(MSG::INFO) << "and " << m_deltaTvec[ilast]/CLHEP::ns <<" ns outside this window"<< endreq;
	}
    } else {
        if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Using deltaTHit = " << m_deltaT/CLHEP::ns << " ns. " << endreq;
    }
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Using timeCut = "  << m_timeCut/CLHEP::ns << " ns. " << endreq;
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Using doBirk = "    << (m_doBirk    ? "true" : "false") << endreq;
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Using doTOFCorr = " << (m_doTOFCorr ? "true" : "false") << endreq;
  }
  if ( ! (m_deltaT > 0.0) )
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "deltaT is not set, ignore hit time in ProcessHits()" << endreq;

  if ( ! m_doTOFCorr && m_timeCut < 1000*CLHEP::ns ) {
    // assuming that if TOF correction is disabled, then we are running cosmic simulation
    // and should not use too restrictive time cut
    m_timeCut = 100000*CLHEP::ns;
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "TOF correction is disabled, settting time cut to " 
        << m_timeCut/CLHEP::ns << " ns. " << endreq;
  }

  m_lateHitTime = 100000*CLHEP::ns - m_deltaT;
  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "All hits with time above " << m_timeCut/CLHEP::ns 
					<< " ns will be stored with time = " << m_lateHitTime/CLHEP::ns << " ns." << endreq;

  // position of MBTS cells
  double Z[2] = {-3510.*CLHEP::mm, 3510.*CLHEP::mm};
  double R[2] = {  290.*CLHEP::mm,  660.*CLHEP::mm};

  status = m_detStore->retrieve(m_tileTBID);
  if(status.isFailure())
    {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "No TileTBID helper" << endreq;
    }
  else 
    {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "TileTBID helper retrieved" << endreq;

    const MbtsDetDescrManager* mbtsMgr; //!< Pointer to MbtsDetDescrManager
    status = m_detStore->retrieve(mbtsMgr);
    if (status.isFailure()) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)
          << "Unable to retrieve MbtsDetDescrManager from DetectorStore" << endreq;
      mbtsMgr = 0;
    }

    for (int side=0; side<nSide; ++side) {
      for (int eta=0; eta<nEta; ++eta) {
        for (int phi=0; phi<nPhi; ++phi) {
          m_id[cell_index(side,phi,eta)] = m_tileTBID->channel_id((side>0)?1:-1,phi,eta);
        }
        // approximate coordinates
        m_deltaT_cell[dist_index(side,eta)] = sqrt(Z[side]*Z[side]+R[eta]*R[eta])/CLHEP::c_light;
        if (mbtsMgr) {
          const CaloDetDescrElement * dde = mbtsMgr->get_element(m_id[cell_index(side,0,eta)]);
          // temporary fix for rel 15.6.10
          if (!dde && side==0) dde=mbtsMgr->get_element(m_tileTBID->channel_id(0,0,eta));
          if (dde) {
            float x = dde->x();
            float y = dde->y();
            float z = dde->z();
            if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)
              << "MBTS " 
              << " side = " << ((side>0)?1:-1)
              << " eta = " << eta
              << " Z = " << z
              << " R = " << sqrt(x*x+y*y) << endreq;
            m_deltaT_cell[dist_index(side,eta)] = sqrt(x*x+y*y+z*z)/CLHEP::c_light;
          }
        }
      }
    }
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Distance to MBTS cells (in mm): ";
  for (unsigned int i=0; i<sizeof(m_deltaT_cell)/sizeof(double); ++i)
    msg(MSG::DEBUG) << m_deltaT_cell[i]*CLHEP::c_light/CLHEP::mm << ", ";
  msg(MSG::DEBUG) << endreq;
}

MinBiasScintillatorSD::~MinBiasScintillatorSD()
{
}

void MinBiasScintillatorSD::Initialize(G4HCofThisEvent* /* HCE */)
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initializing SD" << endreq;

  memset(m_nhits,0,sizeof(m_nhits));
}

G4bool MinBiasScintillatorSD::ProcessHits(G4Step* aStep,G4TouchableHistory* /* ROhist */)
{
  if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "i am in Min Bias Scint process hit" << endreq; 
    
  if ( aStep->GetTotalEnergyDeposit() == 0. )
  {
    if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Edep=0" << endreq;
    return false;
  }

  G4StepPoint * preStep = aStep->GetPreStepPoint();
  G4double totalTime = aStep->GetPostStepPoint()->GetGlobalTime();//------
  G4ThreeVector preStepPointPosition = preStep->GetPosition();
  G4TouchableHistory * theTouchable = (G4TouchableHistory*)(preStep->GetTouchable());
  G4VPhysicalVolume * physVol = theTouchable->GetVolume();
//  G4LogicalVolume * logiVol = physVol->GetLogicalVolume();
//if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "GlobalTime = " << totalTime << endreq;
  
  
  std::string volName = physVol->GetName();
  std::string volNumber = volName.substr(volName.size()-1,1); 
  int side = (preStepPointPosition.z()>0) ? 1 : 0;
  int phi  = physVol->GetCopyNo();
  int eta  = atoi(volNumber.c_str())-1;
  int ind  = cell_index(side,phi,eta);
  //if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "R = " 
  //    << sqrt(preStepPointPosition.x()*preStepPointPosition.x()+preStepPointPosition.y()*preStepPointPosition.y())
  //    << " Z = " << preStepPointPosition.z()
  //    << " eta = " << eta << endreq;

  if (ind>=nCell || ind<0) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Hit in "
        << side << "/" << phi << "/" << eta
        << " index=" <<ind<<" is outside range [0,"<<nCell-1<<"]"<< endreq;
    return false;
  }

  // Take into account Birk's saturation law in organic scintillators.
  G4double edep;
  if (m_doBirk)
    edep = BirkLaw(aStep);
  else
    edep = aStep->GetTotalEnergyDeposit();

  double time = totalTime;

  // time correction - subtract time of flight to the cell from ATLAS center
  if (m_doTOFCorr) {
    time -= m_deltaT_cell[dist_index(side,eta)];
  }
  // check that hit time is within limits
  // if not - put it to maximum allowed time
  if (time > m_timeCut ) {
    time = m_lateHitTime;
  }
  
  // find deltaT for given time 
  m_deltaT = deltaT(time);

//  if(logiVol.find("MBTS") !=G4String::npos) // not clear which name to use, but it's not needed
  {
    if ( m_nhits[ind] > 0 )
    { 
      // hit already exists
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "Additional hit in "
						  << side << "/" << phi << "/" << eta 
						  << " ene=" << edep << " time=" << time << endreq;
      m_hit[ind]->add(edep,time,m_deltaT);
    }
    else
    {
      // First hit in a cell
      if (msgLvl(MSG::VERBOSE)) msg(MSG::VERBOSE) << "First hit in "
						  << side << "/" << phi << "/" << eta 
						  << " ene=" << edep << " time=" << time << endreq;
      m_hit[ind] = new TileSimHit(m_id[ind],edep,time,m_deltaT);  
    }
    ++m_nhits[ind];
  }  
  return true;
}

void MinBiasScintillatorSD::EndOfEvent(G4HCofThisEvent* /* HCE */)
{
  if (! m_iamowner || m_allowMods) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Looking for '" << m_containerName 
	 << "' collection in StoreGate" << endreq;

    m_hitsCollection = m_hitCollHelper.RetrieveNonconstCollection<TileHitVector>(m_containerName);
    /*
    StatusCode status = m_sgSvc->retrieve(m_hitsCollection,m_containerName);
    m_iamowner = (status.isFailure());
    if (m_iamowner) 
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "'" << m_containerName 
          << "' collection not found, need to create it" << endreq;
    */
  }

  else {
    m_hitsCollection = new TileHitVector(m_containerName); 
    StatusCode status = m_sgSvc->record(m_hitsCollection,m_hitsCollection->Name(),true);
    if (status.isFailure())
      {
	if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Can't initialize '" << m_containerName << "' collection" << endreq; 
      }
    else
      {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "I have initialized '" << m_containerName << "' collection" << endreq; 
      }
  }
  
  for (int ind=0; ind<nCell; ++ind) {
    int nhit= m_nhits[ind];
    if (nhit>0) {
      if (msgLvl(MSG::DEBUG)) 
	msg(MSG::DEBUG)
	  << "Cell id=" << m_tileTBID->to_string(m_id[ind])
	  << " nhit=" << nhit << " ene=" << m_hit[ind]->energy()
	  << endreq;
      m_hitsCollection->Insert(TileHit(m_hit[ind]));
      delete m_hit[ind];
    } else {
      if (msgLvl(MSG::VERBOSE)) 
	msg(MSG::VERBOSE)
	  << "Cell id=" << m_tileTBID->to_string(m_id[ind])
	  << " nhit=0"  << endreq;
    }
  }

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Total number of hits is " << m_hitsCollection->size() << endreq;

  if (m_iamowner && !m_allowMods) {
    /*
    StatusCode status = m_sgSvc->setConst(m_hitsCollection);
    if(status.isFailure())
      {
	if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Can't lock Collection '" << m_containerName << "'" << endreq;
      }
    else
      {
	if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Collection '" << m_containerName << "' is locked " << endreq;
      }
    */
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "setting collection "<<m_hitsCollection->Name()<<" to const" << endreq;
    m_hitCollHelper.SetConstCollection<TileHitVector>(m_hitsCollection);
  }
}

G4double MinBiasScintillatorSD::BirkLaw(const G4Step* aStep) const
{
// *** apply BIRK's saturation law to energy deposition ***
// *** only organic scintillators implemented in this version MODEL=1
//
// Note : the material is assumed ideal, which means that impurities
//        and aging effects are not taken into account
//
// algorithm : edep = destep / (1. + RKB*dedx + C*(dedx)**2)
//
// the basic units of the coefficient are g/(MeV*cm**2)
// and de/dx is obtained in MeV/(g/cm**2)
//
// exp. values from NIM 80 (1970) 239-244 :
//
// RKB = 0.013  g/(MeV*cm**2)  and  C = 9.6e-6  g**2/((MeV**2)(cm**4))
//

    const G4String myMaterial = "Scintillator";
    const G4double birk1 = 0.0130*CLHEP::g/(CLHEP::MeV*CLHEP::cm2);
    const G4double birk2 = 9.6e-6*CLHEP::g/(CLHEP::MeV*CLHEP::cm2)*CLHEP::g/(CLHEP::MeV*CLHEP::cm2);

    G4double destep      = aStep->GetTotalEnergyDeposit();
//  doesn't work with shower parameterisation
//  G4Material* material = aStep->GetTrack()->GetMaterial();
//  G4double charge      = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
    G4Material* material = aStep->GetPreStepPoint()->GetMaterial();
    G4double charge      = aStep->GetPreStepPoint()->GetCharge();

// --- no saturation law for neutral particles ---
// ---  and materials other than scintillator  ---
    if ( (charge!=0.) && (material->GetName()==myMaterial) ) {
        G4double rkb = birk1;
// --- correction for particles with more than 1 charge unit ---
// --- based on alpha particle data (only apply for MODEL=1) ---
        if (fabs(charge) > 1.0 ) rkb *= 7.2/12.6;

        G4double dedx = destep/(aStep->GetStepLength())/(material->GetDensity());

        G4double response = destep/(1. + rkb*dedx + birk2*dedx*dedx);

        if (-2==verboseLevel) {
	  G4cout << " Destep: " << destep/CLHEP::keV << " keV"
                   << " response after Birk: "  << response/CLHEP::keV << " keV" << G4endl;
        }
        return response;
    } else {
        return destep;
    }
}

