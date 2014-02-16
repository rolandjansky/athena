/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                        JetFitterGenericTagInfo.h  -  Description
                             -------------------
    begin   : 2012
    authors : Dan Guest 
    email   : dguest@cern.ch

    changes :

 ***************************************************************************/

#include "JetTagInfo/JetFitterGenericTagInfo.h"
#include <stdexcept>
#include <cassert>
#include <limits> 
#include <boost/format.hpp>

namespace Analysis
{

  // --- lookup strings for backward compatibility  ---

  const char* JetFitterGenericTagInfo::N_VTX = "nVTX"; 
  const char* JetFitterGenericTagInfo::N_SINGLE_TRACKS = "nSingleTracks"; 
  const char* JetFitterGenericTagInfo::N_TRACKS_AT_VTX = "nTracksAtVtx"; 
  const char* JetFitterGenericTagInfo::MASS = "mass"; 
  const char* JetFitterGenericTagInfo::ENERGY_FRACTION =  "energyFraction"; 
  const char* JetFitterGenericTagInfo::SIGNIFICANCE_3D = "significance3d"; 
  const char* JetFitterGenericTagInfo::DELTA_ETA = "deltaeta"; 
  const char* JetFitterGenericTagInfo::DELTA_PHI = "deltaphi"; 

  /** Default constructor */
  JetFitterGenericTagInfo::JetFitterGenericTagInfo() : BaseTagInfo()
  {
  }

  /** constructor with infotype */
  JetFitterGenericTagInfo::JetFitterGenericTagInfo(TagInfoType tagJetInfoType) : 
    BaseTagInfo(tagJetInfoType)
  {
  }

  /** Copy constructor */
  JetFitterGenericTagInfo::JetFitterGenericTagInfo(const JetFitterGenericTagInfo& rhs) : 
    BaseTagInfo(rhs)
  {
  }

  /** assigenment operator */
  JetFitterGenericTagInfo& JetFitterGenericTagInfo::operator= (const JetFitterGenericTagInfo& rhs)
  {
    if (this!=&rhs)
      {
	m_experimental_ints = rhs.m_experimental_ints; 
	m_experimental_doubles = rhs.m_experimental_doubles; 
      }
    return *this;
  }

  /** Default destructor */
  JetFitterGenericTagInfo::~JetFitterGenericTagInfo()
  {
  }

  void JetFitterGenericTagInfo::clear() { 
    m_experimental_doubles.clear(); 
    m_experimental_ints.clear(); 
  }

  // experimental variable container access

  double JetFitterGenericTagInfo::getDouble(std::string key) const { 
    DoubleMap::const_iterator itr = m_experimental_doubles.find(key); 
    if (itr == m_experimental_doubles.end() ) { 
      return std::numeric_limits<double>::min(); 
    }
    return itr->second; 
  }

  double JetFitterGenericTagInfo::getDoubleOrThrow(std::string key) const { 
    DoubleMap::const_iterator itr = m_experimental_doubles.find(key); 
    if (itr == m_experimental_doubles.end() ) { 
      throw std::range_error("no stored value for " + 
			     key + " in JetFitterGenericTagInfo"); 
    }
    return itr->second; 
  }
  JetFitterGenericTagInfo::DoubleMap::const_iterator 
  JetFitterGenericTagInfo::getDoublesBegin() const { 
    return m_experimental_doubles.begin(); 
  }
  JetFitterGenericTagInfo::DoubleMap::const_iterator 
  JetFitterGenericTagInfo::getDoublesEnd() const { 
    return m_experimental_doubles.end(); 
  }
  bool JetFitterGenericTagInfo::setDouble(std::string key, double value) {
    bool already_defined = m_experimental_doubles.count(key); 
    (m_experimental_doubles)[key] = value; 
    return already_defined; 
  }
  bool JetFitterGenericTagInfo::setTemporaryDouble(std::string key, double value) {
    m_temporary_variables.push_back(key); 
    return setDouble(key, value); 
  }


  int JetFitterGenericTagInfo::getInt(std::string key) const { 
    IntMap::const_iterator itr = m_experimental_ints.find(key); 
    if (itr == m_experimental_ints.end()) { 
      return std::numeric_limits<int>::min(); 
    }
    return itr->second; 
  }
  
