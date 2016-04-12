/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MDTDQADB_H
#define MDTDQADB_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string.h>
#include <vector>

class MdtDqaDb  {
  std::string m_chamberName;
  int m_timeFlag, m_deadFlag;
  float m_t0ML1, m_t0ML2, m_tdriftML1, m_tdriftML2;
  float m_chi2ndofML1, m_chi2ndofML2, m_t0errML1, m_t0errML2;
  float m_tdrifterrML1, m_tdrifterrML2;
  int m_numDeadML, m_numDeadMezz, m_numDeadTube;
  int m_numNoisyMezz, m_numNoisyTube;
  std::vector<int> m_listDeadML, m_listDeadMezz, m_listDeadTube; 
  std::vector<std::pair<int,float> > m_listNoisyMezz, m_listNoisyTube;

public:
  MdtDqaDb(std::string chamName);
  MdtDqaDb();
  virtual ~MdtDqaDb();
  void Clear();
  void Print(); //method to print to screen
  void Print(std::ofstream *outfile); //print to file
  void FillDb(/*some db info*/);//method to fill the cool tables
  
  //Set methods
  void SetChamberName(std::string chamName);
  void SetTimeFlag(int timeFlag); //method to update the time flag by hand
  void SetDeadFlag(int deadFlag); //method to update the dead flag by hand
  void SetML1(float t0, float tdrift, float chi2ndof, float t0err, float tdrifterr);
  void SetML2(float t0, float tdrift, float chi2ndof, float t0err, float tdrifterr);
  void SetDeadML(std::vector<int> deadML);
  void SetDeadMezz(std::vector<int> deadMezz);
  void SetDeadTube(std::vector<int> deadTube);
  void SetNoisyMezz(std::vector<std::pair<int,float> > noisyMezz);
  void SetNoisyTube(std::vector<std::pair<int,float> > noisyTube);

  //get methods
  std::string getChamberName();
  int getTimeFlag();
  int getDeadFlag();
  int getNumDeadML();
  int getNumDeadMezz();
  int getNumDeadTube();
  int getNumNoisyMezz();
  int getNumNoisyTube();
  float getT0ML1();
  float getT0ML2();
  float getTdriftML1();
  float getTdriftML2();
  float getChi2ndofML1();
  float getChi2ndofML2();
  float getT0errML1();
  float getT0errML2();
  float getTdrifterrML1();
  float getTdrifterrML2();
  std::vector<int> getDeadML();
  std::vector<int> getDeadMezz();
  std::vector<int> getDeadTube();
  std::vector<std::pair<int,float> > getNoisyMezz();
  std::vector<std::pair<int,float> > getNoisyTube();
};

#endif
