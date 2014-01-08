/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCocoupMaker.h"

#include "MboyCoreEvt/MboyPosDigiChoice.h"
#include "MboyCoreEvt/MuonRecDigiStore.h"
#include "MboyCoreEvt/MboyGenTrackStore.h"

//*Wrapped Fortran routine
extern "C" void clearcoco_(int& IdEve);

extern "C" void mdigimdt_(
       int&  Ident,
       char* STANAME,
       int&  AreAmdbIds,
       int&  StationEta,int& StationPhi,int& Multilayer,int& TubeLayer,int& Tube,
       int&  Jtyp,int& Jff,int& Jzz,int& Job,int& Jsl,int& Jtube,
       double& XXX,double& YYY,double& ZZZ,
       double& DriftTime,
       int& ParticleCode,
       int& TDCcount,
       int& ADCcount,
       int& Status,
       int& AthPosOff
       );

extern "C" void mdigirpc_(
       int& Ident,
       char* STANAME,
       int& AreAmdbIds,
       int& StationEta,int& StationPhi,int& DoubletR,int& DoubletZ,int& DoubletPhi,
       int& GasGap,int& MeasuresPhi,int& Strip,  
       int& Jtyp,int& Jff,int& Jzz,int& Job,int& Jspli,int& Jsl,int& Jsz,int& Jstri,
       double& XXX,double& YYY,double& ZZZ,
       int& ParticleCode,
       double& RpcTime,
       int& AthPosOff
       );

extern "C" void mdigitgc_(
       int& Ident,
       char* STANAME,
       int& AreAmdbIds,
       int& StationEta,int& StationPhi,int& GasGap,int& IsStrip,int& Channel,  
       int& Jtyp,int& Jff,int& Jzz,int& Job,int& Jspli,int& Jsl,int& Jsz,int& Jstri,
       double& XXX,double& YYY,double& ZZZ,
       int& ParticleCode,
       int& AthPosOff
       );

extern "C" void mdigicsc_(
       int& Ident,
       char* STANAME,
       int& AreAmdbIds,
       int& StationEta,int& StationPhi,int& ChamberLayer,int& WireLayer,
       int& MeasuresPhi,int& Strip,
       int& Jtyp,int& Jff,int& Jzz,int& Job,int& Jsl,int& Jstri,
       double& SZflag,
       double& XXX,double& YYY,double& ZZZ,
       double& DepositedCharge,
       int& ParticleCode,
       int& AthPosOff
       );

extern "C" void mdigicscclu_(
       int& Ident,
       char* STANAME,
       int& AreAmdbIds,
       int& StationEta,int& StationPhi,int& ChamberLayer,int& WireLayer,
       int& MeasuresPhi,
       int& Jtyp,int& Jff,int& Jzz,int& Job,int& Jsl,
       double& SZflag,
       double& XXX,double& YYY,double& ZZZ, double& Reso,
       int& ParticleCode,
       double& DepositedCharge,
       double& Time,
       int& Status,
       int& TimeStatus
       );

extern "C" void inputgentrack_(
       int&   PDGCode ,
       double& PosX    ,
       double& PosY    ,
       double& PosZ    ,
       double& PX      ,
       double& PY      , 
       double& PZ      
       );

extern "C" void finish_mb_digis_(
       int&   ISTATIS 
       );


MuonboyCocoupMaker::MuonboyCocoupMaker(){}

MuonboyCocoupMaker::~MuonboyCocoupMaker(){}

void MuonboyCocoupMaker::initialize(){}


