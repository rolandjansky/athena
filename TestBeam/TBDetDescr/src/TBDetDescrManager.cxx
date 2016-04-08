/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//========================================
// file TBDetDescrManager.cxx
// 
//========================================

#include "GaudiKernel/MsgStream.h"

#include "TBDetDescr/TBDetDescrManager.h"
#include "TBDetDescr/TBElementContainer.h"


TBDetDescrManager::TBDetDescrManager ( )
  : m_elements(0)
{
  m_TBElementVec.resize(TBElementID::TotalSize);

  // instead of creating yet another vector with all the names
  // for all elements in CTB simulation, let's keep them in the same vector
  // these names are needed only when we initialize manager from FADS

  m_TBElementVec[TBElementID::Unknown                ].m_name = "";
  m_TBElementVec[TBElementID::BEAMPIPE1              ].m_name = "BEAMPIPE1";
  m_TBElementVec[TBElementID::BEAMPIPE2              ].m_name = "BEAMPIPE2";
  m_TBElementVec[TBElementID::SMV                    ].m_name = "SMV";
  m_TBElementVec[TBElementID::S1                     ].m_name = "S1";
  m_TBElementVec[TBElementID::S3                     ].m_name = "S3";
  m_TBElementVec[TBElementID::S2                     ].m_name = "S2";
  m_TBElementVec[TBElementID::SH                     ].m_name = "SH";
  m_TBElementVec[TBElementID::SNEW                   ].m_name = "SNEW";
  m_TBElementVec[TBElementID::MBPSID                 ].m_name = "MBPSID";
  m_TBElementVec[TBElementID::IDET                   ].m_name = "IDET";
  m_TBElementVec[TBElementID::Pixel                  ].m_name = "Pixel";
  m_TBElementVec[TBElementID::SCT                    ].m_name = "SCT";
  m_TBElementVec[TBElementID::CTBTRT                 ].m_name = "CTBTRT";
  m_TBElementVec[TBElementID::TRT                    ].m_name = "TRT";
  m_TBElementVec[TBElementID::CALO                   ].m_name = "CALO";
  m_TBElementVec[TBElementID::LArTBBarrel            ].m_name = "LArTBBarrel";
  m_TBElementVec[TBElementID::Tile                   ].m_name = "Tile";
  m_TBElementVec[TBElementID::MuonWall               ].m_name = "MuonWall";
  m_TBElementVec[TBElementID::PhantomBarrel          ].m_name = "PhantomBarrel";
  m_TBElementVec[TBElementID::CombinedScintillator   ].m_name = "CombinedScintillator";
  m_TBElementVec[TBElementID::DeadMaterial           ].m_name = "DeadMaterial";
  m_TBElementVec[TBElementID::LArCrate               ].m_name = "LArCrate";
  m_TBElementVec[TBElementID::MUON                   ].m_name = "MUON";
  m_TBElementVec[TBElementID::Muon                   ].m_name = "Muon";  
  m_TBElementVec[TBElementID::DUMPIRON               ].m_name = "DUMPIRON";  
  m_TBElementVec[TBElementID::MBPL                   ].m_name = "MBPL";
  m_TBElementVec[TBElementID::MBPS2                  ].m_name = "MBPS2";
  m_TBElementVec[TBElementID::SCDUMP           ].m_name = "SCDUMP";
  m_TBElementVec[TBElementID::SMT              ].m_name = "SMT";
  m_TBElementVec[TBElementID::MUON10x10A       ].m_name = "MUON10x10A";
  m_TBElementVec[TBElementID::MUON10x10B       ].m_name = "MUON10x10B";
  m_TBElementVec[TBElementID::MUON10x10C       ].m_name = "MUON10x10C";
  m_TBElementVec[TBElementID::MUON10x10D       ].m_name = "MUON10x10D";
  m_TBElementVec[TBElementID::MUONHODOSELECTOR ].m_name = "MUONHODOSELECTOR";
  m_TBElementVec[TBElementID::MUONHODOPLANE11  ].m_name = "MUONHODOPLANE11";
  m_TBElementVec[TBElementID::MUONHODOPLANE12  ].m_name = "MUONHODOPLANE12";
  m_TBElementVec[TBElementID::MUONHODOPLANE13  ].m_name = "MUONHODOPLANE13";
  m_TBElementVec[TBElementID::MUONHODOPLANE14  ].m_name = "MUONHODOPLANE14";
  m_TBElementVec[TBElementID::MUONHODOPLANE15  ].m_name = "MUONHODOPLANE15";
  m_TBElementVec[TBElementID::MUONHODOPLANE16  ].m_name = "MUONHODOPLANE16";
  m_TBElementVec[TBElementID::MUONHODOPLANE21  ].m_name = "MUONHODOPLANE21";
  m_TBElementVec[TBElementID::MUONHODOPLANE22  ].m_name = "MUONHODOPLANE22";
  m_TBElementVec[TBElementID::MUONHODOPLANE23  ].m_name = "MUONHODOPLANE23";
  m_TBElementVec[TBElementID::MUONHODOPLANE24  ].m_name = "MUONHODOPLANE24";
  m_TBElementVec[TBElementID::MUONHODOPLANE25  ].m_name = "MUONHODOPLANE25";
  m_TBElementVec[TBElementID::MUONHODOPLANE26  ].m_name = "MUONHODOPLANE26";
}

