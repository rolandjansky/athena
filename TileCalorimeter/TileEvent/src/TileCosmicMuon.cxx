/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//========================================
// file TileCosmicMuon.cxx
// 
//========================================

#include "TileEvent/TileCosmicMuon.h"
  
#include <iostream>
#include <sstream>
#include <iomanip>

double TileCosmicMuon::GetFullPath() const
{
    double length = 0.;
	if (m_pathTop.size() == (unsigned int)3){
       for (unsigned int is = 0; is<m_pathTop.size(); is++) length+=m_pathTop[is];
	}
	if (m_pathBottom.size() == (unsigned int)3){
       for (unsigned int is = 0; is<m_pathBottom.size(); is++) length+=m_pathBottom[is];
	}
	return length;
}

double TileCosmicMuon::GetFullEnergy() const
{
    double energy = 0.;
	if (m_energyTop.size() == (unsigned int)3){
       for (unsigned int is = 0; is<m_energyTop.size(); is++) energy += m_energyTop[is];
	}
	if (m_energyBottom.size() == (unsigned int)3){
       for (unsigned int is = 0; is<m_energyBottom.size(); is++) energy += m_energyBottom[is];
	}
	return energy;
}

TileCosmicMuon::operator std::string() const
{
    std::ostringstream text(std::ostringstream::out);

    text << whoami();
    text << " Time = "           << m_time;
    text << " PositionX = "      << m_positionX;
    text << " PositionY = "      << m_positionY;
    text << " PositionZ = "      << m_positionZ;
    text << " DirectionPhi = "   << m_directionPhi;
    text << " DirectionTheta = " << m_directionTheta;
    text << " FitQuality = "     << m_fitQuality;
    text << " FitNCells = "      << m_fitNCells;

    return text.str();
}
