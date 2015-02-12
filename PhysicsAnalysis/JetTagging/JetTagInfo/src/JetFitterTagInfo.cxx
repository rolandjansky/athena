/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        JetFitterTagInfo.h  -  Description
                             -------------------
    begin   : March 2007
    authors : Giacinto Piacquadio, Christian Weiser (University of Freiburg)
    email   : giacinto.piacquadio@physik.uni-freiburg.de
              christian.weiser@physik.uni-freiburg.de
    changes :

 ***************************************************************************/

#include "JetTagInfo/JetFitterTagInfo.h"

namespace Analysis
{

/** Default constructor */
JetFitterTagInfo::JetFitterTagInfo() : BaseTagInfo(),
				       m_nVTX(-1),
				       m_nSingleTracks(-1),
				       m_nTracksAtVtx(-1),
				       m_mass(-1.),
				       m_energyFraction(-1.),
				       m_significance3d(-1.),
				       m_deltaeta(-1.),
				       m_deltaphi(-1.)
{
  
}

/** constructor with infotype */
JetFitterTagInfo::JetFitterTagInfo(TagInfoType tagJetInfoType) : 
  BaseTagInfo(tagJetInfoType),
  m_nVTX(-1),
  m_nSingleTracks(-1),
  m_nTracksAtVtx(-1),
  m_mass(-1.),
  m_energyFraction(-1.),
  m_significance3d(-1.),
  m_deltaeta(-1.),
  m_deltaphi(-1.)
{
  
}

/** Copy constructor */
JetFitterTagInfo::JetFitterTagInfo(const JetFitterTagInfo& rhs) : 
  BaseTagInfo(rhs),
  m_nVTX(rhs.m_nVTX),
  m_nSingleTracks(rhs.m_nSingleTracks),
  m_nTracksAtVtx(rhs.m_nTracksAtVtx),
  m_mass(rhs.m_mass),
  m_energyFraction(rhs.m_energyFraction),
  m_significance3d(rhs.m_significance3d),
  m_deltaeta(rhs.m_deltaeta),
  m_deltaphi(rhs.m_deltaphi)
{}

/** assigenment operator */
JetFitterTagInfo& JetFitterTagInfo::operator= (const JetFitterTagInfo& rhs)
{
    if (this!=&rhs)
    {
      m_nVTX=rhs.m_nVTX;
      m_nSingleTracks=rhs.m_nSingleTracks;
      m_nTracksAtVtx=rhs.m_nTracksAtVtx;
      m_mass=rhs.m_mass;
      m_energyFraction=rhs.m_energyFraction;
      m_significance3d=rhs.m_significance3d;
      m_deltaeta=rhs.m_deltaeta;
      m_deltaphi=rhs.m_deltaphi;
    }
    return *this;
}

/** Default destructor */
JetFitterTagInfo::~JetFitterTagInfo()
{}

}
