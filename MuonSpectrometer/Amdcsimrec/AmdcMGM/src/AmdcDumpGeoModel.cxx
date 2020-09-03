/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
 
#include <cmath>

/////////////////////////////////////////////////////////
#include "AmdcMGM/AmdcDumpGeoModel.h"


AmdcDumpGeoModel::AmdcDumpGeoModel(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
p_AmdcsimrecAthenaSvc ( "AmdcsimrecAthenaSvc",name ) 
{

  m_AmdcsimrecAthenaSvcUpdatedSvcDONE = false ; 
  m_KountCallsDoIt   = 0 ;

   p_MuonDetectorManager = 0 ; 

// CheckTEC if 1 perform the comparison for TEC 
// EpsLoTEC Min value on the position difference to output warning for TEC

// CenterCscMean:
//  if 1 use mean value of strip corners position instead of center for CSC

// WarningLevel:
//  if 1 print only warnings, 
//  if 2 print warnings only for precise coordinates, 
//  print all otherwise

// EmergencyOut:
//  if 1 initialize() return FAILURE to speed up analysis

// ChamberSelection:
//  if 1 set on Chamber selection by
//   AmdcDumpGeoModel.StationNameSelected        = [ "BIL" , "EIL" ]
//   AmdcDumpGeoModel.StationAbsAmdcJzzSelected  = [     2 ,     1 ]
//   AmdcDumpGeoModel.StationAmdcJffSelected     = [     1 ,     1 ]

// StationSelection:
//  if 1 set on Station selection by
//  AmdcDumpGeoModel.StationSelected  = [ "BIL" , "BML", "BMS", "BOL" ]

//Declare the properties
   declareProperty("SwitchOff"                 , m_SwitchOff                = 1     ) ;
   declareProperty("WarningLevel"              , m_WarningLevel             = 1     ) ;
   declareProperty("CheckMdt"                  , m_CheckMdt                 = 1     ) ;
   declareProperty("EpsLoMdt"                  , m_EpsLoMdt                 = 0.030 ) ;
   declareProperty("CheckRpc"                  , m_CheckRpc                 = 1     ) ;
   declareProperty("EpsLoRpc"                  , m_EpsLoRpc                 = 0.030 ) ;
   declareProperty("CheckTgc"                  , m_CheckTgc                 = 1     ) ;
   declareProperty("EpsLoTgc"                  , m_EpsLoTgc                 = 0.030 ) ;
   declareProperty("CheckCsc"                  , m_CheckCsc                 = 1     ) ;
   declareProperty("EpsLoCsc"                  , m_EpsLoCsc                 = 0.030 ) ;
   declareProperty("CenterCscMean"             , m_CenterCscMean            = 0     ) ;
   declareProperty("EmergencyOut"              , m_EmergencyOut             = 1     ) ;

   declareProperty("AntiStationSelection"          , m_AntiStationSelection         = 0     ) ;
   declareProperty("AntiStationSelected"           , m_AntiStationSelected           ) ;

   declareProperty("StationSelection"          , m_StationSelection         = 0     ) ;
   declareProperty("StationSelected"           , m_StationSelected           ) ;

   declareProperty("ChamberSelection"          , m_ChamberSelection         = 0     ) ;
   declareProperty("StationNameSelected"       , m_StationNameSelected       ) ;
   declareProperty("StationAbsAmdcJzzSelected" , m_StationAbsAmdcJzzSelected ) ;
   declareProperty("StationAmdcJffSelected"    , m_StationAmdcJffSelected    ) ;


   declareProperty("AmdcsimrecAthenaSvc", p_AmdcsimrecAthenaSvc);

   m_Mdt_MaxDiffZ   = 0. ;
   m_Mdt_MaxDiffS   = 0. ; 
   m_Mdt_MaxDiffT   = 0. ; 
   m_Mdt_MaxDiffZs  = 0. ; 
   m_Mdt_MaxDiffSs  = 0. ; 
   m_Mdt_MaxDiffTs  = 0. ; 
   m_Rpc_MaxDiffZ   = 0. ;
   m_Rpc_MaxDiffS   = 0. ; 
   m_Rpc_MaxDiffT   = 0. ; 
   m_Tgc_MaxDiffZ   = 0. ;
   m_Tgc_MaxDiffS   = 0. ; 
   m_Tgc_MaxDiffT   = 0. ; 
   m_Csc_MaxDiffss  = 0. ;
   m_Csc_MaxDiffzz  = 0. ; 
   m_Csc_MaxDifftt  = 0. ; 

}
 
// Initialize
StatusCode AmdcDumpGeoModel::initialize(){

  ATH_MSG_INFO( "Initialisation started     " ) ;
  ATH_MSG_INFO( "================================" ) ;
  ATH_MSG_INFO( "=Proprieties are     " ) ;
  if (m_SwitchOff == 0) {
    ATH_MSG_INFO( "= WarningLevel       " << m_WarningLevel      ) ;
    ATH_MSG_INFO( "= CheckMdt           " << m_CheckMdt          ) ;
    ATH_MSG_INFO( "= EpsLoMdt           " << m_EpsLoMdt          ) ;
    ATH_MSG_INFO( "= CheckRpc           " << m_CheckRpc          ) ;
    ATH_MSG_INFO( "= EpsLoRpc           " << m_EpsLoRpc          ) ;
    ATH_MSG_INFO( "= CheckTgc           " << m_CheckTgc          ) ;
    ATH_MSG_INFO( "= EpsLoTgc           " << m_EpsLoTgc          ) ;
    ATH_MSG_INFO( "= CheckCsc           " << m_CheckCsc          ) ;
    ATH_MSG_INFO( "= EpsLoCsc           " << m_EpsLoCsc          ) ;
    ATH_MSG_INFO( "= CenterCscMean      " << m_CenterCscMean          ) ;

    ATH_MSG_INFO( "= AntiStationSelection   " << m_AntiStationSelection  ) ;
    if ( m_AntiStationSelection == 1) {
      int AntiStationSelectedSize =  m_AntiStationSelected.size() ;
      if (AntiStationSelectedSize < 1){
        ATH_MSG_FATAL( " Badly set Option ") ;
        ATH_MSG_FATAL( " AntiStationSelectedSize =" << AntiStationSelectedSize ) ;
        return StatusCode::FAILURE;
      }
      for (int AntiStationSelectedItem=0; AntiStationSelectedItem<AntiStationSelectedSize ; AntiStationSelectedItem++){
        ATH_MSG_INFO( "=                    " << m_AntiStationSelected[AntiStationSelectedItem] ) ;
      }
    }

    ATH_MSG_INFO( "= StationSelection   " << m_StationSelection  ) ;
    if ( m_StationSelection == 1) {
      int StationSelectedSize =  m_StationSelected.size() ;
      if (StationSelectedSize < 1){
        ATH_MSG_FATAL( " Badly set Option ") ;
        ATH_MSG_FATAL( " StationSelectedSize =" << StationSelectedSize ) ;
        return StatusCode::FAILURE;
      }
      for (int StationSelectedItem=0; StationSelectedItem<StationSelectedSize ; StationSelectedItem++){
        ATH_MSG_INFO( "=                    " << m_StationSelected[StationSelectedItem] ) ;
      }
    }

    ATH_MSG_INFO( "= ChamberSelection   " << m_ChamberSelection  ) ;
    if ( m_ChamberSelection == 1) {
      int StationNameSelectedSize       =  m_StationNameSelected.size() ;
      int StationAbsAmdcJzzSelectedSize =  m_StationAbsAmdcJzzSelected.size() ;
      int StationAmdcJffSelectedSize    =  m_StationAmdcJffSelected.size() ;
      if (
          StationNameSelectedSize       < 1 
       || StationAbsAmdcJzzSelectedSize < 1 
       || StationAmdcJffSelectedSize    < 1 
       || StationNameSelectedSize       != StationAbsAmdcJzzSelectedSize
       || StationAbsAmdcJzzSelectedSize != StationAmdcJffSelectedSize
         ){
        ATH_MSG_FATAL( " Badly set Option ") ;
        ATH_MSG_FATAL( " StationNameSelectedSize       =" << StationNameSelectedSize       ) ;
        ATH_MSG_FATAL( " StationAbsAmdcJzzSelectedSize =" << StationAbsAmdcJzzSelectedSize ) ;
        ATH_MSG_FATAL( " StationAmdcJffSelectedSize    =" << StationAmdcJffSelectedSize    ) ;
        return StatusCode::FAILURE;
      }
      for (int StationNameSelectedItem=0; StationNameSelectedItem<StationNameSelectedSize ; StationNameSelectedItem++){
        ATH_MSG_INFO( "=  " 
           << " " << m_StationNameSelected[StationNameSelectedItem] 
           << " " << m_StationAbsAmdcJzzSelected[StationNameSelectedItem] 
           << " " << m_StationAmdcJffSelected[StationNameSelectedItem] 
           ) ;
      }
    }

    ATH_MSG_INFO( "= EmergencyOut       " << m_EmergencyOut      ) ;
  }
  ATH_MSG_INFO( "= SwitchOff          " << m_SwitchOff          ) ;
  ATH_MSG_INFO( "================================" ) ;

  if (m_SwitchOff == 0) {

//  Set pointer on MuonDetectorManager
    ATH_CHECK(detStore()->retrieve(p_MuonDetectorManager));

    ATH_CHECK(m_idHelperSvc.retrieve());

//Retrieve p_AmdcsimrecAthenaSvc and set up call back
    ATH_CHECK(p_AmdcsimrecAthenaSvc.retrieve());
    ATH_MSG_INFO( "Retrieved service " << p_AmdcsimrecAthenaSvc ) ;

    if (p_AmdcsimrecAthenaSvc->InitializedSvc()) {
      ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is true " ) ;
      m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ; 
    }else{
      ATH_MSG_INFO( "p_AmdcsimrecAthenaSvc->InitializedSvc() is false " ) ;
      ATH_CHECK(regFcnDoIt());
      ATH_MSG_INFO( "Done: regFcnDoIt " ) ;
    }

//  Do something now if possible
    if ( m_AmdcsimrecAthenaSvcUpdatedSvcDONE ){
      ATH_MSG_INFO( "m_AmdcsimrecAthenaSvcUpdatedSvcDONE found true in initialize " ) ;
      ATH_CHECK(DoIt());
    }

  }
  
  ATH_MSG_INFO( "Initialisation ended     " ) ;
  if ( m_EmergencyOut == 1 ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

// Do it
StatusCode AmdcDumpGeoModel::DoItCallback(IOVSVC_CALLBACK_ARGS)
{
  ATH_MSG_INFO( "DoItCallback called     " ) ;  

  if ( !(p_AmdcsimrecAthenaSvc->UsableSvc()) ) {
    ATH_MSG_INFO( "BUT p_AmdcsimrecAthenaSvc found NOT usable yet  " ) ;  
    return StatusCode::SUCCESS;
  }else{
    m_AmdcsimrecAthenaSvcUpdatedSvcDONE = true ;
    ATH_MSG_INFO( "AND p_AmdcsimrecAthenaSvc found usable   " ) ;  
  }

  StatusCode sc = DoIt() ;
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL( "DoIt failed" ) ; 
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;

}

StatusCode AmdcDumpGeoModel::regFcnDoIt()
{

  StatusCode sc = detStore()->regFcn(
                         &AmdcsimrecAthenaSvc::UpdatedSvc,(&*p_AmdcsimrecAthenaSvc),
                         &AmdcDumpGeoModel::DoItCallback,this,true
                        );
  if (sc.isFailure()) {
    ATH_MSG_FATAL( "Unable to register callback on AmdcDumpGeoModel::DoItCallback from AmdcsimrecAthenaSvc::UpdatedSvc " ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "Done: Register callback on AmdcDumpGeoModel::DoItCallback from AmdcsimrecAthenaSvc::UpdatedSvc " ) ;

  return StatusCode::SUCCESS;
  
}
StatusCode AmdcDumpGeoModel::DoIt() 
{
  ATH_MSG_INFO( "DoIt called     " ) ;  

  if ( !m_AmdcsimrecAthenaSvcUpdatedSvcDONE ){
    ATH_MSG_INFO( "DoIt() called BUT m_AmdcsimrecAthenaSvcUpdatedSvcDONE is false    " ) ;
    return StatusCode::SUCCESS;
  }

  if (m_SwitchOff == 0) {

//  Count calls
    m_KountCallsDoIt = m_KountCallsDoIt + 1 ;
    std::string KountCallsDoItASstring = "" ;
    std::stringstream KountCallsDoItASstringstream;
    KountCallsDoItASstringstream << m_KountCallsDoIt ;
    if (m_KountCallsDoIt > 1 ) KountCallsDoItASstring = KountCallsDoItASstringstream.str();

//  Control ouput names
    std::string StringBidon = "";

    if (m_CheckMdt==1) {
      StringBidon = "Out.AmdcMGM.Mdt" + KountCallsDoItASstring ;
      std::ofstream OutFile;
      OutFile.open(StringBidon.c_str());
      OutFile << setiosflags(std::ios::fixed);
      LoopMdtElements(OutFile);
      OutFile.close();
    }

    if (m_CheckRpc==1) {
      StringBidon = "Out.AmdcMGM.Rpc" + KountCallsDoItASstring ;
      std::ofstream OutFile;
      OutFile.open(StringBidon.c_str());
      OutFile << setiosflags(std::ios::fixed);
      LoopRpcElements(OutFile);
      OutFile.close();
    }

    if (m_CheckTgc==1) {
      StringBidon = "Out.AmdcMGM.Tgc" + KountCallsDoItASstring ;
      std::ofstream OutFile;
      OutFile.open(StringBidon.c_str());
      OutFile << setiosflags(std::ios::fixed);
      LoopTgcElements(OutFile);
      OutFile.close();
    }

    if (m_CheckCsc==1) {
      StringBidon = "Out.AmdcMGM.Csc" + KountCallsDoItASstring ;
      std::ofstream OutFile;
      OutFile.open(StringBidon.c_str());
      OutFile << setiosflags(std::ios::fixed);
      LoopCscElements(OutFile);
      OutFile.close();
    }

  }

  return StatusCode::SUCCESS;

}

int AmdcDumpGeoModel::AntiSelectedStation(std::string TheStationName){

  int ToBeReturned = 1 ;
  if ( m_AntiStationSelection != 1) return ToBeReturned;
  int NbSta =  m_AntiStationSelected.size() ;
  for (int ISta=0; ISta<NbSta ; ISta++){
    if (TheStationName == m_AntiStationSelected[ISta]) ToBeReturned = 0 ;
  }
  return ToBeReturned;

}

int AmdcDumpGeoModel::SelectedStation(std::string TheStationName){

  int ToBeReturned = 1 ;
  if ( m_StationSelection != 1) return ToBeReturned;
  ToBeReturned = 0 ;
  int NbSta =  m_StationSelected.size() ;
  for (int ISta=0; ISta<NbSta ; ISta++){
    if (TheStationName == m_StationSelected[ISta]) ToBeReturned = 1 ;
  }
  return ToBeReturned;

}
int AmdcDumpGeoModel::SelectedChamber(std::string TheStationName, int AmdcJzz, int AmdcJff){

  int ToBeReturned = 1 ;
  if ( m_ChamberSelection != 1) return ToBeReturned;
  int StationNameSelectedSize       =  m_StationNameSelected.size() ;
  ToBeReturned = 0 ;
  for (int StationNameSelectedItem=0; StationNameSelectedItem<StationNameSelectedSize ; StationNameSelectedItem++){
    if (
        TheStationName == m_StationNameSelected[StationNameSelectedItem]
    &&  std::abs(AmdcJzz)   == std::abs(m_StationAbsAmdcJzzSelected[StationNameSelectedItem])
    &&  AmdcJff        == m_StationAmdcJffSelected[StationNameSelectedItem]
       ) ToBeReturned = 1 ;
  }
  return ToBeReturned;

}

void AmdcDumpGeoModel::LoopMdtElements(std::ofstream&  OutFile) {

  m_Mdt_MaxDiffZ  = 0. ;
  m_Mdt_MaxDiffS  = 0. ; 
  m_Mdt_MaxDiffT  = 0. ; 
  m_Mdt_MaxDiffZs = 0. ; 
  m_Mdt_MaxDiffSs = 0. ; 
  m_Mdt_MaxDiffTs = 0. ; 
  
  for (int sname_index = 0; sname_index<MuonGM::MuonDetectorManager::NMdtStatType; ++ sname_index) 
  {
   for (int seta_index = 0; seta_index<MuonGM::MuonDetectorManager::NMdtStatEta; ++seta_index)
   {
    for (int sphi_index = 0; sphi_index<MuonGM::MuonDetectorManager::NMdtStatPhi; ++sphi_index)
    {
     for (int dbr_index = 0; dbr_index<MuonGM::MuonDetectorManager::NMdtMultilayer; ++dbr_index)
     {
      const MuonGM::MdtReadoutElement* pReadoutElement = 
          p_MuonDetectorManager->getMdtReadoutElement(sname_index,
     	     					      seta_index,
     	     					      sphi_index,
     	     					      dbr_index);
      if (pReadoutElement == NULL) continue;
      Identifier idr = pReadoutElement->identify();
      Identifier idp = m_idHelperSvc->mdtIdHelper().parentID(idr);
      for ( int tl=1; tl<=pReadoutElement->getNLayers();) 
      {
       int turn = 1 ;
       for ( int tube=1; tube<=pReadoutElement->getNtubesperlayer();) 
       {
        Identifier channelId = m_idHelperSvc->mdtIdHelper().channelID(idp,
	                                           pReadoutElement->getMultilayer(),
						   tl, tube);
        bool IsValid = m_idHelperSvc->mdtIdHelper().valid(channelId);
	if (IsValid) {
         MdtCompare(OutFile,channelId,pReadoutElement);
	}else{
         OutFile 
	                 << "*sname_index/seta_index/sphi_index/dbr_index/tl/tube"	     
	                 << std::endl;
         OutFile 
	                 << " " << sname_index << " " << seta_index << " " << sphi_index	 
                         << " " << dbr_index	 
                         << " " << tl << " " << tube	 
	                 << std::endl;
	 OutFile 
	                 << " Not Valid??????????"
	                 << std::endl;
	}
        if (turn==1) {
          tube = pReadoutElement->getNtubesperlayer()/2 ;
        }
        if (turn==2) {
          tube = pReadoutElement->getNtubesperlayer() ;
        }
        if (turn==3) {
          tube = pReadoutElement->getNtubesperlayer() + 1 ;
        }
        turn +=1 ;
       }
//     tl += pReadoutElement->getNLayers()-1;
       tl += 1;
      }
     }
    }
   }
  }
  
  OutFile 
         << "Max deviations " 
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffZ  
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffS  
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffT  
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffZs 
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffSs 
         << std::setw(12)<<std::setprecision(3)<<  m_Mdt_MaxDiffTs 
	 << std::endl;

}

void AmdcDumpGeoModel::MdtCompare(
 std::ofstream&           OutFile,
 Identifier               channelId,
 const MuonGM::MdtReadoutElement* pReadoutElement)
{
  bool IsValid = m_idHelperSvc->mdtIdHelper().valid(channelId);
  if (IsValid) {
  
    int SPstationName		   = m_idHelperSvc->mdtIdHelper().stationName(channelId);
    std::string SPstationNameString = m_idHelperSvc->mdtIdHelper().stationNameString(SPstationName);
    int SPstationEta		   = m_idHelperSvc->mdtIdHelper().stationEta(channelId);
    int SPstationPhi		   = m_idHelperSvc->mdtIdHelper().stationPhi(channelId);
    int SPmultilayer		   = m_idHelperSvc->mdtIdHelper().multilayer(channelId);
    int SPtubeLayer 		   = m_idHelperSvc->mdtIdHelper().tubeLayer(channelId);
    int SPtube 		           = m_idHelperSvc->mdtIdHelper().tube(channelId);

    const  Amg::Vector3D globalPositionRO = pReadoutElement->ROPos(channelId);
    const  Amg::Vector3D globalPosition   = pReadoutElement->tubePos(channelId);
    double TubeLength                  = pReadoutElement->tubeLength(channelId) ;

    double GeoModX_RO = globalPositionRO.x() ;
    double GeoModY_RO = globalPositionRO.y() ;
    double GeoModZ_RO = globalPositionRO.z() ;
    
    double GeoModX = globalPosition.x();
    double GeoModY = globalPosition.y();
    double GeoModZ = globalPosition.z();

    MdtCompare(
               OutFile             ,
               m_EpsLoMdt          ,
               m_WarningLevel      ,
               SPstationName       , 
               SPstationNameString , 
               SPstationEta        , 
               SPstationPhi        , 
               SPmultilayer        , 
               SPtubeLayer         , 
               SPtube              , 
               GeoModX_RO          ,
               GeoModY_RO          ,
               GeoModZ_RO          ,
               GeoModX             ,
               GeoModY             ,
               GeoModZ             ,
               TubeLength       
              );
   
  }
    
}

void AmdcDumpGeoModel::MdtCompare(
 std::ofstream& OutFile             ,
 double         EpsLoMdt            ,
 int            WarningLevel        ,
 int            SPstationName       , 
 std::string    SPstationNameString , 
 int            SPstationEta        , 
 int            SPstationPhi        , 
 int            SPmultilayer        , 
 int            SPtubeLayer         , 
 int            SPtube              , 
 double         GeoModX_RO          ,
 double         GeoModY_RO          ,
 double         GeoModZ_RO          ,
 double         GeoModX             ,
 double         GeoModY             ,
 double         GeoModZ             ,
 double         TubeLength       
){

   Amdcsimrec* pAmdcsimrec=p_AmdcsimrecAthenaSvc->GetAmdcsimrec();

   int AmdcJff   ;
   int AmdcJzz   ; 
   int AmdcJob   ;
   int AmdcJsl   ;
   int AmdcJtube ;
   int AmdcJtyp  ;
   std::string AmdcStationName = SPstationNameString ;
   AmdcJtyp=pAmdcsimrec->GetJtyp(SPstationNameString);
   pAmdcsimrec->GetAMDCindexFromMDT(SPstationNameString,
  				     SPstationEta, 
  				     SPstationPhi,
  				     SPmultilayer, 
  				     SPtubeLayer,
  				     SPtube,
  				     AmdcJff, 
  				     AmdcJzz, 
  				     AmdcJob, 
  				     AmdcJsl, 
  				     AmdcJtube);
   double FF0 = pAmdcsimrec->PosPhi(AmdcJtyp,AmdcJff,AmdcJzz);
   double DCF = cos(FF0);
   double DSF = sin(FF0);

   int    PosiIsValid  ;
   int    PosiJgeo     ;
   int    PosiJcut     ;
   int    PosiIsBarrel ;
   double PosiPhi      ;
   double PosiZ        ;
   double PosiR        ;
   double PosiS        ;
   double PosiAlfa     ;
   double PosiBeta     ;
   double PosiGamma    ;
   pAmdcsimrec->GetStationPositionParam( SPstationNameString, AmdcJff,AmdcJzz,
                                          PosiIsValid  ,
                                          PosiJgeo     ,
                                          PosiJcut     ,
                                          PosiIsBarrel ,
                                          PosiPhi      ,
                                          PosiZ        ,
                                          PosiR        ,
                                          PosiS        ,
                                          PosiAlfa     ,
                                          PosiBeta     ,
                                          PosiGamma    );
               
   
   
   
// MGM Center
   double GeoModS = DCF * GeoModY - DSF * GeoModX ;
   double GeoModT = DCF * GeoModX + DSF * GeoModY ;
   
   double GeoModSs = GeoModS - PosiS ;
   double GeoModZs = GeoModZ - PosiZ ;
   double GeoModTs = GeoModT - PosiR ;
   if ( PosiIsBarrel <= 0 ){ 
     if ( AmdcJzz > 0 ){
       GeoModSs = -(GeoModS - PosiS) ;
       GeoModZs =   GeoModT - PosiR  ;
       GeoModTs =   GeoModZ - PosiZ  ;
     }else{
       GeoModSs =   GeoModS - PosiS  ;
       GeoModZs =   GeoModT - PosiR  ;
       GeoModTs = -(GeoModZ - PosiZ) ;
     }
   }
   
// MGM RO
   double GeoModS_RO = DCF * GeoModY_RO - DSF * GeoModX_RO ;
   double GeoModT_RO = DCF * GeoModX_RO + DSF * GeoModY_RO ;
   
   double GeoModSs_RO = GeoModS_RO - PosiS ;
   double GeoModZs_RO = GeoModZ_RO - PosiZ ;
   double GeoModTs_RO = GeoModT_RO - PosiR ;
   if ( PosiIsBarrel <= 0 ){ 
     if ( AmdcJzz > 0 ){
       GeoModSs_RO = -(GeoModS_RO - PosiS) ;
       GeoModZs_RO =   GeoModT_RO - PosiR  ;
       GeoModTs_RO =   GeoModZ_RO - PosiZ  ;
     }else{
       GeoModSs_RO =   GeoModS_RO - PosiS  ;
       GeoModZs_RO =   GeoModT_RO - PosiR  ;
       GeoModTs_RO = -(GeoModZ_RO - PosiZ) ;
     }
   }

// MGM distance Center RO
   double MGMDistToRO = sqrt ( (GeoModX-GeoModX_RO)*(GeoModX-GeoModX_RO) 
                             + (GeoModY-GeoModY_RO)*(GeoModY-GeoModY_RO) 
                             + (GeoModZ-GeoModZ_RO)*(GeoModZ-GeoModZ_RO) ) ;
   
// MGM direction Center RO
   double GeoModXDir = GeoModX_RO - GeoModX ;
   double GeoModYDir = GeoModY_RO - GeoModY ;
   double GeoModZDir = GeoModZ_RO - GeoModZ ;
   double GeoModDirNorm = sqrt ( GeoModXDir*GeoModXDir
                               + GeoModYDir*GeoModYDir
                               + GeoModZDir*GeoModZDir ) ;
   GeoModXDir = GeoModXDir/GeoModDirNorm;
   GeoModYDir = GeoModYDir/GeoModDirNorm;
   GeoModZDir = GeoModZDir/GeoModDirNorm;

// Amdc Center
   double XYZ1[3] ;
   double XYZ2[3] ;
   double SZT1[3] ;
   double SZT2[3] ;
   double Radius = 0. ;
   pAmdcsimrec->GetTubePositionFromMDTindex(SPstationNameString,
  					     SPstationEta, 
  					     SPstationPhi,
  					     SPmultilayer, 
  					     SPtubeLayer,
  					     SPtube,
  					     XYZ1,
  					     XYZ2,
  					     SZT1, 
  					     SZT2,
  					     Radius);
   double AmdcX = ( XYZ1[0] + XYZ2[0] )/2.;
   double AmdcY = ( XYZ1[1] + XYZ2[1] )/2.;
   double AmdcZ = ( XYZ1[2] + XYZ2[2] )/2.;
   double AmdcS = DCF * AmdcY - DSF * AmdcX ;
   double AmdcT = DCF * AmdcX + DSF * AmdcY ;
   
   double AmdcSs = AmdcS - PosiS ;
   double AmdcZs = AmdcZ - PosiZ ;
   double AmdcTs = AmdcT - PosiR ;
   if ( PosiIsBarrel <= 0 ){ 
     if ( AmdcJzz > 0 ){
       AmdcSs = -(AmdcS - PosiS) ;
       AmdcZs =   AmdcT - PosiR  ;
       AmdcTs =   AmdcZ - PosiZ  ;
     }else{
       AmdcSs =   AmdcS - PosiS  ;
       AmdcZs =   AmdcT - PosiR  ;
       AmdcTs = -(AmdcZ - PosiZ) ;
     }
   }
   
// Amdc direction Center RO
   double AmdcXDir = XYZ1[0] - XYZ2[0] ;
   double AmdcYDir = XYZ1[1] - XYZ2[1] ;
   double AmdcZDir = XYZ1[2] - XYZ2[2] ;
   double AmdcDirNorm = sqrt ( AmdcXDir*AmdcXDir 
                             + AmdcYDir*AmdcYDir 
                             + AmdcZDir*AmdcZDir ) ;
   AmdcXDir = AmdcXDir/AmdcDirNorm ;
   AmdcYDir = AmdcYDir/AmdcDirNorm;
   AmdcZDir = AmdcZDir/AmdcDirNorm ;

   double ScaDirGeoModAmdc = GeoModXDir * AmdcXDir
                           + GeoModYDir * AmdcYDir
                           + GeoModZDir * AmdcZDir ;
   if ( ScaDirGeoModAmdc < 0 ){
     AmdcXDir = -AmdcXDir ;
     AmdcYDir = -AmdcYDir ;
     AmdcZDir = -AmdcZDir ;
   }                     

// Amdc RO
   double AmdcX_RO = AmdcX + AmdcXDir * MGMDistToRO ;
   double AmdcY_RO = AmdcY + AmdcYDir * MGMDistToRO ;
   double AmdcZ_RO = AmdcZ + AmdcZDir * MGMDistToRO ;
   double AmdcS_RO = DCF * AmdcY_RO - DSF * AmdcX_RO ;
   double AmdcT_RO = DCF * AmdcX_RO + DSF * AmdcY_RO ;
   
   double AmdcSs_RO = AmdcS_RO - PosiS ;
   double AmdcZs_RO = AmdcZ_RO - PosiZ ;
   double AmdcTs_RO = AmdcT_RO - PosiR ;
   if ( PosiIsBarrel <= 0 ){ 
     if ( AmdcJzz > 0 ){
       AmdcSs_RO = -(AmdcS_RO - PosiS) ;
       AmdcZs_RO =   AmdcT_RO - PosiR  ;
       AmdcTs_RO =   AmdcZ_RO - PosiZ  ;
     }else{
       AmdcSs_RO =   AmdcS_RO - PosiS  ;
       AmdcZs_RO =   AmdcT_RO - PosiR  ;
       AmdcTs_RO = -(AmdcZ_RO - PosiZ) ;
     }
   }


   double AmdcTubeLength = sqrt( (XYZ1[0]-XYZ2[0])*(XYZ1[0]-XYZ2[0])
                                +(XYZ1[1]-XYZ2[1])*(XYZ1[1]-XYZ2[1])
                                +(XYZ1[2]-XYZ2[2])*(XYZ1[2]-XYZ2[2]) );
   double DiffX = GeoModX - AmdcX ;
   double DiffY = GeoModY - AmdcY ;
   double DiffZ = GeoModZ - AmdcZ ;
   double DiffS = GeoModS - AmdcS ;
   double DiffT = GeoModT - AmdcT ;
   double DiffX_RO = GeoModX_RO - AmdcX_RO ;
   double DiffY_RO = GeoModY_RO - AmdcY_RO ;
   double DiffZ_RO = GeoModZ_RO - AmdcZ_RO ;
   double DiffS_RO = GeoModS_RO - AmdcS_RO ;
   double DiffT_RO = GeoModT_RO - AmdcT_RO ;

   double DiffZs = GeoModZs - AmdcZs ;
   double DiffSs = GeoModSs - AmdcSs ;
   double DiffTs = GeoModTs - AmdcTs ;
   double DiffZs_RO = GeoModZs_RO - AmdcZs_RO ;
   double DiffSs_RO = GeoModSs_RO - AmdcSs_RO ;
   double DiffTs_RO = GeoModTs_RO - AmdcTs_RO ;

   double DiffLtub = TubeLength - AmdcTubeLength ;
   
   std::string WarningX = " ";
   std::string WarningY = " ";
   std::string WarningZ = " ";
   std::string WarningS = " ";
   std::string WarningT = " ";
   std::string WarningX_RO = " ";
   std::string WarningY_RO = " ";
   std::string WarningZ_RO = " ";
   std::string WarningS_RO = " ";
   std::string WarningT_RO = " ";

   std::string WarningZs = " ";
   std::string WarningSs = " ";
   std::string WarningTs = " ";
   std::string WarningZs_RO = " ";
   std::string WarningSs_RO = " ";
   std::string WarningTs_RO = " ";

   std::string WarningTube = " ";
   int Iwarning = 0 ;

   if (std::abs(DiffZ) > EpsLoMdt ) WarningZ = "  <=========  WARNING !";
   if (std::abs(DiffS) > EpsLoMdt ) WarningS = "  <=========  WARNING !";
   if (std::abs(DiffT) > EpsLoMdt ) WarningT = "  <=========  WARNING !";
   if (std::abs(DiffZ) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffS) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffT) > EpsLoMdt ) Iwarning = 1 ;

   if (std::abs(DiffZ_RO) > EpsLoMdt ) WarningZ_RO = "  <=========  WARNING !";
   if (std::abs(DiffS_RO) > EpsLoMdt ) WarningS_RO = "  <=========  WARNING !";
   if (std::abs(DiffT_RO) > EpsLoMdt ) WarningT_RO = "  <=========  WARNING !";
   if (std::abs(DiffZ_RO) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffS_RO) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffT_RO) > EpsLoMdt ) Iwarning = 1 ;

   if (std::abs(DiffZs) > EpsLoMdt ) WarningZs = "  <=========  WARNING !";
   if (std::abs(DiffSs) > EpsLoMdt ) WarningSs = "  <=========  WARNING !";
   if (std::abs(DiffTs) > EpsLoMdt ) WarningTs = "  <=========  WARNING !";
   if (std::abs(DiffZs) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffSs) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffTs) > EpsLoMdt ) Iwarning = 1 ;
   
   if (std::abs(DiffZs_RO) > EpsLoMdt ) WarningZs_RO = "  <=========  WARNING !";
   if (std::abs(DiffSs_RO) > EpsLoMdt ) WarningSs_RO = "  <=========  WARNING !";
   if (std::abs(DiffTs_RO) > EpsLoMdt ) WarningTs_RO = "  <=========  WARNING !";
   if (std::abs(DiffZs_RO) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffSs_RO) > EpsLoMdt ) Iwarning = 1 ;
   if (std::abs(DiffTs_RO) > EpsLoMdt ) Iwarning = 1 ;

   if (TubeLength <= 0.001 ) WarningTube = "  <=========  WARNING !";
   if (TubeLength <= 0.001 ) Iwarning = 2 ;

   if (std::abs(DiffZ) > EpsLoMdt ) WarningZ = "  <=========  WARNING ! (precise Measurement)";
   if (std::abs(DiffZ) > EpsLoMdt ) Iwarning = 2 ;

   if (Iwarning >= WarningLevel 
   && SelectedStation(AmdcStationName) == 1 
   && AntiSelectedStation(AmdcStationName) == 1 
   && SelectedChamber(AmdcStationName,AmdcJzz,AmdcJff) == 1 ) {

    if ( std::abs(DiffZ)  > std::abs(m_Mdt_MaxDiffZ ) )  m_Mdt_MaxDiffZ  = DiffZ  ;
    if ( std::abs(DiffS)  > std::abs(m_Mdt_MaxDiffS ) )  m_Mdt_MaxDiffS  = DiffS  ;
    if ( std::abs(DiffT)  > std::abs(m_Mdt_MaxDiffT ) )  m_Mdt_MaxDiffT  = DiffT  ;

    if ( std::abs(DiffZs) > std::abs(m_Mdt_MaxDiffZs) )  m_Mdt_MaxDiffZs = DiffZs ;
    if ( std::abs(DiffSs) > std::abs(m_Mdt_MaxDiffSs) )  m_Mdt_MaxDiffSs = DiffSs ;
    if ( std::abs(DiffTs) > std::abs(m_Mdt_MaxDiffTs) )  m_Mdt_MaxDiffTs = DiffTs ;

    OutFile 
  		   << "*StNa/Name/Eta/Phi/MuL/TuL/Tube /" 
  		   << std::setw(4)<< SPstationName	      
  		   << std::setw(3)<< SPstationNameString 
  		   << std::setw(4)<< SPstationEta      
  		   << std::setw(4)<< SPstationPhi      
  		   << std::setw(4)<< SPmultilayer      
  		   << std::setw(4)<< SPtubeLayer       
  		   << std::setw(4)<< SPtube   
  		   << std::endl;
    OutFile 
  		   << " Jtyp/Name/Jzz/Jff/Job/Jsl/Jtube/" 
  		   << std::setw(4)<< AmdcJtyp	     
  		   << std::setw(3)<< AmdcStationName 
  		   << std::setw(4)<< AmdcJzz	      
  		   << std::setw(4)<< AmdcJff	      
  		   << std::setw(4)<< AmdcJob	      
  		   << std::setw(4)<< AmdcJsl	      
  		   << std::setw(4)<< AmdcJtube 
  		   << std::endl;
    OutFile 
  		   << " GM/Am/GM-Am/X " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModX	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcX	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffX 
  		   <<					  WarningX  
  		   << std::endl;
    OutFile 
  		   << "             Y " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModY	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcY	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffY   
  		   <<					  WarningY  
  		   << std::endl;
    OutFile 
  		   << "             Z " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModZ	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcZ	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffZ    
  		   <<					  WarningZ  
  		   << std::endl;
    OutFile 
    		   << "             S " 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModS       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcS       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffS   
	  	   <<					  WarningS  
    		   << std::endl;
    OutFile 
    		   << "             T " 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModT       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcT       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffT    
	  	   <<					  WarningT  
    		   << std::endl;
    OutFile 
  		   << "     (RO)    X " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModX_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcX_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffX_RO   
  		   <<					  WarningX_RO  
  		   << std::endl;
    OutFile 
  		   << "     (RO)    Y " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModY_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcY_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffY_RO   
  		   <<					  WarningY_RO  
  		   << std::endl;
    OutFile 
  		   << "     (RO)    Z " 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModZ_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcZ_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffZ_RO    
  		   <<					  WarningZ_RO  
  		   << std::endl;
    OutFile 
    		   << "     (RO)    S " 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModS_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcS_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffS_RO   
	  	   <<					  WarningS_RO  
    		   << std::endl;
    OutFile 
    		   << "     (RO)    T " 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModT_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcT_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffT_RO    
	  	   <<					  WarningT_RO  
    		   << std::endl;
 
    OutFile 
  		   << "             Zs" 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModZs	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcZs	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffZs    
  		   <<					  WarningZs  
  		   << std::endl;
    OutFile 
    		   << "             Ss" 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModSs       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcSs       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffSs   
	  	   <<					  WarningSs 
    		   << std::endl;
    OutFile 
    		   << "             Ts" 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModTs       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcTs       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffTs    
	  	   <<					  WarningTs  
    		   << std::endl;
    OutFile 
  		   << "     (RO)    Zs" 
  		   << std::setw(12)<<std::setprecision(3)<<GeoModZs_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  AmdcZs_RO	 
  		   << std::setw(12)<<std::setprecision(3)<<  DiffZs_RO    
  		   <<					  WarningZs_RO  
  		   << std::endl;
    OutFile 
    		   << "     (RO)    Ss" 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModSs_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcSs_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffSs_RO   
	  	   <<					  WarningSs_RO  
    		   << std::endl;
    OutFile 
    		   << "     (RO)    Ts" 
	  	   << std::setw(12)<<std::setprecision(3)<<GeoModTs_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  AmdcTs_RO       
	  	   << std::setw(12)<<std::setprecision(3)<<  DiffTs_RO    
	  	   <<					  WarningTs_RO  
    		   << std::endl;

    OutFile 
    		   << "   Tube Length " 
	  	   << std::setw(12)<<std::setprecision(3)<<TubeLength
	  	   << std::setw(12)<<std::setprecision(3)<<AmdcTubeLength
	  	   << std::setw(12)<<std::setprecision(3)<<DiffLtub
	  	   <<					  WarningTube  
    		   << std::endl;
   }

}

