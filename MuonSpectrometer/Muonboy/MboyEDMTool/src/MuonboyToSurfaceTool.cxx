/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkSurfaces/Surface.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToSurfaceTool.h"

//----------------------------------------------------------------//
#include "MboyCoreEvt/MbIOObject.h"
#include "MboyCoreEvt/MbIDTagMDT.h"
#include "MboyCoreEvt/MbIDTagRPC.h"
#include "MboyCoreEvt/MbIDTagTGC.h"
#include "MboyCoreEvt/MbIDTagCSC.h"
#include "MboyCoreEvt/MbIDTagCSCclu.h"

#include "MboyEDMToolInterfaces/IMuonboyToIdentifier.h"

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

//----------------------------------------------------------------//
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
// #include "MuonGeoModel/MdtReadoutElement.h"
// #include "MuonGeoModel/RpcReadoutElement.h"
// #include "MuonGeoModel/TgcReadoutElement.h"
// #include "MuonGeoModel/CscReadoutElement.h"

Trk::MuonboyToSurfaceTool::MuonboyToSurfaceTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p) ,
 p_IMuonboyToIdentifier          ( "MuonboyToIdentifier" ) 
{

  declareInterface<IMuonboyToSurfaceTool>(this);

  declareProperty("IMuonboyToIdentifier"          , p_IMuonboyToIdentifier ) ;

}

Trk::MuonboyToSurfaceTool::~MuonboyToSurfaceTool(){}

// Initialize
StatusCode Trk::MuonboyToSurfaceTool::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" );
    return( StatusCode::FAILURE );
  }

//Retrieve p_IMuonboyToIdentifier
  if ( p_IMuonboyToIdentifier.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_IMuonboyToIdentifier );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_IMuonboyToIdentifier );

//Retrieve p_MuonDetectorManager
  if ( detStore()->retrieve(p_MuonDetectorManager).isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve MuonDetectorManager " );
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved MuonDetectorManager " );


  ATH_MSG_INFO(  "Initialisation ended     " );

  return StatusCode::SUCCESS;

}

StatusCode Trk::MuonboyToSurfaceTool::finalize(){return StatusCode::SUCCESS;}


