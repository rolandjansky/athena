/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################################
//## AUTHOR:      Christophe Le Maner(lemaner@cern.ch)
//## FILE:        CaloCellTimeMonitoring.C
//## MODIFIED:    2006/09/22
//## DESCRIPTION: Build GUI for histos produced by CaloCellTimeMonTool.
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
class CaloCellTimeMon
{

  // cppcheck-suppress unknownMacro
  RQ_OBJECT("CaloCellTimeMon") // necessary if not inherite from ROOT class

 protected:
  enum EPopupMenuEntry
    { // enumerators for signal/slot mechanism of main menu entries
      M_PS_OPEN,
      M_PS_CLOSE,
      M_PS_PRINT,
      M_PS_DELETE,

      M_CALOCELLTIME_SUMMARY,
      M_CALOCELLTIME_TIME,
      M_CALOCELLTIME_TIMEVSENERGY,
    };

 private:
  TGMainFrame          *fMain;
  TGDockableFrame      *fMenuDock;
  TGCanvas             *fGCanvas;
  TGCompositeFrame     *fContainer;
  TRootEmbeddedCanvas  *fECanvas[15];
  TGMenuBar            *fMenuBar;
  TGPopupMenu          *fMenuSummary, *fMenuCaloCell;
  TGPopupMenu          *fMenuPS;
  TGHorizontalFrame    *fHFrame;
  TGComboBox           *fComboLayer;
  TGTextButton         *fDrawSingle, *fDrawLayers, *fSave, *fPrintThis;
  TGStatusBar          *fStatusBar;
  TCanvas              *fCanvas;
  TH1                  *m_h1d, *m_href;
  TH2                  *m_h2d;
  TPostScript          *m_psFile;

  TString              m_fileBaseName;
  TString              m_dirName, m_histoNameBase1;
  TString              m_dirCaloCellTime, m_dirTime, m_dirTimevsEnergy, m_dirSummary;
  TString              m_configOption, m_prevConfigOption, m_drawOption;
  TString              m_psFileName;

  int                  m_canvasW, m_canvasH;

 public:
  CaloCellTimeMon(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~CaloCellTimeMon();
  CaloCellTimeMon(const CaloCellTimeMon&) = delete;
  CaloCellTimeMon& operator=(const CaloCellTimeMon&) = delete;

  void CloseMainWindow();

  void ConfigureMainMenu();
  void ConfigureComboFrame();
  void HandleMainMenu(Int_t id);
  void FillComboLayer();
  void HandleComboLayer();

  void SetNCanvas(int ncanvas);
  void DrawSingle();
  void DrawLayers();
  void DrawSummaryLegend();
  void DrawSummary();

  void Save();
  void PrintThis();
};

//#####################################################################
//#####################################################################
// Class constructor
CaloCellTimeMon::CaloCellTimeMon(const TGWindow *p, UInt_t w, UInt_t h)
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
    fDrawSingle (nullptr),
    fDrawLayers (nullptr),
    fSave (nullptr),
    fPrintThis (nullptr),
    fStatusBar (nullptr),
    fCanvas (nullptr),
    m_h1d (nullptr),
    m_href (nullptr),
    m_h2d (nullptr),
    m_psFile (nullptr),

    m_fileBaseName (gSystem->BaseName(g_rootFile->GetName())),
    m_dirCaloCellTime ("/CaloCellTimeMon/"),
    m_dirTime ("Time"),
    m_dirTimevsEnergy ("TimevsEnergy"),
    m_dirSummary ("Summary"),
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
  fMain->Connect("CloseWindow()", "CaloCellTimeMon", this, "CloseMainWindow()");

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
  fMenuPS->Connect("Activated(Int_t)", "CaloCellTimeMon", this, "HandleMainMenu(Int_t)");

  // create "Summary" menu
  fMenuSummary = new TGPopupMenu(gClient->GetRoot());
  fMenuSummary->AddEntry("Summary", M_CALOCELLTIME_SUMMARY);
  fMenuSummary->Connect("Activated(Int_t)", "CaloCellTimeMon", this, "HandleMainMenu(Int_t)");

  // create "CaloCellTime" menu
  fMenuCaloCell = new TGPopupMenu(gClient->GetRoot());
  fMenuCaloCell->AddEntry("Time vs Energy", M_CALOCELLTIME_TIMEVSENERGY);
  fMenuCaloCell->AddEntry("Time", M_CALOCELLTIME_TIME);
  fMenuCaloCell->Connect("Activated(Int_t)", "CaloCellTimeMon", this, "HandleMainMenu(Int_t)");

  // disable menu entries for user mode
  //if(g_expertMode == false){
  //fMenuSummary->DisableEntry(M_CALOCELLTIME_SUMMARY);
  // }

  // disable menu entries for directories don't exist in root file
  ConfigureMainMenu();

