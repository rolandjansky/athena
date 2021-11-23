#include "L1TopoLUT.h"

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // silence Boost pragma message (fixed in Boost 1.76)
#include <boost/property_tree/json_parser.hpp>

#include <fstream>
#include <map>
#include <iostream>

namespace LVL1MUCTPIPHASE1
{
  bool L1TopoLUT::initializeLUT(const std::string& barrelFileName,
				const std::string& ecfFileName,
				const std::string& side0LUTFileName,
				const std::string& side1LUTFileName)

  {
    m_encoding.clear();
    m_errors.clear();
    bool success = true;

    //read the barrel file
    success = success && initializeLUT(barrelFileName, true);

    //read the endcap+forward file
    success = success && initializeLUT(ecfFileName, false);

    //read the json files 
    //(the input labels are swapped between A-side and C-side)
    success = success && initializeJSON(side1LUTFileName, 0);
    success = success && initializeJSON(side0LUTFileName, 1);

    return success;
  }

  bool L1TopoLUT::initializeLUT(const std::string& inFileName, const bool& isBarrel)
  {
    //map between L1TopoLUTKey -> map between eta/phi -> index
    std::unordered_map<L1TopoLUTKey, std::map<float, unsigned short>, L1TopoLUTKeyHasher> sector_eta_indices, sector_phi_indices;

    std::ifstream inFile(inFileName.c_str());
    if (!inFile) return false;
    while (!inFile.eof() && inFile.good())
    {
      unsigned short side;
      unsigned short subsystem=0;
      unsigned short sectorID;
      unsigned short roi;
      double eta_min, eta_max;
      double phi_min, phi_max;

      inFile >> side;
      if (inFile.eof()) break;

      if (!isBarrel) 
      {
	inFile >> subsystem;
      
	//in the EC+FW file, EC=0 and FW=1, so increment to distinguish from barrel
	subsystem++;
      }

      inFile >> sectorID;
      inFile >> roi;
      inFile >> eta_min;
      inFile >> eta_max;
      inFile >> phi_min;
      inFile >> phi_max;

      double eta = (eta_max+eta_min)/2.;
      double phi = (phi_max+phi_min)/2.;

      L1TopoLUTKey key_no_roi = {side, subsystem, sectorID, 0};

      //hold the eta and phi indices in memory
      std::map<float, unsigned short>* eta_indices = &sector_eta_indices[key_no_roi];
      unsigned short eta_index = 0;
      auto eta_itr = eta_indices->find(float(eta));
      if (eta_itr != eta_indices->end()) eta_index = eta_itr->second;
      else 
      {
	eta_index = eta_indices->size();
	(*eta_indices)[float(eta)] = eta_index;
      }

      std::map<float, unsigned short>* phi_indices = &sector_phi_indices[key_no_roi];
      unsigned short phi_index = 0;
      auto phi_itr = phi_indices->find(float(phi));
      if (phi_itr != phi_indices->end()) phi_index = phi_itr->second;
      else 
      {
	phi_index = phi_indices->size();
	(*phi_indices)[float(phi)] = phi_index;
      }
      
      L1TopoLUTKey key = {side, subsystem, sectorID, roi};
      L1TopoCoordinates value = {eta, phi, eta_min, eta_max, phi_min, phi_max, eta_index, phi_index};
      //crude crude patch by Patrick Czodrowski for the inverse eta coordinates https://its.cern.ch/jira/browse/ATR-24376                                                                                                                                                                                    
      if ( (side == 0 && eta > 0) || (side == 1 && eta < 0) ) {
	value = {-eta, phi, -eta_min, -eta_max, phi_min, phi_max, eta_index, phi_index};
      }

      if (m_encoding.find(key) != m_encoding.end())
      {
	m_errors.push_back("Duplicate key found in L1TopoLUT: "+key.info());
	return false;
      }
      m_encoding[key] = value;
    }

    return true;
  }

  bool L1TopoLUT::initializeJSON(const std::string& inFileName, bool side)
  {
    // Create a root
    pt::ptree root;

    // Load the json file in this ptree
    pt::read_json(inFileName.c_str(), root);


    // Supplement the L1TopoCoordinates with the eta/phi indices for the barrel.
    // These are calculated on-the-fly for the endcap/forward.
    bool success = true;
    success = success && initializeJSONForSubsystem(root, "encode_lookup_barrel", side, 0);

    return success;
  }

  bool L1TopoLUT::initializeJSONForSubsystem(pt::ptree& root,
					     const std::string& nodeName, 
					     bool side, 
					     unsigned short subsystem)
  {
    for (pt::ptree::value_type& sectorID_elem : root.get_child(nodeName))
    {
      unsigned short sectorID = std::stoi(sectorID_elem.first);
      for (pt::ptree::value_type& roi_elem : sectorID_elem.second)
      {
	unsigned short roi = std::stoi(roi_elem.first);
	std::vector<unsigned short> codes;
	for (pt::ptree::value_type& code_elem : roi_elem.second) codes.push_back(code_elem.second.get_value<unsigned short>());
	if (codes.size() != 2) 
	{
	  m_errors.push_back("Invalide eta/phi code size");
	  return false;
	}
	L1TopoLUTKey key = {(unsigned short)(side), subsystem, sectorID, roi}; 
	auto itr = m_encoding.find(key);
	if (itr == m_encoding.end())
	{
	  std::stringstream err;
	  err << "Couldn't find L1TopoLUTKey when reading JSON files: Node = " << nodeName << ", side = " << side << ", subsystem = " << subsystem << ", sector = " << sectorID << ", roi = " << roi;
	  m_errors.push_back(err.str());
	  return false;
	}

	itr->second.ieta = codes[0];
	itr->second.iphi = codes[1];
      }
    }
    return true;
  }

  L1TopoCoordinates L1TopoLUT::getCoordinates(const unsigned short& side,
					      const unsigned short& subsystem,
					      const unsigned short& sectorID,
					      const unsigned short& roi) const
  {
    L1TopoLUTKey key = {side, subsystem, sectorID, roi};
    auto itr = m_encoding.find(key);
    if (itr == m_encoding.end())
    {
      L1TopoCoordinates null;
      return null;
    }

    return itr->second;
  }
}
