/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollListAttrib.cpp
 * @brief Utility to list the metadata of a POOL collection
 * @author K. Karr <Kristo.Karr@cern.ch>
 * @author Marcin.Nowak@cern.ch
 */

#include "PersistentDataModel/Token.h"

#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/ICollectionColumn.h"
#include "CollectionBase/ICollectionCursor.h"
#include "CollectionBase/CollectionRowBuffer.h"
#include "CollectionBase/CollectionService.h"

#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"

#include "CoralBase/AttributeList.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/MessageStream.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/CmdLineArgs2.h"
#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CollectionUtilities/UtilityFuncs.h"
#include "CxxUtils/checker_macros.h"

#include <iostream>
#include <iomanip>
#include <fstream>

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

using namespace std;
using namespace pool;


int main(int argc, const char *argv[])
{
   string thisProgram("CollListAttrib");
   SystemTools::initGaudi();

   try  { 
      pool::CollectionService   collectionService;
      coral::MessageStream log( thisProgram );

      Args2Container argsVec( thisProgram, true, &log );
      argsVec.desc << "Lists the attributes of a collection "
                   << "or pair of collections. " << endl << "If it is a pair, it will also "
                   << "tell which attributes are shared and which are not. " << endl
                   << "It will also print this information to files." ;

      // CLA specific qualifiers
      pool::QualList markers;
      markers["-nogroups"] = ArgQual("Do not print group information", 0);
      markers["-notokens"] = ArgQual("Do not print token information", 0);
      markers["-noattribs"] = ArgQual("Do not print attribute information", 0);

      pool::CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      argsVec.push_back(&cmdLineArgs);  // Add it to the list

      // Classes with shared cli keys and their argument properties
      // Add them to the list
      // SrcInfo : input collections
      // CatInfo : collection catalogs
      CatalogInfo catinfo; argsVec.push_back(&catinfo); 
      SrcInfo     srcinfo; argsVec.push_back(&srcinfo);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      bool doGroups = !cmdLineArgs.hasQual("-nogroups");
      bool doTokens = !cmdLineArgs.hasQual("-notokens");
      bool doAtts   = !cmdLineArgs.hasQual("-noattribs");

      if (srcinfo.nSrc()>2) {
	 cerr << "This program takes a max of 2 input collections" << endl;
	 cerr << "There were " << srcinfo.nSrc() << " inputs collections on the command line" << endl;
	 return 1;
      }
      
      catinfo.setCatalogs( &collectionService );
  
      unsigned int collCounter = 0;
      std::string attribName = "";
      std::string attribTypeName = "";
	
      map<int, pool::ICollection*> collMap;
      map<int, vector<pair<string,string> > > tokSpecs;
      map< int, vector< pair<string,string> > > attSpecs;
	 
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ )  {
	 bool readOnly( true );
	 pool::ICollection* collection;
	 try {
	    collection = collectionService.handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly );
	 }
	 catch ( pool::Exception& poolException ) {
	    std::cerr << "pool::Exception: " << poolException.what() << std::endl;
	    cout << "Unable to open collection " << srcinfo.name(i) << endl;
	    cout << "---> STOPPING HERE" << endl;
	    return 5;
	 }

	 const pool::ICollectionDescription &description = collection->description();
	 collMap[i] = collection;
	 collCounter++;
	 vector< pair<string,string> > spec;
	 for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
	    for( int c=0; c < description.numberOfAttributeColumns( f ); c++ ) {
	       const pool::ICollectionColumn&   column = description.attributeColumn(c, f);
	       spec.push_back(make_pair(column.name(),column.type()));
	    }
	 }
	 attSpecs[i]=spec;
	 spec.clear();
	 for( int f=0; f < description.numberOfCollectionFragments(); f++ ) {
	    for( int c=0; c < description.numberOfTokenColumns( f ); c++ ) {
	       const pool::ICollectionColumn&   column = description.tokenColumn(c, f);
	       spec.push_back(make_pair(column.name(),column.type()));
	    }
	 }
	 tokSpecs[i]=spec;
      }


      std::cout << "--------------------------------------------------------------" << std::endl;
      std::cout << "Collection list: " << endl;
      for (unsigned int i=0; i < collCounter; ++i) {
	 std::cout << "NAME: ";
	 std::cout << collMap[i]->description().name();
	 std::cout << "   TYPE: ";
	 std::cout << collMap[i]->description().type();
	 std::cout << "   NFRAG: ";
	 std::cout << collMap[i]->description().numberOfCollectionFragments();
	 std::cout << std::endl;
      }

      if (doGroups) dumpGroups(collMap[0]->description());
         
      if (collCounter==1) {
	 // For a single input, just dump the description
	 if (doTokens) dumpTokens(collMap[0]->description());
         if (doAtts) dumpAttributes(collMap[0]->description());
      }
      else if (collCounter == 2 && 
	       collMap[0]->description().equals(collMap[1]->description())
	       ) {
	 // If the two have identical descriptions, then dump the first one
	 std::cout << "--------------------------------------------------------------" << std::endl;
	 std::cout << " ***" << std::endl;
	 std::cout << " *** Tokens and Attributes IDENTICAL for these Collections ***" << std::endl;
	 std::cout << " ***" << std::endl;
	 if (doTokens) dumpTokens(collMap[0]->description());
         if (doAtts) dumpAttributes(collMap[0]->description());
      }
      else {
	 // If they differ, then 
	 // -- First, Look at token differences
	 //    -- If not any, then dump tokens
	 //    -- If differences exist, dump common tokens, then differences
	 //    -- also write 3 files: common.tokens, <coll1>Only.tokens, <coll2>Only.tokens
	 // -- Next, look at attribute differences
	 //    -- If not any, then dump attributes
	 //    -- If differences exist, dump common attributes, then differences
	 //    -- also write 3 files: common.atts, <coll1>Only.atts, <coll2>Only.atts

	 stable_sort(tokSpecs[0].begin(),tokSpecs[0].end());
	 stable_sort(tokSpecs[1].begin(),tokSpecs[1].end());
	 stable_sort(attSpecs[0].begin(),attSpecs[0].end());
	 stable_sort(attSpecs[1].begin(),attSpecs[1].end());
	 // Look at token intersection first
	 if (doTokens) dumpOverlapEval(collMap[0]->description(),
			 collMap[1]->description(),
			 tokSpecs[0],
			 tokSpecs[1],
			 "Token");
	 // Look at attribute intersection second
         if (doAtts) dumpOverlapEval(collMap[0]->description(),
			 collMap[1]->description(),
			 attSpecs[0],
			 attSpecs[1],
			 "Attribute");
      }
      std::cout << "---------------------------------------------------------" << std::endl;
      std::cout << std::endl;
      std::cout << "Number of collections scanned is: " 
		<< collCounter << std::endl;
      std::cout << std::endl;

      for( unsigned int i=0; i < srcinfo.nSrc(); i++ ) {      
	 collMap[i]->close();
      }

      return 0;
   }
   catch( pool::Exception& poolException )
   {
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;
      return 1;
   }
   catch( std::exception& exception )
   {
      std::cerr << "std::exception: " << exception.what() << std::endl;
      return 1;
   }
   catch( ... )
   {
      std::cerr << "Unknown exception caught in main()." << std::endl;
      return 1;
   }
}

