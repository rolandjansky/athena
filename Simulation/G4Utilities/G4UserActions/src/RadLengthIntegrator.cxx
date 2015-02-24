/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/RadLengthIntegrator.h"
#include <string>
#include <map>
#include "SimHelpers/ServiceAccessor.h"

#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ITHistSvc.h"

static RadLengthIntegrator ts1("RadLengthIntegrator");

#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Event.hh"
#include "G4Material.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"

void RadLengthIntegrator::BeginOfEventAction(const G4Event* anEvent)
{
	m_detThick.clear();
	G4PrimaryVertex *vert=anEvent->GetPrimaryVertex(0);
	G4PrimaryParticle *part=vert->GetPrimary();
	G4ThreeVector mom=part->GetMomentum();
	m_etaPrimary=mom.eta();
	m_phiPrimary=mom.phi();
}
void RadLengthIntegrator::EndOfEventAction(const G4Event*)
{
    static ITHistSvc* hSvc=0;
    if (!hSvc) {
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        if (svcLocator->service("THistSvc", hSvc).isFailure()){
            ATH_MSG_ERROR( "Error in retreiving hist svc" );
        }
    }

	for (auto it : m_detThick) {
		if (m_etaMap.find(it.first)!=m_etaMap.end()) {
			m_etaMap[it.first]->Fill(m_etaPrimary,it.second,1.);
			m_phiMap[it.first]->Fill(m_phiPrimary,it.second,1.);
		} else {
            // Eta profile
			TProfile *prof=new TProfile((it.first).c_str(),(it.first).c_str(),1000,-6.,6.);
			if (hSvc && hSvc->regHist("/radLen/"+it.first,prof).isFailure()){
                ATH_MSG_ERROR( "Registration of histogram " << (it.first) << " failed" );
            }
			m_etaMap[it.first]=prof;
			prof->Fill(m_etaPrimary,it.second,1.);

            // Phi profile
            TProfile *profPhi=new TProfile((it.first+"Phi").c_str(),(it.first+"Phi").c_str(),500,-M_PI,M_PI);
            if (hSvc && hSvc->regHist("/radLen/"+it.first+"Phi",profPhi).isFailure()){
                ATH_MSG_ERROR( "Registration of histogram " << (it.first+"Phi") << " failed" );
            }
            m_phiMap[it.first]=profPhi;
            profPhi->Fill(m_phiPrimary,it.second,1.);

		} // First event, or not?
	} // Loop over detectors

	static bool rzRegistered = false;
	if (!rzRegistered){
        if (hSvc && hSvc->regHist("/radLen/RZRadLen",m_rzProf).isFailure()){
            ATH_MSG_ERROR( "Registration of histogram RZRadLen failed" );
        } else {
			rzRegistered = true;
		}
	} // Register the R-Z profile
}

void RadLengthIntegrator::SteppingAction(const G4Step* aStep)
{
	G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
	G4LogicalVolume *lv=touchHist->GetVolume()->GetLogicalVolume();
	std::string volName=lv->GetName();
	G4Material *mat=lv->GetMaterial();
	double radl=mat->GetRadlen();
	double stepl=aStep->GetStepLength();
	double thickstep= radl != 0 ? stepl/radl *100 : DBL_MAX;
	std::string::size_type npos;
	std::string detName;
	npos=volName.find("::");
	if (npos!=std::string::npos)
		detName=volName.substr(0,npos);
	else
		detName="Generic";
	m_detThick[detName] += thickstep;

	// Store the R-Z profile as well
    if (!m_rzProf) m_rzProf = new TProfile2D("RZRadLen","RZRadLen",1000,-25000.,25000.,2000,0.,15000.);
    G4ThreeVector midPoint = (aStep->GetPreStepPoint()->GetPosition()+aStep->GetPostStepPoint()->GetPosition())*0.5;
    m_rzProf->Fill( midPoint.z() , midPoint.perp() , thickstep , 1. );
}
