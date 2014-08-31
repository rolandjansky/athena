/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FadsXMLParser/XMLReader.h"
#include "FadsXMLParser/XMLReaderUtil.h"
#include "FadsXMLParser/DOMTreeErrorReporter.h"

#include "FadsXMLParser/AddMaterialFactory.h"
#include "FadsXMLParser/CompositeFactory.h"
#include "FadsXMLParser/ElementFactory.h"

#include "xercesc/dom/DOMNodeList.hpp"

#include <fstream>

namespace FADS {

  factoryMap XMLReader::theMap;

  XMLReader* XMLReader::thePointer=0;

  XMLReader* XMLReader::GetXMLReader()
  {
    if (!thePointer) {
      thePointer=new XMLReader;
      //DescriptionFactory* factory;
      (void) new AddMaterialFactory("addmaterial");
      (void) new CompositeFactory("composite");
      (void) new ElementFactory("element");
    }
    return thePointer;
  }

  XMLReader::XMLReader()
  {
    static int nentries=0;
    if (!nentries)
      {
        try
          {
            XMLPlatformUtils::Initialize();
          }
        catch (const XMLException& toCatch)
          {
            std::cout << "Error during initialization! Message:\n"
                      << XMLReaderUtil::Transcode(toCatch.getMessage()) << std::endl;
            return;
          }
      }
  }


  XMLReader::~XMLReader()
  {
  }

  DescriptionFactory* XMLReader::GetFactory(std::string n)
  {
    if (theMap.find(n)!=theMap.end()) return theMap[n];
    return 0;
  }

  void XMLReader::AddFactory(DescriptionFactory *f)
  {
    // static factoryMap theMap;
    theMap[f->GetName()]=f;
  }

  // ostream& operator<< (ostream& target, const DOMString& s)
  // {
  //     char *p = s.transcode();
  //     std::string ss=p;
  //     target << ss;
  //     delete [] p;
  //     return target;
  // }

  // std::string& XMLReader::Transcode(const XMLString& t)
  // {
  //    static std::string s="";
  //    char *p = t.transcode();
  //    s=p;
  //    delete [] p;
  //    return s;
  // }

  void XMLReader::ProcessNode(const DOMNode& s)
  {
    const XMLCh *t=s.getNodeName();
    std::string n=XMLReaderUtil::Transcode(t);
    std::string hash=n.substr(0,1);
    DescriptionFactory *f=0;
    if (hash!="#") f=XMLReader::GetXMLReader()->GetFactory(n);

    if (f) f->Build(s);   // Preprocessing: attributes retrieved here

    DOMNodeList *list=s.getChildNodes();
    for (unsigned int i=0;i<list->getLength();i++)
      ProcessNode(*list->item(i));

    if (f) f->TerminateNode(s); // Postprocessing (if any)

  }

  void XMLReader::Process(std::string filename)
  {
    // cout<<" filename "<<endl;
    if (filename.empty())
      {
        std::cout<<" XMLReader - no file specified!!!!"<<std::endl;
      }
    std::ifstream testFile(filename.c_str());
    if (!testFile)
      {
        std::cout<<"XMLReader error: file "<<filename<<" not found!"<<
          std::endl<<"\t nothing done, returning..."<<std::endl;
        return;
      }


    XercesDOMParser* parser = new XercesDOMParser;
    parser->setValidationScheme(XercesDOMParser::Val_Always);
    parser->setDoNamespaces(false);
    DOMTreeErrorReporter errorHandler;
    parser->setErrorHandler(&errorHandler);

    // XMLReaderHandlers handler;
    //        parser.setDocumentHandler(&handler);

    bool errorsOccured=false;
    try
      {
        parser->parse(filename.c_str());
      }
    catch (const XMLException& e)
      {
        std::cout << "\nError during parsing: '" << filename << "'\n"
                  << "Exception message is:  \n"
                  <<  XMLReaderUtil::Transcode(e.getMessage()) << std::endl;
        XMLPlatformUtils::Terminate();
        errorsOccured=true;
      }
    catch (const DOMException& e)
      {
        //const unsigned int maxChars = 2047;
        //XMLCh errText[maxChars + 1];

        std::cout << "\nDOM Error during parsing: '" << filename <<
          std::endl << "DOMException code is:  " << e.code << std::endl;

        //              if (DOMImplementation::loadDOMExceptionMsg(e.code, errText, maxChars))
        //                      std::cout << "Message is: " << StrX(errText) << std::endl;
        errorsOccured=true;
      }
    catch (...)
      {
        std::cout << "\nUnexpected exception during parsing: '" << filename << std::endl;
        //            XMLPlatformUtils::Terminate();
        errorsOccured=true;
      }

    if (errorsOccured)
      {
        std::cout << "\nErrors occured, no output available\n" << std::endl;
      }
    else
      {
        DOMNode *doc = parser->getDocument();
        ProcessNode(*doc);
        // Print out the stats that we collected and time taken.
      }

    parser->reset();
    delete parser;

  }

  // std::string XMLReader::trim (const std::string s)
  // //-------------------------------------------------------------
  // {
  //    static const std::string null_string;
  //    if (s.size () == 0) return (s);
  //    std::string temp = s;
  //    std::string::size_type i;
  //    i = temp.find_first_not_of (' ');
  //    if (i == std::string::npos) return (null_string);
  //     // There is at least 1 non blank character in s.
  //    if (i > 0)
  //            {
  //                    temp = temp.substr (i);
  //            }
  //    i = temp.find_last_not_of (' ');
  //    if (i < temp.size ())
  //            {
  //                    temp = temp.substr (0, i + 1);
  //            }
  //    return (temp);
  // }

} // end namespace
