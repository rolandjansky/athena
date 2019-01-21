/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
//      LArFEBTempTool.cxx
//
// Description: Tool to provide FEB TEMP DCS information.
//
// Environment:
//      Software developed for the ATLAS Detector at the CERN LHC
//
// Author List:
//      Kirill Skovpen <kskovpen@cern.ch>
//
//-----------------------------------------------------------------------

#include "LArFEBTempTool.h" 
#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

// constructor 
LArFEBTempTool::LArFEBTempTool(const std::string& type,
			       const std::string& name,
			       const IInterface* parent)
  : AthAlgTool(type,name,parent), m_foldername("/LAR/DCS/FEBTEMP"),
    m_atrlistcol(nullptr),
    m_isinit(false)
{
   declareInterface< ILArFEBTempTool >( this );
   declareProperty("FolderName",m_foldername);   
}

// destructor 
LArFEBTempTool::~LArFEBTempTool()
{ }

// intialize 
StatusCode LArFEBTempTool::initialize()
{
  m_isinit=false;
  return StatusCode::SUCCESS;
}

FEBTemp LArFEBTempTool::getFebTemp( const HWIdentifier& id ) 
{
   FEBTemp temp;
   temp.clear();

   if(!m_isinit) {
     if ( detStore()->retrieve(m_atrlistcol,m_foldername).isFailure() ) {
       ATH_MSG_ERROR ( "Unable to retrieve AttrListCollection "<<m_foldername);
       return temp;
     }
     ATH_MSG_DEBUG ( "Successfully retrieved AttrListCollection" );
     m_isinit = true;
   }

   
   for (CondAttrListCollection::const_iterator citr=m_atrlistcol->begin();
	citr!=m_atrlistcol->end();++citr)
     {
        if( (*citr).first == id.get_identifier32().get_compact() )
	  {
	     float temp1=-1;
             if(! ((*citr).second)["temp1"].isNull()) temp1 = (((*citr).second)["temp1"]).data<float>();
	     float temp2 =-1;
             if(! ((*citr).second)["temp2"].isNull()) temp2 = (((*citr).second)["temp2"]).data<float>();
	     temp.push_back(std::make_pair(temp1,temp2));
	  }
     }

   return temp;
}


