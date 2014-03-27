/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMboyBFTool_H
#define IMboyBFTool_H

#include "GaudiKernel/IAlgTool.h" 

#include "AthenaKernel/IOVSvcDefs.h" 

/////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MuonboyBFAbstract;

  /**
   @class IMboyBFTool

   This class is the tool insuring connection to B Field service 
   
  @author samusog@cern.ch
  
  */

static const InterfaceID IID_IIMboyBFTool("IMboyBFTool", 1, 0);

class IMboyBFTool:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Get acces to the GetMuonboyBFAbstract*/
   virtual StatusCode GetMuonboyBFAbstract(
               MuonboyBFAbstract*& pMuonboyBFAbstract
              ) = 0 ;

   virtual StatusCode UpdatedTool(IOVSVC_CALLBACK_ARGS) = 0;

   virtual int UpdatedToolStamp() = 0;
   virtual int getToroidBarrelOn() = 0;
   virtual int getToroidECTAOn()   = 0;
   virtual int getToroidECTCOn()   = 0;
   virtual int getSolenoidOn()     = 0;
   virtual int getAllToroidOn()    = 0;
   virtual int getFieldStatusOn()  = 0;

};

inline const InterfaceID& IMboyBFTool::interfaceID(){ return IID_IIMboyBFTool; }


#endif

