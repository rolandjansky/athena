/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################################
//#####################################################################
//## AUTHOR:      Christophe Le Maner(lemaner@cern.ch)
//## FILE:        CaloCellVecMonitoring.C
//## MODIFIED:    2006/09/22
//## DESCRIPTION: Build GUI for histos produced by CaloCellVecMonTool.
//##              Based on framework developped by Tayfun Ince
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
class CaloCellVecMon
{

  // cppcheck-suppress unknownMacro
  RQ_OBJECT("CaloCellVecMon") // necessary if not inherite from ROOT class

 protected:
  enum EPopupMenuEntry
    { // enumerators for signal/slot mechanism of main menu entries
      M_PS_OPEN,
      M_PS_CLOSE,
      M_PS_PRINT,
      M_PS_DELETE,

      M_CALOCELLVEC_SUMMARY,
      M_CALOCELLVEC_ENERGY,
      M_CALOCELLVEC_1DRATEETA,
      M_CALOCELLVEC_1DRATEPHI,
      M_CALOCELLVEC_2DAVEN,
      M_CALOCELLVEC_2DNOISE,
      M_CALOCELLVEC_2DRATE,
      M_CALOCELLVEC_ENVSTIME,
      M_CALOCELLVEC_KNOWNBADCHAN,
      M_CALOCELLVEC_NOCONDCHAN,
    };

 private:
  TGMainFrame          *fMain;
  TGDockableFrame      *fMenuDock;
  TGCanvas             *fGCanvas;
  TGCompositeFrame     *fContainer;
  TRootEmbeddedCanvas  *fECanvas[20];
  TGMenuBar            *fMenuBar;
  TGPopupMenu          *fMenuSummary, *fMenuCaloCell;
  TGPopupMenu          *fMenuPS;
  TGHorizontalFrame    *fHFrame;
  TGComboBox           *fComboLayer, *fComboEcut, *fComboEtaSlice;
  TGTextButton         *fDrawSingle, *fDrawLayers, *fDrawRegions, *fDrawEcuts, *fSave, *fPrintThis;
  TGStatusBar          *fStatusBar;
  TCanvas              *fCanvas;
  TH1                  *m_h1d, *m_href;
  TH2                  *m_h2d;
  TPostScript          *m_psFile;

  TString              m_fileBaseName;
  TString              m_dirName, m_histoNameBase1;
  TString              m_dirCaloCellVec, m_dirEnergy, m_dir1dRateEta, m_dir1dRatePhi, m_dir2dAvEn, m_dir2dNoise, m_dir2dRate, m_dirEnvsTime, m_dirKnownBadChan, m_dirNoCondChan, m_dirSummary;
  TString              m_configOption, m_prevConfigOption, m_drawOption;
  TString              m_psFileName;

  int                  m_canvasW, m_canvasH;

 public:
  CaloCellVecMon(const TGWindow *p, UInt_t w, UInt_t h, TString min);
  virtual ~CaloCellVecMon();
  CaloCellVecMon (const CaloCellVecMon&) = delete;
  CaloCellVecMon& operator= (const CaloCellVecMon&) = delete;

  void CloseMainWindow();

  void ConfigureMainMenu();
  void ConfigureComboFrame();
  void HandleMainMenu(Int_t id);
  void FillComboLayer();
  void HandleComboLayer();

  void SetNCanvas(int ncanvas);
  void DrawSingle();
  void DrawLayers();
  void DrawRegions();
  void DrawEcuts();
  void DrawSummary();
  void DrawSummaryLegend();
  void DrawEtaPhiLegend();

  void Save();
  void PrintThis();
};

