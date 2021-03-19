/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################################
//## AUTHOR:      Christophe Le Maner (lemaner@cern.ch)
//## FILE:        CaloClusterVecMonitoring.C
//## MODIFIED:    2006/10/02
//## DESCRIPTION: Build GUI for histos produced by CaloClusterVecMonTool.
//##              Based on framework developped by Tayfun Ince.
//#####################################################################

#include <TROOT.h>
#include <RQ_OBJECT.h>
#include <TGClient.h> 
#include <TGFrame.h>
#include <TGCanvas.h>
#include <TGButton.h> 
#include <TString.h>
using namespace std;

//#####################################################################
//#####################################################################
// Class header
class CaloClusterVecMon
{

  // cppcheck-suppress unknownMacro
  RQ_OBJECT("CaloClusterVecMon") // necessary if not inherite from ROOT class

 protected:
  enum EPopupMenuEntry
    { // enumerators for signal/slot mechanism of main menu entries
      M_PS_OPEN,
      M_PS_CLOSE,
      M_PS_PRINT,
      M_PS_DELETE,

      M_CALOCLUSTERVEC_SUMMARY,
      M_CALOCLUSTERVEC_ENERGY,
      M_CALOCLUSTERVEC_ETA,
      M_CALOCLUSTERVEC_PHI,
      M_CALOCLUSTERVEC_ETAPHI,
      M_CALOCLUSTERVEC_SUMEVSETAPHI,
      M_CALOCLUSTERVEC_SUMEVSETA,
      M_CALOCLUSTERVEC_SUMEVSPHI,
    };

 private:
  TGMainFrame          *fMain;
  TGDockableFrame      *fMenuDock;
  TGCanvas             *fGCanvas;
  TGCompositeFrame     *fContainer;
  TRootEmbeddedCanvas  *fECanvas[15];
  TGMenuBar            *fMenuBar;
  TGPopupMenu          *fMenuSummary, *fMenuCaloCluster;
  TGPopupMenu          *fMenuPS;
  TGHorizontalFrame    *fHFrame;
  TGComboBox           *fComboEcut, *fComboEtaSlice;
  TGTextButton         *fDrawSingle, *fSave, *fPrintThis;
  TGStatusBar          *fStatusBar;
  TCanvas              *fCanvas;
  TH1                  *m_h1d, *m_href;
  TH2                  *m_h2d;
  TPostScript          *m_psFile;

  TString              m_fileBaseName;
  TString              m_dirName, m_histoNameBase1;
  TString              m_dirCaloClusterVec, m_dirEnergy, m_dirEta, m_dirPhi, m_dirEtaPhi, m_dirSumEvsEtaPhi, m_dirSumEvsEta, m_dirSumEvsPhi, m_dirSummary;
  TString              m_configOption, m_prevConfigOption, m_drawOption;
  TString              m_psFileName;

  int                  m_canvasW, m_canvasH;

 public:
  CaloClusterVecMon(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~CaloClusterVecMon();
  CaloClusterVecMon (const CaloClusterVecMon&) = delete;
  CaloClusterVecMon& operator= (const CaloClusterVecMon&) = delete;

  void CloseMainWindow();

  void ConfigureMainMenu();
  void ConfigureComboFrame();
  void HandleMainMenu(Int_t id);
  void FillCombo();
  void HandleCombo();

  void SetNCanvas(int ncanvas);
  void DrawSingle();
  void DrawSummaryLegend();
  void DrawSummary();

  void Save();
  void PrintThis();
};

//#####################################################################
//#####################################################################
// Class constructor
CaloClusterVecMon::CaloClusterVecMon(const TGWindow *p, UInt_t w, UInt_t h)
  : fMain (nullptr),
    fMenuDock (nullptr),
    fGCanvas (nullptr),
    fContainer (nullptr),
    fECanvas (),
    fMenuBar (nullptr),
    fMenuSummary (nullptr),
    fMenuCaloCluster (nullptr),
    fMenuPS (nullptr),
    fHFrame (nullptr),
    fComboEcut (nullptr),
    fComboEtaSlice (nullptr),
    fDrawSingle (nullptr),
    fSave (nullptr),
    fPrintThis (nullptr),
    fStatusBar (nullptr),
    fCanvas (nullptr),
    m_h1d (nullptr),
    m_href (nullptr),
    m_h2d (nullptr),
    m_psFile (nullptr),

