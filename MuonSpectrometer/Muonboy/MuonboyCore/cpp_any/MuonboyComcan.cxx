/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/MuonboyComcan.h"

#include "MuonboyCore/GetSetMuonboyVariable.h"


MuonboyComcan::MuonboyComcan(){}

MuonboyComcan::~MuonboyComcan(){}

void   MuonboyComcan::Print(std::ostream* /*out*/){}

void  MuonboyComcan::SetNBCAN(int Value_i)                                {std::string Name="NBCAN";  int    Value = Value_i ; SetMuboIntVar (Name,NotDef,NotDef,NotDef,Value);}

void  MuonboyComcan::SetCOXCAN(int TrackNber,int ParaNberI,int ParaNberJ,double Value_i){std::string Name="COXCAN" ; SetMuboRealVar(Name,ParaNberJ,ParaNberI,TrackNber,Value_i);}
void  MuonboyComcan::SetPAXCAN(int TrackNber,int ParaNber ,double Value_i)              {std::string Name="PAXCAN" ; SetMuboRealVar(Name,ParaNber ,TrackNber,NotDef   ,Value_i);}
void  MuonboyComcan::SetVXXCAN(int TrackNber,int CompoNber,double Value_i)              {std::string Name="VXXCAN" ; SetMuboRealVar(Name,CompoNber,TrackNber,NotDef   ,Value_i);}