//#####################################################################
//#####################################################################
// Class constructor
CaloCellVecMon::CaloCellVecMon(const TGWindow *p, UInt_t w, UInt_t h, TString min)
  : fMain (nullptr),
    fMenuDock (nullptr),
    fGCanvas (nullptr),
    fContainer (nullptr),
    fECanvas (),
    fMenuBar (nullptr),
    fMenuSummary (nullptr),
    fMenuCaloCell (nullptr),
    fMenuPS (nullptr),
    fHFrame (nullptr),
    fComboLayer (nullptr),
    fComboEcut (nullptr),
    fComboEtaSlice (nullptr),
    fDrawSingle (nullptr),
    fDrawLayers (nullptr),
    fDrawRegions (nullptr),
    fDrawEcuts (nullptr),
    fSave (nullptr),
    fPrintThis (nullptr),
    fStatusBar (nullptr),
    fCanvas (nullptr),
    m_h1d (nullptr),
    m_href (nullptr),
    m_h2d (nullptr),
    m_psFile (nullptr),

    m_fileBaseName (gSystem->BaseName(g_rootFile->GetName())),
    m_dirEnergy ("Energy"),
    m_dir1dRateEta ("1d_Rate"),
    m_dir1dRatePhi ("1d_Rate"),
    m_dir2dAvEn ("2d_AvEn"),
    m_dir2dNoise ("2d_Noise"),
    m_dir2dRate ("2d_Rate"),
    m_dirEnvsTime ("EnvsTime"),
    m_dirKnownBadChan ("KnownBadChan"),
    m_dirNoCondChan ("NoCondChan"),
    m_dirSummary ("Summary"),

    m_canvasW (720),
    m_canvasH (540)
{
  cout << endl << "constructer loaded" <<endl;
  
  // Look for additional "run" directory on top of "LAr" directory
  g_rootFile->cd();
  TIter next0(gDirectory->GetListOfKeys());
  TString rootDirName;
  while(key = (TKey*)next0()){
    rootDirName = key->GetName();
    if(rootDirName(0,3)=="run") rootDirName = rootDirName+"/";
    else rootDirName = "";
    break;
  }


  // initialize variables
  m_dirCaloCellVec = rootDirName + min + "/LAr/CaloMonitoring/CaloCellVecMon/";
  cout << "m_dirCaloCellVec: " << m_dirCaloCellVec <<endl;
  
  /*  //no directory in root file, kamile
  m_dirSummary = "Summary";
  m_dirSummary = "";
  m_dirEnergy = "Energy";
  m_dirEnergy = "";
  m_dir1dRate = "Eta";
  m_dir2dAvEn = "Phi";
  m_dir2dNoise = "EtaPhi";
  m_dir2dRate = "SumEvsEtaPhi";
  m_dirEnvsTime = "SumEvsEta";
  m_dirKnownBadChan = "SumEvsPhi";
  */
  
  // global style options
  gStyle->SetOptStat("emr");
  gStyle->SetPaperSize(TStyle::kA4);
  gStyle->SetStatX(0.99);
  gStyle->SetStatY(0.99);

  // start building gui
  // create a top level window
  fMain = new TGMainFrame(p,w,h);

  // connect(signal, receiver_class, receiver, slot)
  fMain->Connect("CloseWindow()", "CaloCellVecMon", this, "CloseMainWindow()");

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
  fMenuPS->Connect("Activated(Int_t)", "CaloCellVecMon", this, "HandleMainMenu(Int_t)");

  // create "Summary" menu
  fMenuSummary = new TGPopupMenu(gClient->GetRoot());
  fMenuSummary->AddEntry("Summary", M_CALOCELLVEC_SUMMARY);
  fMenuSummary->Connect("Activated(Int_t)", "CaloCellVecMon", this, "HandleMainMenu(Int_t)");

  // create "CaloCellVec" menu
  
  fMenuCaloCell = new TGPopupMenu(gClient->GetRoot());
  fMenuCaloCell->AddEntry("Energy", M_CALOCELLVEC_ENERGY);
  fMenuCaloCell->AddSeparator();
  fMenuCaloCell->AddEntry("1d_Rate Eta", M_CALOCELLVEC_1DRATEETA);
  fMenuCaloCell->AddEntry("1d_Rate Phi", M_CALOCELLVEC_1DRATEPHI);
  fMenuCaloCell->AddEntry("2d_AvEn", M_CALOCELLVEC_2DAVEN);
  fMenuCaloCell->AddEntry("2d_Noise", M_CALOCELLVEC_2DNOISE);
  fMenuCaloCell->AddEntry("2d_Rate", M_CALOCELLVEC_2DRATE);
  fMenuCaloCell->AddSeparator();
  fMenuCaloCell->AddEntry("EnvsTime", M_CALOCELLVEC_ENVSTIME);
  fMenuCaloCell->AddEntry("KnownBadChan", M_CALOCELLVEC_KNOWNBADCHAN);
  fMenuCaloCell->AddEntry("NoCondChan", M_CALOCELLVEC_NOCONDCHAN);
  fMenuCaloCell->Connect("Activated(Int_t)", "CaloCellVecMon", this, "HandleMainMenu(Int_t)");
  
  
  

  // disable menu entries for user mode
  //if(g_expertMode == false){
  //fMenuSummary->DisableEntry(M_CALOCELLVEC_SUMMARY);
  // }

  // disable menu entries for directories don't exist in root file
   
  cout << "----ConfigureMainMenu starts ------" <<endl;
  ConfigureMainMenu();

  // add menu bar with popup menus to the dockable frame
  fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
  fMenuDock->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 2, 2);
  fMenuBar->AddPopup("&PostScript", fMenuPS, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Summary", fMenuSummary, fMenuBarItemLayout);
  fMenuBar->AddPopup("Calo&CellVec", fMenuCaloCell, fMenuBarItemLayout);

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
  fComboLayer = new TGComboBox(fHFrame);
  fComboLayer->Connect("Selected(Int_t)", "CaloCellVecMon", this, "HandleComboLayer()");
  fComboEcut = new TGComboBox(fHFrame);
  fComboEtaSlice = new TGComboBox(fHFrame);
  fComboLayer->Resize(150,20);
  fComboEcut->Resize(100,20);
  fComboEtaSlice->Resize(100,20);
  fHFrame->AddFrame(fComboLayer, hframeLayout);
  fHFrame->AddFrame(fComboEcut, hframeLayout);
  fHFrame->AddFrame(fComboEtaSlice, hframeLayout);
  // text buttons
  fDrawSingle = new TGTextButton(fHFrame,"");
  fDrawSingle->Connect("Clicked()", "CaloCellVecMon", this, "DrawSingle()");
  fHFrame->AddFrame(fDrawSingle, hframeLayout);
  fDrawLayers = new TGTextButton(fHFrame,"");
  fDrawLayers->Connect("Clicked()", "CaloCellVecMon", this, "DrawLayers()");
  fHFrame->AddFrame(fDrawLayers, hframeLayout);
  fDrawRegions = new TGTextButton(fHFrame,"");
  fDrawRegions->Connect("Clicked()", "CaloCellVecMon", this, "DrawRegions()");
  fHFrame->AddFrame(fDrawRegions, hframeLayout);
  fDrawEcuts = new TGTextButton(fHFrame,"");
  fDrawEcuts->Connect("Clicked()", "CaloCellVecMon", this, "DrawEcuts()");
  fHFrame->AddFrame(fDrawEcuts, hframeLayout);
  fSave = new TGTextButton(fHFrame," Save ");
  fSave->Connect("Clicked()", "CaloCellVecMon", this, "Save()");
  fHFrame->AddFrame(fSave, hframeLayout);
  fPrintThis = new TGTextButton(fHFrame," PrintThis ");
  fPrintThis->Connect("Clicked()", "CaloCellVecMon", this, "PrintThis()");
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
  fHFrame->HideFrame(fComboLayer);
  fHFrame->HideFrame(fComboEcut);
  fHFrame->HideFrame(fComboEtaSlice);
  fHFrame->HideFrame(fDrawSingle);
  fHFrame->HideFrame(fDrawLayers);
  fHFrame->HideFrame(fDrawRegions);
  fHFrame->HideFrame(fDrawEcuts);
  
  // initialize layout of the main window and map the GUI
  fMain->Layout();
  fMain->MapWindow();
}

