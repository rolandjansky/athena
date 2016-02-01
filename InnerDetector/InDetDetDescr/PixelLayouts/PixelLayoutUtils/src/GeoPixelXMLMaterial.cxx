/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Build detailed stave support : face plate + carbon foam + cable flex + cooling pipe + end blocks
// This is built one time per layer. 

#include "PixelLayoutUtils/GeoPixelXMLMaterial.h"

#include "InDetGeoModelUtils/InDetMaterialManager.h" 

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoSimplePolygonBrep.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"

#include "GeoModelKernel/GeoTransform.h"

#include "PathResolver/PathResolver.h"

#include <algorithm>
using std::max;

GeoPixelXMLMaterial::GeoPixelXMLMaterial(const PixelGeoBuilderBasics* basics, std::string xmlFileName):
  PixelGeoBuilder(basics),
  m_xmlFileName(xmlFileName)
{

}

GeoVPhysVol* GeoPixelXMLMaterial::Build(std::string prefix) {

  std::string file = PathResolver::find_file (m_xmlFileName, "DATAPATH");
  InitializeXML();
  bool bParsed = ParseFile(file);

  if(!bParsed){
    std::cout<<"XML file "<<m_xmlFileName<<" not found"<<std::endl;
    return 0;
  }

  int nbMaterial = getChildCount("PixelMaterials", 0, "Material");
  std::cout<<"MATERIAL NUMBER : "<<nbMaterial<<std::endl;
  
  if(prefix=="none") prefix="pix"; prefix = prefix+"::";

  // --------------------------------------------------------------------------------------------------
  // ADD the material defined in the XML file
  // --------------------------------------------------------------------------------------------------

  std::vector<std::string> materialDefined;
  bool bAllMaterialDefined = false;
  int iCmpt=0;
  while(!bAllMaterialDefined&&iCmpt<5)
    {
      std::cout<<"-- MATERIAL --------------------------------------------------------"<<std::endl;

      for(int iMat=0; iMat<nbMaterial; iMat++)
	{
	  std::string matName = getChildValue("Material", iMat, "name");
	  matName.erase(std::remove(matName.begin(),matName.end(),' '),matName.end());
	  
	  std::string pixMatName=prefix+matName;
	  if(std::find(materialDefined.begin(), materialDefined.end(), pixMatName)!=materialDefined.end())
	    {
	      
	    }
	  else 
	    {
	      double density = atof(getChildValue("Material", iMat, "density").c_str());
	      int nbComp = getChildCount("Material", iMat, "component");

	      bool bMaterialReadyToDefine = true;
	      std::vector<std::string>compNameVec;
	      std::vector<double>compWeightVec;
	      for(int iComp=0; iComp<nbComp; iComp++) {
		
		std::string compName = getChildValue( "Material", iMat, "component", iComp, "name");
		compName.erase(std::remove(compName.begin(),compName.end(),' '),compName.end());
		double compWeight = atof(getChildValue("Material", iMat, "component", iComp, "weight").c_str());

		compNameVec.push_back(compName);
		compWeightVec.push_back(compWeight);

		if(compName.substr(0,5)==prefix){
		  if(std::find(materialDefined.begin(), materialDefined.end(), compName)==materialDefined.end()){
		    bMaterialReadyToDefine = false;
		    //		    std::cout<<"MATERIAL "<<pixMatName<<"    - component not found "<<compName<<std::endl;
		  }
		}

	      }

	      if(bMaterialReadyToDefine)
		{
		  std::cout<<"xxxxxxxxxxxxxxxx MATERIAL - ready to define "<<pixMatName<<"  "<<density<<std::endl;
		  materialDefined.push_back(pixMatName);
		  GeoMaterial* newMat = new GeoMaterial(pixMatName,density*(CLHEP::g/CLHEP::cm3));
		  for(int ii=0; ii<(int)compNameVec.size(); ii++)
		    {
		      std::cout<<"   - get "<<compNameVec[ii]<<std::endl;
		      if(compNameVec[ii].substr(0,5)=="std::"||compNameVec[ii].substr(0,5)=="sct::"||compNameVec[ii].substr(0,5)=="pix::"||compNameVec[ii].substr(0,7)=="indet::")
			{
			  std::string tmp = compNameVec[ii];
			  std::cout<<"   - get material "<<tmp<<std::endl;
			  GeoMaterial *matComp = const_cast<GeoMaterial*>(matMgr()->getMaterial(tmp));
			  std::cout<<"   - get material "<<(matComp==0)<<std::endl;
			  newMat->add(matComp,compWeightVec[ii]);
			}
		      else
			{
			  GeoElement *matComp = const_cast<GeoElement*>(matMgr()->getElement(compNameVec[ii]));
			  newMat->add(matComp,compWeightVec[ii]);
			}
		    }		

		  std::cout<<">>>>>>>> register new material : "<<pixMatName<<std::endl;
		  matMgr()->addMaterial(newMat);


		  const GeoMaterial *matRead = matMgr()->getMaterial(pixMatName);
		  std::cout<<"MATERIAL READ FROM manager : "<<matRead->getName()<<" "<<matRead->getDensity()/(CLHEP::g/CLHEP::cm3)<<std::endl;

		}
	      //	      else
	      //		std::cout<<"MATERIAL - not ready to define "<<matName<<std::endl;

	      //	      std::cout<<"MATERIAL  - ";for(int i=0; i<materialDefined.size(); i++)std::cout<<materialDefined[i]<<" "; std::cout<<std::endl;
	    }
	}
      
      bAllMaterialDefined=((int)materialDefined.size()==nbMaterial);
      iCmpt++;
    }

  // --------------------------------------------------------------------------------------------------
  // ADD the weighted material defined in the XML file
  // --------------------------------------------------------------------------------------------------

  int nbWeightedMaterial = getChildCount("PixelMaterials", 0, "MaterialWeight");
  std::cout<<"WEIGHTED MATERIAL NUMBER : "<<nbMaterial<<std::endl;
  
  std::cout<<"-- MATERIAL --------------------------------------------------------"<<std::endl;
  
  for(int iMat=0; iMat<nbWeightedMaterial; iMat++)
    {
      std::string matName = getChildValue("MaterialWeight", iMat, "name");
      std::string baseName = getChildValue("MaterialWeight", iMat, "base");
      double weight = getDouble("MaterialWeight", iMat, "weight");
      double linearWeight = getInt("MaterialWeight", iMat, "linearweight",0,0);

      matName.erase(std::remove(matName.begin(),matName.end(),' '),matName.end());
      baseName.erase(std::remove(baseName.begin(),baseName.end(),' '),baseName.end());

      std::cout<<">>>>>>>> register new material : "<<prefix<<matName<<std::endl;
      matMgr()->addWeightMaterial(prefix+matName, prefix+baseName, weight, linearWeight);
    }


  return 0;

}
