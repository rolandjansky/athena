/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_CATALOGINFO
#define UTILITIES_COLLECTION_CATALOGINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"
#include "CxxUtils/checker_macros.h"

/**********************************************************

CatalogInfo is an concretization of a CmdLineArgs2
   to contain catalog info.

Qualifiers: -ccread, -fcread

**********************************************************/

namespace pool
{
   class ICollectionService;
   class IFileCatalog;

   
   class CatalogInfo : public CmdLineArgs2
   {
  public:

     /// Constructors
     CatalogInfo( );

     virtual ~CatalogInfo() {}

     ///
     bool evalArgs(std::vector<std::string>& argv);

     std::string collCatalogN(unsigned int i);
     std::string fileCatalogN(unsigned int i);
     bool useCC() {return m_valid;}

     void          setCatalogs ATLAS_NOT_THREAD_SAFE ( pool::ICollectionService* );
     void          setFileCatalogs( pool::IFileCatalog* );
  
  private:
     bool m_valid;
     std::vector<std::string> m_collCatalogReadVec;
     std::vector<std::string> m_fileCatalogReadVec;

   };

} //end pool namespace

#endif  // UTILITIES_COLLECTION_CATALOGINFO


