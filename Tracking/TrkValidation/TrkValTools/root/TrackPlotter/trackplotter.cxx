/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// To run it do: make trackplotter; trackplotter

#include "trackplotter.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TGTableLayout.h"
#include "TGFrame.h"
#include "TList.h"
#include "TGWindow.h"
#include "TRootEmbeddedCanvas.h"
#include "TEllipse.h"
#include "TBox.h"
#include "TArrow.h"
#include "TPolyLine.h"
#include "TGButton.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TGDoubleSlider.h"
#include "TGaxis.h"
#include "TColor.h"
#include "Riostream.h"

#include "TEnv.h"

#include "PlotValNtuple.h"

Viewer::Viewer(const TGWindow *win, PlotValNtuple* ntuplePlotter) : TGMainFrame(win,800,800),
        //m_trackNtuple(trackNtuple),
        //m_rioNtuple(rioNtuple),
        m_ntuplePlotter(ntuplePlotter),
        //m_entryNumber(0),
        m_singleTrackMode(true),
        m_equalScaling(false),
        m_showDAFinfo(false) {
    const UInt_t max_size = 800;

    // Memory management isn't the best in this class, but make a half
    // hearted attempts not to gush too much....
    fCleanup = new TList;

    TGCompositeFrame *table = new TGCompositeFrame(this,350,350,kSunkenFrame);
    fCleanup->Add(table);

    TGTableLayout* tlo = new TGTableLayout(table, 10, 6);
    //TGTableLayout* tlo = new TGTableLayout(table, 9, 6);
    table->SetLayoutManager(tlo);
    TGLayoutHints* loh = new TGLayoutHints(kLHintsTop|kLHintsLeft|
                                           kLHintsExpandX|kLHintsExpandY);
    AddFrame(table,loh);

    TGTableLayoutHints* tloh;

    // The Canvas
    fMainEmbeddedCanvas =
        new TRootEmbeddedCanvas("Shapes",table,max_size,max_size);
    tloh = new TGTableLayoutHints(2,5,2,6,
                                  kLHintsExpandX|kLHintsExpandY |
                                  kLHintsShrinkX|kLHintsShrinkY |
                                  kLHintsFillX|kLHintsFillY);
    table->AddFrame(fMainEmbeddedCanvas,tloh);

    fCanvas = fMainEmbeddedCanvas->GetCanvas();

    // The sliders
    fHSlider = new TGDoubleHSlider(table,max_size,kDoubleScaleBoth,100,
                                   kHorizontalFrame,GetDefaultFrameBackground(),
                                   kFALSE, kTRUE);

    tloh = new TGTableLayoutHints(2,5,0,1,
                                  kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
    table->AddFrame(fHSlider,tloh);
    fHSlider->Connect("PositionChanged()","Viewer",this,"DoSlider()");

    fVSlider = new TGDoubleVSlider(table,max_size,kDoubleScaleBoth,200,
                                  kVerticalFrame,GetDefaultFrameBackground(),
                                  kTRUE,kTRUE);
    Float_t RangeMin=-1100.; Float_t RangeMax=1100.;
    fVSlider->SetRange(RangeMin, RangeMax);
    fHSlider->SetRange(RangeMin, RangeMax);
    tloh = new TGTableLayoutHints(0,1,2,6,
                                  kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
    table->AddFrame(fVSlider,tloh);
    fVSlider->Connect("PositionChanged()","Viewer",this,"DoSlider()");

    // The scales
    fHScaleCanvas =
        new TRootEmbeddedCanvas("H Scale",table,max_size,50);
    tloh = new TGTableLayoutHints(2,5,1,2,
                                  kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
    table->AddFrame(fHScaleCanvas,tloh);
    fHScaleCanvas->GetCanvas()->cd();

    fHScaleCanvas->GetCanvas()->Range(0,0,1,1);
    fHScale = new TGaxis(0.0,0.5, 1.0,0.5, 0.0,100.0, 510, "-");
    fHScale->SetLabelSize(0.4);
    fHScale->SetName("X Scale");
    fHScale->Draw();
    fHScaleCanvas->GetCanvas()->SetEditable(kFALSE);

    fVScaleCanvas =
        new TRootEmbeddedCanvas("V Scale",table,50,max_size);
    tloh = new TGTableLayoutHints(1,2,2,6,
                                  kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
    table->AddFrame(fVScaleCanvas,tloh);
    fVScaleCanvas->GetCanvas()->cd();

    fVScaleCanvas->GetCanvas()->Range(0,0,1,1);
    fVScale = new TGaxis(0.5,0.0, 0.50001,1.0, 0.0,100.0, 510, "-");
    fVScale->SetLabelSize(0.4);
    fVScale->SetName("Y Scale");
    fVScale->Draw();
    fVScaleCanvas->GetCanvas()->SetEditable(kFALSE);

    // Call this after scales/sliders are setup
    SetRange(-1100,-1100,1100,1100);

    // The shape buttons
    const char* shape_button_name[] = {
                                          "Barrel", "EndCap", "", "", "Zoom Out", "Zoom In", "Close"
                                      };
    UInt_t ind;
    for (ind = 0; ind < 7; ++ind) {
        TGTextButton* button =
            new TGTextButton(table,shape_button_name[ind],ind);
        tloh = new TGTableLayoutHints(5,6,ind+2,ind+1+2,
                                      kLHintsExpandX|kLHintsExpandY |
                                      kLHintsShrinkX|kLHintsShrinkY |
                                      kLHintsFillX|kLHintsFillY);
        table->AddFrame(button,tloh);
        button->Resize(100,button->GetDefaultHeight());
        button->Connect("Clicked()","Viewer",this,"DoButton()");
    }

    const char* ctrl_button_name[2][3] = {{"Prev", "Next", ""},
                                           { "Create EPS File" , "Create PNG File", "Reset View" }};
    //      { "W" , "C", "E" },
    //      { "SW", "S", "SE" }

    UInt_t indx, indy;
    for (indx = 0; indx < 3; ++indx) {
        for (indy = 0; indy < 2; ++indy) {
        
            TGTextButton* button = new TGTextButton(table,
                                                    ctrl_button_name[indy][indx],
                                                    (indy+1)*10 + indx +1);
            tloh = new TGTableLayoutHints(indx+2,indx+1+2, 6+indy,6+indy+1,
                                            kLHintsExpandX|kLHintsExpandY |
                                            kLHintsShrinkX|kLHintsShrinkY |
                                            kLHintsFillX|kLHintsFillY);
            table->AddFrame(button,tloh);
            button->Resize(100,button->GetDefaultHeight());
            button->Connect("Clicked()","Viewer",this,"DoButton()");
        }
    }
  //--------- radio buttons for mode
   
   //TGLayoutHints       *fL3, *fL4;
   //TGRadioButton        *fR[2];
   TGGroupFrame   *fG2 = new TGGroupFrame(table, new TGString("DisplayMode"),kVerticalFrame|kRaisedFrame);

   TGLayoutHints* fL4 = new TGLayoutHints(kLHintsTop | kLHintsLeft,
                           0, 0, 5, 0);
//    fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft |
//                           kLHintsExpandX | kLHintsExpandY,
//                           2, 2, 2, 2);

   fRadioSingleTrackMode[0] = new TGRadioButton(fG2, new TGHotString("single track mode"),        21);
   fRadioSingleTrackMode[1] = new TGRadioButton(fG2, new TGHotString("event mode"),    22);

   for (int i = 0; i < 2; ++i) {
      fG2->AddFrame(fRadioSingleTrackMode[i], fL4);
      //fR[i]->Associate(this);
      fRadioSingleTrackMode[i]->Connect("Clicked()","Viewer",this,"DoRadioButton()");
   }

   fRadioSingleTrackMode[0]->SetState(kButtonDown);
   TGTableLayoutHints* RadioFrameLayoutHint = new TGTableLayoutHints(3,4, 9, 10,
                                            kLHintsExpandX|kLHintsExpandY |
                                            kLHintsShrinkX|kLHintsShrinkY |
                                            kLHintsFillX|kLHintsFillY);
   //table->AddFrame(fG2, new TGLayoutHints(kLHintsLeft | kLHintsTop,0,1,5,5));*/
   table->AddFrame(fG2, RadioFrameLayoutHint);
  
  //--------- check buttons for whatever (options)
   TGGroupFrame   *fCheckGroup = new TGGroupFrame(table, new TGString("Options"),kVerticalFrame|kRaisedFrame);

   TGLayoutHints* CheckButtonsLayoutHint = new TGLayoutHints(kLHintsTop | kLHintsLeft,
                           0, 0, 5, 0);
   TGCheckButton* checkEqualScaling = new TGCheckButton(fCheckGroup, new TGHotString("equal scaling along axis"), 21);
   checkEqualScaling->SetState(kButtonUp);
   TGCheckButton* checkAssgnProbs = new TGCheckButton(fCheckGroup, new TGHotString("show assgn probs for DAF tracks"), 22);
   checkAssgnProbs->SetState(kButtonUp);

   fCheckGroup->AddFrame(checkEqualScaling, CheckButtonsLayoutHint);
   checkEqualScaling->Connect("Clicked()","Viewer",this,"DoCheckButton()");
   fCheckGroup->AddFrame(checkAssgnProbs, CheckButtonsLayoutHint);
   checkAssgnProbs->Connect("Clicked()","Viewer",this,"DoCheckButton()");
   
   TGTableLayoutHints* CheckFrameLayoutHint = new TGTableLayoutHints(4,5, 9, 10,
                                            kLHintsExpandX|kLHintsExpandY |
                                            kLHintsShrinkX|kLHintsShrinkY |
                                            kLHintsFillX|kLHintsFillY);
   table->AddFrame(fCheckGroup, CheckFrameLayoutHint);
   
   table->Layout();

    // exit on close window
    Connect("CloseWindow()","TApplication",gApplication,"Terminate(=0)");

    MapSubwindows();
    Layout();
    MapWindow();

/*    // get the ntuple now
    char *filename = new char[80];
    filename = "TrkValidation.root";
    TFile *f = new TFile(filename);
    if (!f) {
        std::cout << "ERROR: could not find " << filename << std::endl;
        //return 1;
    } else {
        f->cd("TrkValidation.root:/FitterValidation");
        //}
        TTree *tree = (TTree*)gDirectory->Get("OutputTrack");
        if (!tree) {
            std::cout << "ERROR: could not get the tree." << std::endl;
            //return 1;
        } else {
            //int numberOfEntries = tree->GetEntriesFast();
            // get the ntuple with the track data
            m_trackNtuple = new TrackDataNtupleTree(tree);*/
            //m_ntuplePlotter = new PlotValNtuple();
/*        }
    }*/
    
}

Viewer::~Viewer() {
    fCanvas->Clear();
    delete fCanvas;
    fCleanup->Delete();
    delete fCleanup;
    //delete m_trackNtuple;
    //delete m_ntuplePlotter;
}

void Viewer::DoButton() {
    TGButton* button = (TGButton*)gTQSender;
    UInt_t id = button->WidgetId();

    double xmin, ymin, xmax, ymax;
    double xdiff, ydiff;
    fCanvas->GetRange(xmin,ymin,xmax,ymax);
    xdiff = xmax - xmin;
    ydiff = ymax - ymin;

    fCanvas->cd();
    //fprintf(stdout, "Button ID: %d \n", int(id));
    switch (id) {
    case 0:                     // Barrel view
        if (m_ntuplePlotter) {
            m_ntuplePlotter->setMode(BarrelXY);
            fCanvas->Clear();
            Float_t RangeMin=-1100.; Float_t RangeMax=1100.;
            fVSlider->SetRange(RangeMin, RangeMax);
            fHSlider->SetRange(RangeMin, RangeMax);
            SetRange(-1100., -1100., 1100., 1100.);

            if (!m_singleTrackMode) {
                std::vector<TObject*>* rioObjects = m_ntuplePlotter->getRIOGraphics();
                std::vector<TObject*>::const_iterator rioit = rioObjects->begin();
                //std::cout << "Got " << rioObjects.size() << " graphic objects for RIOs  "<< std::endl;
                for( ; rioit!=rioObjects->end();++rioit) {
                    (*rioit)->Draw();
                }
            }
            std::vector<TObject*>* trackObjects  = m_ntuplePlotter->getTrackGraphics();
            std::vector<TObject*>::const_iterator oit = trackObjects->begin();
            for( ; oit!=trackObjects->end();++oit) {
                (*oit)->Draw();
            }
            if (m_showDAFinfo){
                std::vector<TObject*>* compRotObjects  = m_ntuplePlotter->getCompetingROTsGraphics();
                oit = compRotObjects->begin();
                for( ; oit!=compRotObjects->end();++oit) {
                    (*oit)->Draw();
                }
            }
            
            fCanvas->Modified();
            fCanvas->Update();
        }
        break; 
    case 1:                     // Endcap view
            if (m_ntuplePlotter) {
                m_ntuplePlotter->setMode(EndCapPhiZ);
                fCanvas->Clear();
                fVSlider->SetRange(-3500., 3500.);
                fHSlider->SetRange(-2700., 2700.);
                SetRange(-2700., -3500., 2700., 3500.);

                if (!m_singleTrackMode) {
                    std::vector<TObject*>* rioObjects = m_ntuplePlotter->getRIOGraphics();
                    std::vector<TObject*>::const_iterator rioit = rioObjects->begin();
                //std::cout << "Got " << rioObjects.size() << " graphic objects for RIOs  "<< std::endl;
                    for( ; rioit!=rioObjects->end();++rioit) {
                        (*rioit)->Draw();
                    }
                }
                std::vector<TObject*>* trackObjects  = m_ntuplePlotter->getTrackGraphics();
                std::vector<TObject*>::const_iterator oit = trackObjects->begin();
                for( ; oit!=trackObjects->end();++oit) {
                    (*oit)->Draw();
                }
                if (m_showDAFinfo){
                    std::vector<TObject*>* compRotObjects  = m_ntuplePlotter->getCompetingROTsGraphics();
                    oit = compRotObjects->begin();
                    for( ; oit!=compRotObjects->end();++oit) {
                        (*oit)->Draw();
                    }
                }
            
                fCanvas->Modified();
                fCanvas->Update();
            }
            break;
    case 4:                     // Zoom out
        SetRange(xmin-xdiff*.1, ymin-ydiff*.1, xmax+xdiff*.1, ymax+ydiff*.1);
        break;
    case 5:                     // Zoom in
        SetRange(xmin+xdiff*.1, ymin+ydiff*.1, xmax-xdiff*.1, ymax-ydiff*.1);
        break;
    case 6:
        gApplication->Terminate(0);
        break;
    case 11:                    // prev
        if (m_ntuplePlotter) {
            fCanvas->Clear();
            
            if (m_singleTrackMode) {
                int eventNumber, trackNumber, InterationNumber;
                m_ntuplePlotter->setPrevTrack(eventNumber, trackNumber, InterationNumber);
            } else {
                int eventNumber;
                m_ntuplePlotter->setPrevEvent(eventNumber);            
            }
            if (!m_singleTrackMode) {
                std::vector<TObject*>* rioObjects = m_ntuplePlotter->getRIOGraphics();
                std::vector<TObject*>::const_iterator rioit = rioObjects->begin();
                //std::cout << "Got " << rioObjects.size() << " graphic objects for RIOs  "<< std::endl;
                for( ; rioit!=rioObjects->end();++rioit) {
                    (*rioit)->Draw();
                }
            }
            std::vector<TObject*>* trackObjects  = m_ntuplePlotter->getTrackGraphics();
            std::vector<TObject*>::const_iterator oit = trackObjects->begin();
            for( ; oit!=trackObjects->end();++oit) {
                (*oit)->Draw();
            }
            if (m_showDAFinfo){
                std::vector<TObject*>* compRotObjects  = m_ntuplePlotter->getCompetingROTsGraphics();
                oit = compRotObjects->begin();
                for( ; oit!=compRotObjects->end();++oit) {
                    (*oit)->Draw();
                }
            }
            
            fCanvas->Modified();
            fCanvas->Update();
        }
        //fprintf(stdout, "entry %d \n", int(m_entryNumber));
        break;
    case 12:                    // next
        if (m_ntuplePlotter) {
            fCanvas->Clear();
            
            if (m_singleTrackMode) {
                int eventNumber, trackNumber, InterationNumber;
                m_ntuplePlotter->setNextTrack(eventNumber, trackNumber, InterationNumber);
            } else {
                int eventNumber;
                m_ntuplePlotter->setNextEvent(eventNumber);            
            }
            if (!m_singleTrackMode) {
                std::vector<TObject*>* rioObjects = m_ntuplePlotter->getRIOGraphics();
                std::vector<TObject*>::const_iterator rioit = rioObjects->begin();
                //std::cout << "Got " << rioObjects.size() << " graphic objects for RIOs  "<< std::endl;
                for( ; rioit!=rioObjects->end();++rioit) {
                    (*rioit)->Draw();
                }
            }
            std::vector<TObject*>* trackObjects  = m_ntuplePlotter->getTrackGraphics();
            std::vector<TObject*>::const_iterator oit = trackObjects->begin();
            for( ; oit!=trackObjects->end();++oit) {
                (*oit)->Draw();
            }
            if (m_showDAFinfo){
                std::vector<TObject*>* compRotObjects  = m_ntuplePlotter->getCompetingROTsGraphics();
                oit = compRotObjects->begin();
                for( ; oit!=compRotObjects->end();++oit) {
                    (*oit)->Draw();
                }
            }

            fCanvas->Modified();
            fCanvas->Update();
        }
        //fprintf(stdout, "entry %d \n", int(m_entryNumber));
        break;
    case 21:                    // create EPS
        {
        //if(fCanvas)
        char epsFileName[50]="BarrelTrack.eps";
        //sprintf(epsFileName,"BarrelTrack%d.eps",int(m_entryNumber));
        fCanvas->Print( epsFileName,"eps");
        //}
        break;}
    case 22:                    // create PNG
        {//if(fCanvas)
        char pngFileName[50]="BarrelTrack.png";;
        //sprintf(pngFileName,"BarrelTrack%d.png",int(m_entryNumber));
        fCanvas->Print( pngFileName,"png");
        //}
        break;}
    case 23:                    // reset view
        SetRange(-1100., -1100., 1100., 1100.);
        break;
//            case 33:                    // se
//               SetRange(xmin+xdiff*.1, ymin-ydiff*.1, xmax+xdiff*.1, ymax-ydiff*.1);
//               break;
//            case 32:                    // e
//               SetRange(xmin+xdiff*.1, ymin, xmax+xdiff*.1, ymax);
//               break;
//            case 31:                    // ne
//               SetRange(xmin+xdiff*.1, ymin+ydiff*.1, xmax+xdiff*.1, ymax+ydiff*.1);
//               break;
//            case 21:                    // n
//               SetRange(xmin, ymin+ydiff*.1, xmax, ymax+ydiff*.1);
//               break;
//            case 22:                    // c
//               SetRange(50.0-xdiff/2,50-ydiff/2,50+xdiff/2,50+ydiff/2);
//               break;
    } // end switch(id)
}

void Viewer::DoRadioButton() {
    TGButton* button = (TGButton*)gTQSender;
    UInt_t id = button->WidgetId();


    fCanvas->cd();
    fprintf(stdout, "RadioButton ID: %d \n", int(id));
    switch (id) {
    case 21:                     // single track mode
        m_singleTrackMode = true;
        fRadioSingleTrackMode[0]->SetState(kButtonDown);
        fRadioSingleTrackMode[1]->SetState(kButtonUp);
        break;
    case 22:                     // event mode
        m_singleTrackMode = false;
        fRadioSingleTrackMode[0]->SetState(kButtonUp);
        fRadioSingleTrackMode[1]->SetState(kButtonDown);
        break;
    }
}

void Viewer::DoCheckButton() {
    TGButton* button = (TGButton*)gTQSender;
    TGCheckButton* checkButton = dynamic_cast<TGCheckButton*> (button);
    if (!checkButton){
        fprintf(stderr, "did not get the button\n");    
        return;
    }
    UInt_t id = button->WidgetId();
    
    fCanvas->cd();
    //fprintf(stdout, "CheckButton ID: %d \n", int(id));
    switch (id) {
    case 21:                     // equal scaling
        if (checkButton->IsOn()){
            //fprintf(stdout, "CheckButton ID %d is on\n", int(id));
            m_equalScaling = true;
            // force rescaling:
            double xmin, ymin, xmax, ymax;
            fCanvas->GetRange(xmin,ymin,xmax,ymax);
            //SetRange(xmin, ymin, xmax, ymax);
        } else {
            m_equalScaling = false;
        }
        break;
    case 22:                     // draw assignment probabilities for DAF tracks
        if (checkButton->IsOn()){
            m_showDAFinfo = true;
            std::vector<TObject*>* compRotObjects  = m_ntuplePlotter->getCompetingROTsGraphics();
            std::vector<TObject*>::const_iterator oit = compRotObjects->begin();
            for( ; oit!=compRotObjects->end();++oit) {
                (*oit)->Draw();
            }
            fCanvas->Modified();
            fCanvas->Update();
        } else {
            m_showDAFinfo = false;
        }
        break;
    }
}

void Viewer::DoSlider() {
    float xmin, ymin, xmax, ymax;
    fHSlider->GetPosition(xmin,xmax);
    fVSlider->GetPosition(ymin,ymax);
    //std::cout << "Slider:  X=(" << xmin<< "," << xmax<< "), Y=(" << ymin<< "," << ymax << ")"<< std::endl;
/*    float xmin2 = xmin-2000.;
    float ymin2 = ymin-2000.;
    float xmax2 = xmax-2000.;
    float ymax2 = ymax-2000.;*/
    SetRange(xmin,ymin,xmax,ymax, kFALSE);
}

void Viewer::SetRange(Float_t xmin, Float_t ymin, Float_t xmax, Float_t ymax,
                      Bool_t move_slider) {
    
    if (m_equalScaling) {
        //fprintf(stdout, "before scaling: x: [%f,%f] y: [%f, %f]\n", xmin, xmax, ymin, ymax ); 
        double ratio = double(fMainEmbeddedCanvas->GetWidth())/double(fMainEmbeddedCanvas->GetHeight());
        xmax = xmin + (ymax - ymin)*ratio;
        //fprintf(stdout, "width: %d, height: %d, ratio: %f\n", fMainEmbeddedCanvas->GetWidth(), fMainEmbeddedCanvas->GetHeight(), ratio); 
        //fprintf(stdout, "after scaling:  x: [%f,%f] y: [%f, %f]\n", xmin, xmax, ymin, ymax ); 
    }
    fCanvas->Range(xmin,ymin,xmax,ymax);

    //cerr << "x=[" << xmin << " - " << xmax << "], "
    //     << "y=[" << ymin << " - " << ymax << "]\n";

    fHScale->SetWmin(xmin);
    fHScale->SetWmax(xmax);
    fVScale->SetWmin(ymin);
    fVScale->SetWmax(ymax);

    // Set position of slider - don't if DoSlider called SetRange
    //if (move_slider) {
/*        float xmin2 = xmin+2000.;
        float ymin2 = ymin+2000.;
        float xmax2 = xmax+2000.;
        float ymax2 = ymax+2000.;*/
        //xmin+=2000.; ymin+=2000.; xmax+=2000., ymax+=2000.;
        fHSlider->SetPosition(xmin,xmax);
        fVSlider->SetPosition(ymin,ymax);
    //}

    fCanvas->Modified();
    fHScaleCanvas->GetCanvas()->Modified();
    fVScaleCanvas->GetCanvas()->Modified();
    fCanvas->Update();
    fHScaleCanvas->GetCanvas()->Update();
    fVScaleCanvas->GetCanvas()->Update();
    fClient->ForceRedraw();  //prevent GUI from being starved of redraws

}


//---- Main program ------------------------------------------------------------

int main(int argc, char **argv) {
//    TApplication theApp("App", &argc, argv);

    if (gROOT->IsBatch()) {
        fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
        return 1;
    }
    // -----------------------------------
    // Read the current TrackPlotter setup

    // get the TEnv object
    TEnv* trackPlotterSetup = new TEnv("trackplotter.conf");
    // get the ntuple filename
    //char *filename = new char[80];
    const char* filename = trackPlotterSetup->GetValue("TrackingNtupleFileName","TrkValidation.root");
    // track ntuple directory and tree name
    //char *trackNtupleDir = new char[80];
    const char* trackNtupleDir = trackPlotterSetup->GetValue("TrackingNtupleDirectory","Validation");
    //char *trackNtupleTree = new char[80];
    const char* trackNtupleTree = trackPlotterSetup->GetValue("TrackingNtupleTree","Tracks");

    // use DAF data?
    bool doDAFdata = false;
    doDAFdata = trackPlotterSetup->GetValue("PlotDAFdata",0);

    // use RIO data?
    bool doRIOdata = true;
    doRIOdata = trackPlotterSetup->GetValue("PlotPrepRawData", 1);
    //std::cout << "use RIO: -" << trackPlotterSetup->GetValue("PlotPrepRawData", "True") <<"-" << std::endl;
    // rio ntuple directory and tree name
    //char *rioNtupleDir = new char[80];
    const char* rioNtupleDir = trackPlotterSetup->GetValue("PrepRawDataNtupleDirectory","Validation");
    //char *trackNtupleTree = new char[80];
    const char* rioNtupleTree = trackPlotterSetup->GetValue("PrepRawDataNtupleTree","TRT_RIOs");

//     trackPlotterSetup->SetValue("TrackingNtupleFileName", filename);
//     trackPlotterSetup->SetValue("TrackingNtupleDirectory", trackNtupleDir);
//     trackPlotterSetup->SetValue("TrackingNtupleTree", trackNtupleTree);
//     trackPlotterSetup->SetValue("PlotDAFdata", doDAFdata);
//     trackPlotterSetup->SetValue("PlotPrepRawData", doRIOdata);
//     trackPlotterSetup->SetValue("PrepRawDataNtupleDirectory", rioNtupleDir);
//     trackPlotterSetup->SetValue("PrepRawDataNtupleTree", rioNtupleTree);
//     trackPlotterSetup->Save();
    trackPlotterSetup->Print();

    TColor *color_301 = new TColor(301,0.7,0.0,0.0,"");
    TColor *color_302 = new TColor(302,0.0,0.7,0.0,"");
    TColor *color_303 = new TColor(303,0.0,0.0,0.7,"");
    TColor *color_306 = new TColor(306,0.7,0.0,0.7,"");
    TColor *color_310 = new TColor(310,0.8,0.8,0.8,"");
    TColor *color_311 = new TColor(311,0.55,0.0,0.0,"");

//    int trackNumber = 0;
    if ( argc >= 2 ){ // argc should be 2 for correct execution
        //delete[] filename;
        filename = argv[1];
    }else {
        // We print argv[0] assuming it is the program name
        std::cout<<"usage: "<< argv[0] <<" <filename>"<< std::endl;
        //filename = "TrkValidation.root";        
//        trackNumber = atoi(argv[2]);
    }
    //if ( argc >= 3 ){ 
    //    doDAFdata = bool(argv[2]);
    //}
    std::cout<<"Using file "<<filename << (doRIOdata ? " including PrepRawData" : "") << (doDAFdata ? " with DAF information" : "") << std::endl;

    TApplication theApp("App", &argc, argv); 
    TFile *f = new TFile(filename);
    if (!f) {
        std::cout << "ERROR: could not find " << filename << std::endl;
        return 1;
    }
    
    char rootFileName[100];
    sprintf(rootFileName,"%s:/%s",filename, trackNtupleDir);
    //std::cout<<"cd:  "<<rootFileName << std::endl;
    f->cd(rootFileName);
    
    TTree *tree = (TTree*)gDirectory->Get(trackNtupleTree);
    if (!tree) {
        std::cout << "ERROR: could not get the tree: " << filename << ":/" << trackNtupleDir << "/" << trackNtupleTree << std::endl;
            return 1;    
    }
    //delete filename;
    // get the ntuple with the track data
    TrackDataNtupleTree* trackNtuple = new TrackDataNtupleTree(tree, doDAFdata);

    //std::cout<<"got TrackDataNtupleTree"<< std::endl;
    // get the RIOs
    
    sprintf(rootFileName,"%s:/%s",filename, rioNtupleDir);
    //f->cd("TrkValidation.root:/FitterValidation");
    //std::cout<<"cd:  "<<rootFileName << std::endl;
    f->cd(rootFileName);
    
    TTree *RIOtree = 0;
    TrkValidationNtupleRIOs* rioNtuple = 0;
    if (doRIOdata) {
        RIOtree = (TTree*)gDirectory->Get(rioNtupleTree);
        if (!RIOtree) {
            std::cout << "ERROR: could not get the tree: " << filename << ":/" << rioNtupleDir << "/" << rioNtupleTree << std::endl;
                return 1;    
        }
        // get the ntuple with the rio data
        rioNtuple = new TrkValidationNtupleRIOs(RIOtree);
    }
    //delete filename;

    PlotValNtuple theNtuplePlotter( *trackNtuple, rioNtuple);

    Viewer viewer(gClient->GetRoot(), &theNtuplePlotter);

    theApp.Run();

    delete color_301; delete color_302;
    delete color_303; delete color_306;
    delete color_310; delete color_311;
    delete trackNtuple;
    delete rioNtuple;
    return 0;
}
