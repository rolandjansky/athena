/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration  
*/
//***************************************************************************  
//              jFEXForwardJetsInfo - 
//                              -------------------
//     begin                : 07 07 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************  
#include <iostream>
#include <vector>
#include <string>
#include <map>
//#include "L1CaloFEXSim/jFEXForwardJetsAlgo.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

void  LVL1::jFEXForwardJetsInfo::setCentreTTEta(float centreTTEta)
{
  m_centreTTEta = centreTTEta;
}

float LVL1::jFEXForwardJetsInfo::getCentreTTEta()
{
  return m_centreTTEta;
}

void  LVL1::jFEXForwardJetsInfo::setCentreTTPhi(float centreTTPhi)
{
  m_centreTTPhi = centreTTPhi;
}
  
float LVL1::jFEXForwardJetsInfo::getCentreTTPhi()
{
  return m_centreTTPhi;
}
void  LVL1::jFEXForwardJetsInfo::setCentreLocalTTEta(float centreLocalTTEta)
{
  m_centreLocalTTEta = centreLocalTTEta;
}

float LVL1::jFEXForwardJetsInfo::getCentreLocalTTEta()
{
  return m_centreLocalTTEta;
}

void  LVL1::jFEXForwardJetsInfo::setCentreLocalTTPhi(float centreLocalTTPhi)
{
  m_centreLocalTTPhi = centreLocalTTPhi;
}

float LVL1::jFEXForwardJetsInfo::getCentreLocalTTPhi()
{
  return m_centreLocalTTPhi;
}

void LVL1::jFEXForwardJetsInfo::includeTTinSeed(int TT_ID)
{
  m_TTsInSeed.push_back(TT_ID);
}

std::vector<int> LVL1::jFEXForwardJetsInfo::getTTinSeed()
{
  return m_TTsInSeed;
}

void LVL1::jFEXForwardJetsInfo::addToSeedET(int seed_ET)
{
  m_seed_ET += seed_ET;
}

int LVL1::jFEXForwardJetsInfo::getSeedET()
{
  return m_seed_ET;
}

void LVL1::jFEXForwardJetsInfo::addToFirstEnergyRingET(int firstER_ET)
{
  m_firstEnergyRing_ET  = firstER_ET;
}
int LVL1::jFEXForwardJetsInfo::getFirstEnergyRingET()
{
  return m_firstEnergyRing_ET;
}

void LVL1::jFEXForwardJetsInfo::includeTTIDinFirstER(int firstER_TTID)
{
  m_firstEnergyRing_TTIDs.push_back(firstER_TTID);
}
std::vector<int> LVL1::jFEXForwardJetsInfo::getTTIDinFirstER()
{
  return m_firstEnergyRing_TTIDs;
}


void LVL1::jFEXForwardJetsInfo::includeTTIDinSecondER(int secondER_TTID)
{
  m_secondEnergyRing_TTIDs.push_back(secondER_TTID);
}
std::vector<int> LVL1::jFEXForwardJetsInfo::getTTIDinSecondER()
{
  return m_secondEnergyRing_TTIDs;
}

void LVL1::jFEXForwardJetsInfo::addToSecondEnergyRingET(int secondER_ET)
{
  m_secondEnergyRing_ET  = secondER_ET;
}

int LVL1::jFEXForwardJetsInfo::getSecondEnergyRingET()
{
  return m_secondEnergyRing_ET;
}


