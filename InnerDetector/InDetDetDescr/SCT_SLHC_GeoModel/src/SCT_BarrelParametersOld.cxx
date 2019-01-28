/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BarrelParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <cstring>

namespace InDetDDSLHC {

//split strings
void barrelSplitString(const std::string& str, std::vector<std::string>& str_vec, 
		 const std::string& delimiters = "|"){
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
  
  while (std::string::npos != pos || std::string::npos != lastPos)
    {
      // Found a str_vec, add it to the vector.
      str_vec.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

SCT_BarrelParametersOld::SCT_BarrelParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_BarrelParameters(sctdb, athenaComps)
{
  std::cout<<"----------------------Read Local Databse Layers's Parameters-------------------" 
	   <<std::endl;
  //open the SLHC SCT local database text file
  char* textFileName = getenv ("LocalSlhcGeometryDatabase");

  if(textFileName == NULL){
    std::cerr<<"SCT_BarrelParameters::SCT_BarrelParameters: LocalSlhcGeometryDatabase not defined! stop athena"<<std::endl;
    exit(1);
  }

  FILE* db_iumputfile;
  db_iumputfile = fopen(textFileName, "r");
  if(db_iumputfile == NULL){
    std::cerr<<"ERROR: couldn't open "<<textFileName<<std::endl;
    exit(1);
  }
  const int LINELEN = 5000;
  const int NAMELEN = 500;
  int ilayer = -1;
  char line[LINELEN];
  char name[NAMELEN];
  float value;
  char value_strng[LINELEN];
  unsigned int modules_per_ski = 0;
  //container of SCT Barrel Parameters.
  m_SCT_Parameters = new std::map< std::string, float>();
  m_SCT_Parameters_Strng = new std::map<std::string, std::string>();
  //container of modules position and ids
  m_modules_position = new std::vector<std::vector<float>*>();
  m_modules_id = new std::vector<std::vector<int>*>();
  //first read layer parameters then read the other stuff parameters
  while(!feof(db_iumputfile)){
    //read line by line
    fgets(line, sizeof(line), db_iumputfile);
    //std::cout<<"line = "<<line<<std::endl;
    //escape if the line is a comment
    if(strstr(line, "//") != NULL){
      continue;
    }
    //if not a comment, then get the name and the value of the parameter
    sscanf(line, "%499s %99f", name, &value);
    //exit when reaching the Module parameters part
    if(strstr(name, "BRL_PARAMETERS_END") != NULL) break;
    //put string paramters in a different container
    if(strstr(name, "_STRNG") != NULL){
      //re-read the second parameter as a string
      sscanf(line, "%499s %4999s", name, value_strng);
      (*m_SCT_Parameters_Strng)[std::string(name)] = std::string(value_strng);
    }
    //layer number
    else if(strstr(name, "_LAYER") != NULL) ilayer = (int)value;
    //save the number of modules per ski for later use(see below)
    else if(strstr(name, "_NMODSKI") != NULL) modules_per_ski = (int)value;
    //this is the line which contains the modules position in a ski (row)
    else if(strstr(name, "_MODPOS") != NULL){
      //re-anlyse this line: get module positions
      char name_tmp[NAMELEN];
      char value_strg[LINELEN];
      sscanf(line, "%499s %4999s", name_tmp, value_strg);
      //module positions are code in a string (see the txt config file)
      std::vector<std::string> vec_str;
      //a delimeter "|" is used!
      barrelSplitString(std::string(value_strg), vec_str, "|");
      if(vec_str.size() != modules_per_ski){
	std::cerr<<"ERROR in SCT_BarrelParameters::SCT_BarrelParameters(): layer "
		 <<ilayer<<", number of "
		 <<"modules position in MODPOS "<<vec_str.size()
		 <<" is not what is specified "
		 <<"in NMODSKI "<<modules_per_ski<<" !"<<std::endl;
	exit(1);
      }
      //extract the real numbers (positions)
      std::vector<float>* mod_pos = new std::vector<float>();
      //---dbg std::cout<<"Module position: ";
      for(unsigned int i=0; i<modules_per_ski; i++){
	float pos;
	sscanf(vec_str[i].c_str(), "%99f", &pos);
	//---dbg std::cout<<" "<<pos;
	mod_pos->push_back(pos);
      }
      //---dbg std::cout<<std::endl;
      m_modules_position->push_back(mod_pos);
      //don't put this line in the map: escape the rest!
      continue;
    }//end of else if(strstr(name, "_MODPOS") ...)
    //this is the line which contains the modules ids 
    else if(strstr(name, "_MODID") != NULL){
      //re-anlyse this line: get module id
      char name_tmp[NAMELEN];
      char value_strg[LINELEN];
      sscanf(line, "%499s %4999s", name_tmp, value_strg);
      //module ids are code in a string (see the txt config file)
      std::vector<std::string> vec_str;
      //a delimeter "|" is used!
      barrelSplitString(std::string(value_strg), vec_str, "|");
      if(vec_str.size() != modules_per_ski){
	std::cerr<<"ERROR in SCT_BarrelParameters::SCT_BarrelParameters(): number of "
		 <<"modules id in MODID "<<vec_str.size()<<" is not what is pecified "
		 <<"in NMODSKI "<<modules_per_ski<<" !"<<std::endl;
	exit(1);
      }
      //extract the real numbers (ids)
      std::vector<int>* mod_id = new std::vector<int>();
      std::cout<<"Module id: ";
      for(unsigned int i=0; i<modules_per_ski; i++){
	int id;
	sscanf(vec_str[i].c_str(), "%99d", &id);
	std::cout<<" "<<id;
	mod_id->push_back(id);
      }
      std::cout<<std::endl;
      m_modules_id->push_back(mod_id);
      //don't put this line in the map: escape the rest!
      continue;
    }//end of else if(strstr(name, "_MODID") != NULL)
    //fill the map (this should correspond to layers
    (*m_SCT_Parameters)[std::string(name)] = value;
  }
  //close the config file
  fclose(db_iumputfile);
  
  //print the parameters for test
  std::map<std::string, float>::iterator map_itr;
  for(map_itr=m_SCT_Parameters->begin(); map_itr!=m_SCT_Parameters->end(); map_itr++){
    std::cout<<"----SCT_Barrel_Parameters: first = "<<map_itr->first 
	     <<"  second = "<<map_itr->second<<std::endl;
  }
  //print the modules position
  std::cout<<"-----------------Modules positions----------------"<<std::endl;
  for(unsigned int i=0; i<m_modules_position->size(); i++){
    std::cout<<"Layer["<<i<<"] ";
    for(unsigned int j=0; j<(*m_modules_position)[i]->size(); j++)
      std::cout<<(*(*m_modules_position)[i])[j]<<" ";
    std::cout<<std::endl;
  }
  //print the modules id
  std::cout<<"-----------------Modules ids----------------"<<std::endl;
  for(unsigned int i=0; i<m_modules_id->size(); i++){
    std::cout<<"Layer["<<i<<"] ";
    for(unsigned int j=0; j<(*m_modules_id)[i]->size(); j++)
      std::cout<<(*(*m_modules_id)[i])[j]<<" ";
    std::cout<<std::endl;
  }
  std::cout<<"-------------------End of Read Local Databse Parameters-------------"<<std::endl;
}

SCT_BarrelParametersOld::~SCT_BarrelParametersOld(){
  delete m_SCT_Parameters;
  delete m_SCT_Parameters_Strng;
  //container of modules position and ids
  for(unsigned int ii=0; ii<m_modules_position->size(); ii++)
    delete (*m_modules_position)[ii];
  delete m_modules_position;
  for(unsigned int ii=0; ii<m_modules_id->size(); ii++)
    delete (*m_modules_id)[ii];
  delete m_modules_id;
}


int 
SCT_BarrelParametersOld::layoutType() const{
  return (int)(m_SCT_Parameters->find("LAYOUTTYPE"))->second;
}

bool SCT_BarrelParametersOld::doEndCaps() const{
  return (bool)(m_SCT_Parameters->find("DOENDCAPS"))->second;
}

double SCT_BarrelParametersOld::materialIncreaseFactor(int ilayer) const {
  char paraName[50];
  sprintf(paraName, "L%d_MATERIALINCREASEFACTOR", ilayer);
  return (float)(m_SCT_Parameters->find(paraName))->second;
}

//
// Barrel Ski
//
int 
SCT_BarrelParametersOld::skiFirstStagger() const{
  return -1;
}

double 
SCT_BarrelParametersOld::skiRadialSep(int ilayer) const{
  // return 2.8*Gaudi::Units::mm;//mm
  char paraName[50];
  sprintf(paraName, "L%d_MODULESRADIALSEPARATION", ilayer);
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::mm;
}

int
SCT_BarrelParametersOld::moduleType(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_MODTYPE", ilayer);
  return (int)(m_SCT_Parameters->find(paraName))->second;
}
int
SCT_BarrelParametersOld::modulesPerSki(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_NMODSKI", ilayer);
  return (int)(m_SCT_Parameters->find(paraName))->second;
}


double 
SCT_BarrelParametersOld::skiZPosition(int ilayer, int module) const{
  return (*(*m_modules_position)[ilayer])[module];
}

int 
SCT_BarrelParametersOld::skiModuleIdentifier(int ilayer, int module) const{
  return (*(*m_modules_id)[ilayer])[module];
}

//
// Barrel Layer
//
double 
SCT_BarrelParametersOld::tilt(int ilayer) const{
  char paraName[50];
 sprintf(paraName, "L%d_TILT", ilayer);
  std::cout<<"---------2 tilt layer TILT("<<ilayer<<") = "<<(m_SCT_Parameters->find(paraName))->second*Gaudi::Units::degree<<std::endl;
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::degree;
}

double SCT_BarrelParametersOld::radius(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_RLAY", ilayer);
  std::cout<<"---------2 radius layer RLAY("<<ilayer<<") = "<<(m_SCT_Parameters->find(paraName))->second*Gaudi::Units::cm<<std::endl;
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::cylLength(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_CYLLENTGH", ilayer);
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::cm;
}

bool SCT_BarrelParametersOld::doubleSided(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_DOUBLESIDED", ilayer);
  int doubleSided = (int) (m_SCT_Parameters->find(paraName))->second;
  if(doubleSided == 1) return true;
  else return false;
}

int 
SCT_BarrelParametersOld::skisPerLayer(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_NMODPHI", ilayer);
  return (int) (m_SCT_Parameters->find(paraName))->second;
}

int 
SCT_BarrelParametersOld::layerStereoSign(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_LAYTYP", ilayer);
  std::cout<<"---------2 layerStereoSign layer LAYTYP("<<ilayer<<") = "<<(int) (m_SCT_Parameters->find(paraName))->second<<std::endl;
  return (int) (m_SCT_Parameters->find(paraName))->second;
}


double 
SCT_BarrelParametersOld::supportCylInnerRadius(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_RISUP", ilayer);
  std::cout<<"---------2 supportCylInnerRadius RISUP("<<ilayer<<") = "<<(m_SCT_Parameters->find(paraName))->second*Gaudi::Units::cm<<std::endl;
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::supportCylOuterRadius(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_ROSUP", ilayer);
  std::cout<<"---------2 supportCylOuterRadius ROSUP("<<ilayer<<") = "<<(m_SCT_Parameters->find(paraName))->second*Gaudi::Units::cm<<std::endl;
  return (m_SCT_Parameters->find(paraName))->second * Gaudi::Units::cm;
}
std::string 
SCT_BarrelParametersOld::supportCylMaterial(int ilayer) const{
  char paraName[50];
  sprintf(paraName, "L%d_SUPPORTCYLMATERIAL_STRNG", ilayer);
  std::cout<<"---------2  SUPPORTCYLMATERIAL_STRNG("<<ilayer<<") = "<<(m_SCT_Parameters_Strng->find(paraName))->second<<std::endl;
  return (m_SCT_Parameters_Strng->find(paraName))->second;
}
//
// Barrel General
//
//number of SCT layer (intermediate + outer)
int
SCT_BarrelParametersOld::numLayers() const{
  return (int) (m_SCT_Parameters->find("NLAY"))->second;
}

double 
SCT_BarrelParametersOld::barrelInnerRadius() const{
  std::cout<<"---------2 barrelInnerRadius RMIN = "<<(m_SCT_Parameters->find("B_RMIN"))->second * Gaudi::Units::cm<<std::endl;
  
  return (m_SCT_Parameters->find("B_RMIN"))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::barrelIntermediateRadius() const{
  std::cout<<"---------2 barrelIntermediateRadius RINTERMEDIATE = "<<(m_SCT_Parameters->find("B_RINTERMEDIATE"))->second * Gaudi::Units::cm<<std::endl;
  
  return (m_SCT_Parameters->find("B_RINTERMEDIATE"))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::barrelOuterRadius() const{
  std::cout<<"---------2 barrelOuterRadius RMAX = "<<(m_SCT_Parameters->find("B_RMAX"))->second * Gaudi::Units::cm<<std::endl;
  
  return (m_SCT_Parameters->find("B_RMAX"))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::barrelLength() const{
  std::cout<<"---------2 barrelLength B_LEN = "
	   <<(m_SCT_Parameters->find("B_LEN"))->second * Gaudi::Units::cm<<std::endl;
  
  return (m_SCT_Parameters->find("B_LEN"))->second * Gaudi::Units::cm;
}

double 
SCT_BarrelParametersOld::barrelIntermediateLength() const{
  std::cout<<"---------2 barrelIntermediateLength B_IntermediateLEN = "
	   <<(m_SCT_Parameters->find("B_INTERMEDIATELEN"))->second * Gaudi::Units::cm<<std::endl;
  
  return (m_SCT_Parameters->find("B_INTERMEDIATELEN"))->second * Gaudi::Units::cm;
}

double SCT_BarrelParametersOld::barrelServicesMaterialCylinderLength() const {
  char paraName[50];
  sprintf(paraName, "BARRELSERVICESMATERIALCYLINDERLENGTH");
  return (double)(m_SCT_Parameters->find(paraName))->second * Gaudi::Units::cm;
}

double SCT_BarrelParametersOld::barrelServicesMaterialIncreaseFactor() const {
  char paraName[50];
  sprintf(paraName, "BARRELSERVICESMATERIALINCREASEFACTOR");
  return (float)(m_SCT_Parameters->find(paraName))->second;
}

std::string 
SCT_BarrelParametersOld::barrelServicesMaterial() const{
  char paraName[50];
  sprintf(paraName, "BARRELSERVICESMATERIAL_STRNG");
  return (m_SCT_Parameters_Strng->find(paraName))->second;
}
}