    m_fileBaseName (gSystem->BaseName(g_rootFile->GetName())),
    m_dirCaloClusterVec ("/CaloClusterVecMon/"),
    m_dirEnergy ("Energy"),
    m_dirEta ("Eta"),
    m_dirPhi ("Phi"),
    m_dirEtaPhi ("EtaPhi"),
    m_dirSumEvsEtaPhi ("SumEvsEtaPhi"),
    m_dirSumEvsEta ("SumEvsEta"),
    m_dirSumEvsPhi ("SumEvsPhi"),
    m_dirSummary  ("Summary"),
    m_canvasW (720),
    m_canvasH (540)
{
  // global style options
  gStyle->SetOptStat("emr");
  gStyle->SetPaperSize(TStyle::kA4);
  gStyle->SetStatX(0.99);
  gStyle->SetStatY(0.99);

  // start building gui
  // create a top level window
  fMain = new TGMainFrame(p,w,h);

  // connect(signal, receiver_class, receiver, slot)
  fMain->Connect("CloseWindow()", "CaloClusterVecMon", this, "CloseMainWindow()");

  // add dockable frame to the main window
  fMenuDock = new TGDockableFrame(fMain);
  fMain->AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));
  fMenuDock->SetWindowName("Calorimeter Menu");

  // create "Postscript" menu
  fMenuPS = new TGPopupMenu(gClient->GetRoot());
  fMenuPS->AddEntry("Open", M_PS_OPEN);
  fMenuPS->AddEntry("Close", M_PS_CLOSE);
  fMenuPS->AddSeparator();
  fMenuPS->AddEntry("Print", M_PS_PRINT);
  fMenuPS->AddSeparator();
  fMenuPS->AddEntry("Delete", M_PS_DELETE);
  fMenuPS->Connect("Activated(Int_t)", "CaloClusterVecMon", this, "HandleMainMenu(Int_t)");

  // create "Summary" menu
  fMenuSummary = new TGPopupMenu(gClient->GetRoot());
  fMenuSummary->AddEntry("Summary", M_CALOCLUSTERVEC_SUMMARY);
  fMenuSummary->Connect("Activated(Int_t)", "CaloClusterVecMon", this, "HandleMainMenu(Int_t)");

  // create "CaloClusterVec" menu
  fMenuCaloCluster = new TGPopupMenu(gClient->GetRoot());
  fMenuCaloCluster->AddEntry("Energy", M_CALOCLUSTERVEC_ENERGY);
  fMenuCaloCluster->AddSeparator();
  fMenuCaloCluster->AddEntry("Eta", M_CALOCLUSTERVEC_ETA);
  fMenuCaloCluster->AddEntry("Phi", M_CALOCLUSTERVEC_PHI);
  fMenuCaloCluster->AddEntry("EtaPhi", M_CALOCLUSTERVEC_ETAPHI);
  fMenuCaloCluster->AddSeparator();
  fMenuCaloCluster->AddEntry("SumEvsEtaPhi", M_CALOCLUSTERVEC_SUMEVSETAPHI);
  fMenuCaloCluster->AddEntry("SumEvsEta", M_CALOCLUSTERVEC_SUMEVSETA);
  fMenuCaloCluster->AddEntry("SumEvsPhi", M_CALOCLUSTERVEC_SUMEVSPHI);
  fMenuCaloCluster->Connect("Activated(Int_t)", "CaloClusterVecMon", this, "HandleMainMenu(Int_t)");

  // disable menu entries for user mode
  //if(g_expertMode == false){
  //fMenuSummary->DisableEntry(M_CALOCLUSTERVEC_SUMMARY);
  // }

  // disable menu entries for directories don't exist in root file
  ConfigureMainMenu();

  // add menu bar with popup menus to the dockable frame
  fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
  fMenuDock->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 2, 2);
  fMenuBar->AddPopup("&PostScript", fMenuPS, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Summary", fMenuSummary, fMenuBarItemLayout);
  fMenuBar->AddPopup("Calo&ClusterVec", fMenuCaloCluster, fMenuBarItemLayout);

  // add a canvas widget with a viewport and scroll bars to the main window
  fGCanvas = new TGCanvas(fMain, 740, 544);
  fMain->AddFrame(fGCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,2,2,0,2));
  // a composite frame (i.e. container) for embedded canvases
  fContainer = new TGCompositeFrame(fGCanvas->GetViewPort());
  fGCanvas->SetContainer(fContainer);
  // create embedded canvas and place into the container
  fECanvas[0] = new TRootEmbeddedCanvas(0,fContainer, m_canvasW, m_canvasH);
  fContainer->AddFrame(fECanvas[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  // add a horizontal frame for combo boxes and text buttons to the main window
  fHFrame = new TGHorizontalFrame(fMain, 50, 10);
  fMain->AddFrame(fHFrame, new TGLayoutHints(kLHintsCenterX,2,2,0,2));
  TGLayoutHints *hframeLayout = new TGLayoutHints(kLHintsExpandY,4,4,2,2);
  // combo boxes
  fComboEcut = new TGComboBox(fHFrame);
  fComboEtaSlice = new TGComboBox(fHFrame);
  fComboEcut->Resize(80,20);
  fComboEtaSlice->Resize(80,20);
  fHFrame->AddFrame(fComboEcut, hframeLayout);
  fHFrame->AddFrame(fComboEtaSlice, hframeLayout);
  // text buttons
  fDrawSingle = new TGTextButton(fHFrame,"");
  fDrawSingle->Connect("Clicked()", "CaloClusterVecMon", this, "DrawSingle()");
  fHFrame->AddFrame(fDrawSingle, hframeLayout);
  fSave = new TGTextButton(fHFrame," Save ");
  fSave->Connect("Clicked()", "CaloClusterVecMon", this, "Save()");
  fHFrame->AddFrame(fSave, hframeLayout);
  fPrintThis = new TGTextButton(fHFrame," PrintThis ");
  fPrintThis->Connect("Clicked()", "CaloClusterVecMon", this, "PrintThis()");
  fHFrame->AddFrame(fPrintThis, hframeLayout);

  // add status bar for messages to the main window
  fStatusBar = new TGStatusBar(fMain, 50, 10, kHorizontalFrame);
  fMain->AddFrame(fStatusBar, new TGLayoutHints(kLHintsExpandX, 2, 2, 0, 2));
  fStatusBar->SetText("Welcome to Calorimeter monitoring. FILE: " + m_fileBaseName + " is open.");

  // set main window name and position
  fMain->SetWindowName("Calorimeter Monitoring >>> " + m_fileBaseName); 
  fMain->MapSubwindows(); 
  fMain->Resize(fMain->GetDefaultSize());
  fMain->MoveResize(228,0);
  fMain->SetWMPosition(228,0);

  // hide some of the combo boxes and buttons for now
  fHFrame->HideFrame(fComboEcut);
  fHFrame->HideFrame(fComboEtaSlice);
  fHFrame->HideFrame(fDrawSingle);

  // initialize layout of the main window and map the GUI
  fMain->Layout();
  fMain->MapWindow();
}

//#####################################################################
// Class destructor
CaloClusterVecMon::~CaloClusterVecMon()
{
  // fMain->Cleanup() can't delete sub-sub level widgets
  // so fContainer->Cleanup() is a must here
  fContainer->Cleanup();
  fMain->Cleanup();
}

//#####################################################################
// Close main window upon click on "X"
void CaloClusterVecMon::CloseMainWindow()
{
  delete this;
}

//#####################################################################
// Configure menu bar to disable entries 
// for directories don't exist in the root file
void CaloClusterVecMon::ConfigureMainMenu()
{
  TGMenuEntry *entry;
  g_rootFile->cd();
  TString dir = m_dirCaloClusterVec;
  if(gDirectory->Get(dir.Remove(3,'/')) == 0){
    TIter next(fMenuCaloCluster->GetListOfEntries());
    while(entry = (TGMenuEntry*)next()){
      if(entry->GetType() == kMenuEntry) 
	fMenuCaloCluster->DisableEntry(entry->GetEntryId());
    }
    fMenuSummary->DisableEntry(M_CALOCLUSTERVEC_SUMMARY);
  } else{
    g_rootFile->cd(m_dirCaloClusterVec);
    if(gDirectory->Get(m_dirSummary) == 0)     fMenuSummary->DisableEntry(M_CALOCLUSTERVEC_SUMMARY);
    if(gDirectory->Get(m_dirEnergy) == 0)      fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_ENERGY);
    if(gDirectory->Get(m_dirEta) == 0)         fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_ETA);
    if(gDirectory->Get(m_dirPhi) == 0)         fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_PHI);
    if(gDirectory->Get(m_dirEtaPhi) == 0)      fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_ETAPHI);
    if(gDirectory->Get(m_dirSumEvsEtaPhi) == 0)fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_SUMEVSETAPHI);
    if(gDirectory->Get(m_dirSumEvsEta) == 0)   fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_SUMEVSETA);
    if(gDirectory->Get(m_dirSumEvsPhi) == 0)   fMenuCaloCluster->DisableEntry(M_CALOCLUSTERVEC_SUMEVSPHI);
  }
}

