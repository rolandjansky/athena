/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcStand/AmdcsimrecStand.h"
 
#include "AmdcStand/GetAmdcsimrecVariable.h"

#include "AmdcStand/amdcread.h"

#include "TomoTubeDisplacement.h"
#include "TomoTubeDisplacementContinuous.h"

#include "f1get.h"
#include "getindfromamdc.h"
#include "getamdcindfrom.h"
#include "fgetamdcagdd.h"
#include "getnotnomgeo.h"
#include "fgetranges.h"
#include "f1perform.h"

using std::setiosflags;

int AmdcsimrecStand::initFromAscii (std::string fileName)
{
  // Returns non null value on success

  //*Initialize Muon Spectrometer Geometry
  int  SizeName = fileName.size();
  char* FileName = new char[SizeName];
  int  Istate = 0;
  for (int i= 0; i <SizeName; i++) { FileName[i] = fileName[i]; }

  int Iflag = 100 ;
  amdcreadnn_(FileName,SizeName,Istate,Iflag);
  delete [] FileName ;

  return Istate;
}

//*Class implementation
AmdcsimrecStand::AmdcsimrecStand(){}
 
AmdcsimrecStand::~AmdcsimrecStand(){}
 
// Range Sizes
int AmdcsimrecStand::Mtec(){std::string Name="MTEC" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Msta(){std::string Name="MSTA" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mtyp(){std::string Name="MTYP" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mgeo(){std::string Name="MGEO" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mphi(){std::string Name="MPHI" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mzz() {std::string Name="MZZ"  ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mstrp() {std::string Name="Mstrp"  ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::Mgang() {std::string Name="Mgang"  ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
 
// amdcchapos
double AmdcsimrecStand::PosPhi( int jtyp, int jphi, int jzz){std::string Name="Pos_Phi" ; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosZ(   int jtyp, int jphi, int jzz){std::string Name="Pos_Z"   ; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosR(   int jtyp, int jphi, int jzz){std::string Name="Pos_R"   ; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosS(   int jtyp, int jphi, int jzz){std::string Name="Pos_S"   ; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosAlfa(int jtyp, int jphi, int jzz){std::string Name="Pos_Alfa"; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosBeta(int jtyp, int jphi, int jzz){std::string Name="Pos_Beta"; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
double AmdcsimrecStand::PosGama(int jtyp, int jphi, int jzz){std::string Name="Pos_Gama"; return GetAmdcRealVar(Name,jtyp,jphi,jzz);}
int    AmdcsimrecStand::IGEO(   int jtyp, int jphi, int jzz){std::string Name="IGEO"    ; return GetAmdcIntVar(Name,jtyp,jphi,jzz);}
int    AmdcsimrecStand::ICUT(   int jtyp, int jphi, int jzz){std::string Name="ICUT"    ; return GetAmdcIntVar(Name,jtyp,jphi,jzz);}
int    AmdcsimrecStand::IBAREL( int jtyp, int jphi, int jzz){std::string Name="IBAREL"  ; return GetAmdcIntVar(Name,jtyp,jphi,jzz);}
 
// amdcchageo
double AmdcsimrecStand::Geodx (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_dx" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::Geody (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_dy" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::Geodz (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_dz" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoWs (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_Ws" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoWl (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_Wl" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoLe (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_Le" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoEx (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_Ex" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoD1 (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_D1" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoD2 (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_D2" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
double AmdcsimrecStand::GeoD3 (int Jtyp,int Jgeo,int Jobj){std::string Name="Geo_D3" ; return GetAmdcRealVar(Name,Jtyp,Jgeo,Jobj);}
int    AmdcsimrecStand::NOBJ  (int Jtyp,int Jgeo)         {std::string Name="NOBJ"   ; int Ibid=0 ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Ibid);}
int    AmdcsimrecStand::ITEC  (int Jtyp,int Jgeo,int Jobj){std::string Name="ITEC"   ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Jobj);}
int    AmdcsimrecStand::ISTA  (int Jtyp,int Jgeo,int Jobj){std::string Name="ISTA"   ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Jobj);}
int    AmdcsimrecStand::ISPLIX(int Jtyp,int Jgeo,int Jobj){std::string Name="ISPLIX" ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Jobj);}
int    AmdcsimrecStand::ISPLIY(int Jtyp,int Jgeo,int Jobj){std::string Name="ISPLIY" ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Jobj);}
int    AmdcsimrecStand::ISHAPE(int Jtyp,int Jgeo,int Jobj){std::string Name="ISHAPE" ; return GetAmdcIntVar(Name,Jtyp,Jgeo,Jobj);}
 
// amdcchacut
double AmdcsimrecStand::Cutdx(int jcut, int jocu){std::string Name="Cut_dx"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::Cutdy(int jcut, int jocu){std::string Name="Cut_dy"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::CutWs(int jcut, int jocu){std::string Name="Cut_Ws"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::CutWl(int jcut, int jocu){std::string Name="Cut_Wl"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::CutLe(int jcut, int jocu){std::string Name="Cut_Le"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::CutEx(int jcut, int jocu){std::string Name="Cut_Ex"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
double AmdcsimrecStand::CutAn(int jcut, int jocu){std::string Name="Cut_An"   ; int Ibid=0 ; return GetAmdcRealVar(Name,jcut,jocu,Ibid);}
int    AmdcsimrecStand::IOBCUT(int jcut,int jocu){std::string Name="IOBCUT"   ; int Ibid=0 ; return GetAmdcIntVar(Name,jcut,jocu,Ibid);}
int    AmdcsimrecStand::NOBCUT(int joc2){std::string Name="NOBCUT"   ; int Ibid1=0 ; int Ibid2=0 ; return GetAmdcIntVar(Name,joc2,Ibid1,Ibid2);}
int    AmdcsimrecStand::INOCUT(int jtyp, int jgeo, int joc2){std::string Name="INOCUT" ; return GetAmdcIntVar(Name,jtyp,jgeo,joc2);}
 
// amdcchasta
double AmdcsimrecStand::STAX0 (int Jtec,int Jsta)       {std::string Name="STAX0"   ; int Ibid=0 ; return GetAmdcRealVar(Name,Jtec,Jsta,Ibid);}
double AmdcsimrecStand::STAPP (int Jtec,int Jsta)       {std::string Name="STAPP"   ; int Ibid=0 ; return GetAmdcRealVar(Name,Jtec,Jsta,Ibid);}
double AmdcsimrecStand::STARR (int Jtec,int Jsta)       {std::string Name="STARR"   ; int Ibid=0 ; return GetAmdcRealVar(Name,Jtec,Jsta,Ibid);}
double AmdcsimrecStand::STAEE (int Jtec,int Jsta)       {std::string Name="STAEE"   ; int Ibid=0 ; return GetAmdcRealVar(Name,Jtec,Jsta,Ibid);}
double AmdcsimrecStand::STAPG (int Jtec,int Jsta)       {std::string Name="STAPG"   ; int Ibid=0 ; return GetAmdcRealVar(Name,Jtec,Jsta,Ibid);}
double AmdcsimrecStand::STATT (int Jtec,int Jsta,int Jl){std::string Name="STATT"   ; return GetAmdcRealVar(Name,Jtec,Jsta,Jl);}
double AmdcsimrecStand::STAOO (int Jtec,int Jsta,int Jl){std::string Name="STAOO"   ; return GetAmdcRealVar(Name,Jtec,Jsta,Jl);}
int    AmdcsimrecStand::NLAZ  (int Jtec,int Jsta)       {std::string Name="NLAZ"   ; int Ibid=0 ; return GetAmdcIntVar(Name,Jtec,Jsta,Ibid);}
int    AmdcsimrecStand::NLAS  (int Jtec,int Jsta)       {std::string Name="NLAS"   ; int Ibid=0 ; return GetAmdcIntVar(Name,Jtec,Jsta,Ibid);}
int    AmdcsimrecStand::ISTAMA(int Jtec,int Jsta,int Jl){std::string Name="ISTAMA" ; return GetAmdcIntVar(Name,Jtec,Jsta,Jl);}
int    AmdcsimrecStand::IFORMA(int Jtec,int Jsta)       {std::string Name="IFORMA"    ; int Ibid=0 ; return GetAmdcIntVar(Name,Jtec,Jsta,Ibid);}
int    AmdcsimrecStand::IVALIDSTA(int Jtec,int Jsta)    {std::string Name="IVALIDSTA" ; int Ibid=0 ; return GetAmdcIntVar(Name,Jtec,Jsta,Ibid);}
 
// amdcchacar
int AmdcsimrecStand::GetJtyp(std::string TheStationName){

  int ToBeReturned = -1 ;
  for(int Jtyp = 1; Jtyp <=Mtyp(); Jtyp++){
   std::string aStationName = StationName(Jtyp);
   if (TheStationName == aStationName) ToBeReturned = Jtyp;
  }
  return ToBeReturned;

}
std::string AmdcsimrecStand::StationName(int Jtyp){

  int CHARL1 = 3 ;
  std::string TheStationName = "XXX";
  if (Jtyp<=Mtyp()) {
   TheStationName.resize(CHARL1) ;
   std::string Name = "CARTYP";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheStationNameIS = GetAmdcCharVar(Name,Jtyp,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheStationName[i]=TheStationNameIS[i];}
  }
  return TheStationName;

}
int AmdcsimrecStand::GetJtec(std::string TheTechnologyName){

  int ToBeReturned = -1 ;
  for(int Jtec = 1; Jtec <=Mtec(); Jtec++){
   std::string aTechnologyName = TechnologyName(Jtec);
   std::string aTechnologyNameTest(aTechnologyName,0,2);
   if (aTechnologyNameTest == "LB") aTechnologyName = aTechnologyNameTest;
   if (TheTechnologyName == aTechnologyName) ToBeReturned = Jtec;
  }
  return ToBeReturned;

}
std::string AmdcsimrecStand::TechnologyName(int Jtec){

  int CHARL1 = 3 ;
  std::string TheTechnologyName = "XXX";
  if (Jtec<=Mtec()) {
   TheTechnologyName.resize(CHARL1) ;
   std::string Name = "CARTEC";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheTechnologyNameIS = GetAmdcCharVar(Name,Jtec,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheTechnologyName[i]=TheTechnologyNameIS[i];}
  }
  return TheTechnologyName;

}
 
// amdcchastatgc
double AmdcsimrecStand::XtgcAdd  (int Jsta,int I116)          {std::string Name="XtgcAdd"  ; int Ibid=0 ; return GetAmdcRealVar(Name,Jsta,I116,Ibid);}
double AmdcsimrecStand::XtgcStrp (int Jsta,int Istrp,int I12) {std::string Name="XtgcStrp" ;              return GetAmdcRealVar(Name,Jsta,Istrp,I12);}
int    AmdcsimrecStand::NtgcAdd  (int Jsta,int I110)	      {std::string Name="NtgcAdd"  ; int Ibid=0 ; return GetAmdcIntVar(Name,Jsta,I110,Ibid);}
int    AmdcsimrecStand::NtgcGang (int Jsta,int Jgang,int I13) {std::string Name="NtgcGang" ;		  return GetAmdcIntVar(Name,Jsta,Jgang,I13);}

// amdcchadjust
int AmdcsimrecStand::NBadjust (){std::string Name="NBadjust" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}

// amdcchadefor
int AmdcsimrecStand::NBdeform (){std::string Name="NBdeform" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}

// amdcchdwnlay
int AmdcsimrecStand::NBdwnlay (){std::string Name="NBdwnlay" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}

// amdccovers
std::string AmdcsimrecStand::AmdcName(){

  int CHARL1 = 16 ;
  std::string TheAmdcName = "XXXXXXXXXXXXXXXX";
  TheAmdcName.resize(CHARL1) ;
  std::string Name = "C_AMDC_NAME";
  int Ibid1 = 0 ;
  int Ibid2 = 0 ;
  int Ibid3 = 0 ;
  std::string TheAmdcNameIS = GetAmdcCharVar(Name,Ibid1,Ibid2,Ibid3);
  for (int i= 0; i <CHARL1; i++){TheAmdcName[i]=TheAmdcNameIS[i];}
  return TheAmdcName;

}
int AmdcsimrecStand::AmdcVersion (){std::string Name="I_AMDC_VERSION" ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int AmdcsimrecStand::VersionAmdc (){std::string Name="IVERSION_AMDC"  ;int I1=0;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
 
// Strings
std::string AmdcsimrecStand::GetAmdcString() {
   std::string ToBeReturned = "";
   int  Namdc = 0;
   fgetamdccharnumber_(Namdc);
   if (Namdc!=0){
     char* Camdc = new char[Namdc];
     fgetamdccharstring_(Namdc,Camdc);
     ToBeReturned.resize(Namdc);
     for (int i= 0; i <Namdc; i++){ToBeReturned[i]=Camdc[i];}
     delete [] Camdc ;
   }
   return ToBeReturned;
}
std::string AmdcsimrecStand::GetAgddString(){
   std::string ToBeReturned = "";
   int  Nagdd = 0;
   fgetagddcharnumber_(Nagdd);
   if (Nagdd!=0){
     char* Cagdd = new char[Nagdd];
     fgetagddcharstring_(Nagdd,Cagdd);
     ToBeReturned.resize(Nagdd);
     for (int i= 0; i <Nagdd; i++){ToBeReturned[i]=Cagdd[i];}
     delete [] Cagdd ;
   } 
   return ToBeReturned;
}

// More Infos
int AmdcsimrecStand::GetIzzMax(std::string TheStationName){

  int ToBeReturned = -1 ;
  if (GetJtyp(TheStationName) < 0) return ToBeReturned;
  for(int Jff = 1; Jff <=Mphi(); Jff++){
   int kountJzz=0;
   for(int Jzz = 1; Jzz <=Mzz(); Jzz++){
    if (IGEO(GetJtyp(TheStationName),Jff,Jzz) != 0){
     kountJzz = kountJzz + 1 ;
    }
   }
   if (kountJzz > ToBeReturned) ToBeReturned = kountJzz ;
  }
  return ToBeReturned;

}
int AmdcsimrecStand::GetJzzMax(std::string TheStationName){

  int ToBeReturned = -1 ;
  int Jtyp = GetJtyp(TheStationName);
  if (Jtyp < 0) return ToBeReturned;
  for(int Jff = 1; Jff <=Mphi(); Jff++){
   for(int Jzz = 1; Jzz <=Mzz(); Jzz++){
    if (IGEO(Jtyp,Jff,Jzz) != 0){
     if (Jzz > ToBeReturned) ToBeReturned = Jzz;
    }
   }
  }
  return ToBeReturned;

}
int AmdcsimrecStand::GetNobMax(std::string TheStationName){

  int ToBeReturned = -1 ;
  int Jtyp = GetJtyp(TheStationName);
  if (Jtyp < 0) return ToBeReturned;
  for(int Jff = 1; Jff <=Mphi(); Jff++){
   for(int Jzz = 1; Jzz <=Mzz(); Jzz++){
    int Jgeo = IGEO(Jtyp,Jff,Jzz);
    if (Jgeo != 0){
     if (NOBJ(Jtyp,Jgeo) > ToBeReturned) ToBeReturned = NOBJ(Jtyp,Jgeo);
    }
   }
  }
  return ToBeReturned;

}
int AmdcsimrecStand::GetJobj(std::string TheTechnologyName,std::string TheStationName,
                           int Jff,int Jzz,int IndexOjct){

  int ToBeReturned = -1 ;
  int Jtyp = GetJtyp(TheStationName);
  if (Jtyp < 0) return ToBeReturned;
  int Jgeo = IGEO(Jtyp,Jff,Jzz);
  if (Jgeo==0) return ToBeReturned;
  int kountObj = 0;
  for (int Jobj=1;Jobj<=NOBJ(Jtyp,Jgeo);Jobj++){
   std::string aTechnologyName = TechnologyName(ITEC(Jtyp,Jgeo,Jobj));
   if (TheTechnologyName==aTechnologyName){
    kountObj = kountObj + 1 ;
    if (kountObj==IndexOjct) ToBeReturned = Jobj;
   }
  }
  return ToBeReturned;

}


void AmdcsimrecStand::GetStationDisplacementFromAMDC(
   std::string& StationName, int& Jff, int& Jzz,
   double& TranslatS, double& TranslatZ, double& TranslatT,
   double& RotatS, double& RotatZ, double& RotatT ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getdisplacementfromamdcjobn_(STANAME,Jff,Jzz,
     TranslatS,TranslatZ,TranslatT,RotatS,RotatZ,RotatT);
}
void AmdcsimrecStand::GetStationDisplacementFromAMDC(
   std::string& StationName, int& Jff, int& Jzz,int& Job,
   double& TranslatS, double& TranslatZ, double& TranslatT,
   double& RotatS, double& RotatZ, double& RotatT ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getdisplacementfromamdc_(STANAME,Jff,Jzz,Job,
     TranslatS,TranslatZ,TranslatT,RotatS,RotatZ,RotatT);
}

void AmdcsimrecStand::GetStationDisplacementFromAMDCJadjust(
   int& Jadjust,
   int& Jtyp, int& Jff, int& Jzz, int& Jobj,
   double& TranslatS, double& TranslatZ, double& TranslatT,
   double& RotatS, double& RotatZ, double& RotatT ){
     getdisplacementfromamdcjadjust_(Jadjust,Jtyp,Jff,Jzz,Jobj,
     TranslatS,TranslatZ,TranslatT,RotatS,RotatZ,RotatT);
}

void AmdcsimrecStand::GetStationDeformationFromAMDC(
   std::string& StationName, int& Jff, int& Jzz,
   double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
   double& pg, double& tr, double& eg, double& ep, double& en ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getdeformationfromamdcjobn_(STANAME,Jff,Jzz,
			     bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en);
}
void AmdcsimrecStand::GetStationDeformationFromAMDC(
   std::string& StationName, int& Jff, int& Jzz,int& Job,
   double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
   double& pg, double& tr, double& eg, double& ep, double& en ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getdeformationfromamdc_(STANAME,Jff,Jzz,Job,
			     bz,bp,bn,sp,sn,tw,pg,tr,eg,ep,en);
}
void AmdcsimrecStand::GetStationDeformationFromAMDCJdeform(
               int& Jdeform,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj,
               double& bz, double& bp, double& bn, double& sp, double& sn, double& tw,
               double& pg, double& tr, double& eg, double& ep, double& en ){
      getdeformationfromamdcjdeform_( 
               Jdeform,
               Jtyp, Jff, Jzz, Jobj,
	       bz, bp, bn, sp, sn, tw, pg, tr, eg, ep, en);
}
void AmdcsimrecStand::GetStationInternalAlignmentFromAMDCJdwnlay(
               int& Jdwnlay,
               int& Jtyp, int& Jff, int& Jzz, int& Jobj, int& Jlay,
               double& TranslatS, double& TranslatZ, double& TranslatT,
               double& RotatS, double& RotatZ, double& RotatT ){
      getinternalalignmentfromamdcjdwnlay_( 
               Jdwnlay,
               Jtyp, Jff, Jzz, Jobj,Jlay,
               TranslatS,TranslatZ,TranslatT,RotatS,RotatZ,RotatT);
}

//                  ***
void AmdcsimrecStand::GetMDTindexFromAMDC( std::string& StationName,
   int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube,
   int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getmdtindfromamdc_(STANAME,
     Jff,Jzz,Job,Jsl,Jtube,
     StationEta,StationPhi,Multilayer,TubeLayer,Tube);
}
//                  ***
void AmdcsimrecStand::GetCSCindexFromAMDC( std::string& StationName,
   int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag,
   int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
   int& MeasuresPhi, int& Strip ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getcscindfromamdc_(STANAME,
     Jff,Jzz,Job,Jsl,Jtube,SZflag,
     StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip );
}
//                  ***
void AmdcsimrecStand::GetRPCindexFromAMDC( std::string& StationName,
   int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
   int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
   int& GasGap, int& MeasuresPhi, int& Strip ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getrpcindfromamdc_(STANAME,
     Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip );
}
//                  ***
void AmdcsimrecStand::GetTGCindexFromAMDC( std::string& StationName,
   int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri,
   int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     gettgcindfromamdc_(STANAME,
     Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri,
     StationEta,StationPhi,GasGap,IsStrip,Channel );
}


//                               ***
void AmdcsimrecStand::GetAMDCindexFromMDT( std::string& StationName,
   int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube,
   int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getamdcindfrommdt_(STANAME,
     StationEta,StationPhi,Multilayer,TubeLayer,Tube,
     Jff,Jzz,Job,Jsl,Jtube );
}
//                               ***
void AmdcsimrecStand::GetAMDCindexFromCSC( std::string& StationName,
   int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
   int& MeasuresPhi, int& Strip,
   int& Jff, int& Jzz, int& Job, int& Jsl, int& Jtube, double& SZflag ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getamdcindfromcsc_(STANAME,
     StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip,
     Jff,Jzz,Job,Jsl,Jtube,SZflag );
}
//                               ***
void AmdcsimrecStand::GetAMDCindexFromRPC( std::string& StationName,
   int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
   int& GasGap, int& MeasuresPhi, int& Strip,
   int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getamdcindfromrpc_(STANAME,
     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip,
     Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri );
}
//                               ***
void AmdcsimrecStand::GetAMDCindexFromTGC( std::string& StationName,
   int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
   int& Jff, int& Jzz, int& Job, int& Jspli, int& Jsl, int& Jsz, int& Jstri ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     getamdcindfromtgc_(STANAME,
     StationEta,StationPhi,GasGap,IsStrip,Channel,
     Jff,Jzz,Job,Jspli,Jsl,Jsz,Jstri );
}


//                  ****            ***
void AmdcsimrecStand::GetTubePositionFromMDTindex( std::string& StationName,
   int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer, int& Tube,
   double* XYZ1, double* XYZ2,
   double* SZT1, double* SZT2, double& Radius){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1gettubepositionfrommdtindex_(STANAME,
     StationEta,StationPhi,Multilayer,TubeLayer,Tube,
     XYZ1,XYZ2, SZT1,SZT2, Radius );
}
//                  *****            ***
void AmdcsimrecStand::GetStripPositionFromCSCindex( std::string& StationName,
   int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
   int& MeasuresPhi, int& Strip,
   double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4, double* XYZCenter,
   double* SZT1, double* SZT2, double* SZT3, double* SZT4, double* SZTCenter ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getstrippositionfromcscindex_(STANAME,
     StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,Strip,
     XYZ1,XYZ2,XYZ3,XYZ4, XYZCenter, SZT1,SZT2,SZT3,SZT4, SZTCenter );
}
//                  *****            ***
void AmdcsimrecStand::GetStripPositionFromRPCindex( std::string& StationName,
   int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
   int& GasGap, int& MeasuresPhi, int& Strip,
   double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
   double* SZT1, double* SZT2, double* SZT3, double* SZT4 ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getstrippositionfromrpcindex_(STANAME,
     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,Strip,
     XYZ1,XYZ2,XYZ3,XYZ4, SZT1,SZT2,SZT3,SZT4 );
}
//                  *******            ***
void AmdcsimrecStand::GetChannelPositionFromTGCindex( std::string& StationName,
   int& StationEta, int& StationPhi, int& GasGap, int& IsStrip, int& Channel,
   double* XYZ1, double* XYZ2, double* XYZ3, double* XYZ4,
   double* SZT1, double* SZT2, double* SZT3, double* SZT4 ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getchannelpositionfromtgcind_(STANAME,
     StationEta,StationPhi,GasGap,IsStrip,Channel,
     XYZ1,XYZ2,XYZ3,XYZ4, SZT1,SZT2,SZT3,SZT4 );
}


//                  ***         ****
void AmdcsimrecStand::GetMDTindexFromTubePosition( std::string& StationName,
   int& StationEta, int& StationPhi, int& Multilayer, int& TubeLayer,
   double* XYZ, int& Tube ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getmdtindexfromtubeposition_(STANAME,
     StationEta,StationPhi,Multilayer,TubeLayer,
     XYZ, Tube );
}
//                  ***         *****
void AmdcsimrecStand::GetCSCindexFromStripPosition( std::string& StationName,
   int& StationEta, int& StationPhi, int& ChamberLayer, int& WireLayer,
   int& MeasuresPhi,
   double* XYZ, int& Strip ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getcscindexfromstripposition_(STANAME,
     StationEta,StationPhi,ChamberLayer,WireLayer,MeasuresPhi,
     XYZ, Strip );
}
//                  ***         *****
void AmdcsimrecStand::GetRPCindexFromStripPosition( std::string& StationName,
   int& StationEta, int& StationPhi, int& DoubletR, int& DoubletZ, int& DoubletPhi,
   int& GasGap, int& MeasuresPhi,
   double* XYZ, int& Strip ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1getrpcindexfromstripposition_(STANAME,
     StationEta,StationPhi,DoubletR,DoubletZ,DoubletPhi,GasGap,MeasuresPhi,
     XYZ, Strip );
}
//                  ***         *******
void AmdcsimrecStand::GetTGCindexFromChannelPosition( std::string& StationName,
   int& StationEta, int& StationPhi, int& GasGap, int& IsStrip,
   double* XYZ, int& Channel ){
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     f1gettgcindexfromchannelpos_(STANAME,
     StationEta,StationPhi,GasGap,IsStrip,
     XYZ, Channel );
}


//  Extract information from AMDC :
// *********************************

int         AmdcsimrecStand::StationTypeBegin(){
     int JtypBeg = 0;
     int JtypEnd = 0;
     fstationtyperange_( JtypBeg, JtypEnd );
     return JtypBeg;
}
int         AmdcsimrecStand::StationTypeEnd(){
     int JtypBeg = 0;
     int JtypEnd = 0;
     fstationtyperange_( JtypBeg, JtypEnd );
     return JtypEnd;
}
std::string AmdcsimrecStand::GetStationType( int& Jtyp ){
     char STANAME[3];
     f1getstationtype_( Jtyp, STANAME );
     std::string Ctyp = "???";
     Ctyp[0] = STANAME[0];
     Ctyp[1] = STANAME[1];
     Ctyp[2] = STANAME[2];
     return Ctyp;
}
int         AmdcsimrecStand::StationGeomBegin(){
     int JgeoBeg = 0;
     int JgeoEnd = 0;
     fstationgeomrange_( JgeoBeg, JgeoEnd );
     return JgeoBeg;
}
int         AmdcsimrecStand::StationGeomEnd(){
     int JgeoBeg = 0;
     int JgeoEnd = 0;
     fstationgeomrange_( JgeoBeg, JgeoEnd );
     return JgeoEnd;
}
void        AmdcsimrecStand::GetStationDimensions( std::string& StationType, int& Jgeo, int& Jcut,
   int& IsValid, double& DimLocX, double& DimLocY, double& DimLocZ,
   double& CenterLocX, double& CenterLocY, double& CenterLocZ ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1getstationdimensions_( STANAME,Jgeo,Jcut, IsValid,
     DimLocX,DimLocY,DimLocZ, CenterLocX,CenterLocY,CenterLocZ );
}
int         AmdcsimrecStand::GetNumberOfStationObjects( std::string& StationType, int& Jgeo ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     int NbObj = 0;
     f1getnumberofstationobjects_( STANAME,Jgeo, NbObj );
     return NbObj;
}
void        AmdcsimrecStand::GetStationObjectParam( std::string& StationType, int& Jgeo, int& Job,
   int& GeomIsValid, std::string& GeomTechnoName, int& GeomTechnoIndex,
   int& GeomSplitX, int& GeomSplitY, int& GeomShapeIndex,
   double& GeomDx, double& GeomDy, double& GeomDz, double& GeomWs, double& GeomWl,
   double& GeomLe, double& GeomEx, double& GeomD1, double& GeomD2, double& GeomD3 ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     char TECNAME[3];
     f1getstationobjectparam_( STANAME,Jgeo,Job,
     GeomIsValid, TECNAME, GeomTechnoIndex, GeomSplitX, GeomSplitY, GeomShapeIndex,
     GeomDx, GeomDy, GeomDz, GeomWs, GeomWl,
     GeomLe, GeomEx, GeomD1, GeomD2, GeomD3 );
     GeomTechnoName.resize(3);
     GeomTechnoName[0] = TECNAME[0];
     GeomTechnoName[1] = TECNAME[1];
     GeomTechnoName[2] = TECNAME[2];
}
int         AmdcsimrecStand::PositionPhiBegin(){
     int JffBeg = 0;
     int JffEnd = 0;
     fpositionphirange_( JffBeg, JffEnd );
     return JffBeg;
}
int         AmdcsimrecStand::PositionPhiEnd(){
     int JffBeg = 0;
     int JffEnd = 0;
     fpositionphirange_( JffBeg, JffEnd );
     return JffEnd;
}
int         AmdcsimrecStand::PositionZBegin(){
     int JzzBeg = 0;
     int JzzEnd = 0;
     fpositionzrange_( JzzBeg, JzzEnd );
     return JzzBeg;
}
int         AmdcsimrecStand::PositionZEnd(){
     int JzzBeg = 0;
     int JzzEnd = 0;
     fpositionzrange_( JzzBeg, JzzEnd );
     return JzzEnd;
}
void        AmdcsimrecStand::GetStationPositionParam( std::string& StationType, int& Jff, int& Jzz,
   int& PosiIsValid, int& PosiJgeo, int& PosiJcut, int& PosiIsBarrel,
   double& PosiPhi, double& PosiZ, double& PosiR, double& PosiS,
   double& PosiAlfa, double& PosiBeta, double& PosiGamma ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1getstationpositionparam_( STANAME,Jff,Jzz,
     PosiIsValid, PosiJgeo,PosiJcut,PosiIsBarrel,
     PosiPhi,PosiZ,PosiR,PosiS,PosiAlfa,PosiBeta,PosiGamma );
}
int         AmdcsimrecStand::StationCutOutBegin(){
     int JcutBeg = 0;
     int JcutEnd = 0;
     fcutoutrange_( JcutBeg, JcutEnd );
     return JcutBeg;
}
int         AmdcsimrecStand::StationCutOutEnd(){
     int JcutBeg = 0;
     int JcutEnd = 0;
     fcutoutrange_( JcutBeg, JcutEnd );
     return JcutEnd;
}
int         AmdcsimrecStand::GetNumberOfCutOutObjects( std::string& StationType, int& Jgeo, int& Jcut ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     int NbObjCut = 0;
     f1getnumberofcutoutobjects_( STANAME,Jgeo,Jcut, NbObjCut );
     return NbObjCut;
}
void        AmdcsimrecStand::GetStationCutOutObjParam( std::string& StationType, int& Jgeo, int& Jcut, int& JcutObj,
     int& CutJob,   double& CutDx, double& CutDy,
     double& CutWs, double& CutWl, double& CutLe, double& CutEx, double& CutAn ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1getstationcutoutobjparam_( STANAME,Jgeo,Jcut,JcutObj,
     CutJob,CutDx,CutDy,CutWs,CutWl,CutLe,CutEx,CutAn );
}
void        AmdcsimrecStand::PerformDirectDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,
     double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1performdirectdisplacanddeform_( STANAME,Jff,Jzz, Xin,Yin,Zin, Xout,Yout,Zout );
}
void        AmdcsimrecStand::PerformDirectDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz, int& Job,
     double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1performdirectdisplacanddeformbase_( STANAME,Jff,Jzz, Job, Xin,Yin,Zin, Xout,Yout,Zout );
}
void        AmdcsimrecStand::PerformBackDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz,
     double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1performbackdisplacanddeform_( STANAME,Jff,Jzz, Xin,Yin,Zin, Xout,Yout,Zout );
}
void        AmdcsimrecStand::PerformBackDisplacAndDeform( std::string& StationType, int& Jff, int& Jzz, int& Job,
     double& Xin, double& Yin, double& Zin, double& Xout, double& Yout, double& Zout ){
     char STANAME[3];
     STANAME[0] = StationType[0];
     STANAME[1] = StationType[1];
     STANAME[2] = StationType[2];
     f1performbackdisplacanddeformbase_( STANAME,Jff,Jzz, Job, Xin,Yin,Zin, Xout,Yout,Zout );
}

// amdchardsoft
int          AmdcsimrecStand::GetHARDENTRIES()           {std::string Name="HARDENTRIES" ;int I1=0     ;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
std::string  AmdcsimrecStand::GetHARDNAME   (int iEntry) 
{
  int CHARL1 = 7 ;
  std::string TheHardName = "XXXXXXX";
  if (iEntry<=GetHARDENTRIES()) {
   TheHardName.resize(CHARL1) ;
   std::string Name = "HARDNAME";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheHardNameIS = GetAmdcCharVar(Name,iEntry,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheHardName[i]=TheHardNameIS[i];}
  }
  return TheHardName;

}
std::string  AmdcsimrecStand::GetSOFTNAME   (int iEntry) 
{
  int CHARL1 = 3 ;
  std::string TheSoftName = "XXX";
  if (iEntry<=GetHARDENTRIES()) {
   TheSoftName.resize(CHARL1) ;
   std::string Name = "SOFTNAME";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheSoftNameIS = GetAmdcCharVar(Name,iEntry,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheSoftName[i]=TheSoftNameIS[i];}
  }
  return TheSoftName;

}
int          AmdcsimrecStand::GetSOFTOCTANT (int iEntry) {std::string Name="SOFTOCTANT" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetSOFTIZ     (int iEntry) {std::string Name="SOFTIZ"     ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetEntryRank  (std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) 
{
  std::string Name="IentryHardSoft" ;int I1= GetJtyp(SOFTNAME);int I2=SOFTOCTANT; int I3=SOFTIZ; return GetAmdcIntVar(Name,I1,I2,I3);
}
int          AmdcsimrecStand::GetEntryRank  (std::string HARDNAME)  
{
  int iHardEntries_Max = GetHARDENTRIES();
  int ToBeReturned = -1 ;
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
    if ( 
         GetHARDNAME  (iHardEntries) == HARDNAME 
       ){
         ToBeReturned = iHardEntries ;
         break ;
       }
  }
  return ToBeReturned;
}
void         AmdcsimrecStand::TestHardSoftStuff  ()  
{
  int iHardEntries_Max = GetHARDENTRIES();

  std::ofstream  AmdcHardSoftd1;
  AmdcHardSoftd1.open("AmdcHardSoftd1");
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
    AmdcHardSoftd1 
                  << std::setw(7)  << GetHARDNAME(iHardEntries)
                  << "              "
                  << std::setw(3)  << GetSOFTNAME(iHardEntries)
                  << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                  << std::setw(20) << GetSOFTIZ(iHardEntries)
                  << std::endl;
  }
  AmdcHardSoftd1.close();

  int IOK = 1;  

  std::ofstream  AmdcHardSoftd2;
  AmdcHardSoftd2.open("AmdcHardSoftd2");
  IOK = 1;  
  for (int iHardEntries1=1 ; iHardEntries1<=iHardEntries_Max ; iHardEntries1++){
    for (int iHardEntries2=iHardEntries1+1 ; iHardEntries2<=iHardEntries_Max ; iHardEntries2++){

      if ( GetHARDNAME(iHardEntries1) == GetHARDNAME(iHardEntries2) ){
        IOK = 0 ;
        AmdcHardSoftd2 
                      << "*Pb: Same HARDNAME in the 2 endries :" 
                      << std::endl;
        AmdcHardSoftd2 
                      << std::setw(7)  << GetHARDNAME(iHardEntries1)
                      << "              "
                      << std::setw(3)  << GetSOFTNAME(iHardEntries1)
                      << std::setw(20) << GetSOFTOCTANT(iHardEntries1)
                      << std::setw(20) << GetSOFTIZ(iHardEntries1)
                      << std::endl;
        AmdcHardSoftd2 
                      << std::setw(7)  << GetHARDNAME(iHardEntries2)
                      << "              "
                      << std::setw(3)  << GetSOFTNAME(iHardEntries2)
                      << std::setw(20) << GetSOFTOCTANT(iHardEntries2)
                      << std::setw(20) << GetSOFTIZ(iHardEntries2)
                      << std::endl;
      }
    }
  }
  if (IOK == 1 ){
    AmdcHardSoftd2 
                  << " Good: All entries differ by their HARDNAME  " 
                  << std::endl;
  }


  IOK = 1;  
  for (int iHardEntries1=1 ; iHardEntries1<=iHardEntries_Max ; iHardEntries1++){
    for (int iHardEntries2=iHardEntries1+1 ; iHardEntries2<=iHardEntries_Max ; iHardEntries2++){

      if ( 
           GetSOFTNAME  (iHardEntries1) == GetSOFTNAME  (iHardEntries2) 
        && GetSOFTOCTANT(iHardEntries1) == GetSOFTOCTANT(iHardEntries2) 
        && GetSOFTIZ    (iHardEntries1) == GetSOFTIZ    (iHardEntries2) 
         ){
        IOK = 0 ;
        AmdcHardSoftd2 
                      << "*Pb: Same SOFTNAME/SOFTOCTANT/SOFTIZ in the 2 endries :" 
                      << std::endl;
        AmdcHardSoftd2 
                      << std::setw(7)  << GetHARDNAME(iHardEntries1)
                      << "              "
                      << std::setw(3)  << GetSOFTNAME(iHardEntries1)
                      << std::setw(20) << GetSOFTOCTANT(iHardEntries1)
                      << std::setw(20) << GetSOFTIZ(iHardEntries1)
                      << std::endl;
        AmdcHardSoftd2 
                      << std::setw(7)  << GetHARDNAME(iHardEntries2)
                      << "              "
                      << std::setw(3)  << GetSOFTNAME(iHardEntries2)
                      << std::setw(20) << GetSOFTOCTANT(iHardEntries2)
                      << std::setw(20) << GetSOFTIZ(iHardEntries2)
                      << std::endl;
      }
    }
  }
  if (IOK == 1 ){
    AmdcHardSoftd2 
                  << " Good: All entries differ by their SOFTNAME/SOFTOCTANT/SOFTIZ  " 
                  << std::endl;
  }


  IOK = 1;  
  for(int jtyp =  1; jtyp <= Mtyp(); jtyp++){
    for(int jff =  1; jff <= 8; jff++){        
      for(int jzz =  - Mzz(); jzz <=  Mzz(); jzz++){
        int jgeo =  IGEO(jtyp ,jff, jzz);
        if (jgeo > 0){
          int HasMDT = 0;
          for (int jobj = 1 ; jobj <= NOBJ(jtyp,jgeo); jobj++){
            int jtec = ITEC(jtyp,jgeo,jobj) ;
            if ( TechnologyName(jtec) != "MDT" ) continue ;
            HasMDT = 1 ;
          }
          if ( HasMDT == 0 ) continue ;
          std::string SOFTNAME   = StationName(jtyp) ;
          int         SOFTOCTANT = jff ;
          int         SOFTIZ     = jzz ;
          if ( GetEntryRank(SOFTNAME,SOFTOCTANT,SOFTIZ) == -1 ){
            IOK = 0 ;
            AmdcHardSoftd2 
                          << "*Pb: the following SOFTNAME/SOFTOCTANT/SOFTIZ has no entry :" 
                          << std::endl;
            AmdcHardSoftd2 
                          << std::setw(7)  << "       "
                          << "              "
                          << std::setw(3)  << SOFTNAME
                          << std::setw(20) << SOFTOCTANT
                          << std::setw(20) << SOFTIZ
                          << std::endl;
          }
        }
      }
    }
  }
  if (IOK == 1 ){
    AmdcHardSoftd2 
                  << " Good: All MDT chbers known by their SOFTNAME/SOFTOCTANT/SOFTIZ have an entry " 
                  << std::endl;
  }
  
  
  IOK = 1;  
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
    int IdoIt = 1 ;
    int jtyp = GetJtyp( GetSOFTNAME(iHardEntries) )   ;
    int jff  = GetSOFTOCTANT(iHardEntries)                          ;
    int jzz  = GetSOFTIZ(iHardEntries)                              ;
    if ( jtyp <= 0 || jtyp > Mtyp() ){
      IOK = 0 ;
      IdoIt = 0 ;
      AmdcHardSoftd2 
                    << "*Pb: the following entry has out of range jtyp," << jtyp
                    << " i.e wrong SOFTNAME " << GetSOFTNAME(iHardEntries)
                    << std::endl;
      AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
    }
    if ( jff < 0 || jff > 8 ){
      IOK = 0 ;
      IdoIt = 0 ;
      AmdcHardSoftd2 
                    << "*Pb: the following entry has out of range jff, " << jff 
                    << " i.e wrong SOFTOCTANT "  << GetSOFTOCTANT(iHardEntries)
                    << std::endl;
      AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
    }
    if ( jzz < - Mzz() || jzz >  Mzz() ){
      IOK = 0 ;
      IdoIt = 0 ;
      AmdcHardSoftd2 
                    << "*Pb: the following entry has out of range jzz, " << jzz 
                    << " i.e wrong SOFTIZ "  << GetSOFTIZ(iHardEntries)
                    << std::endl;
      AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
    }
    if (IdoIt == 1) {
      int jgeo =  IGEO(jtyp ,jff, jzz);
      if ( jgeo <=0 ){
        IOK = 0 ;
        AmdcHardSoftd2 
                      << "*Pb: the following entry has an unknown MDT SOFTNAME/SOFTOCTANT/SOFTIZ :" 
                      << std::endl;
        AmdcHardSoftd2 
                      << std::setw(7)  << GetHARDNAME(iHardEntries)
                      << "              "
                      << std::setw(3)  << GetSOFTNAME(iHardEntries)
                      << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                      << std::setw(20) << GetSOFTIZ(iHardEntries)
                      << std::endl;
      }
      if ( jgeo > 0 ){
        int HasMDT = 0;
        for (int jobj = 1 ; jobj <= NOBJ(jtyp,jgeo); jobj++){
          int jtec = ITEC(jtyp,jgeo,jobj) ;
          if ( TechnologyName(jtec) != "MDT" ) continue ;
          HasMDT = 1 ;
        }
        if ( HasMDT == 0 ){
          IOK = 0 ;
          AmdcHardSoftd2 
                        << "*Pb: the following entry has an unknown MDT SOFTNAME/SOFTOCTANT/SOFTIZ :" 
                        << std::endl;
          AmdcHardSoftd2 
                        << std::setw(7)  << GetHARDNAME(iHardEntries)
                        << "              "
                        << std::setw(3)  << GetSOFTNAME(iHardEntries)
                        << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                        << std::setw(20) << GetSOFTIZ(iHardEntries)
                        << std::endl;
        }
      }
    }
  }
  if (IOK == 1 ){
    AmdcHardSoftd2 
                  << " Good: All entries do correspong to MDT chbers known by their SOFTNAME/SOFTOCTANT/SOFTIZ " 
                  << std::endl;
  }
  
  
  IOK = 1;  
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
    if ( GetHARDNAME(iHardEntries).size() != 7){
      IOK = 0;
      AmdcHardSoftd2 
                  << "*Pb: the following entry has a too short (should be 7), " << GetHARDNAME(iHardEntries) 
                  << std::endl;
      AmdcHardSoftd2 
                  << std::setw(7)  << GetHARDNAME(iHardEntries)
                  << "              "
                  << std::setw(3)  << GetSOFTNAME(iHardEntries)
                  << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                  << std::setw(20) << GetSOFTIZ(iHardEntries)
                  << std::endl;
    }
  }
  if (IOK == 1 ){
    AmdcHardSoftd2 
                  << " Good: All entries have hard name of correct (7) length " 
                  << std::endl;
  }
  
  if (IOK == 1 ){
  
    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      if ( 
             (GetHARDNAME(iHardEntries)).substr( 4 , 1 ) != "C" 
          && (GetHARDNAME(iHardEntries)).substr( 4 , 1 ) != "A" 
          && (GetHARDNAME(iHardEntries)).substr( 4 , 1 ) != "B" 
         ){
        IOK = 0;
        AmdcHardSoftd2 
                    << "*Pb: the following entry has a wrong 5th character in (should be A or C), " 
                    << GetHARDNAME(iHardEntries) 
                    << std::endl;
        AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: All entries have a correct 5th character in their hard name " 
                    << std::endl;
    }

    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      int ierr = 0 ;
      int iHardIz = GetHARDIZ  ( GetHARDNAME(iHardEntries), ierr)  ;
      if ( ierr == 1 && iHardIz != -99) {
        AmdcHardSoftd2 
                    << "*Pb: STRANGE VALUES FOR " 
                    << " ierr=" <<ierr
                    << " and iHardIz=" <<iHardIz
                    << std::endl;
      }
      if ( ierr == 1 ) {
        IOK = 0;
        AmdcHardSoftd2 
                    << "*Pb: the following entry has wrong 4th  character in (should be 1 to 9), " 
                    << (GetHARDNAME(iHardEntries)).substr( 3 , 1 )
                    << std::endl;
        AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: All entries have correct 4th  character for their hard name " 
                    << std::endl;
    }
    
    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      int ierr = 0 ;
      int iHardSector = GetHARDSector  ( GetHARDNAME(iHardEntries), ierr)  ;
      if ( ierr == 1 && iHardSector != -99) {
        AmdcHardSoftd2 
                    << "*Pb: STRANGE VALUES FOR " 
                    << " ierr=" <<ierr
                    << " and iHardSector=" <<iHardSector
                    << std::endl;
      }
      if ( ierr == 1 ) {
        IOK = 0;
        AmdcHardSoftd2 
                    << "*Pb: the following entry has wrong 6th and 7th  characters in (should be 01 to 16), " 
                    << (GetHARDNAME(iHardEntries)).substr( 5 , 2 )
                    << std::endl;
        AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: All entries have correct 6th and 7th  characters for their hard name " 
                    << std::endl;
    }
    
  }
  
  if (IOK == 1 ){
  
    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      if ( GetHARDNAME(iHardEntries).substr( 0 , 3 ) !=  GetSOFTNAME(iHardEntries) ){
        IOK = 0;
        AmdcHardSoftd2 
                    << "*Pb: the following entry has wrong 3 first  characters in (should be the soft name), " 
                    << GetHARDNAME(iHardEntries) 
                    << std::endl;
        AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: For all entries the hard and soft station name are the same " 
                    << std::endl;
    }
    
    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      int ierr = 0 ;
      int iHardOctant = GetHARDOctant  ( GetHARDNAME(iHardEntries), ierr)  ;
      if ( iHardOctant != GetSOFTOCTANT(iHardEntries) ) {
        IOK = 0;
        AmdcHardSoftd2 
                    << "*Pb: for the following entry, the hard sector and soft octant do NOT correspond " 
                    << " " << iHardOctant
                    << " " << GetSOFTOCTANT(iHardEntries)
                    << std::endl;
        AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: For all entries the hard sector and soft octant do correspond" 
                    << std::endl;
    }
    
    IOK = 1;  
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      int ierr = 0 ;
      int iHardSignedIz = GetHARDSignedIz  ( GetHARDNAME(iHardEntries), ierr)  ;
      if ( iHardSignedIz != GetSOFTIZ(iHardEntries) ) {
        IOK = 0;
        std::string TheComment = "<========THIS IS NOT OK " ;
        if (  
                 GetSOFTNAME(iHardEntries) == "BML"  
             &&  GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 13
             &&  abs( iHardSignedIz ) >= 5 
             &&  ( abs(iHardSignedIz) - abs(GetSOFTIZ(iHardEntries)) ) == 1 
           ) {
          IOK = 1;
          TheComment = "<========BUT THIS IS OK" ;
        }
        if (  
                 GetSOFTNAME(iHardEntries) == "BOG"  
             &&  ( 
                     GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 12
                  || GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 14 
                 )
             &&  ( abs(iHardSignedIz) - 2*abs(GetSOFTIZ(iHardEntries)) ) == 0 
           ) {
          IOK = 1;
          TheComment = "<========BUT THIS IS OK" ;
        }
        if (  
                 GetSOFTNAME(iHardEntries) == "BOF"  
             &&  (   
                     GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 12
                  || GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 14 
                 )
             &&  ( abs(iHardSignedIz) + 1 - 2*abs(GetSOFTIZ(iHardEntries)) ) == 0
           ) {
          IOK = 1;
          TheComment = "<========BUT THIS IS OK" ;
        }

        if (  
                 GetSOFTNAME(iHardEntries) == "EIL"  
             &&  (   
                     GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 1
                  || GetHARDSector  ( GetHARDNAME(iHardEntries), ierr) == 9 
                 )
             &&  ( 
                     ( abs(iHardSignedIz) == 4 && abs(GetSOFTIZ(iHardEntries)) == 5 )
                  || ( abs(iHardSignedIz) == 5 && abs(GetSOFTIZ(iHardEntries)) == 4 )
                 )
           ) {
          IOK = 1;
          TheComment = "<========BUT THIS IS OK" ;
        }
        if (  
                 GetSOFTNAME(iHardEntries) == "EEL"  
             &&  (   
                      GetHARDNAME(iHardEntries) == "EEL2A05"
                   || GetHARDNAME(iHardEntries) == "EEL2C05"
                 )
           ) {
          IOK = 1;
          TheComment = "<========BUT THIS IS OK" ;
        }
        if ( IOK == 1) {
          AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << " ==> IZ Mismatch "
                    << " " << iHardSignedIz
                    << " " << GetSOFTIZ(iHardEntries)
                    << " " << TheComment
                    << std::endl;
        }
        if ( IOK == 0) {
          AmdcHardSoftd2 
                    << "*Pb: for the following entry, the hard IZ and soft IZ do NOT correspond " 
                    << " " << iHardSignedIz
                    << " " << GetSOFTIZ(iHardEntries)
                    << " " << TheComment
                    << std::endl;
          AmdcHardSoftd2 
                    << std::setw(7)  << GetHARDNAME(iHardEntries)
                    << "              "
                    << std::setw(3)  << GetSOFTNAME(iHardEntries)
                    << std::setw(20) << GetSOFTOCTANT(iHardEntries)
                    << std::setw(20) << GetSOFTIZ(iHardEntries)
                    << std::endl;
        }
      }
    }
    if (IOK == 1 ){
      AmdcHardSoftd2 
                    << " Good: For all entries the hard IZ and soft IZ do correspond" 
                    << std::endl;
    }
  }
  
  AmdcHardSoftd2.close();
  
  
}

int          AmdcsimrecStand::GetHARDSector  (std::string HARDNAME,int& ierr)                   
{
 ierr = 0 ;
 if ( HARDNAME.substr( 5 , 2 ) == "01" ) return  1 ;
 if ( HARDNAME.substr( 5 , 2 ) == "02" ) return  2 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "03" ) return  3 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "04" ) return  4 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "05" ) return  5 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "06" ) return  6 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "07" ) return  7 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "08" ) return  8 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "09" ) return  9 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "10" ) return 10 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "11" ) return 11 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "12" ) return 12 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "13" ) return 13 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "14" ) return 14 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "15" ) return 15 ; 
 if ( HARDNAME.substr( 5 , 2 ) == "16" ) return 16 ;
 ierr = 1 ;
 return  -99 ;
}
int          AmdcsimrecStand::GetHARDOctant  (std::string HARDNAME,int& ierr)                   
{
 int iHardSector = GetHARDSector  ( HARDNAME, ierr) ;  
 int iHardOctant = ( (int)  ( ( ( (float) iHardSector ) )/2. ) ) ;
 if ( ( iHardSector - 2*iHardOctant ) > 0 ) iHardOctant = iHardOctant + 1 ;
 return iHardOctant;
}
int          AmdcsimrecStand::GetHARDIZ  (std::string HARDNAME,int& ierr)                  
{
 ierr = 0 ;
 if ( HARDNAME.substr( 3 , 1 ) == "0" ) return 0 ;
 if ( HARDNAME.substr( 3 , 1 ) == "1" ) return 1 ;
 if ( HARDNAME.substr( 3 , 1 ) == "2" ) return 2 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "3" ) return 3 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "4" ) return 4 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "5" ) return 5 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "6" ) return 6 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "7" ) return 7 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "8" ) return 8 ; 
 if ( HARDNAME.substr( 3 , 1 ) == "9" ) return 9 ; 
 ierr = 1 ;
 return -99 ;
}
int          AmdcsimrecStand::GetHARDSignedIz  (std::string HARDNAME,int& ierr)                    
{
 ierr = 0 ;
 int iHardSignedIz = GetHARDIZ ( HARDNAME, ierr) ;
 if ( ierr != 1 ) {
   if ( HARDNAME.substr( 4 , 1 ) == "C" ) return ( -iHardSignedIz ) ;
   if ( HARDNAME.substr( 4 , 1 ) == "A" ) return (  iHardSignedIz ) ;
   if ( HARDNAME.substr( 4 , 1 ) == "B" ) return (              0 ) ;
 }
 ierr = 1 ;
 return -99 ;
}

