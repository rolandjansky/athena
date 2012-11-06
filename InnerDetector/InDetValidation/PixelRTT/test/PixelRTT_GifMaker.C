/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>




void PixelRTT_GifMaker(){

  TFile* MyRef;
  TFile* MyFile;

  MyRef = new TFile ("/afs/cern.ch/user/c/clapoire/RTT/PixelRTT/WebPage/TestMon_16Fev.root");
  MyFile = new TFile ("TestMon.root");

  canvas = new TCanvas ("canvas","",800,600);


  TString mDir = "Pixel/RDO/";
  gStyle->SetPalette(1);

  TString name="Offline_BCID";
  TH1D* Offline_BCID = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_BCID){
    Offline_BCID->Draw();
    TH1D* Offline_BCID_Ref = (TH1D*)MyRef->Get(mDir+name);
    if (Offline_BCID_Ref){
      Offline_BCID_Ref->SetLineColor(2);
      Offline_BCID_Ref->SetLineStyle(2);
      Offline_BCID_Ref->Draw("same");
      canvas->SaveAs(name+".gif");
    }
  }
	
  name="Offline_A_occupancy";
  TH2D* Offline_A_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_A_occupancy){
    Offline_A_occupancy->Draw("colz");
    // 	TH2D* Offline_A_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_A_occupancy_Ref->SetLineColor(2);
    // 	Offline_A_occupancy_Ref->SetLineStyle(2);
    // 	Offline_A_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }
	
  name="Offline_B0_occupancy";
  TH2D* Offline_B0_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if (Offline_B0_occupancy){
    Offline_B0_occupancy->Draw("colz");
    //	TH2D* Offline_B0_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B0_occupancy_Ref->SetLineColor(2);
    // 	Offline_B0_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B0_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_B1_occupancy";
  TH2D* Offline_B1_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if (Offline_B1_occupancy){
    Offline_B1_occupancy->Draw("colz");
    //	TH2D* Offline_B1_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B1_occupancy_Ref->SetLineColor(2);
    // 	Offline_B1_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B1_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_B2_occupancy";
  TH2D* Offline_B2_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if (Offline_B2_occupancy){
    Offline_B2_occupancy->Draw("colz");
    //	TH2D* Offline_B2_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B2_occupancy_Ref->SetLineColor(2);
    // 	Offline_B2_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B2_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_C_occupancy";
  TH2D* Offline_C_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if (Offline_C_occupancy){
    Offline_C_occupancy->Draw("colz");
    //	TH2D* Offline_C_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_C_occupancy_Ref->SetLineColor(2);
    // 	Offline_C_occupancy_Ref->SetLineStyle(2);
    // 	Offline_C_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_pixel_occupancy_all_modules";
  TH2D* Offline_pixel_occupancy_all_modules = (TH2D*)MyFile->Get(mDir+name);
  if (Offline_pixel_occupancy_all_modules){
    Offline_pixel_occupancy_all_modules->Draw("colz");
    canvas->Update();
    canvas->SaveAs(name+".gif");
	  
	  
    TH1D* Offline_pixel_occupancy_all_modules_projX = (TH1D*)Offline_pixel_occupancy_all_modules->ProjectionX();
    Offline_pixel_occupancy_all_modules_projX->SetTitle("Pixel occupancy integrated over all modules projected on the x axis");
    Offline_pixel_occupancy_all_modules_projX->Draw();
    TH2D* Offline_pixel_occupancy_all_modules_Ref = (TH2D*)MyRef->Get(mDir+name);
    if (Offline_pixel_occupancy_all_modules_Ref){
      TH1D* Offline_pixel_occupancy_all_modules_projX_Ref = (TH1D*)Offline_pixel_occupancy_all_modules_Ref->ProjectionX();
      Offline_pixel_occupancy_all_modules_projX_Ref->SetLineColor(2);
      Offline_pixel_occupancy_all_modules_projX_Ref->SetLineStyle(2);
      Offline_pixel_occupancy_all_modules_projX_Ref->Draw("same");
      canvas->Update();
      name="Offline_pixel_occupancy_all_modules";	  
      canvas->SaveAs(name+"_X.gif");
	    
	    
      TH1D* Offline_pixel_occupancy_all_modules_projY = (TH1D*)Offline_pixel_occupancy_all_modules->ProjectionY();
      Offline_pixel_occupancy_all_modules_projY->SetTitle("Pixel occupancy integrated over all modules projected on the y axis");
      Offline_pixel_occupancy_all_modules_projY->Draw();
      TH1D* Offline_pixel_occupancy_all_modules_projY_Ref = (TH1D*)Offline_pixel_occupancy_all_modules_Ref->ProjectionY();
      Offline_pixel_occupancy_all_modules_projY_Ref->SetLineColor(2);
      Offline_pixel_occupancy_all_modules_projY_Ref->SetLineStyle(2);
      Offline_pixel_occupancy_all_modules_projY_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+"_Y.gif");
    }
  }
	
  name="Offline_Lvl1A";
  TH1D* Offline_Lvl11A = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_Lvl11A){
    Offline_Lvl11A->Draw();
    name="Offline_Lvl11A";
    TH1D* Offline_Lvl11A_Ref = (TH1D*)MyRef->Get(mDir+name);
    if (Offline_Lvl11A_Ref){
      Offline_Lvl11A_Ref->SetLineColor(2);
      Offline_Lvl11A_Ref->SetLineStyle(2);
      Offline_Lvl11A_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }
	
  name="Offline_LvlID";
  TH1D* Offline_LvllD = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_LvllD){
    Offline_LvllD->Draw();
    TH1D* Offline_LvllD_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_LvllD_Ref){
      Offline_LvllD_Ref->SetLineColor(2);
      Offline_LvllD_Ref->SetLineStyle(2);
      Offline_LvllD_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }
	
  name="Offline_ToT";
  TH1D* Offline_ToT = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_ToT){
    Offline_ToT->Draw();
    TH1D* Offline_ToT_Ref = (TH1D*)MyRef->Get(mDir+name);
    if (Offline_ToT_Ref){
      Offline_ToT_Ref->SetLineColor(2);
      Offline_ToT_Ref->SetLineStyle(2);
      Offline_ToT_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }
	
  name="Offline_num_hits";
  TH1D* Offline_num_hits = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_num_hits){
    Offline_num_hits->Draw();
    TH1D* Offline_num_hits_Ref = (TH1D*)MyRef->Get(mDir+name);
    if (Offline_num_hits_Ref){
      Offline_num_hits_Ref->SetLineColor(2);
      Offline_num_hits_Ref->SetLineStyle(2);
      Offline_num_hits_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }
	
  mDir = "Pixel/Cluster/";


  TString name="Offline_A_cluster_occupancy";
  TH2D* Offline_A_cluster_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_A_cluster_occupancy ){
    Offline_A_cluster_occupancy->Draw("colz");
    // 	TH2D* Offline_A_cluster_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_A_cluster_occupancy_Ref->SetLineColor(2);
    // 	Offline_A_cluster_occupancy_Ref->SetLineStyle(2);
    // 	Offline_A_cluster_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_B0_cluster_occupancy";
  TH2D* Offline_B0_cluster_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_B0_cluster_occupancy){
    Offline_B0_cluster_occupancy->Draw("colz");
    //	TH2D* Offline_B0_cluster_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B0_cluster_occupancy_Ref->SetLineColor(2);
    // 	Offline_B0_cluster_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B0_cluster_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_B1_cluster_occupancy";
  TH2D* Offline_B1_cluster_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_B1_cluster_occupancy){
    Offline_B1_cluster_occupancy->Draw("colz");
    //	TH2D* Offline_B1_cluster_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B1_cluster_occupancy_Ref->SetLineColor(2);
    // 	Offline_B1_cluster_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B1_cluster_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_B2_cluster_occupancy";
  TH2D* Offline_B2_cluster_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_B2_cluster_occupancy){
    Offline_B2_cluster_occupancy->Draw("colz");
    //	TH2D* Offline_B2_cluster_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_B2_cluster_occupancy_Ref->SetLineColor(2);
    // 	Offline_B2_cluster_occupancy_Ref->SetLineStyle(2);
    // 	Offline_B2_cluster_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_C_cluster_occupancy";
  TH2D* Offline_C_cluster_occupancy = (TH2D*)MyFile->Get(mDir+name);
  if(Offline_C_cluster_occupancy){
    Offline_C_cluster_occupancy->Draw("colz");
    //	TH2D* Offline_C_cluster_occupancy_Ref = (TH2D*)MyRef->Get(mDir+name);
    // 	Offline_C_cluster_occupancy_Ref->SetLineColor(2);
    // 	Offline_C_cluster_occupancy_Ref->SetLineStyle(2);
    // 	Offline_C_cluster_occupancy_Ref->Draw("same");
    canvas->SaveAs(name+".gif");
  }

  name="Offline_cluster_groupsize";
  TH1D* Offline_cluster_groupsize = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_cluster_groupsize){
    Offline_cluster_groupsize->Draw();
    TH1D* Offline_cluster_groupsize_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_cluster_groupsize_Ref){
      Offline_cluster_groupsize_Ref->SetLineColor(2);
      Offline_cluster_groupsize_Ref->SetLineStyle(2);
      Offline_cluster_groupsize_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }

  name="Offline_num_clusters";
  TH1D* Offline_num_clusters = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_num_clusters){
    Offline_num_clusters->Draw();
    TH1D* Offline_num_clusters_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_num_clusters_Ref){
      Offline_num_clusters_Ref->SetLineColor(2);
      Offline_num_clusters_Ref->SetLineStyle(2);
      Offline_num_clusters_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }

  name="Offline_cluster_column_width";
  TH1D* Offline_cluster_column_width = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_cluster_column_width){
    Offline_cluster_column_width->Draw();
    TH1D* Offline_cluster_column_width_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_cluster_column_width_Ref){
      Offline_cluster_column_width_Ref->SetLineColor(2);
      Offline_cluster_column_width_Ref->SetLineStyle(2);
      Offline_cluster_column_width_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }

  name="Offline_cluster_row_width";
  TH1D* Offline_cluster_row_width = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_cluster_row_width){
    Offline_cluster_row_width->Draw();
    TH1D* Offline_cluster_row_width_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_cluster_row_width_Ref){
      Offline_cluster_row_width_Ref->SetLineColor(2);
      Offline_cluster_row_width_Ref->SetLineStyle(2);
      Offline_cluster_row_width_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }

  mDir = "Pixel/SpacePoint/";

  name="Offline_num_spacepoints";
  TH1D* Offline_num_spacepoints = (TH1D*)MyFile->Get(mDir+name);
  if (Offline_num_spacepoints){
    Offline_num_spacepoints->Draw();
    TH1D* Offline_num_spacepoints_Ref = (TH1D*)MyRef->Get(mDir+name);
    if (Offline_num_spacepoints_Ref){
      Offline_num_spacepoints_Ref->SetLineColor(2);
      Offline_num_spacepoints_Ref->SetLineStyle(2);
      Offline_num_spacepoints_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs(name+".gif");
    }
  }

  name="Offline_pixel_sp_x";
  TH1D* Offline_pixel_sp_x = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_pixel_sp_x){
    Offline_pixel_sp_x->Draw();
    name="pixel_sp_x";
    TH1D* Offline_pixel_sp_x_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_pixel_sp_x_Ref){
      Offline_pixel_sp_x_Ref->SetLineColor(2);
      Offline_pixel_sp_x_Ref->SetLineStyle(2);
      Offline_pixel_sp_x_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs("Offline"+name+".gif");
    }
  }

  name="Offline_pixel_sp_y";
  TH1D* Offline_pixel_sp_y = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_pixel_sp_y){
    Offline_pixel_sp_y->Draw();
    name="pixel_sp_y";
    TH1D* Offline_pixel_sp_y_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_pixel_sp_y_Ref){
      Offline_pixel_sp_y_Ref->SetLineColor(2);
      Offline_pixel_sp_y_Ref->SetLineStyle(2);
      Offline_pixel_sp_y_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs("Offline"+name+".gif");
    }
  }

  name="Offline_pixel_sp_z";
  TH1D* Offline_pixel_sp_z = (TH1D*)MyFile->Get(mDir+name);
  if(Offline_pixel_sp_z){
    Offline_pixel_sp_z->Draw();
    name="pixel_sp_z";
    TH1D* Offline_pixel_sp_z_Ref = (TH1D*)MyRef->Get(mDir+name);
    if(Offline_pixel_sp_z_Ref){
      Offline_pixel_sp_z_Ref->SetLineColor(2);
      Offline_pixel_sp_z_Ref->SetLineStyle(2);
      Offline_pixel_sp_z_Ref->Draw("same");
      canvas->Update();
      canvas->SaveAs("Offline"+name+".gif");
    }
  }


  if (Offline_BCID)
    Offline_BCID->Delete();
  if(Offline_BCID_Ref)
    Offline_BCID_Ref->Delete();
  if(Offline_A_occupancy)
    Offline_A_occupancy->Delete();
  if(Offline_B1_occupancy)
    Offline_B1_occupancy->Delete();
  if(Offline_B2_occupancy)
    Offline_B2_occupancy->Delete();
  if(Offline_B0_occupancy)
    Offline_B0_occupancy->Delete();
  if(Offline_C_occupancy)
    Offline_C_occupancy->Delete();
  if(Offline_pixel_occupancy_all_modules)
    Offline_pixel_occupancy_all_modules->Delete();
  if(Offline_pixel_occupancy_all_modules_Ref)
    Offline_pixel_occupancy_all_modules_Ref->Delete();
  if(Offline_Lvl11A)
    Offline_Lvl11A->Delete();
  if(Offline_Lvl11A_Ref)
    Offline_Lvl11A_Ref->Delete();
  if(Offline_LvllD)
    Offline_LvllD->Delete();
  if(Offline_LvllD_Ref)
    Offline_LvllD_Ref->Delete();
  if(Offline_ToT)
    Offline_ToT->Delete();
  if(Offline_ToT_Ref)
    Offline_ToT_Ref->Delete();
  if(Offline_num_hits)
    Offline_num_hits->Delete();
  if(Offline_num_hits_Ref)
    Offline_num_hits_Ref->Delete();
  if(Offline_A_cluster_occupancy)
    Offline_A_cluster_occupancy->Delete();
  if(Offline_B0_cluster_occupancy)
    Offline_B0_cluster_occupancy->Delete();
  if(Offline_B1_cluster_occupancy)
    Offline_B1_cluster_occupancy->Delete();
  if(Offline_B2_cluster_occupancy)
    Offline_B2_cluster_occupancy->Delete();
  if(Offline_C_cluster_occupancy)
    Offline_C_cluster_occupancy->Delete();
  if(Offline_cluster_groupsize)
    Offline_cluster_groupsize->Delete();
  if(Offline_cluster_groupsize_Ref)
    Offline_cluster_groupsize_Ref->Delete();
  if(Offline_num_clusters)
    Offline_num_clusters->Delete();
  if(Offline_num_clusters_Ref)
    Offline_num_clusters_Ref->Delete();
  if(Offline_cluster_column_width)
    Offline_cluster_column_width->Delete();
  if(Offline_cluster_column_width_Ref)
    Offline_cluster_column_width_Ref->Delete();
  if(Offline_cluster_row_width)
    Offline_cluster_row_width->Delete();
  if(Offline_cluster_row_width_Ref)
    Offline_cluster_row_width_Ref->Delete();
  if(Offline_num_spacepoints)
    Offline_num_spacepoints->Delete();
  if(Offline_num_spacepoints_Ref)
    Offline_num_spacepoints_Ref->Delete();
  if(Offline_pixel_sp_x)
    Offline_pixel_sp_x->Delete();
  if(Offline_pixel_sp_x_Ref)
    Offline_pixel_sp_x_Ref->Delete();
  if(Offline_pixel_sp_y)
    Offline_pixel_sp_y->Delete();
  if(Offline_pixel_sp_y_Ref)
    Offline_pixel_sp_y_Ref->Delete();
  if(Offline_pixel_sp_z)
    Offline_pixel_sp_z->Delete();
  if(Offline_pixel_sp_z_Ref)
    Offline_pixel_sp_z_Ref->Delete();




}


