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

   /// Access file name
   const std::string& fileName() const { return m_fileName; }
   /// Set file name
   Placement& setFileName(const std::string& fileName) { m_fileName = fileName; return *this; }
   /// Access container name
   const std::string& containerName() const { return m_containerName; }
   /// Set container name
   Placement& setContainerName(const std::string& containerName) { m_containerName = containerName; return *this; }
   /// Access technology type
   int technology() const { return m_technology; }
   /// Set technology type
   Placement& setTechnology(int technology) { m_technology = technology; return *this; }
   /// Access auxiliary string
   const std::string& auxString() const { return m_auxString; }
   /// Set auxiliary string
   Placement& setAuxString(const std::string& auxString) { m_auxString = auxString; return *this; }

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
   /// Auxiliary string
   std::string m_auxString;
};

#endif
