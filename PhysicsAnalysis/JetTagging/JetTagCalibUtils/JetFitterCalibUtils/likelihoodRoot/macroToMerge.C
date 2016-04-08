/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

{
  TFile filebottom("smoothedhistosbottom.root");
  TFile filecharm("smoothedhistoscharm.root");
  TFile filelight("smoothedhistoslight.root");
  TFile fileoutput("smoothedhistos.root","recreate");
  TH1F* histobottom1=filebottom.Get("bottom_noVTX_significance3d_histo");
  TH1F* histobottom2=filebottom.Get("bottom_noVTX_1SingleTrack_energyFraction_normal_histo");
  TH1F* histobottom3=filebottom.Get("bottom_noVTX_2orMoreSingleTrack_energyFraction_normal_histo");
  TH1F* histobottom4=filebottom.Get("bottom_1orMoreVTX_2TrackVTX_energyFraction_normal_histo");
  TH1F* histobottom5=filebottom.Get("bottom_1orMoreVTX_3TrackVTX_energyFraction_normal_histo");
  TH1F* histobottom6=filebottom.Get("bottom_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo");
  TH1F* histobottom7=filebottom.Get("bottom_1orMoreVTX_significance3d_histo");
  TH1F* histobottom8=filebottom.Get("bottom_1orMoreVTX_2TrackVTX_mass_histo");
  TH1F* histobottom9=filebottom.Get("bottom_1orMoreVTX_3orMoreTrackVTX_mass_histo");
  TH1F* histocharm1=filecharm.Get("charm_noVTX_significance3d_histo");
  TH1F* histocharm2=filecharm.Get("charm_noVTX_1SingleTrack_energyFraction_normal_histo");
  TH1F* histocharm3=filecharm.Get("charm_noVTX_2orMoreSingleTrack_energyFraction_normal_histo");
  TH1F* histocharm4=filecharm.Get("charm_1orMoreVTX_2TrackVTX_energyFraction_normal_histo");
  TH1F* histocharm5=filecharm.Get("charm_1orMoreVTX_3TrackVTX_energyFraction_normal_histo");
  TH1F* histocharm6=filecharm.Get("charm_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo");
  TH1F* histocharm7=filecharm.Get("charm_1orMoreVTX_significance3d_histo");
  TH1F* histocharm8=filecharm.Get("charm_1orMoreVTX_2TrackVTX_mass_histo");
  TH1F* histocharm9=filecharm.Get("charm_1orMoreVTX_3orMoreTrackVTX_mass_histo");
  TH1F* histolight1=filelight.Get("light_noVTX_significance3d_histo");
  TH1F* histolight2=filelight.Get("light_noVTX_1SingleTrack_energyFraction_normal_histo");
  TH1F* histolight3=filelight.Get("light_noVTX_2orMoreSingleTrack_energyFraction_normal_histo");
  TH1F* histolight4=filelight.Get("light_1orMoreVTX_2TrackVTX_energyFraction_normal_histo");
  TH1F* histolight5=filelight.Get("light_1orMoreVTX_3TrackVTX_energyFraction_normal_histo");
  TH1F* histolight6=filelight.Get("light_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal_histo");
  TH1F* histolight7=filelight.Get("light_1orMoreVTX_significance3d_histo");
  TH1F* histolight8=filelight.Get("light_1orMoreVTX_2TrackVTX_mass_histo");
  TH1F* histolight9=filelight.Get("light_1orMoreVTX_3orMoreTrackVTX_mass_histo");
  histobottom1->Write();
  histobottom2->Write();
  histobottom3->Write();
  histobottom4->Write();
  histobottom5->Write();
  histobottom6->Write();
  histobottom7->Write();
  histobottom8->Write();
  histobottom9->Write();
  histocharm1->Write();
  histocharm2->Write();
  histocharm3->Write();
  histocharm4->Write();
  histocharm5->Write();
  histocharm6->Write();
  histocharm7->Write();
  histocharm8->Write();
  histocharm9->Write();
  histolight1->Write();
  histolight2->Write();
  histolight3->Write();
  histolight4->Write();
  histolight5->Write();
  histolight6->Write();
  histolight7->Write();
  histolight8->Write();
  histolight9->Write();
  fileoutput.Write();
}
