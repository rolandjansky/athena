/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEventAthenaPool/JetTPCnvPropertiesTool.h"

JetTPCnvPropertiesTool::JetTPCnvPropertiesTool(const std::string& type ,const std::string& name ,const IInterface* parent) 
  : AthAlgTool( type, name, parent ) {
  declareInterface<IJetTPCnvPropertiesTool>(this);
  // declare properties here

  declareProperty("NoConstitPersistant" , s_write0constit_colls = std::vector<std::string> () );
}
 

StatusCode JetTPCnvPropertiesTool::initialize(){
  ATH_MSG_INFO( " JetTPCnvPropertiesTool::initialize " );
  ATH_MSG_INFO( " s_write0constit_colls size= " <<s_write0constit_colls.size()  );
  
  return StatusCode::SUCCESS;
} 