const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIOObject   (const  MbIOObject*    pMbIOObject )
{
 
 if (!pMbIOObject) {
    ATH_MSG_INFO(  "GetSurfaceForMbIOObject called with null pMbIOObject "
                   );
    return 0 ;
 }

 const MbPOT* pMbPOT = dynamic_cast<const MbPOT*> (pMbIOObject);
 if (!pMbPOT) {
    ATH_MSG_INFO(  "GetSurfaceForMbIOObject called with a pMbIOObject which is NOT a pMbPOT "
                   );
    return 0 ;
 }

 const MbROT* pMbROT = dynamic_cast<const MbROT*> (pMbPOT);
 if (pMbROT) {
   const MbROTMDT* pMbROTMDT = dynamic_cast<const MbROTMDT*> (pMbROT);
   if (pMbROTMDT) {
     const MbIDTagMDT* pMbIDTag = pMbROTMDT ;
     return GetSurfaceForMbIDTagMDT(pMbIDTag);
   }else{
     const MbROTRPC* pMbROTRPC = dynamic_cast<const MbROTRPC*> (pMbROT);
     if (pMbROTRPC) {
       const MbIDTagRPC* pMbIDTag = pMbROTRPC ;
       return GetSurfaceForMbIDTagRPC(pMbIDTag);
     }else{
       const MbROTTGC* pMbROTTGC = dynamic_cast<const MbROTTGC*> (pMbROT);
       if (pMbROTTGC) {
         const MbIDTagTGC* pMbIDTag = pMbROTTGC ;
         return GetSurfaceForMbIDTagTGC(pMbIDTag);
       }else{
         const MbROTCSC* pMbROTCSC = dynamic_cast<const MbROTCSC*> (pMbROT);
         if (pMbROTCSC) {
           const MbIDTagCSC* pMbIDTag = pMbROTCSC ;
           return GetSurfaceForMbIDTagCSC(pMbIDTag);
         }else{
           const MbROTCSCclu* pMbROTCSCclu = dynamic_cast<const MbROTCSCclu*> (pMbROT);
           if (pMbROTCSCclu) {
             const MbIDTagCSCclu* pMbIDTag = pMbROTCSCclu ;
             return GetSurfaceForMbIDTagCSCclu(pMbIDTag);
           }else{
             ATH_MSG_INFO(  "GetSurfaceForMbIOObject called with a pMbIOObject  which is a pMbPOT AND a pMbROT BUT which is UNCASTABLE"  
                            << " for GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech()
                            );
             return 0 ;
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
     return GetSurfaceForMbIDTagMDT(pMbIDTag);
   }else{
     const MbHoleRPC* pMbHoleRPC = dynamic_cast<const MbHoleRPC*> (pMbHole);
     if (pMbHoleRPC) {
       const MbIDTagRPC* pMbIDTag = pMbHoleRPC ;
       return GetSurfaceForMbIDTagRPC(pMbIDTag);
     }else{
       const MbHoleTGC* pMbHoleTGC = dynamic_cast<const MbHoleTGC*> (pMbHole);
       if (pMbHoleTGC) {
         const MbIDTagTGC* pMbIDTag = pMbHoleTGC ;
         return GetSurfaceForMbIDTagTGC(pMbIDTag);
       }else{
         const MbHoleCSC* pMbHoleCSC = dynamic_cast<const MbHoleCSC*> (pMbHole);
         if (pMbHoleCSC) {
           const MbIDTagCSC* pMbIDTag = pMbHoleCSC ;
           return GetSurfaceForMbIDTagCSC(pMbIDTag);
         }else{
           const MbHoleCSCclu* pMbHoleCSCclu = dynamic_cast<const MbHoleCSCclu*> (pMbHole);
           if (pMbHoleCSCclu) {
             const MbIDTagCSCclu* pMbIDTag = pMbHoleCSCclu ;
             return GetSurfaceForMbIDTagCSCclu(pMbIDTag);
           }else{
             ATH_MSG_INFO(  "GetSurfaceForMbIOObject called with a pMbIOObject which is a pMbPOT AND a pMbHole BUT which is UNCASTABLE"  
                            << " for GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech()
                            );
             return 0 ;
           }
         }
       }
     }
   }
 }

 ATH_MSG_INFO(  "GetSurfaceForMbIOObject called with a  pMbIOObject which is a pMbPOT BUT NOT a pMbROT AND NOT a pMbHole "  
                << " for GetType() "<< pMbIOObject->GetType() << " and GetTech() "<< pMbIOObject->GetTech()
                );
 return 0 ;

}

const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIDTagMDT   (const  MbIDTagMDT*    pMbIDTag )
{

  if (!pMbIDTag) {
     ATH_MSG_INFO(  "GetSurfaceForMbIDTagMDT called with null pMbIDTag "
                    );
     return 0 ;
  }
 
  bool isValid ;
  Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIDTagMDT(pMbIDTag,isValid);
  
  if (isValid){
    const MuonGM::MdtReadoutElement* pMdtReadoutElement = p_MuonDetectorManager->getMdtReadoutElement(aIdentifier); 
    if (pMdtReadoutElement) {
      if (pMdtReadoutElement->containsId(aIdentifier)){
        return &( pMdtReadoutElement->surface(aIdentifier)   ) ;
      }else{
        ATH_MSG_INFO(  "GetSurfaceForMbIDTagMDT gets false  containsId "
                       << " => No  Surface computed "
                       );
      }
    }else{
      ATH_MSG_INFO(  "GetSurfaceForMbIDTagMDT gets null pMdtReadoutElement "
                     << " => No  Surface computed "
                     );
    }
  }else{
    ATH_MSG_INFO(  "GetSurfaceForMbIDTagMDT gets false isValid "
                   << " => No  Surface computed "
                   );
  }                                  
                                    
  ATH_MSG_INFO(  "GetSurfaceForMbIDTagMDT gets problem with ");
  ATH_MSG_INFO(  " SGStationName " << pMbIDTag->GetSGStationName  () );
  ATH_MSG_INFO(  " SGStationEta  " << pMbIDTag->GetSGStationEta   () );
  ATH_MSG_INFO(  " SGStationPhi  " << pMbIDTag->GetSGStationPhi   () );
  ATH_MSG_INFO(  " SGMultilayer  " << pMbIDTag->GetSGMultilayer   () );
  ATH_MSG_INFO(  " SGTubeLayer   " << pMbIDTag->GetSGTubeLayer    () );
  ATH_MSG_INFO(  " SGTube        " << pMbIDTag->GetSGTube         () );

  return 0 ;

}
const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIDTagRPC   (const  MbIDTagRPC*    pMbIDTag )
{

  if (!pMbIDTag) {
     ATH_MSG_INFO(  "GetSurfaceForMbIDTagRPC called with null pMbIDTag "
                    );
     return 0 ;
  }
 
  bool isValid ;
  Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIDTagRPC(pMbIDTag,isValid);
  
  if (isValid){
    const MuonGM::RpcReadoutElement* pRpcReadoutElement = p_MuonDetectorManager->getRpcReadoutElement(aIdentifier); 
    if (pRpcReadoutElement) {
      if (pRpcReadoutElement->containsId(aIdentifier)){
        return &( pRpcReadoutElement->surface(aIdentifier)   ) ;
      }else{
        ATH_MSG_INFO(  "GetSurfaceForMbIDTagRPC gets false  containsId "
                       << " => No  Surface computed "
                       );
      }
    }else{
      ATH_MSG_INFO(  "GetSurfaceForMbIDTagRPC gets null pRpcReadoutElement "
                     << " => No  Surface computed "
                     );
    }
  }else{
    ATH_MSG_INFO(  "GetSurfaceForMbIDTagRPC gets false isValid "
                   << " => No  Surface computed "
                   );
  }                                  

  ATH_MSG_INFO(  "GetSurfaceForMbIDTagRPC gets problem with ");
  ATH_MSG_INFO(  " SGStationName " << pMbIDTag->GetSGStationName () );
  ATH_MSG_INFO(  " SGStationEta  " << pMbIDTag->GetSGStationEta  () );
  ATH_MSG_INFO(  " SGStationPhi  " << pMbIDTag->GetSGStationPhi  () );
  ATH_MSG_INFO(  " SGDoubletR    " << pMbIDTag->GetSGDoubletR    () );
  ATH_MSG_INFO(  " SGDoubletZ    " << pMbIDTag->GetSGDoubletZ    () );
  ATH_MSG_INFO(  " SGDoubletPhi  " << pMbIDTag->GetSGDoubletPhi  () );
  ATH_MSG_INFO(  " SGGasGap      " << pMbIDTag->GetSGGasGap      () );
  ATH_MSG_INFO(  " SGMeasuresPhi " << pMbIDTag->GetSGMeasuresPhi () );
  ATH_MSG_INFO(  " SGStrip       " << pMbIDTag->GetSGStrip       () );

  return 0 ;

}
const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIDTagTGC   (const  MbIDTagTGC*    pMbIDTag )
{

  if (!pMbIDTag) {
     ATH_MSG_INFO(  "GetSurfaceForMbIDTagTGC called with null pMbIDTag "
                    );
     return 0 ;
  }
 
  bool isValid ;
  Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIDTagTGC(pMbIDTag,isValid);
  
  if (isValid){
    const MuonGM::TgcReadoutElement* pTgcReadoutElement = p_MuonDetectorManager->getTgcReadoutElement(aIdentifier); 
    if (pTgcReadoutElement) {
      if (pTgcReadoutElement->containsId(aIdentifier)){
        return &( pTgcReadoutElement->surface(aIdentifier)   ) ;
      }else{
        ATH_MSG_INFO(  "GetSurfaceForMbIDTagTGC gets false  containsId "
                       << " => No  Surface computed "
                       );
      }
    }else{
      ATH_MSG_INFO(  "GetSurfaceForMbIDTagTGC gets null pTgcReadoutElement "
                     << " => No  Surface computed "
                     );
    }
  }else{
    ATH_MSG_INFO(  "GetSurfaceForMbIDTagTGC gets false isValid "
                   << " => No  Surface computed "
                   );
  }                                  

  ATH_MSG_INFO(  "GetSurfaceForMbIDTagTGC gets problem with ");
  ATH_MSG_INFO(  " SGStationName " << pMbIDTag->GetSGStationName  () );
  ATH_MSG_INFO(  " SGStationEta  " << pMbIDTag->GetSGStationEta   () );
  ATH_MSG_INFO(  " SGStationPhi  " << pMbIDTag->GetSGStationPhi   () );
  ATH_MSG_INFO(  " SGGasGap      " << pMbIDTag->GetSGGasGap       () );
  ATH_MSG_INFO(  " SGIsStrip     " << pMbIDTag->GetSGIsStrip      () );
  ATH_MSG_INFO(  " SGChannel     " << pMbIDTag->GetSGChannel      () );

  return 0 ;

}
const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIDTagCSC   (const  MbIDTagCSC*    pMbIDTag )
{

  if (!pMbIDTag) {
     ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSC called with null pMbIDTag "
                    );
     return 0 ;
  }
 
  bool isValid ;
  Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIDTagCSC(pMbIDTag,isValid);
  
  if (isValid){
    const MuonGM::CscReadoutElement* pCscReadoutElement = p_MuonDetectorManager->getCscReadoutElement(aIdentifier); 
    if (pCscReadoutElement) {
      if (pCscReadoutElement->containsId(aIdentifier)){
        return &( pCscReadoutElement->surface(aIdentifier)   ) ;
      }else{
        ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSC gets false  containsId "
                       << " => No  Surface computed "
                       );
      }
    }else{
      ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSC gets null pCscReadoutElement "
                     << " => No  Surface computed "
                     );
    }                 
    
  }else{
    ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSC gets false isValid "
                   << " => No  Surface computed "
                   );
  }                                  

  ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSC gets problem with ");
  ATH_MSG_INFO(  " SGStationName  " << pMbIDTag->GetSGStationName  () );
  ATH_MSG_INFO(  " SGStationEta   " << pMbIDTag->GetSGStationEta   () );
  ATH_MSG_INFO(  " SGStationPhi   " << pMbIDTag->GetSGStationPhi   () );
  ATH_MSG_INFO(  " SGChamberLayer " << pMbIDTag->GetSGChamberLayer () );
  ATH_MSG_INFO(  " SGWireLayer    " << pMbIDTag->GetSGWireLayer    () );
  ATH_MSG_INFO(  " SGMeasuresPhi  " << pMbIDTag->GetSGMeasuresPhi  () );
  ATH_MSG_INFO(  " SGStrip        " << pMbIDTag->GetSGStrip        () );

  return 0 ;

}
const Trk::Surface* Trk::MuonboyToSurfaceTool::GetSurfaceForMbIDTagCSCclu(const  MbIDTagCSCclu* pMbIDTag )
{

  if (!pMbIDTag) {
     ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSCclu called with null pMbIDTag "
                    );
     return 0 ;
  }
 
  bool isValid ;
  Identifier aIdentifier =p_IMuonboyToIdentifier->GetIdentifierForMbIDTagCSCclu(pMbIDTag,isValid);
  
  if (isValid){
    const MuonGM::CscReadoutElement* pCscReadoutElement = p_MuonDetectorManager->getCscReadoutElement(aIdentifier); 
    if (pCscReadoutElement) {
      if (pCscReadoutElement->containsId(aIdentifier)){
        return &( pCscReadoutElement->surface(aIdentifier)   ) ;
      }else{
        ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSCclu gets false  containsId "
                       << " => No  Surface computed "
                       );
      }
    }else{
      ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSCclu gets null pCscReadoutElement "
                     << " => No  Surface computed "
                     );
    }
  }else{
    ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSCclu gets false isValid "
                   << " => No  Surface computed "
                   );
  }                                  

  ATH_MSG_INFO(  "GetSurfaceForMbIDTagCSCclu gets problem with ");
  ATH_MSG_INFO(  " SGStationName  " << pMbIDTag->GetSGStationName  () );
  ATH_MSG_INFO(  " SGStationEta   " << pMbIDTag->GetSGStationEta   () );
  ATH_MSG_INFO(  " SGStationPhi   " << pMbIDTag->GetSGStationPhi   () );
  ATH_MSG_INFO(  " SGChamberLayer " << pMbIDTag->GetSGChamberLayer () );
  ATH_MSG_INFO(  " SGWireLayer    " << pMbIDTag->GetSGWireLayer    () );
  ATH_MSG_INFO(  " SGMeasuresPhi  " << pMbIDTag->GetSGMeasuresPhi  () );
  ATH_MSG_INFO(  " SGStrip        " << pMbIDTag->GetSGStrip        () );

  return 0 ;

}
