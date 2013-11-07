/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_SLHC_GeoModel_SCT_GeometryManager_H
#define SCT_SLHC_GeoModel_SCT_GeometryManager_H

#include "SCT_SLHC_GeoModel/SCT_Options.h"
#include "SCT_SLHC_GeoModel/SCT_ParametersBase.h"

namespace InDetDD{
  class SiCommonItems;
}

namespace InDetDDSLHC {

class SCT_BarrelParameters;
class SCT_BarrelModuleParameters;
class SCT_ForwardParameters;
class SCT_ForwardModuleParameters; 
class SCT_GeneralParameters;
class SCT_MaterialManager;
class SCT_DataBase;
class SCT_GeoModelAthenaComps;

class SCT_GeometryManager : public SCT_ParametersBase {

public:

  // Constructor 
  SCT_GeometryManager(const SCT_GeoModelAthenaComps * athenaComps);

  // Destructor 
  ~SCT_GeometryManager();

  // Access to run time options
  const SCT_Options & options();
  void setOptions(const SCT_Options & options);
  
  // Service locator
  InDetDD::SiCommonItems * commonItems() const;
  void setCommonItems(InDetDD::SiCommonItems * commonItems); 

  const SCT_BarrelParameters          * barrelParameters() const;
  const SCT_BarrelModuleParameters    * barrelModuleParameters() const;
  const SCT_ForwardParameters         * forwardParameters() const;
  const SCT_ForwardModuleParameters   * forwardModuleParameters() const;
  const SCT_GeneralParameters         * generalParameters() const;
  const SCT_DataBase                  * sctdb() const;
  SCT_MaterialManager                 * materialManager() const;

private:

  SCT_Options m_options;
  InDetDD::SiCommonItems * m_commonItems;
 
  mutable const SCT_BarrelParameters          * m_barrelParameters;
  mutable const SCT_BarrelModuleParameters    * m_barrelModuleParameters;
  mutable const SCT_ForwardParameters         * m_forwardParameters;
  mutable const SCT_ForwardModuleParameters   * m_forwardModuleParameters;
  mutable const SCT_GeneralParameters         * m_generalParameters;
  mutable SCT_MaterialManager                 * m_materialManager;
  mutable const SCT_DataBase                  * m_sctdb;

  bool m_old;

};
}
#endif // SCT_SLHC_GeoModel_SCT_GeometryManager_H