void AmdcDumpGeoModel::LoopRpcElements(std::ofstream&  OutFile){

  m_Rpc_MaxDiffZ  = 0. ;
  m_Rpc_MaxDiffS  = 0. ; 
  m_Rpc_MaxDiffT  = 0. ; 
  
  for (int sname_index = 0; sname_index<MuonGM::MuonDetectorManager::NRpcStatType; ++ sname_index) 
  {
   for (int seta_index = 0; seta_index<MuonGM::MuonDetectorManager::NRpcStatEta; ++seta_index)
   {
    for (int sphi_index = 0; sphi_index<MuonGM::MuonDetectorManager::NRpcStatPhi; ++sphi_index)
    {
     for (int dbr_index = 0; dbr_index<MuonGM::MuonDetectorManager::NDoubletR; ++dbr_index)
     {
      for (int dbz_index = 0; dbz_index<MuonGM::MuonDetectorManager::NDoubletZ; ++dbz_index)
      {
       int stationName = p_MuonDetectorManager->rpcStationName(sname_index);
       bool isValid=false;
       Identifier id = m_idHelperSvc->rpcIdHelper().channelID(stationName, seta_index, sphi_index, dbr_index, dbz_index, 1, 1, 1, 1, true, &isValid); // last 5 arguments are: int doubletPhi, int gasGap, int measuresPhi, int strip, bool check, bool* isValid
       if (!isValid) continue;
       const MuonGM::RpcReadoutElement* pReadoutElement = p_MuonDetectorManager->getRpcReadoutElement(id);
       if (pReadoutElement == nullptr) continue;
       Identifier idr = pReadoutElement->identify();
       Identifier idp = m_idHelperSvc->rpcIdHelper().parentID(idr);

       int ndbphi = pReadoutElement->NphiStripPanels();
       for (int idbphi = 1; idbphi<=ndbphi; idbphi++)
       {
//I do not understand this JFL(Wed Nov 17 13:43:44 CET 2004)
        int dbp = m_idHelperSvc->rpcIdHelper().doubletPhi(idr);
        if (ndbphi>1 && idbphi>1) dbp = idbphi;
        for ( int igg=1; igg<3; igg++) 
        {
         int measphi = 1;
         for (int strip = 1; strip<=pReadoutElement->NphiStrips();)
         {
          Identifier channelId = m_idHelperSvc->rpcIdHelper().channelID(idp,
             				            m_idHelperSvc->rpcIdHelper().doubletZ(idr),
             				            dbp, igg, measphi, strip);
          bool IsValid = m_idHelperSvc->rpcIdHelper().valid(channelId);
	  if (IsValid) {
           RpcCompare(OutFile,channelId,pReadoutElement);
	  }else{
           OutFile 
	                 << "*sname_index/seta_index /sphi_index/dbr_index/dbz_index/dbp/igg/measphi/strip"   
	                 << std::endl;
           OutFile 
	                 << " " << sname_index << " " << seta_index << " " << sphi_index	 
                         << " " << dbr_index << " " << dbz_index	 
                         << " " << dbp << " " << igg << " " << measphi << " " << strip   
	                 << std::endl;
	   OutFile 
	                 << " Not Valid??????????"
	                 << std::endl;
	  }	   
          strip+=pReadoutElement->NphiStrips()-1;
         }
         measphi = 0;
         for (int strip = 1; strip<=pReadoutElement->NetaStrips();)
         {
          Identifier channelId = m_idHelperSvc->rpcIdHelper().channelID(idp,
             					    m_idHelperSvc->rpcIdHelper().doubletZ(idr),
             					    dbp, igg, measphi, strip);
          bool IsValid = m_idHelperSvc->rpcIdHelper().valid(channelId);
	  if (IsValid) {
           RpcCompare(OutFile,channelId,pReadoutElement);
	  }else{
           OutFile 
	                 << "*sname_index/seta_index /sphi_index/dbr_index/dbz_index/dbp/igg/measphi/strip"   
	                 << std::endl;
           OutFile 
	                 << " " << sname_index << " " << seta_index << " " << sphi_index	 
                         << " " << dbr_index << " " << dbz_index	 
                         << " " << dbp << " " << igg << " " << measphi << " " << strip   
	                 << std::endl;
	   OutFile 
	                 << " Not Valid??????????"
	                 << std::endl;
	  }	   
          strip += pReadoutElement->NetaStrips()-1;
         }
        }
       }
      }
     }
    }
   }
  }
  
  OutFile 
         << "Max deviations " 
         << std::setw(12)<<std::setprecision(3)<<  m_Rpc_MaxDiffZ  
         << std::setw(12)<<std::setprecision(3)<<  m_Rpc_MaxDiffS  
         << std::setw(12)<<std::setprecision(3)<<  m_Rpc_MaxDiffT  
	 << std::endl;

}

