/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS_REGIONCREATOR_H
#define G4ATLASTOOLS_REGIONCREATOR_H

// Base classes
#include "G4AtlasInterfaces/IRegionCreator.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/SystemOfUnits.h"

// STL library
#include <string>

/** @class RegionCreator RegionCreator.h "G4AtlasTools/RegionCreator.h"
 *
 *  Concrete Tool to create G4 Physics Regions
 *
 *  @author ADA
 *  @date   2015-03-23
 */

class RegionCreator final :  public extends<AthAlgTool, IRegionCreator> {
public:
  // Standard constructor and destructor
  RegionCreator(const std::string& type, const std::string& name, const IInterface *parent);
  ~RegionCreator() {}

  StatusCode initialize();

  void Dump();

private:
   Gaudi::Property<std::string> m_regionName{this, "RegionName", "", "Region name (same as the Tool name if not set)"};
  Gaudi::Property<double> m_gammaCut{this, "GammaCut", 1.*Gaudi::Units::mm, "Cut to be applied for gammas"};
  Gaudi::Property<double> m_electronCut{this, "ElectronCut", 1., "Cut to be applied for electrons"};
  Gaudi::Property<double> m_positronCut{this, "PositronCut", 1., "Cut to be applied for positrons"};
  Gaudi::Property<double> m_protonCut{this, "ProtonCut", 1., "Cut to be applied for gammas"};
  Gaudi::Property<std::vector<std::string> > m_logicalVolumes{this, "VolumeList", {}, "List of volumes to be included in this region"};
};

#endif
