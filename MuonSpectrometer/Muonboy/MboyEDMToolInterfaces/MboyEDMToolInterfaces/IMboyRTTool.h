/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyRTTool_H
#define IMboyRTTool_H

#include "GaudiKernel/IAlgTool.h" 

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MuonboyRTAbstract;
class MuonRecDigiStoreAth;

  /**
   @class IMboyRTTool

   This class is the tool insuring connection to RT services. 
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyRTTool("IMboyRTTool", 1, 0);

class IMboyRTTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractWithDigiUpdate(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
              ) = 0 ;

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractWithDigiUpdateT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               MuonRecDigiStoreAth* pMuonRecDigiStoreAth,
               int WireSag
              ) = 0 ;

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstract(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
              ) = 0 ;

   /** Get acces to the selected MuonboyRTAbstract*/
   virtual StatusCode GetMuonboyRTAbstractT0Ctrl(
               MuonboyRTAbstract*& pMuonboyRTAbstract,
               const MuonRecDigiStoreAth* pMuonRecDigiStoreAth
              ) = 0 ;

};

const InterfaceID& IMboyRTTool::interfaceID(){ return IID_IIMboyRTTool; }

#endif