void AmdcDumpGeoModel::RpcCompare(
 std::ofstream&           OutFile,
 Identifier               channelId,
 const MuonGM::RpcReadoutElement* pReadoutElement)
{

  bool IsValid = m_idHelperSvc->rpcIdHelper().valid(channelId);
  if (IsValid) {
   int SPstationName               = m_idHelperSvc->rpcIdHelper().stationName(channelId);
   std::string SPstationNameString = m_idHelperSvc->rpcIdHelper().stationNameString(SPstationName);
   int SPstationEta                = m_idHelperSvc->rpcIdHelper().stationEta(channelId);
   int SPstationPhi                = m_idHelperSvc->rpcIdHelper().stationPhi(channelId);
   int SPDoubletR                  = m_idHelperSvc->rpcIdHelper().doubletR(channelId);
   int SPDoubletZ                  = m_idHelperSvc->rpcIdHelper().doubletZ(channelId);
   int SPDoubletPhi                = m_idHelperSvc->rpcIdHelper().doubletPhi(channelId);
   int SPGasGap                    = m_idHelperSvc->rpcIdHelper().gasGap(channelId);
   int SPMeasuresPhi               = m_idHelperSvc->rpcIdHelper().measuresPhi(channelId);
   int SPStrip                     = m_idHelperSvc->rpcIdHelper().strip(channelId);

   const Amg::Vector3D globalPosition = pReadoutElement->stripPos(channelId);

   double GeoModX = globalPosition.x();
   double GeoModY = globalPosition.y();
   double GeoModZ = globalPosition.z();

   RpcCompare(
              OutFile             ,
              m_EpsLoRpc          ,
              m_WarningLevel      ,
              SPstationName       ,
              SPstationNameString ,
              SPstationEta        ,
              SPstationPhi        ,
              SPDoubletR          ,
              SPDoubletZ          ,
              SPDoubletPhi        ,
              SPGasGap            ,
              SPMeasuresPhi       ,
              SPStrip             ,
              GeoModX             ,
              GeoModY             ,
              GeoModZ             
             );
  }	   

}

