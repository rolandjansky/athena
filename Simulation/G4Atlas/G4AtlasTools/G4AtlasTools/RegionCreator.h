/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_REGIONCREATOR_H 
#define G4ATLASTOOLS_REGIONCREATOR_H

// Base classes
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "AthenaBaseComps/AthAlgTool.h"

// STL library
#include <string>

/** @class RegionCreator RegionCreator.h "G4AtlasTools/RegionCreator.h"
 *  
 *  Concrete Tool to create G4 Physics Regions
 * 
 *  @author ADA
 *  @date   2015-03-23
 */

class RegionCreator final :  public AthAlgTool, virtual public IRegionCreator {
public:
  // Standard constructor and destructor
  RegionCreator(const std::string& type, const std::string& name, const IInterface *parent);
  ~RegionCreator() {}

  StatusCode initialize();
  
  void Dump();
  
  /** Query interface method to make athena happy */
  virtual StatusCode queryInterface(const InterfaceID&, void**);
  
private:
  std::string m_regionName;

  double m_gammaCut=1.;
  double m_electronCut=1.;
  double m_positronCut=1.;
  double m_protonCut=1.;
  
  std::vector<std::string> m_logicalVolumes;
};

#endif
