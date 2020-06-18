/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef InDetServMatBuilderToolSLHC_H
#define InDetServMatBuilderToolSLHC_H

#include "InDetGeoModelUtils/IInDetServMatBuilderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>
#include <list>

class GeoMaterial;
class PixelGeoModelAthenaComps;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class InDetServMatGeometryManager;
class ServiceVolume;
class InDetMaterialManager;

namespace InDetDD {
  class AthenaComps;
}
/**
   Service material for the inner tracker.
   The number and type of services per stave is computed by ComputeStaveServices, except for the cooling, which can be
   manifolded, and is computed later by ConvertStaveServices.
   All services are converted to names known to the InDetMaterialManager (and to the DB) by ConvertStaveServices, the result
   of which is a ServiceMaterial instance. 
   ServiceMaterial is a container of Entry items, each entry containing
    - material name
    - number of occurances (e.g. number of cables of the type given by "material name" that cross a service volume)
    - the weight
    - a flag specifying if the weight is per unit of length (in kg/m) or absolute (in kg)
   
   There are two ways to specify service materials:
    - materials present in the weight table of the MaterialManager, e.g. TwinaxCable. For these materials the weight 
      is taken from the weight table, and the weight specified in the ServiceMaterial::Entry _must_ be set to 1
    - materials for which the weight is computed by ConvertStaveServices as a function of the pipe diameter or the 
      wire gauge. For these materials there should be no entry in the weight tables, and the name specified in 
      ServiceMaterial::Entry refers to a normal GeoMaterial. The density of this material is ignored, since the weight
      is already computed, and only the chemical composition is used from the MaterialManager (and the DB)
 */

class InDetServMatBuilderToolSLHC : virtual public IInDetServMatBuilderTool, public AthAlgTool
{

public:
  /// Constructor
  InDetServMatBuilderToolSLHC(const std::string&,const std::string&,const IInterface*);	    

  /// default destructor
  virtual ~InDetServMatBuilderToolSLHC();
  
  /// standard Athena-Algorithm method
  virtual StatusCode initialize();
  
  /// standard Athena-Algorithm method
  virtual StatusCode finalize();  

  /// Get the services
  virtual const std::vector<const InDetDD::ServiceVolume *> & getServices() const;
  
  /// General service adding method.
  void addService(InDetDD::ServiceVolume * param);
 
  /// Add tube like service after creating material
  void addService(const std::string & name, const GeoMaterial * material, 
		  double rmin, double rmax, double zmin, double zmax, bool zsymm = true);

  /// Add tube-like service with predefined material.
  void addService(const std::string & name, const std::string & materialName, 
		  double rmin, double rmax, double zmin, double zmax, bool zsymm = true);
  
  void addServiceVolume( const ServiceVolume& vol);

private:

  /// Build the geometry.
  void build();

  /// Initialization that must be done after geometry is known
  void geoInit();


  /// Properties
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;


  bool m_init;
  InDetDD::AthenaComps * m_athenaComps;
  InDetServMatGeometryManager * m_geoMgr;
  
  std::vector<const InDetDD::ServiceVolume *> m_services;

  void printNewVolume( const ServiceVolume& vol, const GeoMaterial& mat,
		       const InDetDD::ServiceVolume& param) const;

  //std::string getKnownName( const std::string& name, InDetMaterialManager* matMgr);
  void fixMissingMaterials();

  std::string addPrefix( const std::string& prefix, const std::string& name) const;
  void addMissingMaterial( const std::string& name, 
			   const std::vector<std::string>& comps, const std::vector<double>& fracs, double dens=1.0);

};


#endif //InDetServMatBuilderToolSLHC_H
 
