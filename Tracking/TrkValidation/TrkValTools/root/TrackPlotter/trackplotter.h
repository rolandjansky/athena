/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif

#include "TrkValidationNtupleRIOs.h"

class TList;
class TCanvas;
class TRootEmbeddedCanvas;
class TGaxis;
class TGRadioButton;
class TGDoubleSlider;

class TrackDataNtupleTree;
class PlotValNtuple;


class Viewer : public TGMainFrame {

private:
   //TrackDataNtupleTree *m_trackNtuple;
   //TrkValidationNtupleRIOs *m_rioNtuple;
   PlotValNtuple       *m_ntuplePlotter;   
   //Long64_t             m_entryNumber;
   bool                 m_singleTrackMode;
   bool                 m_equalScaling;
   bool                 m_showDAFinfo;
   TList               *fCleanup;
   TCanvas             *fCanvas;
   TRootEmbeddedCanvas *fMainEmbeddedCanvas;
   TRootEmbeddedCanvas *fHScaleCanvas, *fVScaleCanvas;
   TGaxis              *fHScale, *fVScale;
   TGDoubleSlider      *fHSlider;
   TGDoubleSlider      *fVSlider;
   TGRadioButton       *fRadioSingleTrackMode[2];
public:
   //Viewer(const TGWindow *win, TrackDataNtupleTree* trackNtuple, TrkValidationNtupleRIOs* rioNtuple);
   Viewer(const TGWindow *win, PlotValNtuple* ntuplePlotter);
   virtual ~Viewer();
   void DoButton();
   void DoRadioButton();
   void DoSlider();
   void DoCheckButton();
   void SetRange(Float_t xmin, Float_t ymin, Float_t xmax, Float_t ymax,
                 Bool_t move_slider = kTRUE);
   ClassDef(Viewer,0) //GUI example
};
