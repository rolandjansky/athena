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
   markers["-trigqruns"].desc << "runs for Trigger Query bit position Decoding, format: 1,3,5-12,22"
                              << " NOTE: this option disables the autodetection of run numbers";
   markers["-trigqproject"].desc << "MC project name for Trigger Query (MC only, not data!)";
   markers["-trigqamitag"].desc << "MC project AMI tag for Trigger Query (MC only, not data!)";

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
QueryInfo::evalArgsPass2(std::vector<std::string>& , pool::Args2Container& container)
{
   if( query().find("TriggerPassed") == string::npos && query().find("TriggerNotPassed") == string::npos )
      return true;

   if( coral::MessageStream::msgVerbosity() == coral::Debug
       || coral::MessageStream::msgVerbosity() == coral::Verbose ) {
      queryRemapper.setDebug();
   }

   string amitag, project;
   if( getOpt("-trigqproject", project) ) {
      queryRemapper.setDataPeriod( project );
   }
   if( getOpt("-trigqamitag", amitag) ) {
      queryRemapper.setAMITag( amitag );
   }

   SrcInfo*     srcinfo = 0;
   for( Args2Container::const_iterator ait = container.begin(); ait != container.end(); ++ait) {
      if( (*ait)->name()=="SrcInfo" ) {
         srcinfo = dynamic_cast<SrcInfo*>( *ait );
      }
   }
   if( !srcinfo ) {
      container.m_log << coral::Error << "QueryInfo could not find SourceInfo" << corENDL;
      return false;
   }
      
   string       runs;
   bool         userruns = getOpt("-trigqruns", runs);

   string       srcTech;
   CollectionFactory    *factory = CollectionFactory::get();
   set<unsigned>        runset;
   for( unsigned int i=0; i<srcinfo->nSrc(); i++ ) {
      // get the technology without opening the collection
      CollectionDescription desc( srcinfo->name(i), srcinfo->type(i), srcinfo->connect() );
      factory->resolveDescription( desc );

      if( !userruns ) {
         // need to find out which runs for the trigger query
         auto_ptr<ICollection> collection( factory->create( desc, ICollection::READ ) );
         auto_ptr<ICollectionQuery>  collQuery( collection->newQuery() );
         collQuery->addToOutputList( "RunNumber" );
         ICollectionCursor& cursor = collQuery->execute();
         while( cursor.next() ) {
            unsigned runN = cursor.currentRow().attributeList()["RunNumber"].data<unsigned>();
            runset.insert( runN );
         }
      } 
      string tech = desc.type();
      if( i==0 ) {
         srcTech = tech;
         continue;
      }
      if( srcTech != tech ) {
         container.m_log << coral::Error << "can not mix up imput ROOT and Relational "
                         << " collection types when querying triggers" << corENDL;
         return false;
      }
   }
   
   if( !userruns ) {
      // convert autodetected runs into a string version
      ostringstream runstr;
      unsigned  rbegin = 0, rlast = 0;
      bool      loopend = false;
      for( set<unsigned>::const_iterator r = runset.begin(); !loopend; rlast = *r, ++r) {
         // loopend condition here to provide one more final loop iteration 
         loopend = ( r == runset.end() );
         if( !loopend && rlast && *r == rlast+1 ) {
            // start range
            if( !rbegin )  rbegin = rlast;
         } else {
            if( runstr.str().length() )  runstr << ",";
            if( rbegin ) {
               // there was a range
               runstr << rbegin << "-" << rlast;
               rbegin = 0;
            } else {
               if( rlast )  runstr << rlast;
            }
         }
      } 
      runs = runstr.str();
      container.m_log << coral::Info << "Autodetected runs for trigger conditions: " << runs << corENDL;
   }
   
   queryRemapper.queryRuns( runs );
   m_queries[0] = queryRemapper.triggerQueryRemap( m_queries[0], srcTech );

   return true;
}

