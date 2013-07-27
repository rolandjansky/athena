/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDDATA_MDTDEADCHAMBERSTATUS_H
#define MUONCONDDATA_MDTDEADCHAMBERSTATUS_H

#include "CLIDSvc/CLASS_DEF.h"

#include <map>
#include <vector>
#include <string>


class MdtDeadChamberStatus {
 public:
  
  /// Mdt multilayer data (multilayer, layer, tube)
  typedef std::map<int,std::vector<std::vector<int> > > MdtMultilayerData;
  /// Mdt chamber data (chamber, multilayer data)
  typedef std::map<std::string, MdtMultilayerData> MdtChamberData;

  // Constructor
  MdtDeadChamberStatus();
  
  // Destructor
  virtual ~MdtDeadChamberStatus();

  /// Add defects for a chamber
  bool addChamberDead( const std::string & chamberId, const MdtMultilayerData & multilayerData );
 
  /// Search the map for a chamber
  MdtMultilayerData findChamber( const std::string & chamberId );
  
  /// Print the map - might be useful
  std::string str();

  /// Print data for one chamber 
  std::string chamberText( const std::string & chamberId );
  
  /// Return whether the map is empty
  bool empty() const;
  
  /// Clear the map
  void clear();
  
  // return whole map
  MdtChamberData getDeadChamberMap();
  
  // return vector of dead chambers
  std::vector<std::string> getDeadChambers();
  
  // return vector of dead multilayers
  std::vector<int> getDeadMultilayers(const std::string chamberId);
  
  // return vector of dead layers
  std::vector<int> getDeadLayers(const std::string chamberId, const int multilayerId);
 
  // return vector of dead tubes
  std::vector<int> getDeadTubes(const std::string chamberId, 
     const int multilayerId, const int layerId);

  /// Return the size of the map
  unsigned int nDeadChambers() const;
  
  /// Return the number of dead multilayers for given chamber
  unsigned int nDeadMultilayers(const std::string & chamberId );  
  
  /// Return the number of dead layers for given chamber
  unsigned int nDeadLayers(const std::string & chamberId );
 
  /// Return the number of dead tubes for given chamber
  unsigned int nDeadTubes(const std::string & chamberId );

private:
   
  MdtChamberData m_dataMap;

};


CLASS_DEF( MdtDeadChamberStatus, 228145, 1 )

#endif // MDTDEADCHAMBERSTATUS_H
