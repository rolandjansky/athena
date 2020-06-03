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

#include "MinBiasScintillatorSD.h"

#include "CaloIdentifier/TileTBID.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/MbtsDetDescrManager.h"
#include "CxxUtils/make_unique.h"// For make unique
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"

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

#include "CLHEP/Units/SystemOfUnits.h"

#include <cassert>

MinBiasScintillatorSD::MinBiasScintillatorSD(const G4String& name, const std::string& hitCollectionName,
                                             const MinBiasScintSDOptions& opts)
    : G4VSensitiveDetector(name),
      m_HitColl(hitCollectionName),
      m_numberOfHitsInCell(N_CELLS, 0),
      m_tempSimHit(N_CELLS, nullptr)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StoreGateSvc* detStore(NULL);
  if (svcLocator->service("DetectorStore", detStore).isFailure()) {
    G4ExceptionDescription description;
    description << "MinBiasScintillatorSD constructor - can't find DetStore";
    G4Exception("MinBiasScintillatorSD", "NoDetStore", FatalException, description);
    abort();
  }

  m_deltaT = opts.deltaTHit[opts.deltaTHit.size() - 1] * CLHEP::ns;
  m_deltaTvec = opts.deltaTHit;
  for (int i = m_deltaTvec.size() - 1; i > -1; --i) {
    m_deltaTvec[i] *= CLHEP::ns;
  }
  if (m_deltaTvec.size() % 3 != 1 || m_deltaT <= 0.0) {
    m_deltaTvec.resize(1);
    m_deltaTvec[0] = m_deltaT;
  } // protection agaist wrong vector size
  m_timeCut = opts.timeCut * CLHEP::ns;
  if (m_deltaT > 0.0) m_timeCut = ((int) (m_timeCut / m_deltaT) + 0.5) * m_deltaT;
  if (m_deltaTvec.size() > 1) {
    // m_deltaTvec.size() is 3*Nwindows+1, therefore iisize/3 is number of windows (extra element ignored)
    unsigned int ilast = m_deltaTvec.size() - 1;  // accounting for extra element, ilast is multiple of 3
    unsigned int nWindows = ilast / 3;            // exact integer division, remainder=0
    if (verboseLevel > 5) {
      G4cout << "MinBiasScintillatorSD INFO Using deltaTHit = ";
      for (unsigned int ii = 0; ii < nWindows; ++ii) {
        if (verboseLevel > 5) {
          G4cout << m_deltaTvec[3 * ii] / CLHEP::ns << " ns for window [" << m_deltaTvec[3 * ii + 1] << ","
                 << m_deltaTvec[3 * ii + 2] << "] ";
        }
      }
      G4cout << "and " << m_deltaTvec[ilast] / CLHEP::ns << " ns outside this window" << G4endl;
    }
  } else {
    if (verboseLevel > 5) {
      G4cout << "MinBiasScintillatorSD INFO Using deltaTHit = " << m_deltaT / CLHEP::ns << " ns. " << G4endl;
    }
  }
  if (verboseLevel > 5) {
    G4cout << "MinBiasScintillatorSD INFO Using timeCut = " << m_timeCut / CLHEP::ns << " ns. " << G4endl;
  }
  if (verboseLevel > 5) {
    G4cout << "MinBiasScintillatorSD INFO Using doBirk = " << (m_options.doBirk ? "true" : "false") << G4endl;
  }
  if (verboseLevel > 5) {
    G4cout << "MinBiasScintillatorSD INFO Using doTOFCorr = " << (m_options.doTOFCorrection ? "true" : "false")
           << G4endl;
  }

  if (! (m_deltaT > 0.0)) {
    if (verboseLevel > 5) {
      G4cout << "MinBiasScintillatorSD INFO deltaT is not set, ignore hit time in ProcessHits()" << G4endl;
    }
  }
  if (!m_options.doTOFCorrection && (m_timeCut < 1000 * CLHEP::ns)) {
    // assuming that if TOF correction is disabled, then we are running cosmic simulation
    // and should not use too restrictive time cut
    m_timeCut = 100000 * CLHEP::ns;
    G4cout << "MinBiasScintillatorSD WARNING TOF correction is disabled, setting time cut to " << m_timeCut / CLHEP::ns
           << " ns. " << G4endl;
  }

  m_lateHitTime = 100000 * CLHEP::ns - m_deltaT;
  if (verboseLevel > 5) {
    G4cout << "MinBiasScintillatorSD INFO All hits with time above " << m_timeCut / CLHEP::ns
           << " ns will be stored with time = " << m_lateHitTime / CLHEP::ns << " ns." << G4endl;
  }

  // position of MBTS cells
  double Z[2] = { -3510. * CLHEP::mm, 3510. * CLHEP::mm };
  double R[2] = { 290. * CLHEP::mm, 660. * CLHEP::mm };

  if (detStore->retrieve(m_tileTBID).isFailure()) {
    G4ExceptionDescription description;
    description << "MinBiasScintillatorSD constructor - No TileTBID helper";
    G4Exception("MinBiasScintillatorSD", "NoTileTBID", FatalException, description);
    abort();
  }
  if (verboseLevel > 10) {
    G4cout << "MinBiasScintillatorSD DEBUG TileTBID helper retrieved" << G4endl;
  }
  const MbtsDetDescrManager *mbtsMgr(NULL); //!< Pointer to MbtsDetDescrManager
  if (detStore->retrieve(mbtsMgr).isFailure()) {
    G4cout << "MinBiasScintillatorSD WARNING Unable to retrieve MbtsDetDescrManager from DetectorStore" << G4endl;
  }

  for (int side = 0; side < N_SIDE; ++side) {
    for (int eta = 0; eta < N_ETA; ++eta) {
      for (int phi = 0; phi < N_PHI; ++phi) {
        m_channelID[cell_index(side, phi, eta)] = m_tileTBID->channel_id( (side > 0) ? 1 : -1, phi, eta);
      }
      // approximate coordinates
      m_deltaT_cell[dist_index(side, eta)] = sqrt(Z[side] * Z[side] + R[eta] * R[eta]) / CLHEP::c_light;
      if (mbtsMgr) {
        const CaloDetDescrElement *dde = mbtsMgr->get_element(m_channelID[cell_index(side, 0, eta)]);
        // temporary fix for rel 15.6.10
        if (!dde && side == 0) {
          dde = mbtsMgr->get_element(m_tileTBID->channel_id(0, 0, eta));
        }
        if (dde) {
          float x = dde->x();
          float y = dde->y();
          float z = dde->z();
          if (verboseLevel > 10) {
            G4cout << "MinBiasScintillatorSD DEBUG MBTS " << " side = " << ( (side > 0) ? 1 : -1) << " eta = " << eta
                   << " Z = " << z << " R = " << sqrt(x * x + y * y) << G4endl;
          }
          m_deltaT_cell[dist_index(side, eta)] = sqrt(x * x + y * y + z * z) / CLHEP::c_light;
        }
      }
    }
  } //end of loop over side
  if (verboseLevel > 10) {
    G4cout << "MinBiasScintillatorSD DEBUG Distance to MBTS cells (in mm): ";
    for (unsigned int i = 0; i < sizeof (m_deltaT_cell) / sizeof(double); ++i) {
      G4cout << m_deltaT_cell[i] * CLHEP::c_light / CLHEP::mm << ", ";
    }
    G4cout << G4endl;
  }
  return;
}

