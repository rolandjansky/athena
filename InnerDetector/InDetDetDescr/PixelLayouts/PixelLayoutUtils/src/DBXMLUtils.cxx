/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

  msg(MSG::INFO)<<"XML input data : test if read from CLOB saved in DB ";

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
 

  msg(MSG::INFO)<<"Read XML file from CLOB DB : "<<xmlName<<endreq;
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
  if(res.size()==0)msg(MSG::WARNING)<<"No XML-CLOB entry found for "<<xmlName<<endreq;
  return res;
}


