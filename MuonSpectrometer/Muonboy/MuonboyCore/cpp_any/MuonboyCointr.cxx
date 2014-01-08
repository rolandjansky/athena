/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyCointr.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"


MuonboyCointr::MuonboyCointr(){}
 
MuonboyCointr::~MuonboyCointr(){}
 
void MuonboyCointr::Print(std::ostream* out){
//Loop on all Segments of Inner Muon Spectrometer Stations
  *out << "  MuonboyCointr::Print  " << std::endl;
  *out << "* There are "
       << std::setw(5)<< NBINTR()
       << " inner station track segments defined "
       << std::endl;
  int NberOfSegments =  NBINTR();
  for (int SegmentNber=0; SegmentNber<NberOfSegments; SegmentNber++){
    *out << "* **> Segment track nb.         :"
         << std::setw(5) << SegmentNber
         << std::endl;
    *out << "*     In station                :  "
         << StationName(SegmentNber)
         << std::endl;
    *out << "*     Quality factor            :"
         << std::setw(12)<<std::setprecision(5)<<CHINTR(SegmentNber)
         << std::endl;
    *out << "*     Fitted T0                 :"
         << std::setw(12)<<std::setprecision(5)<<T0INTR(SegmentNber)
         << std::endl;
    *out << "*     Nb. of multilayers        :"
         << std::setw(5) <<MlNber(SegmentNber)
         << std::endl;
    *out << "*     Coordinates of the Point  :"
         << std::setw(12)<<std::setprecision(5)<<PTINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<PTINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<PTINTR(SegmentNber,2)
         << std::endl;
    *out << "*     Coordinates of Global Vector :"
         << std::setw(12)<<std::setprecision(5)<<VEINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<VEINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<VEINTR(SegmentNber,2)
         << std::endl;
    *out << "*     Coordinates of dx vector  :"
         << std::setw(12)<<std::setprecision(5)<<DXINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<DXINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<DXINTR(SegmentNber,2)
         << std::endl;
    *out << "*     Coordinates of dy vector  :"
         << std::setw(12)<<std::setprecision(5)<<DYINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<DYINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<DYINTR(SegmentNber,2)
         << std::endl;
    *out << "*     Coordinates of dz vector  :"
         << std::setw(12)<<std::setprecision(5)<<DZINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<DZINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<DZINTR(SegmentNber,2)
         << std::endl;
    *out << "*     Errors validity    :"
         << std::setw(12)                      <<IERINTR(SegmentNber)
         << std::endl;
    *out << "*     Errors components  :"
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,0)
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,1)
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,2)
         << std::endl;
    *out << "*                        :"
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,3)
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,4)
         << std::setw(12)<<std::setprecision(5)<<ERINTR(SegmentNber,5)
         << std::endl;
    *out << "*     Nb. of tube hits used     :"
         << std::setw(5) << NUINTR(SegmentNber)
         << std::endl;
    *out << "*     Tube hits numbers         :";
    int NberOfHits = NUINTR(SegmentNber);
    for (int HitNber = 0; HitNber<NberOfHits; HitNber++){
      *out << std::setw(9)<<IUINTR(SegmentNber,HitNber);
    }
    *out << std::endl;
    *out << "*     Tube hits radii           :";
    for (int HitNber = 0; HitNber<NberOfHits; HitNber++){
      *out << std::setw(9)<<std::setprecision(4)<<Rdigi(SegmentNber,HitNber);
    }
    *out << std::endl;
    *out << "*     Segments hits radii       :";
    for (int HitNber = 0; HitNber<NberOfHits; HitNber++){
      *out << std::setw(9)<<std::setprecision(4)<<Rtrak(SegmentNber,HitNber);
    }
    *out << std::endl;
    *out << "*     Nb. of strip hits used    :"
         << std::setw(5) << NAINTR(SegmentNber)
         << std::endl;
    *out << "*     Strip hits numbers        :";
    int NberOfHits0 = NUINTR(SegmentNber);
    int NberOfHits1 = NUINTR(SegmentNber)+NAINTR(SegmentNber);
    for (int HitNber = NberOfHits0; HitNber<NberOfHits1; HitNber++){
      *out << std::setw(9)<<IUINTR(SegmentNber,HitNber);
    }
    *out << std::endl;
  }
}


