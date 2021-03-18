/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/*******************************************************************************

 TileCosmicView
 Root Macro to visualize cosmic events in the TileCal

 Author   : Luciano Manhaes de Andrade Filho
 Institute: Universidade Federal do Rio de Janeiro
 email    : luciano.ma.filho@gmail.com
 date     : 13/02/2007

*******************************************************************************/

#include <iostream>
#include <vector>

#include "TTree.h"
#include "TH1F.h"
#include "TGFrame.h"
#include "TGTextEntry.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TGTextView.h"
#include "TRootEmbeddedCanvas.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TBox.h"
#include "TColor.h"
#include "RQ_OBJECT.h"
#include "TGFileDialog.h"
#include "TFile.h"

using namespace std;

/****************************** global variables ******************************/

const char *filetypes[] = {"ROOT files", "*.root", 0, 0};

Float_t a_eneg[64][48];
Float_t c_eneg[64][48];
Float_t d_eneg[64][48];
Float_t e_eneg[64][48];

Int_t a_sample[64][48][9];
Int_t c_sample[64][48][9];
Int_t d_sample[64][48][9];
Int_t e_sample[64][48][9];

Double_t position [3];
Double_t direction[3];
vector<float> *positionv  = 0;
vector<float> *directionv = 0;

Double_t phys_pulse_amp [401];
Double_t phys_pulse_time[401];

Int_t evt;
Int_t ev_num = 0;

TTree *th1000;
TTree *tFit;

int n_modules_a = 0;
int n_modules_c = 0;
int n_modules_d = 0;
int n_modules_e = 0;

vector<int> a_modules;
vector<int> c_modules;
vector<int> d_modules;
vector<int> e_modules;

/****************************** class definition ******************************/

class MyMainFrame 
{
  // cppcheck-suppress unknownMacro
	RQ_OBJECT("MyMainFrame")

private:

	bool has_track;
	bool tmf_is_vector;
	bool read_pulse;
	TH1F *ch[2];

	TGMainFrame         *fMain;
	TGTextEntry         *fFileName;
	TGCheckButton       *fViewPMT;
	TGNumberEntry       *fThreshold;
	TGNumberEntry       *fMaxVal;
	TGCheckButton       *fAnimate;
	TGNumberEntry       *fEventNum;
	TGLabel             *fLabelNumOfEv;
	TGTextView          *fEditor;
	TRootEmbeddedCanvas *fEcanvas;

	void ViewPMT();
	void Open   ();
	void Hough  ();
	void Animate();
	void Samples();
	void EventNum(Long_t value);
	void Thresh  (Long_t value);
	void MaxVal  (Long_t value);
	void Selected();
	void DrawPalette();
	void AddText(char *text);

public:

	MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h);
	virtual ~MyMainFrame();

};

/****************************** global functions ******************************/

void TileCosmicView()
{
	gROOT->ProcessLine(".L TileGeoInterface.C");
	new MyMainFrame(gClient->GetRoot(),300,300);
	tgInit();
}

// Physics Signal Pulse
Double_t PhysPulse(Double_t *xtime, Double_t *par)
{
	Int_t lookup;
	Double_t xamp, xpulse, tdiv, timelocal;

	tdiv = phys_pulse_time[1] - phys_pulse_time[0];
	timelocal = (xtime[0]-4+0.5)*25.0 - par[0]; 
	lookup = (Int_t)((timelocal-phys_pulse_time[0])/tdiv) ;

	if (lookup < 0  ) lookup = 0;
	if (lookup >= 400) lookup = 399;

	xpulse = phys_pulse_amp[lookup] + ((phys_pulse_amp[lookup+1]-phys_pulse_amp[lookup])/tdiv)*(timelocal-phys_pulse_time[lookup]); 
	xamp   = par[1] + par[2]*xpulse;
	return xamp;
}

/**************************** class implementation ****************************/

