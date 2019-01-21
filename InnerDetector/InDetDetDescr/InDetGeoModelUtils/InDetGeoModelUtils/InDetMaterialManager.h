/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETMATERIALMANAGER_H
#define INDETMATERIALMANAGER_H

// Message Stream Member
#include "AthenaKernel/MsgStreamMember.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class GeoMaterial;
class GeoElement;
class AbsMaterialManager;
class StoreGateSvc;
class IGeometryDBSvc;

namespace InDetDD
{
  class AthenaComps;
}

#include <string>
#include <map>

/// InDetMaterialManager. This provides an interface to the GeoModel Material Manager
/// as well as allowing additional materials to be defined or standard ones redefined.
/// It also allows creating new materials based on existing ones but with a different 
/// density. It is also possible to specify a weight table and this is used to create
/// materials with a density such that the the total weight is correct.


class InDetMaterialManager 
{

public:

  InDetMaterialManager(const std::string & managerName, StoreGateSvc* detStore);
  InDetMaterialManager(const std::string & managerName, StoreGateSvc* detStore,
		       IRDBRecordset_ptr weightTable,
		       const std::string & space = "",
		       bool extraFunctionality = false);
  InDetMaterialManager(const std::string & managerName, StoreGateSvc* detStore,
		       IRDBRecordset_ptr weightTable,
		       IRDBRecordset_ptr compositionTable,
		       const std::string & space = "");
  InDetMaterialManager(const std::string & managerName, 
		       const InDetDD::AthenaComps *);
  ~InDetMaterialManager();

  void addWeightTable(IRDBRecordset_ptr weightTable, const std::string & space = "");
  void addWeightMaterial(std::string materialName, std::string materialBase, double weight, int linearWeightFlag);
  void addCompositionTable(IRDBRecordset_ptr compositionTable, const std::string & space = "");
  void addScalingTable(IRDBRecordset_ptr scalingTable);


  bool hasMaterial(const std::string &materialName) const;

  /// Get material. First looks for locally defined material and if not found looks in GeoModel material manager.
  const GeoMaterial* getMaterial(const std::string & materialName);

  /// Get element from GeoModel material manager
  const GeoElement* getElement(const std::string & elementName) const;

  /// Create and get material with a specified density based on an existing material.
  /// If a newName is supplied it creates the new material even if the orginal material
  /// has the same density. It however first checks if the material with NewName exists.
  /// If no newName is supplied then it checks the density of
  /// the existing material. If it is consistent it returns the material.
  /// If it is different it creates a material with the string "Modified" added to the
  /// name.
  const GeoMaterial* getMaterial(const std::string & origMaterialName, 
				 double density, 
				 const std::string & newName = "");

  // Creates a new material based on origMaterialName but with denisty scaled 
  // by scaleFactor. If no newName then will add the string containing the scale 
  // factor. Eg if scale 12.345678 "Silicon" -> "Silicon12_3456" 
  // If the scale factor is 1 and there is no newName then it just returns the
  // original material.
  // scaleFactor must be between 0.001 and 1000.
  const GeoMaterial* getMaterialScaled(const std::string & origMaterialName, 
				       double scaleFactor, 
				       const std::string & newName = "");

  /// Create and get material with a density calculated to give weight in predefined weight table.
  const GeoMaterial * getMaterialForVolume(const std::string & materialName, 
					   double volume, 
					   const std::string & newName = "");

  // Similar to getMaterialForVolume but if weight table uses linear weight, then determine weight 
  // using length. First looks in special table of material compositions which can specify several
  // components and their count. 
  const GeoMaterial * getMaterialForVolumeLength(const std::string & materialName, 
						 double volume, 
						 double length, 
						 const std::string & newName = "");

  // As above but rather than using the special table of material compositions, the compositions is specified
  // in the arguments as a vector of materials and multiplictive factors.
  const GeoMaterial * getMaterialForVolumeLength(const std::string & name,
						 const std::vector<std::string> & materialComponents, 
						 const std::vector<double> factors, 
						 double volume, 
						 double length);
  
  // As above but only one material making up the composition.
  const GeoMaterial * getMaterialForVolumeLength(const std::string & name,
						 const std::string & materialComponent, 
						 double factor, 
						 double volume, 
						 double length);

