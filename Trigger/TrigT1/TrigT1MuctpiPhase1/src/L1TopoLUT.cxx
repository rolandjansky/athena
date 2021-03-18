#include "TrigT1MuctpiPhase1/L1TopoLUT.h"

#include <boost/property_tree/json_parser.hpp>

#include <fstream>

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
    success = success && initializeJSON(side0LUTFileName, 0);
    success = success && initializeJSON(side1LUTFileName, 1);

    return success;
  }

  bool L1TopoLUT::initializeLUT(const std::string& inFileName, const bool& isBarrel)
  {
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

      L1TopoLUTKey key = {side, subsystem, sectorID, roi};
      L1TopoCoordinates value = {eta, phi, eta_min, eta_max, phi_min, phi_max, 0, 0};
      if (m_encoding.find(key) != m_encoding.end())
      {
	m_errors.insert("Duplicate key found in L1TopoLUT: "+key.info());
	return false;
      }
      m_encoding[key] = value;
    }

    return true;
  }

  bool L1TopoLUT::initializeJSON(const std::string& inFileName, const bool& side)
  {
    // Create a root
    pt::ptree root;

    // Load the json file in this ptree
    pt::read_json(inFileName.c_str(), root);


    // Supplement the L1TopoCoordinates with the eta/phi indices for each subsystem
    bool success = true;
    success = success && initializeJSONForSubsystem(root, "encode_lookup_barrel", side, 0);
    success = success && initializeJSONForSubsystem(root, "encode_lookup_endcap", side, 1);
    success = success && initializeJSONForSubsystem(root, "encode_lookup_forward", side, 2);

    return success;
  }

  bool L1TopoLUT::initializeJSONForSubsystem(pt::ptree& root,
					     const std::string& nodeName, 
					     const bool& side, 
					     const unsigned short& subsystem)
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
	  m_errors.insert("Invalide eta/phi code size");
	  return false;
	}
	L1TopoLUTKey key = {(unsigned short)(side), subsystem, sectorID, roi}; 
	auto itr = m_encoding.find(key);
	if (itr == m_encoding.end())
	{
	  m_errors.insert("Couldn't find L1TopoLUTKey when reading JSON files");
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
      L1TopoCoordinates null = {0, 0, 0, 0, 0, 0, 0, 0};
      return null;
    }

    return itr->second;
  }
}
