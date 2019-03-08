/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_FSIHELPER_H
#define SCT_GEOMODEL_SCT_FSIHELPER_H

#include <vector>
#include <map>
#include <string>

class SCT_DataBase;

class FSILocation
{
public:
  FSILocation(std::string name,
	      double radius,
	      double phi,
	      int side);

  const std::string & name() const {return m_name;}
  double radius() const {return m_radius;}
  double phi() const {return m_phi;}
  int side() const {return m_side;}
  

private:
  std::string m_name;
  double m_radius;
  double m_phi;
  int m_side;
};

class FSIDetails
{

public:  
  FSIDetails(const FSILocation * location, 
	     int simType, 
	     const std::string & simTypeString, 
	     const std::string & locationType, 
	     const std::string & actualType);
  const FSILocation & location() const {return *m_location;}
  int simType() const {return m_simType;}
  const std::string & simTypeString() const {return m_simTypeStr;}
  const std::string & locationType() const {return m_locType;}
  const std::string & actualType()   const {return m_actualType;}

private:
  const FSILocation * m_location;
  int m_simType;
  std::string m_simTypeStr;
  std::string m_locType;
  std::string m_actualType;

};


class FSIHelper
{
public:
  FSIHelper(SCT_DataBase * rdb);
  ~FSIHelper();
  
  // Gte vector of FSI details for a given wheel
  const std::vector<const FSIDetails *> & fsiVector(int wheel) const;
  
private:
  void fill();

  std::vector<std::vector<const FSIDetails *> * > m_wheelLocMap;
  std::map<std::string, FSILocation *> m_locationTypes;

  SCT_DataBase * m_rdb;
  
};

#endif // SCT_GEOMODEL_SCT_FSIHELPER_H
