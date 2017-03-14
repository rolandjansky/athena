/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
//  ReadFromText.cpp
//  HDef
//
//  Created by sroe on 16/07/2015.
//  2015 sroe
//

#include "ReadFromText.h"
#include <fstream>
#include <iostream>
// #include <regex> //buggy??
#include <sys/stat.h>
#include <algorithm>
// to find the file:
#include "PathResolver/PathResolver.h"
namespace {
  std::vector<std::string>
  split(const std::string& str, int delimiter(int) = std::isspace) {
    std::vector<std::string> result;
    auto e = str.end();
    auto i = str.begin();

    while (i != e) {
      i = std::find_if_not(i, e, delimiter);
      if (i == e) {
        break;
      }
      auto j = std::find_if(i, e, delimiter);
      result.push_back(std::string(i, j));
      i = j;
    }
    return result;
  }
}


ReadFromText::ReadFromText() : m_source("unspecified file"), m_format("text/plain") {
}

ReadFromText::ReadFromText(const std::string& source) : m_source(source), m_format("text/plain") {
  m_source = PathResolver::find_file(source, "DATAPATH");
}

std::string
ReadFromText::source() const {
  return m_source;
}

std::string
ReadFromText::format() const {
  return m_format;
}

bool
ReadFromText::histoDefinitionMap(std::map<std::string, SingleHistogramDefinition>& usersmap) const {
  bool ok(true);

  for (auto i:m_vectorOfDefinitions) {
    if (i.empty()) {
      continue;
    }
    if (not i.validType()) {
      continue;
    }
    ok = (usersmap.insert(std::pair<std::string, SingleHistogramDefinition>(i.stringIndex(), i))).second;
    if (not ok) {
      std::cout << "ReadFromText: bad insertion for " << i.stringIndex() << std::endl;
    }
  }
  return(not usersmap.empty());
}

bool
ReadFromText::insertDefinition(const SingleHistogramDefinition& oneDefinition) {
  bool ok(true);

  m_vectorOfDefinitions.push_back(oneDefinition);
  return ok;
}

bool
ReadFromText::initialize() {
  bool ok(true);

  std::ifstream myfile;

  myfile.open(source());
  if (myfile.is_open()) {
    std::string line;
    while (std::getline(myfile, line)) {
      ok = insertDefinition(parseTextLine(line));
    }
    myfile.close();
  } else {
    std::cout << "ReadFromText: File '" << source() << "' not found" << std::endl;
    ok = false;
  }

  return ok;
}

