/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************/
/* HisToKomp Class               */
/* 03/Dec/2006                   */
/*********************************/
#include <HisToKomp.h>

// constructor
HisToKomp::HisToKomp()
{
};

HisToKomp::HisToKomp(const char *name)
{
  Initialize();
  SetName(name);
};

////////////////////////////////////////////////
void HisToKomp::Initialize()
{

  SetName("HisToKomp default name"); 
  HisDirec = new TString("."); // directori del fitxer on estan els histogrames
  SetEntriesLegend();
  NFiles = 0;
  InitGrafStyle();

  // series style
  SetSeriesMarkerStyle(20);
  SetSeriesMarkerColor(1);
};

////////////////////////////////////////////////
void HisToKomp::SetFile(const int i, const char *fname, const char *flabel, const int icolor)
{
  char nameh[80];

  FileName[NFiles] = new TString(fname);
  FileLabel[NFiles] = new TString(flabel);
  
  // histos de referencia de les opcions
  sprintf(nameh,"hRef_%d",NFiles);
  HistoRef[NFiles] = new TH1F (nameh, nameh, 1, 0., 1.);

  // definim el color dels histogrames
  SetLineColor(NFiles, icolor);

  // fill style empty
  HistoRef[NFiles]->SetFillStyle(0);

  // obri fitxer
  HisFile[NFiles] = new TFile(FileName[NFiles]->Data());

  // Per defecte el fitxer te la seua normalitzacio. Es pot canviar
  HistoHasNorm[NFiles] = 0;

  // Per defecte el fitxer tampoc te definida una luminositat
  HistoHasLumi[NFiles] = 0;
  
  // per defecte els histos es pinten
  DrawHistoOnOff[NFiles] = 1;
  
  //
  NFiles++;  
}

////////////////////////////////////////////////
void HisToKomp::SetNextFile(const char *fname, const char *flabel, const int icolor)
{
  SetFile(NFiles, fname, flabel, icolor);
}

