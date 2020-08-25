/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDCONTROL_AGDDToolBase_H
#define AGDDCONTROL_AGDDToolBase_H

#include "AGDDControl/IAGDDToolBase.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/StatusCode.h"

class AGDDController;

class AGDDToolBase: public extends<AthAlgTool, IAGDDToolBase>
{
public:
  AGDDToolBase(const std::string& type, const std::string& name,
               const IInterface* parent);
  virtual StatusCode initialize();
  virtual StatusCode construct() {return StatusCode::SUCCESS;}
protected:
  void InitializeAGDD();

  Gaudi::Property<std::vector<std::string> > m_xmlFiles{this, "XMLFiles", {} };
  Gaudi::Property<std::vector<std::string> > m_sectionsToBuild{this, "Sections", {} };
  Gaudi::Property<std::vector<std::string> > m_volumesToBuild{this, "Volumes", {} };
  Gaudi::Property<int> m_parserVerbosity{this, "ParserVerbosity", 0};
  Gaudi::Property<int> m_builderVerbosity{this, "BuilderVerbosity", 0};
  Gaudi::Property<bool> m_printSections{this, "PrintSections", false};
  Gaudi::Property<bool> m_disableSections{this, "DisableSections", true};
  Gaudi::Property<bool> m_locked{this, "Locked", false};
  Gaudi::Property<bool> m_writeDBfile{this, "WriteAGDDFile", false};
  Gaudi::Property<std::string> m_outFileName{this, "OutAGDDXMLName", "mytest.xml"};
  Gaudi::Property<std::string> m_defaultDetector{this, "DefaultDetector", "NoDetector"};
  Gaudi::Property<std::string> m_navigateDetector{this, "NavigateDetector", ""};
  AGDDController* m_controller {};
};

#endif
