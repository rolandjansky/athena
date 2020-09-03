/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CaloUtils/CaloSamplingHelper.h"
#include <algorithm>

const std::map<std::string,CaloSampling::CaloSample> CaloSamplingHelper::m_lookUp{
  {"PreSamplerB",  CaloSampling::PreSamplerB},
  {"EMB1",         CaloSampling::EMB1},
  {"EMB2",         CaloSampling::EMB2},
  {"EMB3",         CaloSampling::EMB3},
  {"PreSamplerE",  CaloSampling::PreSamplerE},
  {"EME1",         CaloSampling::EME1},
  {"EME2",         CaloSampling::EME2},
  {"EME3",         CaloSampling::EME3},
  {"HEC0",         CaloSampling::HEC0},
  {"HEC1",         CaloSampling::HEC1},
  {"HEC2",         CaloSampling::HEC2},
  {"HEC3",         CaloSampling::HEC3},
  {"TileBar0",     CaloSampling::TileBar0},
  {"TileBar1",     CaloSampling::TileBar1},
  {"TileBar2",     CaloSampling::TileBar2},
  {"TileGap1",     CaloSampling::TileGap1},
  {"TileGap2",     CaloSampling::TileGap2},
  {"TileGap3",     CaloSampling::TileGap3},
  {"TileExt0",     CaloSampling::TileExt0},
  {"TileExt1",     CaloSampling::TileExt1},
  {"TileExt2",     CaloSampling::TileExt2},
  {"FCal1",        CaloSampling::FCAL0},
  {"FCal2",        CaloSampling::FCAL1},
  {"FCal3",        CaloSampling::FCAL2},
  {"MiniFCal0",    CaloSampling::MINIFCAL0},
  {"MiniFCal1",    CaloSampling::MINIFCAL1},
  {"MiniFCal2",    CaloSampling::MINIFCAL2},
  {"MiniFCal3",    CaloSampling::MINIFCAL3}
};

const std::string CaloSamplingHelper::m_unknown="Unknown";

const std::string& CaloSamplingHelper::getSamplingName(const CaloSampling::CaloSample theSample) {
  //Linear search for the value in the std::map m_lookUp. 
  //Slow, but acceptable because: 
  //  1. The map has only 28 entries 
  //  2. It's and integer-comparision 
  //  3. The method is only use for log-output (eg not too often)

  //Implementation uses a C++11 lambda function
  //[&theSample] defines the lambda function and says that it depends on the external variable theSampling (captured by reference)
  //(std::pair<std::string,CaloSampling::CaloSample> i) is the parameter of the function
  //{return (i.second==theSample);} Is the code of the function
  auto it=std::find_if(m_lookUp.begin(),m_lookUp.end(),[&theSample](const std::pair<std::string,CaloSampling::CaloSample>& i) {return (i.second==theSample);});
  if (it==m_lookUp.end())
    return m_unknown;
  else
    return it->first;
}

CaloSampling::CaloSample 
CaloSamplingHelper::getSamplingId(const std::string& name)
{
  return m_lookUp.find(name) != m_lookUp.end()
    ? (*m_lookUp.find(name)).second
    : CaloSampling::Unknown;
}


