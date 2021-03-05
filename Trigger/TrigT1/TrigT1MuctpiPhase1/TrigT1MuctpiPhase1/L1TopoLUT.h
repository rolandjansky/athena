// This file is really -*- C++ -*-.

/*                                                                                                                      
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration                                               
*/

#ifndef TRIGT1MUCTPIPHASE1_L1TOPOLUT_H
#define TRIGT1MUCTPIPHASE1_L1TOPOLUT_H

#include <boost/property_tree/ptree.hpp>

#include <unordered_map>
#include <set>
#include <utility>
#include <sstream>

namespace LVL1MUCTPIPHASE1
{
  namespace pt = boost::property_tree;

  struct L1TopoCoordinates
  {
    double eta;
    double phi;
    double eta_min;
    double eta_max;
    double phi_min;
    double phi_max;
    unsigned short ieta;
    unsigned short iphi;
  };

  class L1TopoLUT
  {
  public:
    
    bool initializeLUT(const std::string& barrelFileName, 
		       const std::string& ecfFileName,
		       const std::string& side0LUTFileName,
 		       const std::string& side1LUTFileName);
    
    L1TopoCoordinates getCoordinates(const unsigned short& side,
				     const unsigned short& subsystem,
				     const unsigned short& sectorID,
				     const unsigned short& roi) const;
    
    std::set<std::string> getErrors() const {return m_errors;}

  protected:

    bool initializeLUT(const std::string& inFileName, const bool& isBarrel);
    bool initializeJSON(const std::string& inFileName, const bool& side);
    bool initializeJSONForSubsystem(pt::ptree& root,
				    const std::string& nodeName, 
				    const bool& side,
				    const unsigned short& subsystem);
    
    struct L1TopoLUTKey
    {
      unsigned short side;
      unsigned short subsystem;
      unsigned short sectorID;
      unsigned short roi;

      std::string info()
      {
	std::stringstream str;
	str << "side, subsystem, sectorID, roi = " 
	    << side << ", " << subsystem << ", " << sectorID << ", " << roi;
	return str.str();
      }

      //implement == operator for hashing within unordered_map

      bool operator==(const L1TopoLUTKey& rhs) const
      {
	return (side == rhs.side &&
		subsystem == rhs.subsystem &&
		sectorID == rhs.sectorID &&
		roi == rhs.roi);
      }

    };

    struct L1TopoLUTKeyHasher
    {
      unsigned long operator()(const L1TopoLUTKey& key) const
      {
	return key.side | (key.subsystem << 8) | (key.sectorID << 16) | (key.roi << 24);
      }
    };

    std::unordered_map<L1TopoLUTKey, L1TopoCoordinates, L1TopoLUTKeyHasher> m_encoding;
    std::set<std::string> m_errors;
  };
}

#endif
