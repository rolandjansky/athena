/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDD2GeoSvc/DefaultAGDDTool.h"
#include "AGDDControl/AGDDController.h"

DefaultAGDDTool::DefaultAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent):AGDDToolBase(type,name,parent)
{
	
	ATH_MSG_INFO("This is DefaultAGDDTool's constructor!!!! "<<name);
}

StatusCode DefaultAGDDTool::initialize()
{
	ATH_MSG_INFO("this is DefaultAGDDTool::initialize()!!!!");
	ATH_MSG_INFO("Default detector "<<m_defaultDetector);
	return AGDDToolBase::initialize();
}

StatusCode DefaultAGDDTool::construct() 
{
	ATH_MSG_INFO("this is DefaultAGDDTool::construct()!!!!");
	ATH_MSG_INFO(" Name = "<<name());
	
	ATH_MSG_INFO(" trying to parse files ");
	controller->ParseFiles();
	
	if (m_printSections) 
	{
		ATH_MSG_INFO(" \tPrinting all Sections");
		controller->PrintSections();
	}
	
	if (!m_defaultDetector.empty())
    {
		ATH_MSG_INFO(" setting default detector to "<<m_defaultDetector);
	   	controller->UseGeoModelDetector(m_defaultDetector);
	}
	
	controller->BuildAll();
	
	controller->Clean();
	
	return StatusCode::SUCCESS;
}


