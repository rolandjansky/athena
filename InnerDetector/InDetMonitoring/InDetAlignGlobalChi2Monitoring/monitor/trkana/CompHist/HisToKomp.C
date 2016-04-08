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

  // label boxes
  ResetLegendBoxesY();

  // user options
  UsingUserRange = false;

  //
  return;
};

////////////////////////////////////////////////
void HisToKomp::SetFilesDirectory(const char *fFileDir)
{
  FilesDirectory = new TString(fFileDir);
  return;
}
////////////////////////////////////////////////
void HisToKomp::SetFile(const int i, const char *fname, const char *flabel, const int icolor)
{
  char nameh[120];

  if (FilesDirectory == NULL) {
    FileName[NFiles] = new TString(fname);
  }
  else {
    sprintf(nameh,"%s/%s",FilesDirectory->Data(),fname);
    FileName[NFiles] = new TString(nameh);
  }

  FileLabel[NFiles] = new TString(flabel);
  
  // histos de referencia de les opcions
  sprintf(nameh,"hRef_%d",NFiles);
  HistoRef[NFiles] = new TH1F (nameh, nameh, 1, 0., 1.);

  // definim el color dels histogrames
  SetLineColor(NFiles, icolor);

  // define the default line width
  SetLineWidth(NFiles,2);

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
  
  // by default, histograms are not background
  HistoIsBackground[NFiles] = kFALSE;

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
    cout << " ** HisToKomp ** Error setting line color for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}
////////////////////////////////////////////////
void HisToKomp::SetLineColor(const int icolor)
{
  // This function sets the line style for the latest defined histograms file
  cout << " ** Fill style short cut **  File" << GetNFiles()-1 << " style " << istyle << endl;
  SetLineColor(GetNFiles()-1, icolor);
}
////////////////////////////////////////////////
int HisToKomp::GetLineColor(const int i)
{
  int colorin = -1;

  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    colorin = HistoRef[i]->GetLineColor();
  }
  else {
    cout << " ** HisToKomp ** Error retriving line color for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
  return colorin;
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
void HisToKomp::SetLineWidth(const int i, const int iWidth)
{
  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    if (iWidth>0) {
      HistoRef[i]->SetLineWidth(iWidth);
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting line width for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}
////////////////////////////////////////////////
void HisToKomp::SetLineWidth(const int iWidth)
{
  // This function sets the line style for the latest defined histograms file
  // cout << " ** Line width short cut **  File" << GetNFiles()-1 << " width " << iWidth << endl;
  SetLineWidth(GetNFiles()-1, iWidth);
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
int HisToKomp::GetFillStyle(const int i)
{
  int iStyle = -1;

  // comprova que el fitxer i esta definit
  if (i<= NFiles) {
    iStyle = HistoRef[i]->GetFillStyle();
  }
  else {
    cout << " ** HisToKomp ** Error retriving file style for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
  return iStyle;
}

////////////////////////////////////////////////
void HisToKomp::SetFillColor(const int i, const int icolor)
{
  // comprova que el fitxer i esta definit
  if (CheckFile(i)) {
    if (icolor>0) {
      HistoRef[i]->SetFillColor(icolor);
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
  SetFillColor(GetNFiles()-1, icolor);
}

////////////////////////////////////////////////
void HisToKomp::SetMarkerStyle(const int i, const int iStyle)
{
  // comprova que el fitxer i esta definit
  if (CheckFile(i)) {
    if (iStyle>0) {
      HistoRef[i]->SetMarkerStyle(iStyle);
      cout << "  »» HisToKomp «« file " << i << " set maker style " << iStyle << endl;
    }
  }
  else {
    cout << " ** HisToKomp ** Error setting marker style for file " 
	 << i 
	 << ". File not defined yet."
	 << endl;
  }
}
//////////////////////////////////////////////////
void HisToKomp::SetMarkerStyle(const int istyle)
{
  // This function sets the style for the last defined histograms file
  // cout << " ** Fill style short cut **  File " << GetNFiles()-1 << " style " << istyle << endl;
  SetMarkerStyle(GetNFiles()-1, istyle);
}

////////////////////////////////////////////////
void HisToKomp::FlagAsBackground(const int ifile)
{
  // comprova que el fitxer i esta definit
  if (CheckFile(ifile)) {
    cout << "  »» HisToKomp «« file " << ifile << " flagged as background " << endl;
    HistoIsBackground[ifile] = kTRUE;
  }
  else {
    cout << " ** HisToKomp ** Error setting file " 
	 << ifile 
	 << " as background. File not defined yet."
	 << endl;
  }
  return;
}

//////////////////////////////////////////////////
void HisToKomp::FlagAsBackground()
{
  // Flag as background the last file
  FlagAsBackground(GetNFiles()-1);
  return;
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
  bool PrintAverage = 0;  // print the mean of the histogram
  bool PrintMeanAndRMS = 0; 
  bool PrintEntries=0;
  bool PrintFunLegend=0;  // aquesta s'activa sols si el histograma te una funcio
  bool DrawFunOnTop = 1; // if histograms are fitted, the function is plotted
  bool LogyScale = 0;
  bool Normalize = 1;
  bool PrintTitle = 1;   // print the title of the histogram
  bool PrintLabel = 0;   // use the label of the file as title of the histogram
  bool SeriesPlot = 0;  
  bool SeriesPlotEntries = 0;  
  bool SeriesPlotSigma = 0;
  bool ThereIsBackground = kFALSE; // to check if some file has been defined as background

  // primer processem les opcions 
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    DrawOptions->ToLower();
    if (1 == 0) cout << " Pintant histos amb opcions: " 
		     << DrawOptions->Data() 
		     << endl;
    DrawOnSameFrame = DrawOptions->Contains("same");
    DrawOnNewFrame = DrawOptions->Contains("new");
    PrintLegendOnTop = !DrawOptions->Contains("nolegend");
    PrintAverage = DrawOptions->Contains("printaverage");
    PrintMeanAndRMS = DrawOptions->Contains("printmean");
    PrintEntries = DrawOptions->Contains("printentries");
    LogyScale = DrawOptions->Contains("logy");
    Normalize = !DrawOptions->Contains("nonormalize");
    PrintTitle = !DrawOptions->Contains("notitle");
    PrintLabel = DrawOptions->Contains("printlabelastitle");
    DrawFunOnTop = !(DrawOptions->Contains("nodrawfun")|DrawOptions->Contains("nofun"));
    SeriesPlot = DrawOptions->Contains("seriesplot");
    SeriesPlotEntries = DrawOptions->Contains("entries");    
    SeriesPlotEntries = DrawOptions->Contains("entries");    
    SeriesPlotSigma = DrawOptions->Contains("sigma");    
  }

  // ara continuem
  char nameh[80];
  char chopt[8];
  
  // Si volem pintar una serie crida a la funcio membre que ho fa
  if (SeriesPlot) {
    char Sxopt[80];
    sprintf(Sxopt,"%s","labels,nolegend,line");
    if (SeriesPlotEntries) sprintf(Sxopt,"%s","labels,nolegend,line,entries");
    if (SeriesPlotSigma) sprintf(Sxopt,"%s","labels,nolegend,line,sigma");

    DrawSeries(hname,Sxopt); 
    return;
  }

  // Pilla els histogrames a pintar
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  for (int i=0; i < NFiles; i++) {
    HD[i] = (TH1F*)HisFile[i]->Get(nameh)->Clone(); 
  }

  // check if some file is flagged as background
  for (int i=0; i < NFiles; i++) {
    if (HistoIsBackground[i]) {
      ThereIsBackground = kTRUE;
    } 
  }
  if (ThereIsBackground) HandleBackgroundHistograms();

  // defineix el marc
  float HYMax = MarcMaximY();

  // i ara pinta els histogrames
  for (int i=0; i < NFiles; i++) {
    sprintf(chopt,"");
    if (i == 0) {
      if (DrawOnSameFrame) sprintf(chopt,"same"); 
    }
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
    if (HistoRef[i]!= NULL) HD[i]->SetLineWidth(HistoRef[i]->GetLineWidth());
    if (HistoRef[i]!= NULL) {
      HD[i]->SetFillStyle(HistoRef[i]->GetFillStyle());
      HD[i]->SetFillColor(HistoRef[i]->GetLineColor());
    } 
    if (HistoRef[i]!= NULL) {
      if (HistoRef[i]->GetMarkerStyle() !=1) {
	HD[i]->SetMarkerStyle(HistoRef[i]->GetMarkerStyle());
	HD[i]->SetMarkerSize(0.8);
	HD[i]->SetMarkerColor(HistoRef[i]->GetLineColor());
	sprintf(chopt,"%s %s",chopt,"pe");
      }
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
      

      // Normalize the errors with the luminosity when drawing markers
      if (HistoRef[i]!= NULL) {
	if (HistoRef[i]->GetMarkerStyle() !=1) {
	  Double_t NFactor = HistoLumi[i];
	  
	  Int_t Nbin = HD[i]->GetNbinsX();
	  cout << " NFactor = " << NFactor << " BinX " << Nbin << endl;
	  for (Int_t bin=1; bin<= Nbin; bin++) { 
	    HD[i]->SetBinError(bin,(HD[i]->GetBinError(bin))*NFactor);
	  }
	}
      }
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

    // check if title is wanted
    if (!PrintTitle) HD[i]->SetTitle();

    // check if the label has to be printed as title    
    if (PrintLabel) {
      char NewTitle[80];
      sprintf(NewTitle,"%s",FileLabel[i]->Data());
      HD[i]->SetTitle(NewTitle);  
    }

    // i ara si cal es pinta
    if (DrawHistoOnOff[i]) {
      HD[i]->DrawCopy(chopt);
      if (HistoHasFun[i] && DrawFunOnTop) HistoFun[i]->Draw("same");
    }
  }
   
  ////////////////////////////////
  // legend Boxes
  ResetLegendBoxesY();

  // llegenda dels histogrames
  if (PrintLegendOnTop) {
    PrintHistosLegend();
  }

  //  llegenda dels ajusts
  if (PrintFunLegend && PrintLegendOnTop && DrawFunOnTop) {
    PrintFunctionLegend();
  }

  // Print Average
  if (PrintAverage) {
    PrintAverageLegend();
  }

  // Print mean and RMS
  if (PrintMeanAndRMS) {
    PrintMeanAndRMSLegend();
  }
  // 
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
  bool SeriesMean = 1;
  bool SeriesSigma = 0;
  bool SeriesEntries = 0;

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
    SeriesMean= (DrawOptions->Contains("mean") || SeriesMean);
    SeriesSigma= (DrawOptions->Contains("sigma"));
    SeriesEntries= (DrawOptions->Contains("entries") && !DrawOptions->Contains("mean"));
  }

  // in case of conflict 
  if (SeriesEntries) {
    SeriesMean = 0;
    SeriesSigma = 0;
  }
  
  if (1==0) {
    cout << " --> Series plot: xopt = " << DrawOptions->Data() << endl;
    if (SeriesMean && SeriesSigma) {
      cout << " --> Series plot: mean and sigma" << endl;
    }
    if (SeriesMean && !SeriesSigma) {
      cout << " --> Series plot: mean" << endl;
    }
    if (SeriesEntries) {
      cout << " --> Series plot: entries" << endl;
    }
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
    HSerie->GetXaxis()->SetBinLabel(i+1,FileLabel[i]->Data());
  }  
  // the Y-axis label corresponds with the old X-axis one
  if (SeriesMean || SeriesSigma) HSerie->SetYTitle(HD[0]->GetXaxis()->GetTitle());
  if (SeriesEntries) HSerie->SetYTitle("Entries");

  // i ara omplim els histos de la serie amb els valors
  float valor;
  float vsigma;
  for (int i=0; i < NFiles; i++) {
    // value
    if (SeriesMean) valor = HD[i]->GetMean();
    if (SeriesSigma & !SeriesMean) valor = HD[i]->GetRMS();
    if (SeriesEntries) {
      valor = HD[i]->GetEntries();
      if (HistoHasLumi[i] == 1) {
	valor = valor * HistoLumi[i];
      }
    }
    HSerie->SetBinContent(i+1,valor);

    // error
    if (SeriesMean & SeriesSigma) { 
      vsigma = HD[i]->GetRMS();
      HSerie->SetBinError(i+1,vsigma);
    }
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
  
  ////////////////////////////////
  // legend Boxes
  ResetLegendBoxesY();

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
    if (HD2D[i] == NULL) {
      cout << " ** HisToKomp ** File " << i << " has no histogram " << nameh << endl;
      return;
    }
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
  Bool_t SubstractHistos = false;

  // primer processem les opcions al pintar
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    if (1==0) {
      cout << " Pintant histos amb opcions: " 
	   << DrawOptions->Data() 
	   << endl;
    }
    SubstractHistos = DrawOptions->Contains("substra");
  }
  
  // if substraction is wanted
  if (SubstractHistos) {
    Draw2DSubstraction(hname,xopt);
    return;
  }

  // ara continuem
  char nameh[80];
  char chopt[8];
  
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  cout << " ** Draw2D ** nameh = " << nameh << endl;
  
  // Pilla els histogrames a pintar
  for (int i=0; i < NFiles; i++) {
    if (i == 0) sprintf(chopt,"");
    HD2D[i] = (TH2F*)HisFile[i]->Get(nameh); 
  }

  cout << " ** Draw2D ** histos captured " << nameh << endl;

  // i ara pinta els histogrames
  for (int i=0; i < NFiles; i++) {
    if (i == 0) {
      sprintf(chopt,"box");
    }
    if (i>0) sprintf(chopt,"boxsame");

    if (HistoRef[i]!= NULL) HD2D[i]->SetLineColor(HistoRef[i]->GetLineColor());
    HD2D[i]->DrawCopy(chopt);
  }

  // llegenda dels histogrames
  //PrintHistosLegend();
    
  return;
}

////////////////////////////////////////////////
void HisToKomp::Draw2DSubstraction(const char *hname, const char *xopt)
{
  // primer processem les opcions al pintar
  if (xopt != NULL) {
    DrawOptions = new TString(xopt);
    // no options yet
  }

  // minimum 2 files are needed
  if (NFiles<2) return;

  // keep going...
  char nameh[80];
  char chopt[8];
  
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  if (1==0) cout << " ** Draw2DSubstraction ** importing histograms nameh= " << nameh << endl;
  
  // Pilla els histogrames a pintar
  for (int i=0; i < NFiles; i++) {
    if (i == 0) sprintf(chopt,"");
    HD2D[i] = (TH2F*)HisFile[i]->Get(nameh)->Clone(); 
  }
  if (1==0)cout << " ** Draw2DSubstraction ** histograms nameh= " << nameh << " captured " << endl;

  Int_t ixCurrent = GetNFiles()-1;
  Int_t ixPrevious = ixCurrent - 1; // previous one

  if (1==0) {
    cout << " ** Draw2DSubstraction ** current histograms from file " << ixCurrent << endl
	 << "                          compared to histograms from " << ixPrevious << endl;
  }

  HD2D[ixPrevious]->Scale(-1);
  HD2D[ixCurrent]->Add(HD2D[ixPrevious]);

  // before drawing lets symmetrisize the Z axis
  Float_t zMax = HD2D[ixCurrent]->GetBinContent(HD2D[ixCurrent]->GetMaximumBin());
  Float_t zMin = HD2D[ixCurrent]->GetBinContent(HD2D[ixCurrent]->GetMinimumBin());
  if (zMax>fabs(zMin)) zMin = -zMax;
  if (zMax<fabs(zMin)) zMax = fabs(zMin);
  //
  if (UsingUserRange) {
    zMin = UserRangeMin;  
    zMax = UserRangeMax;
  }

  if (1==0) cout << " ** Draw2DSubstraction ** Range " << zMin << " --> " << zMax << endl; 
  HD2D[ixCurrent]->GetZaxis()->SetRangeUser(zMin,zMax);
  
  // draw
  HD2D[ixCurrent]->Draw("colz");

  // moving the color scale
  gPad->Update();
  TPaletteAxis *palette = (TPaletteAxis*)HD2D[ixCurrent]->GetListOfFunctions()->FindObject("palette");
  palette->SetX1NDC(0.906);
  palette->SetX2NDC(0.930);
  palette->SetY1NDC(0.1);
  palette->SetY2NDC(0.880);
  gPad->Modified();
  //
  
  return;
}
////////////////////////////////////////////////
void HisToKomp::PrintHistosLegend()
{
  v_HiLeg_lx = 0.76;
  v_HiLeg_ly = 0.82;
  v_HiLeg_ux = 0.99;
  v_HiLeg_uy = 0.94;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_HiLeg_uy = GetLegendLowerY()-GetLegendYGap();

  Int_t MaxLength = 0;
  for (int i=0; i < NFiles; i++) {
    if (FileLabel[i]->Sizeof()> MaxLength) MaxLength = FileLabel[i]->Sizeof(); 
  }  

  deltaX = 0.17+0.001*NFiles+0.008*MaxLength; 
  deltaY = 0.07+0.024*NFiles; 
  
  v_HiLeg_lx = v_HiLeg_ux - deltaX;   
  v_HiLeg_ly = v_HiLeg_uy - deltaY;

  if (v_HiLeg_lx < 0.60) v_HiLeg_lx = 0.60;

  TLegend *Rotllo = new TLegend(v_HiLeg_lx, v_HiLeg_ly, v_HiLeg_ux, v_HiLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char LegendStyle[1];
  for (int i=0; i < NFiles; i++) {
    sprintf(LegendStyle,"%s","l");
    if (GetFillStyle(i) !=0) sprintf(LegendStyle,"%s","f");
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],FileLabel[i]->Data(), LegendStyle);
  }
  Rotllo->Draw();

  SetLegendLowerY(v_HiLeg_ly);
}

////////////////////////////////////////////////
void HisToKomp::PrintFunctionLegend()
{
  float v_FuLeg_lx = 0.57;
  float v_FuLeg_ly = 0.82;
  float v_FuLeg_ux = 0.99;
  float v_FuLeg_uy = 0.94;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_FuLeg_uy = GetLegendLowerY() - GetLegendYGap();

  deltaX = 0.36+0.017*NFiles; 
  deltaY = 0.07+0.026*NFiles; 

  v_FuLeg_lx = v_FuLeg_ux - deltaX;
  v_FuLeg_ly = v_FuLeg_uy - deltaY;
  
  if (v_FuLeg_lx < 0.65) v_FuLeg_lx = 0.65;

  TLegend *FunRotllo = new TLegend(v_FuLeg_lx, v_FuLeg_ly, v_FuLeg_ux, v_FuLeg_uy);    
  
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
	double Weight1 = Heigh1/Sigma1;
	double Weight2 = Heigh2/Sigma2;
      
	FitMean =  (Mean1*Weight1 + Mean2*Weight2 ) / (Weight1 + Weight2);
	FitSigma = sqrt( (Sigma1*Sigma1*Weight1*Weight1 + Sigma2*Sigma2*Weight2*Weight2) / (Weight1*Weight1 + Weight2*Weight2) );
	
      }
      // si coneguem la funcio preparem el text
      if (KnownFunc) {
	sprintf(QueDir,"#mu = %4.3f   #sigma = %4.3f ", FitMean, FitSigma);
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
  return;
}

////////////////////////////////////////////////
void HisToKomp::PrintAverageLegend()
{
  float v_AvLeg_lx = 0.66;
  float v_AvLeg_ly = 0.82;
  float v_AvLeg_ux = 0.99;
  float v_AvLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_AvLeg_uy = GetLegendLowerY()-GetLegendYGap();

  deltaX = 0.20+0.015*NFiles; 
  deltaY = 0.08+0.024*NFiles; 

  v_AvLeg_lx = v_AvLeg_ux - deltaX;   
  v_AvLeg_ly = v_AvLeg_uy - deltaY;

  if (v_AvLeg_lx < 0.65) v_AvLeg_lx = 0.65;
  
  TLegend *Rotllo = new TLegend(v_AvLeg_lx, v_AvLeg_ly, v_AvLeg_ux, v_AvLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char QueDir[80];
  char LegendStyle[1];
  for (int i=0; i < NFiles; i++) {
    sprintf(QueDir,"mean = %5.2f ", HD[i]->GetMean());
    sprintf(LegendStyle,"%s","l");
    if (GetFillStyle(i) !=0) sprintf(LegendStyle,"%s","f");
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],QueDir, LegendStyle);
  }
  Rotllo->Draw();
  SetLegendLowerY(v_AvLeg_ly);
}

////////////////////////////////////////////////
void HisToKomp::PrintMeanAndRMSLegend()
{
  float v_MeLeg_lx = 0.57;
  float v_MeLeg_ly = 0.82;
  float v_MeLeg_ux = 0.99;
  float v_MeLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_MeLeg_uy = GetLegendLowerY()-GetLegendYGap();

  deltaX = 0.25+0.015*NFiles; 
  deltaY = 0.10+0.020*NFiles; 

  v_MeLeg_lx = v_MeLeg_ux - deltaX;   
  v_MeLeg_ly = v_MeLeg_uy - deltaY;

  if (v_MeLeg_lx < 0.65) v_MeLeg_lx = 0.65;

  v_MeLeg_ly = v_MeLeg_uy - deltaY;
  
  TLegend *Rotllo = new TLegend(v_MeLeg_lx, v_MeLeg_ly, v_MeLeg_ux, v_MeLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char QueDir[80];
  char LegendStyle[1];
  for (int i=0; i < NFiles; i++) {
    sprintf(QueDir,"#mu =%5.3f rms=%5.3f", HD[i]->GetMean(), HD[i]->GetRMS());
    sprintf(LegendStyle,"%s","l");
    if (GetFillStyle(i) !=0) sprintf(LegendStyle,"%s","f");
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],QueDir, LegendStyle);
  }
  Rotllo->Draw();
  SetLegendLowerY(v_MeLeg_ly);
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
  float v_EnLeg_lx = 0.76;
  float v_EnLeg_ly = 0.82;
  float v_EnLeg_ux = 0.99;
  float v_EnLeg_uy = 0.99;

  float deltaX = 0.22;
  float deltaY = 0.12;

  v_EnLeg_uy = GetLegendLowerY()-GetLegendYGap();

  deltaX = 0.24+0.015*NFiles; 
  deltaY = 0.07+0.024*NFiles; 

  v_EnLeg_lx = v_EnLeg_ux - deltaX;   
  v_EnLeg_ly = v_EnLeg_uy - deltaY;
  
  if (v_EnLeg_lx < 0.65) v_EnLeg_lx = 0.65;

  TLegend *Rotllo = new TLegend(v_EnLeg_lx, v_EnLeg_ly, v_EnLeg_ux, v_EnLeg_uy);
  Rotllo->SetFillStyle(1001);
  Rotllo->SetFillColor(10);
  char QueDir[80];
  for (int i=0; i < NFiles; i++) {
    float entry = HD[i]->GetEntries();
    if (HistoHasLumi[i] == 1) {
      entry = entry * HistoLumi[i];
    }

    
    if (strlen(elabel)) { 
      sprintf(QueDir,"%5.2f %s", entry, elabel);
    } 
    else {
      sprintf(QueDir,"%5.2f %s", entry, EntriesLabel);
    }
    //sprintf(QueDir,"%5.0f event/pb^{-1} ", entry);
    if (FileLabel[i] != NULL) Rotllo->AddEntry(HD[i],QueDir, "l");
  }
  Rotllo->Draw();

}
//////////////////////////////////////////////////
void HisToKomp::InitGrafStyle()
{
  Float_t isize = 0.04;
  Int_t font = 42;

  //  gStyle->SetOptStat(111110);

  gStyle->SetCanvasColor(kWhite);  
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(0);
  
  gStyle->SetLabelFont(font,"X");
  gStyle->SetLabelFont(font,"Y");
  gStyle->SetLabelFont(font,"Z");
  gStyle->SetLabelSize(isize,"x");
  gStyle->SetLabelSize(isize,"y");
  gStyle->SetLabelSize(isize-0.01,"z");
 
  gStyle->SetTitleFont(font,"x");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetTitleFont(font,"z");
  gStyle->SetTitleSize(isize,"x");  
  gStyle->SetTitleSize(isize,"y");
  gStyle->SetTitleSize(isize,"z");
  gStyle->SetTitleYOffset(1.8);

  gStyle->SetOptFit(0);

  //  gStyle->SetStatH(0.15);

  gStyle->SetStatStyle(0); // to view data behind box too!
  gStyle->SetStatBorderSize(1);   // remove shade from stat box
  gStyle->SetLegendBorderSize(1);   // remove shade from stat box
  gStyle->SetTitleBorderSize(1);   // remove shade from stat box
  // gStyle->SetTitleColor(kWhite);

  gStyle->SetStripDecimals(false);
  TGaxis::SetMaxDigits(4);

  gStyle->SetTextFont(font);

  gStyle->SetFrameBorderMode(0);

  gStyle->SetPalette(1);

  gStyle->SetPaintTextFormat("g");

  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.10);
  gStyle->SetPadTopMargin(0.12);
  
  // style
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
  gStyle->SetOptFit(0);
  gStyle->SetOptStat(10);
  gStyle->SetOptStat(0); // Ben
  gStyle->SetStatFont(font);
  gStyle->SetStatFontSize(isize);
  gStyle->SetStatColor(kWhite);
  gStyle->SetStatW(0.22);
  gStyle->SetStatX(0.99);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderMode(0);
  
  gROOT->ForceStyle();
}

//////////////////////////////////////////////////
float HisToKomp::MarcMaximY()
{
  float ValorBo = -999999.; // comencem d'un valor artificialment molt baix
  float ValorTest=0.;

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
void HisToKomp::SetLuminosity(const char* hname, int LumiChan )
{
  char nameh[80];
  double FileLuminosity;

  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  cout << " ** SetLuminosity ** nameh = " << nameh << endl;

  for (int i=0; i < NFiles; i++) {
    HLumi[i] = (TH1F*)HisFile[i]->Get(nameh); 
    if (LumiChan==0) {
      // This case, the total entries of the histogram are used
      FileLuminosity = 1./(HLumi[i]->GetEntries());
      // cout << " File " << i << " luminosity " << FileLuminosity << endl;
    }
    if (LumiChan!=0) {
      // This case, the contents of a given channel are used
      FileLuminosity = 1./(HLumi[i]->GetBinContent(LumiChan));
      //cout << " File " << i << " luminosity " << FileLuminosity << endl;
    }
    SetLuminosity(i,FileLuminosity);
  }
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
//////////////////////////////////////////////////
float HisToKomp::ResetLegendBoxesY()
{
  SetLegendLowerY(0.96);
}
//////////////////////////////////////////////////
float HisToKomp::GetLegendYGap()
{
  return 0.01;
}

//////////////////////////////////////////////////
bool HisToKomp::HExists(const char* hname)
{
  bool Status = true;
  for (int i=0; i < NFiles; i++) {
    TH1F* Check = NULL;
    Check = (TH1F*)HisFile[i]->FindObjectAny(hname); 
    if (Check == NULL) {
      Status = false;
      // cout << " ** NO histogram " << hname << " in file " << i << endl;
    }		
  }
  return Status;
}

//////////////////////////////////////////////////
void HisToKomp::PrintStatsTable(const char* hname)
{
  Int_t HistoEntries[MaxNFiles];
  Float_t EquivEntries[MaxNFiles];
  char nameh[80];
  sprintf(nameh,"%s/%s",HisDirec->Data(),hname);
  for (int i=0; i < NFiles; i++) {
    // HD[i] = (TH1F*)HisFile[i]->Get(nameh)->Clone();
    HistoEntries[i] = (int) ((TH1F*)HisFile[i]->Get(nameh)->Clone())->GetEntries();
    cout << " | " << FileLabel[i]->Data() << " |  " << HistoEntries[i]; 
    if (HistoHasLumi[i]) {
      EquivEntries[i] = HistoEntries[i] * HistoLumi[i];
      cout << "   x Lumi = " << EquivEntries[i];
    }
    cout << endl; 
  }    
  return;
}

//////////////////////////////////////////////////
void HisToKomp::SetUserRange(const float UserMin, const float UserMax)
{
  if (UserMin<UserMax) {
    UsingUserRange = true;
    UserRangeMin = UserMin;
    UserRangeMax = UserMax;
  }
}
//////////////////////////////////////////////////
void HisToKomp::UserRangeOff()
{
  UsingUserRange = false;  
  return;
}

//////////////////////////////////////////////////
void HisToKomp::SetOptTitle(const bool optitle)
{
  gStyle->SetOptTitle(optitle);
  return;
}

//////////////////////////////////////////////////
void HisToKomp::SetYTitle(const char* TitleY)
{
  if (HisYTitle != NULL) delete HisYTitle;
  if (TitleY !=NULL) HisYTitle = new TString(TitleY);
  return;
}

//////////////////////////////////////////////////
void HisToKomp::HandleBackgroundHistograms()
{
  Int_t Previous=-1;
  // check if some file is flagged as background
  for (int i=0; i < NFiles; i++) {
    if (HistoIsBackground[i] && Previous!=-1) {
      // first histo is kept as it was
      // subsequent histos are added
      HD[i]->Add(HD[Previous]);
    } 
    if (HistoIsBackground[i]) {
      Previous = i;
    }
  }
  return;
}