int         MuonboyCointr::NBINTR()                                 {std::string Name="NBINTR"   ; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
double      MuonboyCointr::PTINTR(int SegmentNber,int ComponentNber){std::string Name="PTINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
double      MuonboyCointr::VEINTR(int SegmentNber,int ComponentNber){std::string Name="VEINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
double      MuonboyCointr::CHINTR(int SegmentNber)                  {std::string Name="CHINTR"   ; return GetMuboRealVar(Name,SegmentNber,NotDef,NotDef);}
double      MuonboyCointr::T0INTR(int SegmentNber)                  {std::string Name="T0INTR"   ; return GetMuboRealVar(Name,SegmentNber,NotDef,NotDef);}
double      MuonboyCointr::ET0INTR(int SegmentNber)                 {std::string Name="ET0INTR"  ; return GetMuboRealVar(Name,SegmentNber,NotDef,NotDef);}
int         MuonboyCointr::MlNber(int SegmentNber)                  {std::string Name="NbMlINTR" ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}

int         MuonboyCointr::NUINTR(int SegmentNber)                  {std::string Name="NUINTR"   ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}
int         MuonboyCointr::NAINTR(int SegmentNber)                  {std::string Name="NAINTR"   ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}
double      MuonboyCointr::XDINTR(int SegmentNber,int HitNber)      {std::string Name="XDINTR"   ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::YDINTR(int SegmentNber,int HitNber)      {std::string Name="YDINTR"   ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::ZDINTR(int SegmentNber,int HitNber)      {std::string Name="ZDINTR"   ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::RDINTR(int SegmentNber,int HitNber)      {std::string Name="RDINTR"   ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::XDdINTR(int SegmentNber,int HitNber)     {std::string Name="XDdINTR"  ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::YDdINTR(int SegmentNber,int HitNber)     {std::string Name="YDdINTR"  ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
double      MuonboyCointr::ZDdINTR(int SegmentNber,int HitNber)     {std::string Name="ZDdINTR"  ; return GetMuboRealVar(Name,HitNber,SegmentNber,NotDef);}
int         MuonboyCointr::IUINTR(int SegmentNber,int HitNber)      {std::string Name="IUINTR"   ; return GetMuboIntVar (Name,HitNber,SegmentNber,NotDef);}

std::string MuonboyCointr::StationName(int SegmentNber)             {std::string Name="SNINTR"      ; return GetMuboCharVar(Name,SegmentNber,NotDef,NotDef);}
int         MuonboyCointr::ItypStationName(int SegmentNber)         {std::string Name="ItypMboyAmdc"; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}

int         MuonboyCointr::LinearIndex(int SegmentNber,int Index)   {std::string Name="IUINTRext"; return GetMuboIntVar (Name,Index,SegmentNber,NotDef);}
double      MuonboyCointr::Rdigi(int SegmentNber,int Index)         {std::string Name="RdigINTR" ; return GetMuboRealVar(Name,Index,SegmentNber,NotDef);}
double      MuonboyCointr::Rtrak(int SegmentNber,int Index)         {std::string Name="RtraINTR" ; return GetMuboRealVar(Name,Index,SegmentNber,NotDef);}
double      MuonboyCointr::DXINTR(int SegmentNber,int ComponentNber){std::string Name="DXINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
double      MuonboyCointr::DYINTR(int SegmentNber,int ComponentNber){std::string Name="DYINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
double      MuonboyCointr::DZINTR(int SegmentNber,int ComponentNber){std::string Name="DZINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
double      MuonboyCointr::ERINTR(int SegmentNber,int ComponentNber){std::string Name="ERINTR"   ; return GetMuboRealVar(Name,ComponentNber,SegmentNber,NotDef);}
int         MuonboyCointr::IERINTR(int SegmentNber)                 {std::string Name="IERINTR"  ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}

int         MuonboyCointr::IKINTR(int SegmentNber)                 {std::string Name="IKINTR"  ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}

int         MuonboyCointr::NHINTR(int SegmentNber)                  {std::string Name="NHINTR"   ; return GetMuboIntVar (Name,SegmentNber,NotDef,NotDef);}
int         MuonboyCointr::IHINTR(int SegmentNber,int HoleNber)     {std::string Name="IHINTR"   ; return GetMuboIntVar (Name,HoleNber,SegmentNber,NotDef);}
int         MuonboyCointr::TYHINTR(int SegmentNber,int HoleNber)     {std::string Name="TYHINTR"   ; return GetMuboIntVar (Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::XHINTR(int SegmentNber,int HoleNber)     {std::string Name="XHINTR"   ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::YHINTR(int SegmentNber,int HoleNber)     {std::string Name="YHINTR"   ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::ZHINTR(int SegmentNber,int HoleNber)     {std::string Name="ZHINTR"   ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::XHdINTR(int SegmentNber,int HoleNber)    {std::string Name="XHdINTR"  ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::YHdINTR(int SegmentNber,int HoleNber)    {std::string Name="YHdINTR"  ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
double      MuonboyCointr::ZHdINTR(int SegmentNber,int HoleNber)    {std::string Name="ZHdINTR"  ; return GetMuboRealVar(Name,HoleNber,SegmentNber,NotDef);}
