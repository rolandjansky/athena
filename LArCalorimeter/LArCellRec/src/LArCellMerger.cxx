/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     LArCellMerger
PACKAGE:  offline/Calorimeter/LArCellRec

AUTHORS: G.Unal
CREATION:  9 July 2009

PURPOSE:

********************************************************************/

#include "LArCellRec/LArCellMerger.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "LArRawEvent/LArRawChannelContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "LArCabling/LArCablingService.h"


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

LArCellMerger::LArCellMerger(
			     const std::string& type, 
			     const std::string& name, 
			     const IInterface* parent)
  :AthAlgTool(type, name, parent),
   m_rawChannelContainerName("LArRawChannels_digits"),
   m_calo_id(nullptr),
   m_evt(0),
   m_ncell(0)
{ 
  declareInterface<ICaloCellMakerTool>(this); 

  declareProperty("RawChannelsName",m_rawChannelContainerName,"Name of raw channel container");
}


/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
/////////////////////////////////////////////////////////////////////

StatusCode LArCellMerger::initialize()
{
  m_evt=0;
  m_ncell=0;

   // callback to GeoModel to retrieve identifier helpers, etc..
  const IGeoModelSvc *geoModel=0;
  StatusCode sc = service("GeoModelSvc", geoModel);
  if(sc.isFailure())
  {
    msg(MSG::ERROR) << "Could not locate GeoModelSvc" << endreq;
    return sc;
  }

  // dummy parameters for the callback:
  int dummyInt=0;
  std::list<std::string> dummyList;

  if (geoModel->geoInitialized())
  {
    return geoInit(dummyInt,dummyList);
  }
  else
  {
    sc = detStore()->regFcn(&IGeoModelSvc::geoInit,
			    geoModel,
			    &LArCellMerger::geoInit,this);
    if(sc.isFailure())
    {
      msg(MSG::ERROR) << "Could not register geoInit callback" << endreq;
      return sc;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode LArCellMerger::geoInit(IOVSVC_CALLBACK_ARGS)
{

  const  CaloIdManager* caloIdMgr;
  StatusCode sc = detStore()->retrieve( caloIdMgr );
  if (sc.isFailure()) {
   msg(MSG::ERROR) << "Unable to retrieve CaloIdMgr " << endreq;
   return sc;
  }
  m_calo_id = caloIdMgr->getCaloCell_ID();


  // translate offline ID into online ID
  sc = m_cablingService.retrieve();
  if(sc.isFailure()){
    msg(MSG::ERROR) << "Could not retrieve LArCablingService Tool" << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;

}

StatusCode LArCellMerger::finalize()
{
   msg(MSG::INFO) << "  ---- Summary from LArCellMerger " << endreq;
   msg(MSG::INFO) << "   Number of events processed                           " << m_evt << endreq;
   msg(MSG::INFO) << "   Number of cells from merged raw channel container    " << m_ncell << endreq;
   float ratio=0.;
   if (m_evt>0) ratio=((float)(m_ncell))/((float)(m_evt));
   msg(MSG::INFO) << "   Average number of cells per event                    " << ratio << endreq;

   return StatusCode::SUCCESS;
}

StatusCode LArCellMerger::process(CaloCellContainer * theCont )
{
	
  m_evt++;

  unsigned int nwarnings=0;

  ATH_MSG_DEBUG("in  LArCellMerger::process");
  
  const LArRawChannelContainer* rawColl;
  StatusCode sc = evtStore()->retrieve(rawColl,m_rawChannelContainerName);
  if (sc.isFailure()) {
    msg(MSG::WARNING) << " cannot retrieve raw channel container to merge " << m_rawChannelContainerName << "  Skip  LArCellMerger::process " << endreq;
    return StatusCode::SUCCESS;
  }

  // loop over raw channel container
  //   as this new container is supposed to contain only few cells, we do a simple loop and the basics onlineId to offlineId conversion
  //   this could be a little slow if by mistake this container contains all cells (not what this tool is supposed to be used for)

  LArRawChannelContainer::const_iterator itrRawChannel=rawColl->begin();
  LArRawChannelContainer::const_iterator lastRawChannel=rawColl->end();
  for ( ; itrRawChannel!=lastRawChannel; ++itrRawChannel) 
  {
      const LArRawChannel& theRawChannel = (*itrRawChannel);

      const HWIdentifier hwid=theRawChannel.channelID();
      if (m_cablingService->isOnlineConnected(hwid)) {
          Identifier id = m_cablingService->cnvToIdentifier( hwid);
          IdentifierHash theCellHashID = m_calo_id->calo_cell_hash(id);
          int index = theCont->findIndex(theCellHashID);
          if (index<0) {
               if (nwarnings<100) {
                 msg(MSG::WARNING) << " cell " << hwid.get_compact() << " " << id.get_compact() << " is not in the container " << endreq;
                  nwarnings++;
                  if (nwarnings==100) msg(MSG::WARNING) << "  will not print anymore warnings for this event... " << endreq;
               } 
               continue;
          }
          CaloCell* aCell = theCont->at(index);

          if (aCell) {
	    ATH_MSG_DEBUG(" replace energies in cell hwid= " << hwid.get_compact() << " offlineid = " << id.get_compact()
			  << " " << aCell->ID().get_compact() << " old energy " << aCell->e() << " new energy " << theRawChannel.energy());
	    aCell->setEnergy((float)(theRawChannel.energy()));
	    aCell->setTime((float)(theRawChannel.time())*0.001);    // convert from ps int in raw channel to ns float in calocell
	    aCell->setQuality(theRawChannel.quality());
	    aCell->setProvenance(theRawChannel.provenance());
	    m_ncell++;
          }

      }  // isConnected
  }       // loop over raw channel container


  return StatusCode::SUCCESS;
}
