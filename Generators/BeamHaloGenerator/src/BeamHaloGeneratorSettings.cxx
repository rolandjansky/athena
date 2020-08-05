/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BeamHaloGenerator/BeamHaloGeneratorSettings.h"
#include "BeamHaloGenerator/BeamHaloParticle.h"
#include "BeamHaloGenerator/AsciiInput.h"
#include <iostream>
#include <cmath>

const std::string BeamHaloGeneratorSettings::m_limitNames[ENUM_LIMITS_SIZE] = {"pxLimits",
                                                                               "pyLimits",
                                                                               "pzLimits",
                                                                               "energyLimits",
                                                                               "xLimits",
                                                                               "yLimits",
                                                                               "zLimits",
                                                                               "ptLimits",
                                                                               "phiLimits",
                                                                               "etaLimits",
                                                                               "rLimits",
                                                                               "weightLimits"};

//---------------------------------------------------------------------

BeamHaloGeneratorSettings::BeamHaloGeneratorSettings(std::vector<std::string> settings): 
  m_generatorSettings(settings),
  m_allowedPdgIds(),
  m_limits(),
  m_limitsEnabled(),
  m_shapeRequirement(0),
  m_settingsParsed(false) {
 
  // Initialise the lower and upper limits as zero, and disabled.
  for(int i=0;i<ENUM_LIMITS_SIZE;i++) {
    m_limits.push_back(std::make_pair(0.,0.));
    m_limitsEnabled.push_back(std::make_pair(false,false));
  }
}

//---------------------------------------------------------------------

int BeamHaloGeneratorSettings::parseSettings() {
  std::vector<std::string> strVector;
  std::vector<std::string>::iterator row_itr;
  std::vector<std::string>::iterator row_itr_end;
  std::vector<std::string>::iterator col_itr;
  std::vector<std::string>::iterator col_itr_end;
//  double  upperLimit, lowerLimit;
  
  // Loop over all settings.
  row_itr = m_generatorSettings.begin();
  row_itr_end = m_generatorSettings.end();
  for(;row_itr!=row_itr_end;++row_itr) {
    strVector.clear();
    strVector = AsciiInput::strToStrVec(*row_itr);
    
    if(strVector.size() == 0) continue;
    if(strVector.size() == 1) {
      std::cerr << "A generator setting must be followed by a value" << std::endl;
      continue;
    }

//    lowerLimit = -1.; upperLimit = -1.1; // Disable limits by default;

    col_itr = strVector.begin();
    col_itr_end = strVector.end();
    
    if((*col_itr) == "allowedPdgId") {
      col_itr++;
      for(;col_itr!=col_itr_end;++col_itr) {
	m_allowedPdgIds.push_back(AsciiInput::strToLong(*col_itr));
      }
    }
    else if((*col_itr) == "shape") {
      col_itr++;
      if((*col_itr) == "cylinder") {
        m_shapeRequirement = CYLINDER;
      }
      else {
        std::cerr << "Warning: unknown shape requirement \"" << (*col_itr) << "\" ignored." << std::endl;
      }
    }
    else {
      parseLimitSetting(&strVector);
    }
  }

  printSettings();

  m_settingsParsed = true;

  return 0;
}

//---------------------------------------------------------------------

int BeamHaloGeneratorSettings::parseLimitSetting(std::vector<std::string> *commandVector) {
  std::vector<std::string>::iterator itr = commandVector->begin();
  std::vector<std::string>::iterator itr_end = commandVector->end();
  
  int i = 0;
  while(i<ENUM_LIMITS_SIZE) {
    if((*itr) == m_limitNames[i]) break;
    i++;
  }
  
  if(i == ENUM_LIMITS_SIZE) {
    std::cerr << "Error: " << (*itr)  << " is an un-known generator setting." << std::endl;
    return 1;
  }

  // Look for the lower limit
  itr++;
  if(itr==itr_end) {
    std::cerr << "Warning: " << m_limitNames[i] << " was not followed by a lower or upper limit." << std::endl;
    return 0;
  }
 
  // Enable the lower limit.
  m_limits[i] = std::make_pair(AsciiInput::strToDouble(*itr),0.);
  m_limitsEnabled[i] = std::make_pair(true,false);

  // Look for the upper limit
  itr++;
  if(itr==itr_end) { // no upper limit.
    return 0;
  }
 
  // Enable the upper limit.   
  m_limits[i].second = AsciiInput::strToDouble(*itr);
  m_limitsEnabled[i].second = true;
  
  return 0;
}

//---------------------------------------------------------------------