  // add menu bar with popup menus to the dockable frame
  fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
  fMenuDock->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 2, 2);
  fMenuBar->AddPopup("&PostScript", fMenuPS, fMenuBarItemLayout);
  fMenuBar->AddPopup("&Summary", fMenuSummary, fMenuBarItemLayout);
  fMenuBar->AddPopup("Calo&CellTime", fMenuCaloCell, fMenuBarItemLayout);

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
  fComboLayer->Connect("Selected(Int_t)", "CaloCellTimeMon", this, "HandleComboLayer()");
  fComboLayer->Resize(80,20);
  fHFrame->AddFrame(fComboLayer, hframeLayout);
  // text buttons
  fDrawSingle = new TGTextButton(fHFrame,"");
  fDrawSingle->Connect("Clicked()", "CaloCellTimeMon", this, "DrawSingle()");
  fHFrame->AddFrame(fDrawSingle, hframeLayout);
  fDrawLayers = new TGTextButton(fHFrame,"");
  fDrawLayers->Connect("Clicked()", "CaloCellTimeMon", this, "DrawLayers()");
  fHFrame->AddFrame(fDrawLayers, hframeLayout);
  fSave = new TGTextButton(fHFrame," Save ");
  fSave->Connect("Clicked()", "CaloCellTimeMon", this, "Save()");
  fHFrame->AddFrame(fSave, hframeLayout);
  fPrintThis = new TGTextButton(fHFrame," PrintThis ");
  fPrintThis->Connect("Clicked()", "CaloCellTimeMon", this, "PrintThis()");
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
  fHFrame->HideFrame(fDrawSingle);
  fHFrame->HideFrame(fDrawLayers);

  // initialize layout of the main window and map the GUI
  fMain->Layout();
  fMain->MapWindow();
}

//#####################################################################
// Class destructor
CaloCellTimeMon::~CaloCellTimeMon()
{
  // fMain->Cleanup() can't delete sub-sub level widgets
  // so fContainer->Cleanup() is a must here
  fContainer->Cleanup();
  fMain->Cleanup();
}

//#####################################################################
// Close main window upon click on "X"
void CaloCellTimeMon::CloseMainWindow()
{
  delete this;
}

//#####################################################################
// Configure menu bar to disable entries 
// for directories don't exist in the root file
void CaloCellTimeMon::ConfigureMainMenu()
{
  TGMenuEntry *entry;
  g_rootFile->cd();
  TString dir = m_dirCaloCellTime;
  if(gDirectory->Get(dir.Remove(3,'/')) == 0){
    TIter next(fMenuCaloCell->GetListOfEntries());
    while(entry = (TGMenuEntry*)next()){
      if(entry->GetType() == kMenuEntry) 
	fMenuCaloCell->DisableEntry(entry->GetEntryId());
    }
    fMenuSummary->DisableEntry(M_CALOCELLTIME_SUMMARY);
  } else{
    g_rootFile->cd(m_dirCaloCellTime);
    if(gDirectory->Get(m_dirSummary) == 0)      fMenuSummary->DisableEntry(M_CALOCELLTIME_SUMMARY);
    if(gDirectory->Get(m_dirTime) == 0)         fMenuCaloCell->DisableEntry(M_CALOCELLTIME_TIMEV);
    if(gDirectory->Get(m_dirTimevsEnergy) == 0) fMenuCaloCell->DisableEntry(M_CALOCELLTIME_TIMEVSENERGY);
  }
}

//#####################################################################
// Configure horizontal frame to hide and show necessary
// combo boxes and buttons given the context 
void CaloCellTimeMon::ConfigureComboFrame()
{
  if(m_configOption == "SUMMARY"){
    if(fHFrame->IsVisible(fComboLayer) == 1)    fHFrame->HideFrame(fComboLayer);
    if(fHFrame->IsVisible(fDrawSingle) == 1)    fHFrame->HideFrame(fDrawSingle);
    if(fHFrame->IsVisible(fDrawLayers) == 1)    fHFrame->HideFrame(fDrawLayers);
  }
  
  //######################
  else{
    if(fHFrame->IsVisible(fComboLayer) == 0)    fHFrame->ShowFrame(fComboLayer);
    if(fHFrame->IsVisible(fDrawSingle) == 0)    fHFrame->ShowFrame(fDrawSingle);
    if(fHFrame->IsVisible(fDrawLayers) == 0)    fHFrame->ShowFrame(fDrawLayers);
    fDrawSingle->SetText(" Draw ");
    fDrawLayers->SetText(" Draw Layers");
  }
  fMain->Layout();
}

