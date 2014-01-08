/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyMultiLayerOut.h"

#include "AmdcStand/AmdcsimrecStand.h"
#include "LoadOutLay.h"

MuonboyMultiLayerOut::MuonboyMultiLayerOut() { Reset() ;}

MuonboyMultiLayerOut::~MuonboyMultiLayerOut(){ Reset() ;}

void MuonboyMultiLayerOut::SetMultiLayerOut(std::vector<std::string>& MultiLayerOut)
{

  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    :" 
            << " One gets from MultiLayerOut " << MultiLayerOut.size() << " items "
            << std::endl;

  AnalyseMultiLayerOut(MultiLayerOut) ;

  int MultiLayerOutKSize =  0 ;
  
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : OffMultiLayer          " << m_OffMultiLayerOutK.size()           << std::endl; 
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : CSCWireLayerMultiLayer " << m_CSCWireLayerMultiLayerOutK.size()  << std::endl; 
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : MDTOffMultiLayer       " << m_MDTOffMultiLayerOutK.size()        << std::endl; 
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : RPCOffMultiLayer       " << m_RPCOffMultiLayerOutK.size()        << std::endl; 
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : CSCOffMultiLayer       " << m_CSCOffMultiLayerOutK.size()        << std::endl; 
  std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : TGCOffMultiLayer       " << m_TGCOffMultiLayerOutK.size()        << std::endl; 

  MultiLayerOutKSize =  m_OffMultiLayerOutK.size() ;
  resetloadoutlay_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlay_( 
                m_OffJtyp [MultiLayerOutKItem]  , 
                m_OffJff  [MultiLayerOutKItem]  , 
                m_OffJzz  [MultiLayerOutKItem]  , 
                m_OffJobj [MultiLayerOutKItem] 
               );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead Off              " 
              << " (" <<      m_OffStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_OffJtyp           [MultiLayerOutKItem]
              << "  " <<      m_OffJff            [MultiLayerOutKItem]
              << "  " <<      m_OffJzz            [MultiLayerOutKItem]
              << "  " <<      m_OffJobj           [MultiLayerOutKItem]
              << "  from " << m_OffMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

  MultiLayerOutKSize =  m_CSCWireLayerMultiLayerOutK.size() ;
  resetloadoutlaycscwirelayer_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlaycscwirelayer_( 
                            m_CSCWireLayerJtyp [MultiLayerOutKItem]  , 
                            m_CSCWireLayerJff  [MultiLayerOutKItem]  , 
                            m_CSCWireLayerJzz  [MultiLayerOutKItem]  , 
                            m_CSCWireLayerJobj [MultiLayerOutKItem]  ,
                            m_CSCWireLayerJsl  [MultiLayerOutKItem] 
                           );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead CSC wire layers  " 
              << " (" <<      m_CSCWireLayerStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_CSCWireLayerJtyp           [MultiLayerOutKItem]
              << "  " <<      m_CSCWireLayerJff            [MultiLayerOutKItem]
              << "  " <<      m_CSCWireLayerJzz            [MultiLayerOutKItem]
              << "  " <<      m_CSCWireLayerJobj           [MultiLayerOutKItem]
              << "  " <<      m_CSCWireLayerJsl            [MultiLayerOutKItem]
              << "  from " << m_CSCWireLayerMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

  MultiLayerOutKSize =  m_MDTOffMultiLayerOutK.size() ;
  resetloadoutlaymdtoff_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlaymdtoff_( 
                      m_MDTOffJtyp  [MultiLayerOutKItem]  , 
                      m_MDTOffJff   [MultiLayerOutKItem]  , 
                      m_MDTOffJzz   [MultiLayerOutKItem]  , 
                      m_MDTOffJob   [MultiLayerOutKItem]  ,
                      m_MDTOffJsl   [MultiLayerOutKItem]  ,
                      m_MDTOffJtube [MultiLayerOutKItem] 
                     );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead MDT tube         " 
              << " (" <<      m_MDTOffStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_MDTOffJtyp           [MultiLayerOutKItem]
              << "  " <<      m_MDTOffJff            [MultiLayerOutKItem]
              << "  " <<      m_MDTOffJzz            [MultiLayerOutKItem]
              << "  " <<      m_MDTOffJob            [MultiLayerOutKItem]
              << "  " <<      m_MDTOffJsl            [MultiLayerOutKItem]
              << "  " <<      m_MDTOffJtube          [MultiLayerOutKItem]
              << "  from " << m_MDTOffMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

  MultiLayerOutKSize =  m_RPCOffMultiLayerOutK.size() ;
  resetloadoutlayrpcoff_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlayrpcoff_( 
                      m_RPCOffJtyp  [MultiLayerOutKItem]  , 
                      m_RPCOffJff   [MultiLayerOutKItem]  , 
                      m_RPCOffJzz   [MultiLayerOutKItem]  , 
                      m_RPCOffJob   [MultiLayerOutKItem]  ,
                      m_RPCOffJspli [MultiLayerOutKItem]  ,
                      m_RPCOffJsl   [MultiLayerOutKItem]  ,
                      m_RPCOffJsz   [MultiLayerOutKItem]  ,
                      m_RPCOffJstri [MultiLayerOutKItem] 
                     );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead RPC strip        " 
              << " (" <<      m_RPCOffStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_RPCOffJtyp           [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJff            [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJzz            [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJob            [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJspli          [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJsl            [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJsz            [MultiLayerOutKItem]
              << "  " <<      m_RPCOffJstri          [MultiLayerOutKItem]
              << "  from " << m_RPCOffMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

  MultiLayerOutKSize =  m_CSCOffMultiLayerOutK.size() ;
  resetloadoutlaycscoff_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlaycscoff_( 
                      m_CSCOffJtyp   [MultiLayerOutKItem]  , 
                      m_CSCOffJff    [MultiLayerOutKItem]  , 
                      m_CSCOffJzz    [MultiLayerOutKItem]  , 
                      m_CSCOffJob    [MultiLayerOutKItem]  ,
                      m_CSCOffJsl    [MultiLayerOutKItem]  ,
                      m_CSCOffJtube  [MultiLayerOutKItem]  ,
                      m_CSCOffSZflag [MultiLayerOutKItem] 
                     );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead CSC strip        " 
              << " (" <<      m_CSCOffStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_CSCOffJtyp           [MultiLayerOutKItem]
              << "  " <<      m_CSCOffJff            [MultiLayerOutKItem]
              << "  " <<      m_CSCOffJzz            [MultiLayerOutKItem]
              << "  " <<      m_CSCOffJob            [MultiLayerOutKItem]
              << "  " <<      m_CSCOffJsl            [MultiLayerOutKItem]
              << "  " <<      m_CSCOffJtube          [MultiLayerOutKItem]
              << "  " <<      m_CSCOffSZflag         [MultiLayerOutKItem]
              << "  from " << m_CSCOffMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

  MultiLayerOutKSize =  m_TGCOffMultiLayerOutK.size() ;
  resetloadoutlaytgcoff_( MultiLayerOutKSize ) ; 
  for (int MultiLayerOutKItem=0; MultiLayerOutKItem<MultiLayerOutKSize ; MultiLayerOutKItem++){
    loadoutlaytgcoff_( 
                      m_TGCOffJtyp  [MultiLayerOutKItem]  , 
                      m_TGCOffJff   [MultiLayerOutKItem]  , 
                      m_TGCOffJzz   [MultiLayerOutKItem]  , 
                      m_TGCOffJob   [MultiLayerOutKItem]  ,
                      m_TGCOffJspli [MultiLayerOutKItem]  ,
                      m_TGCOffJsl   [MultiLayerOutKItem]  ,
                      m_TGCOffJsz   [MultiLayerOutKItem]  ,
                      m_TGCOffJstri [MultiLayerOutKItem] 
                     );
    std::cout << "= MuonboyMultiLayerOut::SetMultiLayerOut    : Dead TGC strip        " 
              << " (" <<      m_TGCOffStationName    [MultiLayerOutKItem]  << ")"
              << "  " <<      m_TGCOffJtyp           [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJff            [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJzz            [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJob            [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJspli          [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJsl            [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJsz            [MultiLayerOutKItem]
              << "  " <<      m_TGCOffJstri          [MultiLayerOutKItem]
              << "  from " << m_TGCOffMultiLayerOutK [MultiLayerOutKItem]
              << std::endl;
  }

//Store the MDT tubes dead data in appropriate Amdc arrays
  filldeadtubesamdc_();

//Set Mboy arrays with dead devices data
  filldeadchannels_();
  
}

void MuonboyMultiLayerOut::AnalyseMultiLayerOut(const std::vector<std::string>& MultiLayerOut) 
{ 

  Reset() ;
  
//Check string syntax
  std::vector<int>         V_SepPosSize ;
  std::vector<int>         V_Used                   ;
  std::vector<std::string> V_MultiLayerOutFiltered  ;
  std::vector<std::string> V_Field_0      ; 
  std::vector<std::string> V_Field_1      ;
  std::vector<std::string> V_Field_2      ;
  std::vector<std::string> V_Field_3      ;
  std::vector<std::string> V_Field_4      ;
  std::vector<std::string> V_Field_5      ;
  std::vector<std::string> V_Field_6      ;
  std::vector<std::string> V_Field_7      ;
  std::vector<std::string> V_Field_8      ;
  std::vector<std::string> V_Field_9      ;
  std::vector<int>         V_iField_1  ;
  std::vector<int>         V_iField_2  ;
  std::vector<int>         V_iField_4  ;
  std::vector<int>         V_iField_5  ;
  std::vector<int>         V_iField_6  ;
  std::vector<int>         V_iField_7  ;
  std::vector<int>         V_iField_8  ;
  std::vector<int>         V_iField_9  ;
  
  V_SepPosSize.clear() ;
  V_Used.clear()                   ;
  V_MultiLayerOutFiltered.clear()  ;
  V_Field_0.clear()      ;
  V_Field_1.clear()      ;
  V_Field_2.clear()      ;
  V_Field_3.clear()      ;
  V_Field_4.clear()      ;
  V_Field_5.clear()      ;
  V_Field_6.clear()      ;
  V_Field_7.clear()      ;
  V_Field_8.clear()      ;
  V_Field_9.clear()      ;
  V_iField_1.clear()  ;
  V_iField_2.clear()  ;
  V_iField_4.clear()  ;
  V_iField_5.clear()  ;
  V_iField_6.clear()  ;
  V_iField_7.clear()  ;
  V_iField_8.clear()  ;
  V_iField_9.clear()  ;
  
  int TheSizeMultiLayerOut = MultiLayerOut.size();
  for (int IndexMultiLayerOut= 0 ; IndexMultiLayerOut <TheSizeMultiLayerOut ; IndexMultiLayerOut++){

    const std::string& MLcurrent = MultiLayerOut[IndexMultiLayerOut] ;

    int SepPosSize =0 ;
    std::string Field_0  = "" ;
    std::string Field_1  = "" ;
    std::string Field_2  = "" ;
    std::string Field_3  = "" ;
    std::string Field_4  = "" ;
    std::string Field_5  = "" ;
    std::string Field_6  = "" ;
    std::string Field_7  = "" ;
    std::string Field_8  = "" ;
    std::string Field_9  = "" ; ;
    int iField_1  = -9999;
    int iField_2  = -9999;
    int iField_4  = -9999;
    int iField_5  = -9999;
    int iField_6  = -9999;
    int iField_7  = -9999;
    int iField_8  = -9999;
    int iField_9  = -9999;
    
    if ( !CheckSyntaxAndConvert(
           MLcurrent, SepPosSize ,
           Field_0, Field_1, Field_2,Field_3, Field_4, Field_5, Field_6, Field_7, Field_8, Field_9  ,
                   iField_1,iField_2,        iField_4,iField_5,iField_6,iField_7,iField_8,iField_9  
           ) ) continue ;
   
    V_SepPosSize.push_back ( SepPosSize );
    V_Used.push_back                   ( 0                      );
    V_MultiLayerOutFiltered.push_back  ( MLcurrent              );
    V_Field_0.push_back   ( Field_0  );
    V_Field_1.push_back   ( Field_1  );
    V_Field_2.push_back   ( Field_2  );
    V_Field_3.push_back   ( Field_3  );
    V_Field_4.push_back   ( Field_4  );
    V_Field_5.push_back   ( Field_5  );
    V_Field_6.push_back   ( Field_6  );
    V_Field_7.push_back   ( Field_7  );
    V_Field_8.push_back   ( Field_8  );
    V_Field_9.push_back   ( Field_9  );
    V_iField_1.push_back  ( iField_1  );
    V_iField_2.push_back  ( iField_2  );
    V_iField_4.push_back  ( iField_4  );
    V_iField_5.push_back  ( iField_5  );
    V_iField_6.push_back  ( iField_6  );
    V_iField_7.push_back  ( iField_7  );
    V_iField_8.push_back  ( iField_8  );
    V_iField_9.push_back  ( iField_9  );
  
  }
  
  AmdcsimrecStand aAmdcsimrecStand ;

//Loop on all Jtyp/jff/jzz/Jobj objects
  m_OffMultiLayerOutK.clear()  ;
  m_OffStationName.clear() ;
  m_OffJtyp.clear()        ;
  m_OffJff.clear()         ;
  m_OffJzz.clear()         ;
  m_OffJobj.clear()        ;
  for(int jtyp =  1; jtyp <= aAmdcsimrecStand.Mtyp(); jtyp++){
    for(int jff =  1; jff <= 8; jff++){        
      for(int jzz =  - aAmdcsimrecStand.Mzz(); jzz <=  aAmdcsimrecStand.Mzz(); jzz++){
        int jgeo =  aAmdcsimrecStand.IGEO(jtyp ,jff, jzz);
        if (jgeo > 0){
          for (int jobj=1; jobj <=aAmdcsimrecStand.NOBJ(jtyp,jgeo); jobj++){
            int jtec = aAmdcsimrecStand.ITEC(jtyp,jgeo,jobj);
            std::string TheTecName = aAmdcsimrecStand.TechnologyName(jtec) ;
            if (
                ( TheTecName == m_MDTstring )
             || ( TheTecName == m_RPCstring )
             || ( TheTecName == m_TGCstring )
             || ( TheTecName == m_CSCstring )
                 
               ){
               
               std::string StationName = aAmdcsimrecStand.StationName(jtyp) ;
               int    Jsl         = 1 ;
               int    Jtube       = 1 ;
               double SZflag      = 1 ;
               int    Jspli       = 1 ;
               int    Jsz         = 1 ;
               int    Jstri       = 1 ;
               
               int    StationEta  = 0 ;
               int    StationPhi  = 0 ;
               int    Multilayer  = 0 ;
                
               int    TubeLayer   = 0 ;
               int    Tube        = 0 ;
               
               int    WireLayer   = 0 ;
               int    MeasuresPhi = 0 ;
               int    Strip       = 0 ;
               
               int    DoubletR    = 0 ;
               int    DoubletZ    = 0 ;
               int    DoubletPhi  = 0 ;
               int    GasGap      = 0 ;
               
               int    IsStrip     = 0 ;
               int    Channel     = 0 ;
               
               
               if ( TheTecName == m_MDTstring ) aAmdcsimrecStand.GetMDTindexFromAMDC( StationName,jff,jzz,jobj, 
                                                           Jsl, Jtube,
                                                           StationEta, StationPhi, Multilayer, 
                                                           TubeLayer, Tube );
               
               if ( TheTecName == m_CSCstring ) aAmdcsimrecStand.GetCSCindexFromAMDC( StationName,jff,jzz,jobj, 
                                                           Jsl, Jtube, SZflag,
                                                           StationEta, StationPhi, Multilayer, 
                                                           WireLayer, MeasuresPhi, Strip );
                                
               if ( TheTecName == m_RPCstring ) aAmdcsimrecStand.GetRPCindexFromAMDC( StationName,jff,jzz,jobj,
                                                           Jspli, Jsl, Jsz, Jstri,
                                                           StationEta, StationPhi, DoubletR, 
                                                           DoubletZ, DoubletPhi, GasGap, MeasuresPhi, Strip );
                                
               if ( TheTecName == m_TGCstring ) aAmdcsimrecStand.GetTGCindexFromAMDC( StationName,jff,jzz,jobj, 
                                                           Jspli, Jsl, Jsz, Jstri,
                                                           StationEta, StationPhi, GasGap, 
                                                           IsStrip, Channel );
               
//            Loop on all strings
              int TheSizeOffInst =  V_Field_0.size() ;
              for (int IndexOffInst= 0 ; IndexOffInst <TheSizeOffInst ; IndexOffInst++){
               
                int SepPosSize = V_SepPosSize[IndexOffInst] ;
                std::string MLcurrent      = V_MultiLayerOutFiltered[IndexOffInst]  ; 
                
                int ItMatches = 1 ;

                std::string& Field_StationName =  V_Field_0[IndexOffInst]  ; 
                if (Field_StationName != m_WhateverString){
                  for (int Ibid=0 ; Ibid <3 ; Ibid++){ if ( Field_StationName.substr(Ibid,1) != m_WhateverString ){ if ( Field_StationName.substr(Ibid,1) != StationName.substr(Ibid,1) ) { ItMatches = 0 ; break ; } } }
                  if ( ItMatches == 0 ) continue ;
                }
                
                std::string& Field_Technology  =  V_Field_3[IndexOffInst]  ; if (Field_Technology != m_WhateverString) { if ( Field_Technology  != TheTecName ) continue ; }
                
                std::string& Field_StationEta  =  V_Field_1[IndexOffInst]  ; 
                std::string& Field_StationPhi  =  V_Field_2[IndexOffInst]  ; 
                int          iField_StationEta =  V_iField_1[IndexOffInst] ; if (Field_StationEta != m_WhateverString) { if ( iField_StationEta != StationEta ) continue ; }
                int          iField_StationPhi =  V_iField_2[IndexOffInst] ; if (Field_StationPhi != m_WhateverString) { if ( iField_StationPhi != StationPhi ) continue ; }
                
                std::string& Field_4  = V_Field_4[IndexOffInst]  ; 
                std::string& Field_5  = V_Field_5[IndexOffInst]  ; 
                std::string& Field_6  = V_Field_6[IndexOffInst]  ; 
                std::string& Field_7  = V_Field_7[IndexOffInst]  ; 
                std::string& Field_8  = V_Field_8[IndexOffInst]  ; 
                std::string& Field_9  = V_Field_9 [IndexOffInst] ;

//In MDT(CSC) case, since we are looking to exclude Jtyp/jff/jzz/jobj set which match to stationname/Sttioneta/stationphi/tech/multilayer(chamberLayer) fields
// if the next fiedl exist one cheks that if not * the multilayer(chamberLayer) field matches
// then all the fields after the 6th if they exist should be *
                if ( TheTecName == m_MDTstring || TheTecName == m_CSCstring  ){
                  if ( SepPosSize >= 4 ){
                    std::string& Field_MultiLayer  =  Field_4                  ; 
                    int          iField_MultiLayer =  V_iField_4[IndexOffInst] ; 
                    if (Field_MultiLayer != m_WhateverString){ if ( iField_MultiLayer != Multilayer )  ItMatches = 0 ; }
                  }
                  if ( ItMatches == 0 ) continue ;
                  if ( SepPosSize >= 5 ) { if ( Field_5  != m_WhateverString ) continue ; } 
                  if ( SepPosSize >= 6 ) { if ( Field_6  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 7 ) { if ( Field_7  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 8 ) { if ( Field_8  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 9 ) { if ( Field_9  != m_WhateverString ) continue ; }
                }
//In TGC case, since we are looking to exclude Jtyp/jff/jzz/jobj set which match to stationname/Sttioneta/stationphi/tech fields
// then all the fields after the 5th if they exist should be *
                if ( TheTecName == m_TGCstring  ){
                  if ( SepPosSize >= 4 ) { if ( Field_4  != m_WhateverString ) continue ; } 
                  if ( SepPosSize >= 5 ) { if ( Field_5  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 6 ) { if ( Field_6  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 7 ) { if ( Field_7  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 8 ) { if ( Field_8  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 9 ) { if ( Field_9  != m_WhateverString ) continue ; }
                }
//In RPC case, since we are looking to exclude Jtyp/jff/jzz/jobj set but that the matching is unclear beyond stationname/Sttioneta/stationphi/tech isd
// then all the fields after the 5th if they exist should be *
                if ( TheTecName == m_RPCstring  ){
                  if ( SepPosSize >= 4 ) { if ( Field_4  != m_WhateverString ) continue ; } 
                  if ( SepPosSize >= 5 ) { if ( Field_5  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 6 ) { if ( Field_6  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 7 ) { if ( Field_7  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 8 ) { if ( Field_8  != m_WhateverString ) continue ; }
                  if ( SepPosSize >= 9 ) { if ( Field_9  != m_WhateverString ) continue ; }
                }
                
                V_Used[IndexOffInst] = 1 ;
                m_OffMultiLayerOutK.push_back( MLcurrent   ) ;
                m_OffStationName.push_back( StationName ) ;
                m_OffJtyp.push_back       ( jtyp        ) ;
                m_OffJff.push_back        ( jff         ) ;
                m_OffJzz.push_back        ( jzz         ) ;
                m_OffJobj.push_back       ( jobj        ) ;
                
                break ;
                
              }
            }
          }
        }
      }
    }
  }

//Work on CSC strings dow to wireLayer level; Loop on all strings
  m_CSCWireLayerMultiLayerOutK.clear() ;
  m_CSCWireLayerStationName.clear()    ;
  m_CSCWireLayerJtyp.clear()           ;
  m_CSCWireLayerJff.clear()            ;
  m_CSCWireLayerJzz.clear()            ;
  m_CSCWireLayerJobj.clear()           ;
  m_CSCWireLayerJsl.clear()            ;
  int TheSizeOffInst =  V_Field_0.size() ;
  for (int IndexOffInst= 0 ; IndexOffInst <TheSizeOffInst ; IndexOffInst++){
   
    int ItMatches = 1 ;
    
    
 
//  keep only CSC string with 6 fields (i.e down to wireLayer)
//  and definite (i.e not equal to *) Field_StationName, Field_StationEta, Field_StationPhi, Field_chamberLayer or Field_wireLayer
//  and no * in one of characters of Field_StationName
    std::string& Field_Technology = V_Field_3[IndexOffInst] ; 
    if ( Field_Technology != m_CSCstring ) continue ;
    
    int SepPosSize = V_SepPosSize[IndexOffInst] ;
    std::string MLcurrent      =  V_MultiLayerOutFiltered[IndexOffInst] ; 

    if (SepPosSize != 5 )  continue ;

    std::string& Field_StationName =  V_Field_0[IndexOffInst] ; if ( Field_StationName  == m_WhateverString ) continue ; 

    for (int Ibid=0 ; Ibid <3 ; Ibid++){ if ( Field_StationName.substr(Ibid,1) == m_WhateverString ){ ItMatches = 0 ; break ; } }
    if ( ItMatches == 0 ) continue ;

    std::string& Field_StationEta  =  V_Field_1[IndexOffInst] ; if ( Field_StationEta   == m_WhateverString ) continue ; 
    std::string& Field_StationPhi  =  V_Field_2[IndexOffInst] ; if ( Field_StationPhi   == m_WhateverString ) continue ; 

    int iField_StationEta   =  V_iField_1[IndexOffInst] ; 
    int iField_StationPhi   =  V_iField_2[IndexOffInst] ; 

    std::string& Field_chamberLayer   =  V_Field_4[IndexOffInst] ; if ( Field_chamberLayer == m_WhateverString ) continue ; 
    std::string& Field_wireLayer      =  V_Field_5[IndexOffInst] ; if ( Field_wireLayer    == m_WhateverString ) continue ; 

    int iField_chamberLayer =  V_iField_4[IndexOffInst] ; 
    int iField_wireLayer    =  V_iField_5[IndexOffInst] ; 

    int    MeasuresPhi = 0 ;
    int    Strip       = 1 ;

    int    Jtyp   = aAmdcsimrecStand.GetJtyp(Field_StationName) ;
    int    Jff    = 0                                           ;
    int    Jzz    = 0                                           ;
    int    Job    = 0                                           ;
    int    Jsl    = 0                                           ;
    int    Jtube  = 0                                           ;
    double SZflag = 0.                                          ;
    aAmdcsimrecStand.GetAMDCindexFromCSC(Field_StationName,
               iField_StationEta, iField_StationPhi, iField_chamberLayer, iField_wireLayer,
               MeasuresPhi, Strip,
               Jff, Jzz, Job, Jsl, Jtube , SZflag );

    V_Used[IndexOffInst] = 1 ;
    m_CSCWireLayerMultiLayerOutK.push_back ( MLcurrent         ) ;
    m_CSCWireLayerStationName.push_back    ( Field_StationName ) ;
    m_CSCWireLayerJtyp.push_back           ( Jtyp              ) ;
    m_CSCWireLayerJff.push_back            ( Jff               ) ;
    m_CSCWireLayerJzz.push_back            ( Jzz               ) ;
    m_CSCWireLayerJobj.push_back           ( Job               ) ;
    m_CSCWireLayerJsl.push_back            ( Jsl               ) ;

  }

//  keep only defined string
  m_MDTOffMultiLayerOutK.clear() ;
  m_MDTOffStationName.clear()    ;
  m_MDTOffJtyp.clear()           ;
  m_MDTOffJff.clear()            ;
  m_MDTOffJzz.clear()            ;
  m_MDTOffJob.clear()            ;
  m_MDTOffJsl.clear()            ;
  m_MDTOffJtube.clear()          ;

  m_RPCOffMultiLayerOutK.clear() ;
  m_RPCOffStationName.clear()    ;
  m_RPCOffJtyp.clear()           ;
  m_RPCOffJff.clear()            ;
  m_RPCOffJzz.clear()            ;
  m_RPCOffJob.clear()            ;
  m_RPCOffJspli.clear()          ;
  m_RPCOffJsl.clear()            ;
  m_RPCOffJsz.clear()            ;
  m_RPCOffJstri.clear()          ;
  
  m_CSCOffMultiLayerOutK.clear() ;
  m_CSCOffStationName.clear()    ;
  m_CSCOffJtyp.clear()           ;
  m_CSCOffJff.clear()            ;
  m_CSCOffJzz.clear()            ;
  m_CSCOffJob.clear()            ;
  m_CSCOffJsl.clear()            ;
  m_CSCOffJtube.clear()          ;
  m_CSCOffSZflag.clear()         ;

  m_TGCOffMultiLayerOutK.clear() ;
  m_TGCOffStationName.clear()    ;
  m_TGCOffJtyp.clear()           ;
  m_TGCOffJff.clear()            ;
  m_TGCOffJzz.clear()            ;
  m_TGCOffJob.clear()            ;
  m_TGCOffJspli.clear()          ;
  m_TGCOffJsl.clear()            ;
  m_TGCOffJsz.clear()            ;
  m_TGCOffJstri.clear()          ;
  
  for (int IndexOffInst= 0 ; IndexOffInst <TheSizeOffInst ; IndexOffInst++){
   
    int ItMatches = 1 ;
    
    int SepPosSize = V_SepPosSize[IndexOffInst] ;
    std::string MLcurrent      = V_MultiLayerOutFiltered[IndexOffInst]  ; 
    

    std::string& Field_StationName = V_Field_0[IndexOffInst] ; if ( Field_StationName == m_WhateverString ) continue ; 

    for (int Ibid=0 ; Ibid <3 ; Ibid++){ if ( Field_StationName.substr(Ibid,1) == m_WhateverString ) { ItMatches = 0 ; break ; } }
    if ( ItMatches == 0 ) continue ;

    std::string& Field_StationEta  = V_Field_1[IndexOffInst] ; if ( Field_StationEta  == m_WhateverString ) continue ; 
    std::string& Field_StationPhi  = V_Field_2[IndexOffInst] ; if ( Field_StationPhi  == m_WhateverString ) continue ; 

    int iField_StationEta = V_iField_1[IndexOffInst]  ; 
    int iField_StationPhi = V_iField_2[IndexOffInst]  ; 

    std::string& Field_Technology = V_Field_3[IndexOffInst] ; 

    if ( Field_Technology == m_MDTstring) {
      if ( SepPosSize != 6 )  continue ;
      std::string& Field_Multilayer = V_Field_4[IndexOffInst] ;if ( Field_Multilayer == m_WhateverString ) continue ;
      std::string& Field_TubeLayer  = V_Field_5[IndexOffInst] ;if ( Field_TubeLayer  == m_WhateverString ) continue ;
      std::string& Field_Tube       = V_Field_6[IndexOffInst] ;if ( Field_Tube       == m_WhateverString ) continue ;
      
      int iField_Multilayer = V_iField_4[IndexOffInst]  ; 
      int iField_TubeLayer  = V_iField_5[IndexOffInst]  ; 
      int iField_Tube       = V_iField_6[IndexOffInst]  ; 
      
      int    Jtyp   = aAmdcsimrecStand.GetJtyp(Field_StationName) ;
      int    Jff    = 0                                           ;
      int    Jzz    = 0                                           ;
      int    Job    = 0                                           ;
      int    Jsl    = 0                                           ;
      int    Jtube  = 0                                           ;
      aAmdcsimrecStand.GetAMDCindexFromMDT(Field_StationName,iField_StationEta, iField_StationPhi,
               iField_Multilayer,iField_TubeLayer,iField_Tube,
               Jff, Jzz, Job, Jsl, Jtube );
               
      V_Used[IndexOffInst] = 1 ;
      m_MDTOffMultiLayerOutK.push_back( MLcurrent         ) ;
      m_MDTOffStationName.push_back   ( Field_StationName ) ;
      m_MDTOffJtyp.push_back          ( Jtyp              ) ;
      m_MDTOffJff.push_back           ( Jff               ) ;
      m_MDTOffJzz.push_back           ( Jzz               ) ;
      m_MDTOffJob.push_back           ( Job               ) ;
      m_MDTOffJsl.push_back           ( Jsl               ) ;
      m_MDTOffJtube.push_back         ( Jtube             ) ;
    }
    
    if ( Field_Technology == m_RPCstring) {
      if ( SepPosSize != 9 )  continue ;
      std::string& Field_DoubletR    = V_Field_4[IndexOffInst]  ; if ( Field_DoubletR    == m_WhateverString ) continue ; 
      std::string& Field_DoubletZ    = V_Field_5[IndexOffInst]  ; if ( Field_DoubletZ    == m_WhateverString ) continue ; 
      std::string& Field_DoubletPhi  = V_Field_6[IndexOffInst]  ; if ( Field_DoubletPhi  == m_WhateverString ) continue ; 
      std::string& Field_GasGap      = V_Field_7[IndexOffInst]  ; if ( Field_GasGap      == m_WhateverString ) continue ; 
      std::string& Field_MeasuresPhi = V_Field_8[IndexOffInst]  ; if ( Field_MeasuresPhi == m_WhateverString ) continue ; 
      std::string& Field_Strip       = V_Field_9 [IndexOffInst] ; if ( Field_Strip       == m_WhateverString ) continue ; 

      int iField_DoubletR    = V_iField_4[IndexOffInst]  ; 
      int iField_DoubletZ    = V_iField_5[IndexOffInst]  ; 
      int iField_DoubletPhi  = V_iField_6[IndexOffInst]  ; 
      int iField_GasGap      = V_iField_7[IndexOffInst]  ; 
      int iField_MeasuresPhi = V_iField_8[IndexOffInst]  ; 
      int iField_Strip       = V_iField_9 [IndexOffInst] ; 
      
      int    Jtyp   = aAmdcsimrecStand.GetJtyp(Field_StationName) ;
      int    Jff    = 0                                           ;
      int    Jzz    = 0                                           ;
      int    Job    = 0                                           ;
      int    Jspli  = 0                                           ;
      int    Jsl    = 0                                           ;
      int    Jsz    = 0                                           ;
      int    Jstri  = 0                                           ;
      aAmdcsimrecStand.GetAMDCindexFromRPC(Field_StationName,iField_StationEta, iField_StationPhi,
               iField_DoubletR,iField_DoubletZ,iField_DoubletPhi,
               iField_GasGap,iField_MeasuresPhi,iField_Strip,
               Jff, Jzz, Job, Jspli, Jsl, Jsz, Jstri );
               
      V_Used[IndexOffInst] = 1 ;
      m_RPCOffMultiLayerOutK.push_back( MLcurrent         ) ;
      m_RPCOffStationName.push_back   ( Field_StationName ) ;
      m_RPCOffJtyp.push_back          ( Jtyp              ) ;
      m_RPCOffJff.push_back           ( Jff               ) ;
      m_RPCOffJzz.push_back           ( Jzz               ) ;
      m_RPCOffJob.push_back           ( Job               ) ;
      m_RPCOffJspli.push_back         ( Jspli             ) ;
      m_RPCOffJsl.push_back           ( Jsl               ) ;
      m_RPCOffJsz.push_back           ( Jsz               ) ;
      m_RPCOffJstri.push_back         ( Jstri             ) ;
    }
    
    if ( Field_Technology == m_CSCstring) {
      if ( SepPosSize != 7 )  continue ;
      std::string& Field_chamberLayer = V_Field_4[IndexOffInst] ; if ( Field_chamberLayer == m_WhateverString ) continue ;
      std::string& Field_wireLayer    = V_Field_5[IndexOffInst] ; if ( Field_wireLayer    == m_WhateverString ) continue ;
      std::string& Field_MeasuresPhi  = V_Field_6[IndexOffInst] ; if ( Field_MeasuresPhi  == m_WhateverString ) continue ;
      std::string& Field_Strip        = V_Field_7[IndexOffInst] ; if ( Field_Strip        == m_WhateverString ) continue ;

      int iField_chamberLayer = V_iField_4[IndexOffInst]  ; 
      int iField_wireLayer    = V_iField_5[IndexOffInst]  ; 
      int iField_MeasuresPhi  = V_iField_6[IndexOffInst]  ; 
      int iField_Strip        = V_iField_7[IndexOffInst]  ; 
      
      int    Jtyp   = aAmdcsimrecStand.GetJtyp(Field_StationName) ;
      int    Jff    = 0                                           ;
      int    Jzz    = 0                                           ;
      int    Job    = 0                                           ;
      int    Jsl    = 0                                           ;
      int    Jtube  = 0                                           ;
      double SZflag = 0.                                          ;
      aAmdcsimrecStand.GetAMDCindexFromCSC(Field_StationName,iField_StationEta, iField_StationPhi, 
               iField_chamberLayer, iField_wireLayer,
               iField_MeasuresPhi, iField_Strip,
               Jff, Jzz, Job, Jsl, Jtube, SZflag );
               
      V_Used[IndexOffInst] = 1 ;
      m_CSCOffMultiLayerOutK.push_back( MLcurrent         ) ;
      m_CSCOffStationName.push_back   ( Field_StationName ) ;
      m_CSCOffJtyp.push_back          ( Jtyp              ) ;
      m_CSCOffJff.push_back           ( Jff               ) ;
      m_CSCOffJzz.push_back           ( Jzz               ) ;
      m_CSCOffJob.push_back           ( Job               ) ;
      m_CSCOffJsl.push_back           ( Jsl               ) ;
      m_CSCOffJtube.push_back         ( Jtube             ) ;
      m_CSCOffSZflag.push_back        ( SZflag            ) ;
    }
    
    if ( Field_Technology == m_TGCstring) {
      if ( SepPosSize != 6 )  continue ;
      std::string& Field_GasGap  = V_Field_4[IndexOffInst] ; if ( Field_GasGap  == m_WhateverString ) continue ; 
      std::string& Field_IsStrip = V_Field_5[IndexOffInst] ; if ( Field_IsStrip == m_WhateverString ) continue ; 
      std::string& Field_Channel = V_Field_6[IndexOffInst] ; if ( Field_Channel == m_WhateverString ) continue ; 

      int iField_GasGap     = V_iField_4[IndexOffInst]  ; 
      int iField_IsStrip    = V_iField_5[IndexOffInst]  ; 
      int iField_Channel    = V_iField_6[IndexOffInst]  ; 
      
      int    Jtyp   = aAmdcsimrecStand.GetJtyp(Field_StationName) ;
      int    Jff    = 0                                           ;
      int    Jzz    = 0                                           ;
      int    Job    = 0                                           ;
      int    Jspli  = 0                                           ;
      int    Jsl    = 0                                           ;
      int    Jsz    = 0                                           ;
      int    Jstri  = 0                                           ;
      aAmdcsimrecStand.GetAMDCindexFromTGC(Field_StationName,iField_StationEta, iField_StationPhi, 
               iField_GasGap,iField_IsStrip,iField_Channel,
               Jff, Jzz, Job, Jspli, Jsl, Jsz, Jstri );
               
      V_Used[IndexOffInst] = 1 ;
      m_TGCOffMultiLayerOutK.push_back( MLcurrent         ) ;
      m_TGCOffStationName.push_back   ( Field_StationName ) ;
      m_TGCOffJtyp.push_back          ( Jtyp              ) ;
      m_TGCOffJff.push_back           ( Jff               ) ;
      m_TGCOffJzz.push_back           ( Jzz               ) ;
      m_TGCOffJob.push_back           ( Job               ) ;
      m_TGCOffJspli.push_back         ( Jspli             ) ;
      m_TGCOffJsl.push_back           ( Jsl               ) ;
      m_TGCOffJsz.push_back           ( Jsz               ) ;
      m_TGCOffJstri.push_back         ( Jstri             ) ;
    }
  }

//Loop on all strings and tell which ones were not used
  for (int IndexOffInst= 0 ; IndexOffInst <TheSizeOffInst ; IndexOffInst++){
    if ( V_Used[IndexOffInst] == 1) continue ;
    std::string MLcurrent            =  V_MultiLayerOutFiltered[IndexOffInst] ; 
    std::cout << "= MuonboyMultiLayerOut::AnalyseMultiLayerOut: Not used dead devices string " << MLcurrent 
              << std::endl;
  }

}

std::vector<int>  MuonboyMultiLayerOut::GiveSeparatorPositions(const std::string& MLcurrent){
  std::vector<int> SepPos ;
  std::string::size_type pos = 0;
  while ( (pos = MLcurrent.find(m_SeparatorString, pos)) != std::string::npos ) {
   SepPos.push_back( pos ) ;
   pos++;
  }
  return SepPos;
}

bool  MuonboyMultiLayerOut::ConvertIntoInt(const std::string& Field, const std::string& MLcurrent, int& ResultInt){

  if ( Field != m_WhateverString   ){
    std::istringstream Aistringstream(Field);
    if ( !( Aistringstream >> ResultInt)  ){
      std::cout << "MuonboyMultiLayerOut::ConvertIntoInt: Field " << Field << " in " << MLcurrent << " should be * or an integer " << std::endl; 
      return false ;
    }
  }
  
  return true ;
  
}
void MuonboyMultiLayerOut::Reset()
{
  m_OffMultiLayerOutK.clear()  ;
  m_OffStationName.clear() ;
  m_OffJtyp.clear()        ;
  m_OffJff.clear()         ;
  m_OffJzz.clear()         ;
  m_OffJobj.clear()        ;

  m_CSCWireLayerMultiLayerOutK.clear() ;
  m_CSCWireLayerStationName.clear()    ;
  m_CSCWireLayerJtyp.clear()           ;
  m_CSCWireLayerJff.clear()            ;
  m_CSCWireLayerJzz.clear()            ;
  m_CSCWireLayerJobj.clear()           ;
  m_CSCWireLayerJsl.clear()            ;

  m_MDTOffMultiLayerOutK.clear() ;
  m_MDTOffStationName.clear()    ;
  m_MDTOffJtyp.clear()           ;
  m_MDTOffJff.clear()            ;
  m_MDTOffJzz.clear()            ;
  m_MDTOffJob.clear()            ;
  m_MDTOffJsl.clear()            ;
  m_MDTOffJtube.clear()          ;

  m_RPCOffMultiLayerOutK.clear() ;
  m_RPCOffStationName.clear()    ;
  m_RPCOffJtyp.clear()           ;
  m_RPCOffJff.clear()            ;
  m_RPCOffJzz.clear()            ;
  m_RPCOffJob.clear()            ;
  m_RPCOffJspli.clear()          ;
  m_RPCOffJsl.clear()            ;
  m_RPCOffJsz.clear()            ;
  m_RPCOffJstri.clear()          ;
  
  m_CSCOffMultiLayerOutK.clear() ;
  m_CSCOffStationName.clear()    ;
  m_CSCOffJtyp.clear()           ;
  m_CSCOffJff.clear()            ;
  m_CSCOffJzz.clear()            ;
  m_CSCOffJob.clear()            ;
  m_CSCOffJsl.clear()            ;
  m_CSCOffJtube.clear()          ;
  m_CSCOffSZflag.clear()         ;

  m_TGCOffMultiLayerOutK.clear() ;
  m_TGCOffStationName.clear()    ;
  m_TGCOffJtyp.clear()           ;
  m_TGCOffJff.clear()            ;
  m_TGCOffJzz.clear()            ;
  m_TGCOffJob.clear()            ;
  m_TGCOffJspli.clear()          ;
  m_TGCOffJsl.clear()            ;
  m_TGCOffJsz.clear()            ;
  m_TGCOffJstri.clear()          ;
  
  m_SeparatorString = "/" ;
  m_WhateverString  = "*" ; 
  m_MDTstring  = "MDT"    ; 
  m_RPCstring  = "RPC"    ;
  m_TGCstring  = "TGC"    ;
  m_CSCstring  = "CSC"    ;

}
bool MuonboyMultiLayerOut::CheckSyntaxAndConvert(
                              const std::string& MLcurrent,
                              int& SepPosSize ,
                              std::string& Field_0  ,
                              std::string& Field_1  ,
                              std::string& Field_2  ,
                              std::string& Field_3  ,
                              std::string& Field_4  ,
                              std::string& Field_5  ,
                              std::string& Field_6  ,
                              std::string& Field_7  ,
                              std::string& Field_8  ,
                              std::string& Field_9  ,
                              int& iField_1  ,
                              int& iField_2  ,
                              int& iField_4  ,
                              int& iField_5  ,
                              int& iField_6  ,
                              int& iField_7  ,
                              int& iField_8  ,
                              int& iField_9  
) 
{ 

  SepPosSize =0 ;
  Field_0  = "" ;
  Field_1  = "" ;
  Field_2  = "" ;
  Field_3  = "" ;
  Field_4  = "" ;
  Field_5  = "" ;
  Field_6  = "" ;
  Field_7  = "" ;
  Field_8  = "" ;
  Field_9  = "" ; ;
  iField_1  = -9999;
  iField_2  = -9999;
  iField_4  = -9999;
  iField_5  = -9999;
  iField_6  = -9999;
  iField_7  = -9999;
  iField_8  = -9999;
  iField_9  = -9999;

  std::vector<int> SepPos = GiveSeparatorPositions(MLcurrent) ;
  SepPosSize = int ( SepPos.size() ) ;
  
//The string should contains no more that 10 fields
  if ( SepPosSize > 9 ){
    std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: There should be less than or equal to 9 " << m_SeparatorString << " in " << MLcurrent  << std::endl; 
    return false;
  }

//The string should contains at least 4 fields
  if ( SepPosSize < 3 ){
    std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: There should be greater than or equal to 3 " << m_SeparatorString << " in " << MLcurrent << std::endl; 
    return false;
  }

//The string should not start with a separator
  if ( SepPos[0] == 0 ){
    std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: One should not start with " << m_SeparatorString << std::endl; 
    return false;
  }

//The string should not end with a separator
  int IntTempo = MLcurrent.size()-1 ;
  if ( SepPos[(SepPosSize-1)] == IntTempo ){
    std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: One should not end with " << m_SeparatorString << " in " << MLcurrent << std::endl; 
    return false;
  }

//Field size should not be  0
  for (int Isep=1 ; Isep < SepPosSize ; Isep++){
    int Prob = 0 ;
    if ( SepPos[Isep-1] == ( SepPos[Isep]-1 ) ){
      Prob = 1 ;
      std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: Separators " << m_SeparatorString << " should be separated in " << MLcurrent << std::endl; 
    }
    if ( Prob == 1 ) return false ;
  }

//Extract Fields
                         ExtractField( 0 , Field_0 , MLcurrent , SepPos );
                         ExtractField( 1 , Field_1 , MLcurrent , SepPos );
                         ExtractField( 2 , Field_2 , MLcurrent , SepPos );
                         ExtractField( 3 , Field_3 , MLcurrent , SepPos );
  if ( SepPosSize >= 4 ) ExtractField( 4 , Field_4 , MLcurrent , SepPos );
  if ( SepPosSize >= 5 ) ExtractField( 5 , Field_5 , MLcurrent , SepPos );
  if ( SepPosSize >= 6 ) ExtractField( 6 , Field_6 , MLcurrent , SepPos );
  if ( SepPosSize >= 7 ) ExtractField( 7 , Field_7 , MLcurrent , SepPos );
  if ( SepPosSize >= 8 ) ExtractField( 8 , Field_8 , MLcurrent , SepPos );
  if ( SepPosSize >= 9 ) ExtractField( 9 , Field_9 , MLcurrent , SepPos );

//Technology Field can have only 5 possible values
  if (
     ( Field_3 != m_WhateverString   )
  && ( Field_3 != m_MDTstring        )
  && ( Field_3 != m_RPCstring        )
  && ( Field_3 != m_TGCstring        )
  && ( Field_3 != m_CSCstring        )
   ){
         std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: Field " << Field_3 << " in " << MLcurrent << " should be *, MDT, RPC, TGC or CSC " << std::endl; 
    return false;
  }

//Station Field should * or have 3 characters
  if ( Field_0 != m_WhateverString   ){
    if ( Field_0.size() != 3 ){
      std::cout << "MuonboyMultiLayerOut::CheckSyntaxAndConvert: Field " << Field_0 << " in " << MLcurrent  << " should contain * or  3 characters " << std::endl; 
      return false;
    }
  }

//Convert field to integers
                           if ( !ConvertIntoInt(Field_1 ,MLcurrent,iField_1 ) ) return false ;
                           if ( !ConvertIntoInt(Field_2 ,MLcurrent,iField_2 ) ) return false ;
  if ( SepPosSize >= 4 ) { if ( !ConvertIntoInt(Field_4 ,MLcurrent,iField_4 ) ) return false ; }
  if ( SepPosSize >= 5 ) { if ( !ConvertIntoInt(Field_5 ,MLcurrent,iField_5 ) ) return false ; }
  if ( SepPosSize >= 6 ) { if ( !ConvertIntoInt(Field_6 ,MLcurrent,iField_6 ) ) return false ; }
  if ( SepPosSize >= 7 ) { if ( !ConvertIntoInt(Field_7 ,MLcurrent,iField_7 ) ) return false ; }
  if ( SepPosSize >= 8 ) { if ( !ConvertIntoInt(Field_8 ,MLcurrent,iField_8 ) ) return false ; }
  if ( SepPosSize >= 9 ) { if ( !ConvertIntoInt(Field_9 ,MLcurrent,iField_9 ) ) return false ; }

  return true;
}
  
void MuonboyMultiLayerOut::ExtractField(int FieldRank, std::string& Field, const std::string& MLcurrent, const std::vector<int>&  SepPos ){

 Field = "" ; 
 
 int SepPosSize    = int ( SepPos.size()    ) ; if ( SepPosSize    == 0 ) return ;
 int MLcurrentSize = int ( MLcurrent.size() ) ; if ( MLcurrentSize == 0 ) return ;
 
 if ( 0 > FieldRank || FieldRank  > SepPosSize ) return ;
 
 int Lindex = 0 ;
 if ( FieldRank != 0) Lindex = SepPos[FieldRank-1] + 1 ;
    
 int Hindex = MLcurrentSize ;
 if ( FieldRank != SepPosSize ) Hindex = SepPos[FieldRank]                ;
 if ( FieldRank != 0          ) Hindex = Hindex - SepPos[FieldRank-1] - 1 ;

 Field  = MLcurrent.substr(Lindex , Hindex ) ;

}