//#####################################################################
// Class destructor
CaloCellVecMon::~CaloCellVecMon()
{
  // fMain->Cleanup() can't delete sub-sub level widgets
  // so fContainer->Cleanup() is a must here
  fContainer->Cleanup();
  fMain->Cleanup();
}

//#####################################################################
// Close main window upon click on "X"
void CaloCellVecMon::CloseMainWindow()
{
  delete this;
}

//#####################################################################
// Configure menu bar to disable entries 
// for directories don't exist in the root file
void CaloCellVecMon::ConfigureMainMenu()
{
  TGMenuEntry *entry;
  g_rootFile->cd();
  TString dir = m_dirCaloCellVec;
  cout << "m_dirCaloCellVec:  " << m_dirCaloCellVec << endl;
  
  if(gDirectory->Get(dir.Remove(3,'/')) == 0){
    cout << "gDirectory->Get(dir.Remove(3,'/'))  " << gDirectory->Get(dir.Remove(3,'/')) << endl; 
    TIter next(fMenuCaloCell->GetListOfEntries());
    while(entry = (TGMenuEntry*)next()){
      if(entry->GetType() == kMenuEntry) 
	fMenuCaloCell->DisableEntry(entry->GetEntryId());
    }
    fMenuSummary->DisableEntry(M_CALOCELLVEC_SUMMARY);
  } 
  else{    
    g_rootFile->cd(m_dirCaloCellVec);
    if(gDirectory->Get(m_dirSummary) == 0)     fMenuSummary->DisableEntry(M_CALOCELLVEC_SUMMARY);
    if(gDirectory->Get(m_dirEnergy) == 0)      fMenuCaloCell->DisableEntry(M_CALOCELLVEC_ENERGY);
    if(gDirectory->Get(m_dir1dRateEta) == 0)         fMenuCaloCell->DisableEntry(M_CALOCELLVEC_1DRATEETA);
    if(gDirectory->Get(m_dir1dRatePhi) == 0)         fMenuCaloCell->DisableEntry(M_CALOCELLVEC_1DRATEPHI);
    if(gDirectory->Get(m_dir2dAvEn) == 0)         fMenuCaloCell->DisableEntry(M_CALOCELLVEC_2DAVEN);
    if(gDirectory->Get(m_dir2dNoise) == 0)      fMenuCaloCell->DisableEntry(M_CALOCELLVEC_2DNOISE);
    if(gDirectory->Get(m_dir2dRate) == 0)fMenuCaloCell->DisableEntry(M_CALOCELLVEC_2DRATE);
    if(gDirectory->Get(m_dirEnvsTime) == 0)   fMenuCaloCell->DisableEntry(M_CALOCELLVEC_ENVSTIME);
    if(gDirectory->Get(m_dirKnownBadChan) == 0)   fMenuCaloCell->DisableEntry(M_CALOCELLVEC_KNOWNBADCHAN);
    if(gDirectory->Get(m_dirNoCondChan) == 0)   fMenuCaloCell->DisableEntry(M_CALOCELLVEC_NOCONDCHAN);
  }
}