void AmdcDumpGeoModel::RpcCompare(
 std::ofstream& OutFile              ,
 double         EpsLoRpc             ,
 int            WarningLevel         ,
 int            SPstationName        ,
 std::string    SPstationNameString  ,
 int            SPstationEta         ,
 int            SPstationPhi         ,
 int            SPDoubletR           ,
 int            SPDoubletZ           ,
 int            SPDoubletPhi         ,
 int            SPGasGap             ,
 int            SPMeasuresPhi        ,
 int            SPStrip              ,
 double         GeoModX              ,
 double         GeoModY              ,
 double         GeoModZ                  
){

   Amdcsimrec* pAmdcsimrec=p_AmdcsimrecAthenaSvc->GetAmdcsimrec();

   int AmdcJff   ;
   int AmdcJzz   ; 
   int AmdcJob   ;
   int AmdcJspli ;
   int AmdcJsl   ;
   int AmdcJsz   ;
   int AmdcJstri ;
   int AmdcJtyp  ;
   std::string AmdcStationName = SPstationNameString ;
   AmdcJtyp=pAmdcsimrec->GetJtyp(SPstationNameString);
   pAmdcsimrec->GetAMDCindexFromRPC(SPstationNameString,
                                     SPstationEta, 
			             SPstationPhi,
       			             SPDoubletR, 
       			             SPDoubletZ, 
       			             SPDoubletPhi,
        		             SPGasGap, 
        		             SPMeasuresPhi, 
        		             SPStrip,
                                     AmdcJff, 
                                     AmdcJzz, 
                                     AmdcJob, 
                                     AmdcJspli, 
                                     AmdcJsl, 
                                     AmdcJsz, 
                                     AmdcJstri);
   double FF0 = pAmdcsimrec->PosPhi(AmdcJtyp,AmdcJff,AmdcJzz);
   double DCF = cos(FF0);
   double DSF = sin(FF0);
   double GeoModS = DCF * GeoModY - DSF * GeoModX ;
   double GeoModT = DCF * GeoModX + DSF * GeoModY ;
   double XYZ1[3] ;
   double XYZ2[3] ;
   double XYZ3[3] ;
   double XYZ4[3] ;
   double SZT1[3] ;
   double SZT2[3] ;
   double SZT3[3] ;
   double SZT4[3] ;
   pAmdcsimrec->GetStripPositionFromRPCindex(SPstationNameString,
                                              SPstationEta, 
			                      SPstationPhi,
       			                      SPDoubletR, 
       			                      SPDoubletZ, 
       			                      SPDoubletPhi,
        		                      SPGasGap, 
        		                      SPMeasuresPhi, 
        		                      SPStrip,
       		                              XYZ1,
       		                              XYZ2,
        		                      XYZ3,
        		                      XYZ4,
        		                      SZT1,
        		                      SZT2,
        		                      SZT3,
        		                      SZT4);
   double AmdcX = ( XYZ1[0] + XYZ2[0] + XYZ3[0] + XYZ4[0] )/4.;
   double AmdcY = ( XYZ1[1] + XYZ2[1] + XYZ3[1] + XYZ4[1] )/4.;
   double AmdcZ = ( XYZ1[2] + XYZ2[2] + XYZ3[2] + XYZ4[2] )/4.;
   double AmdcS = DCF * AmdcY - DSF * AmdcX ;
   double AmdcT = DCF * AmdcX + DSF * AmdcY ;
   double DiffX = GeoModX - AmdcX ;
   double DiffY = GeoModY - AmdcY ;
   double DiffZ = GeoModZ - AmdcZ ;
   double DiffS = GeoModS - AmdcS ;
   double DiffT = GeoModT - AmdcT ;
   std::string WarningX = " ";
   std::string WarningY = " ";
   std::string WarningZ = " ";
   std::string WarningS = " ";
   std::string WarningT = " ";
   int Iwarning = 0 ;

   if (std::abs(DiffZ) > EpsLoRpc ) WarningZ = "  <=========  WARNING !";
   if (std::abs(DiffS) > EpsLoRpc ) WarningS = "  <=========  WARNING !";
   if (std::abs(DiffT) > EpsLoRpc ) WarningT = "  <=========  WARNING !";
   if (std::abs(DiffZ) > EpsLoRpc ) Iwarning = 1 ;
   if (std::abs(DiffS) > EpsLoRpc ) Iwarning = 1 ;
   if (std::abs(DiffT) > EpsLoRpc ) Iwarning = 1 ;

   if (AmdcJsz == 2 && std::abs(DiffZ) > EpsLoRpc ) WarningZ = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcJsz == 1 && std::abs(DiffS) > EpsLoRpc ) WarningS = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcJsz == 2 && std::abs(DiffZ) > EpsLoRpc ) Iwarning = 2 ;
   if (AmdcJsz == 1 && std::abs(DiffS) > EpsLoRpc ) Iwarning = 2 ;

   if (Iwarning >= WarningLevel 
   && SelectedStation(AmdcStationName) == 1 
   && AntiSelectedStation(AmdcStationName) == 1 
   && SelectedChamber(AmdcStationName,AmdcJzz,AmdcJff) == 1 ) {

    if ( std::abs(DiffZ) > std::abs(m_Rpc_MaxDiffZ) )  m_Rpc_MaxDiffZ = DiffZ ;
    if ( std::abs(DiffS) > std::abs(m_Rpc_MaxDiffS) )  m_Rpc_MaxDiffS = DiffS ;
    if ( std::abs(DiffT) > std::abs(m_Rpc_MaxDiffT) )  m_Rpc_MaxDiffT = DiffT ;

    OutFile 
                 << "*StNa/Name/Eta/Phi/DbR/DbZ/DbP/GG /Mphi/Strip/" 
                 << std::setw(4)<< SPstationName	    
                 << std::setw(3)<< SPstationNameString 
                 << std::setw(4)<< SPstationEta	     
                 << std::setw(4)<< SPstationPhi	     
                 << std::setw(4)<< SPDoubletR	     
                 << std::setw(4)<< SPDoubletZ	     
                 << std::setw(4)<< SPDoubletPhi	     
                 << std::setw(4)<< SPGasGap   
                 << std::setw(4)<< SPMeasuresPhi   
                 << std::setw(4)<< SPStrip   
                 << std::endl;
    OutFile 
                 << " Jtyp/Name/Jzz/Jff/ Job/ Jspli/Jsl/Jsz /Jstri/" 
                 << std::setw(4)<< AmdcJtyp	   
                 << std::setw(3)<< AmdcStationName 
                 << std::setw(4)<< AmdcJzz	    
                 << std::setw(4)<< AmdcJff	    
                 << std::setw(6)<< AmdcJob	    
                 << std::setw(6)<< AmdcJspli	    
                 << std::setw(4)<< AmdcJsl	    
                 << std::setw(4)<< AmdcJsz	    
                 << std::setw(4)<< AmdcJstri 
                 << std::endl;
    OutFile 
    		 << " GM/Am/GM-Am/X " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModX       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcX       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffX 
	  	 <<					WarningX  
    		 << std::endl;
    OutFile 
    		 << "             Y " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModY       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcY       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffY   
	  	 <<					WarningY  
    		 << std::endl;
    OutFile 
    		 << "             Z " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffZ    
	  	 <<					WarningZ  
    		 << std::endl;
    OutFile 
    		 << "             S " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModS       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcS       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffS   
	  	 <<					WarningS  
    		 << std::endl;
    OutFile 
    		 << "             T " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModT       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcT       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffT    
	  	 <<					WarningT  
    		 << std::endl;
   }

}