MyMainFrame::MyMainFrame(const TGWindow *p,UInt_t w,UInt_t h) 
{

	fMain = new TGMainFrame(p,w,h);

	TGGroupFrame *FramePalette = new TGGroupFrame(fMain,"Palette"      , kHorizontalFrame);
	TGGroupFrame *FrameControl = new TGGroupFrame(fMain,"Event Control",   kVerticalFrame);
	TGGroupFrame *FrameInput   = new TGGroupFrame(fMain,"Input"        , kHorizontalFrame);

	TGHorizontalFrame *FrameExit       = new TGHorizontalFrame(fMain       ,300,40);
	TGHorizontalFrame *FrameSel        = new TGHorizontalFrame(fMain       ,300,40);
	TGHorizontalFrame *FrameEvNum      = new TGHorizontalFrame(FrameControl,300,40);
	TGHorizontalFrame *FrameThresh     = new TGHorizontalFrame(FrameControl,300,40);
	TGHorizontalFrame *FrameOpenDialog = new TGHorizontalFrame(FrameInput  ,300,40);

	TGTextButton *ButtonOpen   = new TGTextButton(FrameOpenDialog,"   &Open...  "    );
	TGTextButton *ButtonSel    = new TGTextButton(FrameSel       , "&Show Info"      );
	TGTextButton *ButtonSample = new TGTextButton(FrameSel       , "&Plot Samples"   );
	TGTextButton *ButtonHough  = new TGTextButton(FrameSel       , "&Hough Transform");
	TGTextButton *ButtonExit   = new TGTextButton(FrameExit,"     &Exit     ","gApplication->Terminate(0)");

	        fLabelNumOfEv = new TGLabel(FrameEvNum     , new TGString("of 0000"    ));
	TGLabel *LabelEvNum   = new TGLabel(FrameEvNum     , new TGString("Entry:"     ));
	TGLabel *LabelThresh  = new TGLabel(FrameThresh    , new TGString("Threshold:" ));
	TGLabel *LabelMeV     = new TGLabel(FrameThresh    , new TGString("MeV"        ));
	TGLabel *MinVal       = new TGLabel(FramePalette   , new TGString("From 0.0"   ));
	TGLabel *LTo          = new TGLabel(FramePalette   , new TGString("to"         ));
	TGLabel *LMev         = new TGLabel(FramePalette   , new TGString("MeV"        ));
	TGLabel *LabelNTuple  = new TGLabel(FrameOpenDialog, new TGString("nTuple"     ));

	fEventNum  = new TGNumberEntry(FrameEvNum  ,    0, 9, -1, TGNumberFormat::kNESInteger, 
                                   TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax,    0,   200);
	fThreshold = new TGNumberEntry(FrameThresh ,  100, 9, -1, TGNumberFormat::kNESInteger, 
                                   TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax, -900,100000);
	fMaxVal    = new TGNumberEntry(FramePalette, 2000, 5, -1, TGNumberFormat::kNESInteger, 
                                   TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMinMax,    0, 10000);

	fFileName  = new TGTextEntry(FrameOpenDialog, new TGTextBuffer(5));

	fViewPMT   = new TGCheckButton(FrameThresh,"&View PMTs"     ,1);
	fAnimate   = new TGCheckButton(FrameEvNum ,"&Animate     "  ,0);

	fEditor    = new TGTextView(fMain, 100, 300, kSunkenFrame | kDoubleBorder);

	fEcanvas   = new TRootEmbeddedCanvas("Palette",FramePalette,130,22);

	  ButtonOpen->Connect("Clicked()"       ,"MyMainFrame",this,    "Open()"      );
	   ButtonSel->Connect("Clicked()"       ,"MyMainFrame",this,"Selected()"      );
	ButtonSample->Connect("Clicked()"       ,"MyMainFrame",this, "Samples()"      );
	 ButtonHough->Connect("Clicked()"       ,"MyMainFrame",this,   "Hough()"      );
	    fAnimate->Connect("Clicked()"       ,"MyMainFrame",this, "Animate()"      );
	    fViewPMT->Connect("Clicked()"       ,"MyMainFrame",this, "ViewPMT()"      );
	   fEventNum->Connect("ValueSet(Long_t)","MyMainFrame",this,"EventNum(Long_t)");
	  fThreshold->Connect("ValueSet(Long_t)","MyMainFrame",this,  "Thresh(Long_t)");
	     fMaxVal->Connect("ValueSet(Long_t)","MyMainFrame",this,  "MaxVal(Long_t)");

	     FrameInput->AddFrame(FrameOpenDialog, new TGLayoutHints(kLHintsExpandX           ,-2, -2, 4, -6));
	   FrameControl->AddFrame(FrameEvNum     , new TGLayoutHints(kLHintsExpandX           , 2,  2, 2,  2));
	   FrameControl->AddFrame(FrameThresh    , new TGLayoutHints(kLHintsExpandX           , 2,  2, 2,  2));
	       FrameSel->AddFrame(ButtonSel      , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  0, 2,  2));
	       FrameSel->AddFrame(ButtonSample   , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  0, 2,  2));
	       FrameSel->AddFrame(ButtonHough    , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  0, 2,  2));
	FrameOpenDialog->AddFrame(LabelNTuple    , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  0, 5,  2));
	FrameOpenDialog->AddFrame(ButtonOpen     , new TGLayoutHints(kLHintsTop | kLHintsRight, 2,  0, 2,  2));
	FrameOpenDialog->AddFrame(fFileName      , new TGLayoutHints(kLHintsExpandX           , 5,  2, 2,  2));
	     FrameEvNum->AddFrame(LabelEvNum     , new TGLayoutHints(kLHintsTop | kLHintsLeft ,25,  2, 5,  2));
	     FrameEvNum->AddFrame(fEventNum      , new TGLayoutHints(kLHintsTop | kLHintsLeft , 3,  5, 3,  4));
	     FrameEvNum->AddFrame(fLabelNumOfEv  , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  2, 6,  2));
	     FrameEvNum->AddFrame(fAnimate       , new TGLayoutHints(kLHintsTop | kLHintsRight, 5, -1, 5,  2));
	    FrameThresh->AddFrame(LabelThresh    , new TGLayoutHints(kLHintsTop | kLHintsLeft , 0,  2, 5,  2));
	    FrameThresh->AddFrame(fThreshold     , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  5, 3,  4));
	    FrameThresh->AddFrame(fViewPMT       , new TGLayoutHints(kLHintsTop | kLHintsRight, 5,  0, 5,  2));
	    FrameThresh->AddFrame(LabelMeV       , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2, 20, 7,  2));
	      FrameExit->AddFrame(ButtonExit     , new TGLayoutHints(kLHintsTop | kLHintsRight, 5,  5, 3,  4));
	   FramePalette->AddFrame(MinVal         , new TGLayoutHints(kLHintsTop | kLHintsLeft , 2,  2, 11, 2));
	   FramePalette->AddFrame(LMev           , new TGLayoutHints(kLHintsTop | kLHintsRight, 2,  2, 11, 2));
	   FramePalette->AddFrame(fMaxVal        , new TGLayoutHints(kLHintsTop | kLHintsRight, 2,  2, 8,  2));
	   FramePalette->AddFrame(LTo            , new TGLayoutHints(kLHintsTop | kLHintsRight, 2,  2, 11, 2));
	   FramePalette->AddFrame(fEcanvas       , new TGLayoutHints(kLHintsExpandX           , 2,  2, 8,  2));

	fMain->AddFrame(FrameInput     , new TGLayoutHints(kLHintsExpandX               ,2,2,-2,2));
	fMain->AddFrame(FrameControl   , new TGLayoutHints(kLHintsExpandX               ,2,2,-2,2));
	fMain->AddFrame(FrameSel       , new TGLayoutHints(kLHintsExpandX               ,2,2, 2,2));
	fMain->AddFrame(FrameExit      , new TGLayoutHints(kLHintsExpandX|kLHintsBottom ,2,2, 2,2));
	fMain->AddFrame(FramePalette   , new TGLayoutHints(kLHintsExpandX|kLHintsBottom ,2,2, 2,2));
	fMain->AddFrame(fEditor        , new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,2,2, 2,2));

	fMain->SetWindowName("TileCal 3D View");
	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();
	fMain->SetWMPosition(792,  0);
	fMain->SetWMSize    (350,670);

	DrawPalette();

	read_pulse = false;

	// histograms creation
	gStyle->SetOptStat(000000);
	gStyle->SetOptFit(101);
	gStyle->SetTitleFontSize(0.08);
	gStyle->SetStatW(0.15);
	gStyle->SetStatH(0.25);
	
	ch[0] = new TH1F("h1", "PMT right", 9, 0, 9);
	ch[1] = new TH1F("h2", "PMT left" , 9, 0, 9);
}

