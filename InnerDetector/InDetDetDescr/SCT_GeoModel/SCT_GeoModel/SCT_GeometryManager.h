/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_GeometryManager_H
#define SCT_GeoModel_SCT_GeometryManager_H

#include "SCT_GeoModel/SCT_Options.h"

namespace InDetDD{
  class SiCommonItems;
  class DistortedMaterialManager;
}

class SCT_BarrelParameters;
class SCT_BarrelModuleParameters;
class SCT_ForwardParameters;
class SCT_ForwardModuleParameters; 
class SCT_GeneralParameters;
class SCT_GeoModelAthenaComps;

class SCT_GeometryManager {

public:

  // Constructor 
  SCT_GeometryManager();

  // Destructor 
  ~SCT_GeometryManager();

  // Access to run time options
  const SCT_Options & options() const;
  void setOptions(const SCT_Options & options);

  // Access to athena components
  const SCT_GeoModelAthenaComps * athenaComps() const;
  void setAthenaComps(const SCT_GeoModelAthenaComps * athenaComps);

  // To be passed to detector element.
  InDetDD::SiCommonItems * commonItems() const;
  void setCommonItems(InDetDD::SiCommonItems * commonItems); 

  const SCT_BarrelParameters          * barrelParameters() const;
  const SCT_BarrelModuleParameters    * barrelModuleParameters() const;
  const SCT_ForwardParameters         * forwardParameters() const;
  const SCT_ForwardModuleParameters   * forwardModuleParameters() const;
  const SCT_GeneralParameters         * generalParameters() const;
  const InDetDD::DistortedMaterialManager  * distortedMatManager() const;

private:

  SCT_Options m_options;
  const SCT_GeoModelAthenaComps * m_athenaComps;
  InDetDD::SiCommonItems * m_commonItems;
 
  mutable const SCT_BarrelParameters          * m_barrelParameters;
  mutable const SCT_BarrelModuleParameters    * m_barrelModuleParameters;
  mutable const SCT_ForwardParameters         * m_forwardParameters;
  mutable const SCT_ForwardModuleParameters   * m_forwardModuleParameters;
  mutable const SCT_GeneralParameters         * m_generalParameters;
  mutable const InDetDD::DistortedMaterialManager  * m_distortedMatManager;

};


#endif // SCT_GeoModel_SCT_GeometryManager_H
