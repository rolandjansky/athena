/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"

#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>
#include <cmath>
#include <cstring>

namespace InDetDDSLHC {

//split strings
void fwdSplitString(const std::string& str, std::vector<std::string>& str_vec, 
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

SCT_ForwardParametersOld::SCT_ForwardParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ForwardParameters(sctdb, athenaComps)
{
  //m_rdb = SCT_DataBase::instance();
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
  char line[LINELEN];
  char name[NAMELEN];
  float value;
  char value_strng[5000];
  int iwheel = -1;
  unsigned int rings_per_wheel = 0;
  //container of SCT Barrel Parameters.
  m_SCT_Fwd_Parameters = new std::map<std::string, float>();
  m_SCT_Fwd_Parameters_Strng = new std::map<std::string, std::string>();
  m_wheels_rings = new std::vector<std::vector<unsigned int>*>();
  bool sct_fwd_parameters = false;
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
    //escape lines before the FWD PARAMETERS section (in the txt file)
    if(!sct_fwd_parameters){
      if(!strcmp(name, "SCT_FWD_PARAMETERS_BEGIN")) sct_fwd_parameters = true;
      continue;
    }
    //stop reading when reaching the next section (module section)
    else if(strstr(name, "SCT_FWD_PARAMETERS_END") != NULL) break;
    //------------------------------------------------------------------------------
    //save wheel number
    else if(strstr(name, "_WHEEL") != NULL) iwheel = (int)value;
    //save the number of rings per wheel for later use(see below)
    else if(strstr(name, "_NUMRINGS") != NULL) rings_per_wheel = (int)value;
    //this is the line which contains the wheel rings (special treat of this line)
    else if(strstr(name, "_RINGS") != NULL){
      //re-anlyse this line: get module positions
      char name_tmp[NAMELEN];
      char value_strg[LINELEN];
      sscanf(line, "%499s %4999s", name_tmp, value_strg);
      //module positions are code in a string (see the txt config file)
      std::vector<std::string> vec_str;
      //a delimeter "|" is used!
      fwdSplitString(std::string(value_strg), vec_str, "|");
      if(vec_str.size() != rings_per_wheel){
	std::cerr<<"ERROR in SCT_ForwardParameters::"
		 <<"SCT_ForwardParameters(): wheel "
		 <<iwheel<<", number of "
		 <<"modules position in  W"<<iwheel<<"_WHEELRINGS "
		 <<vec_str.size()
		 <<" is not what is specified "
		 <<"in W"<<iwheel<<"_NUMRINGS "<<rings_per_wheel
		 <<" !"<<std::endl;
	exit(1);
      }
      //extract the real numbers (positions)
      std::vector<unsigned int>* wheel_rings = new std::vector<unsigned int>();
      //std::cerr<<"Ring Types: "<<iwheel<<" ";
      for(unsigned int i=0; i<rings_per_wheel; i++){
	unsigned int ring;
	sscanf(vec_str[i].c_str(), "%99u", &ring);
	//std::cerr<<" "<<ring;
	wheel_rings->push_back(ring);
      }
      std::cerr<<std::endl;
      m_wheels_rings->push_back(wheel_rings);
      //don't put this line in the map: escape the rest!
      continue;
    }//end of else if(strstr(name, "_"WHEELRINGS) ...)
    //------------------------------------------------------------------------------
    //---else{
    //put string paramters in a different container
    if(strstr(name, "_STRNG") != NULL){
      //re-read the second parameter as a string
      sscanf(line, "%499s %4999s", name, value_strng);
      (*m_SCT_Fwd_Parameters_Strng)[std::string(name)] = std::string(value_strng);
    }
    //fill the parameters map with the other parameters
    else (*m_SCT_Fwd_Parameters)[std::string(name)] = value;
    //---}
  }//end of while(!feof(db_iumputfile))
  //close the config file
  fclose(db_iumputfile);
  
  //print the parameters for test
  std::map<std::string, float>::iterator map_itr;
  std::cout<<"-------------------Begin of SCT Forward Wheels Parameters-------------"<<std::endl;
  for(map_itr=m_SCT_Fwd_Parameters->begin(); map_itr!=m_SCT_Fwd_Parameters->end(); map_itr++){
    std::cout<<"----SCT_ForwardParameters: parameter = "<<map_itr->first 
	     <<",  values = "<<map_itr->second<<std::endl;
  }
  std::cout<<"-------------------End of SCT Forward Wheels Parameters---------------"<<std::endl;
}


SCT_ForwardParametersOld::~SCT_ForwardParametersOld(){
  delete m_SCT_Fwd_Parameters;
  delete m_SCT_Fwd_Parameters_Strng;
  //container of modules position and ids
  for(unsigned int ii=0; ii<m_wheels_rings->size(); ii++)
    delete (*m_wheels_rings)[ii];
  delete m_wheels_rings;
}

int 
SCT_ForwardParametersOld::layoutType() const
{
  return (int)(m_SCT_Fwd_Parameters->find("LAYOUTTYPE"))->second;
}
//
// Forward Wheel
//
double SCT_ForwardParametersOld::materialIncreaseFactor(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_MATERIALINCREASEFACTOR", iWheel);
  return (float)(m_SCT_Fwd_Parameters->find(paraName))->second;
}

int
SCT_ForwardParametersOld::fwdNumWheels() const
{
  return (int)(m_SCT_Fwd_Parameters->find("NUMWHEELS"))->second;
  //return m_rdb->fwdGeneral()->getInt("NUMWHEELS");
}


double
SCT_ForwardParametersOld::fwdWheelZPosition(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_DISKZPOSITION", iWheel);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
  //return m_rdb->fwdWheel(iWheel)->getDouble("ZPOSITION") * Gaudi::Units::mm;
}

int
SCT_ForwardParametersOld::fwdWheelNumRings(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_NUMRINGS", iWheel);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
  // return m_rdb->fwdWheel(iWheel)->getInt("NUMRINGS");
}

int 
SCT_ForwardParametersOld::fwdWheelRingType(int iWheel, int iRingIndex) const
{
  return (*(*m_wheels_rings)[iWheel])[iRingIndex];
}

//
// Forward DiscSupport
//
double
SCT_ForwardParametersOld::fwdDiscSupportInnerRadius(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_DISKINNERRADIUS", iWheel);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
  //---return m_rdb->fwdDiscSupport()->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdDiscSupportOuterRadius(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_DISKOUTERRADIUS", iWheel);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
  //---return m_rdb->fwdDiscSupport()->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}
 
double
SCT_ForwardParametersOld::fwdDiscSupportThickness(int iWheel) const
{
  char paraName[50];
  sprintf(paraName, "W%d_DISKTHICKNESS", iWheel);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
  //---return m_rdb->fwdDiscSupport()->getDouble("THICKNESS") * Gaudi::Units::mm;
}

std::string
SCT_ForwardParametersOld::fwdDiscSupportMaterial(int iWheel) const
{
  //---return std::string("sct::DiscSupport");
  char paraName[50];
  sprintf(paraName, "W%d_FWDDISCSUPPORTMATERIAL_STRNG", iWheel);
  return (m_SCT_Fwd_Parameters_Strng->find(paraName))->second;
  //---return m_rdb->fwdDiscSupport()->getString("MATERIAL");
}

//
// Forward Ring
//

int 
SCT_ForwardParametersOld::fwdRingNumModules(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_NUMMODULES", iRing);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
  //---return m_rdb->fwdRing(iRing)->getInt("NUMMODULES");
}

double
SCT_ForwardParametersOld::fwdRingInnerRadius(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_INNERRADIUS", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdRingMiddleRadius(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_MIDDLERADIUS", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
}
double
SCT_ForwardParametersOld::fwdRingOuterRadius(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_OUTERRADIUS", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdRingOffset(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_OFFSET", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdRingModuleStagger(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_MODULESTAGGER", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::mm;
  //---return m_rdb->fwdRing(iRing)->getDouble("MODULESTAGGER") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdRingPhiOfRefModule(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_PHIOFREFMODULE", iRing);
  return (double)(m_SCT_Fwd_Parameters->find(paraName))->second*Gaudi::Units::radian;
  //---return m_rdb->fwdRing(iRing)->getDouble("PHIOFREFMODULE") * Gaudi::Units::deg;
}

int
SCT_ForwardParametersOld::fwdRingStaggerOfRefModule(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_STAGGEROFREFMODULE", iRing);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
  //---int index = fwdWheelRingMapIndex(iRing, ec);
  //---if (index < 0) return 0;
  //---return m_rdb->fwdWheelRingMap(index)->getInt("STAGGEROFREFMODULE");
}

int
SCT_ForwardParametersOld::fwdWheelDoubleSided(int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_DOUBLESIDED", iRing);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
}

// Returns +/-1 
/*
int
SCT_ForwardParametersOld::fwdWheelStereoType(m_iWheel, int iRing) const
{
  char paraName[50];
  sprintf(paraName, "Ring_%d_STEREOTYPE", iRing);
  return (int)(m_SCT_Fwd_Parameters->find(paraName))->second;
  //--- return m_rdb->fwdWheel(iWheel)->getInt("STEREOTYPE");
}
*/
//
// Forward General
//

double
SCT_ForwardParametersOld::fwdInnerRadius() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_INNERRADIUS"))->second*Gaudi::Units::mm;
  // return m_rdb->fwdGeneral()->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdIntermediateRadius() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_INTERMEDIATERADIUS"))->second*Gaudi::Units::mm;
  // return m_rdb->fwdGeneral()->getDouble("INNERRADIUS") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdOuterRadius() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_OUTERRADIUS"))->second*Gaudi::Units::mm;
  // return m_rdb->fwdGeneral()->getDouble("OUTERRADIUS") * Gaudi::Units::mm;
}




double
SCT_ForwardParametersOld::fwdZMin() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_ZMIN"))->second*Gaudi::Units::mm;
  //return m_rdb->fwdGeneral()->getDouble("ZMIN") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdZIntermediate() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_ZINTERMEDIATE"))->second*Gaudi::Units::mm;
  //return m_rdb->fwdGeneral()->getDouble("ZMIN") * Gaudi::Units::mm;
}

double
SCT_ForwardParametersOld::fwdZMax() const
{
  return (double)(m_SCT_Fwd_Parameters->find("FWD_ZMAX"))->second*Gaudi::Units::mm;
  //return m_rdb->fwdGeneral()->getDouble("ZMAX") * Gaudi::Units::mm;
}

}