void AmdcDumpGeoModel::LoopTgcElements(std::ofstream&  OutFile){

  m_Tgc_MaxDiffZ  = 0. ;
  m_Tgc_MaxDiffS  = 0. ; 
  m_Tgc_MaxDiffT  = 0. ; 

  for (int sname_index = 0; sname_index<MuonGM::MuonDetectorManager::NTgcStatType; ++ sname_index) 
  {
   for (int seta_index = 5; seta_index<MuonGM::MuonDetectorManager::NTgcStatEta; ++seta_index)
   {
    for (int sphi_index = 0; sphi_index<MuonGM::MuonDetectorManager::NTgcStatPhi; ++sphi_index)
    {
     const MuonGM::TgcReadoutElement* pReadoutElement = 
         p_MuonDetectorManager->getTgcReadoutElement(sname_index,
     						     seta_index,
     						     sphi_index);
     if (pReadoutElement == NULL) continue;
     Identifier idr  = pReadoutElement->identify();
     Identifier idp  = m_idHelperSvc->tgcIdHelper().parentID(idr);
     Identifier idp1 = m_idHelperSvc->tgcIdHelper().elementID(m_idHelperSvc->tgcIdHelper().stationName(idp),
     					  -m_idHelperSvc->tgcIdHelper().stationEta(idp),
     					  m_idHelperSvc->tgcIdHelper().stationPhi(idp));
     const MuonGM::TgcReadoutElement* pReadoutElement1 = 
         p_MuonDetectorManager->getTgcReadoutElement(idp1);

     for (int ngg=0; ngg<pReadoutElement->NwirePlanes(); ++ngg)
     {
      int isStrip = 0 ;
      for (int channel = 1; channel<=pReadoutElement->getNGangs(ngg+1);)
      {
       Identifier channelId = m_idHelperSvc->tgcIdHelper().channelID(idp,ngg+1, isStrip, channel);
       bool IsValid = m_idHelperSvc->tgcIdHelper().valid(channelId);
       if (IsValid) {
        TgcCompare(OutFile,channelId,pReadoutElement);
       }else{
        OutFile 
		      << "*sname_index/seta_index/sphi_index/ngg/isStrip/channel"	
		      << std::endl;
        OutFile 
		      << " idp " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index    
        	      << " " << ngg << " " << isStrip << " " << channel	
		      << std::endl;
	OutFile 
		      << " Not Valid??????????"
		      << std::endl;
       }
       channel += pReadoutElement->getNGangs(ngg+1)-1;
       if (channel == 1 ){
        channel = 2 ;
        OutFile  
                << " getNGangs " << pReadoutElement->getNGangs(ngg+1) 
		<< " Is this expected? " 
		<< " " << m_idHelperSvc->tgcIdHelper().show_to_string(channelId)
	        << std::endl; 
	
       }
      }
      for (int channel = 1; channel<=pReadoutElement1->getNGangs(ngg+1);)
      {
       Identifier channelId = m_idHelperSvc->tgcIdHelper().channelID(idp1,ngg+1, isStrip, channel);
       bool IsValid = m_idHelperSvc->tgcIdHelper().valid(channelId);
       if (IsValid) {
        TgcCompare(OutFile,channelId,pReadoutElement1);
       }else{
        OutFile 
		      << "*sname_index/seta_index/sphi_index/ngg/isStrip/channel"	
		      << std::endl;
        OutFile 
		      << " idp1 " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index    
        	      << " " << ngg << " " << isStrip << " " << channel	
		      << std::endl;
	OutFile 
		      << " Not Valid??????????"
		      << std::endl;
       }
       channel += pReadoutElement1->getNGangs(ngg+1)-1;
       if (channel == 1 ){
        channel = 2 ;
        OutFile  
                << " getNGangs " << pReadoutElement1->getNGangs(ngg+1) 
		<< " Is this expected? " 
		<< " " << m_idHelperSvc->tgcIdHelper().show_to_string(channelId)
	        << std::endl; 
	
       }
      }
      isStrip = 1 ;
      for (int channel = 1; channel<=pReadoutElement->getNStrips(ngg+1);)
      {
       Identifier channelId = m_idHelperSvc->tgcIdHelper().channelID(idp, ngg+1, isStrip, channel);
       bool IsValid = m_idHelperSvc->tgcIdHelper().valid(channelId);
       if (IsValid) {
        TgcCompare(OutFile,channelId,pReadoutElement);
       }else{
        OutFile 
		      << "*sname_index/seta_index/sphi_index/ngg/isStrip/channel"	
		      << std::endl;
        OutFile 
		      << " idp " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index    
        	      << " " << ngg << " " << isStrip << " " << channel	
		      << std::endl;
	OutFile 
		      << " Not Valid??????????"
		      << std::endl;
       }
       channel += pReadoutElement->getNStrips(ngg+1)-1;
       if (channel == 1 ){
        channel = 2 ;
       }
      }
      for (int channel = 1; channel<=pReadoutElement1->getNStrips(ngg+1);)
      {
       Identifier channelId = m_idHelperSvc->tgcIdHelper().channelID(idp1,ngg+1, isStrip, channel);
       bool IsValid = m_idHelperSvc->tgcIdHelper().valid(channelId);
       if (IsValid) {
        TgcCompare(OutFile,channelId,pReadoutElement1);
       }else{
        OutFile 
		      << "*sname_index/seta_index/sphi_index/ngg/isStrip/channel"	
		      << std::endl;
        OutFile 
		      << " idp1 " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index    
        	      << " " << ngg << " " << isStrip << " " << channel	
		      << std::endl;
	OutFile 
		      << " Not Valid??????????"
		      << std::endl;
       }
       channel += pReadoutElement1->getNStrips(ngg+1)-1;
       if (channel == 1 ){
        channel = 2 ;
//         OutFile  
//                 << " getNStrips " << pReadoutElement1->getNStrips(ngg+1) 
// 		<< " Is this expected? " 
// 		<< " " << m_idHelperSvc->tgcIdHelper().show_to_string(channelId)
// 	        << std::endl; 
	
       }
      }
     }
    }
   }
  }
  
  OutFile 
         << "Max deviations " 
         << std::setw(12)<<std::setprecision(3)<<  m_Tgc_MaxDiffZ  
         << std::setw(12)<<std::setprecision(3)<<  m_Tgc_MaxDiffS  
         << std::setw(12)<<std::setprecision(3)<<  m_Tgc_MaxDiffT  
	 << std::endl;

}

