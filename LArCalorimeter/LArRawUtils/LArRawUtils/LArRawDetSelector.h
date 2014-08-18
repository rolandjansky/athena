/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWUTILS_LARRAWROISELECTOR_H
#define LARRAWUTILS_LARRAWROISELECTOR_H


#include "LArRawEvent/LArRawChannelContainer.h" 
#include "LArRawEvent/LArRawChannel.h" 
#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

/** 
	This class provide access to LArRawChannels in each sub detector
	Input: LAr Detector Identifier
		Allowed are 
			lar_em
			lar_hec
			lar_fcal
*/
class LArRawDetSelector {

  public: 

   LArRawDetSelector ( const LArRawChannelContainer* c=NULL );
   virtual ~LArRawDetSelector () {} 

   void setDet( const Identifier& id) ; 
   void setDet(const HWIdentifier& chid );

   virtual bool select(const LArRawChannel * rawchan); 

 private: 
   const LArOnlineID* m_onlineID; 
   const CaloCell_ID* m_caloCellID;
   bool m_em; 
   bool m_hec; 
   bool m_fcal;  
};


#endif 
