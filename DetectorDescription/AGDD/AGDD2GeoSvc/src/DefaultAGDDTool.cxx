/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2GeoSvc/DefaultAGDDTool.h"
#include "AGDDControl/AGDDController.h"

DefaultAGDDTool::DefaultAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent):AGDDToolBase(type,name,parent)
{
	
	ATH_MSG_INFO("This is DefaultAGDDTool's constructor!!!! "<<name);
}

StatusCode DefaultAGDDTool::initialize ATLAS_NOT_THREAD_SAFE ()
{
	ATH_MSG_INFO("this is DefaultAGDDTool::initialize()!!!!");
	ATH_MSG_INFO("Default detector "<<m_defaultDetector.value());
	return AGDDToolBase::initialize();
}

StatusCode DefaultAGDDTool::construct ATLAS_NOT_THREAD_SAFE () 
{
	ATH_MSG_INFO("this is DefaultAGDDTool::construct()!!!!");
	ATH_MSG_INFO(" Name = "<<name());
	
	ATH_MSG_INFO(" trying to parse files ");
        IAGDDtoGeoSvc::LockedController controller = m_svc->getController();
	controller->ParseFiles();
	
	if (m_printSections) 
	{
		ATH_MSG_INFO(" \tPrinting all Sections");
		controller->PrintSections();
	}
	
	if (!m_defaultDetector.empty())
    {
		ATH_MSG_INFO(" setting default detector to "<<m_defaultDetector.value());
	   	controller->UseGeoModelDetector(m_defaultDetector.value());
	}
	
	controller->BuildAll();
	
	controller->Clean();
	
	return StatusCode::SUCCESS;
}