  // Define composite material : function used to create dynamically a new composite material by adding
  //            a defined volume of glue/grease to an already existing material  (IBL stave)
  const GeoMaterial * getCompositeMaterialForVolume(const std::string & newMatName,
						    const double volumeTot,
						    const double volume1, const std::string & matName1,
						    const double volume2, const std::string & matName2
						    );

  // Define a new material composition.
  const GeoMaterial * getMaterial(const std::string & name,
				  const std::vector<std::string> & materialComponents, 
				  const std::vector<double> & fractWeights, 
				  double density);


  /// Add material
  void addMaterial(GeoMaterial *material);

  //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_msg << lvl; }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl){ return m_msg.get().level() <= lvl; }


private:

  class MaterialByWeight  {
  public:
    MaterialByWeight() : weight(0), linearWeightFlag(false) {} 
    MaterialByWeight(const std::string & name_in, double weight_in, bool linearWeightFlag_in) 
      : name(name_in), weight(weight_in), linearWeightFlag(linearWeightFlag_in) {}
    MaterialByWeight(double weight_in) 
      : weight(weight_in), linearWeightFlag(false) {}
    std::string name;
    double weight;
    bool linearWeightFlag;
  };

  class MaterialComponent {
  public:
    MaterialComponent() : factor(1), actualLength(-1) {}
    MaterialComponent(const std::string & name_in, double factor_in = 1, bool actualLength_in = -1) 
      :  name(name_in), factor(factor_in), actualLength(actualLength_in) {}
    std::string name;
    double factor;
    double actualLength;
  };

  /// Class to hold information need to create a material
  class MaterialDef {
  public:
    MaterialDef();
    MaterialDef(const std::string & name, double density);
    void addComponent(const std::string & compName, double fraction);
    void setCreated() {m_created = true;}
    unsigned int numComponents() const {return m_components.size();}
    bool isCreated() const {return m_created;}
    const std::string & name() const {return m_name;}
    double density() const {return m_density;}
    const std::string & compName(unsigned int i) const {return m_components[i];}
    double fraction(unsigned int i) const {return m_fractions[i];}
    double totalFraction() const;

  private:
    std::string m_name;
    double m_density;
    std::vector<std::string> m_components;
    std::vector<double> m_fractions;
    bool m_created;
  };
  
  
  const AbsMaterialManager * retrieveManager(StoreGateSvc* detStore);
  const GeoMaterial* getAdditionalMaterial(const std::string & materialName) const; 
  bool compareDensity(double d1, double d2) const;
  void addWeightTableOld(IRDBRecordset_ptr weightTable, const std::string & space);

  // Internal versions. The public versions allow materials to be have extra scaling.
  const GeoMaterial* getMaterialInternal(const std::string & materialName) const;
  const GeoMaterial* getMaterialInternal(const std::string & origMaterialName, 
					 double density, 
					 const std::string & newName = "");
  const GeoMaterial* getMaterialScaledInternal(const std::string & origMaterialName, 
					       double scaleFactor, 
					       const std::string & newName = "");
  const GeoMaterial * getMaterialInternal(const std::string & name,
					  const std::vector<std::string> & materialComponents, 
					  const std::vector<double> & fractWeights, 
					  double density);

  // Methods to return material with extra scaling.
  const GeoMaterial * extraScaledMaterial(const std::string & materialName, 
					  const std::string & newName, 
					  const GeoMaterial * origMaterial);

  const GeoMaterial * extraScaledMaterial(const std::string & materialName, 
					  const GeoMaterial * origMaterial);

  const IGeometryDBSvc * db();
  void addTextFileMaterials();
  void createMaterial(const MaterialDef & material);
  double getExtraScaleFactor(const std::string & materialName);

  const AbsMaterialManager *m_materialManager;
  std::string m_managerName;

  typedef std::map<std::string, const GeoMaterial *> MaterialStore;
  MaterialStore m_store;

  typedef std::map<std::string, MaterialByWeight > MaterialWeightMap;
  MaterialWeightMap m_weightMap;

  typedef std::map<std::string, MaterialComponent > MaterialCompositionMap;
  MaterialCompositionMap m_matCompositionMap;

  typedef std::map<std::string, double > ExtraScaleFactorMap;
  ExtraScaleFactorMap m_scalingMap;

  //Declaring private message stream member.
  mutable Athena::MsgStreamMember m_msg;

  // Has linear weight flag. 
  bool m_extraFunctionality;

  const InDetDD::AthenaComps * m_athenaComps;

};


#endif // INDETMATERIALMANAGER_H