//#####################################################################
// Configure horizontal frame to hide and show necessary
// combo boxes and buttons given the context 
void CaloClusterVecMon::ConfigureComboFrame()
{
  if(m_configOption == "SUMMARY"){
    if(fHFrame->IsVisible(fDrawSingle) == 1)    fHFrame->HideFrame(fDrawSingle);
    if(fHFrame->IsVisible(fComboEtaSlice) == 1) fHFrame->HideFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fComboEcut) == 1)     fHFrame->HideFrame(fComboEcut);
    //fDrawSingle->SetText(" Draw Summary ");
  }

  //######################
  else if(m_configOption == "ENERGY"){
    if(fHFrame->IsVisible(fComboEcut) == 1)     fHFrame->HideFrame(fComboEcut);
    if(fHFrame->IsVisible(fComboEtaSlice) == 0) fHFrame->ShowFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fDrawSingle) == 0)    fHFrame->ShowFrame(fDrawSingle);
    fDrawSingle->SetText(" Draw One ");
  }

  //######################
  else if(m_configOption == "ETA" || m_configOption == "PHI" || m_configOption == "ETAPHI" || m_configOption == "SUMEVSETAPHI" || m_configOption == "SUMEVSETA" || m_configOption == "SUMEVSPHI"){
    if(fHFrame->IsVisible(fComboEcut) == 0)     fHFrame->ShowFrame(fComboEcut);
    if(fHFrame->IsVisible(fComboEtaSlice) == 1) fHFrame->HideFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fDrawSingle) == 0)    fHFrame->ShowFrame(fDrawSingle);
    fDrawSingle->SetText(" Draw One ");
  }
  fMain->Layout();
}

