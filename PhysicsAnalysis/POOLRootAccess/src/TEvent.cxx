/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLRootAccess/TEvent.h"

#include "AthenaKernel/ICollectionSize.h"

#include "GaudiKernel/IEventProcessor.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TFile.h"
#include "TChain.h"



namespace POOL {

IAppMgrUI* Init( const char* options ) {
   IAppMgrUI* theApp = Gaudi::createApplicationMgr();
   //check if already configured 
   if(theApp->FSMState() != Gaudi::StateMachine::OFFLINE) return theApp;
   //set the joboptions 
   SmartIF<IProperty> propMgr(theApp);
   propMgr->setProperty("JobOptionsPath",options);
   //configure and return
   theApp->configure(); 
   propMgr->setProperty("OutputLevel","3"); //INFO
   theApp->initialize();
   return theApp;
}

TEvent::~TEvent() {
  //need to destroy my storegate, selector, and loop 
  if(m_evtLoop.isSet()) m_evtLoop.release();
  if(m_evtSelect.isSet()) m_evtSelect.release();
  if(m_evtStore.isSet()) m_evtStore.release();
}

TEvent::TEvent(const std::string& name ) : TEvent( kPOOLAccess , name ) { }

TEvent::TEvent(EReadMode mode, const std::string& name) : 
   m_joSvc("JobOptionsSvc","TEvent"+name),
   m_evtLoop("AthenaEventLoopMgr/"+name+"_EventLoopMgr","TEvent"+name),
   m_evtSelect("EventSelectorAthenaPool/"+name+"_EventSelector","TEvent"+name),
   m_evtStore("StoreGateSvc/"+name,"TEvent"+name),
   m_activeStoreSvc("ActiveStoreSvc","TEvent"+name),
   m_inputMetaStore("InputMetaDataStore","TEvent"+name) /*fixme, when reading multiple files at once?*/ {

   //FIXME: Should protect against attempt to mix POOL with nonPOOL
   if(mode==kPOOLAccess) {
    InitPOOL();
   } else {
    InitxAOD();
    //add the access mode property to catalog 
    m_joSvc->addPropertyToCatalogue( m_evtSelect.name() , IntegerProperty( "AccessMode" , int(mode) ) );
   }

   //check if a SelectorType has been specified in the joSvc 
   auto properties = m_joSvc->getProperties("TEvent");
   if(properties) {
      for(auto prop : *properties) {
         if(prop->name()=="EventSelectorType") m_evtSelect.setTypeAndName(prop->toString() + "/" + name+"_EventSelector");
      }
   }

   m_joSvc->addPropertyToCatalogue( m_evtLoop.name() , StringProperty( "ClearStorePolicy" , "BeginEvent") );
   m_joSvc->addPropertyToCatalogue( m_evtLoop.name() , StringProperty( "EvtSel" , m_evtSelect.typeAndName() ) );
   m_joSvc->addPropertyToCatalogue( m_evtLoop.name() , StringProperty( "EvtStore" , m_evtStore.typeAndName() ) );
   m_joSvc->addPropertyToCatalogue( m_evtLoop.name() , StringProperty( "EventPrintoutInterval" , "999999999" ) );

   if(m_evtSelect.type()=="Athena::xAODEventSelector") {
      m_joSvc->addPropertyToCatalogue( m_evtSelect.name() , BooleanProperty( "ReadMetaDataWithPool" , true) );
   }

   //set outputlevels to WARNING 
   m_joSvc->addPropertyToCatalogue( m_evtLoop.name() , IntegerProperty( "OutputLevel" , 4 ) );
   m_joSvc->addPropertyToCatalogue( m_evtSelect.name() , IntegerProperty( "OutputLevel" , 4 ) );
   m_joSvc->addPropertyToCatalogue( m_evtStore.name() , IntegerProperty( "OutputLevel" , 4 ) );
   m_joSvc->addPropertyToCatalogue( m_activeStoreSvc.name() , IntegerProperty( "OutputLevel" , 4 ) );
   //suppress messages below WARNING too
   //do this here to stop some pre initialize INFO messages from showing
   ServiceHandle<IProperty> messageSvc("MessageSvc","");
   std::vector<std::string> p;
   Gaudi::Parsers::parse(p,messageSvc->getProperty("setWarning").toString());
   p.push_back( m_evtLoop.name() );
   p.push_back( m_evtSelect.name() );
   p.push_back( m_evtStore.name() );
   p.push_back( m_activeStoreSvc.name() );
   messageSvc->setProperty(  StringArrayProperty( "setWarning" , p ) );
   
   //also push this into the joboptionsvc so that if a reinit happens then these settings are remembered 
   //m_joSvc->addPropertyToCatalogue( "MessageSvc", StringArrayProperty( "setWarning", p ) );
   
}


void TEvent::setActive() {
  m_activeStoreSvc->setStore(&*m_evtStore);
}

long TEvent::getEntries() {
   if(m_size>=0) return m_size;
   if(!m_evtLoop.isSet()) { if(m_evtLoop.retrieve().isFailure()) return 0; }
   m_size = dynamic_cast<ICollectionSize*>(&*m_evtLoop)->size();
   return m_size;
}

StatusCode TEvent::readFrom( TFile* file ) {
   if(file==0 || file->IsZombie()) return StatusCode::FAILURE;
   return readFrom( file->GetName() );
}

StatusCode TEvent::readFrom( const char* file ) {
   if(m_curEntry >= 0) {
      std::cout << "Unable to change file after already reading" << std::endl;
      return StatusCode::FAILURE;
   }
   std::string prop = "[";
   bool hasFile(false);
   //see if contains wildcard
   //separate by comma
   TString sFileIn(file);
   std::unique_ptr<TObjArray> sFiles(sFileIn.Tokenize(","));
   std::unique_ptr<TIterator> itr(sFiles->MakeIterator());
   TObject* obj;
   while( (obj = itr->Next()) ) {
     TString sFile(static_cast<TObjString*>(obj)->String());
     if(sFile.Contains('*')) {
      std::unique_ptr<TObjArray> theFiles(gSystem->GetFromPipe(("ls " + std::string(file)).c_str()).Tokenize("\n"));
      for(int i=0;i<theFiles->GetEntries();i++) {
         //std::cout << "Adding " << dynamic_cast<TObjString*>(theFiles->At(i))->String().Data() << std::endl;
         if(i != 0) prop += " , ";
         prop += "'"; prop += gSystem->ExpandPathName(dynamic_cast<TObjString*>(theFiles->At(i))->String().Data()); prop += "'";
      }
     } else {
       if(hasFile) prop += " , ";
	prop += "'"; prop += gSystem->ExpandPathName(sFile.Data()); prop += "'";
     }
     hasFile=true;
   }

  
   prop += "]";
   //std::cout << prop << std::endl;
   return m_joSvc->addPropertyToCatalogue( m_evtSelect.name() , StringProperty( "InputCollections" , prop ) );
}

StatusCode TEvent::readFrom(TChain* files) {

  //add all files to the property list by giving a comma separated list of files
  std::string sfiles;
  std::unique_ptr<TIterator> itr(files->GetListOfFiles()->MakeIterator());
  TObject* obj;
  bool hasFile(false);
  while( (obj = itr->Next()) ) {
    if(hasFile) sfiles += ",";
    sfiles += obj->GetTitle();
    hasFile=true;
  }
  //std::cout << sfiles << std::endl;
  return readFrom(sfiles.c_str());

}

int TEvent::getEntry( long entry ) {
   if(!m_evtLoop.isSet()) { if(m_evtLoop.retrieve().isFailure()) return -1; }
   if(entry==0 || entry != m_curEntry+1) m_evtLoop->seek(entry); //need to seek on first read or if not reading next event
   //ensure our storegate is the active store
   setActive();
   StatusCode out = dynamic_cast<IEventProcessor*>(&*m_evtLoop)->nextEvent(entry+1);
   m_curEntry = entry;
   return (out.isSuccess()) ? 0 : -1;
}


  //python bindings

  PyObject* TEvent::retrieve( PyObject* tp, PyObject* pykey )
 {
   return AthenaInternal::retrieveObjectFromStore( &*evtStore() , tp, pykey );
 }

  PyObject* TEvent::get_item( PyObject* pykey ){
    return AthenaInternal::py_sg_getitem( &*evtStore() , pykey );
  }
  PyObject* TEvent::contains( PyObject* tp, PyObject* pykey ) {
    return AthenaInternal::py_sg_contains( &*evtStore() , tp,  pykey );
  }
  PyObject* TEvent::retrieveMetaInput(PyObject* tp, PyObject* pykey) {
    return AthenaInternal::retrieveObjectFromStore( &*inputMetaStore() , tp, pykey );
  }
  PyObject* TEvent::record  (PyObject* obj, PyObject* pykey,bool allowMods,bool resetOnly,bool noHist) {
    return AthenaInternal::recordObjectToStore( &*evtStore() , obj, pykey,allowMods,resetOnly,noHist );
  }



} //end of POOL namespace
