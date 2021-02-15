/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#if XERCES_VERSION_MAJOR < 3
#include <xercesc/dom/DOMWriter.hpp>
#else
#include <xercesc/dom/DOMLSSerializer.hpp>
#endif
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>

#include <xercesc/sax/SAXException.hpp>

#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/util/TransService.hpp>

#include <ctime>
#include <stdexcept>

#include "CollectionUtilities/ArgQual.h"
#include "CollectionUtilities/CmdLineArgs2.h"
#include "CollectionUtilities/Args2Container.h"
#include <boost/scoped_array.hpp>

#define corENDL coral::MessageStream::endmsg

XERCES_CPP_NAMESPACE_USE

using namespace pool;
using namespace std;

// Stole this little snippet from OOPWeb.com 
// http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
// to fill in a basic failure in std::string 
// Essentially it does for C++ what strtok does for C

void Tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiters = " ")
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/***************************************************************

Args2Container is an extension of a vector of CmdLineArgs2

It has a name and a desc. Name is normally the program name.
It also adds several utility methods such as

printHelp(verbose) :
   - (verbose = false) print out names of ArgQuals 
     for all contained pointers
   - (verbose = true) print out names _and_ desc of ArgQuals 
     for all contained pointers

checkValid() : Loop over pointers and call valid() method

evalArgs(argv) : save time/errors by calling evalArgs 
   on the container rather than the individual CmdLineArg2's

***************************************************************/
Args2Container::Args2Container( std::string name, bool doxml, coral::MessageStream *l )
      : m_log( l? *l : *new coral::MessageStream("CmdLine") ),
        m_name(name),
        m_xml(doxml),
        m_xmlOpt( FILEONLY ),
	m_ver( V2 ),
        m_a2c_cla2("local"),
	m_xFileName(name)
{
   QualList markers;
   markers["-help"] = ArgQual("Print detailed description of all options", 0);
   markers["-h"] = markers["-help"];
   markers["-xmlMode"].desc << "How to merge xml and cli arguments when both present. "
			    << "Options = (FILEONLY, OVERRIDE, APPEND) DEFAULT = OVERRIDE";
   markers["-xmlInput"].desc << "This takes a file name as input. " << std::endl
                             << "NOTE: The CLI args will be discarded! ";
   markers["-xmlOut"].desc << "This takes a name for the xml file with the arguments "
                           << "Default = utility name, e.g util.xml";
   markers["-compatibility"].desc << "If this is set, then it uses the old xml format" << std::endl 
                                  << "  Options are : "; 
   for (unsigned int i=0; i<NUM_VERSIONS; ++i) { 
      markers["-compatibility"].desc << " V" << i+1; 
   }
   m_a2c_cla2.setArgQuals(markers);
   this->push_back(&m_a2c_cla2);
}

void Args2Container::init()
{
   for( Args2Container::iterator it = this->begin(); it != this->end(); ++it )
      (*it)->init();
}

//-----------------------------------------------------------------------
// printHelp : loop over contents and print name (and desc) of qualifiers
//-----------------------------------------------------------------------
void Args2Container::printHelp(bool verbose) const
{
   std::cerr << "Usage: " << m_name << std::endl;
   if (desc.str().size()>0)
      std::cerr << desc.str() << std::endl; 
   for (Args2Container::const_iterator cmdit = this->begin(); cmdit != this->end(); ++cmdit)
   {
      for (QualList::const_iterator it = (*cmdit)->getArgQuals().begin();
                                    it != (*cmdit)->getArgQuals().end();
                                  ++it)
      {
         std::cerr << "Option = " << it->first << " ";
         if ( verbose ) it->second.print(std::cerr);
         std::cerr << std::endl;
      }
      std::cerr << std::endl;
   }
}

bool Args2Container::checkValid() const
{
   for (Args2Container::const_iterator ait = this->begin(); ait != this->end(); ++ait) {
      if ( (*ait)->valid() == false ) {
         std::cerr << "Invalid Qualifier" << std::endl;
         return false;
      }
   }
   return true;
}

