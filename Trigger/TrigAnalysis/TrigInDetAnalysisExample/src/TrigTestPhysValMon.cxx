/* emacs: this is -*- c++ -*- */
/**
 **     @file    TrigTestPhysValMon.cxx
 **
 **     @brief   implementation of an IHLTMonToolAC 
 **
 **     @author  mark sutton
 **     @date    Wed 28 Oct 2009 02:23:23 CET 
 **
 **     Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **/


#include "TrigInDetAnalysisExample/TrigTestPhysValMon.h"



TrigTestPhysValMon::TrigTestPhysValMon(const std::string & type, const std::string & name, const IInterface* parent) 
  : TrigTestBase( type, name, parent)
{
  ATH_MSG_INFO("TrigTestPhysValMon::TrigTestPhysValMon() compiled: " << __DATE__ << " " << __TIME__);
}



TrigTestPhysValMon::~TrigTestPhysValMon() { 
  ATH_MSG_INFO("TrigTestPhysValMon::~TrigTestPhysValMon()");
}



StatusCode TrigTestPhysValMon::initialize() {
 
  ATH_MSG_DEBUG(" ----- enter init() ----- ");
  ATH_MSG_INFO("TrigTestPhysValMon::init() " << gDirectory->GetName());

  //  StatusCode sc = TrigTestBase::init();
  StatusCode sc = ManagedMonitorToolBase::initialize();

  ATH_MSG_DEBUG(" -----  exit init() ----- ");
  return sc;
}



#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestPhysValMon::bookHistograms() 
#else
StatusCode TrigTestPhysValMon::bookHistograms(bool newEventsBlock, bool newLumiBlock, bool newRun) 
#endif
{
  ATH_MSG_VERBOSE(" ----- enter bookHistograms() ----- ");
  
  ATH_MSG_DEBUG("TrigTestPhysValMon::bookHistograms() " << gDirectory->GetName());
    

  ATH_MSG_INFO("TrigTestPhysValMon::bookHistograms()"); 
  //	       << "\tbuildNtuple "   << m_buildNtuple
  //	       << "\tNewEventBlock " << newEventsBlock 
  //	       << "\tNewLumiBlock "  << newLumiBlock 
  //	       << "\tNewRun "        << newRun);
  

#ifdef ManagedMonitorToolBase_Uses_API_201401
  StatusCode sc = TrigTestBase::book();
#else
  StatusCode sc = TrigTestBase::book( newEventsBlock, neLumiBloock, newRun ); 
#endif 

  ATH_MSG_VERBOSE(" ----- exit book() ----- ");
  
  return sc;
}




StatusCode TrigTestPhysValMon::fillHistograms() { 
  ATH_MSG_VERBOSE(" ----- enter fill() ----- ");
  StatusCode sc = TrigTestBase::fill();
  ATH_MSG_VERBOSE(" ----- exit fill() ----- ");
  return sc;
}






#ifdef ManagedMonitorToolBase_Uses_API_201401
StatusCode TrigTestPhysValMon::procHistograms() 
#else
StatusCode TrigTestPhysValMon::procHistograms(bool /*endOfEventsBlock*/, bool /*endOfLumiBlock*/, bool endOfRun)
#endif
{
  ATH_MSG_INFO(" ----- enter proc() ----- ");  
  StatusCode sc = TrigTestBase::proc();
  ATH_MSG_VERBOSE(" ------ exit proc() ------ ");
  return sc;
}


