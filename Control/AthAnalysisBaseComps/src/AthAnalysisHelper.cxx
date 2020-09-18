/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "AthContainers/AuxTypeRegistry.h"

const std::string AthAnalysisHelper::UNDEFINED = "__UNDEFINED__";

ServiceHandle<Gaudi::Interfaces::IOptionsSvc> AthAnalysisHelper::joSvc = ServiceHandle<Gaudi::Interfaces::IOptionsSvc>("JobOptionsSvc","AthAnalysisHelper");

//need a constructor, implemented here, so that the dictionary library is linked to 
//the implementation library (see ldd libAthAnalysisBaseCompsDict.so ... needs a link)
AthAnalysisHelper::AthAnalysisHelper() { } 


IAppMgrUI* AthAnalysisHelper::initGaudi(const char* options) {
    IAppMgrUI* theApp = Gaudi::createApplicationMgr();
    //check if already configured 
    if(theApp->FSMState() != Gaudi::StateMachine::OFFLINE) return theApp;
    //set the joboptions 
    SmartIF<IProperty> propMgr(theApp);
    if(strlen(options)) {
      if (propMgr->setProperty("JobOptionsPath",options).isFailure()) return nullptr;
    } else {
      //no joboptions given
      if (propMgr->setProperty( "JobOptionsType", "NONE" ).isFailure()) return nullptr;
    }
    //using this instead of the default EventLoopMgr means some services (e.g. EventSelector) are not created, which is good! :-)
    if (propMgr->setProperty("EventLoop","MinimalEventLoopMgr").isFailure()) return nullptr;

    //configure and return
    if (theApp->configure().isFailure()) return nullptr;
    propMgr->setProperty("OutputLevel","3").ignore(); //INFO
    if (theApp->initialize().isFailure()) return nullptr;
    return theApp;
}

bool AthAnalysisHelper::toolExists( const std::string& fullName ) {
  ServiceHandle<IToolSvc> toolSvc("ToolSvc","AthAnalysisHelper");
  if(toolSvc.retrieve().isFailure()) return false;
  auto existingTools = toolSvc->getInstances();
  for(auto& toolName : existingTools) {
    if(fullName==toolName) { toolSvc.release().ignore(); return true; }
  }
  toolSvc.release().ignore();
  return false;
}

void AthAnalysisHelper::dumpJobOptionProperties(const std::string& client) {
   ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AthAnalysisHelper");
   if(joSvc.retrieve().isFailure()) return;
   for(const auto& [name,value] : joSvc->items()) {
     if(name.find(client)!=0) continue; //must start with client
     std::cout << name << " = " << value << std::endl;
   }
}

std::string AthAnalysisHelper::getProperty(const std::string& client, const std::string& property) {
   ServiceHandle<Gaudi::Interfaces::IOptionsSvc> joSvc("JobOptionsSvc","AthAnalysisHelper");
   if(joSvc.retrieve().isFailure()) return UNDEFINED;
   return joSvc->get(client+"."+property, UNDEFINED);
}

void AthAnalysisHelper::printAuxElement(const SG::AuxElement& ae) {
   const SG::auxid_set_t& auxids = ae.getAuxIDs();
   SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
   for( SG::auxid_t aux : auxids) {
      std::string name = reg.getName( aux );
      std::cout << "    " << name << " = ";
      const std::type_info& typeinfo = *reg.getType(aux);
#define PRINT_AE( TYPE ) \
if(typeinfo==typeid(TYPE) && ae.isAvailable<TYPE>(name)) std::cout << ae.auxdata<TYPE>(name) << " (" << #TYPE << ")" << std::endl;

      PRINT_AE( bool )
      else PRINT_AE(uint)
      else PRINT_AE(int)
      else PRINT_AE(float)
      else PRINT_AE(double)
      else PRINT_AE(unsigned char)
      else PRINT_AE(char)
      else PRINT_AE(short)
      else PRINT_AE(unsigned short)
      else PRINT_AE(unsigned long long)
      else std::cout << " (" << System::typeinfoName( typeinfo ) << ")" << std::endl;
      
#undef PRINT_AE
   }
}

void AthAnalysisHelper::dumpProperties(const IProperty& component) {
  for(auto p : component.getProperties()) {
    std::cout << p->name() << " = " << p->toString() << std::endl;
  }
}
