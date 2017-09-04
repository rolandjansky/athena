/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYTools/SUSYCrossSectionPMG.h"

SUSY::CrossSectionDBPMG::CrossSectionDBPMG(const std::string& txtfilenameOrDir)
  : m_pmgxs("")
{
  m_pmgxs.setTypeAndName("PMGTools::PMGCrossSectionTool/PMGCrossSectionTool");
  m_pmgxs.retrieve().ignore(); // Ignore the status code
  m_pmgxs->readInfosFromDir(txtfilenameOrDir);
}

void SUSY::CrossSectionDBPMG::loadFile(const std::string& txtfilename)
{
  std::vector<std::string> xs_files;
  xs_files.push_back(txtfilename);
  m_pmgxs->readInfosFromFiles(xs_files);
}