//#####################################################################
// Fill combobox for layer monitored
void CaloClusterVecMon::FillCombo()
{
  fComboEcut->RemoveEntries(0,fComboEcut->GetNumberOfEntries());
  fComboEtaSlice->RemoveEntries(0,fComboEtaSlice->GetNumberOfEntries());
  
  //  cout << __LINE__ << endl;
  
  if (m_configOption == "ENERGY"){
    g_rootFile->cd();
    g_rootFile->cd(m_dirName);
    TKey *key;
    TString etaslice;
    TSortedList *etasliceList = new TSortedList();
    TIter next(gDirectory->GetListOfKeys());
    while(key = (TKey*)next()){
      TString name = key->GetName();
      etaslice = name(m_histoNameBase1.Length(),7);
      if(etasliceList->FindObject(etaslice))continue;
      etasliceList->Add(new TObjString(etaslice));
    }
    
    TIter next(etasliceList);
    int i=0;
    while(i<etasliceList->GetSize()){
      fComboEtaSlice->AddEntry(next()->GetName(), i++);
    }
    fComboEtaSlice->Select(0);
    
  }
  else {
    g_rootFile->cd();
    g_rootFile->cd(m_dirName);
    cout << "m_dirName = " << m_dirName << endl;
    TKey *key;
    TString ecut;
    TSortedList *ecutList = new TSortedList();
    TIter next(gDirectory->GetListOfKeys());
    while(key = (TKey*)next()){
      TString name = key->GetName();
      cout << "m_histoNameBase1 = " << m_histoNameBase1 << endl;
      ecut = name(m_histoNameBase1.Length(),5);
      cout << "ecut = " << ecut << endl;
      if(ecutList->FindObject(ecut))continue;
      ecutList->Add(new TObjString(ecut));
    }
    
    TIter next(ecutList);
    int i=0;
    while(i<ecutList->GetSize()){
      fComboEcut->AddEntry(next()->GetName(), i++);
    }
    fComboEcut->Select(0);
  }

  m_prevConfigOption = m_configOption;
}

