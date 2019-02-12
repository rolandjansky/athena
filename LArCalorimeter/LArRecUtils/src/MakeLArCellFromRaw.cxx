/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// implementation of MakeLArCellFromRaw
//                             H.Ma              Dec 2002
//                 Updated to use DataPool   HMA  Jun 2003 

#include "LArRecUtils/MakeLArCellFromRaw.h"

#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArEM_ID.h"
#include "CaloIdentifier/LArHEC_ID.h"
#include "CaloIdentifier/LArFCAL_ID.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "LArRawUtils/LArRoI_Map.h" 
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "StoreGate/StoreGateSvc.h"


#include "CLHEP/Units/SystemOfUnits.h"
#include <iostream> 

using CLHEP::GeV;

MakeLArCellFromRaw::MakeLArCellFromRaw()
  : m_dataPool(0),
    m_msgSvc (0),
    m_cablingSvc (0),
    m_onlineID (0),
    m_ethreshold(-1.e5) 
{
}


MakeLArCellFromRaw::~MakeLArCellFromRaw()
{
  delete m_dataPool;
 return ; 
}

void MakeLArCellFromRaw::initialize( const LArRoI_Map* roiMap ,
	const std::vector<CaloCellCorrection*>* pCorr, unsigned int poolMaxSize )
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  ISvcLocator* svcLoc = Gaudi::svcLocator( );

  StatusCode sc = svcLoc->service("MessageSvc", m_msgSvc);
  if (sc.isFailure())
  {
     std::cout << "MakeLArCellFromRaw ERROR cannot retrieve MessageSvc " << std::endl;
    return;
  }

  MsgStream log(m_msgSvc, "MakeLArCellFromRaw");

  log << MSG::INFO <<" initializing MakeLArCellFromRaw"<< endmsg ;

  if(pCorr) log <<MSG::INFO <<" Number of Corrections "<< pCorr->size()<< endmsg ;
  

  const CaloDetDescrManager* man = CaloDetDescrManager::instance();

  IToolSvc* p_toolSvc;
  StatusCode status = svcLoc->service( "ToolSvc",p_toolSvc );
  if(status.isFailure())
  {
    log <<MSG::ERROR << "cannot find ToolSvc in MakeLArCellFromRaw " << endmsg;
    return;
  }
  sc = p_toolSvc->retrieveTool("LArCablingLegacyService",m_cablingSvc);
  if (sc.isFailure())
  {
    log <<MSG::INFO<< "cannot find LArCablingLegacyService in MakeLArCellFromRaw " << endmsg;
    return;
  }


  if (m_dataPool) {
    delete m_dataPool;
    m_dataPool = nullptr;
  }

  // create DataPool if requested
  if (poolMaxSize!=0) {
    m_dataPool = new DataPool<LArCell>();
    m_dataPool->reserve(poolMaxSize);
    log << MSG::INFO << "MakeLArCellFromRaw Creating DataPool<LArCell> of size " << poolMaxSize << endmsg ;
  }else{
    log << MSG::INFO << "MakeLArCellFromRaw do not use DataPool" << endmsg ;
  }


  const CaloIdManager* caloCIM = man->getCalo_Mgr();
  const LArEM_ID& em_id = *( caloCIM->getEM_ID() );
  const LArHEC_ID& hec_id = *( caloCIM->getHEC_ID() );
  const LArFCAL_ID& fcal_id = *(caloCIM->getFCAL_ID() );

// retrive onlineID helper from detStore

   StoreGateSvc* detStore;
   if (svcLoc->service("DetectorStore", detStore).isFailure()) {
     log << MSG::ERROR   << "Unable to access DetectoreStoren MakeLArCellFromRaw" << endmsg ;
     return;
   }

