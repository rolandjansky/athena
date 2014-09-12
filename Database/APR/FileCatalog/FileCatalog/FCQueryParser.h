/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FILECATALOG_FCQUERYPARSER_H
#define FILECATALOG_FCQUERYPARSER_H

#include <string>
#include <vector>

namespace pool {
    /**
     * @class FCQueryParser FCQueryParser.h
     *
     * Given a query, the parse result tells whether to dispatch to FileCatalog      * or to MetaDataCatalog. If the query contains a subset of keywords: 
     * pfname,lfname,guid,pfn,lfn, dispatch to FC otherwise to Meta.
     *
     * @author: Z.Xie
     */
  class FCQueryParser {
  public:
    FCQueryParser( const std::string& whereClause );
    void parse();
    const std::vector<std::string>& parseResult() const;
    bool isMetaQuery() const;
  private:
    mutable std::string m_whereClause;
    std::vector<std::string> m_parseResult;
  private:
    std::string removeQuoted(const std::string& phrase) const;
    void chop(const std::string& input,
	      const std::string& delim, 
	      std::vector<std::string>& result) const;
    void trim( std::string& input ) const;
    
  };
}//ns pool
#endif
  
