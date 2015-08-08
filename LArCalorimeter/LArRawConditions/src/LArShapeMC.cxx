/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArShapeMC.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "LArElecCalib/ILArMCSymTool.h"

#include <iostream> 
using namespace std ;

LArShapeMC::LArShapeMC() : ILArShape(), 
			   LArConditionsContainer<LArShapeP1>(), 
			   m_larmcsym("LArMCSymTool")
{} 

StatusCode LArShapeMC::initialize()
{

  if(m_larmcsym.retrieve().isFailure()){
      MsgStream log(Athena::getMessageSvc(), "LArRampMC");
      log << MSG::WARNING << "Could not retrieve LArMCSymTool " << endreq; 
      return (StatusCode::FAILURE);
  }

  return (CONTAINER::initialize()); 
}

LArShapeMC::~LArShapeMC() {}


namespace {

void do_get_shape (const std::vector<float>& shape,
                   const HWIdentifier& CellID,
                   int tbin,
                   int mode,
                   bool flag,
                   std::vector<float>& v)
{
  v.clear();

  if( shape.size() == 0 ){
    MsgStream logstr(Athena::getMessageSvc(), "LArShapeComplete");
    logstr << MSG::DEBUG << "Invalid ID 0x" << MSG::hex << CellID
           << MSG::dec << endreq;
    return; 
  }

  //int NOFCPhases   = 25;
  //int NOFCTimeBins = 25;
  
  int NOFCPhases   = 50; // not really used in current implementation
  int NOFCTimeBins = 24; // fits to CTB/P3C data
  
  if ( mode == 10 ) { /* 10 = H8 CTB 2004 RTM OFC */
// Keep this old implementation for mode==10 only. 

    NOFCPhases   = 50 ;
    NOFCTimeBins = 24 ;
    
    unsigned int MaxSamples   = NOFCTimeBins * 5 + ( NOFCPhases - NOFCTimeBins );

    if ( tbin >= NOFCPhases) { /* invalid tbin: return empty vector */
     MsgStream logstr(Athena::getMessageSvc(), "LArShapeComplete");
     logstr << MSG::ERROR << " LArShapeComplete: Invalid tbin " << tbin
           << " for cell 0x " << MSG::hex << CellID << MSG::dec << endreq;
    }
    else if ( shape.size() < MaxSamples ) {
     MsgStream logstr(Athena::getMessageSvc(), "LArShapeComplete");
     logstr << MSG::WARNING
           << "stored vector is too small for TB mode" << endreq;
    }
    else {
     int min = 0, max = 5;
     // ??? Does the flag=true behavior make sense?
     if (shape.size() > (flag ? MaxSamples : NOFCPhases)) {
      min = 1;
      max = 6;
     }
     for (int i=min;i<max;i++) {
      v.push_back (shape[i*NOFCTimeBins+tbin]);
     }
    }
  } else 
  {
// new implementation, as discussed in April 2006. 
//  return all possible samples spaced by 25ns (32 samples)
//  tbin>0 gives earlier pulse. 
//  tbin is not restricted to be less 25ns. 

     unsigned int size = shape.size(); 
     for (unsigned int i = 0; i< size ; i += NOFCTimeBins ) 
     { 
	unsigned int j = i+tbin ; 
        if( j > size )
	  v.push_back(0); 
	else
	  v.push_back(shape[j]); 
     } 
  }
}


#if 0
bool setup_cabling_service (const Identifier& CellID,
                            HWIdentifier& OnId)
{
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isFailure()) {
    MsgStream logstr(Athena::getMessageSvc(), "LArShapeMC");
    logstr << MSG::ERROR << "Could not retrieve ToolSvc " << endreq;
    return false; 
  }

  LArCablingService* cablingService;
  sc = toolSvc->retrieveTool("LArCablingService",cablingService);
  if(sc.isFailure()){
    MsgStream logstr(Athena::getMessageSvc(), "LArShapeMC");
    logstr << MSG::ERROR << "Could not retrieve LArCablingService Tool "
           << endreq;
    return false; 
  }

  OnId = cablingService->createSignalChannelID(CellID);  

  return true;
}
#endif


} // anonymous namespace


/* 
 * retrieve Shape
 */
LArShapeMC::ShapeRef_t
LArShapeMC::Shape(const HWIdentifier& CellID,
                  int gain,
                  int tbin=0,
                  int mode=0 ) const 
{ 
  static std::vector<float> v;   
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  do_get_shape (get(SymCellID, gain).m_vShape, CellID, tbin, mode, false, v);
  return v;
}

LArShapeMC::ShapeRef_t
LArShapeMC::ShapeDer(const HWIdentifier& CellID,
                     int gain,
                     int tbin=0,
                     int mode=0 ) const 
{ 
  static std::vector<float> v; 
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  do_get_shape (get(SymCellID, gain).m_vShapeDer, CellID, tbin, mode, true, v);
  return v;
}


LArShapeMC::ShapeRef_t
LArShapeMC::Shape(const Identifier&  CellID,
                  int gain,
                  int tbin=0,
                  int mode=0 ) const
{
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID);
  return Shape(SymCellID, gain, tbin, mode);
}

LArShapeMC::ShapeRef_t
LArShapeMC::ShapeDer(const Identifier&  CellID,
                     int gain,
                     int tbin=0,
                     int mode=0) const
{
  HWIdentifier SymCellID = m_larmcsym->symOnline(CellID); 
  return ShapeDer(SymCellID, gain, tbin, mode);
}
