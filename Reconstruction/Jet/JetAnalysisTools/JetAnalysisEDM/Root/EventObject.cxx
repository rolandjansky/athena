/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../JetAnalysisEDM/EventObject.h"


namespace JetAnalysisEDM {
  
  EventObject::EventObject( const ::Long64_t& master, TTree *tree ):
    m_varMap(NULL,"")
  {

    EventNumber.setName("EventNumber");
    RunNumber.setName( "RunNumber" );
    bcid.setName("bcid") ;
    
    EventNumber.setMaster(&master);
    RunNumber.setMaster(&master);
    bcid.setMaster(&master);
    m_varMap.setParameters("",&master);
    if(tree) {
      readFrom(tree);
    }
  }

  void EventObject::readFrom(TTree *tree){
    m_varMap.readFrom(tree);
    EventNumber.readFrom(tree);
    RunNumber.readFrom(tree);
    bcid.readFrom(tree);
  }

  void EventObject::writeTo(TTree* tree){ 
    m_varMap.writeTo(tree) ;
    EventNumber.writeTo(tree);
    RunNumber.writeTo(tree);
    bcid.writeTo(tree);
  }

  void EventObject::readAllActive() const { 
    EventNumber(); RunNumber(); bcid();
    m_varMap.readActive();
  }

  float EventObject::getF(const string_t & mName){return get<float>(mName);}
  float EventObject::getI(const string_t & mName){return get<int>(mName);}
  const std::vector<float>& EventObject::getVecF(const string_t & mName){return get<std::vector<float>&>(mName);}
  const std::vector<int>&   EventObject::getVecI(const string_t & mName){return get<std::vector<int>&>(mName);}

}
