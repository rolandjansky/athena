/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToIdentifier.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbIOObject.h"
#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"
#include "MboyCoreEvt/MbIDTagCSC.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

#include "MboyCoreEvt/MbPOT.h"

#include "MboyCoreEvt/MbROT.h"
#include "MboyCoreEvt/MbROTMDT.h"
#include "MboyCoreEvt/MbROTRPC.h"
#include "MboyCoreEvt/MbROTTGC.h"
#include "MboyCoreEvt/MbROTCSC.h"
#include "MboyCoreEvt/MbROTCSCclu.h"

#include "MboyCoreEvt/MbHole.h"
#include "MboyCoreEvt/MbHoleMDT.h"
#include "MboyCoreEvt/MbHoleRPC.h"
#include "MboyCoreEvt/MbHoleTGC.h"
#include "MboyCoreEvt/MbHoleCSC.h"
#include "MboyCoreEvt/MbHoleCSCclu.h"

MuonboyToIdentifier::MuonboyToIdentifier(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p) 
{

  declareInterface<IMuonboyToIdentifier>(this);

}

MuonboyToIdentifier::~MuonboyToIdentifier(){}

// Initialize
StatusCode MuonboyToIdentifier::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve Helpers
  sc = detStore()->retrieve(m_mdtId,"MDTIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve MDTIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_cscId,"CSCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve CSCIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_rpcId,"RPCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve RPCIDHELPER" );
    return( StatusCode::FAILURE );
  }
  sc = detStore()->retrieve(m_tgcId,"TGCIDHELPER");
  if (sc.isFailure()){
    ATH_MSG_FATAL( "Cannot retrieve TGCIDHELPER" );
    return( StatusCode::FAILURE );
  }


  ATH_MSG_INFO( "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode MuonboyToIdentifier::finalize(){return StatusCode::SUCCESS;}


Identifier MuonboyToIdentifier::GetIdentifierForMbIOObject   (const  MbIOObject*    pMbIOObject, bool& isValid )
{

 isValid = false ;  
 
 if (!pMbIOObject) {
    ATH_MSG_INFO( "GetIdentifierForMbIOObject called with null pMbIOObject "
                   );
    Identifier aIdentifier; return aIdentifier ;
 }

 const MbPOT* pMbPOT = dynamic_cast<const MbPOT*> (pMbIOObject);
 if (!pMbPOT) {
    ATH_MSG_INFO( "GetIdentifierForMbIOObject called with a pMbIOObject which is NOT a pMbPOT "
                   );
    Identifier aIdentifier; return aIdentifier ;
 }

 const MbROT* pMbROT = dynamic_cast<const MbROT*> (pMbPOT);
 if (pMbROT) {
   const MbROTMDT* pMbROTMDT = dynamic_cast<const MbROTMDT*> (pMbROT);
   if (pMbROTMDT) {
     const MbIDTagMDT* pMbIDTag = pMbROTMDT ;
     return GetIdentifierForMbIDTagMDT(pMbIDTag,isValid);
   }else{
     const MbROTRPC* pMbROTRPC = dynamic_cast<const MbROTRPC*> (pMbROT);
     if (pMbROTRPC) {
       const MbIDTagRPC* pMbIDTag = pMbROTRPC ;
       return GetIdentifierForMbIDTagRPC(pMbIDTag,isValid);
     }else{
       const MbROTTGC* pMbROTTGC = dynamic_cast<const MbROTTGC*> (pMbROT);
       if (pMbROTTGC) {
         const MbIDTagTGC* pMbIDTag = pMbROTTGC ;
         return GetIdentifierForMbIDTagTGC(pMbIDTag,isValid);
       }else{
         const MbROTCSC* pMbROTCSC = dynamic_cast<const MbROTCSC*> (pMbROT);
         if (pMbROTCSC) {
           const MbIDTagCSC* pMbIDTag = pMbROTCSC ;
           return GetIdentifierForMbIDTagCSC(pMbIDTag,isValid);
         }else{
           const MbROTCSCclu* pMbROTCSCclu = dynamic_cast<const MbROTCSCclu*> (pMbROT);
           if (pMbROTCSCclu) {
             const MbIDTagCSCclu* pMbIDTag = pMbROTCSCclu ;
             return GetIdentifierForMbIDTagCSCclu(pMbIDTag,isValid);
           }else{
             ATH_MSG_INFO( "GetIdentifierForMbIOObject called with a pMbIOObject  which is a pMbPOT AND a pMbROT BUT which is UNCASTABLE"  
                            << " for GetType() "<< pMbIOObject->GetType() 
                            << " and GetTech() "<< pMbIOObject->GetTech()
                            );
             Identifier aIdentifier; return aIdentifier ;
           }
         }
       }
     }
   }
 }

 const MbHole* pMbHole = dynamic_cast<const MbHole*> (pMbPOT);
 if (pMbHole) {
   const MbHoleMDT* pMbHoleMDT = dynamic_cast<const MbHoleMDT*> (pMbHole);
   if (pMbHoleMDT) {
     const MbIDTagMDT* pMbIDTag = pMbHoleMDT ;
     return GetIdentifierForMbIDTagMDT(pMbIDTag,isValid);
   }else{
     const MbHoleRPC* pMbHoleRPC = dynamic_cast<const MbHoleRPC*> (pMbHole);
     if (pMbHoleRPC) {
       const MbIDTagRPC* pMbIDTag = pMbHoleRPC ;
       return GetIdentifierForMbIDTagRPC(pMbIDTag,isValid);
     }else{
       const MbHoleTGC* pMbHoleTGC = dynamic_cast<const MbHoleTGC*> (pMbHole);
       if (pMbHoleTGC) {
         const MbIDTagTGC* pMbIDTag = pMbHoleTGC ;
         return GetIdentifierForMbIDTagTGC(pMbIDTag,isValid);
       }else{
         const MbHoleCSC* pMbHoleCSC = dynamic_cast<const MbHoleCSC*> (pMbHole);
         if (pMbHoleCSC) {
           const MbIDTagCSC* pMbIDTag = pMbHoleCSC ;
           return GetIdentifierForMbIDTagCSC(pMbIDTag,isValid);
         }else{
           const MbHoleCSCclu* pMbHoleCSCclu = dynamic_cast<const MbHoleCSCclu*> (pMbHole);
           if (pMbHoleCSCclu) {
             const MbIDTagCSCclu* pMbIDTag = pMbHoleCSCclu ;
             return GetIdentifierForMbIDTagCSCclu(pMbIDTag,isValid);
           }else{
             ATH_MSG_INFO( "GetIdentifierForMbIOObject called with a pMbIOObject which is a pMbPOT AND a pMbHole BUT which is UNCASTABLE"  
                            << " for GetType() "<< pMbIOObject->GetType() 
                            << " and GetTech() "<< pMbIOObject->GetTech()
                            );
             Identifier aIdentifier; return aIdentifier ;
           }
         }
       }
     }
   }
 }

 ATH_MSG_INFO( "GetIdentifierForMbIOObject called with a  pMbIOObject which is a pMbPOT BUT NOT a pMbROT AND NOT a pMbHole "  
                << " for GetType() "<< pMbIOObject->GetType() 
                << " and GetTech() "<< pMbIOObject->GetTech()
                );
 Identifier aIdentifier; return aIdentifier ;

}

Identifier MuonboyToIdentifier::GetIdentifierForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag, bool& isValid )
{

  isValid = false ;  

  if (!pMbIDTag) {
     ATH_MSG_INFO( "GetIdentifierForMbIDTagMDT called with null pMbIDTag "
                    );
     Identifier aIdentifier; return aIdentifier ;
  }
 
  std::string SGStationName   = pMbIDTag->GetSGStationName  () ;
  int         SGStationEta    = pMbIDTag->GetSGStationEta   () ;
  int         SGStationPhi    = pMbIDTag->GetSGStationPhi   () ;
  
  int         SGMultilayer    = pMbIDTag->GetSGMultilayer   () ;
  int         SGTubeLayer     = pMbIDTag->GetSGTubeLayer    () ;
  int         SGTube          = pMbIDTag->GetSGTube         () ;

  bool check = true;
  Identifier aIdentifier = m_mdtId->channelID(SGStationName,
                                              SGStationEta,SGStationPhi,
                                              SGMultilayer,SGTubeLayer,SGTube,
                                              check, &isValid );
  if (!isValid){
    ATH_MSG_INFO( "GetIdentifierForMbIDTagMDT gets false isValid "
                   );
  }                                  
  return aIdentifier ;

}
Identifier MuonboyToIdentifier::GetIdentifierForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag, bool& isValid )
{

  isValid = false ;  

  if (!pMbIDTag) {
     ATH_MSG_INFO( "GetIdentifierForMbIDTagRPC called with null pMbIDTag "
                    );
     Identifier aIdentifier; return aIdentifier ;
  }
 
  std::string SGStationName   = pMbIDTag->GetSGStationName  () ;
  int         SGStationEta    = pMbIDTag->GetSGStationEta   () ;
  int         SGStationPhi    = pMbIDTag->GetSGStationPhi   () ;
  
  int         SGDoubletR      = pMbIDTag->GetSGDoubletR    () ;
  int         SGDoubletZ      = pMbIDTag->GetSGDoubletZ    () ;
  int         SGDoubletPhi    = pMbIDTag->GetSGDoubletPhi  () ;
  int         SGGasGap        = pMbIDTag->GetSGGasGap      () ;
  int         SGMeasuresPhi   = pMbIDTag->GetSGMeasuresPhi () ;
  int         SGStrip         = pMbIDTag->GetSGStrip       () ;
  
  bool check = true;
  Identifier aIdentifier = m_rpcId->channelID(SGStationName,
                                              SGStationEta,SGStationPhi,
                                              SGDoubletR, SGDoubletZ, SGDoubletPhi,
                                              SGGasGap, SGMeasuresPhi, SGStrip,
                                              check, &isValid );
  if (!isValid){
    ATH_MSG_INFO( "GetIdentifierForMbIDTagRPC gets false isValid "
                   );
  }                                  
  return aIdentifier ;

}
Identifier MuonboyToIdentifier::GetIdentifierForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag, bool& isValid )
{

  isValid = false ;  

  if (!pMbIDTag) {
     ATH_MSG_INFO( "GetIdentifierForMbIDTagTGC called with null pMbIDTag "
                    );
     Identifier aIdentifier; return aIdentifier ;
  }
 
  std::string SGStationName   = pMbIDTag->GetSGStationName  () ;
  int         SGStationEta    = pMbIDTag->GetSGStationEta   () ;
  int         SGStationPhi    = pMbIDTag->GetSGStationPhi   () ;
  
  int         SGGasGap        = pMbIDTag->GetSGGasGap      () ;
  int         SGIsStrip       = pMbIDTag->GetSGIsStrip     () ;
  int         SGChannel       = pMbIDTag->GetSGChannel     () ;

  bool check = true;
  Identifier aIdentifier = m_tgcId->channelID(SGStationName,
                                              SGStationEta,SGStationPhi,
                                              SGGasGap, SGIsStrip, SGChannel,
                                              check, &isValid );
  if (!isValid){
    ATH_MSG_INFO( "GetIdentifierForMbIDTagTGC gets false isValid "
                   );
  }                                  
  return aIdentifier ;

}
Identifier MuonboyToIdentifier::GetIdentifierForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag, bool& isValid )
{

  isValid = false ;  

  if (!pMbIDTag) {
     ATH_MSG_INFO( "GetIdentifierForMbIDTagCSC called with null pMbIDTag "
                    );
     Identifier aIdentifier; return aIdentifier ;
  }
 
  std::string SGStationName   = pMbIDTag->GetSGStationName  () ;
  int         SGStationEta    = pMbIDTag->GetSGStationEta   () ;
  int         SGStationPhi    = pMbIDTag->GetSGStationPhi   () ;

  int         SGChamberLayer  = pMbIDTag->GetSGChamberLayer () ;
  int         SGWireLayer     = pMbIDTag->GetSGWireLayer    () ;
  int         SGMeasuresPhi   = pMbIDTag->GetSGMeasuresPhi  () ;
  int         SGStrip         = pMbIDTag->GetSGStrip        () ;

  bool check = true;
  Identifier aIdentifier = m_cscId->channelID(SGStationName,
                                              SGStationEta,SGStationPhi,
                                              SGChamberLayer, SGWireLayer, SGMeasuresPhi,SGStrip,
                                              check, &isValid );
  if (!isValid){
    ATH_MSG_INFO( "GetIdentifierForMbIDTagCSC gets false isValid "
                   );
  }                                  
  return aIdentifier ;

}
Identifier MuonboyToIdentifier::GetIdentifierForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag, bool& isValid )
{

  isValid = false ;  

  if (!pMbIDTag) {
     ATH_MSG_INFO( "GetIdentifierForMbIDTagCSCclu called with null pMbIDTag "
                    );
     Identifier aIdentifier; return aIdentifier ;
  }
 
  std::string SGStationName   = pMbIDTag->GetSGStationName  () ;
  int         SGStationEta    = pMbIDTag->GetSGStationEta   () ;
  int         SGStationPhi    = pMbIDTag->GetSGStationPhi   () ;

  int         SGChamberLayer  = pMbIDTag->GetSGChamberLayer () ;
  int         SGWireLayer     = pMbIDTag->GetSGWireLayer    () ;
  int         SGMeasuresPhi   = pMbIDTag->GetSGMeasuresPhi  () ;
  int         SGStrip         = pMbIDTag->GetSGStrip        () ;

  bool check = true;
  Identifier aIdentifier = m_cscId->channelID(SGStationName,
                                              SGStationEta,SGStationPhi,
                                              SGChamberLayer, SGWireLayer, SGMeasuresPhi,SGStrip,
                                              check, &isValid );
  if (!isValid){
    ATH_MSG_INFO( "GetIdentifierForMbIDTagCSC gets false isValid "
                   );
  }                                  
  return aIdentifier ;

}
