/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonAGDD/MuonAGDDTool.h"
#include "MuonAGDD/MuonAGDDToolHelper.h"

#include "AGDDControl/AGDDController.h"
#include "AGDDControl/AGDD2GeoModelBuilder.h"
#include "AGDDKernel/AliasStore.h"

using namespace MuonGM;

MuonAGDDTool::MuonAGDDTool(const std::string& type, const std::string& name, 
				 const IInterface* parent):AGDDToolBase(type,name,parent),
    				m_readAGDD(true),m_locked(false),m_dumpAGDD(false),
					m_overrideConfiguration(false),m_buildNSW(true)
{
  	declareProperty( "Structures" ,     m_structuresToBuild);
  	declareProperty( "ReadAGDD",   		m_readAGDD);
  	declareProperty( "DumpAGDD",		m_dumpAGDD);
  	declareProperty( "OverrideConfiguration",m_overrideConfiguration=false);
	declareProperty( "BuildNSW",		m_buildNSW);
}

StatusCode MuonAGDDTool::initialize()
{
	StatusCode result;
	result = AGDDToolBase::initialize();
	if (result.isFailure())
    {
    	ATH_MSG_FATAL("could not initialize AGDDToolBase!!! ");
    }
	
	if (m_buildNSW) 
	{
		MuonAGDDToolHelper theHelper;
		theHelper.SetNSWComponents();
	}
	
	return result;
}

StatusCode MuonAGDDTool::construct() 
{
	MuonAGDDToolHelper theHelper;
		
	m_controller->UseGeoModelDetector("Muon");
	
	if (!m_locked)
	{
		ATH_MSG_INFO(" Reading AGDD2GeoSwitches flags ");
		m_structuresFromFlags=theHelper.ReadAGDDFlags();
		for (unsigned int i=0;i<m_structuresFromFlags.size();i++)
			ATH_MSG_INFO(" ----> "<<m_structuresFromFlags[i]);
	}
	
	if (!m_readAGDD)
	{
		ATH_MSG_INFO(" trying to parse files ");
		m_controller->ParseFiles();
		
		m_controller->BuildAll();
		m_controller->Clean();

		return StatusCode::SUCCESS;
	}
	
	ATH_MSG_INFO(" now reading AGDD blob ");
	
	std::string AGDDfile=theHelper.GetAGDD(m_dumpAGDD);
	
	m_controller->ParseString(AGDDfile);
	
	if (m_printSections)
	{	
		ATH_MSG_INFO("\t Printing all sections");
		m_controller->PrintSections();
	}
	
	
	ATH_MSG_INFO(" now dumping the flags ");
	for (unsigned int i =0;i<m_structuresFromFlags.size();i++)
	{
		ATH_MSG_INFO("\t\t----- "<<m_structuresFromFlags[i]<<" "<<ALIAS(m_structuresFromFlags[i]));
		if (!m_buildNSW && m_structuresFromFlags[i]=="NewSmallWheel") continue;
		m_controller->GetBuilder()->BuildFromVolume(m_structuresFromFlags[i]);
	}

	m_controller->Clean();

	return StatusCode::SUCCESS;
}