void TBDetDescrManager::initialize(const std::vector<TBElement> & vec)
{
  if (m_elements>0) 
    m_TBElementVec.clear();

  m_elements = 0;
  m_TBElementVec.resize(TBElementID::TotalSize);

  std::vector<TBElement>::const_iterator itr=vec.begin();
  std::vector<TBElement>::const_iterator end=vec.end();

  for ( ; itr!=end; ++itr) {
    unsigned int iid = itr->id();
    if ( iid >= m_TBElementVec.size() )
      m_TBElementVec.resize(iid+1);
    m_TBElementVec[iid] = (*itr);
    if ( m_TBElementVec[iid].id() != TBElementID::Unknown )
      ++m_elements;
  }
}

void TBDetDescrManager::initialize(const TBElementContainer * pTBElementContainer)
{
  TBElementContainer::const_iterator itr = pTBElementContainer->begin();
  TBElementContainer::const_iterator end = pTBElementContainer->end();

  std::vector<TBElement> vec;
  
  for (; itr!=end; ++itr)
    vec.push_back(*(*itr));

  initialize(vec);
}

void TBDetDescrManager::getAll(TBElementContainer * pTBElementContainer) const
{
  std::vector<TBElement>::const_iterator itr=m_TBElementVec.begin();
  std::vector<TBElement>::const_iterator end=m_TBElementVec.end();

  for ( ; itr!=end; ++itr) {
    if (itr->id() != TBElementID::Unknown) {
      TBElement* pTBElement = new TBElement(*itr);
      pTBElementContainer->push_back(pTBElement);
    }
  }
}

const TBElement & TBDetDescrManager::getElement(TBElementID::TBElementID id) const
{
  // no check here if element is initialized or not
  // all the checks should be done by caller  (element.id() != TBElementID::Unknown)
  return m_TBElementVec[id];
}

void TBDetDescrManager::setElement(const TBElement & element)
{
  unsigned int iid = element.id();

  if ( iid >= m_TBElementVec.size())
    m_TBElementVec.resize(iid+1);

  // we clear old element, so number of elements decreased
  if ( m_TBElementVec[iid].id() != TBElementID::Unknown )
    --m_elements;

  m_TBElementVec[iid] = element;
  
  // we probably created new element, so number of elements increased
  if ( m_TBElementVec[iid].id() != TBElementID::Unknown )
    ++m_elements;
}

void TBDetDescrManager::print(MsgStream &log) const
{
  std::vector<TBElement>::const_iterator itr=m_TBElementVec.begin();
  std::vector<TBElement>::const_iterator end=m_TBElementVec.end();

  log << MSG::DEBUG
      << "Total number of TBElements in TBDetDescrManager = "
      << m_elements
      << endreq;
  
  for ( ; itr!=end; ++itr) {
    if (itr->id() != TBElementID::Unknown) {
      log << MSG::DEBUG 
          << (std::string) (*itr)
          << endreq;
    }
  }
}