MinBiasScintillatorSD::~MinBiasScintillatorSD() {
}

void MinBiasScintillatorSD::Initialize(G4HCofThisEvent* /* HCE */) {
  if (verboseLevel > 10) {
    G4cout << "MinBiasScintillatorSD::Initialize()" << G4endl;
  }

  if (!m_HitColl.isValid()) {
    m_HitColl = CxxUtils::make_unique<TileHitVector>(m_HitColl.name());
  }
}

G4bool MinBiasScintillatorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /* ROhist */) {
  if (verboseLevel > 10) {
    G4cout << "MinBiasScintillatorSD::ProcessHits()" << G4endl;
  }

  if ( aStep->GetTotalEnergyDeposit() == 0. ) { //FIXME checking equality with floating point number

    if (verboseLevel > 10) {
      G4cout << "MinBiasScintillatorSD::ProcessHits VERBOSE Edep=0" << G4endl;
    }
    return false;
  }

  const G4StepPoint *preStep = aStep->GetPreStepPoint();
  const G4ThreeVector preStepPointPosition = preStep->GetPosition();
  const G4TouchableHistory *theTouchable = (G4TouchableHistory*)(preStep->GetTouchable());
  const G4VPhysicalVolume *physVol = theTouchable->GetVolume();
  //  G4LogicalVolume *logiVol = physVol->GetLogicalVolume();
  //if (verboseLevel>5) { G4cout << "GlobalTime = " << totalTime << G4endl; }

  const std::string volName = physVol->GetName();
  const std::string volNumber = volName.substr(volName.size() - 1, 1);
  const int side = (preStepPointPosition.z() > 0) ? 1 : 0;
  int phi = physVol->GetCopyNo();// should be in the range [0,nPhi) or 16969 if there is no any copyNo
  // for new geometry copyNo is taken from envelope
  if ( phi > N_PHI )
  phi = theTouchable->GetVolume(3)->GetCopyNo();
  int eta = atoi(volNumber.c_str()) - 1;
  int ind = cell_index(side, phi, eta);
  //if (verboseLevel>5) { G4cout << "R = "
  //    << sqrt(preStepPointPosition.x()*preStepPointPosition.x()+preStepPointPosition.y()*preStepPointPosition.y())
  //    << " Z = " << preStepPointPosition.z()
  //    << " eta = " << eta << G4endl; }

  if (ind >= N_CELLS || ind < 0) {
    G4cout << "MinBiasScintillatorSD::ProcessHits ERROR Hit in "
    << side << "/" << phi << "/" << eta
    << " index=" << ind << " is outside range [0," << N_CELLS - 1 << "]" << G4endl;
    return false;
  }

  // Take into account Birk's saturation law in organic scintillators.
  const G4double edep(BirkLaw(aStep));

  double time(aStep->GetPostStepPoint()->GetGlobalTime());
  // time correction - subtract time of flight to the cell from ATLAS center
  if (m_options.doTOFCorrection) {
    time -= m_deltaT_cell[dist_index(side, eta)];
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
    if ( m_tempSimHit[ind] ) {
      // hit already exists
      if (verboseLevel > 10) {
        G4cout << "MinBiasScintillatorSD::ProcessHits VERBOSE Additional hit in "
        << side << "/" << phi << "/" << eta
        << " energy=" << edep << " time=" << time << G4endl;
      }
      m_tempSimHit[ind]->add(edep, time, m_deltaT);
    } else {
      // First hit in a cell
      if (verboseLevel > 10) {
        G4cout << "MinBiasScintillatorSD::ProcessHits VERBOSE First hit in "
        << side << "/" << phi << "/" << eta
        << " energy=" << edep << " time=" << time << G4endl;
      }
      m_tempSimHit[ind] = new TileSimHit(m_channelID[ind], edep, time, m_deltaT);
      m_numberOfHitsInCell[ind] = 0;
    }
    ++m_numberOfHitsInCell[ind];
  }
  return true;
}

