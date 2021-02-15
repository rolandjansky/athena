/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollSplitByGUID.cpp
 * @brief Utility to list the file GUIDs used by a POOL collection and split the
 * collection into sub-collections by GUID
 * @author K. Karr <Kristo.Karr@cern.ch>, C. Nicholson <Caitriana.Nicholson@cern.ch>
 * $Id: CollSplitByGUID.cxx,v 1.14 2008-11-13 14:26:05 cranshaw Exp $
 */

#include "CollectionBase/ICollection.h"

#include "CollectionBase/CollectionService.h"
#include "CollectionBase/CollectionDescription.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/TokenList.h"
#include "CollectionBase/ICollectionQuery.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionMetadata.h"
#include "CollectionBase/ICollectionDataEditor.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/AttributeListException.h"

#include "PersistencySvc/ISession.h"
#include "PersistencySvc/DatabaseConnectionPolicy.h"
#include "PersistencySvc/ITransaction.h"

#include "GoodRunsLists/TGoodRun.h"
#include "GoodRunsLists/TGoodRunsList.h"
#include "GoodRunsLists/TGoodRunsListReader.h"
#include "GoodRunsLists/TGoodRunsListWriter.h"

#include "CollectionUtilities/CmdLineArgs2.h"
#include "CollectionUtilities/ArgQual.h"
#include "POOLCollectionTools/Args2Container.h"
#include "POOLCollectionTools/SrcInfo.h"
#include "POOLCollectionTools/MetaInfo.h"
#include "POOLCollectionTools/QueryInfo.h"
#include "POOLCollectionTools/CatalogInfo.h"
#include "POOLCollectionTools/UtilityFuncs.h"

#include "StorageSvc/DbStatus.h"

#include <iostream>
#include <string>

using namespace athenapool;

const std::string FILE_CATALOG_CONNECTION = 
  "xmlcatalog_file:PoolFileCatalog.xml";
const std::string COLLECTION_CATALOG_CONNECTION =
  "xmlcatalog_file:CollectionCatalog.xml";

const int DEFAULT_MIN_EVENTS = 0;

using namespace std;

bool runPairSort( const pair<unsigned int, int> &p1, const pair<unsigned int, int> &p2){
   return p1.second > p2.second;
}

