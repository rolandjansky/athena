/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHistNtup/MTNtup.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ThreadGaudi.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include <sstream>

/////////////////////////////////////////////////////////////////////////////

MTNtup::MTNtup(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_ntuple(0)
{}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTNtup::initialize() {

   StatusCode sc = StatusCode::SUCCESS;

   ATH_MSG_INFO("in initialize()");

#ifdef ATLAS_GAUDI_V21
   SmartIF<IService> tmp_msgSvc(msgSvc());
   if(tmp_msgSvc.isValid()) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
                   << tmp_msgSvc->name());
   }
  
   // Create a column-wise ntuple
   SmartIF<IService> tmp_ntupSvc(ntupleSvc());
   if(tmp_ntupSvc.isValid()) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Ntuple Service = "
                   << tmp_ntupSvc->name());
   }
#else
   Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
   if(tmp_msgSvc != 0) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = "
                   << tmp_msgSvc->name());
   }
  
   // Create a column-wise ntuple
   Service* tmp_ntupSvc = dynamic_cast<Service*> (ntupleSvc());
   if(tmp_ntupSvc != 0) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Ntuple Service = "
                   << tmp_ntupSvc->name());
   }
#endif

   /*
   //  TB try
   std::string ntname="/STR"+getGaudiThreadIDfromName(name()).erase(0,2);
   // another ntuple (booking got from iPatRecAlgo iPaNtuple
   SmartDataPtr<NTuple::Directory>
   DiriPat(ntupleSvc(), ntname+"/COL");
  
   if ( !DiriPat )  {
   DiriPat = ntupleSvc()->createDirectory(ntname+"/COL");
   }
   ATH_MSG_INFO("Booking of "+ntname+"/COL done ");

  
   // Book the Ntuple AND CHECK IT:
   m_ntuple = ntupleService()->book(DiriPat.ptr(),
   10,
   CLID_ColumnWiseTuple,
   "iPatRec Ntuple");



   if (!m_ntuple) {
   log << MSG::ERROR  << "Failed to book ntuple : iPatNtuple"
   );
   return StatusCode::FAILURE;
   }
   sc = m_ntuple->addItem ("Event", m_event);
   sc = m_ntuple->addItem ("Run", m_run);
   sc = m_ntuple->addItem ("Size", m_size, 0, 100);
   sc = m_ntuple->addItem ("rundata", m_size, m_rundata);
  

   ATH_MSG_INFO("Booking went fine ");
   */

  
   // make stream name 
   std::string ntname=std::string("/NTUPLES/STR")+getGaudiThreadIDfromName(name());
   // ntname contains NULL character at the end ( it is inherited from the alg name)
  
  
   // remove as many nulls at the end as needed
   while ( ntname[ntname.size()-1] == '\0' )
      ntname.erase(ntname.size()-1);
  
   std::string dirname = ntname+std::string("/COL");
  
   ATH_MSG_INFO("Will use this stream " << ntname.c_str());
   NTuple::Directory *col=0;
   NTupleFilePtr file1(ntupleSvc(), ntname.c_str());
   ATH_MSG_INFO("Will use this dir " << dirname.c_str());
  
   if ( 0 != file1 )    {
      if ( !(col=ntupleSvc()->createDirectory(dirname.c_str())) )   {
         ATH_MSG_ERROR("Cannot create directory " << dirname );  
      }
   } else {
      ATH_MSG_ERROR("Cannot ACCESS file pointer " << ntname );
      return StatusCode::FAILURE;
   }  
  
   ATH_MSG_INFO("Created directory " << dirname);
   NTuplePtr nt1(ntupleSvc(), (dirname+"/10").c_str());
  
   // Check if already booked
   if ( 0 == nt1 )    {
      nt1 = ntupleSvc()->book (col, 10, CLID_ColumnWiseTuple, "Col Wise");
      if ( 0 != nt1 ) {
         m_ntuple = nt1;
         sc = nt1->addItem ("Event", m_event);
         if ( sc == StatusCode::FAILURE ) {
            ATH_MSG_ERROR("could not add item to col wise ntuple" 
                          );
         }
         sc = nt1->addItem ("Run", m_run);
         sc = nt1->addItem ("Size", m_size, 0, 100);
         sc = nt1->addItem ("rundata", m_size, m_rundata);
      } else {
         ATH_MSG_ERROR("could not book Ntuple");
      }
   } else {
      ATH_MSG_ERROR("Ntuple is already booked");
   }
  
   return sc;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTNtup::execute() {

   ATH_MSG_INFO("in execute()");

   // Get Event data  
   const EventInfo* pevt;
   if (StatusCode::SUCCESS != evtStore()->retrieve(pevt,"")) {
      ATH_MSG_ERROR(" could not get event");
      return StatusCode::FAILURE;
   } else {
      m_size = 2 + (pevt->event_ID()->event_number() % 3);
      m_event = pevt->event_ID()->event_number();
      m_run = pevt->event_ID()->run_number();
      m_rundata[0] = m_run;
      m_rundata[1] = m_event;
      for (int i=2; i<m_size; i++) {
         m_rundata[i] = i;
      }
   }

   // Fill ntuple
   if (!(ntupleSvc()->writeRecord(m_ntuple)).isSuccess()) {
      ATH_MSG_ERROR("problems writing ntuple record");
   }else
      ATH_MSG_INFO("One record written ");
  
   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTNtup::finalize() {
   ATH_MSG_INFO("in finalize()");
   return StatusCode::SUCCESS;
}
