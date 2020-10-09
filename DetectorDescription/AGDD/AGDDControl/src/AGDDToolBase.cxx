/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDDToolBase.h"
#include "AGDDControl/AGDDController.h"

AGDDToolBase::AGDDToolBase(const std::string& type, const std::string& name,
                           const IInterface* parent)
  : base_class(type,name,parent)
{
  ATH_MSG_DEBUG(" trying to get the controller");
  m_controller = AGDDController::GetController();
}

StatusCode AGDDToolBase::initialize()
{
  InitializeAGDD();
  return StatusCode::SUCCESS;
}

void AGDDToolBase::InitializeAGDD()
{
	ATH_MSG_INFO(" initializing AGDD builder");
	
	m_controller->Locked(m_locked);
	
	ATH_MSG_INFO(" XML file ");
  	for (unsigned int i=0;i<m_xmlFiles.size();i++)
  	{
  		ATH_MSG_INFO(" \t file "<<m_xmlFiles[i]);
		m_controller->AddFile(m_xmlFiles[i]);		
  	}
  	ATH_MSG_INFO (" ---------");	
	
	
	ATH_MSG_INFO(" Sections ");
  	for (unsigned int i=0;i<m_sectionsToBuild.size();i++)
  	{
  		ATH_MSG_INFO(" \t section "<<m_sectionsToBuild[i]);
		m_controller->AddSection(m_sectionsToBuild[i]);		
  	}
  	ATH_MSG_INFO (" ---------");
	
	
	ATH_MSG_INFO(" Volumes ");
  	for (unsigned int i=0;i<m_volumesToBuild.size();i++)
  	{
  		ATH_MSG_INFO(" \t volume "<<m_volumesToBuild[i]);
		m_controller->AddVolume(m_volumesToBuild[i]);		
  	}
  	ATH_MSG_INFO (" ---------");

}