bool BeamHaloGeneratorSettings::checkParticle(BeamHaloParticle *beamHaloParticle) {
  if(!m_settingsParsed) {
    if(parseSettings() != 0) return false;  
  }

  // Search the allowed PDG id list if it has been defined.
  if(m_allowedPdgIds.size() != 0) {
    std::vector<long>::iterator itr = m_allowedPdgIds.begin();
    std::vector<long>::iterator itr_end = m_allowedPdgIds.end();
    while(itr!=itr_end) {
      if(beamHaloParticle->pdgId() == (*itr)) break;
      ++itr;
    }
    if(itr==itr_end) {
      return false;
    }
  }

  // First implement the cuts which are not related to the particle
  // trajectory.
  if(!checkSetting(W_LIMIT, std::abs(beamHaloParticle->weight()))) return false;
  if(!checkSetting(PX_LIMIT, std::abs(beamHaloParticle->fourVector().px()))) return false;
  if(!checkSetting(PY_LIMIT, std::abs(beamHaloParticle->fourVector().py()))) return false;
  if(!checkSetting(PZ_LIMIT, std::abs(beamHaloParticle->fourVector().pz()))) return false;
  if(!checkSetting(E_LIMIT, std::abs(beamHaloParticle->fourVector().e()))) return false;
  if(!checkSetting(PT_LIMIT, beamHaloParticle->fourVector().perp())) return false;
  if(!checkSetting(ETA_LIMIT, std::abs(beamHaloParticle->fourVector().pseudoRapidity()))) return false;
  if(!checkSetting(PHI_LIMIT, std::abs(beamHaloParticle->fourVector().phi()))) return false;


  // Require the selection at the scoring plane.
  if(m_shapeRequirement == NONE) {
    if(!checkSetting(X_LIMIT, std::abs(beamHaloParticle->positionAtScoringPlane().x()))) return false;
    if(!checkSetting(Y_LIMIT, std::abs(beamHaloParticle->positionAtScoringPlane().y()))) return false;
    if(!checkSetting(R_LIMIT, beamHaloParticle->positionAtScoringPlane().perp())) return false;
  }
  else if(m_shapeRequirement == CYLINDER) {
    if(!checkCylinder(beamHaloParticle)) return false;
  }
  else {
    std::cerr << "Warning: m_shapeRequirement = " << m_shapeRequirement << " is an unknown requirement." << std::endl;
  }
 
  return true;
}

//---------------------------------------------------------------------

bool BeamHaloGeneratorSettings::checkCylinder(BeamHaloParticle *beamHaloParticle) {
  double rmin = m_limits[R_LIMIT].first;
  double rmax = m_limits[R_LIMIT].second;
  double zmin = m_limits[Z_LIMIT].first;
  double zmax = m_limits[Z_LIMIT].second;

  // Check if the particle radius at the scoring place is outside the
  // outer radius of the cylinder and the particle trajectory is away
  // from the cylinder reject the particle.
  if(beamHaloParticle->positionAtScoringPlane().perp() > rmax && 
     beamHaloParticle->fourVector().theta() > 0.) return false;
  
  // If the particle has no pT then check if it is inside the radius
  // of the cylinder.
  if(beamHaloParticle->fourVector().perp() == 0.) {
    if(beamHaloParticle->positionAtScoringPlane().perp() < rmin || 
       beamHaloParticle->positionAtScoringPlane().perp() > rmin) {
      return false;
    }
    else {
      return true;
    }
  }

  // r = sqrt(x^2 + y^2)
  //
  // tan(theta) = pT/pz
  // tan(theta) = r/z
  //
  // z = (r*pz)/pT

  // The z position where the particle crosses the inner radius.
  double innerZ = (rmin*beamHaloParticle->fourVector().z())/beamHaloParticle->fourVector().perp();

  // The z position where the particle crosses the outer radius.
  double outerZ = (rmax*beamHaloParticle->fourVector().z())/beamHaloParticle->fourVector().perp();
  
  // If the trajectory of the particle crosses either the inner or
  // outer surface within the length of the cylinder keep it.
  if((innerZ >= zmin && innerZ <= zmax) ||
     (outerZ >= zmin && outerZ <= zmax)) {
    return true;
  }

  return false;
}

//---------------------------------------------------------------------

bool BeamHaloGeneratorSettings::checkSetting(int index, double value) {
  if(index >= ENUM_LIMITS_SIZE) {
    std::cerr << "Error: the index " << index << " is out of range." << std::endl;
    return false;
  }

  if(m_limitsEnabled[index].first && value < m_limits[index].first) return false;
  if(m_limitsEnabled[index].second && value >= m_limits[index].second) return false;

  return true;
}

//---------------------------------------------------------------------

void BeamHaloGeneratorSettings::printSettings() {
  std::cout << "============== BeamHaloGeneratorSettings ==============" << std::endl;
  std::cout << " allowedPdfIds: ";
  if(m_allowedPdgIds.size() != 0) {
    std::vector<long>::iterator pdgId_itr = m_allowedPdgIds.begin();
    std::vector<long>::iterator pdgId_itr_end = m_allowedPdgIds.end();
    for(;pdgId_itr != pdgId_itr_end; ++pdgId_itr) {
      std::cout << (*pdgId_itr) << " ";
    }
    std::cout << std::endl;
  }
  else {
    std::cout << "All PDG IDs are allowed." << std::endl;
  }

  std::cout << " shapeRequirement: ";  
  switch (m_shapeRequirement) {
  case(NONE) : std::cout << "NONE"; break; 
  case(CYLINDER) : std::cout << "CYLINDER"; break;
  default : std::cout << "UNKNOWN"; break;
  }
  std::cout << std::endl;

  for(int i=0;i<ENUM_LIMITS_SIZE;i++) {
    std::cout << " " << m_limitNames[i] << ": {";
    if(!m_limitsEnabled[i].first) std::cout << "disabled, ";
    else std::cout << m_limits[i].first << ", ";
    if(!m_limitsEnabled[i].second) std::cout << "disabled}";
    else std::cout << m_limits[i].second << "}";
    std::cout << std::endl;
  }
  std::cout << "=======================================================" << std::endl;
}

