/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/RadLengthIntegrator.h"
#include <iostream>
#include <iostream>
#include <string>
#include <map>
#include <cfloat>
#include "SimHelpers/ServiceAccessor.h"

#include <TH1.h>
#include <TProfile.h>

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
	detThick.clear();
	G4PrimaryVertex *vert=anEvent->GetPrimaryVertex(0);
	G4PrimaryParticle *part=vert->GetPrimary();
	G4ThreeVector mom=part->GetMomentum();
	etaPrimary=mom.eta();
	phiPrimary=mom.phi();
}
void RadLengthIntegrator::EndOfEventAction(const G4Event*)
{

    static ITHistSvc* hSvc=0;
    if (!hSvc)
    {
        ISvcLocator* svcLocator = Gaudi::svcLocator();
        if (svcLocator->service("THistSvc", hSvc).isFailure()){
            ATH_MSG_ERROR( "Error in retreiving hist svc" );
        }
    }

	std::map<std::string,double,std::less<std::string> >::const_iterator it;
	for (it=detThick.begin();it!=detThick.end();it++)
	{
		if (histoMap.find((*it).first)!=histoMap.end())
		{
			histoMap[(*it).first]->Fill(etaPrimary,(*it).second,1.);
		}
		else
		{
			TProfile *prof=new TProfile(((*it).first).c_str(),((*it).first).c_str(),500,-3.,3.);
			if (hSvc){
                if (hSvc->regHist("/radLen/"+(*it).first,prof).isFailure()){
                    ATH_MSG_ERROR( "Registration of histogram " << ((*it).first) << " failed" );
                }
            }
			histoMap[(*it).first]=prof;
			prof->Fill(etaPrimary,(*it).second,1.);

		}
	}

}
void RadLengthIntegrator::BeginOfRunAction(const G4Run*)
{
}

void RadLengthIntegrator::EndOfRunAction(const G4Run*)
{
}

void RadLengthIntegrator::SteppingAction(const G4Step* aStep)
{
//	G4StepPoint *preStep=aStep->GetPreStepPoint();
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
	detThick[detName] += thickstep;
}
