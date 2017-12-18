/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/NSWAGDDTool.h"
#include "MuonAGDD/MuonAGDDToolHelper.h"
#include "AGDDControl/AGDDController.h"
#include "AGDD2GeoSvc/IAGDD2GeoSvc.h"

#include "AGDDModel/AGDDParameterStore.h"
#include "AGDDKernel/AGDDDetector.h"
#include "AGDDKernel/AGDDDetectorStore.h"




using namespace MuonGM;


NSWAGDDTool::NSWAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent):AGDDToolBase(type,name,parent)
{
  //std::cout<<"This is NSWAGDDTool's constructor!!!! "<<name<<std::endl;
}

StatusCode NSWAGDDTool::initialize()
{
	ATH_MSG_INFO("this is NSWAGDDTool::initialize()!!!!");

	StatusCode result;
	result = AGDDToolBase::initialize();
	if (result.isFailure())
    {
    	ATH_MSG_FATAL("could not initialize AGDDToolBase!!! ");
    }
	
	static int iEntries=0;
	
	if (!iEntries) 
	{
		iEntries=1;
		MuonAGDDToolHelper theHelper;
		theHelper.SetNSWComponents();
	}

	return result;
}

StatusCode NSWAGDDTool::construct() 
{
	ATH_MSG_INFO("this is NSWAGDDTool::construct()!!!!");
	ATH_MSG_INFO(" Name = "<<name());
	
	ATH_MSG_INFO(" trying to parse files ");
	m_controller->ParseFiles();
	
	if (m_printSections) 
	{
		ATH_MSG_INFO("\t Printing all sections ");
		m_controller->PrintSections();
	}
	
	m_controller->UseGeoModelDetector("Muon");
	
	m_controller->BuildAll();
	
	// part needed to build the NSW RO geometry
	
	ATH_MSG_INFO("\t Building NSW Readout Geometry ");
	MuonAGDDToolHelper theHelper;
	bool testRet=theHelper.BuildMScomponents();
	if (!testRet) ATH_MSG_ERROR("something went wrong building the RO geometry!!! ");
	
	m_controller->Clean();
	
	return StatusCode::SUCCESS;
}