void  MuonboyComcan::SetCHICAN(int TrackNber,double Value_i)             {std::string Name="CHICAN"; SetMuboRealVar(Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetNDFCAN(int TrackNber,int    Value_i)             {std::string Name="NDFCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetCXXCAN(int TrackNber,int FitKind,double Value_i) {std::string Name="CXXCAN"; SetMuboRealVar(Name,FitKind,TrackNber,NotDef,Value_i);}

void  MuonboyComcan::SetNBMSXX(int TrackNber,int    Value_i) {std::string Name="NBMSXX"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetNZXCAN(int TrackNber,int    Value_i) {std::string Name="NZXCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetNSXCAN(int TrackNber,int    Value_i) {std::string Name="NSXCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetNZOCAN(int TrackNber,int    Value_i) {std::string Name="NZOCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetNHXCAN(int TrackNber,int    Value_i) {std::string Name="NHXCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetISECAN(int TrackNber,int    Value_i) {std::string Name="ISECAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}
void  MuonboyComcan::SetFLGCAN(int TrackNber,int    Value_i) {std::string Name="FLGCAN"; SetMuboIntVar (Name,TrackNber,NotDef,NotDef,Value_i);}

int    MuonboyComcan::NBCAN()                                          {std::string Name="NBCAN";  return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}
int    MuonboyComcan::NFICAN()                                         {std::string Name="NFICAN"; return GetMuboIntVar (Name,NotDef,NotDef,NotDef);}

int    MuonboyComcan::FLGCAN(int TrackNber)                            {std::string Name="FLGCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}

double MuonboyComcan::CHICAN(int TrackNber)                            {std::string Name="CHICAN"; return GetMuboRealVar(Name,TrackNber,NotDef,NotDef);}
double MuonboyComcan::CXXCAN(int TrackNber,int FitKind)                {std::string Name="CXXCAN"; return GetMuboRealVar(Name,FitKind,TrackNber,NotDef);}
double MuonboyComcan::VMUCAN(int TrackNber,int CompoNber)              {std::string Name="VMUCAN"; return GetMuboRealVar(Name,CompoNber,TrackNber,NotDef);}

double MuonboyComcan::COXCAN(int TrackNber,int ParaNberI,int ParaNberJ){std::string Name="COXCAN"; return GetMuboRealVar(Name,ParaNberJ,ParaNberI,TrackNber);}
double MuonboyComcan::PAXCAN(int TrackNber,int ParaNber)               {std::string Name="PAXCAN"; return GetMuboRealVar(Name,ParaNber,TrackNber,NotDef);}
double MuonboyComcan::VXXCAN(int TrackNber,int CompoNber)              {std::string Name="VXXCAN"; return GetMuboRealVar(Name,CompoNber,TrackNber,NotDef);}

double MuonboyComcan::COYCAN(int TrackNber,int ParaNberI,int ParaNberJ){std::string Name="COYCAN"; return GetMuboRealVar(Name,ParaNberJ,ParaNberI,TrackNber);}
double MuonboyComcan::PAYCAN(int TrackNber,int ParaNber)               {std::string Name="PAYCAN"; return GetMuboRealVar(Name,ParaNber,TrackNber,NotDef);}
double MuonboyComcan::VYYCAN(int TrackNber,int CompoNber)              {std::string Name="VYYCAN"; return GetMuboRealVar(Name,CompoNber,TrackNber,NotDef);}

double MuonboyComcan::COZCAN(int TrackNber,int ParaNberI,int ParaNberJ){std::string Name="COZCAN"; return GetMuboRealVar(Name,ParaNberJ,ParaNberI,TrackNber);}
double MuonboyComcan::PAZCAN(int TrackNber,int ParaNber)               {std::string Name="PAZCAN"; return GetMuboRealVar(Name,ParaNber,TrackNber,NotDef);}
double MuonboyComcan::VZZCAN(int TrackNber,int CompoNber)              {std::string Name="VZZCAN"; return GetMuboRealVar(Name,CompoNber,TrackNber,NotDef);}

int    MuonboyComcan::NBMSXX(int TrackNber)                            {std::string Name="NBMSXX"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
double MuonboyComcan::RRMSXX(int TrackNber,int MulScatNber)            {std::string Name="RRMSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::X0MSXX(int TrackNber,int MulScatNber)            {std::string Name="X0MSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::S1MSXX(int TrackNber,int MulScatNber)            {std::string Name="S1MSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::S2MSXX(int TrackNber,int MulScatNber)            {std::string Name="S2MSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::THMSXX(int TrackNber,int MulScatNber)            {std::string Name="THMSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::ENMSXX(int TrackNber)                            {std::string Name="ENMSXX"; return GetMuboRealVar(Name,TrackNber,NotDef,NotDef);}
double MuonboyComcan::EAMSXX(int TrackNber,int MulScatNber)            {std::string Name="EAMSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::EBMSXX(int TrackNber,int MulScatNber)            {std::string Name="EBMSXX"; return GetMuboRealVar(Name,MulScatNber,TrackNber,NotDef);}
double MuonboyComcan::VinVouXX (int TrackNber,int MulScatNber,int Ivinvouparam){std::string Name="VinVouXX" ; return GetMuboRealVar(Name,Ivinvouparam,MulScatNber,TrackNber);}
double MuonboyComcan::VinVouXX2(int TrackNber,int MulScatNber,int Ivinvouparam){std::string Name="VinVouXX2"; return GetMuboRealVar(Name,Ivinvouparam,MulScatNber,TrackNber);}
int    MuonboyComcan::LLMSXX(int TrackNber,int MulScatNber)            {std::string Name="LLMSXX"; return GetMuboIntVar (Name,MulScatNber,TrackNber,NotDef);}

double MuonboyComcan::RZXCAN(int TrackNber,int Index)                  {std::string Name="RZXCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RZICAN(int TrackNber,int Index)                  {std::string Name="RZICAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RZISIM(int TrackNber,int Index)                  {std::string Name="RZISIM"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::EZIFIT(int TrackNber,int Index)                  {std::string Name="EZIFIT"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RSXCAN(int TrackNber,int Index)                  {std::string Name="RSXCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RSISIM(int TrackNber,int Index)                  {std::string Name="RSISIM"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ESIFIT(int TrackNber,int Index)                  {std::string Name="ESIFIT"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::XDCCAN(int TrackNber,int Index)		       {std::string Name="XDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::YDCCAN(int TrackNber,int Index)		       {std::string Name="YDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ZDCCAN(int TrackNber,int Index)		       {std::string Name="ZDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ENCCAN(int TrackNber,int Index)		       {std::string Name="ENCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RDCCAN(int TrackNber,int Index)		       {std::string Name="RDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::SDCCAN(int TrackNber,int Index)		       {std::string Name="SDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::XDCdCAN(int TrackNber,int Index)                 {std::string Name="XDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::YDCdCAN(int TrackNber,int Index)                 {std::string Name="YDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ZDCdCAN(int TrackNber,int Index)                 {std::string Name="ZDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::XSDCCAN(int TrackNber,int Index)		       {std::string Name="XSDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::YSDCCAN(int TrackNber,int Index)		       {std::string Name="YSDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ZSDCCAN(int TrackNber,int Index)		       {std::string Name="ZSDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ESNCCAN(int TrackNber,int Index)		       {std::string Name="ESNCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::RSDCCAN(int TrackNber,int Index)		       {std::string Name="RSDCCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::XSDCdCAN(int TrackNber,int Index)                {std::string Name="XSDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::YSDCdCAN(int TrackNber,int Index)                {std::string Name="YSDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}
double MuonboyComcan::ZSDCdCAN(int TrackNber,int Index)                {std::string Name="ZSDCdCAN"; return GetMuboRealVar(Name,Index,TrackNber,NotDef);}

int    MuonboyComcan::NDFCAN(int TrackNber)                            {std::string Name="NDFCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::ISECAN(int TrackNber)                            {std::string Name="ISECAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::ITICAN(int TrackNber,int Index)                  {std::string Name="ITICAN"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}
int    MuonboyComcan::ITXCAN(int TrackNber,int Index)                  {std::string Name="ITXCAN"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}
int    MuonboyComcan::NZXCAN(int TrackNber)                            {std::string Name="NZXCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::NZOCAN(int TrackNber)                            {std::string Name="NZOCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::IZXCAN(int TrackNber,int Index)                  {std::string Name="IZXCAN"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}
int    MuonboyComcan::NSXCAN(int TrackNber)                            {std::string Name="NSXCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::ISXCAN(int TrackNber,int Index)                  {std::string Name="ISXCAN"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}

int    MuonboyComcan::NHXCAN(int TrackNber)                          {std::string Name="NHXCAN"; return GetMuboIntVar (Name,TrackNber,NotDef,NotDef);}
int    MuonboyComcan::IHXCAN(int TrackNber,int HoleNber)             {std::string Name="IHXCAN"; return GetMuboIntVar (Name,HoleNber,TrackNber,NotDef);}
int    MuonboyComcan::TYHCAN(int TrackNber,int HoleNber)             {std::string Name="TYHCAN"; return GetMuboIntVar (Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::ENHCAN(int TrackNber,int HoleNber)             {std::string Name="ENHCAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::XHACAN(int TrackNber,int HoleNber)             {std::string Name="XHACAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::YHACAN(int TrackNber,int HoleNber)             {std::string Name="YHACAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::ZHACAN(int TrackNber,int HoleNber)             {std::string Name="ZHACAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::XHdCAN(int TrackNber,int HoleNber)             {std::string Name="XHdCAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::YHdCAN(int TrackNber,int HoleNber)             {std::string Name="YHdCAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}
double MuonboyComcan::ZHdCAN(int TrackNber,int HoleNber)             {std::string Name="ZHdCAN"; return GetMuboRealVar(Name,HoleNber,TrackNber,NotDef);}

int    MuonboyComcan::Index1stCo(int TrackNber,int Index)              {std::string Name="IZXCANext"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}
int    MuonboyComcan::Index2ndCo(int TrackNber,int Index)              {std::string Name="ISXCANext"; return GetMuboIntVar (Name,Index,TrackNber,NotDef);}

