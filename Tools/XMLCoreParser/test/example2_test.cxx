/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "XMLCoreParser/XMLCoreParser.h" 
#include <iostream>

class Printer : public XMLCoreFactory
{
public:

  void do_start (XMLCoreParser& /*parser*/, const XMLCoreNode& node)
      {
        std::string node_name = get_name (node);

        std::cout << "Printer factory::do_start>" << node_name << std::endl;
        
          // Output any attributes on this element

        int count = attribute_number (node);
        for (int i = 0; i < count; i++)
          {
            std::string name = get_name (node, i);
            std::cout << "  " << name << "=\"";
            
            std::string value = get_value (node, name);
            std::cout << value << "\"" << std::endl;
          }
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

  p.register_external_entity ("InnerDetector", "OtherInDet.xml");
  p.register_external_entity ("LArCalorimeter", "NULL");
  p.register_external_entity ("TileCalorimeter", "NULL");
  p.register_external_entity ("LVL1Calorimeter", "NULL");
  p.register_external_entity ("MuonSpectrometer", "NULL");
  p.register_external_entity ("MuonSpectrometer", "");

  const char* dir = getenv("XMLCOREPARSER_DATA");
  if (!dir)
    dir = "../data";
  std::string file = dir;
  if (argc > 1)
    file = argv[1]; 
  else
    file += "/ATLAS_AGDD.xml";
 
  p.visit (file); 
  return (0); 
} 
 
