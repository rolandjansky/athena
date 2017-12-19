/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef UTILITIES_COLLECTION_QUERYINFO
#define UTILITIES_COLLECTION_QUERYINFO

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"

/**********************************************************

QueryInfo is an concretization of a CmdLineArgs2
   to contain query info.

Qualifiers: -query, -queryopt

**********************************************************/

namespace pool
{
   
   class QueryInfo : public CmdLineArgs2
   {
  public:

     /// Constructors
     QueryInfo( );

     virtual ~QueryInfo() {}

     ///
     bool       evalArgs(std::vector<std::string>& argv);

     bool       evalArgsPass2(std::vector<std::string>& argv, pool::Args2Container& container);

     /// return the query string.  
     const std::string& query( size_t qn=0 );
     size_t             nQuery() const { return m_queries.size(); }
     const std::vector<std::string>& queries() const { return m_queries; }
     
     const std::string& queryOptions() const { return m_queryOptions; }

  private:
     std::vector<std::string>   m_queries;
     std::string                m_queryOptions;
   };

  
} // end pool namespace

#endif  // UTILITIES_COLLECTION_QUERYINFO