////////////////////////////////////////////////
void HisToKomp::SetLabel(const int i, const char *flabel)
{
  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    if (FileLabel[i] == NULL) { // l'etiqueta no esta definida encara. Cal crear-la
      FileLabel[i] = new TString(flabel);
    }
    else { // i si esta definida, cal rescriure-la.
      char labelf[80];
      //char* labelf;
      strcpy(labelf,".");
      strcat(labelf,flabel);
      FileLabel[i]->ReadBuffer(labelf);
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting label for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}

////////////////////////////////////////////////
void HisToKomp::SetLabel(const char *flabel)
{
  // This function sets the label style for the latest defined histograms file
  SetLabel(GetNFiles()-1, flabel);
}

////////////////////////////////////////////////
char HisToKomp::GetLabel(const int i)
{
  // This function sets the label style for the latest defined histograms file
  return *FileLabel[i];
}

////////////////////////////////////////////////
void HisToKomp::SetLineStyle(const int istyle)
{
  // This function sets the line style for the latest defined histograms file
  cout << " ** Fill style short cut **  File" << GetNFiles()-1 << " style " << istyle << endl;
  SetLineStyle(GetNFiles()-1, istyle);
}

////////////////////////////////////////////////
void HisToKomp::SetLineColor(const int i, const int icolor)
{
  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    if (icolor>0) {
      HistoRef[i]->SetLineColor(icolor);
    }
    else {
      HistoRef[i]->SetLineColor(i+1);
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting color for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}

////////////////////////////////////////////////
void HisToKomp::SetLineStyle(const int i, const int istyle)
{
  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    if (istyle>0) {
      HistoRef[i]->SetLineStyle(istyle);
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting line style for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}

////////////////////////////////////////////////
void HisToKomp::SetLineStyle(const int istyle)
{
  // This function sets the line style for the lates defined histograms file
  cout << " ** Fill style short cut **  File" << GetNFiles()-1 << " style " << istyle << endl;
  SetLineStyle(GetNFiles()-1, istyle);
}

////////////////////////////////////////////////
void HisToKomp::SetFillStyle(const int i, const int istyle)
{
  // comprova que el fitxer i esta definit
  if (CheckFile(i)) {
    if (istyle>0) {
      HistoRef[i]->SetFillStyle(istyle);
      cout << "  »» HisToKomp «« file " << i << " set file style " << istyle << endl;
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting fill style for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}

//////////////////////////////////////////////////
void HisToKomp::SetFillStyle(const int istyle)
{
  // This function sets the style for the last defined histograms file
  // cout << " ** Fill style short cut **  File " << GetNFiles()-1 << " style " << istyle << endl;
  SetFillStyle(GetNFiles()-1, istyle);
}

////////////////////////////////////////////////
void HisToKomp::SetFillColor(const int i, const int icolor)
{
  // comprova que el fitxer i esta definit
  if (CheckFile(i)) {
    if (istyle>0) {
      HistoRef[i]->SetFillStyle(icolor);
      cout << "  »» HisToKomp «« file " << i << " set file style " << icolor << endl;
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting fill color for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}

//////////////////////////////////////////////////
void HisToKomp::SetFillColor(const int icolor)
{
  // This function sets the style for the last defined histograms file
  // cout << " ** Fill style short cut **  File " << GetNFiles()-1 << " style " << istyle << endl;
  SetFillStyle(GetNFiles()-1, icolor);
}

//////////////////////////////////////////////////
void HisToKomp::SetSeriesMarkerStyle(const int istyle)
{
  SeriesMarkerStyle = istyle;
}

//////////////////////////////////////////////////
void HisToKomp::SetSeriesMarkerColor(const int icolor)
{
  SeriesMarkerColor = icolor;
}

////////////////////////////////////////////////
void HisToKomp::SetDirectory(const char *dname)
{
  char nameh[80];
  //char* nameh;
  strcpy(nameh,".");
  strcat(nameh,dname);
  HisDirec->ReadBuffer(nameh);
}

  
////////////////////////////////////////////////
void HisToKomp::Draw(const char *hname, const char *xopt)
{
  bool DrawOnSameFrame = 0;
  bool DrawOnNewFrame = 0;
  bool PrintLegendOnTop = 1;
  bool PrintAverage = 0;
  bool PrintEntries=0;
  bool PrintFunLegend=0;  // aquesta s'activa sols si el histograma te una funcio
  bool LogyScale = 0;
  bool Normalize = 1;
  bool SeriesPlot = 0;  

  // primer processem les opcions 
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    if (1 == 0) cout << " Pintant histos amb opcions: " 
		     << DrawOptions->Data() 
		     << endl;
    DrawOnSameFrame = DrawOptions->Contains("same");
    DrawOnNewFrame = DrawOptions->Contains("new");
    PrintLegendOnTop = !DrawOptions->Contains("nolegend");
    PrintAverage = DrawOptions->Contains("printaverage");
    PrintEntries = DrawOptions->Contains("printentries");
    LogyScale = DrawOptions->Contains("logy");
    Normalize = !DrawOptions->Contains("nonormalize");
    SeriesPlot = DrawOptions->Contains("seriesplot");
  }

  // ara continuem
  char nameh[80];
  char chopt[8];
  
  // Si volem pintar una serie crida a la funcio membre que ho fa
  if (SeriesPlot) {
    DrawSeries(hname,"labels,nolegend,line"); 
    return;
  }

  // Pilla els histogrames a pintar
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  for (int i=0; i < NFiles; i++) {
    HD[i] = (TH1F*)HisFile[i]->Get(nameh); 
  }


  // defineix el marc
  float HYMax = MarcMaximY();

  // i ara pinta els histogrames
  for (int i=0; i < NFiles; i++) {
    //sprintf(chopt,"hist");
    sprintf(chopt,"");
    if (i == 0) {
      if (DrawOnSameFrame) sprintf(chopt,"same"); 
    }
    // if (i>0) sprintf(chopt,"samehist");
    if (i>0) sprintf(chopt,"same");
    if (DrawOnNewFrame) sprintf(chopt,"");

    // mira si te una funcio ajustada
    TList *FunList;
    HistoHasFun[i] = 0;
    FunList = HD[i]->GetListOfFunctions();
    if (FunList->GetEntries() != 0) { // Efectivament, hi ha funcions en la llista
      HistoFun[i] = (TF1*) FunList->First(); // prenem la primera
      HistoHasFun[i] = 1;
      PrintFunLegend=1;
    }

    // adjusta l'estil
    if (HistoRef[i]!= NULL) HD[i]->SetLineColor(HistoRef[i]->GetLineColor());
    if (HistoRef[i]!= NULL) HD[i]->SetLineStyle(HistoRef[i]->GetLineStyle());
    if (HistoRef[i]!= NULL) HD[i]->SetLineWidth(2);
    if (HistoRef[i]!= NULL) {
      HD[i]->SetFillStyle(HistoRef[i]->GetFillStyle());
      HD[i]->SetFillColor(HistoRef[i]->GetFillColor());
    } 

    if (HistoHasFun[i]) {
      HistoFun[i]->SetLineColor(HistoRef[i]->GetLineColor());
      HistoFun[i]->SetLineWidth(1);
    }

    // histo normalitzat
    if (HistoHasNorm[i] == 1 && Normalize) {
      cout << " normalitzant histo " << i << endl;
      HD[i]->SetNormFactor(HistoRef[i]->GetNormFactor());
    }
    
    // histo te definida una lluminositat
    if (HistoHasLumi[i] == 1 && Normalize) {
      HD[i]->Scale(HistoLumi[i]);
    }

    if (HistoHasFun[i] == 1 && HistoHasLumi[i] == 1) {
      // proba a vore si normalitze les funcions
      // Cas 1: ajust a una gaussiana sols
      if (strcmp(HistoFun[i]->GetName(),"gaus")==0) {
	double Heigh1 = HistoFun[i]->GetParameter(0);
	Heigh1 = Heigh1 * HistoLumi[i];
	HistoFun[i]->SetParameter(0,Heigh1);
      }
      // Cas 2: ajust a dos gaussianas sumades
      if (strcmp(HistoFun[i]->GetName(),"GaussPlusGauss")==0) {
	double Heigh1 = HistoFun[i]->GetParameter(0);
	double Heigh2 = HistoFun[i]->GetParameter(3);
	Heigh1 = Heigh1 * HistoLumi[i];
	HistoFun[i]->SetParameter(0,Heigh1);
	Heigh2 = Heigh2 * HistoLumi[i];
	HistoFun[i]->SetParameter(3,Heigh2);
      }
    }

    // marquem el maxim;
    if (Normalize) HD[i]->GetYaxis()->SetRangeUser(0.,HYMax);

    if (LogyScale) HD[i]->GetYaxis()->SetRangeUser(1.,HYMax);


    // i ara si cal es pinta
    if (DrawHistoOnOff[i]) HD[i]->DrawCopy(chopt);
  }
  
  
  // llegenda dels histogrames
  if (PrintLegendOnTop) {
    PrintHistosLegend();
  }

  //  llegenda dels ajusts
  if (PrintFunLegend && PrintLegendOnTop) {
    PrintFunctionLegend();
  }

  // Print Average
  if (PrintAverage) {
    PrintAverageLegend();
  }
  // Print Average
  if (PrintEntries) {
    PrintEntriesLegend();
  }
  // Ja hem acabat

}
////////////////////////////////////////////////
void HisToKomp::DrawSeries(const char *hname, const char *xopt)
{
  bool PrintLegendOnTop = 1;
  bool PrintAverage = 0;
  bool LogyScale = 0;
  bool UseLegendLabels = 1;
  bool DrawLine=0;

  // primer processem les opcions 
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    if (1 == 0) cout << " Pintant histos amb opcions: " 
		     << DrawOptions->Data() 
		     << endl;
    PrintLegendOnTop = !DrawOptions->Contains("nolegend");
    PrintAverage = DrawOptions->Contains("printaverage");
    LogyScale = DrawOptions->Contains("logy");
    UseLegendLabels = DrawOptions->Contains("labels");
    DrawLine = DrawOptions->Contains("line");
  }

  // ara continuem
  char nameh[80];
  char chopt[8];
  
  // Pilla els histogrames a pintar
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  for (int i=0; i < NFiles; i++) {
    HD[i] = (TH1F*)HisFile[i]->Get(nameh); 
  }

  // defineix el histograma marc on pintem la serie
  TH1F* HSerie = new TH1F("serie",HD[0]->GetName(),
			 NFiles, -0.5, NFiles-0.5); 
  
  // canviem els valors del eix per les etiquetes
  for (int i=0; i < NFiles; i++) {
    HSerie->GetXaxis()->SetBinLabel(i+1,GetLabel(i));
  }  

  // i ara omplim els histo de la serie amb els valors
  float valor;
  for (int i=0; i < NFiles; i++) {
    valor = HD[i]->GetMean();
    HSerie->SetBinContent(i+1,valor);
  }

  // opcions per a pintar la serie
  sprintf(chopt,"p");
  if (SeriesMarkerStyle == 0) {
    sprintf(chopt,"");
  }
  else {
    sprintf(chopt,"p");
    HSerie->SetMarkerStyle(SeriesMarkerStyle);
    HSerie->SetMarkerColor(SeriesMarkerColor);
  }

  //opcio de pintar la linea
  if (DrawLine) {
    sprintf(chopt,"l%s",chopt);
    HSerie->SetLineColor(SeriesMarkerColor);
  }
  
  // pintem la serie
  HSerie->DrawCopy(chopt);
  
  // llegenda dels histogrames
  if (PrintLegendOnTop) {
    PrintHistosLegend();
  }


  // Print Average
  if (PrintAverage) {
    PrintAverageLegend();
  }

  // borrem
  delete HSerie;
  
  // Ja hem acabat
}

////////////////////////////////////////////////
void HisToKomp::DrawProf(const char *hname, const char *xopt)
{
  //
  // Pren un histograma en 2D crea un profile i el pinta
  //
  
  // ara continuem
  char nameh[80];
  char chopt[8];
  
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  
  
  float PYmax = -99999999.;  // fiquem valors sin sentit
  float PYmin =  99999999.;
  float thisMax, thisMin;
  // Pilla els histogrames a pintar
  for (int i=0; i < NFiles; i++) {
    if (i == 0) sprintf(chopt,"");
    HD2D[i] = (TH2F*)HisFile[i]->Get(nameh); 
    // maxim i minim per al rang dels histos
    thisMax = HD2D[i]->ProfileX()->GetMaximum();
    thisMin = HD2D[i]->ProfileX()->GetMinimum();
    if (thisMax>PYmax) PYmax = thisMax;
    if (thisMin<PYmin) PYmin = thisMin;
    HD2D[i]->SetMarkerStyle(20);
  }
  float delta = 0.05*(PYmax - PYmin);
  PYmax += delta;
  PYmin -= delta;

  // i ara defineix els profiles
  for (int i=0; i < NFiles; i++) {
    if (i == 0) {
      sprintf(chopt,"");
    }
    if (i>0) sprintf(chopt,"same");

    TProfile *HProfi = HD2D[i]->ProfileX(); 

    if (HistoRef[i]!= NULL) {
      HProfi->SetLineColor(HistoRef[i]->GetLineColor());
      HProfi->SetMarkerColor(HistoRef[i]->GetLineColor());
    }
    if (i==0) {
      HProfi->SetMaximum(PYmax);
      HProfi->SetMinimum(PYmin);
    }
    HProfi->DrawCopy(chopt);
  }
}

////////////////////////////////////////////////
void HisToKomp::Draw2D(const char *hname, const char *xopt)
{
  //
  // Aquesta macro esta feta pensant en el plot de Muge de split tracks
  //
  TCanvas *Can2D = new TCanvas("TrkSplit","TrkSplit",25,25,800,800);

  // parteix el canvas segons els fitxers
  switch (NFiles) {
  case 2:
    Can2D->Divide(2,1);
    break;
  case 3:
    Can2D->Divide(3,1);
    break;
  case 4:
    Can2D->Divide(2,2);
    break;
  case 5:
    Can2D->Divide(3,2);
    break;
  case 6:
    Can2D->Divide(3,2);
    break;
  case 7:
    Can2D->Divide(4,2);
    break;
  case 8:
    Can2D->Divide(4,2);
    break;
  default:
    Can2D->Divide(5,5);
  }

  // primer processem les opcions al pintar
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    cout << " Pintant histos amb opcions: " 
	 << DrawOptions->Data() 
	 << endl;
    // aci hi cal ara fer el tractament de les opcions... no esta fet encara
  }
  
  // ara continuem
  char nameh[80];
  char chopt[8];
  
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  cout << " ** pinta ** nameh = " << nameh << endl;
  
  // Pilla els histogrames a pintar
  for (int i=0; i < NFiles; i++) {
    if (i == 0) sprintf(chopt,"");
    HD2D[i] = (TH2F*)HisFile[i]->Get(nameh); 
  }

  // defineix el marc
  float HYMax = MarcMaximY();
  cout << " Maxim histo= " << HYMax << endl;

  // i ara pinta els histogrames
  for (int i=0; i < NFiles; i++) {
    if (i == 0) {
      sprintf(chopt,"");
    }
    if (i>0) sprintf(chopt,"same");
    Can2D->cd(i+1);

    HD2D[i]->SetMaximum(HYMax);
    if (HistoRef[i]!= NULL) HD2D[i]->SetLineColor(HistoRef[i]->GetLineColor());
    HD2D[i]->DrawCopy("box");
  }

  // llegenda dels histogrames
  PrintHistosLegend();
    
  Can2D->Print("TrkSplit.gif");
}

////////////////////////////////////////////////
void HisToKomp::PrintHistosLegend()
{
  v_HiLeg_lx = 0.76;
  v_HiLeg_ly = 0.82;
  v_HiLeg_ux = 0.98;
  v_HiLeg_uy = 0.94;

  float deltaX = 0.22;
  float deltaY = 0.12;

  if (NFiles>2) {
    deltaX = 0.25;
    deltaY = 0.15;
  }
  if (NFiles>3) {
    deltaX = 0.28;
    deltaY = 0.24;
  }
  if (NFiles>5) {
    deltaX = 0.33;
    deltaY = 0.29;
  }
  if (NFiles>7) {
    deltaX = 0.38;
    deltaY = 0.34;
  }

  v_HiLeg_lx = v_HiLeg_ux - deltaX;
  v_HiLeg_ly = v_HiLeg_uy - deltaY;
  
  TLegend *Rotllo = new TLegend(v_HiLeg_lx, v_HiLeg_ly, v_HiLeg_ux, v_HiLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  for (int i=0; i < NFiles; i++) {
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],FileLabel[i]->Data(), "l");
  }
  Rotllo->Draw();

  SetLegendLowerY(v_HiLeg_ly);
}

////////////////////////////////////////////////
void HisToKomp::PrintFunctionLegend()
{
  float v_FuLeg_lx = 0.72;
  float v_FuLeg_ly = 0.82;
  float v_FuLeg_ux = 0.99;
  float v_FuLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_FuLeg_uy = GetLegendLowerY() -0.02;

  if (NFiles>2) {
    deltaX = 0.25;
    deltaY = 0.15;
  }
  if (NFiles>3) {
    deltaX = 0.28;
    deltaY = 0.24;
  }
  if (NFiles>5) {
    deltaX = 0.33;
    deltaY = 0.29;
  }
  if (NFiles>7) {
    deltaX = 0.38;
    deltaY = 0.34;
  }

  //v_HiLeg_lx = v_HiLeg_ux - deltaX;
  v_FuLeg_ly = v_FuLeg_uy - deltaY;
  

  //  TLegend *FunRotllo = new TLegend(0.65, 0.60, 0.98, 0.80);    
  TLegend *FunRotllo = new TLegend(0.65, v_FuLeg_ly, v_FuLeg_ux, v_FuLeg_uy);    
  
  // fill style per a que eisca blanc i no transparent
  FunRotllo->SetFillStyle(1001);
  FunRotllo->SetFillColor(10);
  
  char QueDir[80];
  float FitMean;
  float FitSigma;
  bool KnownFunc = 0;
  
  for (int i=0; i < NFiles; i++) {
    if (HistoHasFun[i]) {
      // Cas 1: ajust a una gaussiana sols
      if (strcmp(HistoFun[i]->GetName(),"gaus")==0) {
	KnownFunc = 1;
	FitMean = HistoFun[i]->GetParameter(1);
	FitSigma = HistoFun[i]->GetParameter(2);
      }
      // Cas 2: ajust a dos gaussianas sumades
      if (strcmp(HistoFun[i]->GetName(),"GaussPlusGauss")==0) {
	KnownFunc = 1;
	double Heigh1 = HistoFun[i]->GetParameter(0);
	double Mean1  = HistoFun[i]->GetParameter(1);
	double Sigma1 = HistoFun[i]->GetParameter(2);
	double Heigh2 = HistoFun[i]->GetParameter(3);
	double Mean2  = HistoFun[i]->GetParameter(4);
	double Sigma2 = HistoFun[i]->GetParameter(5);
	FitMean =  (Mean1  *Heigh1        + Mean2  *Heigh2       ) / (Heigh1        + Heigh2       );
	FitSigma = (Sigma1 *Heigh1*Heigh1 + Sigma2 *Heigh2*Heigh2) / (Heigh1*Heigh1 + Heigh2*Heigh2);
      }
      // si coneguem la funcio preparem el text
      if (KnownFunc) {
	sprintf(QueDir,"#mu = %4.2f   #sigma = %4.2f ", FitMean, FitSigma);
	FunRotllo->AddEntry(HD[i], QueDir, "l");
      }
      else {
	cout << " Unknown function: " << HistoFun[i]->GetName() << "  Desole ! " << endl;
      }
    }
    else {
      FunRotllo->AddEntry(HD[i], "fit failed", "l");
    }
  }
  FunRotllo->Draw();
  SetLegendLowerY(v_FuLeg_ly);

}

////////////////////////////////////////////////
void HisToKomp::PrintAverageLegend()
{
  float v_AvLeg_lx = 0.76;
  float v_AvLeg_ly = 0.82;
  float v_AvLeg_ux = 0.99;
  float v_AvLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_AvLeg_uy = GetLegendLowerY() -0.02;

  if (NFiles>2) {
    deltaX = 0.25;
    deltaY = 0.15;
  }
  if (NFiles>3) {
    deltaX = 0.28;
    deltaY = 0.24;
  }
  if (NFiles>5) {
    deltaX = 0.33;
    deltaY = 0.29;
  }
  if (NFiles>7) {
    deltaX = 0.38;
    deltaY = 0.34;
  }

  v_AvLeg_ly = v_AvLeg_uy - deltaY;
  
  TLegend *Rotllo = new TLegend(v_HiLeg_lx, v_AvLeg_ly, v_HiLeg_ux, v_AvLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char QueDir[80];
  for (int i=0; i < NFiles; i++) {
    sprintf(QueDir,"mean = %5.2f ", HD[i]->GetMean());
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],QueDir, "l");
  }
  Rotllo->Draw();
  SetLegendLowerY(v_AvLeg_ly);
}

//////////////////////////////////////////////////
void HisToKomp::SetEntriesLegend(const char* elabel)
{
  if (strlen(elabel)>0) {
    EntriesLabel = elabel;
    sprintf(EntriesLabel,"%s",elabel);
  }
}

////////////////////////////////////////////////
void HisToKomp::PrintEntriesLegend(const char* elabel)
{
  float v_AvLeg_lx = 0.76;
  float v_AvLeg_ly = 0.82;
  float v_AvLeg_ux = 0.99;
  float v_AvLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_AvLeg_uy = GetLegendLowerY() -0.02;

  if (NFiles>3) {
    deltaX = 0.28;
    deltaY = 0.24;
  }
  if (NFiles>5) {
    deltaX = 0.33;
    deltaY = 0.29;
  }
  if (NFiles>7) {
    deltaX = 0.38;
    deltaY = 0.34;
  }

  v_AvLeg_ly = v_AvLeg_uy - deltaY;
  
  TLegend *Rotllo = new TLegend(v_HiLeg_lx, v_AvLeg_ly, v_HiLeg_ux, v_AvLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char QueDir[80];
  for (int i=0; i < NFiles; i++) {
    float entry = HD[i]->GetEntries();
    if (HistoHasLumi[i] == 1) {
      entry = entry * HistoLumi[i];
    }

    
    if (strlen(elabel)) { 
      sprintf(QueDir,"%5.1f %s", entry, elabel);
    } 
    else {
      sprintf(QueDir,"%5.1f %s", entry, EntriesLabel);
    }
    //sprintf(QueDir,"%5.0f event/pb^{-1} ", entry);
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],QueDir, "l");
  }
  Rotllo->Draw();

}
//////////////////////////////////////////////////
void HisToKomp::InitGrafStyle()
{
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  //gStyle->SetOptFit(10);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  //  gStyle->SetStatFontSize(0);
  gStyle->SetStatColor(0);
  gStyle->SetStatW(0.22);
  //gStyle->SetStatH(0.22);
}

//////////////////////////////////////////////////
float HisToKomp::MarcMaximY()
{
  float ValorBo = -999999.; // comencem d'un valor artificialment molt baix
  float ValorTest;

  for (int i=0; i < NFiles; i++) {
    ValorTest = HD[i]->GetMaximum(); 

    // in case files have a different luminosity
    if (HistoHasLumi[i]==1) ValorTest = ValorTest*HistoLumi[i];
 
    // storing the maximum
    if (ValorTest>ValorBo) ValorBo = ValorTest; 
  }  

  if (ValorBo>0) ValorBo = 1.15 * ValorBo; 
  if (ValorBo<0) ValorBo = 0.95 * ValorBo; 

  return ValorBo;
}

//////////////////////////////////////////////////
void HisToKomp::SetNorm(const int iFile, const double fNorm)
{
  if (CheckFile(iFile)) {
    cout << " ** HistoKomp ** setting norm "<< fNorm << endl;
    HistoRef[iFile]->SetNormFactor(fNorm);
    HistoHasNorm[iFile] = 1;
  }
  else {
    cout << " ** HisToKomp ** Error setting Norm for file " 
	 << iFile 
	 << ". File not defined yet."
	 << endl;
  }
}

//////////////////////////////////////////////////
void HisToKomp::SetNorm(const double fNorm)
{
  // This function sets the norm for the last defined histograms file
  SetNorm(GetNFiles()-1, fNorm);
}

//////////////////////////////////////////////////
void HisToKomp::SetLuminosity(const int iFile, const double fLumi)
{
  if (CheckFile(iFile)) {
    cout << " ** HistoKomp ** File '" << FileLabel[iFile]->Data()
	 << "' setting Luminosity "<< fLumi << endl;
    HistoLumi[iFile] = fLumi;
    HistoHasLumi[iFile] = 1;
  }
  else {
    cout << " ** HisToKomp ** Error setting Luminosity for file " 
	 << iFile 
	 << ". File not defined yet."
	 << endl;
  }
}
//////////////////////////////////////////////////
void HisToKomp::SetLuminosity(const double fLumi)
{
  // This function sets the norm for the last defined histograms file
  SetLuminosity(GetNFiles()-1, fLumi);
}

//////////////////////////////////////////////////
bool HisToKomp::CheckFile(const int iFile)
{
  
  bool GoodFile = 0;
  // comprova que el fitxer iFile esta definit
  if ((0<=iFile) && (iFile < NFiles)) {
    GoodFile = 1;
  }
  return GoodFile;
}

//////////////////////////////////////////////////
int HisToKomp::GetNFiles()
{
  return NFiles;
}

//////////////////////////////////////////////////
void HisToKomp::SwitchOff(const int iFile)
{
  DrawHistoOnOff[iFile] = 0;
}

//////////////////////////////////////////////////
void HisToKomp::SwitchOn(const int iFile)
{
  DrawHistoOnOff[iFile] = 1;
}

//////////////////////////////////////////////////
void HisToKomp::SetLegendLowerY(float lowerY)
{
  CurrentLegendLowerY = lowerY;
}

//////////////////////////////////////////////////
float HisToKomp::GetLegendLowerY()
{
  return CurrentLegendLowerY;
}