//
// Evaluate the argv vector for a named argument
//
bool Args2Container::evalArgs(const std::string cliarg, std::vector<std::string>& argv)
{
   // Evaluate the args for the Args2Container local cla2
   if( ! m_a2c_cla2.evalArgs(argv) ) {
      m_log << coral::Error << "Args2Container has invalid internal argument" << corENDL;
      return false;
   }
   // Print out detailed help if requested 
   if( m_a2c_cla2.hasQual("-help") ||  m_a2c_cla2.hasQual("-h") ) {
      printHelp(true);
      return false;
   }
   // Print out simple help  if not enough options given
   if( argv.size() < 2 ) {
      printHelp(false);
      return false;
   }
   
   // If this is OK then check if xmlInput is one of the options
   bool xmlpresent = false;
   CmdLineArgs2::iterator clit = m_a2c_cla2.begin();
   while (clit!=m_a2c_cla2.end()) {
      if (clit->first == "-compatibility") {
         if (argv[clit->second.first]=="V1") m_ver=V1;
         else if (argv[clit->second.first]=="V2") m_ver=V2;
         else { m_log << coral::Error << "compatibility version " << argv[clit->second.first] 
                    << " not supported" << corENDL; }
      }
      if (clit->first == "-xmlOut") {
         m_xFileName = argv[clit->second.first];
         m_log << coral::Info << "Will use " << m_xFileName << " for output xml filename"
             << corENDL;
      } 
      if (clit->first == "-xmlMode") { 
         std::string mode = argv[clit->second.first]; 
         if      (mode=="FILEONLY") m_xmlOpt = FILEONLY; 
         else if (mode=="OVERRIDE") m_xmlOpt = OVERRIDE; 
         else if (mode=="APPEND")   m_xmlOpt = APPEND; 
         else {  
            m_log << coral::Warning << "Unrecognized mode : " << mode << corENDL;
         } 
      }
      ++clit;
   }
   // Don't rewrite actual argv with xml until all other CLI arguments have been checked
   // ---> Use second loop
   std::string file;
   if( m_a2c_cla2.getOpt("-xmlInput", file) ) {
      m_log << coral::Info << "Taking input from xml file: " << file << corENDL;
      //argv = this->fillCmdLineArgsFromXML(file);
      xmlpresent = true; 
   } 

   if (xmlpresent) { 
      // if xml parameter are present, then need to decide how to  
      // merge them with any cli parameters 
 
      // Step 1 : Get evaluation of argv from cli 
      std::string exeName = argv[0]; 
 
      // If Args2Container is valid, then eval args for all content cla2's 
      for (Args2Container::const_iterator ait = this->begin(); ait != this->end(); ++ait) {
         if ( cliarg=="ALL" || (*ait)->name()==cliarg) {
	    (*ait)->ignoreMissingArgs( true );
            (void) (*ait)->evalArgs(argv); 
	    (*ait)->ignoreMissingArgs( false );
         } 
      } 
      //for (int l = 0; l < argv.size(); ++l) std::cout << l << " | " << argv[l] << std::endl; 
 
      // Step 2 : put the markers plus contents for the cli evalArgs into a map 
 
      std::map<std::string,std::vector<std::string> > cliArgFrags; 
      for (Args2Container::const_iterator cl_it = this->begin(); cl_it != this->end(); ++cl_it) { 
         for ( CmdLineArgs2::iterator it = (*cl_it)->begin(); it != (*cl_it)->end(); ++it) { 
            std::string marker = it->first; 
            std::vector<std::string> list; 
            //std::cout << "In cliarg loop for " << marker << std::endl; 
            for (int index = it->second.first; index < it->second.second; ++index) { 
               list.push_back(argv[index]); 
               //std::cout << "In cliarg loop : index " << index << " : " << argv[index] << std::endl; 
            } 
            cliArgFrags.insert(std::make_pair(marker,list)); 
         } 
      } 
   
      // Step 3 : remake argv based on xml contents and re-evaluate arguments 
 
      // If Args2Container is valid, then eval args for all content cla2's
      argv = this->fillCmdLineArgsFromXML(file); 
      // Now check whether any merging is needed 
      if (m_xmlOpt != FILEONLY) { 
         //std::cout << "I am processing non fileonly option " << m_xmlOpt << std::endl; 
         this->init(); 
         for (Args2Container::const_iterator ait = this->begin(); ait != this->end(); ++ait) { 
            if ( cliarg=="ALL" || (*ait)->name()==cliarg) { 
               //bool disregardMe = (*ait)->evalArgs(argv); 
               (void) (*ait)->evalArgs(argv); 
            } 
         } 
 
         // Step 4 : put the markers plus contents for the xml evalArgs into a map 
         //for (int l = 0; l < argv.size(); ++l) std::cout << l << "::" << argv[l] << std::endl; 
       
         std::map<std::string,std::vector<std::string> > xmlArgFrags; 
         for (Args2Container::const_iterator cl_it = this->begin(); cl_it != this->end(); ++cl_it) { 
            for ( CmdLineArgs2::iterator it = (*cl_it)->begin(); it != (*cl_it)->end(); ++it) { 
               std::string marker = it->first; 
               std::vector<std::string> list; 
               for (int index = it->second.first; index < it->second.second; ++index) { 
                  list.push_back(argv[index]); 
                  //std::cout << "In xmlarg loop : index " << index << " : " << argv[index] << std::endl; 
               } 
               xmlArgFrags.insert(std::make_pair(marker,list)); 
            } 
         } 
 
         // Step 5 : using config parameters merge the xml and cli map contents into a new argv 
 
         std::vector<std::string> argv_new; 
         // Loop over command line map 
         for (std::map<std::string,std::vector<std::string> >::iterator it = cliArgFrags.begin();  
	      it != cliArgFrags.end(); ++it) { 
            std::map<std::string,std::vector<std::string> >::iterator fit = xmlArgFrags.find(it->first); 
            // if ovveride, then replace xml map with commandline map 
            if (fit != xmlArgFrags.end() && m_xmlOpt == OVERRIDE) fit->second = it->second; 
            // if append, then add commandline args to xml args 
            if (fit != xmlArgFrags.end() && m_xmlOpt == APPEND) { 
               for (unsigned int j = 0; j < it->second.size(); ++j)  
                  fit->second.push_back( (it->second)[j] ); 
            } 
         } 
         // Now take xmlArgFrags as the merged map and rebuild the argv from its contents 
         argv_new.push_back(exeName); 
         for (std::map<std::string,std::vector<std::string> >::iterator it =xmlArgFrags.begin();  
	      it != xmlArgFrags.end(); ++it) { 
            argv_new.push_back(it->first); 
            for (unsigned int j = 0; j < it->second.size(); ++j) argv_new.push_back( (it->second)[j] ); 
         } 
         argv = argv_new; 
         //std::cout << "Now dump merged argv" << std::endl; 
         //for (unsigned int j = 0; j < argv.size(); ++j) std::cout << " >> " << argv[j] << std::endl; 
      } 
   } 
 
   // POST ARGV MANIPULATION

   // If Args2Container is valid, then eval args for all content cla2's
   this->init(); 
   //for (int l = 0; l < argv.size(); ++l) std::cout << l << " | " << argv[l] << std::endl;
   for (Args2Container::const_iterator ait = this->begin(); ait != this->end(); ++ait) {
      if ( cliarg=="ALL" || (*ait)->name()==cliarg) {
         if ( (*ait)->evalArgs(argv) == false ) {
            m_log << coral::Error << "Unable to eval args for " << (*ait)->name() << corENDL;
            return false;
         }
      }
   }
   for( Args2Container::const_iterator ait = begin(); ait != end(); ++ait ) {
      if ( cliarg=="ALL" || (*ait)->name()==cliarg) {
         if ( (*ait)->evalArgsPass2(argv, *this) == false ) {
            m_log << coral::Error << "Unable to eval args (pass2) for " << (*ait)->name() << corENDL;
            return false;
         }
      }
   }

   // If output xml is requested ...
   // Note that it always contains what was finally evaluated, NOT just the command line
   if (doXML()) {
      try {
         XMLPlatformUtils::Initialize();
      }
      catch( const XMLException& ) {
         m_log << coral::Error << "Error during initialization! :" << corENDL << corENDL;
      }
      writeXMLContent(argv);
   }
   return true;
}

