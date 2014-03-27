/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMuonboyToIdentifier_H
#define IMuonboyToIdentifier_H

#include "GaudiKernel/IAlgTool.h" 

#include "Identifier/Identifier.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMToolInterfaces/IMboyEDMToolMisc.h"

class MbIOObject   ;

class MbIDTagMDT   ;
class MbIDTagRPC   ;
class MbIDTagTGC   ;
class MbIDTagCSC   ;
class MbIDTagCSCclu;

static const InterfaceID IID_IMuonboyToIdentifier("IMuonboyToIdentifier", 1, 0);

  /**
   @class IMuonboyToIdentifier

    This tool Identifier for a MbIOObject object
   
  @author samusog@cern.ch
  
  */

class IMuonboyToIdentifier:virtual public IAlgTool {

public:
///////////////////////////////////

   static const InterfaceID& interfaceID();

   /** Return Identifier for a  MbIOObject */
   virtual Identifier GetIdentifierForMbIOObject   (const  MbIOObject*    pMbIOObject, bool& isValid ) = 0 ;

   /** Return Identifier for a  MbIDTag */
   virtual Identifier GetIdentifierForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag, bool& isValid ) = 0 ;
   virtual Identifier GetIdentifierForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag, bool& isValid ) = 0 ;
   virtual Identifier GetIdentifierForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag, bool& isValid ) = 0 ;
   virtual Identifier GetIdentifierForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag, bool& isValid ) = 0 ;
   virtual Identifier GetIdentifierForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag, bool& isValid ) = 0 ;

};

inline const InterfaceID& IMuonboyToIdentifier::interfaceID() { return IID_IMuonboyToIdentifier;}

#endif