// retrieve OnlineID helper from detStore
   sc = detStore->retrieve(m_onlineID, "LArOnlineID");
   if (sc.isFailure()) {
     log << MSG::ERROR <<  "MakeLArCellFromRaw: Could not get LArOnlineID helper !" << endmsg ;
      return;
   }


  int n_em   = 0 ; 
  int n_hec  = 0 ; 
  int n_fcal = 0 ; 

  int n_em_err   = 0 ; 
  int n_hec_err  = 0 ; 
  int n_fcal_err = 0 ; 


  const std::vector<Identifier>& emIds   = em_id.channel_ids();
  const std::vector<Identifier>& hecIds  = hec_id.channel_ids();
  const std::vector<Identifier>& fcalIds = fcal_id.channel_ids();

  double en=50. * GeV;
  double time=0;
  double qu=0;

  CellInfo info0; 
  info0.eta=0; 
  info0.phi=0; 
  info0.x=0; 
  info0.y=0; 
  info0.z=0; 
  info0.fcal=false; 
  info0.tt=0; 
  info0.eCorr=1.; 
  info0.elem = 0 ; 

  // Make sure that the OnOff map has been set up.
  if (!m_cablingSvc->checkOnOff().isSuccess()) {
    MsgStream log(m_msgSvc, "MakeLArCellFromRaw");
    log << MSG::ERROR << "Accessing OnOff map" << endmsg;
  }


  // EM
  std::vector<Identifier>::const_iterator it = emIds.begin();
  std::vector<Identifier>::const_iterator it2 = emIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;

    try{ 
     
      HWIdentifier sigId 
              =m_cablingSvc->createSignalChannelID(chan_id);
// GU 
      HWIdentifier feb   = m_onlineID->feb_Id(sigId);
      CELL_VEC::size_type chan  = m_onlineID->channel(sigId);   
      CELL_VEC& cellVec= m_cellMap[ feb.get_identifier32().get_compact() ] ; 
      if (cellVec.size()==0)cellVec.resize(128,info0); 

      CellInfo& cell = cellVec[chan] ;

      if(roiMap) cell.tt  = roiMap->TrigTowerID(sigId); 

      const CaloDetDescrElement* caloDDE =man->get_element( chan_id);

      cell.id  =  chan_id ; 
      cell.eta =  caloDDE->eta();
      cell.phi =  caloDDE->phi(); 
      cell.fcal= false; 
      cell.elem =caloDDE; 
// to compute correction, assume that cell is in mid gain 
      const CaloGain::CaloGain g=CaloGain::LARMEDIUMGAIN;
      
      ++n_em; 
      if(pCorr)  {
	// make a fake LArCell, and get the correction factor. 
	   LArCell larCell(caloDDE,en,time,qu,g); 
           cell.eCorr   = getCorrection(&larCell, *pCorr, ctx);
      }
    } catch (LArID_Exception& ex){
                    ++n_em_err; 
    }
  } // end of EM 



  // HEC
  it = hecIds.begin();
  it2 = hecIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;

    try{ 
     
      HWIdentifier sigId 
              =m_cablingSvc->createSignalChannelID(chan_id);
// to change using LArOnline methods
      HWIdentifier feb   = m_onlineID->feb_Id(sigId);
      CELL_VEC::size_type chan  = m_onlineID->channel(sigId);  
      CELL_VEC& cellVec= m_cellMap[ feb.get_identifier32().get_compact() ] ; 
      if (cellVec.size()==0)cellVec.resize(128,info0); 

      CellInfo& cell = cellVec[chan] ;

      if(roiMap) cell.tt  = roiMap->TrigTowerID(sigId); 

      const CaloDetDescrElement* caloDDE =man->get_element( chan_id);

      cell.id  =  chan_id ; 
      cell.eta =  caloDDE->eta();
      cell.phi = caloDDE->phi(); 
      cell.fcal= false; 
      cell.elem =caloDDE; 
      const CaloGain::CaloGain g=CaloGain::LARMEDIUMGAIN ;

      if(pCorr)  {
	// make a fake LArCell, and get the correction factor. 
	   LArCell larCell(caloDDE,en,time,qu,g); 
           cell.eCorr   = getCorrection(&larCell, *pCorr, ctx);
      }
      ++n_hec; 
    } catch (LArID_Exception& ex){
                    ++n_hec_err; 
    }
  } // end of HEC

  // FCAL
  it = fcalIds.begin();
  it2 = fcalIds.end();
  for(; it!=it2;++it)
  {
    const Identifier& chan_id = *it ;

    try{ 
     
      HWIdentifier sigId 
              =m_cablingSvc->createSignalChannelID(chan_id);
// GU to change to use LArOnlineID method
      HWIdentifier feb   = m_onlineID->feb_Id(sigId);
      CELL_VEC::size_type chan  = m_onlineID->channel(sigId);  
      CELL_VEC& cellVec= m_cellMap[ feb.get_identifier32().get_compact() ] ; 
      if (cellVec.size()==0)cellVec.resize(128,info0); 

      CellInfo& cell = cellVec[chan] ;

      if(roiMap) cell.tt  = roiMap->TrigTowerID(sigId); 

      const CaloDetDescrElement* caloDDE =man->get_element( chan_id);

      cell.id  =  chan_id ; 
      cell.eta =  caloDDE->eta();
      cell.phi = caloDDE->phi(); 
      cell.fcal= false; 
      cell.elem =caloDDE; 
      const CaloGain::CaloGain g=CaloGain::LARMEDIUMGAIN ;

      if(pCorr)  {
	// make a fake LArCell, and get the correction factor. 
	   LArCell larCell(caloDDE,en,time,qu,g); 
           cell.eCorr   = getCorrection(&larCell, *pCorr, ctx);
      }
      ++n_fcal; 
    } catch (LArID_Exception& ex){
                    ++n_fcal_err; 
    }
  } // end of FCAL


  log <<MSG::INFO <<" number of em, hec, fcal cells="<<n_em<<" "<<n_hec<<" "<<n_fcal<<endmsg;
  log <<MSG::INFO<<" number of exceptions for em,hec,fcal="<<n_em_err<<" "<<n_hec_err<<" "<<n_fcal_err<< endmsg;
  log <<MSG::INFO<<" done with initializing MakeLArCellFromRaw"<< endmsg;
  return; 
}