//#####################################################################
// Fill combobox for layer monitored
void CaloCellTimeMon::FillComboLayer()
{
  if(m_prevConfigOption == m_configOption)return;

  fComboLayer->RemoveEntries(0,fComboLayer->GetNumberOfEntries());

  g_rootFile->cd();
  g_rootFile->cd(m_dirName);
  TKey *key;
  TString lay;
  TSortedList *layList = new TSortedList();
  TIter next(gDirectory->GetListOfKeys());
  while(key = (TKey*)next()){
    TString name = key->GetName();
    lay = name(m_histoNameBase1.Length(),3);
    if(layList->FindObject(lay))continue;
    //cout << "layer = " << lay <<endl;
    layList->Add(new TObjString(lay));
  }
  
  TIter next(layList);
  int i=0;
  while(i<layList->GetSize()){
    fComboLayer->AddEntry(next()->GetName(), i++);
  }
  fComboLayer->Select(0);

  m_prevConfigOption = m_configOption;
}

//#####################################################################
// Fill combobox for layer monitored
void CaloCellTimeMon::HandleComboLayer()
{
  if(m_prevConfigOption == m_configOption)return;

  else fComboLayer->Select(0);
}

//#####################################################################
// Draw single histo
void CaloCellTimeMon::DrawSingle()
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

  TGTextLBEntry *tEntry = (TGTextLBEntry*)fComboLayer->GetSelectedEntry();
  TString lay = tEntry->GetText()->GetString();
  TString histoName;

  //######################
  fCanvas->Clear();
  fCanvas->Divide(1,1);
  histoName = m_histoNameBase1 + lay;

  g_rootFile->cd(m_dirName);
  gStyle->SetOptStat(0);
  fCanvas->cd(1);
  if(m_drawOption="TIME"){
    m_h1d = (TH1*)gDirectory->Get(histoName);
    gStyle->SetOptStat("emrou");
    m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
  }
  else if(m_drawOption="TIMEVSENERGY"){
    m_h2d = (TH2*)gDirectory->Get(histoName);
    gStyle->SetOptStat(0);
    m_h2d->Draw();
  }
  g_DrawRunInfo();
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw histos for the 4 layers at once
void CaloCellTimeMon::DrawLayers()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  fStatusBar->SetText("Plotting from DIRECTORY: " + m_dirName);
  
  TGTextLBEntry *tEntry;
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
    TString histoName1 = m_histoNameBase1  + lay;
    int padIndex = (i%nLayerPerCanvas) + 1;
    fCanvas->cd(padIndex);
    if(m_drawOption == "TIME"){
      m_h1d = (TH1*)gDirectory->Get(histoName1);
      gStyle->SetOptStat("emrou");
      m_h1d->Draw();  g_DrawReference(m_h1d);g_DrawRunInfo();
    } 
    else if(m_drawOption == "TIMEVSENERGY"){
      m_h2d = (TH2*)gDirectory->Get(histoName1);
      gStyle->SetOptStat(0);
      m_h2d->SetXTitle("Time"); m_h2d->SetYTitle("Energy");
      m_h2d->Draw("COL"); //DrawEtaPhiLegend();
    }
  } 
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw all summary plots
void CaloCellTimeMon::DrawSummary()
{
  g_rootFile->cd(); gDirectory->DeleteAll(); // delete prev objects in memory
  m_dirName = m_dirCaloCellTime + m_dirSummary;
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
      m_h1d->Draw(); g_DrawReference(m_h1d);g_DrawRunInfo();
    }
  }
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Draw legend for summary histograms for what each color mean
void CaloCellTimeMon::DrawSummaryLegend()
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
void CaloCellTimeMon::SetNCanvas(int ncanvas)
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
void CaloCellTimeMon::Save()
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
void CaloCellTimeMon::PrintThis()
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
void CaloCellTimeMon::HandleMainMenu(Int_t id)
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

  case M_CALOCELLTIME_SUMMARY:
    m_dirName = m_dirCaloCellTime + m_dirSummary;
    m_configOption = "SUMMARY";
    DrawSummary();
    break;
    
  case M_CALOCELLTIME_TIME:
    m_dirName = m_dirCaloCellTime + m_dirTime;
    m_histoNameBase1 = "Time";
    m_configOption = "TIME";
    m_drawOption = "TIME";
    break;

  case M_CALOCELLTIME_TIMEVSENERGY:
    m_dirName = m_dirCaloCellTime + m_dirTimevsEnergy;
    m_histoNameBase1 = "TimevsEnergy";
    m_configOption = "TIMEVSENERGY";
    m_drawOption = "TIMEVSENERGY";
    break;
  }
  
  fMain->SetWindowName("Calo Cell Time Monitoring >>> " + m_fileBaseName + ":" + m_dirName);
  //Fill combobox with layers monitored and draw first selection
  if(id > M_CALOCELLTIME_SUMMARY){
    FillComboLayer();
    DrawSingle();
  }
  ConfigureComboFrame();  // hide/show comboboxes/buttons given the context
}
