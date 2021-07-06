/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//#####################################################################
//## AUTHOR:      Christophe Le Maner (lemaner@cern.ch)
//## FILE:        CaloClusterShwrMonitoring.C
//## MODIFIED:    2006/09/21
//## DESCRIPTION: Build GUI for histos produced by CaloClusterShwrMon.
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
class CaloClusterShwrMon
{

  // cppcheck-suppress unknownMacro
  RQ_OBJECT("CaloClusterShwrMon") // necessary if not inherite from ROOT class

 protected:
  enum EPopupMenuEntry
    { // enumerators for signal/slot mechanism of main menu entries
      M_PS_OPEN,
      M_PS_CLOSE,
      M_PS_PRINT,
      M_PS_DELETE,

      M_CALOCLUSTERSHWR_ETAPHI,
      M_CALOCLUSTERSHWR_WIDTHETA,
      M_CALOCLUSTERSHWR_WIDTHPHI,
      M_CALOCLUSTERSHWR_LEADCELLEFRAC,
    };

 private:
  TGMainFrame          *fMain;
  TGDockableFrame      *fMenuDock;
  TGCanvas             *fGCanvas;
  TGCompositeFrame     *fContainer;
  TRootEmbeddedCanvas  *fECanvas[15];
  TGMenuBar            *fMenuBar;
  TGPopupMenu          *fMenuCaloCluster;
  TGPopupMenu          *fMenuPS;
  TGHorizontalFrame    *fHFrame;
  TGTextButton         *fSave, *fPrintThis;
  TGStatusBar          *fStatusBar;
  TCanvas              *fCanvas;
  TH1                  *m_h1d, *m_href;
  TH2                  *m_h2d;
  TPostScript          *m_psFile;

  TString              m_fileBaseName;
  TString              m_dirName, m_histoNameBase1;
  TString              m_dirCaloClusterShwr;
  TString              m_configOption, m_prevConfigOption, m_drawOption;
  TString              m_psFileName;
  
  int                  m_canvasW, m_canvasH;
  
 public:
  CaloClusterShwrMon(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~CaloClusterShwrMon();
  CaloClusterShwrMon(const CaloClusterShwrMon&) = delete;
  CaloClusterShwrMon& operator= (const CaloClusterShwrMon&) = delete;

  void CloseMainWindow();
  
  void ConfigureMainMenu();
  void ConfigureComboFrame();
  void HandleMainMenu(Int_t id);

  void SetNCanvas(int ncanvas);
  void DrawSingle();

  void Save();
  void PrintThis();
};

//#####################################################################
//#####################################################################
// Class constructor
CaloClusterShwrMon::CaloClusterShwrMon(const TGWindow *p, UInt_t w, UInt_t h)
  : fMain (nullptr),
    fMenuDock (nullptr),
    fGCanvas (nullptr),
    fContainer (nullptr),
    fECanvas (),
    fMenuBar (nullptr),
    fMenuCaloCluster (nullptr),
    fMenuPS (nullptr),
    fHFrame (nullptr),
    fSave (nullptr),
    fPrintThis (nullptr),
    fStatusBar (nullptr),
    fCanvas (nullptr),
    m_h1d (nullptr),
    m_href (nullptr),
    m_h2d (nullptr),
    m_psFile (nullptr),
    
    m_fileBaseName (gSystem->BaseName(g_rootFile->GetName())),
    m_dirCaloClusterShwr ("/CaloClusterShwrMon/"),
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
  fMain->Connect("CloseWindow()", "CaloClusterShwrMon", this, "CloseMainWindow()");

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
  fMenuPS->Connect("Activated(Int_t)", "CaloClusterShwrMon", this, "HandleMainMenu(Int_t)");

  // create "CaloClusterShwr" menu
  fMenuCaloCluster = new TGPopupMenu(gClient->GetRoot());
  fMenuCaloCluster->AddEntry("EtaPhi", M_CALOCLUSTERSHWR_ETAPHI);
  fMenuCaloCluster->AddEntry("Width Eta", M_CALOCLUSTERSHWR_WIDTHETA);
  fMenuCaloCluster->AddEntry("Width Phi", M_CALOCLUSTERSHWR_WIDTHPHI);
  fMenuCaloCluster->AddEntry("Lead cell Efrac", M_CALOCLUSTERSHWR_LEADCELLEFRAC);
  fMenuCaloCluster->Connect("Activated(Int_t)", "CaloClusterShwrMon", this, "HandleMainMenu(Int_t)");

  // disable menu entries for user mode
  //if(g_expertMode == false){
  //fMenuSummary->DisableEntry(M_CALOCLUSTERSHOWER_SUMMARY);
  // }

  // disable menu entries for directories don't exist in root file
  ConfigureMainMenu();

