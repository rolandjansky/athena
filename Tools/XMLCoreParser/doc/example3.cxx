/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "XMLCoreParser/XMLCoreParser.h" 
#include <iostream>
#include <xercesc/util/XMLString.hpp> 

class Printer : public XMLCoreFactory
{
public:

  void do_start (XMLCoreParser& /*parser*/, const XMLCoreNode& node)
      {
        const XMLCh* nodeName = ((const DOMNode&) node).getNodeName();

        char* temp = XMLString::transcode (nodeName);
        std::cout << "<" << temp;
        delete [] temp;
        
          // Output any attributes on this element
        DOMNamedNodeMap* attributes = ((const DOMNode&) node).getAttributes();
        int attrCount = attributes->getLength();
        for (int i = 0; i < attrCount; i++)
          {
            DOMNode* attribute = attributes->item(i);
            
            temp = XMLString::transcode (attribute->getNodeName());
            std::cout << " " << temp << "=\"";
            delete [] temp;
            
            temp = XMLString::transcode (attribute->getNodeValue());
            std::cout << temp << "\"";
            delete [] temp;
          }
        
          //
          //  Test for the presence of children, which includes both
          //  text content and nested elements.
          //
        DOMNode* child = ((const DOMNode&) node).getFirstChild();
        if (child != 0)
          {
              // There are children. Close start-tag, and output children.
              // No escapes are legal here
            
            std::cout << ">" << std::endl;
          }
      }

  void do_end (XMLCoreParser& /*parser*/, const XMLCoreNode& node)
      {
        const XMLCh* nodeName = ((const DOMNode&) node).getNodeName();

        char* temp = XMLString::transcode (nodeName);

          //
          //  Test for the presence of children, which includes both
          //  text content and nested elements.
          //
        DOMNode* child = ((const DOMNode&) node).getFirstChild();
        if (child != 0)
          {
              //
              // Done with children.  Output the end tag.
              //
            
            std::cout << "</" << temp << ">" << std::endl;
          }
        else
          {
            //
            //  There were no children. Output the short form close of
            //  the element start tag, making it an empty-element tag.
            //
            
            std::cout << "/>" << std::endl;
          }
        if (temp != 0) delete [] temp;
      }

  void do_comment (XMLCoreParser& /*parser*/, const std::string& comment)
      {
        std::cout << "<!--" << comment << "-->" << std::endl;
      }
};
 
int main (int argc, char* argv[]) 
{ 
  XMLCoreParser p;
  Printer printer;
 
  p.register_default_factory (&printer);

  if (argc > 1) 
    { 
      const char* file = argv[1]; 
 
      p.visit (file); 
    }

  return (0); 
} 
 
 
