/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*****************************************************************************
//  Filename : TrigTileMuToNtuple.cxx
//*****************************************************************************

//Gaudi Includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

//TileCalo include

#include "TileEvent/TileMuContainer.h"
#include "TrigTileMuId/TrigTileMuToNtuple.h"

//const int max_ntag=50;

// Constructor & deconstructor
TrigTileMuToNtuple::TrigTileMuToNtuple(std::string name, ISvcLocator* 
pSvcLocator)
   :AthAlgorithm(name, pSvcLocator),
    m_ntuplePtr(NULL)
{
   m_TileMuContainer="TileMuObj";
   m_ntupleLoc="/FILE1/TileMuTag";
   m_ntupleID=100;
   m_max_ntag=50;
   m_close=1;
   declareProperty("TileMuTagsOutputName", m_TileMuContainer);    
   declareProperty("NTupleLoc", m_ntupleLoc);
   declareProperty("NTupleID", m_ntupleID);
   declareProperty("MaxNtag", m_max_ntag);
   declareProperty("CloseNtuple", m_close);
}

TrigTileMuToNtuple::~TrigTileMuToNtuple()
{
}

// Alg standard interface function
StatusCode TrigTileMuToNtuple::initialize() 
{ 
   StatusCode sc;

       m_ntupleLoc="/NTUPLES/FILE1/TileMuTag";
   // + m_ntupleLoc;

   SmartDataPtr<NTuple::Directory> DirPtr(ntupleSvc(), m_ntupleLoc);
   if(!DirPtr) DirPtr=ntupleSvc()->createDirectory(m_ntupleLoc);
   if(!DirPtr) {
     ATH_MSG_ERROR("Invalid Ntuple Directory: "<< m_ntupleLoc);
      return StatusCode::FAILURE;
   }

   m_ntuplePtr=ntupleSvc()->book(DirPtr.ptr(), m_ntupleID, 
                                     CLID_ColumnWiseTuple, "TileMuTag-Ntuple");
   if(!m_ntuplePtr) {
     ATH_MSG_ERROR("Failed to book ntuple: TileMuTagNtuple");
      return StatusCode::FAILURE;
   }

   sc=m_ntuplePtr->addItem("TileMu/ntag",m_ntag,0,m_max_ntag);
   sc=m_ntuplePtr->addItem("TileMu/etatag",m_ntag,m_eta,-1.5,1.5);
   sc=m_ntuplePtr->addItem("TileMu/phitag",m_ntag,m_phi,0.,6.3);  
   sc=m_ntuplePtr->addItem("TileMu/energydepVec",m_ntag, m_energy,4);
   sc=m_ntuplePtr->addItem("TileMu/quality",m_ntag,m_quality,0.,1.);

   if(sc == StatusCode::FAILURE) {
     ATH_MSG_ERROR("cannot add item to column wise ntuple");
      return StatusCode::FAILURE;
   }

   ATH_MSG_INFO("initialisation completed");
   return StatusCode::SUCCESS;
}

StatusCode TrigTileMuToNtuple::execute()
{
   StatusCode sc;

   // step1: read  from TDS
   const TileMuContainer*  mutags_cont = nullptr;
   sc=evtStore()->retrieve(mutags_cont, m_TileMuContainer);
   if(sc == StatusCode::FAILURE) {
     ATH_MSG_ERROR("cannot retrieve TileMuContainer in TDS");
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

      if (m_ntag >= m_max_ntag) break;
   }      
 
   //  write  ntuple (is useful to keep it  open to write also MC truth) 
   if (m_close == 1)
	{
   sc=ntupleSvc()->writeRecord(m_ntuplePtr);
   if(sc.isFailure()) {
     ATH_MSG_ERROR("failed  to write Ntuple");
      return( StatusCode::FAILURE);
        }
   }

   // Execution completed.
   ATH_MSG_DEBUG("execute() completed successfully");
   return StatusCode::SUCCESS;
}

StatusCode TrigTileMuToNtuple::finalize()
{
   ATH_MSG_INFO("finalize() successfully");
   return StatusCode::SUCCESS;
}

