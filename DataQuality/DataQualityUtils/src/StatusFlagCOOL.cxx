/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: StatusFlagCOOL.cxx,v 1.6 2009-02-13 12:32:11 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/StatusFlagCOOL.h"

//CORAL API include files
#include "CoralBase/Attribute.h"

//COOL API include files (CoolKernel)
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/StorageType.h"

ClassImp(dqutils::StatusFlagCOOL)

namespace dqutils{

StatusFlagCOOL::
StatusFlagCOOL (std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU)
  : StatusFlagCOOLBase(dbStr, folderStr, runS, lumiS, runU, lumiU)
{
}

StatusFlagCOOL::
StatusFlagCOOL (int runS, int lumiS, int runU, int lumiU)
  : StatusFlagCOOLBase(runS, lumiS, runU, lumiU) 
{
}

StatusFlagCOOL::
StatusFlagCOOL()
  : StatusFlagCOOLBase() 
{
}

cool::RecordSpecification 
StatusFlagCOOL::
createSpec() {
  //std::cout << "Preparing RecordSpecification" << std::endl;
    cool::RecordSpecification spec;
    spec.extend("Code",cool::StorageType::Int32);
    spec.extend("deadFrac",cool::StorageType::Float);
    spec.extend("Thrust",cool::StorageType::Float);
    if (!(spec==coolFolder->payloadSpecification())) {
        std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
    }
    return spec;
}

coral::AttributeList 
StatusFlagCOOL::
createPayload(int colourCode, float dfrac, float thrust, const cool::RecordSpecification& spec) {
    coral::AttributeList payload = cool::Record( spec ).attributeList();
    payload["Code"].data<cool::Int32>() = colourCode;
    payload["deadFrac"].data<cool::Float>() = dfrac;
    payload["Thrust"].data<cool::Float>() = thrust;
    //std::cout << "Creating payload: ";
    //std::cout << "[Code : " << colourCode << "],";
    //std::cout << "[DeadFraction : " << dfrac << "],";
    //std::cout << "[Thrust : " << thrust << "]" << std::endl;
    return payload;
}

void 
StatusFlagCOOL::
insert(cool::ChannelId channelId, int code, float dfrac, float thrust, std::string tag_name) {
    try {
        cool::RecordSpecification spec = this->createSpec();
        coral::AttributeList payload = this->createPayload(code, dfrac, thrust, spec);
        //std::cout << "Trying to store payload [channel " << this->getCoolFolder()->channelName(channelId)  <<" ("<< channelId  <<")]...";
	insert_helper(channelId, payload, tag_name);
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
}

void 
StatusFlagCOOL::
insert(std::string channelName, int code, float dfrac, float thrust, std::string tag_name) {
  try {
    this->insert(this->getCoolFolder()->channelId(channelName), code, dfrac, thrust, tag_name);
  }
  catch (cool::Exception& e) {
    std::cout << "Unknown exception caught!" << e.what() << std::endl;
  }
}

} //namespace dqutils