bool CaloSamplingHelper::getSamplings(const std::vector<CaloCell_ID::SUBCALO>& theCalos,
				      std::vector<CaloSampling::CaloSample>& theSamplings) {

  const size_t oldSize = theSamplings.size();
  for (auto fCalo : theCalos) {
    switch (fCalo)
      {
      case CaloCell_ID::LAREM:
	theSamplings.push_back(CaloSampling::PreSamplerB);
	theSamplings.push_back(CaloSampling::EMB1);
	theSamplings.push_back(CaloSampling::EMB2);
	theSamplings.push_back(CaloSampling::EMB3);
	theSamplings.push_back(CaloSampling::PreSamplerE);
	theSamplings.push_back(CaloSampling::EME1);
	theSamplings.push_back(CaloSampling::EME2);
	theSamplings.push_back(CaloSampling::EME3);
	break;
      case CaloCell_ID::LARHEC:
	theSamplings.push_back(CaloSampling::HEC0);
	theSamplings.push_back(CaloSampling::HEC1);
	theSamplings.push_back(CaloSampling::HEC2);
	theSamplings.push_back(CaloSampling::HEC3);
	break;
      case CaloCell_ID::LARFCAL:
	theSamplings.push_back(CaloSampling::FCAL0);
	theSamplings.push_back(CaloSampling::FCAL1);
	theSamplings.push_back(CaloSampling::FCAL2);
	break;
      case CaloCell_ID::TILE:
	theSamplings.push_back(CaloSampling::TileBar0);
	theSamplings.push_back(CaloSampling::TileBar1);
	theSamplings.push_back(CaloSampling::TileBar2);
	theSamplings.push_back(CaloSampling::TileExt0);
	theSamplings.push_back(CaloSampling::TileExt1);
	theSamplings.push_back(CaloSampling::TileExt2);
	theSamplings.push_back(CaloSampling::TileGap1);
	theSamplings.push_back(CaloSampling::TileGap2);
	theSamplings.push_back(CaloSampling::TileGap3);
	break;
      case CaloCell_ID::LARMINIFCAL:
	theSamplings.push_back(CaloSampling::MINIFCAL0);
	theSamplings.push_back(CaloSampling::MINIFCAL1);
	theSamplings.push_back(CaloSampling::MINIFCAL2);
	theSamplings.push_back(CaloSampling::MINIFCAL3);
	break;
      default:
	break;
      }
  }
  return oldSize < theSamplings.size();
}


bool CaloSamplingHelper::getSamplings(const CaloCell_ID::SUBCALO& theCalo,
					     std::vector<CaloSampling::CaloSample>& theSamplings) {
  const std::vector<CaloCell_ID::SUBCALO> caloVector={theCalo};
  return getSamplings(caloVector,theSamplings);
}


bool CaloSamplingHelper::getCalos(const std::vector<CaloSampling::CaloSample>& theSamplings,
				  std::vector<CaloCell_ID::SUBCALO>& theCalos) {
  const size_t oldSize = theCalos.size();
  for (auto fSample : theSamplings) {
    CaloCell_ID::SUBCALO theCaloId = CaloCell_ID::NOT_VALID;
    switch ( fSample ) {
    case CaloSampling::PreSamplerB:
    case CaloSampling::EMB1:
    case CaloSampling::EMB2:
    case CaloSampling::EMB3:
    case CaloSampling::PreSamplerE:
    case CaloSampling::EME1:
    case CaloSampling::EME2:
    case CaloSampling::EME3:
      theCaloId = CaloCell_ID::LAREM;
      break;
    case CaloSampling::HEC0:
    case CaloSampling::HEC1:
    case CaloSampling::HEC2:
    case CaloSampling::HEC3:
      theCaloId = CaloCell_ID::LARHEC;
      break;
    case CaloSampling::FCAL0:
    case CaloSampling::FCAL1:
    case CaloSampling::FCAL2:
      theCaloId = CaloCell_ID::LARFCAL;
      break;
    case CaloSampling::TileBar0:
    case CaloSampling::TileBar1:
    case CaloSampling::TileBar2:
    case CaloSampling::TileGap1:
    case CaloSampling::TileGap2:
    case CaloSampling::TileGap3:
    case CaloSampling::TileExt0:
    case CaloSampling::TileExt1:
    case CaloSampling::TileExt2:
      theCaloId = CaloCell_ID::TILE;
      break;
    case CaloSampling::MINIFCAL0:
    case CaloSampling::MINIFCAL1:
    case CaloSampling::MINIFCAL2:
    case CaloSampling::MINIFCAL3:
      theCaloId = CaloCell_ID::LARMINIFCAL;
      break;
    default:
      break;
    }
    // check if not there yet (actually a std::set would be better than a vector)
    if ( std::find(theCalos.begin(),theCalos.end(),theCaloId) == theCalos.end()){
      theCalos.push_back(theCaloId);
    }
  }
  return theCalos.size() > oldSize;
}