//
// Evaluate the argv vector for ALL contents
//
bool Args2Container::evalArgs(std::vector<std::string>& argv)
{
   // MN: this is a hack to allow users skip the pesky RootCollection type specifier
   
   // don't try iterators, they are invalidated by insertion
   bool edited = false;
   for( size_t i=0; i<argv.size(); ) {
      if( argv[i] == "-src" || argv[i] == "-dst" ) {
         ++i;
         while( i < argv.size() && argv[i][0] != '-' ) {
            size_t len = argv[i].length();
            if( len>5 && argv[i].substr(len-5,5) == ".root" ) {
               if( i+1 == argv.size() || (argv[i+1] != "RootCollection" && argv[i+1] != "PHYSICAL_NAME" && argv[i+1] != "LOGICAL_NAME") ) {
                  // add the missing "RootCollection" 
                  argv.insert( argv.begin()+i+1, "RootCollection" );
                  edited = true;
                  if( argv[i].substr(0,4) != "PFN:" ) {
                     // and cut off the .root extension
                     argv[i] = argv[i].substr(0,len-5);
                  }
               }
            }
            i++;
         }
      } else i++;
   }

   if( edited ) {
      m_log << coral::Warning << "Fixed command line options for RootCollections. New arguments are: " << corENDL << "> ";
      size_t i=0;
      while( i < argv.size() ) {
         m_log << coral::Warning << argv[i] << " " ; ++i;
      }
      m_log << coral::Warning << corENDL;
   }
   
   if( !evalArgs("ALL", argv) ) return false;
   m_argv = argv;

   QualList all_opts;
   for( Args2Container::const_iterator cmdit = begin(); cmdit != end(); ++cmdit )  {
      all_opts.insert( (*cmdit)->getArgQuals().begin(),  (*cmdit)->getArgQuals().end() );
   }
   for( CmdLineArgs2::const_iterator oi = m_a2c_cla2.begin(); oi !=  m_a2c_cla2.end(); ++oi ) {
      if( all_opts.find( oi->first ) == all_opts.end() ) {
         m_log << coral::Error << "Unknown option! " << oi->first << endl<< corENDL;
         printHelp( false );
         return false;
      }
   }
   return true;
}


