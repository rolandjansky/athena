/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
   Test program for TinyXML.
*/


#ifdef TIXML_USE_STL
	#include <iostream>
	#include <sstream>
	using namespace std;
#else
	#include <stdio.h>
#endif

#if defined( WIN32 ) && defined( TUNE )
	#include <crtdbg.h>
	_CrtMemState startMemState;
	_CrtMemState endMemState;
#endif

#include "tinyxml.h"

static int gPass = 0;
static int gFail = 0;


bool XmlTest (const char* testString, const char* expected, const char* found, bool noEcho = false)
{
	bool pass = !strcmp( expected, found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");

	if ( noEcho )
		printf (" %s\n", testString);
	else
		printf (" %s [%s][%s]\n", testString, expected, found);

	if ( pass )
		++gPass;
	else
		++gFail;
	return pass;
}


bool XmlTest( const char* testString, int expected, int found, bool noEcho = false )
{
	bool pass = ( expected == found );
	if ( pass )
		printf ("[pass]");
	else
		printf ("[fail]");

	if ( noEcho )
		printf (" %s\n", testString);
	else
		printf (" %s [%d][%d]\n", testString, expected, found);

	if ( pass )
		++gPass;
	else
		++gFail;
	return pass;
}


//
// This file demonstrates some basic functionality of TinyXml.
// Note that the example is very contrived. It presumes you know
// what is in the XML file. But it does test the basic operations,
// and show how to add and remove nodes.
//

int main()
{
	//

		TiXmlDocument doc( "mytest.xml" );
		bool loadOkay = doc.LoadFile();

		if ( !loadOkay )
		{
			printf( "Could not load test file 'amdb_simrec.r.02.02.Initial.Light'. Error='%s'. Exiting.\n", doc.ErrorDesc() );
			exit( 1 );
		}

		printf( "** Demo doc read from disk: ** \n\n" );
		printf( "** Printing via doc.Print **\n" );
		doc.Print( stdout );

		{
			printf( "** Printing via TiXmlPrinter **\n" );
			TiXmlPrinter printer;
			doc.Accept( &printer );
			fprintf( stdout, "%s", printer.CStr() );
		}
		
		TiXmlNode* node = 0;
		TiXmlElement* todoElement = 0;
		node = doc.RootElement();
		assert( node );
		XmlTest( "Root element exists.", true, ( node != 0 && node->ToElement() ) );
		XmlTest ( "Root element value is 'AGDD'.", "AGDD",  node->Value());
		todoElement = node->ToElement();
		TiXmlElement *element, *subelement;
		for( element = todoElement->FirstChildElement();
			 element;
			 element = element->NextSiblingElement() )
		{
			std::cout<<" element  "<<element->Value()<<std::endl;
			for (subelement=element->FirstChildElement();subelement;
				subelement = subelement->NextSiblingElement() )
					std::cout<<" \telement  "<<subelement->Value()<<std::endl;
		}
}