// amdcxtomo
int          AmdcsimrecStand::GetXTOMOENTRIES()           {std::string Name="XTOMOENTRIES" ;int I1=0     ;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
std::string  AmdcsimrecStand::GetXtomoChberName   (int iEntry) 
{
  int CHARL1 = 7 ;
  std::string TheXtomoChberName = "XXXXXXX";
  if (iEntry<=GetXTOMOENTRIES()) {
   TheXtomoChberName.resize(CHARL1) ;
   std::string Name = "XtomoChberName";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheXtomoChberNameIS = GetAmdcCharVar(Name,iEntry,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheXtomoChberName[i]=TheXtomoChberNameIS[i];}
  }
  return TheXtomoChberName;
}
std::string  AmdcsimrecStand::GetXtomoSite   (int iEntry) 
{
  int CHARL1 = 3 ;
  std::string TheXtomoSite = "XXX";
  if (iEntry<=GetXTOMOENTRIES()) {
   TheXtomoSite.resize(CHARL1) ;
   std::string Name = "XtomoSite";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheXtomoSiteIS = GetAmdcCharVar(Name,iEntry,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheXtomoSite[i]=TheXtomoSiteIS[i];}
  }
  return TheXtomoSite;
}
int          AmdcsimrecStand::GetXtomoSiteId (int iEntry) {std::string Name="XtomoSiteId" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetXtomoTime   (int iEntry) {std::string Name="XtomoTime"   ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetXtomoPassed (int iEntry) {std::string Name="XtomoPassed" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
std::string  AmdcsimrecStand::GetXtomoSide   (int iEntry) 
{
  int CHARL1 = 1 ;
  std::string TheXtomoSide = "X";
  if (iEntry<=GetXTOMOENTRIES()) {
   TheXtomoSide.resize(CHARL1) ;
   std::string Name = "XtomoSide";
   int Ibid1 = 0 ;
   int Ibid2 = 0 ;
   std::string TheXtomoSideIS = GetAmdcCharVar(Name,iEntry,Ibid1,Ibid2);
   for (int i= 0; i <CHARL1; i++){TheXtomoSide[i]=TheXtomoSideIS[i];}
  }
  return TheXtomoSide;
}
int          AmdcsimrecStand::GetXtomoNberTube1 (int iEntry) {std::string Name="XtomoNberTube1" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetXtomoNberTube2 (int iEntry) {std::string Name="XtomoNberTube2" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetXtomoNberML    (int iEntry) {std::string Name="XtomoNberML"    ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}
int          AmdcsimrecStand::GetXtomoNberLayer (int iEntry) {std::string Name="XtomoNberLayer" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcIntVar(Name,I1,I2,I3);}

double       AmdcsimrecStand::GetXtomoML1NYtub (int iEntry) {std::string Name="XtomoML1NYtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NZtub (int iEntry) {std::string Name="XtomoML1NZtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NDely (int iEntry) {std::string Name="XtomoML1NDely" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NDelz (int iEntry) {std::string Name="XtomoML1NDelz" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NDela (int iEntry) {std::string Name="XtomoML1NDela" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NYpit (int iEntry) {std::string Name="XtomoML1NYpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1NZpit (int iEntry) {std::string Name="XtomoML1NZpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PYtub (int iEntry) {std::string Name="XtomoML1PYtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PZtub (int iEntry) {std::string Name="XtomoML1PZtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PDely (int iEntry) {std::string Name="XtomoML1PDely" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PDelz (int iEntry) {std::string Name="XtomoML1PDelz" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PDela (int iEntry) {std::string Name="XtomoML1PDela" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PYpit (int iEntry) {std::string Name="XtomoML1PYpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML1PZpit (int iEntry) {std::string Name="XtomoML1PZpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NYtub (int iEntry) {std::string Name="XtomoML2NYtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NZtub (int iEntry) {std::string Name="XtomoML2NZtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NDely (int iEntry) {std::string Name="XtomoML2NDely" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NDelz (int iEntry) {std::string Name="XtomoML2NDelz" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NDela (int iEntry) {std::string Name="XtomoML2NDela" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NYpit (int iEntry) {std::string Name="XtomoML2NYpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2NZpit (int iEntry) {std::string Name="XtomoML2NZpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PYtub (int iEntry) {std::string Name="XtomoML2PYtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PZtub (int iEntry) {std::string Name="XtomoML2PZtub" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PDely (int iEntry) {std::string Name="XtomoML2PDely" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PDelz (int iEntry) {std::string Name="XtomoML2PDelz" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PDela (int iEntry) {std::string Name="XtomoML2PDela" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PYpit (int iEntry) {std::string Name="XtomoML2PYpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}
double       AmdcsimrecStand::GetXtomoML2PZpit (int iEntry) {std::string Name="XtomoML2PZpit" ;int I1=iEntry;int I2=0; int I3=0; return GetAmdcRealVar(Name,I1,I2,I3);}

int          AmdcsimrecStand::GetXtomoEntryRank(std::string SOFTNAME, int SOFTOCTANT, int SOFTIZ) 
{
  std::string Name="IentryXtomo" ;int I1= GetJtyp(SOFTNAME);int I2=SOFTOCTANT; int I3=SOFTIZ; return GetAmdcIntVar(Name,I1,I2,I3);
}
int          AmdcsimrecStand::GetXtomoEntryRank  (std::string HARDNAME)  
{
  int iHardEntries_Max = GetXTOMOENTRIES ();
  int ToBeReturned = -1 ;
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
    if ( 
         GetXtomoChberName(iHardEntries) == HARDNAME 
       ){
         ToBeReturned = iHardEntries ;
         break ;
       }
  }
  return ToBeReturned;
}
void         AmdcsimrecStand::TestXtomoStuff  ()  
{

  int iXtomoEntries_Max = GetXTOMOENTRIES();

  int IOK = 1;  

  std::ofstream  OutTestXtomoStuff;
  OutTestXtomoStuff.open("Out.AmdcDump.TestXtomoStuff");
  
//Check if the hardware name of a given station appears only once in the Xtomo table
  IOK = 1;  
  for (int iXtomoEntries1=1 ; iXtomoEntries1<=iXtomoEntries_Max ; iXtomoEntries1++){
    for (int iXtomoEntries2=iXtomoEntries1+1 ; iXtomoEntries2<=iXtomoEntries_Max ; iXtomoEntries2++){

      if ( GetXtomoChberName(iXtomoEntries1) == GetXtomoChberName(iXtomoEntries2) ){
        IOK = 0 ;
        OutTestXtomoStuff 
                      << "*Pb: Same XtomoChberName in the 2 endries :" 
                      << std::endl;
        OutTestXtomoStuff 
                      << std::setw(7)  << GetXtomoChberName(iXtomoEntries1)
                      << std::endl;
      }
    }
  }
  if (IOK == 1 ){
    OutTestXtomoStuff 
                  << " Good: All entries differ by their XtomoChberName  " 
                  << std::endl;
  }

  
//Check if the hardware name of a given station appearing in the Xtomo table appears also in the hard-soft name table
  IOK = 1;  
  int iHardEntries_Max = GetHARDENTRIES();
  for (int iXtomoEntries=1 ; iXtomoEntries<=iXtomoEntries_Max ; iXtomoEntries++){
    int subIOK = 0 ;
    for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){
      if ( subIOK == 0 ) {
       if ( GetXtomoChberName(iXtomoEntries) == GetHARDNAME(iHardEntries) ) subIOK = 1 ;
      }
    }
    if ( subIOK == 0 ){
        OutTestXtomoStuff 
                      << "*Pb: Xtom entry " << GetXtomoChberName(iXtomoEntries) 
                      << " has not corresponding entry in hardsoft section " 
                      << std::endl;
    }
  }
  if (IOK == 1 ){
    OutTestXtomoStuff 
                  << " Good: All entries have a corresponding entry in hardsoft section  " 
                  << std::endl;
  }


//Check if all the stations positionned by the current amdc file have a corresponding entry in the  Xtom table
  IOK = 1;  
  for(int jtyp =  1; jtyp <= Mtyp(); jtyp++){
    for(int jff =  1; jff <= 8; jff++){        
      for(int jzz =  - Mzz(); jzz <=  Mzz(); jzz++){
        int jgeo =  IGEO(jtyp ,jff, jzz);
        if (jgeo > 0){
          int HasMDT = 0;
          for (int jobj = 1 ; jobj <= NOBJ(jtyp,jgeo); jobj++){
            int jtec = ITEC(jtyp,jgeo,jobj) ;
            if ( TechnologyName(jtec) != "MDT" ) continue ;
            HasMDT = 1 ;
          }
          if ( HasMDT == 0 ) continue ;
          std::string TheStationName   = StationName(jtyp) ;
          if ( GetXtomoEntryRank(TheStationName,jff,jzz) == -1 ){
            IOK = 0 ;
            OutTestXtomoStuff 
                          << "*Pb: the following Station/jff/jzz has no Xtomo entry : " 
                          << std::setw(3) << TheStationName
                          << std::setw(5) << jff
                          << std::setw(5) << jzz
                          ;
            if (  GetEntryRank(TheStationName,jff,jzz) == -1 ){           
             OutTestXtomoStuff 
                           << " (No HARDNAME in hardsoft section too)      "
                           << std::endl;
            }else{
             OutTestXtomoStuff 
                           << " ( HARDNAME "
                           << GetHARDNAME(GetEntryRank(TheStationName,jff,jzz))
                           << " )      "
                           << std::endl;
            }
          }
        }
      }
    }
  }
  if (IOK == 1 ){
    OutTestXtomoStuff 
                  << " Good: All MDT chbers known by their Station/jff/jzz have an entry " 
                  << std::endl;
  }


//Check if all the stations positionned by the current amdc file have correct nber of multilayers, of mdt layer and of tube in Xtomo stuff
  IOK = 1;  
  for(int jtyp =  1; jtyp <= Mtyp(); jtyp++){
    for(int jff =  1; jff <= 8; jff++){        
      for(int jzz =  - Mzz(); jzz <=  Mzz(); jzz++){
      
        std::string TheStationName   = StationName(jtyp) ;

        int NberOfMDTMultiLayers_Amdc  = NberOfMDTMultiLayers  (TheStationName,jff,jzz) ;
        
        if ( NberOfMDTMultiLayers_Amdc <= 0 ) continue ;
        
        int XtomoEntryRank = GetXtomoEntryRank(TheStationName,jff,jzz) ;

        if ( XtomoEntryRank <= 0 ) continue ;
        
        if ( NberOfMDTMultiLayers_Amdc != GetXtomoNberML(XtomoEntryRank) ){
          IOK = 0 ;
          OutTestXtomoStuff 
                      << "*Pb: the following Station/jff/jzz " 
                      << std::setw(3)  << TheStationName
                      << std::setw(3)  << jff
                      << std::setw(3)  << jzz
                      << ", i.e " <<std::setw(10) << GetXtomoChberName(XtomoEntryRank) << ", "
                      << " has different Nber of Multilayers in "
                      << " Amdc  " << NberOfMDTMultiLayers_Amdc
                      << " and Xtomo table  " << GetXtomoNberML(XtomoEntryRank)
                      << std::endl ;            
        }else{
          for (int MulLay = 1 ; MulLay <= NberOfMDTMultiLayers_Amdc ; MulLay++){
            int NberOfMDTLayers_Amdc = NberOfMDTLayers       (TheStationName,jff,jzz,MulLay) ;
            if ( NberOfMDTLayers_Amdc != GetXtomoNberLayer(XtomoEntryRank) ){
              IOK = 0 ;
              OutTestXtomoStuff 
                      << "*Pb: the following Station/jff/jzz/MulLay " 
                      << std::setw(3)  << TheStationName
                      << std::setw(3)  << jff
                      << std::setw(3)  << jzz
                      << ", i.e " <<std::setw(10) << GetXtomoChberName(XtomoEntryRank) << ", "
                      << std::setw(3)  << MulLay
                      << " has different Nber of layer in "
                      << " Amdc  " << NberOfMDTLayers_Amdc
                      << " and Xtomo table  " << GetXtomoNberLayer(XtomoEntryRank)
                      << std::endl ;            
            }else{
              for (int jlay = 1 ; jlay <= NberOfMDTLayers_Amdc ; jlay++){
                int NberOfMDTTubes_Amdc = NberOfMDTTubes       (TheStationName,jff,jzz,MulLay,jlay) ;
                int NberOfMDTTubes_Xtomo1 = GetXtomoNberTube1(XtomoEntryRank);
                int NberOfMDTTubes_Xtomo2 = GetXtomoNberTube2(XtomoEntryRank);
                int NberOfMDTTubes_Diff = NberOfMDTTubes_Amdc - NberOfMDTTubes_Xtomo1 ;
                if ( MulLay == 2) NberOfMDTTubes_Diff = NberOfMDTTubes_Amdc - NberOfMDTTubes_Xtomo2 ;
                if ( NberOfMDTLayers_Amdc != GetXtomoNberLayer(XtomoEntryRank) ){
                  IOK = 0 ;
                  OutTestXtomoStuff 
                      << "*Pb: the following Station/jff/jzz/MulLay/jlay " 
                      << std::setw(3)  << TheStationName
                      << std::setw(3)  << jff
                      << std::setw(3)  << jzz
                      << ", i.e " <<std::setw(10) << GetXtomoChberName(XtomoEntryRank) << ", "
                      << std::setw(3)  << MulLay
                      << std::setw(3)  << jlay
                      << " has different Nber of tubes "
                      << " Amdc  " << NberOfMDTTubes_Amdc
                      << " and Xtomo table  " << ( NberOfMDTTubes_Amdc - NberOfMDTTubes_Diff )
                      << std::endl ;            
                }
              }
            }
          }
        }
      }
    }
  }
  if (IOK == 1 ){
    OutTestXtomoStuff 
                  << " Good: All MDT chbers known have " 
                  << " same nber of multilayers "
                  << " same nber of mdt layer "
                  << " same nber of tube "
                  << "in Amdc and Xtomo stuff " 
                  << std::endl;
  }



  std::ofstream  OutTestXtomoStuff2;
  OutTestXtomoStuff2.open("Out.AmdcDump.TestXtomoStuff2");
  OutTestXtomoStuff2<< setiosflags(std::ios::fixed); 
  
  std::ofstream  OutTestXtomoStuff3;
  OutTestXtomoStuff3.open("Out.AmdcDump.TestXtomoStuff3");
  OutTestXtomoStuff3<< setiosflags(std::ios::fixed); 
 
//Dump tubes position
//Loop on Station types
  for(int jtyp =  1; jtyp <= Mtyp(); jtyp++){
//  Loop on Eta
    for(int jff =  1; jff <= 8; jff++){        
//    Loop on Phi
      for(int jzz =  - Mzz(); jzz <=  Mzz(); jzz++){
      
        std::string TheStationName   = StationName(jtyp) ;

        int NberOfMDTMultiLayers_Amdc  = NberOfMDTMultiLayers  (TheStationName,jff,jzz) ;
        
        if ( NberOfMDTMultiLayers_Amdc <= 0 ) continue ;
        
        int XtomoEntryRank = GetXtomoEntryRank(TheStationName,jff,jzz) ;

        if ( XtomoEntryRank <= 0 ) continue ;
        
        if ( NberOfMDTMultiLayers_Amdc != GetXtomoNberML(XtomoEntryRank) ) continue ;
        
//      Loop on Multilayers
        for (int MulLay = 1 ; MulLay <= NberOfMDTMultiLayers_Amdc ; MulLay++){
        
          int NberOfMDTLayers_Amdc = NberOfMDTLayers       (TheStationName,jff,jzz,MulLay) ;
          if ( NberOfMDTLayers_Amdc != GetXtomoNberLayer(XtomoEntryRank) ) continue;
          
//        Loop on "side"
          for (int iside = 1 ; iside <= 2 ; iside++){
          
//          Loop on layers
            for (int jlayFB = 1 ; jlayFB <= NberOfMDTLayers_Amdc ; jlayFB++){
            
//            This is how Florian labels layers
              int jlay = jlayFB ;
              if (MulLay == 1) jlay = NberOfMDTLayers_Amdc - jlayFB + 1 ;

              int NberOfMDTTubes_Amdc = NberOfMDTTubes       (TheStationName,jff,jzz,MulLay,jlay) ;
//notused     int NberOfMDTTubes_Xtomo1 = GetXtomoNberTube1(XtomoEntryRank);
//notused     int NberOfMDTTubes_Xtomo2 = GetXtomoNberTube2(XtomoEntryRank);
//notused     int NberOfMDTTubes_Diff = NberOfMDTTubes_Amdc - NberOfMDTTubes_Xtomo1 ;
//notused     if ( MulLay == 2) NberOfMDTTubes_Diff = NberOfMDTTubes_Amdc - NberOfMDTTubes_Xtomo2 ;
              if ( NberOfMDTLayers_Amdc != GetXtomoNberLayer(XtomoEntryRank) ) continue ;

//            Loop on tubes
              for (int Tube = 1 ; Tube <= NberOfMDTTubes_Amdc ; Tube++){
                              
//              get tube ends
                double XYZ1[3] ;
                double XYZ2[3] ;
                double SZT1[3] ;
                double SZT2[3] ;
                double Radius = 0. ;
                GetTubePositionFromMDTindex( TheStationName, jzz, jff, MulLay, jlay, Tube,
                                            XYZ1, XYZ2, SZT1, SZT2, Radius 
                                           );

//              Recompute positions including xtomo data
//              This is cheap geometry; will work only without A lines; will change this later
                double Phi0 = PosPhi(jtyp,jff,jzz);
                double SZT_Z = 0. ;
                double SZT_S = 0. ;
                double SZT_T = 0. ;
                double szt_s = 0. ;
                double szt_z = 0. ;
                double szt_t = 0. ;
                double XYZ_X = 0. ;
                double XYZ_Y = 0. ;
                double XYZ_Z = 0. ;

//              Work out "side ends"
                XYZ_X = XYZ1[0] ;
                XYZ_Y = XYZ1[1] ;
                XYZ_Z = XYZ1[2] ;
                if  (iside == 2) {
                  XYZ_X = XYZ2[0] ;
                  XYZ_Y = XYZ2[1] ;
                  XYZ_Z = XYZ2[2] ;
                }
		if ( TheStationName.substr( 0 , 1 ) == "E" && XYZ_Z>0){
		  XYZ_X = XYZ2[0] ;
                  XYZ_Y = XYZ2[1] ;
                  XYZ_Z = XYZ2[2] ;
		  if (iside == 2){
		    XYZ_X = XYZ1[0] ;
		    XYZ_Y = XYZ1[1] ;
		    XYZ_Z = XYZ1[2] ;
		  }
		}

//              Go to SZT system
                SZT_Z = XYZ_Z ;
                SZT_S = -sin(Phi0) * XYZ_X + cos(Phi0) * XYZ_Y ;
                SZT_T =  cos(Phi0) * XYZ_X + sin(Phi0) * XYZ_Y ;

//              Go to szt system
                szt_s = SZT_S ;
                szt_z = SZT_Z ;
                szt_t = SZT_T ;
                if ( TheStationName.substr( 0 , 1 ) == "E" ){
                  szt_z = SZT_T ;
                  szt_s =-SZT_S ;
                  szt_t = SZT_Z ;
		  if (jzz < 0 ){
                    szt_s = SZT_S ;
                    szt_t =-SZT_Z ;
                  }
		  
                }

//              Apply Tomo modifs
	       	int optionflag=0;
		double Delta_szt_z_p = 0. ;
		double Delta_szt_t_p = 0. ;
		double Delta_szt_z_n = 0. ;
		double Delta_szt_t_n = 0. ;
		tomotubedisplacement_(
		                    optionflag,
		                    TheStationName, jff, jzz, MulLay, jlay, Tube,
		                    Delta_szt_z_n , Delta_szt_t_n ,
		                    Delta_szt_z_p , Delta_szt_t_p 
		                   );
//               
		double szt_s_c = szt_s ;
		double szt_z_c = szt_z ;
		double szt_t_c = szt_t ;
		double Delta_szt_s_c = 0. ;
		double Delta_szt_z_c = 0. ;
		double Delta_szt_t_c = 0. ;
                int Iside_In = 2*iside - 3 ; 
                tomotubedisplacementcontinuous_(
                                    Iside_In ,
		                    optionflag,
		                    TheStationName, jff, jzz, MulLay,
                                    szt_s_c      , szt_z_c      , szt_t_c        , 
                                    Delta_szt_s_c, Delta_szt_z_c, Delta_szt_t_c
		                   );
//               
		double Delta_szt_z = Delta_szt_z_n ;
		double Delta_szt_t = Delta_szt_t_n ;
		double Delta_szt_s = 0.            ;
                if (iside == 2) {
		  Delta_szt_z = Delta_szt_z_p ;
		  Delta_szt_t = Delta_szt_t_p ;
                }
                szt_s += Delta_szt_s ;
                szt_z += Delta_szt_z ;
                szt_t += Delta_szt_t ;
//               
                szt_s_c += Delta_szt_s_c ;
                szt_z_c += Delta_szt_z_c ;
                szt_t_c += Delta_szt_t_c ;
	
//              Go back to SZT system
                SZT_S = szt_s ;
                SZT_Z = szt_z ;
                SZT_T = szt_t ;
                if ( TheStationName.substr( 0 , 1 ) == "E" ){
                  SZT_T = szt_z ;
                  SZT_S =-szt_s ;
                  SZT_Z = szt_t ;
		  if (jzz < 0 ){
                    SZT_S = szt_s ;
                    SZT_Z =-szt_t ;
		  }
                }

//              Go back to XYZ system
                XYZ_Z = SZT_Z ;
                XYZ_X = -sin(Phi0) * SZT_S + cos(Phi0) * SZT_T ;
                XYZ_Y =  cos(Phi0) * SZT_S + sin(Phi0) * SZT_T ;

//              Dump
                OutTestXtomoStuff2
                        <<  GetXtomoChberName(XtomoEntryRank) 
                        <<"_ENDPLUG_ML" 
                        << std::setw(1) << MulLay 
                        << "_L"  
                        << std::setw(1) << jlayFB 
                        << "_"  ;
                if ( Tube < 10 )  { OutTestXtomoStuff2 << "0" << std::setw(1) << Tube ;
                }else{              OutTestXtomoStuff2 << std::setw(2)        << Tube ; }
                if ( iside == 1 ) { OutTestXtomoStuff2 << "_N" ;
                }else{              OutTestXtomoStuff2 << "_P" ; }
                OutTestXtomoStuff2 
                        << std::setw(12)<<std::setprecision(3)<<  XYZ_X  
                        << std::setw(12)<<std::setprecision(3)<<  XYZ_Y  
                        << std::setw(12)<<std::setprecision(3)<<  XYZ_Z  
                        << std::endl ;  
//               
                OutTestXtomoStuff3
                        <<  GetXtomoChberName(XtomoEntryRank) 
                        <<"_ENDPLUG_ML" 
                        << std::setw(1) << MulLay 
                        << "_L"  
                        << std::setw(1) << jlayFB 
                        << "_"  ;
                if ( Tube < 10 )  { OutTestXtomoStuff3 << "0" << std::setw(1) << Tube ;
                }else{              OutTestXtomoStuff3 << std::setw(2)        << Tube ; }
                if ( iside == 1 ) { OutTestXtomoStuff3 << "_N" ;
                }else{              OutTestXtomoStuff3 << "_P" ; }
                OutTestXtomoStuff3 
                        << std::setw(12)<<std::setprecision(3)<<  szt_s - szt_s_c
                        << std::setw(12)<<std::setprecision(3)<<  szt_z - szt_z_c
                        << std::setw(12)<<std::setprecision(3)<<  szt_t - szt_t_c
                        << std::endl ;  

              }
            }
          }
        }
      }
    }
  }

}
int     AmdcsimrecStand::NberOfMDTMultiLayers  (std::string& StationName,int jphi,int jzz                    )
{
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     int ToBeReturned = -1 ;
     getnberofmdtmultilayers_( STANAME,jphi,jzz, ToBeReturned );
     return ToBeReturned;
}
int     AmdcsimrecStand::NberOfMDTLayers       (std::string& StationName,int jphi,int jzz,int jMuLay         )
{
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     int ToBeReturned = -1 ;
     getnberofmdtlayers_( STANAME,jphi,jzz, jMuLay, ToBeReturned );
     return ToBeReturned;
}
int     AmdcsimrecStand::NberOfMDTTubes        (std::string& StationName,int jphi,int jzz,int jMuLay,int jLay)
{
     char STANAME[3];
     STANAME[0] = StationName[0];
     STANAME[1] = StationName[1];
     STANAME[2] = StationName[2];
     int ToBeReturned = -1 ;
     getnberofmdttubes_( STANAME,jphi,jzz, jMuLay, jLay, ToBeReturned );
     return ToBeReturned;
}
