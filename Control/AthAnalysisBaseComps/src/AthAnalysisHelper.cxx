/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "AthContainers/AuxTypeRegistry.h"

//need a constructor, implemented here, so that the dictionary library is linked to 
//the implementation library (see ldd libAthAnalysisBaseCompsDict.so ... needs a link)
AthAnalysisHelper::AthAnalysisHelper() { } 


void AthAnalysisHelper::dumpJobOptionProperties(const std::string& client) {
   ServiceHandle<IJobOptionsSvc> joSvc("JobOptionsSvc","AthAnalysisHelper");
   if(joSvc.retrieve().isFailure()) return;
   std::vector<std::string> clients = joSvc->getClients();
   for(auto& cl : clients) {
      if(cl.find(client)!=0) continue; //must start with client
      auto props = joSvc->getProperties(cl);
      if(!props) continue;
      for(auto prop : *props) {
         std::cout << cl << "." << prop->name() << " = " << prop->toString() << std::endl;
      }
   }
   joSvc.release().ignore();
}

void AthAnalysisHelper::printAuxElement(const SG::AuxElement& ae) {
   const SG::auxid_set_t& auxids = ae.getAuxIDs();
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
   for( auto& aux : auxids) {
      std::string name = reg.getName( aux );
      std::cout << "    " << name << " = ";
      const std::type_info& typeinfo = *reg.getType(aux);
      if(typeinfo==typeid(bool)) std::cout << ae.auxdata<bool>(name) << " (bool)" << std::endl;
      else if(typeinfo==typeid(uint)) std::cout << ae.auxdata<uint>(name) << " (unsigned int)" << std::endl;
         else if(typeinfo==typeid(float)) std::cout << ae.auxdata<float>(name) << " (float)" << std::endl;
         else if(typeinfo==typeid(int)) std::cout << ae.auxdata<int>(name) << " (int)" << std::endl;
         else if(typeinfo==typeid(double)) std::cout << ae.auxdata<double>(name) << " (double)" << std::endl;
         else if(typeinfo==typeid(unsigned char)) std::cout << int(ae.auxdata<unsigned char>(name)) << " (unsigned char)" << std::endl;  //always print as an int, that's what chars usually mean!
         else if(typeinfo==typeid(char)) std::cout << int(ae.auxdata<char>(name)) << " (char)" << std::endl;  //always print as an int, that's what chars usually mean!
         else if(typeinfo==typeid(short)) std::cout << ae.auxdata<short>(name) << " (short)" << std::endl;
         else if(typeinfo==typeid(unsigned short)) std::cout << ae.auxdata<unsigned short>(name) << " (unsigned short)" << std::endl;
      else std::cout << " (" << System::typeinfoName( typeinfo ) << ")" << std::endl;
   }
}