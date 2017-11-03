/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "CollectionUtilities/QueryInfo.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CoralBase/MessageStream.h"
#include "CollectionUtilities/Args2Container.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionFactory.h"

#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CoralBase/Attribute.h"

#include "POOLCore/Exception.h"

#include <memory>
#include <set>
#include <sstream>

#include <iostream>

#define corENDL coral::MessageStream::endmsg

using namespace std;
using namespace pool;


   
QueryInfo::QueryInfo( ) : CmdLineArgs2("QueryInfo") 
{
   QualList markers;
   markers["-query"] = ArgQual("[predicate string (DEFAULT = \"\")]", 1, true);
   markers["-queryopt"].desc << "<comma separated attribute list> (DEFAULT = * )" << endl
                             << "if the Primary token is missing from the list, the first listed token will become the Primary";
   this->setArgQuals(markers);
}


const std::string&
QueryInfo::query( size_t qn )
{
   if( m_queries.size() == 0 )
      m_queries.push_back(string(""));
   if( m_queries.size() == 1 )
      return m_queries[0];
   if( m_queries.size() > qn )
      return m_queries[qn];
   throw pool::Exception( "Attempt to get query# bigger than supplied on the command line",
                          "QueryInfo::query(N)", "QueryInfo" );
}
   


bool
QueryInfo::evalArgs(std::vector<std::string>& argv)
{
   if( !CmdLineArgs2::evalArgs(argv) ) return false;

   (void)getOpts("-query", m_queries);
   (void)getOpt("-queryopt", m_queryOptions);

   return true;
}


bool
QueryInfo::evalArgsPass2(std::vector<std::string>& , pool::Args2Container& )
{
   return true;
}







