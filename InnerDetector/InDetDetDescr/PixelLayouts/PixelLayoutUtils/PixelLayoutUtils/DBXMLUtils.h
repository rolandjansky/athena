/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef DBXMLUtils_H
#define DBXMLUtils_H

#include "PixelGeoModel/PixelGeoBuilder.h"

class DBXMLUtils : public PixelGeoBuilder  {

 public:

  DBXMLUtils(const PixelGeoBuilderBasics* basics);
  ~DBXMLUtils();
  
  void readXMLtableFromDB();
  std::string readXMLFromDB(std::string xmlName) const;

 private:
  
  IRDBRecordset_ptr pixelXMLTable_ptr;
  std::map<char,int> xmlDBIndices;

};

#endif