  int JetFitterGenericTagInfo::getIntOrThrow(std::string key) const { 
    IntMap::const_iterator itr = m_experimental_ints.find(key); 
    if (itr == m_experimental_ints.end()) { 
      throw std::range_error("no stored value for " + key + 
			     " in JetFitterGenericTagInfo"); 
    }
    return itr->second; 
  }
  
  JetFitterGenericTagInfo::IntMap::const_iterator 
  JetFitterGenericTagInfo::getIntsBegin() const { 
    return m_experimental_ints.begin(); 
  }
  JetFitterGenericTagInfo::IntMap::const_iterator 
  JetFitterGenericTagInfo::getIntsEnd() const { 
    return m_experimental_ints.end(); 
  }
  bool JetFitterGenericTagInfo::setInt(std::string key, int value) {
    bool already_defined = m_experimental_ints.count(key); 
    (m_experimental_ints)[key] = value; 
    return already_defined; 
  }
  bool JetFitterGenericTagInfo::setTemporaryInt(std::string key, int value) { 
    m_temporary_variables.push_back(key); 
    return setInt(key, value); 
  }

  int JetFitterGenericTagInfo::clearTemporary() { 
    int total_removed = 0; 
    for (std::vector<std::string>::const_iterator 
	   itr = m_temporary_variables.begin(); 
	 itr != m_temporary_variables.end(); 
	 itr++) { 
      total_removed += m_experimental_ints.erase(*itr); 
      total_removed += m_experimental_doubles.erase(*itr); 
    }
    m_temporary_variables.clear(); 
    return total_removed; 
  }

  // ----- functions for backward compatibility 

  int JetFitterGenericTagInfo::nVTX() const
  {
    return getInt(JetFitterGenericTagInfo::N_VTX);
  }

  int JetFitterGenericTagInfo::nSingleTracks() const
  {
    return getInt(JetFitterGenericTagInfo::N_SINGLE_TRACKS);
  }

  int JetFitterGenericTagInfo::nTracksAtVtx() const
  {
    return getInt(JetFitterGenericTagInfo::N_TRACKS_AT_VTX);
  }

  double JetFitterGenericTagInfo::mass() const
  {
    return getDouble(JetFitterGenericTagInfo::MASS);
  }
  double JetFitterGenericTagInfo::energyFraction() const
  {
    return getDouble(JetFitterGenericTagInfo::ENERGY_FRACTION);
  }
  double JetFitterGenericTagInfo::significance3d() const 
  {
    return getDouble(JetFitterGenericTagInfo::SIGNIFICANCE_3D);
  }

  double JetFitterGenericTagInfo::deltaeta() const 
  {
    return getDouble(JetFitterGenericTagInfo::DELTA_ETA); 
  }

  double JetFitterGenericTagInfo::deltaphi() const 
  {
    return getDouble(JetFitterGenericTagInfo::DELTA_PHI);
  }

  void JetFitterGenericTagInfo::setnVTX(int nVTX)
  {
    setInt(JetFitterGenericTagInfo::N_VTX,nVTX); 
  }
  void JetFitterGenericTagInfo::setnSingleTracks(int nSingleTracks)
  {
    setInt(JetFitterGenericTagInfo::N_SINGLE_TRACKS,nSingleTracks);
  }

  void JetFitterGenericTagInfo::setnTracksAtVtx(int nTracksAtVtx)
  {
    setInt(JetFitterGenericTagInfo::N_TRACKS_AT_VTX,nTracksAtVtx);
  }

  void JetFitterGenericTagInfo::setMass(double mass)
  {
    setDouble(JetFitterGenericTagInfo::MASS,mass);
  }
  void JetFitterGenericTagInfo::setEnergyFraction(double energyFraction)
  {
    setDouble(JetFitterGenericTagInfo::ENERGY_FRACTION,energyFraction);
  }

  void JetFitterGenericTagInfo::setSignificance3d(double significance3d) 
  {
    setDouble(JetFitterGenericTagInfo::SIGNIFICANCE_3D,significance3d);
  }

  inline Analysis::JetFitterGenericTagInfo* Analysis::JetFitterGenericTagInfo::clone() const
  {
    return new JetFitterGenericTagInfo(*this);
  }

  void JetFitterGenericTagInfo::setDeltaeta(double deltaeta) {
    setDouble(JetFitterGenericTagInfo::DELTA_ETA,deltaeta);
  }

  void JetFitterGenericTagInfo::setDeltaphi(double deltaphi) {
    setDouble(JetFitterGenericTagInfo::DELTA_PHI,deltaphi);
  }



}

