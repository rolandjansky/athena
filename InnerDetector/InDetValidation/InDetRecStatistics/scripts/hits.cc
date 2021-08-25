/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// File: $Id: hits.cc,v 1.3 2008-07-23 20:51:23 adye Exp $
#include <iostream>
#include <string>

#include "TROOT.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TProfile2D.h"
#include "TCut.h"

using std::cout;
using std::endl;
using std::string;

// Not used at the moment
Double_t overflows (const TH1* h)
{
  Double_t v;
  Int_t xmax= h->GetNbinsX()+1;
  if (dynamic_cast<const TH2*>(h)) {
    Int_t ymax= h->GetNbinsY()+1;
    v  = h->GetBinContent (0,0);
    v += h->GetBinContent (xmax, 0);
    v += h->GetBinContent (0,    ymax);
    v += h->GetBinContent (xmax, ymax);
  } else {
    v  = h->GetBinContent (0);
    v += h->GetBinContent (xmax);
  }
  return v;
}

class HitPlots {
public:
  TTree* _chain;
  TCanvas _canvas;
  TStyle _style;
  TCut _hit_cut, _track_cut;
  string _pre, _ext;
  Long64_t _nentries, _firstentry;
  HitPlots() : _chain(0), _nentries(1000000000), _firstentry(0) {};
  HitPlots(TTree *chain, const TCut hit_cut="", const TCut track_cut="",
           const string & pre="", const string & ext="",
           Long64_t nentries=1000000000, Long64_t firstentry=0);
  ~HitPlots() { gROOT->SetStyle("Default"); };
  Long64_t Draw       (const char* name, const char* title, const char* varexp, const TCut  selection="", const char* opt1="", const char* opt2="",
                       Double_t min=-1111, Double_t max=-1111);
  Long64_t DrawHits   (const char* name, const char* title, const char* varexp, const char* selection="", const char* option="",
                       Double_t min=-1111, Double_t max=-1111);
  Long64_t DrawTracks (const char* name, const char* title, const char* varexp, const char* selection="", const char* option="",
                       Double_t min=-1111, Double_t max=-1111);
  void DrawGif(const string & name);
  TH1* GetHistogram() const { return _chain->GetHistogram(); }
};

HitPlots::HitPlots (TTree *chain, const TCut hit_cut, const TCut track_cut,
                    const string & pre, const string & ext, Long64_t nentries, Long64_t firstentry)
  : _chain(chain), _canvas("chits","ID Hits Canvas"), _style("shits","ID Hits Style"),
    _hit_cut(hit_cut), _track_cut(track_cut),
    _pre(pre), _ext(ext), _nentries(nentries), _firstentry(firstentry)
{
  _canvas.cd();
  _style.cd();
}

Long64_t HitPlots::Draw (const char* name, const char* title, const char* varexp, const TCut selection, const char* opt1, const char* opt2,
                         Double_t min, Double_t max)
{
#if DUPOLD==2
  title= 0;
#endif
  Long64_t nsel;
  if (title || min != -1111 || max != -1111) {  // NB. this method doesn't work properly for scatter plots, so don't specify these variables in that case
    string opt= "goff";
    if (opt1 && *opt1) { opt += ","; opt += opt1; }
    nsel= _chain->Draw(varexp,selection,opt.c_str(),_nentries,_firstentry);
    TH1* h= GetHistogram();
    if (!h) {
      cerr << "No histogram from " << varexp << endl;
      return 0;
    }
    if (title) h->SetTitle (title);
    if (min != -1111)              h->SetMinimum (min);
    if (max != -1111 && max > min) h->SetMaximum (max);
    h->Draw(opt2);
  } else {
    string opt= string(opt1?opt1:"")+string(opt2?opt2:"");
    nsel= _chain->Draw(varexp,selection,opt.c_str(),_nentries,_firstentry);
  }
  if (name) DrawGif (name);
  return nsel;
}

Long64_t HitPlots::DrawHits (const char* name, const char* title, const char* varexp, const char* selection, const char* option,
                             Double_t min, Double_t max)
{
  return Draw(name,title,varexp,_hit_cut&&TCut(selection),"",option,min,max);
}

Long64_t HitPlots::DrawTracks (const char* name, const char* title, const char* varexp, const char* selection, const char* option,
                               Double_t min, Double_t max)
{
  return Draw(name,title,varexp,_track_cut&&TCut(selection),"profile",option,min,max);
}

