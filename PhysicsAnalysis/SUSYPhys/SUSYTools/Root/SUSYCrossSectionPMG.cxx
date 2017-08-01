/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SUSYTools/SUSYCrossSectionPMG.h"

SUSY::CrossSectionDBPMG::CrossSectionDBPMG(const std::string& txtfilenameOrDir)
{
  pmgxs.readInfosFromDir(txtfilenameOrDir);
}

void SUSY::CrossSectionDBPMG::loadFile(const std::string& txtfilename)
{
  std::vector<std::string> xs_files;
  xs_files.push_back(txtfilename);
  pmgxs.readInfosFromFiles(xs_files);
}