void MinBiasScintillatorSD::EndOfAthenaEvent() {
  //Convert TileSimHits to TileHits and insert into output collection
  for (int ind = 0; ind < N_CELLS; ++ind) {
    if (m_tempSimHit[ind]) {
      if (verboseLevel > 5) {
        G4cout << "MinBiasScintillatorSD::EndOfAthenaEvent DEBUG Cell id=" << m_tileTBID->to_string(m_channelID[ind])
               << " nhit=" << m_numberOfHitsInCell[ind] << " energy=" << m_tempSimHit[ind]->energy() << G4endl;
      }
      m_HitColl->Insert(TileHit(m_tempSimHit[ind]));
      //Clean up as we go through.
      delete m_tempSimHit[ind];
      m_tempSimHit[ind] = nullptr;
    } else {
      if (verboseLevel > 10) {
        G4cout << "MinBiasScintillatorSD::EndOfAthenaEvent VERBOSE Cell id=" << m_tileTBID->to_string(m_channelID[ind])
               << " nhit=0" << G4endl;
      }
    }
  }
  std::vector<int> t1(N_CELLS, 0);
  m_numberOfHitsInCell.swap(t1);
  if (verboseLevel > 5) {
    G4cout << "MinBiasScintillatorSD::EndOfAthenaEvent DEBUG Total number of hits is " << m_HitColl->size() << G4endl;
  }
}

G4double MinBiasScintillatorSD::BirkLaw(const G4Step* aStep) const {
  // Skip the rest of the method if we aren't using Birk's law.
  if (!m_options.doBirk) {
    return aStep->GetTotalEnergyDeposit() * aStep->GetTrack()->GetWeight();
  }

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
  const G4String myMaterial("Scintillator");
  const G4double birk1(0.0130 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2));
  const G4double birk2(9.6e-6 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2));

  const G4double destep(aStep->GetTotalEnergyDeposit() * aStep->GetTrack()->GetWeight());
  //  doesn't work with shower parameterization
  //  G4Material* material = aStep->GetTrack()->GetMaterial();
  //  G4double charge      = aStep->GetTrack()->GetDefinition()->GetPDGCharge();
  const G4Material* material = aStep->GetPreStepPoint()->GetMaterial();
  const G4double charge(aStep->GetPreStepPoint()->GetCharge());

  // --- no saturation law for neutral particles ---
  // ---  and materials other than scintillator  ---
  if ( (charge != 0.) && (material->GetName() == myMaterial)) { //FIXME checking for non-equality for a floating point number

    G4double rkb = birk1;
    // --- correction for particles with more than 1 charge unit ---
    // --- based on alpha particle data (only apply for MODEL=1) ---
    if (fabs(charge) > 1.0) {
      rkb *= 7.2 / 12.6;
    }
    const G4double dedx = destep / (aStep->GetStepLength()) / (material->GetDensity());
    const G4double response = destep / (1. + rkb * dedx + birk2 * dedx * dedx);
    if (-2 == verboseLevel) {
      G4cout << " Destep: " << destep / CLHEP::keV << " keV" << " response after Birk: " << response / CLHEP::keV
             << " keV" << G4endl;
    }
    return response;
  }
  return destep;
}
