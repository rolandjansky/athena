/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERSISTENTDATAMODEL_PLACEMENT_H
#define PERSISTENTDATAMODEL_PLACEMENT_H

/** @file Placement.h
 *  @brief This file contains the class definition for the Placement class (migrated from POOL).
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 **/

#include <string>

/** @class Placement
 *  @brief This class holds all the necessary information to guide the writing of an object in a physical place.
 **/

class Placement {
public:
   /// Default Constructor
   Placement();

   /// Constructor with initialization
   Placement(const std::string& fileName, const std::string& containerName, long technology);

   /// Set file name
   void setFileName(const std::string& fileName);
   /// Access file name
   const std::string& fileName() const;

   /// Set container name
   void setContainerName(const std::string& containerName);
   /// Access container name
   const std::string& containerName() const;

   /// Set technology type
   void setTechnology(long technology);
   /// Access technoliogy type
   long technology() const;

   /// Retrieve the string representation of the placement.
   const std::string toString() const;
   /// Build from the string representation of a placement.
   Placement& fromString(const std::string& from);

private:
   /// Technology identifier
   long m_technology;
   /// File name
   std::string m_fileName;
   /// Container name
   std::string m_containerName;
};

#endif
