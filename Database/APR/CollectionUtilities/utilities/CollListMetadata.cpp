/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CollListMetadata.cpp
 * @brief Utility to print metadata from a POOL collection
 * @author Marcin.Nowak@cern.ch
 */
#include "CollectionBase/ICollectionDescription.h"
#include "CollectionBase/CollectionService.h"
#include "CollectionBase/ICollection.h"
#include "CollectionBase/ICollectionMetadata.h"

#include "CoralBase/MessageStream.h"
#include "POOLCore/Exception.h"
#include "POOLCore/SystemTools.h"

#include "CollectionUtilities/Args2Container.h"
#include "CollectionUtilities/SrcInfo.h"
#include "CollectionUtilities/CatalogInfo.h"
#include "CollectionUtilities/CollectionMetadataParser.h"

#include <xercesc/dom/DOM.hpp> 
#include <xercesc/dom/DOMImplementation.hpp> 
#include <xercesc/dom/DOMImplementationLS.hpp> 
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMNode.hpp> 
#include <xercesc/parsers/XercesDOMParser.hpp> 
 
#include <xercesc/sax/SAXException.hpp> 
 
#include <xercesc/framework/StdOutFormatTarget.hpp> 
#include <xercesc/framework/LocalFileFormatTarget.hpp> 
         
#include <xercesc/util/XMLUni.hpp> 
#include <xercesc/util/PlatformUtils.hpp> 
#include <xercesc/util/OutOfMemoryException.hpp> 
#include <xercesc/util/TransService.hpp> 

XERCES_CPP_NAMESPACE_USE 

using namespace std;
using namespace pool;