MyMainFrame::~MyMainFrame() 
{
	fMain->Cleanup();
	delete fMain;
}

void MyMainFrame::DrawPalette()
{
	TCanvas *fCanvas = fEcanvas->GetCanvas();
	fCanvas->cd();

	Float_t r,g,b;
	for (int i = 0; i < 256; i++)
	{
		TBox *box = new TBox(i/256.0, 0, (i+1)/256.0, 1);

		r = 0.5 + 0.5*(255-i)/255.0;
		if (i > 200) g = 0.0; else g = 1.0 - 1.0*i/200.0;
		b = 0.0;

		box->SetFillColor(TColor::GetColor(r,g,b));
		box->Draw();
	}

	fCanvas->SetEditable(kFALSE);
	fCanvas->Update();
}

void MyMainFrame::ViewPMT()
{
	tgViewPMT(fViewPMT->IsDown());
}

void MyMainFrame::Animate()
{
	tgAnimate(fAnimate->IsDown());
}

void MyMainFrame::Open()
{
	TGFileInfo   *fi = new TGFileInfo;
	fi->fFileTypes   = filetypes;
	TGFileDialog *fd = new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, fi);

	TFile *f = new TFile(fi->fFilename);

	            th1000   = (TTree*)f->Get("h1000");
	if(!th1000) th1000   = (TTree*)f->Get("TileRec/h1000");

	           tFit = (TTree*)f->Get("TileRec/ComTimeTileMuon");
	if (!tFit) tFit = (TTree*)f->Get("CollectionTree");
	if (!tFit) tFit = (TTree*)f->Get("CBNT/3333");

	has_track = tFit != 0;

	/***************************** scan modules *******************************/

	string branch_name;

	a_modules.clear();
	c_modules.clear();
	d_modules.clear();
	e_modules.clear();

	n_modules_a = 0;
	n_modules_c = 0;
	n_modules_d = 0;
	n_modules_e = 0;

	for (int i = 0; i < 64; i++)
	{
		if (i < 9)
			branch_name = Form("EfitA0%d", i+1);
		else
			branch_name = Form("EfitA%d" , i+1);

		if (th1000->GetBranch(branch_name.c_str()))
		{
			n_modules_a++;
			a_modules.push_back(i+1);
			 tgEnableModule(1, i+1, true );
		}
		else tgEnableModule(1, i+1, false);

		if (i < 9)
			branch_name = Form("EfitC0%d", i+1);
		else
			branch_name = Form("EfitC%d" , i+1);

		if (th1000->GetBranch(branch_name.c_str()))
		{
			n_modules_c++;
			c_modules.push_back(i+1);
			 tgEnableModule(2, i+1, true );
		}
		else tgEnableModule(2, i+1, false);

		if (i < 9)
			branch_name = Form("EfitD0%d", i+1);
		else
			branch_name = Form("EfitD%d" , i+1);

		if (th1000->GetBranch(branch_name.c_str()))
		{
			n_modules_d++;
			d_modules.push_back(i+1);
			 tgEnableModule(3, i+1, true );
		}
		else tgEnableModule(3, i+1, false);

		if (i < 9)
			branch_name = Form("EfitE0%d", i+1);
		else
			branch_name = Form("EfitE%d" , i+1);

		if (th1000->GetBranch(branch_name.c_str()))
		{
			n_modules_e++;
			e_modules.push_back(i+1);
			 tgEnableModule(4, i+1, true );
		}
		else tgEnableModule(4, i+1, false);
	}

	/************************** set branch address ****************************/

	for (int k = 0; k < n_modules_a; k++)
	{
		if (a_modules[k] < 10)
			branch_name = Form("EfitA0%d", a_modules[k]);
		else
			branch_name = Form("EfitA%d" , a_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &a_eneg[k]);

		if (a_modules[k] < 10)
			branch_name = Form("SampleA0%d", a_modules[k]);
		else
			branch_name = Form("SampleA%d" , a_modules[k]);	

		th1000->SetBranchAddress(branch_name.c_str(), &a_sample[k]);
	}

	for (int k = 0; k < n_modules_c; k++)
	{
		if (c_modules[k] < 10)
			branch_name = Form("EfitC0%d", c_modules[k]);
		else
			branch_name = Form("EfitC%d" , c_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &c_eneg[k]);

		if (c_modules[k] < 10)
			branch_name = Form("SampleC0%d", c_modules[k]);
		else
			branch_name = Form("SampleC%d" , c_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &c_sample[k]);
	}

	for (int k = 0; k < n_modules_d; k++)
	{
		if (d_modules[k] < 10)
			branch_name = Form("EfitD0%d", d_modules[k]);
		else
			branch_name = Form("EfitD%d" , d_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &d_eneg[k]);

		if (d_modules[k] < 10)
			branch_name = Form("SampleD0%d", d_modules[k]);
		else
			branch_name = Form("SampleD%d" , d_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &d_sample[k]);
	}

	for (int k = 0; k < n_modules_e; k++)
	{
		if (e_modules[k] < 10)
			branch_name = Form("EfitE0%d", e_modules[k]);
		else
			branch_name = Form("EfitE%d" , e_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(),   &e_eneg[k]);

		if (e_modules[k] < 10)
			branch_name = Form("SampleD0%d", e_modules[k]);
		else
			branch_name = Form("SampleD%d" , e_modules[k]);

		th1000->SetBranchAddress(branch_name.c_str(), &e_sample[k]);
	}

	if (tFit)
	{
		     if (tFit->GetBranch("tmf_pos"               )) tFit->SetBranchAddress("tmf_pos"                , &position );
		else if (tFit->GetBranch("tmf_position"          )) tFit->SetBranchAddress("tmf_position"           , &position );
		else if (tFit->GetBranch("TileMuonFitterPosition")) tFit->SetBranchAddress("TileMuonFitterPosition" , &positionv);
		else if (tFit->GetBranch("position"              )) tFit->SetBranchAddress("position"               , &position );
		
		     if (tFit->GetBranch("tmf_direction"          )) tFit->SetBranchAddress("tmf_direction"           , &direction );
		else if (tFit->GetBranch("TileMuonFitterDirection")) tFit->SetBranchAddress("TileMuonFitterDirection" , &directionv);
		else if (tFit->GetBranch("direcion"               )) tFit->SetBranchAddress("direction"               , &direction );
		else has_track = false;
	}
	
	tmf_is_vector = tFit->GetBranch("TileMuonFitterDirection");

	th1000->SetBranchAddress("Evt", &evt);

	/******************************* update GUI *******************************/

	ev_num = tFit->GetEntries();
	string text = Form("of %d",ev_num);
	fLabelNumOfEv->SetText(text.c_str());
	fFileName->SetText(fi->fFilename);
	fEventNum->SetNumber(0);
	fEventNum->SetLimitValues(0,ev_num-1);
	EventNum(0);
}