//#####################################################################
// Fill combobox for layer monitored
void CaloClusterVecMon::HandleCombo()
{
  if(m_prevConfigOption == m_configOption)return;

  if (m_configOption == "ENERGY"){
    fComboEtaSlice->Select(0);
  }
  else {
    fComboEcut->Select(0);
  }
}

//#####################################################################
// Draw single histo
void CaloClusterVecMon::DrawSingle()
{
  //  gStyle->SetPalette(1,0); // pretty colors for 2d histo

  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);

  if(fContainer->GetList()->GetSize() != 1){
    fContainer->Cleanup();
    fECanvas[0] = new TRootEmbeddedCanvas(0,fContainer, m_canvasW, m_canvasH);
    fContainer->AddFrame(fECanvas[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    fMain->MapSubwindows();
  }
  fCanvas = fECanvas[0]->GetCanvas();

  TGTextLBEntry *tEntry;
  if (m_configOption == "ENERGY") {
    tEntry = (TGTextLBEntry*)fComboEtaSlice->GetSelectedEntry();
    //    TString etaslice = tEntry->GetText()->GetString();
    TString cut = tEntry->GetText()->GetString();
  }
  else {
    tEntry = (TGTextLBEntry*)fComboEcut->GetSelectedEntry();
    //    TString ecut = tEntry->GetText()->GetString();
    TString cut = tEntry->GetText()->GetString();
  }

  //######################
  fCanvas->Clear();
  fCanvas->Divide(1,1);
  //  if(m_configOption == "ENERGY") histoName = m_histoNameBase1 + etaslice;
  //  else histoName = m_histoNameBase1 + ecut;
  histoName = m_histoNameBase1 + cut;
  //cout << __LINE__ << endl;
  //cout << "m_configOption = " << m_configOption << " et histoName = " << histoName << endl;

  g_rootFile->cd(m_dirName);
  gStyle->SetOptStat(0);
  m_h1d = (TH1*)gDirectory->Get(histoName);
  fCanvas->cd(1);
  m_h1d->Draw(); g_DrawRunInfo();
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw all summary plots
void CaloClusterVecMon::DrawSummary()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  m_dirName = m_dirCaloClusterVec + m_dirSummary;
  TKey *key; TSortedList *histoList = new TSortedList();
  if(gDirectory->Get(m_dirName) != 0){
    g_rootFile->cd(m_dirName);
    TIter next(gDirectory->GetListOfKeys());
    while(key = (TKey*)next()){
      TString histoName = key->GetName();
      TString histoPath = m_dirName + "/" + histoName;
      histoList->Add(new TObjString(histoPath));
    }
  }
  g_rootFile->cd();
  
  int nHisto = histoList->GetSize();  // number of summary histos
  if(nHisto == 0)return;
  //  int nHistoPerCanvas = 2;   // set number of histo per canvas
  //  int nCanvas = (nHisto - 1)/nHistoPerCanvas + 1;   // number of canvases
  //  if(nHisto == 1){
  //   nHistoPerCanvas = 1; nCanvas = 1;
  //  }
  
  //  SetNCanvas(nCanvas);  // set number of canvases in the container
  SetNCanvas(1);
  
  fCanvas = fECanvas[0]->GetCanvas();
  fCanvas->Clear();
  fCanvas->Divide(2,1);
  TIter next(histoList);
  for(int i=0; i<nHisto; i++){
    //    int canvasIndex = i/nHistoPerCanvas; 
    //    fCanvas = fECanvas[canvasIndex]->GetCanvas();
    //    if(i%nHistoPerCanvas == 0){
    //      fCanvas->Clear();
    //      fCanvas->Divide(1, nHistoPerCanvas);
    //    }
    TString histoName = next()->GetName();
    cout << "i = " << i << " et histoName = " << histoName << endl;

    if (histoName.Contains("EtaPhi")){
      m_h2d = (TH2*)gDirectory->Get(histoName);
      //int padIndex = (i%nHistoPerCanvas) + 1;
      //fCanvas->cd(padIndex);
      fCanvas->cd(i+1);
      m_h2d->SetStats(0); m_h2d->SetMinimum(0); m_h2d->SetMaximum(2);
      m_h2d->SetXTitle("Eta"); m_h2d->SetYTitle("Phi");
      m_h2d->Draw("COL:TEXT"); DrawSummaryLegend();
      //m_h2d->Draw();
    }
    else {
      m_h1d = (TH1*)gDirectory->Get(histoName);
      fCanvas->cd(i+1);
      m_h1d->SetXTitle("# cell"); m_h1d->SetYTitle("# event");
      m_h1d->Draw(); //DrawReference(m_h1d);
    }
  }
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw legend for summary histograms for what each color mean
void CaloClusterVecMon::DrawSummaryLegend()
{
  Int_t colors[] = {10, 3, 2}; gStyle->SetPalette(3,colors);
  TPaveText *pt= new TPaveText(0.84,0.94,0.99,0.99,"trNDC");
  TText *t3 = pt->AddText("Red(2) #equiv Warning");
  t3->SetTextColor(2);
  pt->SetBorderSize(1); pt->SetTextAlign(12);
  pt->Draw();
}

//#####################################################################
// Set number of canvases in the container for multiple histograms
void CaloClusterVecMon::SetNCanvas(int ncanvas)
{
  if(fContainer->GetList()->GetSize() != ncanvas){
    fContainer->Cleanup();
    for(int i=0; i<ncanvas; i++){
      fECanvas[i] = new TRootEmbeddedCanvas(0,fContainer, m_canvasW, m_canvasH);
      fContainer->AddFrame(fECanvas[i],new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    }
    fMain->MapSubwindows();
  }
}

//#####################################################################
// Save canvas(es) in a ps file and keep it open
void CaloClusterVecMon::Save()
{
  if(m_psFile == 0){
    TString dir(".");
    TGFileInfo fi;
    const char *filetypes[] = {"PostScript files", "*.ps", 0, 0};
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
    dir = fi.fIniDir;
    if(!fi.fFilename){
      fStatusBar->SetText("NO FILE SELECTED OR NAME TYPED!");
      return;
    } else{
      m_psFileName = fi.fFilename;
      m_psFile = new TPostScript(m_psFileName,112);
      m_psFile->Off();
    }
  }
  m_psFile->On();
  int nCanvas = fContainer->GetList()->GetSize();
  for(int i=0; i<nCanvas; i++){
    fCanvas = fECanvas[i]->GetCanvas();
    m_psFile->NewPage();
    fCanvas->Draw();
  }
  TString psFileBaseName = gSystem->BaseName(m_psFile->GetName());
  fStatusBar->SetText("Canvas(es) is(are) saved in FILE: " + psFileBaseName +
		      " and file is left open for future saves in this session." + 
		      " CLOSE FILE BEFORE EXIT.");
  m_psFile->Off(); 
}

//#####################################################################
// Print current active canvas to the printer directly
void CaloClusterVecMon::PrintThis()
{
  TString temp = "temp.ps";
  TPostScript ps(temp,112);
  int nCanvas = fContainer->GetList()->GetSize();
  for(int i=0; i<nCanvas; i++){
    fCanvas = fECanvas[i]->GetCanvas();
    ps.NewPage();
    fCanvas->Draw();
  }
  ps.Close();
  TString printCommand = g_printCommand + " " + g_printerName + " " + temp;
  gSystem->Exec(printCommand);
  fStatusBar->SetText("Canvas(es) is(are) sent to PRINTER: " + g_printerName);
  gSystem->Exec("rm -f " + temp);
}

//#####################################################################
// Execute lines between "case ... break" 
// upon click on given top menu entry
void CaloClusterVecMon::HandleMainMenu(Int_t id)
{
  switch (id) {
  case M_PS_OPEN:
    if(m_psFile != 0){
      fStatusBar->SetText("FILE: " + m_psFileName + " IS OPEN!");
      break;
    }
    TString dir(".");
    TGFileInfo fi;
    const char *filetypes[] = {"PostScript files", "*.ps", 0, 0};
    fi.fFileTypes = filetypes;
    fi.fIniDir    = StrDup(dir);
    new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
    dir = fi.fIniDir;
    if(!fi.fFilename){
      fStatusBar->SetText("NO FILE SELECTED OR NAME TYPED!");
      break;
    } else{
      m_psFileName = fi.fFilename;
      m_psFile = new TPostScript(m_psFileName,112);
      m_psFile->Off();
      fStatusBar->SetText("FILE: " + m_psFileName + " is open.");
    }
    break;
    
  case M_PS_CLOSE:
    if(m_psFile == 0){
      fStatusBar->SetText("NO OPEN PS FILE EXIST!");
    } else{
      m_psFile->On();
      m_psFile->Close();
      fStatusBar->SetText("FILE: " + m_psFileName + " is closed.");
      m_psFile = 0;
    }
    break;
    
  case M_PS_PRINT:
    if(m_psFileName == ""){
      fStatusBar->SetText("NO SAVED PS FILE EXIST!");
      break;
    } else{
      if(m_psFile != 0){ 
	m_psFile->On(); 
	m_psFile->Close(); 
	m_psFile = 0; 
      } 
      TString printCommand = g_printCommand + " " + g_printerName + " " + m_psFileName;
      gSystem->Exec(printCommand);
      fStatusBar->SetText("FILE: " + m_psFileName +
			" is sent to PRINTER: " + g_printerName);
    }
    break;
   
  case M_PS_DELETE:
    if(m_psFileName == ""){
      fStatusBar->SetText("NO SAVED PS FILE EXIST!");
    } else{
      gSystem->Exec("rm -f " + m_psFileName);
      fStatusBar->SetText("FILE: " + m_psFileName + " is removed.");
    }
    break;

  case M_CALOCLUSTERVEC_SUMMARY:
    m_dirName = m_dirCaloClusterVec + m_dirSummary;
    m_configOption = "SUMMARY";
    DrawSummary();
    break;

  case M_CALOCLUSTERVEC_ENERGY:
    m_dirName = m_dirCaloClusterVec + m_dirEnergy;
    m_histoNameBase1 = "Energy";
    m_configOption = "ENERGY";
    break;

  case M_CALOCLUSTERVEC_ETA:
    m_dirName = m_dirCaloClusterVec + m_dirEta;
    m_histoNameBase1 = "Eta";
    m_configOption = "ETA";
    break;

  case M_CALOCLUSTERVEC_PHI:
    m_dirName = m_dirCaloClusterVec + m_dirPhi;
    m_histoNameBase1 = "Phi";
    m_configOption = "PHI";
    break;

  case M_CALOCLUSTERVEC_ETAPHI:
    m_dirName = m_dirCaloClusterVec + m_dirEtaPhi;
    m_histoNameBase1 = "EtaPhi";
    m_configOption = "ETAPHI";
    break;

  case M_CALOCLUSTERVEC_SUMEVSETAPHI:
    m_dirName = m_dirCaloClusterVec + m_dirSumEvsEtaPhi;
    m_histoNameBase1 = "SumEvsEtaPhi";
    m_configOption = "SUMEVSETAPHI";
    break;

  case M_CALOCLUSTERVEC_SUMEVSETA:
    m_dirName = m_dirCaloClusterVec + m_dirSumEvsEta;
    m_histoNameBase1 = "SumEvsEta";
    m_configOption = "SUMEVSETA";
    break;

  case M_CALOCLUSTERVEC_SUMEVSPHI:
    m_dirName = m_dirCaloClusterVec + m_dirSumEvsPhi;
    m_histoNameBase1 = "SumEvsPhi";
    m_configOption = "SUMEVSPHI";
    break;
  }
  
  fMain->SetWindowName("Calo Cluster Vector Monitoring >>> " + m_fileBaseName + ":" + m_dirName);
  //Fill combobox with layers monitored and draw first selection
  if(id > M_CALOCLUSTERVEC_SUMMARY){
    FillCombo();
    DrawSingle();
  }
  ConfigureComboFrame();  // hide/show comboboxes/buttons given the context
}