//#####################################################################
// Configure horizontal frame to hide and show necessary
// combo boxes and buttons given the context 
void CaloCellVecMon::ConfigureComboFrame()
{
  /*
  if(m_configOption == "SUMMARY"){
    if(fHFrame->IsVisible(fComboLayer)    == 1) fHFrame->HideFrame(fComboLayer);
    if(fHFrame->IsVisible(fComboEtaSlice) == 1) fHFrame->HideFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fComboEcut)     == 1) fHFrame->HideFrame(fComboEcut);
    if(fHFrame->IsVisible(fDrawSingle)    == 1) fHFrame->HideFrame(fDrawSingle);
    if(fHFrame->IsVisible(fDrawLayers)    == 1) fHFrame->HideFrame(fDrawLayers);
    if(fHFrame->IsVisible(fDrawRegions)   == 1) fHFrame->HideFrame(fDrawRegions);
    if(fHFrame->IsVisible(fDrawEcuts)     == 1) fHFrame->HideFrame(fDrawEcuts);
    //fDrawSingle->SetText(" Draw Summary ");
  }
  */

  //######################
  if(m_configOption == "ENERGY"){
    cout << "configOption == ENERGY" << endl;
    if(fHFrame->IsVisible(fComboLayer)    == 0) fHFrame->ShowFrame(fComboLayer);
    if(fHFrame->IsVisible(fComboEtaSlice) == 1) fHFrame->ShowFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fComboEcut)     == 1) fHFrame->ShowFrame(fComboEcut);
    if(fHFrame->IsVisible(fDrawSingle)    == 0) fHFrame->ShowFrame(fDrawSingle);
    if(fHFrame->IsVisible(fDrawLayers)    == 0) fHFrame->ShowFrame(fDrawLayers);
    if(fHFrame->IsVisible(fDrawRegions)   == 1) fHFrame->ShowFrame(fDrawRegions);
    if(fHFrame->IsVisible(fDrawEcuts)     == 0) fHFrame->HideFrame(fDrawEcuts);
    fDrawSingle->SetText(" Draw ");
    fDrawLayers->SetText(" Draw Layers ");
    //fDrawRegions->SetText(" ");
  }

  //######################
  else if(m_configOption == "ETA" || m_configOption == "PHI" || m_configOption == "GEN" 
	  || m_configOption == "ETAPHI" || m_configOption == "SUMEVSETAPHI" 
	  || m_configOption == "SUMEVSETA" || m_configOption == "SUMEVSPHI"){
    if(fHFrame->IsVisible(fComboLayer)    == 0) fHFrame->ShowFrame(fComboLayer);
    if(fHFrame->IsVisible(fComboEtaSlice) == 1) fHFrame->ShowFrame(fComboEtaSlice);
    if(fHFrame->IsVisible(fComboEcut)     == 1) fHFrame->ShowFrame(fComboEcut);
    if(fHFrame->IsVisible(fDrawSingle)    == 0) fHFrame->ShowFrame(fDrawSingle);
    if(fHFrame->IsVisible(fDrawLayers)    == 0) fHFrame->ShowFrame(fDrawLayers);
    if(fHFrame->IsVisible(fDrawRegions)   == 0) fHFrame->HideFrame(fDrawRegions);
    if(fHFrame->IsVisible(fDrawEcuts)     == 1) fHFrame->ShowFrame(fDrawEcuts);
    fDrawSingle->SetText(" Draw ");
    fDrawLayers->SetText(" Draw Layers ");
    //fDrawEcuts->SetText(" Draw Energy cuts ");
  }
  fMain->Layout();
  
  fHFrame->HideFrame(fComboEtaSlice);
  fHFrame->HideFrame(fComboEcut);
  fHFrame->HideFrame(fDrawRegions);
  fHFrame->HideFrame(fDrawEcuts);

}

//#####################################################################
// Fill combobox for layer monitored
void CaloCellVecMon::FillComboLayer()
{
  if(m_prevConfigOption == m_configOption) 
    return;

  fComboLayer->RemoveEntries(0,fComboLayer->GetNumberOfEntries());
  fComboEcut->RemoveEntries(0,fComboEcut->GetNumberOfEntries());
  fComboEtaSlice->RemoveEntries(0,fComboEtaSlice->GetNumberOfEntries());

  g_rootFile->cd();
  g_rootFile->cd(m_dirName);
  TKey *key;
  TString lay;
  TSortedList *layList = new TSortedList();
  TIter next(gDirectory->GetListOfKeys());
  while(key = (TKey*)next()){
    TString name = key->GetName();
    cout << "name: " << name <<endl; 
    TString type = name(0,m_histoNameBase1.Length());
   if( type == m_histoNameBase1){
    /* 
    if (m_configOption == "ENERGY")
      //lay = name(m_histoNameBase1.Length()+7,3);
      lay = name(m_histoNameBase1.Length(),15);
    else if(m_configOption == "ETA" || m_configOption == "PHI"
	    || m_configOption == "ETAPHI" || m_configOption == "SUMEVSETAPHI" 
	    || m_configOption == "SUMEVSETA" || m_configOption == "SUMEVSPHI") 
      //lay = name(m_histoNameBase1.Length()+5,3);
    */
      lay = name(m_histoNameBase1.Length(),15);
    if(layList->FindObject(lay)) 
      continue;
    cout << "layer = " << lay <<endl;
    layList->Add(new TObjString(lay));
   }
  }
  
  TIter next(layList);
  int i=0;
  while(i<layList->GetSize())
    fComboLayer->AddEntry(next()->GetName(), i++);
  fComboLayer->Select(1);
  
  m_prevConfigOption = m_configOption;
}

