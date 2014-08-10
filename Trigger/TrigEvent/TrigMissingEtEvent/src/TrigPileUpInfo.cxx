/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigMissingEtEvent/TrigPileUpInfo.h"

#include <vector>


TrigPileUpInfo::TrigPileUpInfo(){
  clear();
}


void TrigPileUpInfo::clear(){
  m_FJ.clear();
  m_BJ.clear();
  m_PV_Track.clear();
}


int TrigPileUpInfo::getFJ(unsigned int index) const{
  if (index < m_FJ.size())
    return m_FJ.at(index);
  else
    return 0; //<! safe but silent!
}

int TrigPileUpInfo::getBJ(unsigned int index) const{
  if (index < m_BJ.size())
    return m_BJ.at(index);
  else
    return 0; //<! safe but silent!
}

int TrigPileUpInfo::getPV(unsigned int index) const{
  if (index < m_PV_Track.size())
    return m_PV_Track.at(index);
  else
    return 0; //<! safe but silent!
}

std::vector<int> TrigPileUpInfo::getFJVec() const{
  return m_FJ;
}

std::vector<int> TrigPileUpInfo::getBJVec() const{
  return m_BJ;
}

std::vector<int> TrigPileUpInfo::getPVVec() const{
  return m_PV_Track;
}


void TrigPileUpInfo::addFJ(int FJ){
  m_FJ.push_back(FJ);
}

void TrigPileUpInfo::addBJ(int BJ){
  m_BJ.push_back(BJ);
}

void TrigPileUpInfo::addPV(int PV_Track){
  m_PV_Track.push_back(PV_Track);
}
