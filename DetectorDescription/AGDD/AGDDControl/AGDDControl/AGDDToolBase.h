/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDToolBase_H
#define AGDDToolBase_H

#include "AGDDControl/IAGDDToolBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

class IInterface;
class AGDDController;

class AGDDToolBase: public AthAlgTool, virtual public IAGDDToolBase
{
public:
	AGDDToolBase(const std::string& type, const std::string& name, 
				 const IInterface* parent);
	virtual StatusCode initialize();
	virtual StatusCode construct() {return StatusCode::SUCCESS;}
protected:
	std::vector<std::string> m_xmlFiles;
	std::vector<std::string> m_sectionsToBuild;
	std::vector<std::string> m_volumesToBuild;
	int m_parserVerbosity;
	int m_builderVerbosity;
	bool m_printSections;
	bool m_disableSections;
	bool m_locked;
	std::string m_defaultDetector;
	std::string m_navigateDetector;
	
	void InitializeAGDD();
	
	AGDDController* m_controller;
};

#endif
