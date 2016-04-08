/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************/
/* HisToKomp Class               */
/* 03/Dec/2006                   */
/*********************************/

#include <iostream>
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TCanvas.h"

#ifndef __HisToKomp__
#define __HisToKomp__
 
#ifndef ROOT_TFile
#include "TFile.h"
#endif

#ifndef ROOT_TH1
#include "TH1.h"
#endif

#ifndef ROOT_TH2
#include "TH2.h"
#endif

#ifndef ROOT_TF1
#include "TF1.h"
#endif

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif

#define MaxNFiles 10

//class HisToKomp : public TString, public TNamed, public TFile {
class HisToKomp : public TNamed{
 
 private:
  // member functions
  //  static int MaxNFiles = 10;
  int NFiles;
  TString *FileName[MaxNFiles]; // nom dels fitxers
  TString *HisDirec;     // directori del fitxer on estan els histogrames
  TH1F *HistoRef[MaxNFiles];    // aquest histos es gasten per a guardar les opcions
  TFile *HisFile[MaxNFiles];    // fitxers
  TString *FileLabel[MaxNFiles];// etiqueta per aquest fitxer
  TH1F *HD[MaxNFiles];  // variable interna on guarda els histogrames a pintar.
  TH2F *HD2D[MaxNFiles];  // variable interna on guarda els histogrames de 2D a pintar.
  TProfile *HD2P[MaxNFiles];  // variable interna on guarda el profile dels histos2D a pintar.
  bool HistoHasFun[MaxNFiles];  // Just to know if histogram has a fitted function
  TF1 *HistoFun[MaxNFiles];     // the function
  TString *DrawOptions; //options al pintar els histogrames
  bool HistoHasNorm[MaxNFiles];  // Just to know if histogram has a defined norm
  bool HistoHasLumi[MaxNFiles];  // Just to know if the file has a defined luminosity
  double HistoLumi[MaxNFiles];  // luminosity 
  bool DrawHistoOnOff[MaxNFiles]; // diu si els histogrames d'eixe fitxer es pinten

  float v_HiLeg_lx;
  float v_HiLeg_ly;
  float v_HiLeg_ux;
  float v_HiLeg_uy;

  float v_FuLeg_lx;
  float v_FuLeg_ly;
  float v_FuLeg_ux;
  float v_FuLeg_uy;

  float CurrentLegendLowerY;

  const char* EntriesLabel;

  int SeriesMarkerStyle;
  int SeriesMarkerColor;

  // member variables

 protected:
  bool FileOpen;
  //
  void Initialize(); 
  void InitGrafStyle();
  float MarcMaximY();
  float GetLegendLowerY();
  void SetLegendLowerY(float);
  
 public:  
  //constructor
  HisToKomp();
  HisToKomp(const char *name);

  bool CheckFile(const int);
  void Draw(const char*, const char* xopt=NULL);
  void DrawSeries(const char*, const char* xopt=NULL);
  void DrawProf(const char*, const char* xopt=NULL);
  void Draw2D(const char*, const char* xopt=NULL);
  int  GetNFiles();
  char GetLabel(const int);
  void PrintEntriesLegend(const char* elabel="");
  void PrintAverageLegend();
  void PrintHistosLegend();
  void PrintFunctionLegend();
  void SetDirectory(const char*);
  void SetEntriesLegend(const char* elabel="entries");
  void SetFile(const int, const char*, const char* flabel="Default label", const int icolor=0);
  void SetFillStyle(const int, const int iStyle);
  void SetFillStyle(const int iStyle);
  void SetLabel(const int, const char*);
  void SetLabel(const char*);
  void SetLineColor(const int, const int iColor=0);
  void SetLineStyle(const int, const int iStyle);
  void SetLineStyle(const int iStyle);
  void SetLuminosity(const int, const double);
  void SetLuminosity(const double);
  void SetSeriesMarkerColor(const int);
  void SetSeriesMarkerStyle(const int);
  void SetNextFile(const char*, const char* flabel="Default label", const int icolor=0);
  void SetNorm(const int, const double fNorm=1.);
  void SetNorm(const double fNorm=1.);
  void SwitchOff(const int);
  void SwitchOn(const int);
};

//#include <HisToKomp.cc>

#endif