//#####################################################################
// Fill combobox for layer monitored
void CaloCellVecMon::HandleComboLayer()
{
  if(m_prevConfigOption == m_configOption)
    return;
  
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
      if(etasliceList->FindObject(etaslice))
	continue;
      etasliceList->Add(new TObjString(etaslice));
    }
    
    TIter next(etasliceList);
    int i=0;
    while(i<etasliceList->GetSize())
      fComboEtaSlice->AddEntry(next()->GetName(), i++);
    fComboEtaSlice->Select(1);
    
  }
  else {
    g_rootFile->cd();
    g_rootFile->cd(m_dirName);
    TKey *key;
    TString ecut;
    TSortedList *ecutList = new TSortedList();
    TIter next(gDirectory->GetListOfKeys());
    while(key = (TKey*)next()){
      TString name = key->GetName();
      //cout << "m_histoNameBase1 = " << m_histoNameBase1 << endl;
      ecut = name(m_histoNameBase1.Length(),5);
      //cout << "ecut = " << ecut << endl;
      if(ecutList->FindObject(ecut)) 
	continue;
      ecutList->Add(new TObjString(ecut));
    }
    
    TIter next(ecutList);
    int i=0;
    while(i<ecutList->GetSize())
      fComboEcut->AddEntry(next()->GetName(), i++);
    fComboEcut->Select(0);
  }
}