int main(int argc, char *argv[])
{
   std::string thisProgram("CollSplitByRun.exe");
   coral::MessageStream log( thisProgram );

   // Convert argv to vector of strings
   std::vector<std::string> argv_v;
   for (int i=0; i<argc; ++ i) argv_v.push_back(std::string(argv[i]));

   try
   {
      int		maxSplit( 500 );
      pool::DbStatus sc = pool::DbStatus::Success;

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram,true);

      pool::QualList markers;
      //markers.insert( make_pair("-maxsplit",  ArgQual()) );
      //markers["-maxsplit"].desc << "Limit number of produced subcollections. Spillover will be stored in the last collection";
      markers.insert( std::make_pair("-minevents", pool::ArgQual()) );
      markers["-minevents"].desc << "minimum number of events to write to output collection";
      markers.insert( std::make_pair("-splitatt",  pool::ArgQual()) );
      markers["-splitatt"].desc << "Name of attribute to use for boundaries of split (DEFAULT=RunNumber)";
      markers.insert( std::make_pair("-lumikey",   pool::ArgQual()) );
      markers["-lumikey"].desc << "metadata key to use for lumi record in inputs";
    
      pool::CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      argsVec.push_back(&cmdLineArgs);  // Add it to the list

      // Classes with shared cli keys and their argument properties
      // Add them to the list
      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      QueryInfo queryinfo; argsVec.push_back(&queryinfo);
      SrcInfo srcinfo; argsVec.push_back(&srcinfo);
      MetaInfo metainfo; argsVec.push_back(&metainfo);

      // Print out help if requested
      if( argc == 2 || argc == 1 )
      {
         argsVec.desc << "*******************************************************************\n"
                      << "* CollSplitByGUID is a tool for querying an event collection, or  *\n"
                      << "* collections, and outputting the resultant events as a number of *\n"
                      << "* sub-collections, one for each GUID of the files containing the  *\n"
                      << "* events. Currently, these sub-collections are output as LOCAL,   *\n"
                      << "* ROOT-based collections and will appear in the directory from    *\n"
                      << "* which you call CollSplitByGUID.                                 *\n"
                      << "*******************************************************************\n";
         if (argc == 1) argsVec.printHelp(false);
         if (argc==2 && std::string(argv[1])=="-help") argsVec.printHelp(true);
         return 1;
      }

      // Check that all cmd line args are valid
      argsVec.evalArgs(argv_v);
      if (!argsVec.checkValid()) return 1;

      int minEvents = DEFAULT_MIN_EVENTS;
      std::string splitAtt = "RunNumber";
      string lumikey("OutputLumirange");
      std::map < std::string, std::pair <int,int> >::iterator mit=cmdLineArgs.begin();
      while (mit!=cmdLineArgs.end()) {
         std::string qual = mit->first;
         int ifirst = mit->second.first;
     
         if (qual == "-splitatt") {
            cout << "splitAtt hardcoded to RunNumber" << endl;
         }
 
         if (qual == "-lumikey") {
            lumikey = argv_v[ifirst];
         }
          
         if (qual == "-minevents") {
            minEvents = atoi( argv_v[ifirst].c_str() );
            std::cout << "Minimum number of events: " << minEvents << std::endl; 
         }
         ++mit;
      }

/*
      for ( unsigned int i=0; i<catinfo.nArgs("-fcread"); i++ )
      {
         collectionService->addReadFileCatalog( catinfo.collCatConnect(i) );
      }
      for( unsigned int i=0; i<catinfo.nArgs("-ccread"); i++ )      {
         collectionService->addReadCollectionCatalog( catinfo.collCatConnect(i) );
      }

    
      pool::IDataSvc* dataSvc = pool::DataSvcFactory::instance( collectionService->getFileCatalog() );
      pool::DatabaseConnectionPolicy policy;
      policy.setReadMode( pool::DatabaseConnectionPolicy::READ );
      dataSvc->session().setDefaultConnectionPolicy( policy );
      sc = dataSvc->transaction().start( pool::ITransaction::READ );
      if( !sc.isSuccess() )
      {
         std::cerr << "CollListFileGUID: ERROR Cannot start database "
	 	   << "transaction." << std::endl;
         return 1;
      }
*/
      pool::CollectionService* collectionService( new pool::CollectionService() );

      catinfo.setCatalogs( collectionService );

      std::map<int, pool::ICollection*> collMap;
      //int collectionNumber = 1;
      std::map< unsigned int, Root::TGoodRunsList> runGRLMap;  // map to keep run --> GRL
      std::map< unsigned int, pool::ICollection*> subCollMap;  // map to keep run --> sub-collection
      Root::TGoodRunsList grl_total("NORUNS");
      // for each input collection, loop through events and write out sub-collections
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ )
      {
         std::map< unsigned int, int > eventsPerRun;
         std::multimap< std::string, unsigned int> invCollMap;  // map to keep sub-collection name --> run

         log << coral::Info
             << "Opening source collection " << srcinfo.name(i)
             << " of type " << srcinfo.type(i) <<  coral::MessageStream::endmsg;
         bool readOnly( true );
         pool::ICollection *collection = collectionService->handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly );
         collMap[i] = collection;

         const pool::ICollectionDescription &firstDesc = collMap[0]->description();
         const pool::ICollectionDescription &nextDesc = collection->description();

         if( (queryinfo.query() != "" || queryinfo.queryOptions() != "") && !nextDesc.equals(firstDesc) ) {
            log << coral::Warning << " The schemas of one or more "
                << "input collections are different and a query has been "
                << "requested. This may lead to unexpected behavior."
                << coral::MessageStream::endmsg; 
         }

         pool::ICollectionQuery *collQuery = collection->newQuery();
         pool::ICollectionQuery *collQuery2 = collection->newQuery();
         //collQuery->setCondition( queryinfo.query() );
         //collQuery2->setCondition( queryinfo.query() );
         if( queryinfo.queryOptions().size() ) {
            collQuery->addToOutputList( queryinfo.queryOptions() );
            collQuery2->addToOutputList( queryinfo.queryOptions() );
         } else {
            collQuery->selectAll();
            collQuery2->selectAll();
         }
         
         log << coral::Info << "Executing query for the source collection" << coral::MessageStream::endmsg;
         pool::ICollectionCursor& cursor = collQuery->execute();

         // set parameters for the sub-collections. 
         // currently caters only for writing local, Root-based collections.
         std::string subCollType = "RootCollection";
         std::string subCollConnect = "";

         // first loop: for each event, find the GUID / fileId and 
         // count how many events match that GUID
         int totalEvents = 0;
         int uniqueRuns = 0;

         // Token name to split on (if not specified, use default for each source coll)
         //string refname = ( splitAtt.size()? splitAtt : collection->description().eventReferenceColumnName() );
         while( cursor.next() ) {
            const coral::AttributeList &atts = cursor.currentRow().attributeList();
            unsigned int run;
            try {
               run = atts[splitAtt].data<unsigned int>();
            }
            catch (coral::AttributeListException e) {
               cout << "Could not find splitting attribute " << splitAtt << endl;
               cout << e.what();
               return 2;
            }

            // track the event count for that run
            if( eventsPerRun.find( run ) == eventsPerRun.end() ) {
	       // new unique GUID found
               eventsPerRun[ run ] = 1;
	       uniqueRuns++;
            }
            else {
               eventsPerRun[ run ]++;
            }
            totalEvents++;
         }

         //
         // Go get lumi metadata from src
         pool::ICollectionMetadata& meta = collection->metadata();
         string lumixml;
         if (meta.existsKey(lumikey)) lumixml = string(meta.getValueForKey(lumikey));
         else cout << "Lumikey not found" << endl;
         //string lumixml(meta.getValueForKey(lumikey));

         // Parse the xml string
         // Possible FIXME: Could use AddXMLString method and retrieve merged runlist outside 
         //                 of loop, BUT this means not checking valid until end
         Root::TGoodRunsListReader* reader =  new Root::TGoodRunsListReader();
         reader->SetXMLString(lumixml);
         bool xmlIsValid = reader->Interpret();

         if (xmlIsValid) {
            // get the GRL
            Root::TGoodRunsList grl = reader->GetMergedGoodRunsList();
            grl_total = grl_total.GetSumWith(grl);
            vector<int> grl_runs = grl.GetRunlist();
            // loop over runs in AttributeList and fill runGRLmap
            std::map< unsigned int, int >::iterator test_it = eventsPerRun.begin();
            while (test_it != eventsPerRun.end()) {
               unsigned int trun = test_it->first;
               Root::TGoodRunsList grl_thisrun;
               Root::TGoodRunsList::iterator grl_it = grl.find(trun);
               string status("True");
               if (grl_it==grl.end()) {
                  cout << "WARNING: Encountered runlist in " << srcinfo.name(i) 
                       << " with runs in attributelist which are not in it's lumirange record" << endl;
                  status = "False";
               }
               else {
                  Root::TGoodRun::iterator gr_it = grl_it->second.begin();
                  while (gr_it != grl_it->second.end()) {
                     for (Int_t j = gr_it->Begin(); j < gr_it->End()+1; ++j) {
                        //cout << " iterating over GoodRun, lb " << trun << "," << j << endl;
                        grl_thisrun.AddRunLumiBlock(trun,j);
                     }
                     ++gr_it;
                  }
               }

               // Grab metadata for that run
               if( runGRLMap.find( trun ) == runGRLMap.end() ) {
                  runGRLMap.insert(make_pair(trun, Root::TGoodRunsList()));
                  //cout << "Setting version " << grl.GetVersion() 
                  //     << " and Split " << thisProgram << " for " << trun << endl;
                  runGRLMap[trun].SetVersion(grl.GetVersion());
		  runGRLMap[trun].AddMetaData("Split",thisProgram);
                  //cout << "Set version " << runGRLMap[trun].GetVersion() << endl;
               }
               runGRLMap[trun] = runGRLMap[trun].GetSumWith(grl_thisrun);
               ++test_it;
            }
         }
         else {
            cout << "WARNING: Lumi record in " << srcinfo.name(i) << " is not valid xml" << endl;
         }

	 log << coral::Info << "Collection " << srcinfo.name(i) << " has " << totalEvents
	     << " entries with " << uniqueRuns << " unique Runs"
	     << coral::MessageStream::endmsg;

         // make suitable collections, each with nEvents >= minEvents
         int heldEvents = 0;
         int runCounter = 0;
	 int collectionNumber = 1;
         std::vector<unsigned int> heldRuns;   //to hold GUIDs until minEvents reached 
         pool::ICollection* subCollection = 0;
	 string subCollName;
	 
	 // sort the GUIDs by cardinality
	 vector<pair<unsigned int,int> > sortedRuns( eventsPerRun.begin(), eventsPerRun.end() );
	 sort( sortedRuns.begin(), sortedRuns.end(), runPairSort );
	 
	 vector<pair<unsigned int,int> >::iterator runIter = sortedRuns.begin();
         while( runIter != sortedRuns.end() ) 
         {
            runCounter++;
            unsigned int run = runIter->first;
            int thisNumEvents = runIter->second;
            int counter = thisNumEvents + heldEvents;
	    bool collLimit = ( collectionNumber > maxSplit );

            // test for various conditions in which a new sub-collection ought to be created
            if( counter >= minEvents || runCounter >= uniqueRuns || collLimit ) 
            {
               if( collectionNumber == 1   // need to create at least 1 output collection
		   || ( counter >= minEvents   // enough events for a new collection
			&& !collLimit ) )   // but not over the max collection limit
               {
                  // create a new sub-collection
                  std::stringstream collectionNameStream; 
                  collectionNameStream << "sub_collection_" << collectionNumber; 
                  subCollName = collectionNameStream.str();
      
                  pool::CollectionDescription destDescription( srcinfo.name(i), "type" );
                  destDescription = nextDesc;
                  pool::CollectionDescription newDestDesc( destDescription );
                  newDestDesc.setName( subCollName );
                  newDestDesc.setType( subCollType );               
                  newDestDesc.setConnection( subCollConnect );
		  
                  subCollection = collectionService->create( newDestDesc, true );
                  subCollection->dataEditor().setRowCacheSize( 0 );
		  collectionNumber++;
               }

	       // map to appropriate GUID
	       subCollMap[ run ] = subCollection;
	       invCollMap.insert( std::pair<std::string, unsigned int>(subCollName, run));
	       for( size_t i=0; i<heldRuns.size(); i++)
	       { 
		  unsigned int thisId = heldRuns[i];
		  subCollMap[ thisId ] = subCollection; 
		  invCollMap.insert( std::pair<std::string, unsigned int>(subCollName, thisId));
	       }
	       heldRuns.clear();
	       heldEvents = 0;
	    }
            else {
	       // accumulate GUIDs to reach minimum event number per collection
               heldRuns.push_back( run ); 
               heldEvents += thisNumEvents;
            }
            ++runIter;
         }

         std::string lastCollName = "";
         for( std::multimap<std::string, unsigned int>::const_iterator invCollIter = invCollMap.begin(); 
              invCollIter != invCollMap.end(); 
              ++invCollIter) 
         { 
            std::string thisCollName = invCollIter->first;
            if (thisCollName == lastCollName)
               log << coral::Info << " " << invCollIter->second;
            else {   
               if (invCollIter != invCollMap.begin())
                  log << coral::Info << coral::MessageStream::endmsg; 
               log << coral::Info << "Created new sub-collection " << thisCollName << " with files:\t" << invCollIter->second;
            }
            lastCollName = thisCollName;
         }
         log << coral::Info << coral::MessageStream::endmsg; 
 
         // second loop: write events to correct sub-collection
         pool::ICollectionCursor& cursor2 = collQuery2->execute();
         // Token name to split on (if not specified, use default for each source coll)
         const std::string srcRefName = ( splitAtt.size()? splitAtt : collection->description().eventReferenceColumnName() );
         int cnt2 = 0;
         while( cursor2.next() ) {
            ++cnt2;
            //cout << "src " << i << ", cursor2 " << cnt2 << endl;
            // get file Run from event
            const coral::AttributeList &atts = cursor2.currentRow().attributeList();
            unsigned int run;
            try {
               run = atts[splitAtt].data<unsigned int>();
            }
            catch (coral::AttributeListException e) {
               cout << "Could not find splitting attribute " << splitAtt
                    << " for query2" << endl;
               cout << e.what();
               return 2;
            }
            pool::CollectionRowBuffer& rowBuffer = subCollMap[run]->dataEditor().rowBuffer();
            rowBuffer.setAttributeList(atts);
            subCollMap[run]->dataEditor().insertRow( rowBuffer );
         }

         log << coral::Info << "Finished writing all events from input collection " << collection->description().name() << coral::MessageStream::endmsg;
 
         //bool metaok = metainfo.copyMetadata(collection,subCollMap);
         //if (!metaok) cout << "Problem copying metadata for " << srcinfo.name(i) << endl;
         vector<pool::ICollection*> inputs, outputs;
         inputs.push_back(collection);
         outputs = vectorizeMapPayload(subCollMap);
         if (metainfo.checkMetadata(inputs,outputs)) {
            metainfo.readMetadata(inputs);
            metainfo.writeMetadata(outputs);
         }
         else {
            cout << "Problem copying metadata for " << srcinfo.name(i) << endl;
         }

         // commit changes and close all sub-collections
         std::map< unsigned int, pool::ICollection* >::iterator closeIter = subCollMap.begin();
         while ( closeIter != subCollMap.end() )
         {
            pool::ICollection* subCollection = closeIter->second;
            if( subCollection->isOpen() ) {
	       subCollection->commit();  
	    }
            ++closeIter;
         }

      } // for each input collection

      for( unsigned int i=0; i< srcinfo.nSrc(); i++ ) {      
         collMap[i]->close();
      }

      vector<pool::ICollection*> outputs;
      outputs = vectorizeMapPayload(subCollMap);
      metainfo.writeMetadata(outputs);

      //
      // Check for runs with no events
      // Add that record to ALL subcollections
      //
      std::map< unsigned int, pool::ICollection* >::iterator closeIter = subCollMap.begin();
      while ( closeIter != subCollMap.end() )
      {
         unsigned int run = closeIter->first;
         grl_total = grl_total.GetPartNotIn(runGRLMap[run]);
         ++closeIter;
      }
      //
      // Add split luminosity metadata + any no event runs
      //
      closeIter = subCollMap.begin();
      while ( closeIter != subCollMap.end() )
      {
         unsigned int run = closeIter->first;
         pool::ICollection* subCollection = closeIter->second;
         if( subCollection->isOpen() ) {
            Root::TGoodRunsListWriter blue;
            cout << "Metadata for run " << run 
                 << " and version " << runGRLMap[run].GetVersion() 
                 << " and metadata of size " << runGRLMap[run].GetMetaDataSize() << endl;
            if (runGRLMap.find(run) != runGRLMap.end()) {
               blue.SetGoodRunsList(runGRLMap[run]);
               stringstream srun; srun << run;
               subCollection->metadata().setValueForKey(srun.str() + "/" + lumikey,
                                                        string(blue.GetXMLString()));
            }
            if (!grl_total.IsEmpty()) {
               blue.SetGoodRunsList(grl_total);
               subCollection->metadata().setValueForKey("NORUNS/" + lumikey,
                                                        string(blue.GetXMLString()));
            }
            else {
               cout << "No empty runs" << endl;
            }
            subCollection->commit();  
            subCollection->close();
         }
         ++closeIter;
      }

      return 0;
   }
   catch( std::exception& exception )
   {
      std::cerr << "std::Exception: " << exception.what() << std::endl;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception. " << std::endl;
      return 1;
   }
} // nSrc loop
