//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILEPEEKER_FILEMETADATA
#define FILEPEEKER_FILEMETADATA

#include <set>
#include <string>
#include <iostream>
#include <vector>

class FileMetaData {

 public:
  std::string m_fileName;
  std::set<unsigned> m_runNumbers;
  std::set<unsigned> m_lbNumbers;

  std::string m_project;
  std::string m_stream;

  std::string m_condTag;
  std::string m_geoTag;


  std::string m_guid;

  unsigned m_nEvents{0};

  unsigned m_beamEnergy{0};
  std::string m_beamType;

  double m_trtRangeCut{-1.0}; // From /Simulation/Parameters

  bool m_isMC{true};

  bool m_valid{false};

  std::vector<std::string> m_sgKeys;


  void dump(std::ostream& out=std::cout) const;

  void keyValueDump(std::ostream& out=std::cout) const;

};

#endif