//#####################################################################
// Draw single histo
void CaloCellVecMon::DrawSingle()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);

  if(fContainer->GetList()->GetSize() != 1){
    fContainer->Cleanup();
    fECanvas[0] = new TRootEmbeddedCanvas(0,fContainer, m_canvasW, m_canvasH);
    fContainer->AddFrame(fECanvas[0], new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
    fMain->MapSubwindows();
  }
  fCanvas = fECanvas[0]->GetCanvas();

  TGTextLBEntry *tEntry = (TGTextLBEntry*)fComboLayer->GetSelectedEntry();
  TString lay = tEntry->GetText()->GetString();
  if (m_drawOption == "ENERGY") 
    tEntry = (TGTextLBEntry*)fComboEtaSlice->GetSelectedEntry();
  else 
    tEntry = (TGTextLBEntry*)fComboEcut->GetSelectedEntry();
  TString cut = tEntry->GetText()->GetString();
  TString histoName;

  //######################
  fCanvas->Clear();
  fCanvas->Divide(1,1);
  //histoName = m_histoNameBase1 + cut + lay;
  histoName = m_histoNameBase1 + lay;
  cout << __LINE__ << endl;
  cout << "m_drawOption = " << m_drawOption << " et histoName = " << histoName << endl;
  
  g_rootFile->cd(m_dirName);
  if (m_drawOption == "ENERGY"){
    m_h1d = (TH1*)gDirectory->Get(histoName);
    gStyle->SetOptStat("emrou");
    fCanvas->cd(1);
    gStyle->SetOptStat("emrou");
    if(m_h1d->GetMaximum() > 0) 
      gPad->SetLogy(1);
    gPad->SetGridx(0);
    gPad->SetGridy(1);
    m_h1d->Draw();g_DrawReference(m_h1d);g_DrawRunInfo();
  }
  else if (m_drawOption == "ETAPHI"){
    m_h2d = (TH2*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1,0); // pretty colors for 2d histo
    gPad->SetLogy(0);
    //gPad->SetLogz(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    m_h2d->SetXTitle("Eta"); m_h2d->SetYTitle("Phi");
    //    m_h2d->SetStats(0); m_h2d->SetMinimum(0); m_h2d->SetMaximum(2);
    //m_h2d->Draw("COL"); DrawEtaPhiLegend();
    m_h2d->Draw("colz");g_DrawRunInfo();
  }
  else{
    m_h1d = (TH1*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    gStyle->SetOptStat(0);
    gPad->SetLogy(0);
    gPad->SetGridx(0);
    gPad->SetGridy(1);
    m_h1d->Draw();g_DrawReference(m_h1d);g_DrawRunInfo();
  }
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw histos for the 4 layers at once
void CaloCellVecMon::DrawLayers()
{

  cout << "Draw Layers" <<endl;
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);
  
  TGTextLBEntry *tEntry;
  if(m_configOption == "ENERGY") tEntry = (TGTextLBEntry*)fComboEtaSlice->GetSelectedEntry();
  else tEntry = (TGTextLBEntry*)fComboEcut->GetSelectedEntry();
  TString cut = tEntry->GetText()->GetString();
  //######################
  int nLayer = fComboLayer->GetNumberOfEntries();  // number of eta slices
  int nLayerPerCanvas = 4;   // set number of layer per canvas 
  int nCanvas = (nLayer - 1)/nLayerPerCanvas + 1;   // number of canvases
  //cout << "nCanvas = " << nCanvas << endl;
  
  SetNCanvas(nCanvas);  // set number of canvases in the container
  
  g_rootFile->cd(m_dirName);
  for(int i=0; i<nLayer; i++){
    int canvasIndex = i/nLayerPerCanvas;
    fCanvas = fECanvas[canvasIndex]->GetCanvas();
    if(i%nLayerPerCanvas == 0){
      fCanvas->Clear();
      fCanvas->Divide(2, nLayerPerCanvas/2);
    }
    tEntry = (TGTextLBEntry*)(TGListBox*)fComboLayer->GetListBox()->GetEntry(i);
    TString lay = tEntry->GetText()->GetString();
    //TString histoName1 = m_histoNameBase1  + cut + lay;
    TString histoName1 = m_histoNameBase1  + lay;
    int padIndex = (i%nLayerPerCanvas) + 1;
    fCanvas->cd(padIndex);

    if(m_drawOption == "ENERGY"){
      m_h1d = (TH1*)gDirectory->Get(histoName1);
      gStyle->SetOptStat("emrou");
      if(m_h1d->GetMaximum() > 0)
	gPad->SetLogy(1); //switch on log scale
      gPad->SetGridy(1); 
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
    } 
    else if(m_drawOption == "ETAPHI"){
      m_h2d = (TH2*)gDirectory->Get(histoName1);
      gStyle->SetOptStat("emrou");
      gPad->SetLogy(0);
      gPad->SetLogz(0);
      gPad->SetGridx(0);
      gPad->SetGridy(0);
      m_h2d->SetXTitle("Eta"); m_h2d->SetYTitle("Phi");
      //     m_h2d->SetStats(0); m_h2d->SetMinimum(0); m_h2d->SetMaximum(2);
      //  m_h2d->Draw("COL"); g_DrawRunInfo();//DrawEtaPhiLegend();
      m_h2d->Draw("colz");g_DrawRunInfo();
    }
    else{
      m_h1d = (TH1*)gDirectory->Get(histoName1);
      gStyle->SetOptStat("emrou");
      gPad->SetLogy(0); //switch off log scale
      gPad->SetGridy(1); 
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
    }
  } 
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw histos for all regions at once 
void CaloCellVecMon::DrawRegions()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);
   
  TGTextLBEntry *tEntry = (TGTextLBEntry*)fComboLayer->GetSelectedEntry();
  TString lay = tEntry->GetText()->GetString();
  cout << "lay = " << lay << endl;
  //######################
  if(m_drawOption == "ENERGY"){
    int nEtaSlice = fComboEtaSlice->GetNumberOfEntries();  // number of eta slices
    int nEtaSlicePerCanvas = 4;   // set number of eta slices per canvas = number of layer
    int nCanvas = (nEtaSlice - 1)/nEtaSlicePerCanvas + 1;   // number of canvases
    //cout << "nCanvas = " << nCanvas << endl;

    SetNCanvas(nCanvas);  // set number of canvases in the container
    
    g_rootFile->cd(m_dirName);
    for(int i=0; i<nEtaSlice; i++){
      int canvasIndex = i/nEtaSlicePerCanvas;
      fCanvas = fECanvas[canvasIndex]->GetCanvas();
      if(i%nEtaSlicePerCanvas == 0){
	fCanvas->Clear();
	fCanvas->Divide(2, nEtaSlicePerCanvas/2);
      }
      tEntry = (TGTextLBEntry*)(TGListBox*)fComboEtaSlice->GetListBox()->GetEntry(i);
      TString cut = tEntry->GetText()->GetString();
      TString histoName1 = m_histoNameBase1  + cut + lay;
      int padIndex = (i%nEtaSlicePerCanvas) + 1;
      fCanvas->cd(padIndex);
      m_h1d = (TH1*)gDirectory->Get(histoName1);
      gStyle->SetOptStat("emrou");
      if(m_h1d->GetMaximum() > 0)
	gPad->SetLogy(1); //switch on log scale
      gPad->SetGridy(1); 
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
      fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
    } 
  } 
  else return;
}