void AmdcDumpGeoModel::TgcCompare(
 std::ofstream&           OutFile,
 Identifier               channelId,
 const MuonGM::TgcReadoutElement* pReadoutElement){

  bool IsValid = m_idHelperSvc->tgcIdHelper().valid(channelId);
  if (IsValid) {
   int         SPstationName       = m_idHelperSvc->tgcIdHelper().stationName(channelId);
   std::string SPstationNameString = m_idHelperSvc->tgcIdHelper().stationNameString(SPstationName);
   int         SPstationEta        = m_idHelperSvc->tgcIdHelper().stationEta(channelId);
   int         SPstationPhi        = m_idHelperSvc->tgcIdHelper().stationPhi(channelId);
   int         SPGasGap            = m_idHelperSvc->tgcIdHelper().gasGap(channelId);
   int         SPIsStrip           = m_idHelperSvc->tgcIdHelper().isStrip(channelId);
   int         SPChannel           = m_idHelperSvc->tgcIdHelper().channel(channelId);

   const Amg::Vector3D globalPosition = pReadoutElement->channelPos(channelId);
   double GeoModX = globalPosition.x();
   double GeoModY = globalPosition.y();
   double GeoModZ = globalPosition.z();

   TgcCompare(
              OutFile             ,
              m_EpsLoTgc          ,
              m_WarningLevel      ,
              SPstationName       ,
              SPstationNameString ,
              SPstationEta        ,
              SPstationPhi        ,
              SPGasGap            ,
              SPIsStrip           ,
              SPChannel           ,
              GeoModX             ,
              GeoModY             ,
              GeoModZ              
             );
  }
  
}

void AmdcDumpGeoModel::TgcCompare(
 std::ofstream& OutFile             ,
 double         EpsLoTgc            ,
 int            WarningLevel        ,
 int            SPstationName       ,
 std::string    SPstationNameString ,
 int            SPstationEta        ,
 int            SPstationPhi        ,
 int            SPGasGap            ,
 int            SPIsStrip           ,
 int            SPChannel           ,
 double         GeoModX             ,
 double         GeoModY             ,
 double         GeoModZ              
){

   Amdcsimrec* pAmdcsimrec=p_AmdcsimrecAthenaSvc->GetAmdcsimrec();

   int AmdcJff   ;
   int AmdcJzz   ; 
   int AmdcJob   ;
   int AmdcJspli ;
   int AmdcJsl   ;
   int AmdcJsz   ;
   int AmdcJstri ;
   int AmdcJtyp  ;
   std::string AmdcStationName = SPstationNameString ;
   AmdcJtyp=pAmdcsimrec->GetJtyp(SPstationNameString);
   pAmdcsimrec->GetAMDCindexFromTGC(SPstationNameString,
                                     SPstationEta, 
			             SPstationPhi,
				     SPGasGap, 
				     SPIsStrip, 
				     SPChannel,
                                     AmdcJff, 
                                     AmdcJzz, 
                                     AmdcJob, 
                                     AmdcJspli, 
                                     AmdcJsl, 
                                     AmdcJsz, 
                                     AmdcJstri);
   int    AmdcJgeo = pAmdcsimrec->IGEO(AmdcJtyp,AmdcJff,AmdcJzz) ;
   double Z0       = pAmdcsimrec->PosZ(AmdcJtyp,AmdcJff,AmdcJzz);
   double FF0      = pAmdcsimrec->PosPhi(AmdcJtyp,AmdcJff,AmdcJzz);
   if (Z0 > 0.0 ){
    FF0 = FF0 + pAmdcsimrec->Geodx(AmdcJtyp,AmdcJgeo,AmdcJob)*acos(-1.0)/180.0 ;
   }else{
    FF0 = FF0 - pAmdcsimrec->Geodx(AmdcJtyp,AmdcJgeo,AmdcJob)*acos(-1.0)/180.0 ;
   }
   double DCF = cos(FF0);
   double DSF = sin(FF0);
   double GeoModS = DCF * GeoModY - DSF * GeoModX ;
   double GeoModT = DCF * GeoModX + DSF * GeoModY ;
   double XYZ1[3] ;
   double XYZ2[3] ;
   double XYZ3[3] ;
   double XYZ4[3] ;
   double SZT1[3] ;
   double SZT2[3] ;
   double SZT3[3] ;
   double SZT4[3] ;
   pAmdcsimrec->GetChannelPositionFromTGCindex(SPstationNameString,
                                                SPstationEta, 
			                        SPstationPhi,
        		                        SPGasGap, 
        		                        SPIsStrip, 
        		                        SPChannel,
       		                                XYZ1,
       		                                XYZ2,
        		                        XYZ3,
        		                        XYZ4,
        		                        SZT1,
        		                        SZT2,
        		                        SZT3,
        		                        SZT4);
   double AmdcX = ( XYZ1[0] + XYZ2[0] + XYZ3[0] + XYZ4[0] )/4.;
   double AmdcY = ( XYZ1[1] + XYZ2[1] + XYZ3[1] + XYZ4[1] )/4.;
   double AmdcZ = ( XYZ1[2] + XYZ2[2] + XYZ3[2] + XYZ4[2] )/4.;
   double AmdcS = DCF * AmdcY - DSF * AmdcX ;
   double AmdcT = DCF * AmdcX + DSF * AmdcY ;
   double DiffX = GeoModX - AmdcX ;
   double DiffY = GeoModY - AmdcY ;
   double DiffZ = GeoModZ - AmdcZ ;
   double DiffS = GeoModS - AmdcS ;
   double DiffT = GeoModT - AmdcT ;
   std::string WarningX = " ";
   std::string WarningY = " ";
   std::string WarningZ = " ";
   std::string WarningS = " ";
   std::string WarningT = " ";
   int Iwarning = 0 ;

   if (std::abs(DiffZ) > EpsLoTgc ) WarningZ = "  <=========  WARNING !";
   if (std::abs(DiffS) > EpsLoTgc ) WarningS = "  <=========  WARNING !";
   if (std::abs(DiffT) > EpsLoTgc ) WarningT = "  <=========  WARNING !";
   if (std::abs(DiffZ) > EpsLoTgc ) Iwarning = 1 ;
   if (std::abs(DiffS) > EpsLoTgc ) Iwarning = 1 ;
   if (std::abs(DiffT) > EpsLoTgc ) Iwarning = 1 ;

   if (AmdcJsz == 1 && std::abs(DiffS) > EpsLoTgc ) WarningS = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcJsz == 2 && std::abs(DiffT) > EpsLoTgc ) WarningT = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcJsz == 1 && std::abs(DiffS) > EpsLoTgc ) Iwarning = 2 ;
   if (AmdcJsz == 2 && std::abs(DiffT) > EpsLoTgc ) Iwarning = 2 ;

   if (Iwarning >= WarningLevel 
   && SelectedStation(AmdcStationName) == 1 
   && AntiSelectedStation(AmdcStationName) == 1 
   && SelectedChamber(AmdcStationName,AmdcJzz,AmdcJff) == 1 ) {

    if ( std::abs(DiffZ) > std::abs(m_Tgc_MaxDiffZ) )  m_Tgc_MaxDiffZ = DiffZ ;
    if ( std::abs(DiffS) > std::abs(m_Tgc_MaxDiffS) )  m_Tgc_MaxDiffS = DiffS ;
    if ( std::abs(DiffT) > std::abs(m_Tgc_MaxDiffT) )  m_Tgc_MaxDiffT = DiffT ;

    OutFile 
                 << "*StNa/Name/Eta/Phi/GG             /IsSt/Chann/" 
                 << std::setw(4)<< SPstationName	    
                 << std::setw(3)<< SPstationNameString 
                 << std::setw(4)<< SPstationEta	     
                 << std::setw(4)<< SPstationPhi	     
                 << std::setw(16)<< SPGasGap   
                 << std::setw(4)<< SPIsStrip   
                 << std::setw(4)<< SPChannel   
                 << std::endl;
    OutFile 
                 << " Jtyp/Name/Jzz/Jff/ Job/ Jspli/Jsl/Jsz /Jstri/" 
                 << std::setw(4)<< AmdcJtyp	   
                 << std::setw(3)<< AmdcStationName 
                 << std::setw(4)<< AmdcJzz	    
                 << std::setw(4)<< AmdcJff	    
                 << std::setw(6)<< AmdcJob	    
                 << std::setw(6)<< AmdcJspli	    
                 << std::setw(4)<< AmdcJsl	    
                 << std::setw(4)<< AmdcJsz	    
                 << std::setw(4)<< AmdcJstri 
                 << std::endl;
    OutFile 
    		 << " GM/Am/GM-Am/X " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModX       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcX       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffX 
	  	 <<					WarningX  
    		 << std::endl;
    OutFile 
    		 << "             Y " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModY       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcY       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffY   
	  	 <<					WarningY  
    		 << std::endl;
    OutFile 
    		 << "             Z " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffZ    
	  	 <<					WarningZ  
    		 << std::endl;
    OutFile 
    		 << "             S " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModS       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcS       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffS   
	  	 <<					WarningS  
    		 << std::endl;
    OutFile 
    		 << "             T " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModT       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcT       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffT    
	  	 <<					WarningT  
    		 << std::endl;
   }

}

