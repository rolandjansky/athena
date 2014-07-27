/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TrigTileMuToNtuple.cxx
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "StoreGate/StoreGateSvc.h"

//TileCalo include

#include "TileEvent/TileMuContainer.h"
#include "TrigTileMuId/TrigTileMuToNtuple.h"

//const int max_ntag=50;

// Constructor & deconstructor
TrigTileMuToNtuple::TrigTileMuToNtuple(std::string name, ISvcLocator* 
pSvcLocator)
   :Algorithm(name, pSvcLocator)
{
   m_TileMuContainer="TileMuObj";
   m_ntupleLoc="/FILE1/TileMuTag";
   m_ntupleID=100;
   max_ntag=50;
   m_close=1;
   declareProperty("TileMuTagsOutputName", m_TileMuContainer);    
   declareProperty("NTupleLoc", m_ntupleLoc);
   declareProperty("NTupleID", m_ntupleID);
   declareProperty("MaxNtag", max_ntag);
   declareProperty("CloseNtuple", m_close);
}

TrigTileMuToNtuple::~TrigTileMuToNtuple()
{
}

// Alg standard interface function
StatusCode TrigTileMuToNtuple::initialize() 
{ 
   MsgStream log(messageService(), name());
   
   StatusCode sc;
   sc=service("StoreGateSvc", m_storeGate);
   if(sc.isFailure()) {
      log<<MSG::ERROR<<"Unable to get pointer to StoreGate Service"<<endreq;
      return sc;
   }

       m_ntupleLoc="/NTUPLES/FILE1/TileMuTag";
   // + m_ntupleLoc;

   SmartDataPtr<NTuple::Directory> DirPtr(ntupleService(), m_ntupleLoc);
   if(!DirPtr) DirPtr=ntupleService()->createDirectory(m_ntupleLoc);
   if(!DirPtr) {
     log<<MSG::ERROR<<"Invalid Ntuple Directory: "<< m_ntupleLoc <<endreq;
      return StatusCode::FAILURE;
   }

   m_ntuplePtr=ntupleService()->book(DirPtr.ptr(), m_ntupleID, 
                                     CLID_ColumnWiseTuple, "TileMuTag-Ntuple");
   if(!m_ntuplePtr) {
      log<<MSG::ERROR<<"Failed to book ntuple: TileMuTagNtuple"<<endreq;
      return StatusCode::FAILURE;
   }

   sc=m_ntuplePtr->addItem("TileMu/ntag",m_ntag,0,max_ntag);
   sc=m_ntuplePtr->addItem("TileMu/etatag",m_ntag,m_eta,-1.5,1.5);
   sc=m_ntuplePtr->addItem("TileMu/phitag",m_ntag,m_phi,0.,6.3);  
   sc=m_ntuplePtr->addItem("TileMu/energydepVec",m_ntag, m_energy,4);
   sc=m_ntuplePtr->addItem("TileMu/quality",m_ntag,m_quality,0.,1.);

   if(sc == StatusCode::FAILURE) {
      log<<MSG::ERROR<<"cannot add item to column wise ntuple"<<endreq;
      return StatusCode::FAILURE;
   }

   log<<MSG::INFO<<"initialisation completed"<<endreq ;
   return StatusCode::SUCCESS;
}

StatusCode TrigTileMuToNtuple::execute()
{
   MsgStream log(messageService(), name());
   StatusCode sc;

   // step1: read  from TDS
   const TileMuContainer*  mutags_cont;
   sc=m_storeGate->retrieve(mutags_cont, m_TileMuContainer);
   if(sc == StatusCode::FAILURE) {
      log<<MSG::ERROR<<"cannot retrieve TileMuContainer in TDS"<<endreq;
      return StatusCode::FAILURE;
   }

   
   TileMuContainer::const_iterator it= mutags_cont->begin(); 
   TileMuContainer::const_iterator end=mutags_cont->end(); 
   m_ntag=0;
   for(; it != end; ++it) {
      m_eta[m_ntag]=(*it)->eta();
      m_energy[m_ntag][0] =  ((*it)->enedep()) [0];
      m_energy[m_ntag][1] =  ((*it)->enedep()) [1];
      m_energy[m_ntag][2] =  ((*it)->enedep()) [2];
      m_energy[m_ntag][3] =  ((*it)->enedep()) [3];

      m_phi[m_ntag]=(*it)->phi();
      m_quality[m_ntag]=(*it)->quality();
      m_ntag++;

      if (m_ntag >= max_ntag) break;
   }      
 
   //  write  ntuple (is useful to keep it  open to write also MC truth) 
   if (m_close == 1)
	{
   sc=ntupleService()->writeRecord(m_ntuplePtr);
   if(sc.isFailure()) {
     log<<MSG::ERROR<<"failed  to write Ntuple"<<endreq;
      return( StatusCode::FAILURE);
        }
   }

   // Execution completed.
   log<<MSG::DEBUG<<"execute() completed successfully"<<endreq;
   return StatusCode::SUCCESS;
}

StatusCode TrigTileMuToNtuple::finalize()
{
   MsgStream log(messageService(), name());
   log<<MSG::INFO<<"finalize() successfully"<<endreq;
   return StatusCode::SUCCESS;
}