  // add menu bar with popup menus to the dockable frame
  fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
  fMenuDock->AddFrame(fMenuBar, new TGLayoutHints(kLHintsTop | kLHintsExpandX));
  fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 2, 2);
  fMenuBar->AddPopup("&PostScript", fMenuPS, fMenuBarItemLayout);
  fMenuBar->AddPopup("Calo&ClusterShwr", fMenuCaloCluster, fMenuBarItemLayout);

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
  fSave = new TGTextButton(fHFrame," Save ");
  fSave->Connect("Clicked()", "CaloClusterShwrMon", this, "Save()");
  fHFrame->AddFrame(fSave, hframeLayout);
  fPrintThis = new TGTextButton(fHFrame," PrintThis ");
  fPrintThis->Connect("Clicked()", "CaloClusterShwrMon", this, "PrintThis()");
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

  // initialize layout of the main window and map the GUI
  fMain->Layout();
  fMain->MapWindow();
}

//#####################################################################
// Class destructor
CaloClusterShwrMon::~CaloClusterShwrMon()
{
  // fMain->Cleanup() can't delete sub-sub level widgets
  // so fContainer->Cleanup() is a must here
  fContainer->Cleanup();
  fMain->Cleanup();
}

//#####################################################################
// Close main window upon click on "X"
void CaloClusterShwrMon::CloseMainWindow()
{
  delete this;
}

//#####################################################################
// Configure menu bar to disable entries 
// for directories don't exist in the root file
void CaloClusterShwrMon::ConfigureMainMenu()
{
  TGMenuEntry *entry;
  g_rootFile->cd();
  TString dir = m_dirCaloClusterShwr;
  if(gDirectory->Get(dir.Remove(3,'/')) == 0){
    TIter next(fMenuCaloCluster->GetListOfEntries());
    while(entry = (TGMenuEntry*)next()){
      if(entry->GetType() == kMenuEntry) 
	fMenuCaloCluster->DisableEntry(entry->GetEntryId());
    }
  }
}

//#####################################################################
// Configure horizontal frame to hide and show necessary
// combo boxes and buttons given the context 
void CaloClusterShwrMon::ConfigureComboFrame()
{
  fMain->Layout();
}

//#####################################################################
// Draw single histo
void CaloClusterShwrMon::DrawSingle()
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
  
  //######################
  fCanvas->Clear();
  fCanvas->Divide(1,1);
  TString histoName = m_histoNameBase1;
  
  g_rootFile->cd(m_dirName);
  gStyle->SetOptStat(0);
  if(m_configOption=="ETAPHI"){
    m_h2d = (TH2*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    m_h2d->Draw();
  }
  else if(m_configOption="WIDTHETA"){
    m_h1d = (TH1*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    cout << "histoname=" << histoName <<endl;
    m_h1d->Draw();
  }
  else if(m_configOption="WIDTHPHI"){
    m_h1d = (TH1*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    cout << "histoname=" << histoName <<endl;
    m_h1d->Draw();
  }
  else if(m_configOption="LEADCELLEFRAC"){
    m_h1d = (TH1*)gDirectory->Get(histoName);
    fCanvas->cd(1);
    cout << "histoname=" << histoName <<endl;
    m_h1d->Draw();
  }
  g_DrawRunInfo();
  fCanvas->cd(); fCanvas->Modified(); fCanvas->Update();
}

//#####################################################################
// Set number of canvases in the container for multiple histograms
void CaloClusterShwrMon::SetNCanvas(int ncanvas)
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
void CaloClusterShwrMon::Save()
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
void CaloClusterShwrMon::PrintThis()
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
void CaloClusterShwrMon::HandleMainMenu(Int_t id)
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
    
  case M_CALOCLUSTERSHWR_ETAPHI:
    m_dirName = m_dirCaloClusterShwr;
    cout << "m_dirName = " << m_dirName << endl;
    m_histoNameBase1 = "EtaPhi";
    m_configOption = "ETAPHI";
    break;
    
  case M_CALOCLUSTERSHWR_WIDTHETA:
    m_dirName = m_dirCaloClusterShwr;
    m_histoNameBase1 = "widthEta";
    m_configOption = "WIDTHETA";
    break;
    
  case M_CALOCLUSTERSHWR_WIDTHPHI:
    m_dirName = m_dirCaloClusterShwr;
    m_histoNameBase1 = "widthPhi";
    m_configOption = "WIDTHPHI";
    break;
    
  case M_CALOCLUSTERSHWR_LEADCELLEFRAC:
    m_dirName = m_dirCaloClusterShwr;
    m_histoNameBase1 = "leadCellEfrac";
    m_configOption = "LEADCELLEFRAC";
    break;
  }
  fMain->SetWindowName("Calo Cluster Shower Monitoring >>> " + m_fileBaseName + ":" + m_dirName);
  // Draw first selection
  DrawSingle();
  ConfigureComboFrame();  // hide/show comboboxes/buttons given the context
}
