/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MboyNativeHelper.h"

//----------------------------------------------------------------//
#include "MboyAthEvt/MB_SegmentAth.h"
#include "MboyCoreEvt/MbROTMDT.h"
#include "MboyCoreEvt/MbROTRPC.h"
#include "MboyCoreEvt/MbROTTGC.h"
#include "MboyCoreEvt/MbROTCSC.h"
#include "MboyCoreEvt/MbROTCSCclu.h"
#include "MboyCoreEvt/MbHoleMDT.h"
#include "MboyCoreEvt/MbHoleRPC.h"
#include "MboyCoreEvt/MbHoleTGC.h"
#include "MboyCoreEvt/MbHoleCSC.h"
#include "MboyCoreEvt/MbHoleCSCclu.h"

MboyNativeHelper::MboyNativeHelper(const std::string& t, 
                                   const std::string& n,
                                   const IInterface*  p ):AthAlgTool(t,n,p)
{

  declareInterface<IMboyNativeHelper>(this);

}

MboyNativeHelper::~MboyNativeHelper(){}

// Initialize
StatusCode MboyNativeHelper::initialize(){

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

  ATH_MSG_INFO(  "Initialisation ended     " ) ;
  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MboyNativeHelper::finalize(){return StatusCode::SUCCESS;}

// Dumps MB_SegmentAth
StatusCode MboyNativeHelper::Dump_MB_SegmentAth(
 MB_SegmentAth* pSegment,
 std::ofstream* pOutCurrent){

  if ( IsValid(pSegment) == 0 ) return StatusCode::SUCCESS;

//Get Segment Point (Global coordinates)
  double X_pS, Y_pS , Z_pS ;
  GetPoint(pSegment,X_pS, Y_pS , Z_pS);
  Amg::Vector3D pS(X_pS, Y_pS , Z_pS);

//Get Segment Direction (Global coordinates)
  double X_dS, Y_dS , Z_dS ;
  GetDirection(pSegment,X_dS, Y_dS , Z_dS);
  Amg::Vector3D dS(X_dS, Y_dS , Z_dS);
  dS.normalize();

//Get dX vector (Global coordinates)
  double X_dX, Y_dX , Z_dX ;
  pSegment->Getdx(X_dX, Y_dX , Z_dX);
  Amg::Vector3D dX(X_dX, Y_dX , Z_dX);
  dX.normalize();

//Get dY vector (Global coordinates)
  double X_dY, Y_dY , Z_dY ;
  pSegment->Getdy(X_dY, Y_dY , Z_dY);
  Amg::Vector3D dY(X_dY, Y_dY , Z_dY);
  dY.normalize();

//Get dZ vector (Global coordinates)
  Amg::Vector3D dZ = dX.cross(dY);

//Get Segment Direction in local coordinates
  Amg::Vector3D LocdS(
                   dS.dot(dX),
                   dS.dot(dY),
                   dS.dot(dZ)
                   );

//Get Angles
//double ThealphaXZ = atan2(LocdS.z(),LocdS.x());
//double ThealphaYZ = atan2(LocdS.z(),LocdS.y());
   
  *pOutCurrent 
     << "Xaxis     " 
     << " " << std::setw(12)<<std::setprecision(5) << dX.x()
     << " " << std::setw(12)<<std::setprecision(5) << dX.y()
     << " " << std::setw(12)<<std::setprecision(5) << dX.z()
     <<std::endl;

  *pOutCurrent 
     << "Yaxis     " 
     << " " << std::setw(12)<<std::setprecision(5) << dY.x()
     << " " << std::setw(12)<<std::setprecision(5) << dY.y()
     << " " << std::setw(12)<<std::setprecision(5) << dY.z()
     <<std::endl;

  *pOutCurrent 
      << "Position  " 
      << " " << std::setw(12)<<std::setprecision(5) << pS.x()
      << " " << std::setw(12)<<std::setprecision(5) << pS.y()
      << " " << std::setw(12)<<std::setprecision(5) << pS.z()
      <<std::endl;

  *pOutCurrent 
      << "Direction " 
      << " " << std::setw(12)<<std::setprecision(5) << dS.x()
      << " " << std::setw(12)<<std::setprecision(5) << dS.y()
      << " " << std::setw(12)<<std::setprecision(5) << dS.z()
      <<std::endl;

  *pOutCurrent 
      << "Errors    " 
      <<std::endl;
  for (int Ind1=0; Ind1<4; Ind1++){
    *pOutCurrent 
        << " " << std::setw(12)<<std::setprecision(5) << pSegment->Wij(Ind1+1,1)
        << " " << std::setw(12)<<std::setprecision(5) << pSegment->Wij(Ind1+1,2)
        << " " << std::setw(12)<<std::setprecision(5) << pSegment->Wij(Ind1+1,3)
        << " " << std::setw(12)<<std::setprecision(5) << pSegment->Wij(Ind1+1,4)
        <<std::endl;
  }
  *pOutCurrent
       << "dx Axz mm/mrad         "
       << std::setw(12)<<std::setprecision(4)<< 1./sqrt(pSegment->Wij(1,1))
       << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(pSegment->Wij(2,2))
       << std::setw(12)<<std::setprecision(4)<< pSegment->Wij(1,2)/(sqrt(pSegment->Wij(1,1)*pSegment->Wij(2,2)))
       << std::endl;
  *pOutCurrent 
       << "dy Axz micron/microrad "
       << std::setw(12)<<std::setprecision(4)<< 1000./sqrt(pSegment->Wij(3,3))
       << std::setw(12)<<std::setprecision(4)<< 1000000./sqrt(pSegment->Wij(4,4))
       << std::setw(12)<<std::setprecision(4)<< pSegment->Wij(3,4)/(sqrt(pSegment->Wij(3,3)*pSegment->Wij(4,4)))
       << std::endl;

  return StatusCode::SUCCESS;

}
   
// Dumps elements
StatusCode MboyNativeHelper::Dump_MB_SegmentAthtElements(
 MB_SegmentAth* pSegment,
 std::ofstream* pOutCurrent){

  if ( IsValid(pSegment) == 0 ) return StatusCode::SUCCESS;
  StatusCode sc = StatusCode::SUCCESS;


  int NberOfEls=pSegment->GetNberOfElements(); 
  for (int ItemEls=0; ItemEls<NberOfEls; ItemEls++){
    const MbROTMDT*    pMbROTMDT     = pSegment->LiIdGetMbROTMDT    (ItemEls);
    const MbROTRPC*    pMbROTRPC     = pSegment->LiIdGetMbROTRPC    (ItemEls);
    const MbROTTGC*    pMbROTTGC     = pSegment->LiIdGetMbROTTGC    (ItemEls);
    const MbROTCSC*    pMbROTCSC     = pSegment->LiIdGetMbROTCSC    (ItemEls);
    const MbROTCSCclu* pMbROTCSCclu  = pSegment->LiIdGetMbROTCSCclu (ItemEls);
    const MbHoleMDT*      pMbHoleMDT       = pSegment->LiIdGetMbHoleMDT    (ItemEls);
    const MbHoleRPC*      pMbHoleRPC       = pSegment->LiIdGetMbHoleRPC    (ItemEls);
    const MbHoleTGC*      pMbHoleTGC       = pSegment->LiIdGetMbHoleTGC    (ItemEls);
    const MbHoleCSC*      pMbHoleCSC       = pSegment->LiIdGetMbHoleCSC    (ItemEls);
    const MbHoleCSCclu*   pMbHoleCSCclu    = pSegment->LiIdGetMbHoleCSCclu (ItemEls);
    if ( pMbROTMDT ) {
      sc = dumpMbROTMDT (pMbROTMDT   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbROTMDT failed" ) ;
    }
    if ( pMbROTRPC ) {
      sc = dumpMbROTRPC (pMbROTRPC   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbROTRPC failed" ) ;
    }
    if ( pMbROTTGC ) {
      sc = dumpMbROTTGC (pMbROTTGC   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbROTTGC failed" ) ;
    }
    if ( pMbROTCSC ) {
      sc = dumpMbROTCSC(pMbROTCSC,pOutCurrent);
      if (sc.isFailure())  ATH_MSG_WARNING( "dumpMbROTCSC failed" ) ;
    }
    if ( pMbROTCSCclu ) {
      sc = dumpMbROTCSCclu(pMbROTCSCclu,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbROTCSCclu failed" ) ;
    }

    if ( pMbHoleMDT ) {
      sc = dumpMbHolemdt (pMbHoleMDT   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbHolemdt failed" ) ;
    }
    if ( pMbHoleRPC ) {
      sc = dumpMbHolerpc (pMbHoleRPC   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbHolerpc failed" ) ;
    }
    if ( pMbHoleTGC ) {
      sc = dumpMbHoletgc (pMbHoleTGC   ,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbHoletgc failed" ) ;
    }
    if ( pMbHoleCSC ) {
      sc = dumpMbHolecsc(pMbHoleCSC,pOutCurrent);
      if (sc.isFailure())  ATH_MSG_WARNING( "dumpMbHolecsc failed" ) ;
    }
    if ( pMbHoleCSCclu ) {
      sc = dumpMbHolecscclu(pMbHoleCSCclu,pOutCurrent);
      if (sc.isFailure()) ATH_MSG_WARNING( "dumpMbHolecscclu failed" ) ;
    }
  }

  return StatusCode::SUCCESS;

}

// Dump Elements
StatusCode MboyNativeHelper::dumpMbROTMDT (const MbROTMDT*    pMbROTMDT   ,std::ofstream* pOutCurrent){

  if (pMbROTMDT){
    std::string SGStationName = pMbROTMDT->GetSGStationName  () ;
    int         SGStationEta  = pMbROTMDT->GetSGStationEta   () ;
    int         SGStationPhi  = pMbROTMDT->GetSGStationPhi   () ;
    int         SGMultilayer  = pMbROTMDT->GetSGMultilayer   () ;
    int         SGTubeLayer   = pMbROTMDT->GetSGTubeLayer    () ;
    int         SGTube        = pMbROTMDT->GetSGTube	    () ;
    *pOutCurrent 
      << std::setw(5)<< pMbROTMDT->GetType()
      << std::setw(5)<< pMbROTMDT->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGMultilayer
      << std::setw(4)<< SGTubeLayer
      << std::setw(4)<< SGTube
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbROTMDT pMbROTMDT = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbROTRPC (const MbROTRPC*    pMbROTRPC   ,std::ofstream* pOutCurrent){

  if (pMbROTRPC){
    std::string SGStationName  = pMbROTRPC->GetSGStationName() ;
    int         SGStationEta   = pMbROTRPC->GetSGStationEta () ;
    int         SGStationPhi   = pMbROTRPC->GetSGStationPhi () ;
    int         SGDoubletR     = pMbROTRPC->GetSGDoubletR   () ;
    int         SGDoubletZ     = pMbROTRPC->GetSGDoubletZ   () ;
    int         SGDoubletPhi   = pMbROTRPC->GetSGDoubletPhi () ;
    int         SGGasGap       = pMbROTRPC->GetSGGasGap     () ;
    int         SGMeasuresPhi  = pMbROTRPC->GetSGMeasuresPhi() ;
    int         SGStrip        = pMbROTRPC->GetSGStrip	   () ;
    *pOutCurrent 
      << std::setw(5)<< pMbROTRPC->GetType()
      << std::setw(5)<< pMbROTRPC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGDoubletR
      << std::setw(4)<< SGDoubletZ
      << std::setw(4)<< SGDoubletPhi
      << std::setw(4)<< SGGasGap
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbROTRPC pMbROTRPC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbROTTGC (const MbROTTGC*    pMbROTTGC   ,std::ofstream* pOutCurrent){

  if (pMbROTTGC){
    std::string SGStationName =  pMbROTTGC->GetSGStationName();
    int         SGStationEta  =  pMbROTTGC->GetSGStationEta ();
    int         SGStationPhi  =  pMbROTTGC->GetSGStationPhi ();
    int         SGGasGap      =  pMbROTTGC->GetSGGasGap     ();
    int         SGIsStrip     =  pMbROTTGC->GetSGIsStrip    ();
    int         SGChannel     =  pMbROTTGC->GetSGChannel    ();
    *pOutCurrent 
      << std::setw(5)<< pMbROTTGC->GetType()
      << std::setw(5)<< pMbROTTGC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGGasGap
      << std::setw(4)<< SGIsStrip
      << std::setw(4)<< SGChannel
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbROTTGC pMbROTTGC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbROTCSC(const MbROTCSC* pMbROTCSC,std::ofstream* pOutCurrent){

  if (pMbROTCSC){
    std::string SGStationName = pMbROTCSC->GetSGStationName ();
    int         SGStationEta  = pMbROTCSC->GetSGStationEta  ();
    int         SGStationPhi  = pMbROTCSC->GetSGStationPhi  ();
    int         SGChamberLayer= pMbROTCSC->GetSGChamberLayer();
    int         SGWireLayer   = pMbROTCSC->GetSGWireLayer   ();
    int         SGMeasuresPhi = pMbROTCSC->GetSGMeasuresPhi ();
    int         SGStrip       = pMbROTCSC->GetSGStrip       ();
    *pOutCurrent 
      << std::setw(5)<< pMbROTCSC->GetType()
      << std::setw(5)<< pMbROTCSC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGChamberLayer
      << std::setw(4)<< SGWireLayer
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbROTCSC pMbROTCSC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbROTCSCclu (const MbROTCSCclu* pMbROTCSCclu ,std::ofstream* pOutCurrent){

  if (pMbROTCSCclu){
    std::string SGStationName = pMbROTCSCclu->GetSGStationName ();
    int         SGStationEta  = pMbROTCSCclu->GetSGStationEta  ();
    int         SGStationPhi  = pMbROTCSCclu->GetSGStationPhi  ();
    int         SGChamberLayer= pMbROTCSCclu->GetSGChamberLayer();
    int         SGWireLayer   = pMbROTCSCclu->GetSGWireLayer   ();
    int         SGMeasuresPhi = pMbROTCSCclu->GetSGMeasuresPhi ();
    int         SGStrip       = pMbROTCSCclu->GetSGStrip       ();
    *pOutCurrent 
      << std::setw(5)<< pMbROTCSCclu->GetType()
      << std::setw(5)<< pMbROTCSCclu->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGChamberLayer
      << std::setw(4)<< SGWireLayer
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbROTCSCclu pMbROTCSCclu = 0" ) ;
  }

  return StatusCode::SUCCESS;
}
StatusCode MboyNativeHelper::dumpMbHolemdt (const MbHoleMDT*    pMbHoleMDT   ,std::ofstream* pOutCurrent){

  if (pMbHoleMDT){
    std::string SGStationName = pMbHoleMDT->GetSGStationName  () ;
    int         SGStationEta  = pMbHoleMDT->GetSGStationEta   () ;
    int         SGStationPhi  = pMbHoleMDT->GetSGStationPhi   () ;
    int         SGMultilayer  = pMbHoleMDT->GetSGMultilayer   () ;
    int         SGTubeLayer   = pMbHoleMDT->GetSGTubeLayer    () ;
    int         SGTube        = pMbHoleMDT->GetSGTube	    () ;
    *pOutCurrent 
      << std::setw(5)<< pMbHoleMDT->GetType()
      << std::setw(5)<< pMbHoleMDT->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGMultilayer
      << std::setw(4)<< SGTubeLayer
      << std::setw(4)<< SGTube
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbHolemdt pMbHoleMDT = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbHolerpc (const MbHoleRPC*    pMbHoleRPC   ,std::ofstream* pOutCurrent){

  if (pMbHoleRPC){
    std::string SGStationName  = pMbHoleRPC->GetSGStationName() ;
    int         SGStationEta   = pMbHoleRPC->GetSGStationEta () ;
    int         SGStationPhi   = pMbHoleRPC->GetSGStationPhi () ;
    int         SGDoubletR     = pMbHoleRPC->GetSGDoubletR   () ;
    int         SGDoubletZ     = pMbHoleRPC->GetSGDoubletZ   () ;
    int         SGDoubletPhi   = pMbHoleRPC->GetSGDoubletPhi () ;
    int         SGGasGap       = pMbHoleRPC->GetSGGasGap     () ;
    int         SGMeasuresPhi  = pMbHoleRPC->GetSGMeasuresPhi() ;
    int         SGStrip        = pMbHoleRPC->GetSGStrip	   () ;
    *pOutCurrent 
      << std::setw(5)<< pMbHoleRPC->GetType()
      << std::setw(5)<< pMbHoleRPC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGDoubletR
      << std::setw(4)<< SGDoubletZ
      << std::setw(4)<< SGDoubletPhi
      << std::setw(4)<< SGGasGap
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbHolerpc pMbHoleRPC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbHoletgc (const MbHoleTGC*    pMbHoleTGC   ,std::ofstream* pOutCurrent){

  if (pMbHoleTGC){
    std::string SGStationName =  pMbHoleTGC->GetSGStationName();
    int         SGStationEta  =  pMbHoleTGC->GetSGStationEta ();
    int         SGStationPhi  =  pMbHoleTGC->GetSGStationPhi ();
    int         SGGasGap      =  pMbHoleTGC->GetSGGasGap     ();
    int         SGIsStrip     =  pMbHoleTGC->GetSGIsStrip    ();
    int         SGChannel     =  pMbHoleTGC->GetSGChannel    ();
    *pOutCurrent 
      << std::setw(5)<< pMbHoleTGC->GetType()
      << std::setw(5)<< pMbHoleTGC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGGasGap
      << std::setw(4)<< SGIsStrip
      << std::setw(4)<< SGChannel
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbHoletgc pMbHoleTGC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbHolecsc(const MbHoleCSC* pMbHoleCSC,std::ofstream* pOutCurrent){

  if (pMbHoleCSC){
    std::string SGStationName = pMbHoleCSC->GetSGStationName ();
    int         SGStationEta  = pMbHoleCSC->GetSGStationEta  ();
    int         SGStationPhi  = pMbHoleCSC->GetSGStationPhi  ();
    int         SGChamberLayer= pMbHoleCSC->GetSGChamberLayer();
    int         SGWireLayer   = pMbHoleCSC->GetSGWireLayer   ();
    int         SGMeasuresPhi = pMbHoleCSC->GetSGMeasuresPhi ();
    int         SGStrip       = pMbHoleCSC->GetSGStrip       ();
    *pOutCurrent 
      << std::setw(5)<< pMbHoleCSC->GetType()
      << std::setw(5)<< pMbHoleCSC->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGChamberLayer
      << std::setw(4)<< SGWireLayer
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbHolecsc pMbHoleCSC = 0" ) ;
  }

  return StatusCode::SUCCESS;

}
StatusCode MboyNativeHelper::dumpMbHolecscclu (const MbHoleCSCclu* pMbHoleCSCclu ,std::ofstream* pOutCurrent){

  if (pMbHoleCSCclu){
    std::string SGStationName = pMbHoleCSCclu->GetSGStationName ();
    int         SGStationEta  = pMbHoleCSCclu->GetSGStationEta  ();
    int         SGStationPhi  = pMbHoleCSCclu->GetSGStationPhi  ();
    int         SGChamberLayer= pMbHoleCSCclu->GetSGChamberLayer();
    int         SGWireLayer   = pMbHoleCSCclu->GetSGWireLayer   ();
    int         SGMeasuresPhi = pMbHoleCSCclu->GetSGMeasuresPhi ();
    int         SGStrip       = pMbHoleCSCclu->GetSGStrip       ();
    *pOutCurrent 
      << std::setw(5)<< pMbHoleCSCclu->GetType()
      << std::setw(5)<< pMbHoleCSCclu->GetTech()
      << "  "
      << std::setw(3)<< SGStationName
      << std::setw(4)<< SGStationEta
      << std::setw(4)<< SGStationPhi
      << std::setw(4)<< SGChamberLayer
      << std::setw(4)<< SGWireLayer
      << std::setw(4)<< SGMeasuresPhi
      << std::setw(4)<< SGStrip
      << std::endl;
  }else{
    ATH_MSG_INFO(  "dumpMbHolecscclu pMbHoleCSCclu = 0" ) ;
  }

  return StatusCode::SUCCESS;
}

// Segment Associated Station Name
std::string MboyNativeHelper::SegmentAssociatedStationName( MB_SegmentAth* pSegment){
  std::string ToBeReturned = "XXX" ;
  if ( IsValid(pSegment) == 0 ) return ToBeReturned;
  return pSegment->GetStationName() ;
}
   
// Reconstructed Objects Validity
int MboyNativeHelper::IsValid( MB_SegmentAth* pSegment){
  if ( pSegment == 0 ) return 0 ;
//If there is no error matrix, give up
  if ( pSegment->ErrorValidity() == 0) return 0;
  return 1 ;
}

// Segments Sharing State
int MboyNativeHelper::SharingState(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) == 0 ) return 0 ;
  return  pSegment->GetTrackSharing();
}

// Utilities for Segment
void MboyNativeHelper::GetPoint(
 MB_SegmentAth* pSegment,
 double& Xpt,
 double& Ypt,
 double& Zpt
){
  Xpt = 0. ;
  Ypt = 0. ;
  Zpt = 0. ;
  if ( IsValid(pSegment) != 0 ) pSegment->GetPoint(Xpt, Ypt , Zpt);
}
void MboyNativeHelper::GetDirection(
 MB_SegmentAth* pSegment,
 double& Xdir,
 double& Ydir,
 double& Zdir
){
  Xdir = 0. ;
  Ydir = 0. ;
  Zdir = 0. ;
  if ( IsValid(pSegment) != 0 ) pSegment->GetDirection(Xdir, Ydir , Zdir);
}


double MboyNativeHelper::GetfitQuality(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetQualityFactor() ;
  return 9999. ;
}



int MboyNativeHelper::GetNberOfDigiMDT(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiMDT() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigiRPC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiRPC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigiTGC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiTGC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigiCSC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiCSC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigiCSCclu(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiCSCclu() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigiCSCPrecise(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigiCSCPrecise() ;
  return 0 ;
}

int MboyNativeHelper::GetNberOfHoleMDT(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfHoleMDT() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfHoleRPC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfHoleRPC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfHoleTGC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfHoleTGC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfHoleCSC(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfHoleCSC() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfHoleCSCclu(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfHoleCSCclu() ;
  return 0 ;
}


int MboyNativeHelper::GetNberOfElements(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfElements() ;
  return 0 ;
}
int MboyNativeHelper::GetNberOfDigi(MB_SegmentAth* pSegment){
  if ( IsValid(pSegment) != 0 ) return pSegment->GetNberOfDigi() ;
  return 0 ;
}