void MyMainFrame::EventNum(Long_t value)
{
	int ev = (int)fEventNum->GetNumber();

	th1000->GetEntry(ev);
	if (has_track) tFit->GetEntry(ev);

	for (int k = 0; k < n_modules_a; k++) tgSetValue(1, a_modules[k], a_eneg[k]);
	for (int k = 0; k < n_modules_c; k++) tgSetValue(2, c_modules[k], c_eneg[k]);
	for (int k = 0; k < n_modules_d; k++) tgSetValue(3, d_modules[k], d_eneg[k]);
	for (int k = 0; k < n_modules_e; k++) tgSetValue(4, e_modules[k], e_eneg[k]);

	if (has_track)
	{
		if (tmf_is_vector)
			tgDrawTrackv(positionv, directionv);
		else
			tgDrawTrack (position , direction );
	}
	else tgUpdate();
}

void MyMainFrame::Thresh(Long_t value)
{
	tgThreshold(fThreshold->GetNumber());
}

void MyMainFrame::MaxVal(Long_t value)
{
	tgSetMaxValue(fMaxVal->GetNumber());
}

void MyMainFrame::Selected()
{
	string sec_name = "", cell_name = "";
	int sector, drawer, pmt1, pmt2;
	bool isMBTS;
	bool ok = tgGetSelected(sector, drawer, pmt1, pmt2, &sec_name, &cell_name);

	if (!ok)
	{
		AddText("Oops! You should select a cell first");
		return;
	}

	Float_t en_pmt2 = 0;
	if (pmt2 != 1 || sector > 2) 
		en_pmt2 = tgValue[sector-1][drawer-1][pmt2-1]*1000;
	else
		en_pmt2 = tgValue[   1    ][drawer-1][pmt2-1]*1000;

	if(cell_name.substr(0,4)=="MBTS") 
	{
		pmt2    = 1;
		en_pmt2 = 0;
	}

	Float_t en_pmt1 = tgValue[sector-1][drawer-1][pmt1-1]*1000;

	Float_t energy  = en_pmt1+en_pmt2;

	stringstream ss;

	ss << "-------------- #Event " << evt << " --------------" << endl << endl;

	ss << "Module           : " <<  sec_name.c_str() << drawer << endl;
	ss << "Cell             : " << cell_name.c_str() << endl;
	ss << "PMT    right     : " << pmt1    << endl;
	ss << "PMT    left      : " << pmt2    << endl;
	ss << "Energy right(MeV): " << en_pmt1 << endl;
	ss << "Energy left (MeV): " << en_pmt2 << endl;
	ss << "Total       (MeV): " << energy  << endl;

	AddText(ss.str().c_str());
}

