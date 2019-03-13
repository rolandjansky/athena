/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/********************************************************************

NAME:     LArCellFakeProbHElectronics.cxx
PACKAGE:  offline/LArCalorimeter/LArRecUtils

AUTHORS:  Kai Voss <kai.voss@cern.ch>
CREATED:  15 February 2005

PURPOSE:  Scales down the energy of cells due to simulated 
          failure of the readout

********************************************************************/
#include "LArCellFakeProbElectronics.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArIdentifier/LArOnlineID.h"


#include "CLHEP/Units/SystemOfUnits.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellFakeProbElectronics::LArCellFakeProbElectronics(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_cablingService(nullptr),
    m_onlineHelper(nullptr)
{
  declareInterface<ICellWeightTool>(this);
  declareProperty("Dead_FEC_FEB_CHAN",   m_inputStringIDs);
}




/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellFakeProbElectronics::initialize()
{
  ATH_CHECK(  detStore()->retrieve( m_onlineHelper, "LArOnlineID") );

  IToolSvc* toolSvc = nullptr;
  ATH_CHECK(  service( "ToolSvc",toolSvc  ) );
  ATH_CHECK(  toolSvc->retrieveTool("LArCablingLegacyService",m_cablingService) );

  // convert string identifiers m_inputStringIDs into identifiers m_inputIDs
  ATH_CHECK( read_problems() );
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////
// PROCESS:
// 
/////////////////////////////////////////////////////////////////////


double  LArCellFakeProbElectronics::wtCell(const CaloCell * theCell ) const
{
  double weight=1; // default weight



  // get calo id helper
  const CaloCell_ID* idHelper = nullptr;
  if (detStore()->retrieve (idHelper, "CaloCell_ID").isFailure()) {
    return 1;
  }
  
  HWIdentifier id;
  try{
    id = m_cablingService->createSignalChannelID(theCell->ID());
  }catch(LArID_Exception & except)
    {
      return 1.;
    }
  //int FT = m_onlineHelper->feedthrough(id);
  //int chan = m_onlineHelper->channel(id);
  //int feb = m_onlineHelper->slot(id);
  
  // find channel in map of dead channels
  std::map<HWIdentifier,double>::const_iterator cur  =   m_deadChannels.find(id);
  if (cur != m_deadChannels.end()){
    weight= cur->second;
  }



  return weight;
}




StatusCode LArCellFakeProbElectronics::read_problems()
/*-------------------------------------------------------*/
{
  // convect vector of string identifier into vector of identifier

  std::vector<std::string>::const_iterator itrStringID=m_inputStringIDs.begin();
  // decode property here
  for (;itrStringID!=m_inputStringIDs.end();++itrStringID) {
    std::string theString=*itrStringID;
    std::stringstream is;
    is << theString << std::endl;
    

    int iBarrel,iSide,iFT,iSlot,iChannel;
    double weight;
    is >>iBarrel >> iSide >> iFT >> iSlot >> iChannel >>weight;

    // FT dead -> Kill all Channels 
    if (iSlot == 999) {
      int maxSlot = 14;
      if (iBarrel == 1) maxSlot=13;
      
      for (int slot=0; slot<=maxSlot;slot++){
	for (int chan=0; chan<=127;chan++){
	  add_cell(iBarrel,iSide,iFT,slot,chan,weight);
	}
      }
      
    }else if(iChannel == 999){ // slot dead -> Kill all Channels 
      for (int chan=0; chan<=127;chan++){
	add_cell(iBarrel,iSide,iFT,iSlot,chan,weight);
      }
      
    }else{
      
      add_cell(iBarrel,iSide,iFT,iSlot,iChannel,weight);
    }
    
    

  }

  std::map<HWIdentifier,double>::const_iterator itr2Chan=m_deadChannels.begin();
  for (; itr2Chan != m_deadChannels.end();++itr2Chan) {
    ATH_MSG_VERBOSE( "Selected online channel: " << itr2Chan->first << "   weight: " << itr2Chan->second  );

  }


  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////
// add cell to  list of dead cells //
/////////////////////////////////////
StatusCode LArCellFakeProbElectronics::add_cell(int iBarrel,int iSide,int iFT,int iSlot,int iChannel,double weight)
{
  try {
    HWIdentifier l_channelId = m_onlineHelper->channel_Id (iBarrel,
							   iSide,
							   iFT,
							   iSlot,
							   iChannel );   
   m_deadChannels[l_channelId]=weight;
  }
  catch(LArOnlID_Exception & except){
    
    
    ATH_MSG_ERROR(  " LArOnlId exception creating l_channelId " 
                    << (std::string)except
                    << " barrel_ec, side, feedthrough, slot, channel= " << iBarrel << " " 
                    << iSide << " " 
                    << iFT << " " 
                    << iSlot << " "
                    << iChannel 
                    );
  }
  
  return StatusCode::SUCCESS;
}
