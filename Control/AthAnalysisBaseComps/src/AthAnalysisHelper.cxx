/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthAnalysisBaseComps/AthAnalysisHelper.h"

#include "AthContainers/AuxTypeRegistry.h"

ServiceHandle<IJobOptionsSvc> AthAnalysisHelper::joSvc = ServiceHandle<IJobOptionsSvc>("JobOptionsSvc","AthAnalysisHelper");

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
      propMgr->setProperty("JobOptionsPath",options); 
    } else {
      propMgr->setProperty( "JobOptionsType", "NONE" ); //no joboptions given
    }
    propMgr->setProperty("EventLoop","MinimalEventLoopMgr"); //using this instead of the default EventLoopMgr means some services (e.g. EventSelector) are not created, which is good! :-)

    //configure and return
    theApp->configure(); 
    propMgr->setProperty("OutputLevel","3"); //INFO
    theApp->initialize();
    return theApp;
}

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