void AmdcDumpGeoModel::LoopCscElements(std::ofstream&  OutFile){

  m_Csc_MaxDiffss  = 0. ;
  m_Csc_MaxDiffzz  = 0. ; 
  m_Csc_MaxDifftt  = 0. ; 

  for (int sname_index = 0; sname_index<MuonGM::MuonDetectorManager::NCscStatType; ++ sname_index) 
  {
   for (int seta_index = 1; seta_index<MuonGM::MuonDetectorManager::NCscStatEta; ++seta_index)
   {
    for (int sphi_index = 0; sphi_index<MuonGM::MuonDetectorManager::NCscStatPhi; ++sphi_index)
    {
     for (int ml=0; ml<MuonGM::MuonDetectorManager::NCscChamberLayer; ++ml)
     {
      const MuonGM::CscReadoutElement* pReadoutElement = 
          p_MuonDetectorManager->getCscReadoutElement(sname_index,
     	     					      seta_index,
     	     					      sphi_index,
     	     					      ml);
      if (pReadoutElement == NULL) continue;

      Identifier idr  = pReadoutElement->identify();
      Identifier idp  = m_idHelperSvc->cscIdHelper().parentID(idr);
      Identifier idp1 = m_idHelperSvc->cscIdHelper().elementID(m_idHelperSvc->cscIdHelper().stationName(idp),
     	     				   -m_idHelperSvc->cscIdHelper().stationEta(idp),
     	     				   m_idHelperSvc->cscIdHelper().stationPhi(idp));
      Identifier idp1ch = m_idHelperSvc->cscIdHelper().channelID(idp1, 
                                             pReadoutElement->ChamberLayer(), 
					     1, 0, 1);
//Why is it not getCscReadoutElement(idp1) JFL(Tue Nov 30 10:17:40 CET 2004)
      const MuonGM::CscReadoutElement* pReadoutElement1 = 
          p_MuonDetectorManager->getCscReadoutElement(idp1ch);

      for (int gg=1; gg<=pReadoutElement->Ngasgaps(); ++gg)
      {
       int MeasuresPhi = 0 ;
       for (int Strip=1; Strip<=pReadoutElement->NetaStrips(gg);)
       {
        Identifier channelId = m_idHelperSvc->cscIdHelper().channelID(idp, ml+1, gg, MeasuresPhi, Strip);
        bool IsValid = m_idHelperSvc->cscIdHelper().valid(channelId);
        if (IsValid) {
         CscCompare(OutFile,channelId,pReadoutElement);
        }else{
         OutFile 
		      << "*sname_index/seta_index/sphi_index/ml/MeasuresPhi/gg/Strip"	
		      << std::endl;
         OutFile 
		      << " idp " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index << " " << ml   
        	      << " " << MeasuresPhi << " " << gg  << " " << Strip	
		      << std::endl;
	 OutFile 
		      << " Not Valid??????????"
		      << std::endl;
        }
        Strip += pReadoutElement->NetaStrips(gg)-1;
        if (Strip == 1 ){
         Strip = 2 ;
         OutFile  
                 << " NetaStrips " << pReadoutElement->NetaStrips(gg) 
		 << " Is this expected? " 
		 << " " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
	         << std::endl; 
	
        }
       }
       for (int Strip=1; Strip<=pReadoutElement1->NetaStrips(gg);)
       {
        Identifier channelId = m_idHelperSvc->cscIdHelper().channelID(idp1, ml+1, gg, MeasuresPhi, Strip);
        bool IsValid = m_idHelperSvc->cscIdHelper().valid(channelId);
        if (IsValid) {
         CscCompare(OutFile,channelId,pReadoutElement1);
        }else{
         OutFile 
		      << "*sname_index/seta_index/sphi_index/ml/MeasuresPhi/gg/Strip"	
		      << std::endl;
         OutFile 
		      << " idp1 " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index << " " << ml   
        	      << " " << MeasuresPhi << " " << gg  << " " << Strip	
		      << std::endl;
	 OutFile 
		      << " Not Valid??????????"
		      << std::endl;
        }
        Strip += pReadoutElement1->NetaStrips(gg)-1;
        if (Strip == 1 ){
         Strip = 2 ;
         OutFile  
                 << " NetaStrips " << pReadoutElement1->NetaStrips(gg) 
		 << " Is this expected? " 
		 << " " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
	         << std::endl; 
	
        }
       }
       MeasuresPhi = 1 ;
       for (int Strip=1; Strip<=pReadoutElement->NphiStrips(gg);)
       {
        Identifier channelId = m_idHelperSvc->cscIdHelper().channelID(idp, ml+1, gg, MeasuresPhi, Strip);
        bool IsValid = m_idHelperSvc->cscIdHelper().valid(channelId);
        if (IsValid) {
         CscCompare(OutFile,channelId,pReadoutElement);
        }else{
         OutFile 
		      << "*sname_index/seta_index/sphi_index/ml/MeasuresPhi/gg/Strip"	
		      << std::endl;
         OutFile 
		      << " idp " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index << " " << ml   
        	      << " " << MeasuresPhi << " " << gg  << " " << Strip	
		      << std::endl;
	 OutFile 
		      << " Not Valid??????????"
		      << std::endl;
        }
        if (pReadoutElement->NphiStrips(gg) == 1 ){
         Strip = 2 ;
         OutFile  
                 << " NphiStrips " << pReadoutElement->NphiStrips(gg) 
		 << " Is this expected? " 
		 << " " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
	         << std::endl; 
	
        }else{
	 if (pReadoutElement->NphiStrips(gg) > 2){
	  if (Strip==1){
           Strip = (int) ( pReadoutElement->NphiStrips(gg)/2. + 0.5 );
	  }else{
	   if (Strip==pReadoutElement->NphiStrips(gg)){
	    Strip = Strip + 1 ;
	   }else{
            Strip = pReadoutElement->NphiStrips(gg);
	   }
	  }
	 }else{
	  if (Strip==pReadoutElement->NphiStrips(gg)){
	   Strip = Strip + 1 ;
	  }else{
           Strip = pReadoutElement->NphiStrips(gg);
	  }
	 }
	}
       }
       for (int Strip=1; Strip<=pReadoutElement1->NphiStrips(gg);)
       {
        Identifier channelId = m_idHelperSvc->cscIdHelper().channelID(idp1, ml+1, gg, MeasuresPhi, Strip);
        bool IsValid = m_idHelperSvc->cscIdHelper().valid(channelId);
        if (IsValid) {
         CscCompare(OutFile,channelId,pReadoutElement1);
        }else{
         OutFile 
		      << "*sname_index/seta_index/sphi_index/ml/MeasuresPhi/gg/Strip"	
		      << std::endl;
         OutFile 
		      << " idp1 " 
		      << " " << sname_index << " " << seta_index << " " << sphi_index << " " << ml   
        	      << " " << MeasuresPhi << " " << gg  << " " << Strip	
		      << std::endl;
	 OutFile 
		      << " Not Valid??????????"
		      << std::endl;
        }
        if (pReadoutElement->NphiStrips(gg) == 1 ){
         Strip = 2 ;
         OutFile  
                 << " NphiStrips " << pReadoutElement1->NphiStrips(gg) 
		 << " Is this expected? " 
		 << " " << m_idHelperSvc->cscIdHelper().show_to_string(channelId)
	         << std::endl; 
	
        }else{
	 if (pReadoutElement->NphiStrips(gg) > 2){
	  if (Strip==1){
           Strip = (int) ( pReadoutElement->NphiStrips(gg)/2. + 0.5 );
	  }else{
	   if (Strip==pReadoutElement->NphiStrips(gg)){
	    Strip = Strip + 1 ;
	   }else{
            Strip = pReadoutElement->NphiStrips(gg);
	   }
	  }
	 }else{
	  if (Strip==pReadoutElement->NphiStrips(gg)){
	   Strip = Strip + 1 ;
	  }else{
           Strip = pReadoutElement->NphiStrips(gg);
	  }
	 }
	}
       }
      }
     }
    }
   }
  }
  
  OutFile 
         << "Max deviations " 
         << std::setw(12)<<std::setprecision(3)<<  m_Csc_MaxDiffss  
         << std::setw(12)<<std::setprecision(3)<<  m_Csc_MaxDiffzz  
         << std::setw(12)<<std::setprecision(3)<<  m_Csc_MaxDifftt  
	 << std::endl;

}

void AmdcDumpGeoModel::CscCompare(
 std::ofstream&           OutFile,
 Identifier               channelId,
 const MuonGM::CscReadoutElement* pReadoutElement){

  bool IsValid = m_idHelperSvc->cscIdHelper().valid(channelId);
  if (IsValid) {
   int          SPstationName       = m_idHelperSvc->cscIdHelper().stationName(channelId);
   std::string  SPstationNameString = m_idHelperSvc->cscIdHelper().stationNameString(SPstationName);
   int          SPstationEta        = m_idHelperSvc->cscIdHelper().stationEta(channelId);
   int          SPstationPhi        = m_idHelperSvc->cscIdHelper().stationPhi(channelId);
   int          SPChamberLayer      = m_idHelperSvc->cscIdHelper().chamberLayer(channelId);
   int          SPWireLayer	    = m_idHelperSvc->cscIdHelper().wireLayer(channelId);
   int          SPMeasuresPhi	    = m_idHelperSvc->cscIdHelper().measuresPhi(channelId);
   int          SPStrip	            = m_idHelperSvc->cscIdHelper().strip(channelId);
 
   const Amg::Vector3D globalPosition = pReadoutElement->stripPos(channelId);
   double GeoModX = globalPosition.x();
   double GeoModY = globalPosition.y();
   double GeoModZ = globalPosition.z();

   CscCompare(
              OutFile             ,
              m_EpsLoCsc          ,
              m_WarningLevel      ,
              SPstationName       ,
              SPstationNameString ,
              SPstationEta        ,
              SPstationPhi        ,
              SPChamberLayer      ,
              SPWireLayer         ,
              SPMeasuresPhi       ,
              SPStrip             ,
              GeoModX             ,
              GeoModY             ,
              GeoModZ                       
             );
  }

}

