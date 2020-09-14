/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "PixelLayoutUtils/DBXMLUtils.h"
#include "PixelGeoModel/PixelGeoBuilder.h"
#include "InDetGeoModelUtils/OraclePixGeoAccessor.h"
#include "PathResolver/PathResolver.h"
#include <sstream>
#include <fstream>

DBXMLUtils::DBXMLUtils(const PixelGeoBuilderBasics* basics):
  PixelGeoBuilder(basics) 
{
  readXMLtableFromDB();
}

DBXMLUtils::~DBXMLUtils() 
{}

void DBXMLUtils::readXMLtableFromDB()
{

  msg(MSG::VERBOSE)<<"XML input data : test if read from CLOB saved in DB ";

  pixelXMLTable_ptr = geoAccessor().getTable("PIXXDD");
  if (!pixelXMLTable_ptr||!pixelXMLTable_ptr)
    {
      return;
    }

}


std::string DBXMLUtils::readXMLFromDB(std::string xmlName) const
{

  std::string res=std::string("");

  // Remove .xml suffix if it exists
  std::string prefix=""; 
  if(xmlName.size()>4)prefix=xmlName.substr(xmlName.size()-4,4);
 
  if(prefix.size()>0) xmlName=xmlName.substr(0,xmlName.size()-4);
 

  msg(MSG::VERBOSE)<<"Read XML file from CLOB DB : "<<xmlName<<endreq;
  std::string xmlNameGeo = xmlName+"Geo";

  int nbCLOB=0;
  for(int i=0; i<(int)geoAccessor().getTableSize(pixelXMLTable_ptr);i++)
    {
      std::string keyword = geoAccessor().getString(pixelXMLTable_ptr,"KEYWORD",i);
      if(keyword==xmlName||keyword==xmlNameGeo){
	msg(MSG::DEBUG)<<"CLOB : "<<i<<" "<<keyword<<" / "<<xmlName<<endreq;
	if(res.size()>0) res=res.substr(0,res.size()-1);
	res = res + geoAccessor().getString(pixelXMLTable_ptr,"XMLCLOB",i);
	nbCLOB++;
      }
    }

  if(nbCLOB>1) res="<data>\n"+res+"</data>\n";
  if(res.size()==0)msg(MSG::ERROR)<<"No XML-CLOB entry found for "<<xmlName<<endreq;
  return res;
}

int DBXMLUtils::getSchemaVersion(std::string xmlName) const
{

  int res=-1;

  // Remove .xml suffix if it exists
  std::string prefix=""; 
  std::string latest_file = "";

  if(xmlName.size()>4)prefix=xmlName.substr(xmlName.size()-4,4);
 
  if(prefix.size()>0) xmlName=xmlName.substr(0,xmlName.size()-4);
 

  msg(MSG::VERBOSE)<<"Read XML file Version from DB : "<<xmlName<<endreq;
  std::string xmlNameGeo = xmlName+"Geo";

  int nbCLOB=0;
  for(int i=0; i<(int)geoAccessor().getTableSize(pixelXMLTable_ptr);i++)
    {
      std::string keyword = geoAccessor().getString(pixelXMLTable_ptr,"KEYWORD",i);
      if(keyword==xmlName||keyword==xmlNameGeo){
	msg(MSG::DEBUG)<<"CLOB : "<<i<<" "<<keyword<<" / "<<xmlName<<endreq;
	int version = geoAccessor().getInt(pixelXMLTable_ptr,"VERSION",i);
	if(version > res) {
	  res = version;
	  latest_file = keyword;
	}
	else msg(MSG::WARNING)<<"Older version detected for "<<keyword<<" ("<<version<<") than "<<latest_file<<" ("<<res<<"). Taking newest ("<<res<<")!"<<endreq;
	nbCLOB++;
      }
    }

  if(res==-1)msg(MSG::ERROR)<<"No XML-VERSION entry found for "<<xmlName<<endreq;
  return res;
}