bool Args2Container::evalArgs( int argc, const char *argv[] )
{
   m_argv.resize( argc );
   for( int i=0; i<argc; ++i )
      m_argv[i] = argv[i];
   return evalArgs( m_argv );
}



typedef std::basic_string<XMLCh> XercesString;

// Converts from a narrow-character string to a wide-character string.
inline XercesString fromNative(const char* str)
{
    boost::scoped_array<XMLCh> ptr( XMLString::transcode(str) );
    return XercesString(ptr.get( ));
}

// Converts from a narrow-character string to a wide-charactr string.
inline XercesString fromNative(const std::string& str)
{
    return fromNative(str.c_str( ));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XMLCh* str)
{
   if( !str ) return "";
   boost::scoped_array<char> ptr( XMLString::transcode(str) );
   return std::string(ptr.get( ));
}

// Converts from a wide-character string to a narrow-character string.
inline std::string toNative(const XercesString& str)
{
    return toNative(str.c_str( ));
}





void Args2Container::writeXMLContent(std::vector<std::string>& argv)
{
  // Part 1 - was defineXMLContent()

   // Initialaize to zero
   DOMDocument* newDocument = 0;
   try {
    // Create empty document based on Args2Container.dtd
      const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};
      DOMImplementation* impl(DOMImplementationRegistry::getDOMImplementation (ls_id));
      DOMDocumentType* Arg2Type = impl->createDocumentType(XMLString::transcode("ArgList"),
                                                           XMLString::transcode(""),
                                                           XMLString::transcode("") );
      newDocument = impl->createDocument(0, XMLString::transcode("ArgList"), Arg2Type);

      // Create the base element, assign the id, and add to tree
      DOMElement* toolBase = newDocument->createElement(XMLString::transcode("ToolInfo"));
      toolBase->setAttribute(XMLString::transcode("toolID"),
                             XMLString::transcode(m_name.c_str()));
      time_t tm = time(NULL);
      char * date = asctime(localtime(&tm));
      toolBase->setAttribute(XMLString::transcode("date"),
                             XMLString::transcode(date));
      newDocument->getDocumentElement()->appendChild(toolBase);

      // Loop over container contents
      for (Args2Container::const_iterator cmdit = this->begin(); cmdit != this->end(); ++cmdit)
      {
         // Loop over ArgQual's in each CLA2
         for (QualList::const_iterator it = (*cmdit)->getArgQuals().begin();
                                       it != (*cmdit)->getArgQuals().end();
                                     ++it)
         {
            // Get the pointer to the CLA2
            CmdLineArgs2* ptr = (*cmdit);
	    if (ptr->name() != "local") { 
	       // Get pointer to the data for the qualifier 
	       CmdLineArgs2::const_iterator ptr_it = ptr->find(it->first); 
	       int first=0,last=0; 
	       // Second part of iterator is pair of endpoints for the data in the argv 
	       if (ptr_it != ptr->end()) {first=ptr_it->second.first;last=ptr_it->second.second;} 
	       else continue; 
	       // Fill a stringstream with everything in the argv for this qualifier 
	       std::stringstream argString; 
	       for (int j=first; j<last; ++j) {  
		  argString << argv[j]; 
		  if (j < (last - 1) ) argString << " "; 
	       } 
	       // Create an element for the CLA2 and append it to the toolBase 
	       DOMElement* cliArg = newDocument->createElement(XMLString::transcode("CliArg")); 
	       toolBase->appendChild(cliArg); 
	       // Then set the attribute data name,option 
	       //   and create a child text node with the data for that option 
	       cliArg->setAttribute(XMLString::transcode("name"), 
				    XMLString::transcode((*cmdit)->name().c_str())); 
	       cliArg->setAttribute(XMLString::transcode("option"), 
				    XMLString::transcode(it->first.c_str())); 
	       // Check whether argument takes data 
	       if (it->second.argsize) {  
		  if (m_ver==V1) { 
		     m_log << coral::Info  << "using compatibility for xml" << corENDL;
		     cliArg->appendChild( 
			newDocument->createTextNode(XMLString::transcode(argString.str().c_str())) 
			); 
		  } 
		  else { 
		     for (int j=first; j<last; ++j) { 
			//std::cout << it->first << " : " << j << " : " << argv[j] << std::endl; 
			DOMElement* datum = newDocument->createElement(XMLString::transcode("datum")); 
			cliArg->appendChild(datum); 
			datum->appendChild( 
			   newDocument->createTextNode(XMLString::transcode(argv[j].c_str())) 
			   ); 
		     } 
		  } // version check 
	       }
	    }
	 } // QualList iterator 
      } // Args2Container iterator 	    
   }
   catch (...) {
      m_log << coral::Error << "Problem in defineXMLContent" << corENDL;
      // AV Would it not be better to reset newDocument=0 and exit?
   }

   // If the document could not be created, exit
   if (!newDocument) {
      m_log << coral::Error << "Unable to write XML" << corENDL;
      return;
   }

  // Part 2 - was writeXML()
   try {
      // get a writer instance
      const XMLCh ls_id [] = {chLatin_L, chLatin_S, chNull};
      DOMImplementation* impl(DOMImplementationRegistry::getDOMImplementation (ls_id));

#if XERCES_VERSION_MAJOR < 3
      DOMWriter         *theSerializer = ((DOMImplementationLS*)impl)->createDOMWriter();

      if (theSerializer->canSetFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true))
          theSerializer->setFeature(XMLUni::fgDOMWRTDiscardDefaultContent, true);

      if (theSerializer->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true))
          theSerializer->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      if (theSerializer->canSetFeature(XMLUni::fgDOMWRTBOM, true))
          theSerializer->setFeature(XMLUni::fgDOMWRTBOM, true);

      // set a target as the file argument
      std::string file(m_xFileName+".xml");
      m_log << coral::Debug << "About to write file " << file <<  coral::MessageStream::endmsg;
      XMLFormatTarget* myFormTarget = new LocalFileFormatTarget(file.c_str());

      // write document to target
      theSerializer->writeNode(myFormTarget, *newDocument);

      // clean up the mess
      if (theSerializer!=NULL && theSerializer!=0) delete theSerializer;
      if (myFormTarget!=NULL && myFormTarget!=0)   delete myFormTarget;
      if (newDocument!=NULL && newDocument!=0)     delete newDocument;
