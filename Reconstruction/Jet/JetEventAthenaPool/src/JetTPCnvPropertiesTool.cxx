/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetEventAthenaPool/JetTPCnvPropertiesTool.h"
#include "GaudiKernel/MsgStream.h"

JetTPCnvPropertiesTool::JetTPCnvPropertiesTool(const std::string& type ,const std::string& name ,const IInterface* parent) 
  : AlgTool( type, name, parent ) {
  declareInterface<IJetTPCnvPropertiesTool>(this);
  // declare properties here

  declareProperty("NoConstitPersistant" , s_write0constit_colls = std::vector<std::string> () );
}
 

StatusCode JetTPCnvPropertiesTool::initialize(){
  MsgStream mLog( msgSvc(), "JetTPCnvPropertiesTool" );

  mLog << MSG::INFO << " JetTPCnvPropertiesTool::initialize "<<endreq;
  mLog << MSG::INFO << " s_write0constit_colls size= " <<s_write0constit_colls.size() <<endreq;
  
  return StatusCode::SUCCESS;
} 
