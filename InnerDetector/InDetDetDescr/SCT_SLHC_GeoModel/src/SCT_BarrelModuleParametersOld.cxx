/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SLHC_GeoModel/SCT_BarrelModuleParametersOld.h"
#include "SCT_SLHC_GeoModel/SCT_GeometryManager.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>
#include <cstring>

using std::abs;

namespace InDetDDSLHC {

//split strings
void barrelModSplitString(const std::string& str, std::vector<std::string>& str_vec, 
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
//
// A few hard wired numbers (some of which should go in NOVA)
//


const double PITCH = 80*Gaudi::Units::micrometer;
const double HALF_ACTIVE_STRIP_LENGTH = 31*Gaudi::Units::mm;
const double NOMINAL_WAFER_LENGTH = 63.960*Gaudi::Units::mm;
const double REF_DISTANCE_BETWEEN_FIDUCIALS = 2.19*Gaudi::Units::mm; 
const double DISTANCE_CORNER_MARK_TO_CENTER = 31.750*Gaudi::Units::mm; 
const double DISTANCE_CORNER_MARK_TO_FIDUCIAL = 0.8*Gaudi::Units::mm; 
const double DISTANCE_CENTER_TO_CENTER = 2*(DISTANCE_CORNER_MARK_TO_CENTER - 
					      DISTANCE_CORNER_MARK_TO_FIDUCIAL)
                                         + REF_DISTANCE_BETWEEN_FIDUCIALS;


SCT_BarrelModuleParametersOld::SCT_BarrelModuleParametersOld(const SCT_DataBase * sctdb, const SCT_GeoModelAthenaComps * athenaComps)
  : SCT_BarrelModuleParameters(sctdb, athenaComps)
{
  IMessageSvc* msgSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  svcLocator->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "SCT_BarrelModuleParameters");

  log<<MSG::DEBUG<<"========== Read Local Databse Modules Parameters =======" 
     << endmsg;
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
  float value;
  char value_strng[5000];
  bool moduleParameters = false;
  //container of modules parameters
  m_SCT_Modules = new std::map< std::string, float>();
  m_SCT_Modules_Strng = new std::map<std::string, std::string>();
  while(!feof(db_iumputfile)){
    //read line by line
    fgets(line, sizeof(line), db_iumputfile);
    //escape if the line is a comment
    if(strstr(line, "//") != NULL){
      continue;
    }
    //if not a comment, then get the name and the value of the parameter
    sscanf(line, "%499s %99f", name, &value);
    
    if(!moduleParameters){
      if(!strcmp(name, "SCT_BRL_MODULES_PARAMETERS_BEGIN")) moduleParameters = true;
      continue;
    }else if(!strcmp(name, "SCT_BRL_MODULES_PARAMETERS_END")) break;
    else{
      //put string parameters in a different container
      if(strstr(name, "_STRNG") != NULL){
	//re-read the second parameter as a string
	sscanf(line, "%499s %4999s", name, value_strng);
	(*m_SCT_Modules_Strng)[std::string(name)] = std::string(value_strng);
      }else{
	//fill in the map (the container)
	(*m_SCT_Modules)[std::string(name)] = value;
      }
    } 
  }
  //print the values for test
  std::map<std::string, float>::iterator map_itr;
  std::cout<<"-----------------Begin of SCT Barrel Modules parameters----------------"<<std::endl;
  for(map_itr=m_SCT_Modules->begin(); map_itr!=m_SCT_Modules->end(); map_itr++){
    std::cout<<"SCT_BarrelModuleParameters: first = "<<map_itr->first 
	     <<"  second = "<<map_itr->second<<std::endl;
  }
  fclose(db_iumputfile);
  std::cout<<"--------------- --End of SCT Barrel Modules parameters-------  ---------" <<std::endl;
}


//
// Barrel Sensors
//
double 
SCT_BarrelModuleParametersOld::sensorThickness(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_DXYZ1", moduleType);
  std::cout<<"-----------2 sensorThickness DXYZ1 mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second <<std::endl;
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
}

double 
SCT_BarrelModuleParametersOld::sensorWidth(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_DXYZ2", moduleType);
  std::cout<<"-----------2 DXYZ2 sensorWidth mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm <<std::endl;
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
}

double 
SCT_BarrelModuleParametersOld::sensorLength(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_DXYZ3", moduleType);
  float sensorLen = (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
  std::cout<<"-----------2 SensorLentgh DXYZ3 mod_typ("<<moduleType<<") = "<<sensorLen <<std::endl;
  return sensorLen;
}

std::string 
SCT_BarrelModuleParametersOld::sensorMaterial(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_SENSORMATERIAL_STRNG", moduleType);
  std::string sensorMaterial = (m_SCT_Modules_Strng->find(paraName))->second;
  std::cout<<"-----------2 Barrel SENSORMATERIAL_STRNG("<<moduleType<<") = "<<sensorMaterial <<std::endl;
  return sensorMaterial;
}

//
// Barrel BaseBoard
//
double 
SCT_BarrelModuleParametersOld::baseBoardThickness(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_BBTHICK", moduleType);
  std::cout<<"-----------2 baseBoardThickness BBTHICK mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm <<std::endl;
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
}

double 
SCT_BarrelModuleParametersOld::baseBoardWidth(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_BBWID", moduleType);
  std::cout<<"-----------2 baseBoardWidth BBWID mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm <<std::endl;
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
}

double SCT_BarrelModuleParametersOld::baseBoardLength(int moduleType) const{
  char paraName[50];  
  sprintf(paraName, "BRL_M%d_BBLEN", moduleType);
  std::cout<<"-----------2 baseBoardLength BBLEN mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second <<std::endl;
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
}

std::string 
SCT_BarrelModuleParametersOld::baseBoardMaterial(int moduleType) const 
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_BASEBOARDMATERIAL_STRNG", moduleType);
  std::string baseboardMaterial = (m_SCT_Modules_Strng->find(paraName))->second;
  std::cout<<"-----------2 Barrel BASEBOARDMATERIAL_STRNG("<<moduleType<<") = "<<baseboardMaterial <<std::endl;
  return baseboardMaterial;
}
double SCT_BarrelModuleParametersOld::baseBoardOffsetY(int /*moduleType*/) const{
  //if(moduleType == 1)
    return -5.7*Gaudi::Units::mm;
  //else
  //  return -5.7*Gaudi::Units::mm;
}

double SCT_BarrelModuleParametersOld::baseBoardOffsetZ(int moduleType) const{
  if(moduleType == 1)
    return -7.1*Gaudi::Units::mm;
  else
    return -1.9*Gaudi::Units::mm;
}

//
// Barrel Module
//
double 
SCT_BarrelModuleParametersOld::moduleStereoAngle(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_STEREOANGLE", moduleType);
  return (m_SCT_Modules->find(paraName))->second*Gaudi::Units::milliradian;
}

double 
SCT_BarrelModuleParametersOld::moduleInterSidesGap(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_INTERSIDESGAP", moduleType);
  return (double)(m_SCT_Modules->find(paraName))->second*Gaudi::Units::mm;
}

// Barrel Module Side Design

double 
SCT_BarrelModuleParametersOld::barrelModelSideStripPitch(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_PITCH", moduleType);
  std::cout<<"-----------2 barrelModelSideStripPitch PITCH mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second*Gaudi::Units::micrometer <<std::endl;
  return (double)(m_SCT_Modules->find(paraName))->second*Gaudi::Units::micrometer;
}

double
SCT_BarrelModuleParametersOld::barrelModelSideStripLength(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_STRIPLEN", moduleType);
  double stripLen = (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
  std::cout<<"-----------2 barrelModelSideStripLength STRIPLEN mod_typ("<<moduleType<<") = "<<stripLen <<std::endl;
  return stripLen;
}

double 
SCT_BarrelModuleParametersOld::barrelModelSideTotalDeadLength(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_STRIPDEADLEN", moduleType);
  double stripdeadLen = (m_SCT_Modules->find(paraName))->second*Gaudi::Units::cm;
  std::cout<<"-----------2 barrelModelSideTotalDeadLength STRIPDEADLEN mod_typ("<<moduleType<<") = "<<stripdeadLen<<std::endl;
  return stripdeadLen;
}

int 
SCT_BarrelModuleParametersOld::barrelModelSideCrystals(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_NUMBEROFWAFERS", moduleType);
  return (int)(m_SCT_Modules->find(paraName))->second;
}

int 
SCT_BarrelModuleParametersOld::barrelModelSideDiodes(int moduleType) const
{
  return barrelModelSideCells(moduleType) + 2*barrelModelSideShift(moduleType);
}

int 
SCT_BarrelModuleParametersOld::barrelModelSideCells(int moduleType) const
{
  char paraName[50];
  sprintf(paraName, "BRL_M%d_NRO", moduleType);
  std::cout<<"-----------2 barrelModelSideCells NRO mod_typ("<<moduleType<<") = "<<(m_SCT_Modules->find(paraName))->second <<std::endl;
  return (int)(m_SCT_Modules->find(paraName))->second;
}

int
SCT_BarrelModuleParametersOld::barrelModelSideShift(int /*moduleType*/) const
{
  return 0;
}

double 
SCT_BarrelModuleParametersOld::barrelDeadEdge(int moduleType) const
{
  char paraName1[50];
  char paraName2[50];
  sprintf(paraName1, "BRL_M%d_DXYZ3", moduleType);
  sprintf(paraName2, "BRL_M%d_STRIPLEN", moduleType);
  float deadEdge = 0.5*((m_SCT_Modules->find(paraName1))->second-(m_SCT_Modules->find(paraName2))->second)*Gaudi::Units::mm;
  std::cout<<"-----------2 barrelDeadEdge DEADED mod_typ("<<moduleType<<") = "<<deadEdge<<std::endl;
 return deadEdge;
}

SCT_BarrelModuleParametersOld::~SCT_BarrelModuleParametersOld(){
   delete m_SCT_Modules; m_SCT_Modules=0;
   delete m_SCT_Modules_Strng; m_SCT_Modules_Strng=0;
}
}