void AmdcDumpGeoModel::CscCompare(
 std::ofstream& OutFile             ,
 double         EpsLoCsc            ,
 int            WarningLevel        ,
 int            SPstationName       ,
 std::string    SPstationNameString ,
 int            SPstationEta        ,
 int            SPstationPhi        ,
 int            SPChamberLayer      ,
 int            SPWireLayer         ,
 int            SPMeasuresPhi       ,
 int            SPStrip             ,
 double         GeoModX             ,
 double         GeoModY             ,
 double         GeoModZ                       
){


   Amdcsimrec* pAmdcsimrec=p_AmdcsimrecAthenaSvc->GetAmdcsimrec();

   int    AmdcJff    ;
   int    AmdcJzz    ; 
   int    AmdcJob    ;
   int    AmdcJsl    ;
   int    AmdcJtube  ;
   double AmdcSZflag ;
   int    AmdcJtyp   ;
   std::string AmdcStationName = SPstationNameString ;
   AmdcJtyp=pAmdcsimrec->GetJtyp(SPstationNameString);
   pAmdcsimrec->GetAMDCindexFromCSC(SPstationNameString,
                                     SPstationEta, 
	                             SPstationPhi, 
	                             SPChamberLayer, 
	                             SPWireLayer,
                                     SPMeasuresPhi, 
	                             SPStrip,
                                     AmdcJff, 
	                             AmdcJzz, 
	                             AmdcJob, 
	                             AmdcJsl, 
	                             AmdcJtube, 
	                             AmdcSZflag);
   double FF0 = pAmdcsimrec->PosPhi(AmdcJtyp,AmdcJff,AmdcJzz);
   double DCF = cos(FF0);
   double DSF = sin(FF0);
   double SIZ0 = 1.0 ;
   if ( pAmdcsimrec->PosZ(AmdcJtyp,AmdcJff,AmdcJzz) < 0 ) SIZ0 = -SIZ0 ;
   double AA0 = SIZ0*pAmdcsimrec->PosGama(AmdcJtyp,AmdcJff,AmdcJzz) * M_PI / 180. ;
   double DCA = cos(AA0);
   double DSA = sin(AA0);
   double GeoModT =   DCF * GeoModX + DSF * GeoModY ;
   double GeoModS = - DSF * GeoModX + DCF * GeoModY  ;
   double GeoModss= - GeoModS ;
   double GeoModzz=   DSA * GeoModZ + DCA * GeoModT ;
   double GeoModtt=   DCA * GeoModZ - DSA * GeoModT ;
   double XYZ1[3] ;
   double XYZ2[3] ;
   double XYZ3[3] ;
   double XYZ4[3] ;
   double XYZCenter[3] ;
   double SZT1[3] ;
   double SZT2[3] ;
   double SZT3[3] ;
   double SZT4[3] ;
   double SZTCenter[3] ;
   pAmdcsimrec->GetStripPositionFromCSCindex(SPstationNameString,
                                              SPstationEta, 
	                                      SPstationPhi, 
	                                      SPChamberLayer, 
	                                      SPWireLayer,
                                              SPMeasuresPhi, 
	                                      SPStrip,
       		                              XYZ1,
       		                              XYZ2,
        		                      XYZ3,
        		                      XYZ4,
					      XYZCenter,
        		                      SZT1,
        		                      SZT2,
        		                      SZT3,
        		                      SZT4,
					      SZTCenter);
   double AmdcX = XYZCenter[0];
   double AmdcY = XYZCenter[1];
   double AmdcZ = XYZCenter[2];
   if (m_CenterCscMean == 1){
    AmdcX = ( XYZ1[0] + XYZ2[0] + XYZ3[0] + XYZ4[0] )/4.;
    AmdcY = ( XYZ1[1] + XYZ2[1] + XYZ3[1] + XYZ4[1] )/4.;
    AmdcZ = ( XYZ1[2] + XYZ2[2] + XYZ3[2] + XYZ4[2] )/4.;
   }
   double AmdcT =   DCF * AmdcX + DSF * AmdcY ;
   double AmdcS = - DSF * AmdcX + DCF * AmdcY ;
   double Amdcss= - AmdcS ;
   double Amdczz=   DSA * AmdcZ + DCA * AmdcT ;
   double Amdctt=   DCA * AmdcZ - DSA * AmdcT ;

   double DiffX = GeoModX  - AmdcX ;
   double DiffY = GeoModY  - AmdcY ;
   double DiffZ = GeoModZ  - AmdcZ ;
   double Diffss= GeoModss - Amdcss ;
   double Diffzz= GeoModzz - Amdczz ;
   double Difftt= GeoModtt - Amdctt ;
   std::string WarningX = " ";
   std::string WarningY = " ";
   std::string WarningZ = " ";
   std::string Warningss= " ";
   std::string Warningzz= " ";
   std::string Warningtt= " ";
   int Iwarning = 0 ;

   if (std::abs(Diffss)> EpsLoCsc ) Warningss = "  <=========  WARNING !";
   if (std::abs(Diffzz)> EpsLoCsc ) Warningzz = "  <=========  WARNING !";
   if (std::abs(Difftt)> EpsLoCsc ) Warningtt = "  <=========  WARNING !";
   if (std::abs(Diffss)> EpsLoCsc ) Iwarning = 1 ;
   if (std::abs(Diffzz)> EpsLoCsc ) Iwarning = 1 ;
   if (std::abs(Difftt)> EpsLoCsc ) Iwarning = 1 ;

   if (AmdcSZflag < 0 && std::abs(Diffss)> EpsLoCsc ) Warningss = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcSZflag > 0 && std::abs(Diffzz)> EpsLoCsc ) Warningzz = "  <=========  WARNING ! (precise Measurement)";
   if (AmdcSZflag < 0 && std::abs(Diffss)> EpsLoCsc ) Iwarning = 2 ;
   if (AmdcSZflag > 0 && std::abs(Diffzz)> EpsLoCsc ) Iwarning = 2 ;

   if (Iwarning >= WarningLevel 
   && SelectedStation(AmdcStationName) == 1 
   && AntiSelectedStation(AmdcStationName) == 1 
   && SelectedChamber(AmdcStationName,AmdcJzz,AmdcJff) == 1 ) {

    if ( std::abs(Diffss) > std::abs(m_Csc_MaxDiffss) )  m_Csc_MaxDiffss = Diffss ;
    if ( std::abs(Diffzz) > std::abs(m_Csc_MaxDiffzz) )  m_Csc_MaxDiffzz = Diffzz ;
    if ( std::abs(Difftt) > std::abs(m_Csc_MaxDifftt) )  m_Csc_MaxDifftt = Difftt ;

    OutFile 
                 << "*StNa/Name/Eta/Phi/chLa/wLa/Strip/meaPhi/" 
                 << std::setw(4)<< SPstationName	    
                 << std::setw(3)<< SPstationNameString 
                 << std::setw(4)<< SPstationEta	     
                 << std::setw(4)<< SPstationPhi	     
                 << std::setw(4)<< SPChamberLayer   
                 << std::setw(4)<< SPWireLayer   
                 << std::setw(4)<< SPStrip   
                 << std::setw(4)<< SPMeasuresPhi   
                 << std::endl;
    OutFile 
                 << " Jtyp/Name/Jzz/Jff/ Job/Jsl/Jtube/SZflag/" 
                 << std::setw(4)<< AmdcJtyp	   
                 << std::setw(3)<< AmdcStationName 
                 << std::setw(4)<< AmdcJzz	    
                 << std::setw(4)<< AmdcJff	    
                 << std::setw(4)<< AmdcJob	    
                 << std::setw(4)<< AmdcJsl	    
                 << std::setw(4)<< AmdcJtube	    
                 << std::setw(6)<<std::setprecision(1)<< AmdcSZflag 
                 << std::endl;
    OutFile 
    		 << " GM/Am/GM-Am/X " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModX       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcX       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffX 
	  	 <<					WarningX  
    		 << std::endl;
    OutFile 
    		 << "             Y " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModY       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcY       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffY   
	  	 <<					WarningY  
    		 << std::endl;
    OutFile 
    		 << "             Z " 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  AmdcZ       
	  	 << std::setw(12)<<std::setprecision(3)<<  DiffZ    
	  	 <<					WarningZ  
    		 << std::endl;
    OutFile 
    		 << "             ss" 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModss       
	  	 << std::setw(12)<<std::setprecision(3)<<  Amdcss       
	  	 << std::setw(12)<<std::setprecision(3)<<  Diffss   
	  	 <<					Warningss 
    		 << std::endl;
    OutFile 
    		 << "             zz" 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModzz       
	  	 << std::setw(12)<<std::setprecision(3)<<  Amdczz       
	  	 << std::setw(12)<<std::setprecision(3)<<  Diffzz   
	  	 <<					Warningzz  
    		 << std::endl;
    OutFile 
    		 << "             tt" 
	  	 << std::setw(12)<<std::setprecision(3)<<GeoModtt       
	  	 << std::setw(12)<<std::setprecision(3)<<  Amdctt       
	  	 << std::setw(12)<<std::setprecision(3)<<  Difftt    
	  	 <<					Warningtt  
    		 << std::endl;
   }

}

//  //THIS IS FOR MUONGEOMODEL P for CSCS
//  //Mismatch in Identifier scheme
//    if (Corrected_SPMeasuresPhi == 1 && SPstationEta > 0) {
//     if(Corrected_SPstationNameString == "CSS") 
//       Corrected_SPStrip = 28 - ( Corrected_SPStrip - 1 ) ;
//     if(Corrected_SPstationNameString == "CSL") 
//       Corrected_SPStrip = 44 - ( Corrected_SPStrip - 1 ) ;
//    }
 
//  //THIS IS FOR MUONGEOMODEL P for TGC
//  //Mismatch in Identifier scheme
//  if (Corrected_SPstationNameString[2] == 'F' || Corrected_SPstationNameString[1] == '4') Corrected_SPstationPhi = Corrected_SPstationPhi - 1 ;
//  if (Corrected_SPstationNameString[2] == 'E' && Corrected_SPstationNameString[1] != '4') Corrected_SPstationPhi = Corrected_SPstationPhi - 2 ;
//  if (Corrected_SPstationPhi <= 0) {
//   if (Corrected_SPstationNameString[2] == 'F' || Corrected_SPstationNameString[1] == '4') Corrected_SPstationPhi = Corrected_SPstationPhi + 24 ;
//   if (Corrected_SPstationNameString[2] == 'E' && Corrected_SPstationNameString[1] != '4') Corrected_SPstationPhi = Corrected_SPstationPhi + 48 ;
//  }