#else
      DOMLSSerializer *theSerializer = ((DOMImplementationLS*)impl)->createLSSerializer();

      if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true))
          theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTDiscardDefaultContent, true);

      if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true))
          theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);

      if (theSerializer->getDomConfig()->canSetParameter(XMLUni::fgDOMWRTBOM, true))
          theSerializer->getDomConfig()->setParameter(XMLUni::fgDOMWRTBOM, true);

      // set a target as the file argument
      std::string file(m_xFileName+".xml");
      m_log << coral::Debug << "About to write file " << file <<  coral::MessageStream::endmsg;
      XMLFormatTarget* myFormTarget = new LocalFileFormatTarget(file.c_str());

      // write document to target
      // See http://xerces.apache.org/xerces-c/program-dom-3.html
      DOMLSOutput* theOutput = ((DOMImplementationLS*)impl)->createLSOutput();
      theOutput->setByteStream(myFormTarget);
      theSerializer->write(newDocument, theOutput);

      // clean up the mess
      delete theSerializer;
      delete theOutput;
      delete myFormTarget;
      delete newDocument;
#endif
   }
   catch (const SAXException& e) {
      char* s = XMLString::transcode(e.getMessage( ));
      m_log << coral::Error << "xml error: " << s << corENDL;
      XMLString::release (&s);
   } 
   catch (const DOMException& e) {
      char* s = XMLString::transcode(e.getMessage( ));
      m_log << coral::Error << "xml error: " << s << corENDL;
      XMLString::release (&s);
   }
   catch( const XMLException& e ) {
      m_log << coral::Warning << "Failed writing XML options file: " << toNative(e.getMessage()) << coral::MessageStream::endmsg;
   }
}

