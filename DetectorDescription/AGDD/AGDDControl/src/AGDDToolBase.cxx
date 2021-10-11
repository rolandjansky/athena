/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AGDDControl/AGDDToolBase.h"
#include "AGDDControl/AGDDController.h"

AGDDToolBase::AGDDToolBase(const std::string& type, const std::string& name,
                           const IInterface* parent)
  : base_class(type,name,parent)
{
  ATH_MSG_DEBUG(" trying to get the controller");
}

StatusCode AGDDToolBase::initialize ATLAS_NOT_THREAD_SAFE ()
{
  ATH_CHECK( m_svc.retrieve() );
  InitializeAGDD();
  return StatusCode::SUCCESS;
}

void AGDDToolBase::InitializeAGDD()
{
	ATH_MSG_INFO(" initializing AGDD builder");

        IAGDDtoGeoSvc::LockedController controller = m_svc->getController();
	controller->Locked(m_locked);
	
	ATH_MSG_INFO(" XML file ");
  	for (unsigned int i=0;i<m_xmlFiles.size();i++)
  	{
  		ATH_MSG_INFO(" \t file "<<m_xmlFiles[i]);
		controller->AddFile(m_xmlFiles[i]);		
  	}
  	ATH_MSG_INFO (" ---------");	
	
	
	ATH_MSG_INFO(" Sections ");
  	for (unsigned int i=0;i<m_sectionsToBuild.size();i++)
  	{
  		ATH_MSG_INFO(" \t section "<<m_sectionsToBuild[i]);
		controller->AddSection(m_sectionsToBuild[i]);		
  	}
  	ATH_MSG_INFO (" ---------");
	
	
	ATH_MSG_INFO(" Volumes ");
  	for (unsigned int i=0;i<m_volumesToBuild.size();i++)
  	{
  		ATH_MSG_INFO(" \t volume "<<m_volumesToBuild[i]);
		controller->AddVolume(m_volumesToBuild[i]);		
  	}
  	ATH_MSG_INFO (" ---------");

}