LArCell* MakeLArCellFromRaw::getLArCell(const HWIdentifier& id, int e, int t, int q ) const
{
  return getLArCell(id, e, t, q, CaloGain::UNKNOWNGAIN);
  
}


// This method is use for converting LArRawChannel to LArCell 
LArCell* MakeLArCellFromRaw::getLArCell(const HWIdentifier& id, int e, int t, int q,  CaloGain::CaloGain g ) const
{
// GU to change

  HWIdentifier feb   = m_onlineID->feb_Id(id);
  unsigned int chan  = m_onlineID->channel(id); 
  unsigned int tt; // not needed for this method. 
//  unsigned int ifeb = m_onlineID->feb_Hash(feb);
  return getLArCell(feb.get_identifier32().get_compact() ,  chan , e,  t,  q, g, tt) ; 
} 


LArCell* MakeLArCellFromRaw::getLArCell(unsigned int feb, unsigned int chan ,
	int e, int t, int q, unsigned int& ttId ) const 
{
  return getLArCell(feb, chan ,e, t, q,  CaloGain::UNKNOWNGAIN, ttId );
  
}

// This method is used for converting directly from BS to LArCell. 
LArCell* MakeLArCellFromRaw::getLArCell(unsigned int feb, unsigned int chan ,
	int e, int t, int q, CaloGain::CaloGain g, unsigned int& ttId ) const 
{

  CELL_MAP::iterator it =m_cellMap.find( feb ); 

  if(it == m_cellMap.end()){
	MsgStream log(m_msgSvc, "MakeLArCellFromRaw");
	log << MSG::ERROR <<" MakeLArCellFromRaw ERROR, failed to find existing cells. "<< endmsg; 
	std::cout <<std::hex<<"FEBID = "<< feb <<std::dec<< std::endl ; 
	//DR	assert(0); 
	return 0;
  }

  CELL_VEC& cellVec = (*it).second; 
  if(cellVec.size()<= chan){
	MsgStream log(m_msgSvc, "MakeLArCellFromRaw");
	log << MSG::FATAL <<" MakeLArCellFromRaw ERROR, channel number= "<<chan<< endmsg ;
	assert(0);
	return 0;
  }
  
  CellInfo& info = cellVec[chan] ; 
  // do not create cell if it is not connected. 
  if(info.elem == 0) return 0; 

  //remove /1000 (GeV->MeV)
  double de = e * info.eCorr; 
  LArCell* cell =0;

  // DR convert time from ps (in LArRawChannel ) to ns
  double time = t/1000.0;

  if (m_dataPool!=0) {
    cell = new ( (*m_dataPool)()) LArCell(info.elem,de,time,q,g); 
  }
  else
    {
    cell = new LArCell(info.elem,de,time,q,g); 
    }

  ttId = info.tt; 

  return cell; 

}

double
MakeLArCellFromRaw::getCorrection(LArCell* cell, 
                                  const std::vector<CaloCellCorrection*>& vCorr,
                                  const EventContext& ctx) const
{
// LArCell was made with energy = 50. 
 	double en= 50. * GeV ; 
//	cell->setEnergy(en); 

        // apply corrections. 
        for (const CaloCellCorrection* corr : vCorr)
         {
           corr->MakeCorrection (cell, ctx);
         } 

	double c=  cell->energy()/en; 
// 	std::cout<<" Correction init = "<<c<<std::endl;
	return c;        
}



void MakeLArCellFromRaw::setThreshold(float t)
{
  m_ethreshold = t; 
  return; 
}