int main(int argc, const char *argv[])
{
   string thisProgram("CollListMetadata");
   string file("CollMetadata.xml");
   SystemTools::initGaudi();
  
   try
   { 
      coral::MessageStream log( thisProgram );
      pool::CollectionService   collectionService;

      // vector of CmdLineArg objects
      Args2Container argsVec(thisProgram, true, &log);
      argsVec.desc << thisProgram
                   << " is a tool for listing metadata stored with POOL Collections."
                   << endl;

      QualList markers;
      markers["-xmlSummary"] =  ArgQual("Produce CollMetadata.xml with metadata for all nodes", 0);
      markers["-raw"] =  ArgQual("Prints metadata without any parsing (forced ON right now)", 0);
      markers["-stat"] = ArgQual("Prints metadata statistics only", 0);
      markers["-stats"] = markers["-stat"];
      
      CmdLineArgs2 cmdLineArgs;
      cmdLineArgs.setArgQuals(markers);
      argsVec.push_back(&cmdLineArgs); 

      CatalogInfo catinfo; argsVec.push_back(&catinfo);
      SrcInfo srcinfo;     argsVec.push_back(&srcinfo);

      // Check that all cmd line args are valid
      if( !argsVec.evalArgs(argc, argv) ) return 1;
      if( !argsVec.checkValid() ) return 1;

      bool summary = cmdLineArgs.hasQual("-xmlSummary");
      bool raw = cmdLineArgs.hasQual("-raw");
      bool stats = cmdLineArgs.hasQual("-stat") || cmdLineArgs.hasQual("-stats"); 
      
      catinfo.setCatalogs( &collectionService );


      std::map<int, pool::ICollection*> collMap;
      std::map< std::string, int > pfnMap; 
      for( unsigned int i=0; i<srcinfo.nSrc(); i++ ) {
         cout<< "Printing metadata from collection '" <<  srcinfo.name(i)
             << "', type: " <<  srcinfo.type(i)
             << endl;

         bool readOnly( true );
         pool::ICollection *collection = collectionService.handle( srcinfo.name(i), srcinfo.type(i), srcinfo.connect(), readOnly );

         long long entries = collection->metadata().entries();
         cout << "Metadata entries: " << entries << endl;

         if( stats ) {
            size_t keymax(0), keytot(0), valmax(0), valtot(0), valzero(0);
            pool::ICollectionMetadata::const_iterator  mdIter = collection->metadata().begin();
            while( mdIter != collection->metadata().end() ) {
               size_t keylen = mdIter.key().length();
               size_t vallen = mdIter.value()? strlen(mdIter.value()) : 0;
               if( keylen > keymax ) keymax = keylen;
               keytot += keylen;
               if( vallen > valmax ) valmax = vallen;
               valtot += vallen;
               if( vallen == 0 )  valzero++;
               ++mdIter;
            }
            if( !entries ) {
               cout << "No metadata found!" << endl;
            } else {
               cout << "Maximum key length  : " << keymax <<", average: " << keytot/entries << endl;
               cout << "Maximum value length: " << valmax <<", average: " << valtot/entries
                    << ", empty values count: " << valzero
                    << endl;
            }
            return 0;
         }
	  
       
         if( raw ) {
            pool::ICollectionMetadata::const_iterator  mdIter = collection->metadata().begin();
            while( mdIter != collection->metadata().end() ) {
               cout << "  Metadata Key= " << mdIter.key();
               cout << ",  Value= " << mdIter.value() << endl;
               ++mdIter;
            }
            return 0;
         }
       
         // send the metadata to the parser
         CollectionMetadataParser parser;
         parser.init(collection->metadata());
         // get a list of the nodes and iterate over the metadata for each node
         for (unsigned int n = 0; n < parser.numLevels(); ++n) {
            cout << endl << " >>> " << srcinfo.name(i)
                 << " provenance LEVEL: " << n << " <<<" 
                 << endl << endl;
            set<ProvNode> nodes = parser.listNodes(n);
            //set<ProvNode> nodes = parser.listNodes();
            for (set<ProvNode>::const_iterator nodes_it  = nodes.begin();
                 nodes_it != nodes.end();
                 ++nodes_it) {
               set< pair<MetaKey,MetaValue> > data = parser.getUniqueMetaForNode(*nodes_it);
               cout << "Metadata from Node " << *nodes_it << ":" << endl;
               for (set< pair<MetaKey,MetaValue> >::const_iterator it = data.begin();
                    it != data.end();
                    ++it) {
                  cout << "  Key=" << it->first << "  Value=" << it->second << endl;
               }
            }
         }

         if (summary) {
            try {
               const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};
               DOMImplementation* impl(DOMImplementationRegistry::getDOMImplementation (ls_id));
               DOMDocumentType* CMTType = impl->createDocumentType(XMLString::transcode("CollMetadataTree"),
                                                                   XMLString::transcode(""),
                                                                   XMLString::transcode("") );
               DOMDocument* newDocument = impl->createDocument(0, XMLString::transcode("CollMetadataTree"), CMTType);
               set<ProvNode> nodes = parser.listNodes();

               for (set<ProvNode>::const_iterator nodes_it  = nodes.begin();
                    nodes_it != nodes.end();
                    ++nodes_it) {
                  DOMElement* provbase = newDocument->createElement(XMLString::transcode("ProvNode"));
                  provbase->setAttribute(XMLString::transcode("name"),
                                         XMLString::transcode((*nodes_it).c_str()));
                  newDocument->getDocumentElement()->appendChild(provbase);
                  set< pair<MetaKey,MetaValue> > metas = parser.getUniqueMetaForNode(*nodes_it);
                  set< pair<MetaKey,MetaValue> >::const_iterator metit = metas.begin();
                  while(metit != metas.end()) {
                     DOMElement* meta_el = newDocument->createElement(XMLString::transcode("KeyValuePair"));
                     meta_el->setAttribute(XMLString::transcode("key"),
                                           XMLString::transcode(metit->first.c_str()));
                     meta_el->setAttribute(XMLString::transcode("value"),
                                           XMLString::transcode(metit->second.c_str()));
                     provbase->appendChild(meta_el);
                     ++metit;
                  }
               }
               // get a writer instance
               DOMLSSerializer   *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

               if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true))
                  theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true);

               if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
                  theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

               if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTBOM, true))
                  theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTBOM, true);

               // set a target as the file argument
               //std::string file("CollMetadata.xml");
               std::cout << "About to write summary file " << file << std::endl;
               XMLFormatTarget* myFormTarget = new LocalFileFormatTarget(file.c_str());

               // write document to target
               // See http://xerces.apache.org/xerces-c/program-dom-3.html
               DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
               theOutput->setByteStream(myFormTarget);
               theSerializer->write(newDocument, theOutput);

               // clean up the mess
               if (theSerializer!=NULL) delete theSerializer;
               if (theOutput!=NULL) delete theOutput;
               if (myFormTarget!=NULL)   delete myFormTarget;
               if (newDocument!=NULL)     delete newDocument;
            }
            catch (const SAXException& e) {
               char* s = XMLString::transcode(e.getMessage( ));
               std::cout << "xml error: " << s << "\n";
               XMLString::release (&s);
            }
            catch (const DOMException& e) {
               char* s = XMLString::transcode(e.getMessage( ));
               std::cout << "xml error: " << s << "\n";
               XMLString::release (&s);
            }
         }
      }

   
      log << coral::Info << "Finished listing metadata" <<  coral::MessageStream::endmsg;
      return 0;

   }
   catch( pool::Exception& poolException )
   {
      std::cerr << "pool::Exception: " << poolException.what() << std::endl;
      return 1;
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
}

