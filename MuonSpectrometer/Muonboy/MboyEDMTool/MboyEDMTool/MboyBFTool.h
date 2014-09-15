/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MboyBFTool_H
#define MboyBFTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h" 
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "MboyEDMToolInterfaces/IMboyBFTool.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MboyBFTool

   This class is the tool insuring connection to B Field service 
   
  @author samusog@cern.ch
  
  */

class MuonboyBFAth;

class MboyBFTool:public AthAlgTool, virtual public IMboyBFTool{
public:
    MboyBFTool(const std::string&,const std::string&,const IInterface*);
    virtual ~MboyBFTool 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Get acces to the GetMuonboyBFAbstract*/
   virtual StatusCode GetMuonboyBFAbstract(
               MuonboyBFAbstract*& pMuonboyBFAbstract
              );

   virtual StatusCode UpdatedTool(IOVSVC_CALLBACK_ARGS);

   virtual int UpdatedToolStamp()  ;
   virtual int getToroidBarrelOn() ;
   virtual int getToroidECTAOn()   ;
   virtual int getToroidECTCOn()   ;
   virtual int getSolenoidOn()     ;
   virtual int getAllToroidOn()    ;
   virtual int getFieldStatusOn()  ;


///////////////////////////////////

   ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle;
       
   MuonboyBFAth*      p_MuonboyBFAth      ; //!< Pointer On MuonboyBFAth
 
   int m_UpdatedToolStamp  ;

};

#endif

