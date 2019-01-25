/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_ForwardModuleParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_DataBase.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <iostream>
#include <cstring>

namespace InDetDDSLHC {

//split strings
void fwdModSplitString(const std::string& str, std::vector<std::string>& str_vec, 
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
SCT_ForwardModuleParametersOld::SCT_ForwardModuleParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_ForwardModuleParameters(sctdb, athenaComps)
{
  //m_rdb = SCT_DataBase::instance();

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
  char line[5000];
  char name[500];
  char value_strng[5000];
  float value;
  bool sct_fwd_moduleParameters = false;
  //container of modules parameters
  m_SCT_Fwd_Modules = new std::map<std::string, float>();
  m_SCT_Fwd_Modules_Strng = new std::map<std::string, std::string>();
  while(!feof(db_iumputfile)){
    //read line by line
    fgets(line, sizeof(line), db_iumputfile);
    //escape if the line is a comment
    if(strstr(line, "//") != NULL){
      continue;
    }
    //if not a comment, then get the name and the value of the parameter
    sscanf(line, "%499s %99f", name, &value);
    if(!sct_fwd_moduleParameters){
      if(!strcmp(name, "SCT_FWD_MODULES_PARAMETERS_BEGIN")) sct_fwd_moduleParameters = true;
      continue;
    }
    else if(!strcmp(name, "SCT_FWD_MODULES_PARAMETERS_END")) break;
    else{
      //put string paramters in a different container
      if(strstr(name, "_STRNG") != NULL){
	//re-read the second parameter as a string
	sscanf(line, "%499s %4999s", name, value_strng);
	(*m_SCT_Fwd_Modules_Strng)[std::string(name)] = std::string(value_strng);
      }else{
	//and fill in the map (the container)
	(*m_SCT_Fwd_Modules)[std::string(name)] = value;
      }
    } 
  }//end of while(!feof(db_iumputfile))
  fclose(db_iumputfile);
  //print the values for test
  std::cout<<"-------------------Begin of SCT Forward Modules Parameters-------------"
	   <<std::endl;
  std::map<std::string, float>::iterator map_itr;
  for(map_itr=m_SCT_Fwd_Modules->begin(); map_itr!=m_SCT_Fwd_Modules->end(); map_itr++){
    std::cout<<"----SCT_ForwardParameters: parameter = "<<map_itr->first 
	     <<",  values = "<<map_itr->second<<std::endl;
  }
  std::cout<<"-------------------End of SCT Forward Modules Parameters---------------"
	   <<std::endl;
}

//
// Forward Sensor
//
int
SCT_ForwardModuleParametersOld::fwdSensorNumWafers(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_NUMWAFERS", iModuleType);
  return (int)(m_SCT_Fwd_Modules->find(paraName))->second;
  //---return m_rdb->fwdSensor(iModuleType)->getInt("NUMWAFERS");
}

double 
SCT_ForwardModuleParametersOld::moduleInterSidesGap(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_INTERSIDESGAP", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorThickness(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORTHICKNESS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  2.0 * m_rdb->zsmo()->getDouble("DZSC") * Gaudi::Units::cm;
 //---return  m_rdb->fwdSensor(iModuleType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorLength(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORLENGTH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("RLF") * Gaudi::Units::cm;
//---return  m_rdb->fwdSensor(iModuleType)->getDouble("LENGTHFAR") * Gaudi::Units::mm;
}
double 
SCT_ForwardModuleParametersOld::fwdSensorInnerWidth(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORINNERWIDTH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
}
double 
SCT_ForwardModuleParametersOld::fwdSensorOuterWidth(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSOROUTERWIDTH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorInnerRadius(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORINNERRADIUS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("RINNERF") * Gaudi::Units::cm;
  //---return  m_rdb->fwdSensor(iModuleType)->getDouble("INNERWIDTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorOuterRadius(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSOROUTERRADIUS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("ROUTERF") * Gaudi::Units::cm;
  //---return  m_rdb->fwdSensor(iModuleType)->getDouble("OUTERWIDTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorMiddleRadius(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORMIDDLERADIUS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("ROUTERF") * Gaudi::Units::cm;
//---return  m_rdb->fwdSensor(iModuleType)->getDouble("OUTERWIDTHFAR") * Gaudi::Units::mm;
}
double 
SCT_ForwardModuleParametersOld::fwdSensorDeltaPhi(int iModuleType) const{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORDELTAPHI", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::radian;
}

std::string 
SCT_ForwardModuleParametersOld::fwdSensorMaterial(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SENSORMATERIAL_STRNG", iModuleType);
  return (m_SCT_Fwd_Modules_Strng->find(paraName))->second;
  //---return m_rdb->fwdSensor(iModuleType)->getString("MATERIALFAR");
}

double 
SCT_ForwardModuleParametersOld::fwdSensorActiveHalfLength(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_ACTIVEHALFLENGTH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("RSEF") * Gaudi::Units::cm;
 //---return  m_rdb->fwdSensor(iModuleType)->getDouble("ACTIVEHALFLENGTHFAR") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSensorAngularPitch(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_ANGULARPITCH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::radian;
  //---return  m_rdb->zsmi(iModuleType)->getDouble("PHISTR") * Gaudi::Units::radian;
  //---return  m_rdb->fwdSensor(iModuleType)->getDouble("ANGULARPITCH") * Gaudi::Units::radian;
}

int
SCT_ForwardModuleParametersOld::fwdSensorNumReadoutStrips(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_NUMREADOUTSTRIPS", iModuleType);
  return (int)(m_SCT_Fwd_Modules->find(paraName))->second;
  //---return fwdSensorNumStrips(iModuleType) - 2 * fwdSensorStripShift(iModuleType);
  //---return m_rdb->fwdSensor(iModuleType)->getInt("NUMREADOUTSTRIPS");
}

double 
SCT_ForwardModuleParametersOld::fwdModuleStereoAngle(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_STEREOANGLE", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::milliradian;
  //---return 40 * Gaudi::Units::milliradian;
  //---return m_rdb->fwdModule(iModuleType)->getDouble("STEREOANGLE") * Gaudi::Units::milliradian;
}

//
// Forward Spine
//
double 
SCT_ForwardModuleParametersOld::fwdSpineThickness(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SPINETHICKNESS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //---return 1*Gaudi::Units::mm;
  //---return  m_rdb->fwdSpine(iModuleType)->getDouble("THICKNESS") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSpineLength(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SPINELENGTH", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
  //return 8*Gaudi::Units::cm;
  //---  return (m_rdb->fwdSensor(iModuleType)->getDouble("LENGTHNEAR") * Gaudi::Units::mm
  //---	  + m_rdb->fwdSensor(iModuleType)->getDouble("LENGTHFAR") * Gaudi::Units::mm + 2*Gaudi::Units::cm);
//return  m_rdb->fwdSpine(iModuleType)->getDouble("WIDTH") * Gaudi::Units::mm;
}

double 
SCT_ForwardModuleParametersOld::fwdSpineMiddleRadius(int iModuleType) const
{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SPINEMIDDLERADIUS", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::mm;
}
double 
SCT_ForwardModuleParametersOld::fwdSpineDeltaPhi(int iModuleType) const{
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SPINEDELTAPHI", iModuleType);
  return (double)(m_SCT_Fwd_Modules->find(paraName))->second*Gaudi::Units::radian;
}

std::string 
SCT_ForwardModuleParametersOld::fwdSpineMaterial(int iModuleType) const
{
  //---return std::string("sct::FwdSpineMid");
  char paraName[50];
  sprintf(paraName, "FWD_M%d_SPINEMATERIAL_STRNG", iModuleType);
  return (m_SCT_Fwd_Modules_Strng->find(paraName))->second;
  //---return m_rdb->fwdSpine(iModuleType)->getString("MATERIAL");
}

SCT_ForwardModuleParametersOld::~SCT_ForwardModuleParametersOld(){

  delete m_SCT_Fwd_Modules; m_SCT_Fwd_Modules=0;
  delete m_SCT_Fwd_Modules_Strng; m_SCT_Fwd_Modules_Strng=0;

}

}
