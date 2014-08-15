/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VertexTimeOffset/VertexTimeOffset.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/BuiltinsClids.h"

#include "G4PrimaryVertex.hh"
#include "CLHEP/Units/PhysicalConstants.h"

static VertexTimeOffset vt("VertexTimeOffset");

bool VertexTimeOffset::EditVertex(G4PrimaryVertex *v)
{
	static int iSGfirst=0;
	StatusCode st;
	static StoreGateSvc* storeGate=0;
	if (!iSGfirst)
	{
		if (verboseLevel>0) std::cout <<"trying to get a handle on StoreGate"<<std::endl;
  		ISvcLocator* svcLocator = Gaudi::svcLocator();
  		st=svcLocator->service("StoreGateSvc", storeGate);
  		if (st.isFailure())
			std::cout<<" storeGateSvc(); could not access StoreGateSvc"<<std::endl;
		else
			if (verboseLevel>0) std::cout <<" got StoreGate!"<<std::endl;
		iSGfirst++;
	}
	
	static double tVert=0;
	if (isFirstVertex)
	{
		if (verboseLevel>0) std::cout <<" trying to get the vertex time "<<std::endl;
		isFirstVertex=false;

		DataHandle<double> p_vTime;
		if ((st=storeGate->retrieve(p_vTime,"PrimarySimEventTime"))==StatusCode::SUCCESS)
		{
			if (verboseLevel>0) std::cout <<" got the time: "<< (*p_vTime) <<" ns"<<std::endl;
			tVert = (*p_vTime);
		}
		else 
			std::cout<<" could not retrieve primary event time!!! "<<std::endl;
  	}
//  		if (verboseLevel > 0) 
//    	std::cout<<" VertexTimeOffset::EditVertex: vertex at "<<zVert<<" mm"<<std::endl;
		if (verboseLevel>0) std::cout <<"setting vertex time to "<<tVert<<std::endl;		
		v->SetT0(tVert*CLHEP::ns);
	
    return true;

}

void VertexTimeOffset::EventInitialization()
{
	isFirstVertex=true;
}