//#####################################################################
// Draw histos for all energy cuts at once 
void CaloCellVecMon::DrawEcuts()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);
   
  TGTextLBEntry *tEntry = (TGTextLBEntry*)fComboLayer->GetSelectedEntry();
  TString lay = tEntry->GetText()->GetString();
  cout << "lay = " << lay << endl;
  //######################
  int nEcut = fComboEcut->GetNumberOfEntries();  // number of eta slices
  int nEcutPerCanvas = 4;   // set number of eta slices per canvas = number of layer
  int nCanvas = (nEcut - 1)/nEcutPerCanvas + 1;   // number of canvases
  //cout << "nCanvas = " << nCanvas << endl;
  
  SetNCanvas(nCanvas);  // set number of canvases in the container
  
  g_rootFile->cd(m_dirName);
  for(int i=0; i<nEcut; i++){
    int canvasIndex = i/nEcutPerCanvas;
    fCanvas = fECanvas[canvasIndex]->GetCanvas();
    if(i%nEcutPerCanvas == 0){
      fCanvas->Clear();
      fCanvas->Divide(2, nEcutPerCanvas/2);
    }
    tEntry = (TGTextLBEntry*)(TGListBox*)fComboEcut->GetListBox()->GetEntry(i);
    TString cut = tEntry->GetText()->GetString();
    TString histoName1 = m_histoNameBase1  + cut + lay;
    int padIndex = (i%nEcutPerCanvas) + 1;
    fCanvas->cd(padIndex);
    if(m_drawOption == "ETA" || m_drawOption == "PHI" || m_drawOption == "GEN"){
      m_h1d = (TH1*)gDirectory->Get(histoName1);
      gPad->SetLogy(0); //switch off log scale
      gPad->SetGridy(1); 
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
    } 
    else if (m_drawOption == "ETAPHI"){
      m_h2d = (TH2*)gDirectory->Get(histoName1);
      gPad->SetLogy(0); //switch off log scale
      gPad->SetLogz(0);
      gPad->SetGridx(0);
      gPad->SetGridy(0);
      m_h2d->SetXTitle("Eta"); m_h2d->SetYTitle("Phi");
      //     m_h2d->SetStats(0); m_h2d->SetMinimum(0); m_h2d->SetMaximum(2);
      m_h2d->Draw("COL"); g_DrawRunInfo();//DrawEtaPhiLegend();
    } 
    else return;
  }
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw all summary plots
void CaloCellVecMon::DrawSummary()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  m_dirName = m_dirCaloCellVec + m_dirSummary;
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
  if(nHisto == 0) 
    return;
  //int nHistoPerCanvas = 2;   // set number of histo per canvas
  //int nCanvas = (nHisto - 1)/nHistoPerCanvas + 1;   // number of canvases
  //if(nHisto == 1){
  //  nHistoPerCanvas = 1; nCanvas = 1;
  //}
  
  //SetNCanvas(nCanvas);  // set number of canvases in the container
  SetNCanvas(1);
  
  fCanvas = fECanvas[0]->GetCanvas();
  fCanvas->Clear();
  fCanvas->Divide(2,1);
  TIter next(histoList);
  for(int i=0; i<nHisto; i++){
    //int canvasIndex = i/nHistoPerCanvas; 
    //fCanvas = fECanvas[canvasIndex]->GetCanvas();
    //if(i%nHistoPerCanvas == 0){
    //fCanvas->Clear();
    //fCanvas->Divide(1, nHistoPerCanvas);
    //}
    TString histoName = next()->GetName();
    //cout << "i = " << i << " et histoName = " << histoName << endl;
    
    if (histoName.Contains("EtaPhi")){
      m_h2d = (TH2*)gDirectory->Get(histoName);
      //int padIndex = (i%nHistoPerCanvas) + 1;
      //fCanvas->cd(padIndex);
      fCanvas->cd(i+1);
      gPad->SetLogy(0);
      gPad->SetLogz(0);
      gPad->SetGridx(0);
      gPad->SetGridy(0);
      m_h2d->SetXTitle("Eta"); m_h2d->SetYTitle("Phi");
      m_h2d->SetStats(0); m_h2d->SetMinimum(0); m_h2d->SetMaximum(2);
      m_h2d->Draw("COL"); DrawEtaPhiLegend();
      //m_h2d->Draw();
    }
    else {
      m_h1d = (TH1*)gDirectory->Get(histoName);
      //fCanvas->cd(padIndex);
      fCanvas->cd(i+1);
      gStyle->SetOptStat(0);
      gPad->SetLogy(0);
      gPad->SetGridx(0);
      gPad->SetGridy(0);
      m_h1d->SetXTitle("# cell"); m_h1d->SetYTitle("# event");
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
    }
  }
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw legend for summary histograms for what each color mean
void CaloCellVecMon::DrawSummaryLegend()
{
  Int_t colors[] = {10, 3, 2}; gStyle->SetPalette(3,colors);
  TPaveText *pt= new TPaveText(0.84,0.94,0.99,0.99,"trNDC");
  TText *t3 = pt->AddText("Red(2) #equiv Warning");
  t3->SetTextColor(2);
  pt->SetBorderSize(1); pt->SetTextAlign(12);
  pt->Draw();
}

