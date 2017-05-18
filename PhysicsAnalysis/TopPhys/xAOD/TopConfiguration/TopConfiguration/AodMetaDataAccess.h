/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TOPCONFIGURATION_AODMETADATAACCESS_H
#define TOPCONFIGURATION_AODMETADATAACCESS_H

#include <map>
#include <string>

namespace top {

class AodMetaDataAccess {

public:

   AodMetaDataAccess();
   virtual ~AodMetaDataAccess();
   virtual void loadWithFilesFrom(std::string const & fileListPath);

   /**
    * @brief retrieve an AOD meta-data value
    * 
    * @param folderName the folder name, e.g. "/TagInfo"
    * @param keyName the key name, e.g. "project_name"
    *
    * @return the meta-data value as a string
    */
   std::string get(std::string const & folderName, std::string const & keyName) const;

   /**
    * @brief retrieve an AOD meta-data value
    * 
    * @param folderName the folder name, e.g. "/TagInfo"
    * @param keyName the key name, e.g. "project_name"
    *
    * @return the meta-data value as a string, or @p defaultValue if the meta-data entry does not exist
    */
   std::string get(std::string const & folderName, std::string const & keyName, std::string const & defaultValue) const;

   /**
    * @brief check whether AOD meta-data has been loaded
    *
    * @return whether the AOD meta-data has been loaded
    */
   bool valid() const;

   /**
    * @brief check whether the data are simulated or real
    *
    * @return true for MC, false for real data
    */
   bool isSimulation() const;

   /**
    * @brief check whether this sample is simulation, and then check if it was simulated with ATLFASTII
    *
    * @return true for AFII, false otherwise (FS or Data)
    */
   bool isAFII() const;


private:

   std::map<std::string, std::map< std::string, std::string > > * m_data;

};

}

#endif
