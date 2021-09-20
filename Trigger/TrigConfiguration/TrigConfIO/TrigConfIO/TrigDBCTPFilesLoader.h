/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file TrigConfIO/TrigDBCTPFilesLoader.h
 * @brief Loader class for Trigger configuration (L1 hardware files) from the Trigger DB
 */

#ifndef TRIGCONFIO_TrigDBCTPFilesLoader_H
#define TRIGCONFIO_TrigDBCTPFilesLoader_H

#include "TrigConfData/L1CTPFiles.h"

#include "TrigConfIO/TrigDBLoader.h"

#include <map>
#include <vector>

namespace coral {
   class IQuery;
   class AttributeList;
}

namespace TrigConf {

   /**
    * @brief Loader of trigger configurations from Json files
    */
   class TrigDBCTPFilesLoader : public TrigDBLoader {
   public:

      /** Constructor */
      TrigDBCTPFilesLoader(const std::string & connection);

      /** Destructor */
      virtual ~TrigDBCTPFilesLoader();

      /**
       * @brief Load content from the Trigger DB into an L1CTPFiles object for a given super master key (SMK)
       * @param smk [in] the SMK that should be loaded
       * @param ctpfiles [out] the loaded L1CTPFiles
       * @param loadMask [in] bit mask which specifies which content should be loaded (default is 0x0F which loads all content)
       *   0x01 - CTPFiles, 0x02 - SMX files, 0x04 - TMC, 0x08 - MUCTPI
       * @param outFileName [in] if set, an outputfile with the raw data blob is written
       */
      bool loadHardwareFiles( unsigned int smk,
                              L1CTPFiles & ctpfiles,
                              uint8_t loadMask = 0x0F,
                              const std::string & outFileName = "") const;
                              
   private:

      void loadCTPFiles(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query) const;

      void loadCTPSMX(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query) const;

      void loadTMC(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query) const;

      void loadMUCTPI(L1CTPFiles & ctpfiles, std::unique_ptr<coral::IQuery> query) const;

      std::vector<uint32_t> loadDBFieldIntoVector(const coral::AttributeList& row, const std::string& field, size_t size) const;

      std::string loadDBFieldIntoString(const coral::AttributeList &row, const std::string &field) const;

      std::map<size_t, QueryDefinition> m_link_queries;
      std::map<size_t, QueryDefinition> m_ctpfiles_queries;
      std::map<size_t, QueryDefinition> m_ctpsmx_queries;
      std::map<size_t, QueryDefinition> m_muctpi_queries;
      std::map<size_t, QueryDefinition> m_tmcsig_queries;
   };

}

#endif