void MyMainFrame::Samples()
{

	/******************************* get info *********************************/

	string sec_name = "", cell_name = "";
	int sector, drawer, pmt1, pmt2;
	bool ok = tgGetSelected(sector, drawer, pmt1, pmt2, &sec_name, &cell_name);

	if (!ok)
	{
		AddText("Oops! You should select a cell first");
		return;
	}

	if(cell_name.substr(0,4)=="MBTS") pmt2 = 1;

	/************************** fill graphic samples **************************/

	int y[2][9];

	int index = 0;
	if      (sector == 1)
	{
		for (int i = 0; i < n_modules_a; i++) if (drawer == a_modules[i]) index = i;
	}
	else if (sector == 2)
	{
		for (int i = 0; i < n_modules_c; i++) if (drawer == c_modules[i]) index = i;
	}
	else if (sector == 3)
	{
		for (int i = 0; i < n_modules_d; i++) if (drawer == d_modules[i]) index = i;
	}
	else if (sector == 4)
	{
		for (int i = 0; i < n_modules_e; i++) if (drawer == e_modules[i]) index = i;
	}

	for (int i = 0; i < 9; i++)
	{
		if (sector == 1)
		{
			if (pmt1 == 1)
			{
				if (tgModEnabled[0][drawer-1])
					y[0][i] = a_sample[index][pmt1-1][i];
				else
					y[0][i] = 0;

				if (tgModEnabled[1][drawer-1])
					y[1][i] = c_sample[index][pmt1-1][i];
				else
					y[1][i] = 0;
			}
			else
			{
				y[0][i] = a_sample[index][pmt1-1][i];
				y[1][i] = a_sample[index][pmt2-1][i];
			}
		}
		else if (sector == 2)
		{
			y[0][i] = c_sample[index][pmt1-1][i];
			y[1][i] = c_sample[index][pmt2-1][i];
		}
		else if (sector == 3)
		{
			y[0][i] = d_sample[index][pmt1-1][i];
			y[1][i] = d_sample[index][pmt2-1][i];
		}
		else if (sector == 4)
		{
			y[0][i] = e_sample[index][pmt1-1][i];
			y[1][i] = e_sample[index][pmt2-1][i];
		}
	}

	/***************************** perform the fit ****************************/

	// get the pulse shape
	if (!read_pulse) 
	{
		read_pulse = true;

		ifstream pulsefile;
		pulsefile.open("pulsehi_physics.dat");

		for(Int_t i = 0; i < 401; i++)
		{
			pulsefile >> phys_pulse_time[i] >> phys_pulse_amp[i];
		}

		pulsefile.close();
	}

	int pmt[2]; pmt[0] = pmt1; pmt[1] = pmt2;

	// canvas
	stringstream title;
	title << "Module " << sec_name.c_str() << drawer << " - Cell " << cell_name.c_str() << " - #Event " << evt; 
	cSamples = new TCanvas("Samples", title.str().c_str(), 500, 500);
	cSamples->Divide(1,2);

	//fit
	TF1 *plotphyspulse = new TF1("plotphyspulse", PhysPulse, 0.0, 9, 3);

	for (Int_t chan = 0; chan < 2; chan++)
	{
		cSamples->cd(chan+1);
		ch[chan]->Reset();

		for (Int_t samp = 0; samp < 9; samp++)
		{
			ch[chan]->Fill(samp , y[chan][samp]);
			ch[chan]->SetBinError(samp+1,1.5);
		}

		//ch[chan]->SetMaximum(200);
		//ch[chan]->SetMinimum(80);
		ch[chan]->Draw("HIST");
		ch[chan]->GetXaxis()->SetLabelSize(0.06);
		ch[chan]->GetYaxis()->SetLabelSize(0.06);

		plotphyspulse->SetParameters(0.0,10.0,100.0);

		ch[chan]->Fit("plotphyspulse","Q");     
	}
}

void MyMainFrame::AddText(char *text)
{
	TGText txt;
	txt.LoadBuffer(text);
	fEditor->AddText(&txt);
}

void MyMainFrame::Hough()
{
	tgComputeHough(&a_modules, &c_modules, &d_modules, &e_modules);
}