//------------------------------------------------------------------------------
//
// fillCmdLineArgsFromXML
//
//   Given a file name it will try to read it as xml
//   and return a vector<string> which can be used to replace the argv_v.
//   DTD is implicit in code, but not checked explicitly.
//
//------------------------------------------------------------------------------
std::vector<std::string> Args2Container::fillCmdLineArgsFromXML(std::string file)
{
   std::vector<std::string> args;
   args.push_back(m_name);

   try {
      XMLPlatformUtils::Initialize();
   }
   catch( const XMLException& ) {
      m_log << coral::Error << "fillCmdLineArgsFromXML: Error during initialization!" << corENDL;
      return args;
   }

   XercesDOMParser *parser = new XercesDOMParser;
   parser->setValidationScheme(XercesDOMParser::Val_Auto);
   parser->setDoNamespaces(false);
   //DOMTreeErrorReporter *errReporter = new DOMTreeErrorReporter();
   //parser->setErrorHandler(errReporter);

   bool errorsOccured = false;
   try
   {
       parser->parse(file.c_str());
   }
   catch (const OutOfMemoryException&)
   {
      m_log << coral::Error << "OutOfMemoryException" << corENDL;
      errorsOccured = true;
   }
   catch (const XMLException& e)
   {
      char* s = XMLString::transcode(e.getMessage( ));
      m_log << coral::Error << "An error occurred during parsing" << endl
	  << " Message: " << s << corENDL;
      XMLString::release (&s);
      errorsOccured = true;
   }

   catch (const DOMException& e)
   {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];

      m_log << coral::Error  << "\nDOM Error during parsing: '" << file << endl
	  << "DOMException code is:  " << e.code << corENDL;

      if( DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars) ) {
         char* s = XMLString::transcode(errText);
	 m_log << coral::Error << "Message is: " << s << corENDL;
         XMLString::release (&s);
      }

      errorsOccured = true;
   }

   catch (...)
   {
      m_log << coral::Error << "An error occurred during parsing\n " << corENDL;
      errorsOccured = true;
   }

   // if any errors just exit now 
   if (errorsOccured) return args;

   DOMDocument* doc = parser->getDocument();
   if( !doc ) {
      m_log << coral::Error << "XML parser error? File=" << file << corENDL;
      return args;
   }
   
   // Look for ToolInfo elements
   DOMNodeList* toolList = doc->getElementsByTagName(XMLString::transcode("ToolInfo"));
   // If there is not exactly one exit now
   if (toolList->getLength()!=1) {
      m_log << coral::Error << "Wrong number of ToolInfo records" << corENDL;
      return args;
   }
   // since there is only one, take the first item 
   DOMNode* toolInfo = toolList->item(0);
   if (toolInfo!=NULL) {
      DOMNamedNodeMap* toolAttrs=toolInfo->getAttributes();
      string toolID = toNative( toolAttrs->getNamedItem(XMLString::transcode("toolID"))->getNodeValue() );
      // check that the toolID matches the name, but don't make it fatal;
      if (toolID!=m_name) {
	 m_log << coral::Warning << "XML toolID does not match this utility" << endl
	     << "toolID= " << toolID << corENDL;
      }
      // Loop over any children of the toolInfo
      if(toolInfo->hasChildNodes()) {
         DOMNodeList* cliArgList = toolInfo->getChildNodes();
         for (unsigned int i=0; i<cliArgList->getLength(); ++i) {
            DOMNode* thisArg = cliArgList->item(i);
            // Only process children that are CliArg's
            if( toNative(thisArg->getNodeName()) == "CliArg" ) {
               DOMNamedNodeMap* attrs = thisArg->getAttributes();
               // Oy Vay, can we make assigning a string any more complicated
               string cla2Name = toNative( attrs->getNamedItem(XMLString::transcode("name"))->getNodeValue() );
               bool exists=false;
               std::vector<CmdLineArgs2*>::const_iterator thisit = this->begin();
               while (thisit != this->end()) {
                  if ((*thisit)->name()==cla2Name) {exists=true; break;}
                  ++thisit;
               }
               if( exists ) {
		  string option = toNative( attrs->getNamedItem(XMLString::transcode("option"))->getNodeValue() );
                  args.push_back(option);
                  // Check whether the node has any text associated with it
                  // Treat the first node as that text -- should be true
                  if( thisArg->hasChildNodes() ) {
                     DOMNodeList* data = thisArg->getChildNodes(); 
 		     if( data->getLength() == 1 && data->item(0)->getNodeValue() != 0 ) {
			// Old format with single text value for a cliarg
			string optionText = toNative( thisArg->getFirstChild()->getNodeValue() );
			//std::cout << "Found text node with value " << optionText << std::endl; 
			//break it apart if it is a multiarg, otherwise take as in quotes text 
			if ( (*thisit)->getArgQual(option).multiarg==false ) { 
			   args.push_back(optionText); 
			} 
			else { 
			   std::vector<std::string> tokens; 
			   Tokenize(optionText,tokens); 
			   for (std::vector<std::string>::iterator tit=tokens.begin(); 
				tit != tokens.end(); 
				++tit) 
			      args.push_back(*tit); 
			}
		     }
                     else {
			// New format with multiple datum elements, each of which is an arg 
			for (unsigned int i=0; i<data->getLength(); ++i) {
			   if( toNative( data->item(i)->getNodeName() ) == "datum" ) {
			      if( data->item(i)->hasChildNodes() ) {
				 args.push_back( toNative( data->item(i)->getFirstChild()->getNodeValue()) );
			      }
			      else 
				 m_log << coral::Error << "Found empty/bad datum" << corENDL;
			   }
			}
		     }
                  }
               }
               else
		  m_log << coral::Warning << "Class " << cla2Name << " is not used for " << m_name << corENDL;
	    }
         }
      }
   }

   // trimming whitespaces from the front and the back of all values
   std::vector<string>::iterator arg = args.begin(), end = args.end();
   for( ; arg != end; ++arg ) {
      while( (*arg)[0] == ' ' || (*arg)[0] == '\n' || (*arg)[0] == '\t' )
	 arg->replace(0,1,"");
      while(true) {
	 char last = (*arg)[arg->size()-1];
	 if( last == ' ' || last == '\n' || last == '\t' )
	    arg->replace(arg->size()-1,1,"");
	 else
	    break;
      }
   }
   return args;
}