void HitPlots::DrawGif(const string & name) {
  if (_ext=="") return;
  string epsname= _pre+name+_ext;
  _canvas.SaveAs(epsname.c_str());
}

void hits(TTree *chain, const string & pre="", const string & ext="",
          const TCut hit_cut="",
          const TCut track_cut="",
          const bool maps= false)
{
  if (  *hit_cut.GetTitle()) cout << "hit   cut: " <<   hit_cut.GetTitle() << endl;
  if (*track_cut.GetTitle()) cout << "track cut: " << track_cut.GetTitle() << endl;

  HitPlots plots (chain, hit_cut, TCut("abs(1./track_qoverpt)>1000")&&track_cut, pre, ext);

#if DUPOLD==2
  plots.DrawHits("All_RZ",0,"sqrt(pow(hit_globalx,2)+pow(hit_globaly,2)):hit_globalz");
#else
  plots.DrawHits("All_RZ",0,"sqrt(pow(hit_globalx,2)+pow(hit_globaly,2)):hit_globalz>>hrz(40,-3200,3200,40,0,1150)");
#endif

  if (maps) {
    plots.DrawHits("Pixel_Barrel_Layer0",0,"hit_phi:hit_globalz","hit_dettype==0&&hit_layer==0&&hit_isbarrel==0");
    plots.DrawHits("Pixel_Barrel_Layer1",0,"hit_phi:hit_globalz","hit_dettype==0&&hit_layer==1&&hit_isbarrel==0");
    plots.DrawHits("Pixel_Barrel_Layer2",0,"hit_phi:hit_globalz","hit_dettype==0&&hit_layer==2&&hit_isbarrel==0");
    plots.DrawHits("Pixel_EndcapPosZ_Layer0",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==0&&hit_isbarrel==2");
    plots.DrawHits("Pixel_EndcapPosZ_Layer1",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==1&&hit_isbarrel==2");
    plots.DrawHits("Pixel_EndcapPosZ_Layer2",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==2&&hit_isbarrel==2");
    plots.DrawHits("Pixel_EndcapNegZ_Layer0",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==0&&hit_isbarrel==-2");
    plots.DrawHits("Pixel_EndcapNegZ_Layer1",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==1&&hit_isbarrel==-2");
    plots.DrawHits("Pixel_EndcapNegZ_Layer2",0,"hit_globaly:hit_globalx","hit_dettype==0&&hit_layer==2&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer0",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==0&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer1",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==1&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer2",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==2&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer3",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==3&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer4",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==4&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer5",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==5&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer6",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==6&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer7",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==7&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapNegZ_Layer8",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==8&&hit_isbarrel==-2");
    plots.DrawHits("SCT_EndcapPosZ_Layer0",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==0&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer1",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==1&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer2",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==2&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer3",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==3&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer4",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==4&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer5",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==5&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer6",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==6&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer7",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==7&&hit_isbarrel==2");
    plots.DrawHits("SCT_EndcapPosZ_Layer8",0,"hit_globaly:hit_globalx","hit_dettype==1&&hit_layer==8&&hit_isbarrel==2");
    plots.DrawHits("SCT_Barrel_Layer0",0,"hit_phi:hit_globalz","hit_dettype==1&&hit_layer==0&&hit_isbarrel==0");
    plots.DrawHits("SCT_Barrel_Layer1",0,"hit_phi:hit_globalz","hit_dettype==1&&hit_layer==1&&hit_isbarrel==0");
    plots.DrawHits("SCT_Barrel_Layer2",0,"hit_phi:hit_globalz","hit_dettype==1&&hit_layer==2&&hit_isbarrel==0");
    plots.DrawHits("SCT_Barrel_Layer3",0,"hit_phi:hit_globalz","hit_dettype==1&&hit_layer==3&&hit_isbarrel==0");
    plots.DrawHits("TRT_NegZ_Endcap",0,"hit_globaly:hit_globalx","hit_dettype==2&&hit_isbarrel==-2");
    plots.DrawHits("TRT_NegZ_Barrel",0,"hit_globaly:hit_globalx","hit_dettype==2&&hit_isbarrel==-1");
    plots.DrawHits("TRT_PosZ_Barrel",0,"hit_globaly:hit_globalx","hit_dettype==2&&hit_isbarrel==1");
    plots.DrawHits("TRT_PosZ_Endcap",0,"hit_globaly:hit_globalx","hit_dettype==2&&hit_isbarrel==2");
  }

  plots.DrawTracks("nHits_pixel_phi","Pixel hits/track vs #phi;#phi","track_npixhits:track_phi>>hpixelphi(32,-3.14159,3.14159)");
  plots.DrawTracks("nHits_SCT_phi","SCT hits/track vs #phi;#phi","track_nscthits:track_phi>>hsctphi(32,-3.14159,3.14159)");
  plots.DrawTracks("nHits_TRT_phi","TRT hits/track vs #phi;#phi","track_ntrthits:track_phi>>htrtphi(32,-3.14159,3.14159)");
  plots.DrawTracks("nHits_pixel_eta","Pixel hits/track vs #eta;#eta","track_npixhits:track_eta>>hpixeleta(30,-3,3)");
  plots.DrawTracks("nHits_SCT_eta","SCT hits/track vs #eta;#eta","track_nscthits:track_eta>>hscteta(30,-3,3)");
  plots.DrawTracks("nHits_TRT_eta","TRT hits/track vs #eta;#eta","track_ntrthits:track_eta>>htrteta(30,-3,3");

  plots.DrawTracks ("nHits_nblayer_eta","Pixel b-layer hits/track vs #eta;#eta","m_track_nblayer:track_eta>>hblayereta(30,-3,3)","m_track_nblayer>=0");
  plots.DrawTracks ("nHits_nbla_sh_eta","Pixel b-layer shared hits/track vs #eta;#eta","m_track_nbla_sh:track_eta>>hbla_sheta(30,-3,3)","m_track_nbla_sh>=0");
  plots.DrawTracks ("nHits_npix_sh_eta","Pixel shared hits/track vs #eta;#eta","m_track_npix_sh:track_eta>>hpix_sheta(30,-3,3)","m_track_npix_sh>=0");
  plots.DrawTracks ("nHits_nsct_sh_eta","SCT shared hits/track vs #eta;#eta","m_track_nsct_sh:track_eta>>hsct_sheta(30,-3,3)","m_track_nsct_sh>=0");
  plots.DrawTracks ("nHits_nbla_out_eta","Pixel b-layer outliers/track vs #eta;#eta","m_track_nbla_out:track_eta>>hbla_outeta(30,-3,3)","m_track_nbla_out>=0");
  plots.DrawTracks ("nHits_nganged_eta","Ganged pixel hits/track vs #eta;#eta","m_track_nganged:track_eta>>hgangedeta(30,-3,3)","m_track_nganged>=0");
  plots.DrawTracks ("nHits_npix_ho_eta","Pixel holes/track vs #eta;#eta","m_track_npix_ho:track_eta>>hpix_hoeta(30,-3,3)","m_track_npix_ho>=0");
  plots.DrawTracks ("nHits_nsct_ho_eta","SCT holes/track vs #eta;#eta","m_track_nsct_ho:track_eta>>hsct_hoeta(30,-3,3)","m_track_nsct_ho>=0");
  plots.DrawTracks ("nHits_ntrt_highthr_eta","TRT high-threshold hits/track vs #eta;#eta","m_track_ntrt_highthr:track_eta>>htrt_highthreta(30,-3,3)","m_track_ntrt_highthr>=0");
  plots.DrawTracks ("nHits_ntrt_highthrout_eta","TRT high-threshold outliers/track vs #eta;#eta","m_track_ntrt_highthrout:track_eta>>hntrt_highthrouteta(30,-3,3)","m_track_ntrt_highthrout>=0");

  plots._style.SetPalette(1,0);
  plots._style.SetOptStat(0);

  plots.DrawTracks("nHits_pixel_2d","Pixel hits;#eta;#phi","track_npixhits:track_phi:track_eta>>hpixel(30,-3,3,32,-3.14159,3.14159)","","colz",2.5);
  plots.DrawTracks("nHits_SCT_2d","SCT hits;#eta;#phi","track_nscthits:track_phi:track_eta>>hsct(30,-3,3,32,-3.14159,3.14159)","","colz",7);
  plots.DrawTracks("nHits_TRT_2d","TRT hits;#eta;#phi","track_ntrthits:track_phi:track_eta>>htrt(30,-3,3,32,-3.14159,3.14159)","","colz",25);
}