SingleHistogramDefinition
ReadFromText::parseTextLine(const std::string& line) {
  SingleHistogramDefinition s;

  typedef std::vector<std::string> StringVec;
  enum RegXHistoGroups {
    TOTAL, TYPE, NAME, TITLE, NXBINS, XLO, XHI, NYBINS, YLO, YHI, XLABEL, YLABEL, FOLDER, NGROUPS
  };
  enum HistoType {
    TH1, TPROFILE, TH2, INVALID
  };
  // bool found(true);
  /** use lamda expression for delimiter function, get largest chunks between quotes (")
     TH1 case:
     [0] TH1F    002_Al_N_01
     [1] Visible E_{T} (GeV)
     [2] 100       0     100
     [3] Visible E_{T} (GeV)
     [4]          <empty>
     [5] Y axis label
     [6] myfolder

     TProfile
     [0] TProfile    Identifier
     [1] "title of this histogram"
     [2] xbins xlo xhi  ylo yhi
     [3] "x axis title"
     [4]          <empty>
     [5] "y axis title"
     [6] myfolder

     TH2 case:
     [0] TH2F    2D_Identifier
     [1] "title of this histogram"
     [2] xbins xlo xhi ybins ylo yhi
     [3] "x axis title"
     [4]          <empty>
     [5] "y axis title"
     [6] myfolder
   **/
  HistoType hType(TH1);
  // todo: should make an array of functions and delegate
  StringVec m(NGROUPS);
  m[TOTAL] = line;
  StringVec chunks = split(line, [](int d) -> int {
    return d == (int) '"';
  });
  const int minimumPossibleSize(3);
  if (chunks.size() < minimumPossibleSize) {
    return s;
  }
  StringVec typeAndName = split(chunks[0]);
  m[TYPE] = typeAndName[0];
  m[NAME] = typeAndName[1];
  m[TITLE] = chunks[1];
  StringVec nBinsStartEnd = split(chunks[2]);
  std::cout << "chunks[2]: '" << chunks[2] << "'" << std::endl;
  if (nBinsStartEnd.size() < 3) {
    return s;
  }
  m[NXBINS] = nBinsStartEnd[0];
  m[XLO] = nBinsStartEnd[1];
  m[XHI] = nBinsStartEnd[2];
  if (nBinsStartEnd.size() == 5) {      // TProfile; limits given for Y axis also
    hType = TPROFILE;
    m[NYBINS] = "";
    m[YLO] = nBinsStartEnd[3];
    m[YHI] = nBinsStartEnd[4];
  }
  if (nBinsStartEnd.size() == 6) {      // 2D histogram; limits given for Y axis also
    hType = TH2;
    m[NYBINS] = nBinsStartEnd[3];
    m[YLO] = nBinsStartEnd[4];
    m[YHI] = nBinsStartEnd[5];
  }
  m[XLABEL] = chunks[3];
  // chunks[4] is the space between two quoted strings
  m[YLABEL] = chunks[5];
  std::string str = chunks[6];
  // remove all spaces
  str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
  m[FOLDER] = str;
  /**
     for (const auto &s:m){
     if (s.empty()) std::cout<<"ReadFromText: Problem parsing this line: "<<line<<std::endl;
     found&=!(s.empty());
     }**/
  unsigned int nbinsX(0);
  float xLo(std::nanf(""));
  float xHi(std::nanf(""));
  try{
    nbinsX = (unsigned int) (std::stoul(m[NXBINS]));
    xLo = std::stof(m[XLO]);
    xHi = std::stof(m[XHI]);
  } catch (std::invalid_argument& e) {
    std::cout << "Problem converting some of the arguments : " << m[NXBINS] << "; " << m[XLO] << "; " << m[XHI] <<
      std::endl;
    hType = INVALID;
  }

  switch (hType) {
  case TH1:
  {
    // SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, NBins_t nbinsX,Var_t xLo, Var_t
    // xHi,Titles_t xName);
    SingleHistogramDefinition sx(m[NAME], m[TYPE], m[TITLE], nbinsX, xLo, xHi, m[XLABEL], m[YLABEL], m[FOLDER]);
    return sx;

    break;
  }

  case TPROFILE:
  {
    const unsigned int nBinsY = 0;
    const float yLo(std::stof(m[YLO]));
    const float yHi(std::stof(m[YHI]));
    // SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, NBins_t nbinsX,  NBins_t
    // nbinsY,Var_t xLo, Var_t xHi, Var_t yLo, Var_t yHi, Titles_t xName, Titles_t yName, Titles_t theFolder="");
    SingleHistogramDefinition sx(m[NAME], m[TYPE], m[TITLE], nbinsX, nBinsY, xLo, xHi, yLo, yHi, m[XLABEL], m[YLABEL],
                                 m[FOLDER]);
    return sx;

    break;
  }

  case  TH2:
  {
    const unsigned int nBinsY(std::stof(m[NYBINS]));
    const float yLo(std::stof(m[YLO]));
    const float yHi(std::stof(m[YHI]));
    // SingleHistogramDefinition(Titles_t name, Titles_t histoType, Titles_t title, NBins_t nbinsX,  NBins_t
    // nbinsY,Var_t xLo, Var_t xHi, Var_t yLo, Var_t yHi, Titles_t xName, Titles_t yName, Titles_t theFolder="");
    SingleHistogramDefinition sx(m[NAME], m[TYPE], m[TITLE], nbinsX, nBinsY, xLo, xHi, yLo, yHi, m[XLABEL], m[YLABEL],
                                 m[FOLDER]);
    return sx;

    break;
  }

  default:
  {
    std::cout << "ReadFromText.cxx: Possible problem in parsing this line: \n" << line << "\nof file " << source() <<
      std::endl;
    return SingleHistogramDefinition();
  }
  }
}

bool
ReadFromText::sourceExists() const {
  struct stat buffer;

  return(stat(m_source.c_str(), &buffer) == 0);
}