void MuonboyCocoupMaker::execute(
     const MboyPosDigiChoice* pMboyPosDigiChoice ,
     MuonRecDigiStore*        pMuonRecDigiStore,
     const MboyGenTrackStore* pMboyGenTrackStore
){
     
 int AthPosOffMDT = pMboyPosDigiChoice->GetPosOffMDT();
 int AthPosOffRPC = pMboyPosDigiChoice->GetPosOffRPC();
 int AthPosOffTGC = pMboyPosDigiChoice->GetPosOffTGC();
 int AthPosOffCSC = pMboyPosDigiChoice->GetPosOffCSC();

   int IdEve = 0 ;
   if (pMuonRecDigiStore) IdEve = pMuonRecDigiStore->GetEventNber();

//*Clear muonboy commons digits
   clearcoco_(IdEve);

  if (pMuonRecDigiStore){
//*Loop on MDT digis and give them to Muonboy
    int TotalNberOfDigi = pMuonRecDigiStore->NberOfDigi();
    for (int ItemNber=0; ItemNber<TotalNberOfDigi ; ItemNber++){

      int Ident = ItemNber;

      MuonRecDigitMDT*    pMuonRecDigitMDT    = pMuonRecDigiStore->NonCte_LiIdGetMuonRecDigitMDT(ItemNber)   ;
      if (pMuonRecDigitMDT){
	int DigitIdent = pMuonRecDigitMDT->GetIdentifier();
	if (DigitIdent != -1) Ident = DigitIdent;
        char STANAME[3];
        pMuonRecDigitMDT->GetStationNameChar(STANAME);
        int AreAmdbIds = pMuonRecDigitMDT->GetAreAmdbIds();
        int    StationEta = pMuonRecDigitMDT->GetSGStationEta() ;
        int    StationPhi = pMuonRecDigitMDT->GetSGStationPhi() ;
        int    Multilayer = pMuonRecDigitMDT->GetSGMultilayer() ;
        int    TubeLayer  = pMuonRecDigitMDT->GetSGTubeLayer () ;
        int    Tube       = pMuonRecDigitMDT->GetSGTube      () ;
        int    Jtyp       = pMuonRecDigitMDT->GetJTYP        () ;
        int    Jff        = pMuonRecDigitMDT->GetJFF         () ;
        int    Jzz        = pMuonRecDigitMDT->GetJZZ         () ;
        int    Job        = pMuonRecDigitMDT->GetJOB         () ;
        int    Jsl        = pMuonRecDigitMDT->GetJSL         () ;
        int    Jtube      = pMuonRecDigitMDT->GetJTUBE       () ;
        double XXX = pMuonRecDigitMDT->GetX();
        double YYY = pMuonRecDigitMDT->GetY();
        double ZZZ = pMuonRecDigitMDT->GetZ();
        int    ParticleCode    = pMuonRecDigitMDT->GetParticleCode();
        double DriftTime       = pMuonRecDigitMDT->GetDriftTime();
        int    TDCcount        = pMuonRecDigitMDT->GetTDCcount();
        int    ADCcount        = pMuonRecDigitMDT->GetADCcount();
        int    Status          = pMuonRecDigitMDT->GetStatus();
        XXX = XXX / 10. ;
        YYY = YYY / 10. ;
        ZZZ = ZZZ / 10. ;
        mdigimdt_(Ident,STANAME,
          AreAmdbIds,
          StationEta,StationPhi,Multilayer,TubeLayer,Tube,
          Jtyp,Jff,Jzz,Job,Jsl,Jtube,
          XXX,YYY,ZZZ,
          DriftTime,
          ParticleCode,
          TDCcount,
          ADCcount,
          Status,
          AthPosOffMDT
          );
        pMuonRecDigitMDT->SetSGStationEta(StationEta) ;
        pMuonRecDigitMDT->SetSGStationPhi(StationPhi) ;
        pMuonRecDigitMDT->SetSGMultilayer(Multilayer) ;
        pMuonRecDigitMDT->SetSGTubeLayer (TubeLayer ) ;
        pMuonRecDigitMDT->SetSGTube      (Tube      ) ;
        pMuonRecDigitMDT->SetJTYP        (Jtyp      ) ;
        pMuonRecDigitMDT->SetJFF         (Jff       ) ;
        pMuonRecDigitMDT->SetJZZ         (Jzz       ) ;
        pMuonRecDigitMDT->SetJOB         (Job       ) ;
        pMuonRecDigitMDT->SetJSL         (Jsl       ) ;
        pMuonRecDigitMDT->SetJTUBE       (Jtube     ) ;
      }else{

        MuonRecDigitRPC*    pMuonRecDigitRPC    = pMuonRecDigiStore->NonCte_LiIdGetMuonRecDigitRPC(ItemNber)   ;
        if (pMuonRecDigitRPC){
	int DigitIdent = pMuonRecDigitRPC->GetIdentifier();
	if (DigitIdent != -1) Ident = DigitIdent;
          char STANAME[3];
          pMuonRecDigitRPC->GetStationNameChar(STANAME);
          int AreAmdbIds = pMuonRecDigitRPC->GetAreAmdbIds();
          int    StationEta  = pMuonRecDigitRPC->GetSGStationEta () ; 
          int    StationPhi  = pMuonRecDigitRPC->GetSGStationPhi () ;
          int    DoubletR    = pMuonRecDigitRPC->GetSGDoubletR   () ;
          int    DoubletZ    = pMuonRecDigitRPC->GetSGDoubletZ   () ;
          int    DoubletPhi  = pMuonRecDigitRPC->GetSGDoubletPhi () ;
          int    GasGap      = pMuonRecDigitRPC->GetSGGasGap     () ;
          int    MeasuresPhi = pMuonRecDigitRPC->GetSGMeasuresPhi() ;
          int    Strip       = pMuonRecDigitRPC->GetSGStrip      () ;
          int    Jtyp        = pMuonRecDigitRPC->GetJTYP         () ;	      
          int    Jff         = pMuonRecDigitRPC->GetJFF          () ; 	     
          int    Jzz         = pMuonRecDigitRPC->GetJZZ          () ; 	     
          int    Job         = pMuonRecDigitRPC->GetJOB          () ; 	     
          int    Jspli       = pMuonRecDigitRPC->GetJSPLI        () ;   
          int    Jsl         = pMuonRecDigitRPC->GetJSL          () ; 	     
          int    Jsz         = pMuonRecDigitRPC->GetJSZ          () ; 	     
          int    Jstri       = pMuonRecDigitRPC->GetJSTRI        () ;   
          double RpcTime     = pMuonRecDigitRPC->GetRpcTime      () ;   
          double XXX = pMuonRecDigitRPC->GetX();  	     
          double YYY = pMuonRecDigitRPC->GetY();  	     
          double ZZZ = pMuonRecDigitRPC->GetZ();  	     
          int    ParticleCode = pMuonRecDigitRPC->GetParticleCode();    
          XXX = XXX / 10. ;
          YYY = YYY / 10. ;
          ZZZ = ZZZ / 10. ;
          mdigirpc_(Ident,STANAME,
            AreAmdbIds,
            StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,
            GasGap,MeasuresPhi,Strip,  
            Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
            XXX,YYY,ZZZ,
            ParticleCode,
            RpcTime,
            AthPosOffRPC
            );
          pMuonRecDigitRPC->SetSGStationEta (StationEta ) ; 
          pMuonRecDigitRPC->SetSGStationPhi (StationPhi ) ;
          pMuonRecDigitRPC->SetSGDoubletR   (DoubletR   ) ;
          pMuonRecDigitRPC->SetSGDoubletZ   (DoubletZ   ) ;
          pMuonRecDigitRPC->SetSGDoubletPhi (DoubletPhi ) ;
          pMuonRecDigitRPC->SetSGGasGap     (GasGap     ) ;
          pMuonRecDigitRPC->SetSGMeasuresPhi(MeasuresPhi) ;
          pMuonRecDigitRPC->SetSGStrip      (Strip      ) ;
          pMuonRecDigitRPC->SetJTYP         (Jtyp       ) ;		   
          pMuonRecDigitRPC->SetJFF          (Jff        ) ;		   
          pMuonRecDigitRPC->SetJZZ          (Jzz        ) ;		   
          pMuonRecDigitRPC->SetJOB          (Job        ) ;		   
          pMuonRecDigitRPC->SetJSPLI        (Jspli      ) ;	
          pMuonRecDigitRPC->SetJSL          (Jsl        ) ;		   
          pMuonRecDigitRPC->SetJSZ          (Jsz        ) ;		   
          pMuonRecDigitRPC->SetJSTRI        (Jstri      ) ;	
        }else{

          MuonRecDigitTGC*    pMuonRecDigitTGC    = pMuonRecDigiStore->NonCte_LiIdGetMuonRecDigitTGC(ItemNber)   ;
          if (pMuonRecDigitTGC){
	    int DigitIdent = pMuonRecDigitTGC->GetIdentifier();
	    if (DigitIdent != -1) Ident = DigitIdent;
            char STANAME[3];
            pMuonRecDigitTGC->GetStationNameChar(STANAME);
            int AreAmdbIds = pMuonRecDigitTGC->GetAreAmdbIds();
            int    StationEta = pMuonRecDigitTGC->GetSGStationEta () ;
            int    StationPhi = pMuonRecDigitTGC->GetSGStationPhi () ;
            int    GasGap     = pMuonRecDigitTGC->GetSGGasGap     () ;
            int    IsStrip    = pMuonRecDigitTGC->GetSGIsStrip    () ;
            int    Channel    = pMuonRecDigitTGC->GetSGChannel    () ;
            int    Jtyp       = pMuonRecDigitTGC->GetJTYP         () ;	      
            int    Jff        = pMuonRecDigitTGC->GetJFF          () ; 	     
            int    Jzz        = pMuonRecDigitTGC->GetJZZ          () ; 	     
            int    Job        = pMuonRecDigitTGC->GetJOB          () ; 	     
            int    Jspli      = pMuonRecDigitTGC->GetJSPLI        () ;   
            int    Jsl        = pMuonRecDigitTGC->GetJSL          () ; 	     
            int    Jsz        = pMuonRecDigitTGC->GetJSZ          () ; 	     
            int    Jstri      = pMuonRecDigitTGC->GetJSTRI        () ;   
            double XXX = pMuonRecDigitTGC->GetX();        
            double YYY = pMuonRecDigitTGC->GetY();        
            double ZZZ = pMuonRecDigitTGC->GetZ();        
            int    ParticleCode = pMuonRecDigitTGC->GetParticleCode();    
            XXX = XXX / 10. ;
            YYY = YYY / 10. ;
            ZZZ = ZZZ / 10. ;
            mdigitgc_(Ident,STANAME,
              AreAmdbIds,
              StationEta,StationPhi,GasGap,IsStrip,Channel,  
              Jtyp,Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
              XXX,YYY,ZZZ,
              ParticleCode,
              AthPosOffTGC
              );
            pMuonRecDigitTGC->SetSGStationEta (StationEta) ;
            pMuonRecDigitTGC->SetSGStationPhi (StationPhi) ;
            pMuonRecDigitTGC->SetSGGasGap     (GasGap    ) ;
            pMuonRecDigitTGC->SetSGIsStrip    (IsStrip   ) ;
            pMuonRecDigitTGC->SetSGChannel    (Channel   ) ;
            pMuonRecDigitTGC->SetJTYP         (Jtyp      ) ;	   
            pMuonRecDigitTGC->SetJFF          (Jff       ) ;	   
            pMuonRecDigitTGC->SetJZZ          (Jzz       ) ;	   
            pMuonRecDigitTGC->SetJOB          (Job       ) ;	   
            pMuonRecDigitTGC->SetJSPLI        (Jspli     ) ;   
            pMuonRecDigitTGC->SetJSL          (Jsl       ) ;	   
            pMuonRecDigitTGC->SetJSZ          (Jsz       ) ;	   
            pMuonRecDigitTGC->SetJSTRI        (Jstri     ) ;   
          }else{

            MuonRecDigitCSCClu*    pMuonRecDigitCSCClu    = pMuonRecDigiStore->NonCte_LiIdGetMuonRecDigitCSCClu(ItemNber);
            if (pMuonRecDigitCSCClu){
	      int DigitIdent = pMuonRecDigitCSCClu->GetIdentifier();
	      if (DigitIdent != -1) Ident = DigitIdent;
              char STANAME[3];
              pMuonRecDigitCSCClu->GetStationNameChar(STANAME);
              int AreAmdbIds = pMuonRecDigitCSCClu->GetAreAmdbIds();
              int    StationEta   = pMuonRecDigitCSCClu->GetSGStationEta  () ;
              int    StationPhi   = pMuonRecDigitCSCClu->GetSGStationPhi  () ;
              int    ChamberLayer = pMuonRecDigitCSCClu->GetSGChamberLayer() ;
              int    WireLayer    = pMuonRecDigitCSCClu->GetSGWireLayer   () ;
              int    MeasuresPhi  = pMuonRecDigitCSCClu->GetSGMeasuresPhi () ;
              int    Jtyp         = pMuonRecDigitCSCClu->GetJTYP          () ;	      
              int    Jff          = pMuonRecDigitCSCClu->GetJFF           () ;	    
              int    Jzz          = pMuonRecDigitCSCClu->GetJZZ           () ;	    
              int    Job          = pMuonRecDigitCSCClu->GetJOB           () ;	    
              int    Jsl          = pMuonRecDigitCSCClu->GetJSL           () ;	    
              double SZflag       = pMuonRecDigitCSCClu->GetSZFLAG        () ;  
              double XXX = pMuonRecDigitCSCClu->GetX();	      
              double YYY = pMuonRecDigitCSCClu->GetY();	      
              double ZZZ = pMuonRecDigitCSCClu->GetZ();	      
              double Reso = pMuonRecDigitCSCClu->GetError();
              int    ParticleCode    = pMuonRecDigitCSCClu->GetParticleCode(); 
              double DepositedCharge = pMuonRecDigitCSCClu->GetDepositedCharge();
              double Time            = pMuonRecDigitCSCClu->GetTime();
              int    Status          = pMuonRecDigitCSCClu->GetStatus();
              int    TimeStatus      = pMuonRecDigitCSCClu->GetTimeStatus();
	      XXX = XXX / 10. ;
	      YYY = YYY / 10. ;
	      ZZZ = ZZZ / 10. ;
	      Reso = Reso / 10. ;
              mdigicscclu_(Ident,STANAME,
		  AreAmdbIds,
		  StationEta,StationPhi,ChamberLayer,WireLayer,
		  MeasuresPhi,
		  Jtyp,Jff,Jzz,Job,Jsl,SZflag,
		  XXX,YYY,ZZZ,
		  Reso,
		  ParticleCode,
                  DepositedCharge,
                  Time,
                  Status,
                  TimeStatus
		  );
              pMuonRecDigitCSCClu->SetSGStationEta  (StationEta  ) ;
              pMuonRecDigitCSCClu->SetSGStationPhi  (StationPhi  ) ;
              pMuonRecDigitCSCClu->SetSGChamberLayer(ChamberLayer) ;
              pMuonRecDigitCSCClu->SetSGWireLayer   (WireLayer   ) ;
              pMuonRecDigitCSCClu->SetSGMeasuresPhi (MeasuresPhi ) ;
              pMuonRecDigitCSCClu->SetJTYP	  (Jtyp        ) ;	   
              pMuonRecDigitCSCClu->SetJFF 	  (Jff         ) ;	 
              pMuonRecDigitCSCClu->SetJZZ 	  (Jzz         ) ;	 
              pMuonRecDigitCSCClu->SetJOB 	  (Job         ) ;	 
              pMuonRecDigitCSCClu->SetJSL 	  (Jsl         ) ;	 
              pMuonRecDigitCSCClu->SetSZFLAG	  (SZflag      ) ;  
            }else{  

              MuonRecDigitCSC*    pMuonRecDigitCSC    = pMuonRecDigiStore->NonCte_LiIdGetMuonRecDigitCSC(ItemNber);
              if (pMuonRecDigitCSC){
	        int DigitIdent = pMuonRecDigitCSC->GetIdentifier();
	        if (DigitIdent != -1) Ident = DigitIdent;
                char STANAME[3];
                pMuonRecDigitCSC->GetStationNameChar(STANAME);
                int AreAmdbIds = pMuonRecDigitCSC->GetAreAmdbIds();
                int    StationEta   = pMuonRecDigitCSC->GetSGStationEta  () ;
                int    StationPhi   = pMuonRecDigitCSC->GetSGStationPhi  () ;
                int    ChamberLayer = pMuonRecDigitCSC->GetSGChamberLayer() ;
                int    WireLayer    = pMuonRecDigitCSC->GetSGWireLayer   () ;
                int    MeasuresPhi  = pMuonRecDigitCSC->GetSGMeasuresPhi () ;
                int    Strip        = pMuonRecDigitCSC->GetSGStrip       () ;
                int    Jtyp         = pMuonRecDigitCSC->GetJTYP          () ;	      
                int    Jff          = pMuonRecDigitCSC->GetJFF           () ;	    
                int    Jzz          = pMuonRecDigitCSC->GetJZZ           () ;	    
                int    Job          = pMuonRecDigitCSC->GetJOB           () ;	    
                int    Jsl          = pMuonRecDigitCSC->GetJSL           () ;	    
                int    Jstri        = pMuonRecDigitCSC->GetJSTRI         () ; 
                double SZflag       = pMuonRecDigitCSC->GetSZFLAG        () ;  
                double XXX = pMuonRecDigitCSC->GetX();	      
                double YYY = pMuonRecDigitCSC->GetY();	      
                double ZZZ = pMuonRecDigitCSC->GetZ();	      
                double DepositedCharge = pMuonRecDigitCSC->GetDepositedCharge();
                int    ParticleCode    = pMuonRecDigitCSC->GetParticleCode(); 
                XXX = XXX / 10. ;
                YYY = YYY / 10. ;
                ZZZ = ZZZ / 10. ;
                mdigicsc_(Ident,STANAME,
                  AreAmdbIds,
                  StationEta,StationPhi,ChamberLayer,WireLayer,
                  MeasuresPhi,Strip,
                  Jtyp,Jff,Jzz,Job,Jsl,Jstri,SZflag,
                  XXX,YYY,ZZZ,
                  DepositedCharge,
                  ParticleCode,
                  AthPosOffCSC
                  );
                pMuonRecDigitCSC->SetSGStationEta  (StationEta  ) ;
                pMuonRecDigitCSC->SetSGStationPhi  (StationPhi  ) ;
                pMuonRecDigitCSC->SetSGChamberLayer(ChamberLayer) ;
                pMuonRecDigitCSC->SetSGWireLayer   (WireLayer   ) ;
                pMuonRecDigitCSC->SetSGMeasuresPhi (MeasuresPhi ) ;
                pMuonRecDigitCSC->SetSGStrip	  (Strip       ) ;
                pMuonRecDigitCSC->SetJTYP	  (Jtyp        ) ;	   
                pMuonRecDigitCSC->SetJFF 	  (Jff         ) ;	 
                pMuonRecDigitCSC->SetJZZ 	  (Jzz         ) ;	 
                pMuonRecDigitCSC->SetJOB 	  (Job         ) ;	 
                pMuonRecDigitCSC->SetJSL 	  (Jsl         ) ;	 
                pMuonRecDigitCSC->SetJSTRI	  (Jstri       ) ; 
                pMuonRecDigitCSC->SetSZFLAG	  (SZflag      ) ;  
              }  

            } 
            
          } 
          
        } 
        
      } 
     
    }  

  }
  
  int ISTATIS = 1 ;
  finish_mb_digis_(ISTATIS);

  if (pMboyGenTrackStore){
    int NberOfGenTracks = pMboyGenTrackStore->NberOfMboyGenTrack();
    for (int GenTrackNber=0; GenTrackNber<NberOfGenTracks; GenTrackNber++){
      const MboyGenTrack* pMboyGenTrack = pMboyGenTrackStore->GetMboyGenTrack(GenTrackNber);
      int    PDGCode = pMboyGenTrack->GetPDGCode() ;
      double PosX    = pMboyGenTrack->GetPosX   () ;
      double PosY    = pMboyGenTrack->GetPosY   () ;
      double PosZ    = pMboyGenTrack->GetPosZ   () ;
      double PX      = pMboyGenTrack->GetPX     () ;
      double PY      = pMboyGenTrack->GetPY     () ;
      double PZ      = pMboyGenTrack->GetPZ     () ;
      inputgentrack_(
        PDGCode ,
        PosX    ,
        PosY    ,
        PosZ    ,
        PX      ,
        PY      , 
        PZ 
      );     
    }
  }

}


void MuonboyCocoupMaker::finalize(){}