//#####################################################################
// Draw legend for EtaPhi histograms for what each color mean
void CaloCellVecMon::DrawEtaPhiLegend()
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
void CaloCellVecMon::SetNCanvas(int ncanvas)
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
void CaloCellVecMon::Save()
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
    } 
    else{
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
void CaloCellVecMon::PrintThis()
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
void CaloCellVecMon::HandleMainMenu(Int_t id)
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
    } 
    else{
      m_psFileName = fi.fFilename;
      m_psFile = new TPostScript(m_psFileName,112);
      m_psFile->Off();
      fStatusBar->SetText("FILE: " + m_psFileName + " is open.");
    }
    break;
    
  case M_PS_CLOSE:
    if(m_psFile == 0){
      fStatusBar->SetText("NO OPEN PS FILE EXIST!");
    } 
    else{
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
    }
    else{
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
    } 
    else{
      gSystem->Exec("rm -f " + m_psFileName);
      fStatusBar->SetText("FILE: " + m_psFileName + " is removed.");
    }
    break;
    
  case M_CALOCELLVEC_SUMMARY:
    m_dirName = m_dirCaloCellVec + m_dirSummary;
    m_configOption = "SUMMARY";
    DrawSummary();
    break;
    
  case M_CALOCELLVEC_ENERGY:
    m_dirName = m_dirCaloCellVec + m_dirEnergy;
    //m_histoNameBase1 = "Energy";
    m_histoNameBase1 = "CellEnergy";
    m_configOption = "ENERGY";
    m_drawOption = "ENERGY";
    break;
    
  case M_CALOCELLVEC_1DRATEETA:
    m_dirName = m_dirCaloCellVec + m_dir1dRateEta;
    m_histoNameBase1 = "CellRatevsEta";
    m_configOption = "ETA";
    m_drawOption = "ETA";
    break;

  case M_CALOCELLVEC_1DRATEPHI:
    m_dirName = m_dirCaloCellVec + m_dir1dRatePhi;
    m_histoNameBase1 = "CellRatevsPhi";
    m_configOption = "PHI";
    m_drawOption = "PHI";
    break;

  case M_CALOCELLVEC_2DAVEN:
    m_dirName = m_dirCaloCellVec + m_dir2dAvEn;
    m_histoNameBase1 = "CellAvEnvsEtaPhi";
    m_configOption = "ETAPHI";
    m_drawOption = "ETAPHI";
    break;

  case M_CALOCELLVEC_2DNOISE:
    m_dirName = m_dirCaloCellVec + m_dir2dNoise;
    //m_histoNameBase1 = "EtaPhi";
    m_histoNameBase1 = "CellNoisevsEtaPhi";
    m_configOption = "ETAPHI2";
    m_drawOption = "ETAPHI";
    break;

  case M_CALOCELLVEC_2DRATE:
    m_dirName = m_dirCaloCellVec + m_dir2dRate;
    m_histoNameBase1 = "CellRatevsEtaPhi";
    m_configOption = "ETAPHI3";
    m_drawOption = "ETAPHI";
    break;

  case M_CALOCELLVEC_ENVSTIME:
    m_dirName = m_dirCaloCellVec + m_dirEnvsTime;
    m_histoNameBase1 = "CellEnvsTime";
    m_configOption = "GEN";
    m_drawOption = "GEN";
    break;

  case M_CALOCELLVEC_KNOWNBADCHAN:
    m_dirName = m_dirCaloCellVec + m_dirKnownBadChan;
    m_histoNameBase1 = "CellBadchanvsEtaPhi";
    m_configOption = "ETAPI4";
    m_drawOption = "ETAPHI";
    break;

  case M_CALOCELLVEC_NOCONDCHAN:
    m_dirName = m_dirCaloCellVec + m_dirNoCondChan;
    m_histoNameBase1 = "CellNocondvsEtaPhi";
    m_configOption = "ETAPHI5";
    m_drawOption = "ETAPHI";
    break;


  }
  
  fMain->SetWindowName("Calo Cell Vector Monitoring >>> " + m_fileBaseName + ":" + m_dirName);
  //Fill combobox with layers monitored and draw first selection
  if(id > M_CALOCELLVEC_SUMMARY){
    cout << "id > M_CALOCELLVEC_SUMMARY: " << id << ">" << M_CALOCELLVEC_SUMMARY << endl;
    cout << "m_dirName: " << m_dirName << endl;
    cout << "m_histoNameBase1:" << m_histoNameBase1 <<endl;
    cout << "m_configOption: "<< m_configOption <<endl;
    cout << "m_drawOption: " << m_drawOption <<endl;
    cout << "---- FillComboLayer starts ----" <<endl; 
    FillComboLayer();
    cout << "------ DrawSingle starts ------" << endl;
    DrawSingle();
  }
  
  cout << " --------- ConfigureComboFrame starts ----" <<endl;
  ConfigureComboFrame();  // hide/show comboboxes/buttons given the context
}
