/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonboyToIdentifier_H
#define MuonboyToIdentifier_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "MboyEDMToolInterfaces/IMuonboyToIdentifier.h"

class MdtIdHelper ;
class RpcIdHelper ;
class TgcIdHelper ;
class CscIdHelper ;

/////////////////////////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyEDMToolMisc.h"

  /**
   @class MuonboyToIdentifier

    This tool Identifier for a MbIOObject object
   
  @author samusog@cern.ch
  
  */

class MuonboyToIdentifier: public AthAlgTool, virtual public IMuonboyToIdentifier{
public:
    MuonboyToIdentifier(const std::string&,const std::string&,const IInterface*);
    virtual ~MuonboyToIdentifier 	 ();

public:
///////////////////////////////////

   virtual StatusCode initialize	();
   virtual StatusCode finalize  	();

   /** Return Identifier for a  MbIOObject */
   virtual Identifier GetIdentifierForMbIOObject   (const  MbIOObject*    pMbIOObject, bool& isValid ) ;

   /** Return Identifier for a  MbIDTag */
   virtual Identifier GetIdentifierForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag, bool& isValid );
   virtual Identifier GetIdentifierForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag, bool& isValid );
   virtual Identifier GetIdentifierForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag, bool& isValid );
   virtual Identifier GetIdentifierForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag, bool& isValid );
   virtual Identifier GetIdentifierForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag, bool& isValid );

private:
///////////////////////////////////

   const MdtIdHelper * m_mdtId ; //!< IdHelper
   const RpcIdHelper * m_rpcId ; //!< IdHelper
   const TgcIdHelper * m_tgcId ; //!< IdHelper
   const CscIdHelper * m_cscId ; //!< IdHelper

};

#endif

