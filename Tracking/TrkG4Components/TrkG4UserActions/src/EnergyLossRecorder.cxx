/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EnergyLossRecorder.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkG4UserActions/EnergyLossRecorder.h"
#include <iostream>
#include "G4Step.hh"
//#include "G4Material.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
// TrkValInterfaces
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkValInterfaces/IPositionMomentumWriter.h"


static EnergyLossRecorder sntp("EnergyLossRecorder");

EnergyLossRecorder::EnergyLossRecorder(std::string s)
: FADS::UserAction(s),
   m_pmWriter("Trk::PositionMomentumWriter/PosMomWriter"),
   m_entries(0),
   m_retrieved(false)
{}


void EnergyLossRecorder::BeginOfEventAction(const G4Event*)
{
    if (!m_retrieved && !m_pmWriter.empty() && m_pmWriter.retrieve().isFailure())
        std::cout << "[EnergyLossRecorder] BeginOfRunAction - problem retrieving the " << m_pmWriter << std::endl;
    else 
        m_retrieved = true;
}

void EnergyLossRecorder::EndOfEventAction(const G4Event*)
{
    if (!m_pmWriter.empty())
        m_pmWriter->finalizeTrack();   
    m_entries = 0;
}

void EnergyLossRecorder::BeginOfRunAction(const G4Run*)
{


}

void EnergyLossRecorder::EndOfRunAction(const G4Run*)
{}

void EnergyLossRecorder::SteppingAction(const G4Step* aStep)
{

    // kill secondary particles
    if (aStep->GetTrack()->GetParentID()) {
        aStep->GetTrack()->SetTrackStatus(fStopAndKill);
        return;
    }
    // we require a minimum amount of material for recording the step
    
    // the material information
    G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
    // G4LogicalVolume
    G4LogicalVolume *lv= touchHist ? touchHist->GetVolume()->GetLogicalVolume() : 0;
    G4Material *mat    = lv ? lv->GetMaterial() : 0;
    	
    // log the information // cut off air
    if (mat && mat->GetRadlen() < 200000.) {
        // keep primary particles - calculate the kinematics for them
        G4ThreeVector pos   = aStep->GetPreStepPoint()->GetPosition();
        double px = aStep->GetPreStepPoint()->GetMomentum().x();
        double py = aStep->GetPreStepPoint()->GetMomentum().y();
        double pz = aStep->GetPreStepPoint()->GetMomentum().z();
        Amg::Vector3D position(pos.x(),pos.y(),pos.z());
        Amg::Vector3D momentum(px ,py, pz);
        
        
        // record the starting parameters at the first step
        if (m_entries==0) {
            // increase the counter
            ++m_entries;
            double  m   = aStep->GetTrack()->GetDynamicParticle()->GetMass();
            int pdgCode = aStep->GetTrack()->GetDynamicParticle()->GetPDGcode();
            m_pmWriter->initializeTrack(position,momentum,m,pdgCode);
        } else 
            m_pmWriter->recordTrackState(position,momentum);
    }
}

