#include <iostream>
#include "./atlasstyle-00-03-05/AtlasLabels.C"
#include "./atlasstyle-00-03-05/AtlasUtils.C"
#include "./atlasstyle-00-03-05/AtlasStyle.C"
void CreateColors();

void MakeSystd0z0(){
  
  SetAtlasStyle();
   
  TExec *SetPaletteColors = new TExec("SetPaletteColors", "CreateColors();");
  // Get maps
  //////////////////////////////////////////////////////////////////////////////////////////////
  
  std::vector<std::string> thelist;
  std::vector<std::string> thelist_sagitta;
  std::vector<std::string> theRMSlist;
  std::vector<std::string> theRMS2Dlist;
  
  std::string file1, file1_sagitta;
  std::vector<float> avglist;
  std::vector<double> centerx_list;
  std::vector<double> centery_list;
  std::vector<double> binx_list;
  std::vector<double> biny_list;
  std::vector<TH2D*> bin2Dvector_d0;
  std::vector<TH2D*> bin2Dvector_z0;
  std::vector<TH2D*> bin2Dvector_sagitta;
  std::vector<TH2D*> bin2Dvectornom_d0;
  std::vector<TH2D*> bin2Dvectornom_z0;
  std::vector<TH2D*> bin2Dvectornom_sagitta;
  std::vector<TH2D*> bin2DvectorRMS_d0;
  std::vector<TH2D*> bin2DvectorRMS_z0;
  std::vector<TH2D*> bin2DvectorRMS_sagitta;
  
  bool fDebug = false;
  bool doPreTSI = false;
  bool doPred0fix = false;
  bool doPostd0fix = true;
  
  
  //files containing original Zmumu maps for 30x30 epochs
  if(doPreTSI) file1 = "correctionmaps_Recommendations2016data_recopretsiwithrms_30x30.root";
  if(doPred0fix) file1 = "correctionmaps_Recommendations2016data_recopred0withrmsv2_30x30.root";
  if(doPostd0fix) file1 = "correctionmaps_Recommendations2016data_recopostd0withrmsv2_30x30.root";
  
  //files containing reweighted to E/p Zmumu maps for 30x30 epochs
  if(doPreTSI) file1_sagitta = "correctionmaps_pretsiwithrms_30x30_correctedEp.root";
  if(doPred0fix) file1_sagitta = "correctionmaps_pred0withrmsv2_30x30_correctedEp.root";
  if(doPostd0fix) file1_sagitta = "correctionmaps_postd0withrmsv2_30x30_correctedEp.root";
  
  //E/p files containing 30x30 epoch maps
  /*if(doPreTSI) file1_sagitta = "./eop_2016/preTS1/Scaling.root";
  if(doPred0fix) file1_sagitta = "./eop_2016/preD0fix/Scaling.root";
  if(doPostd0fix) file1_sagitta = "./eop_2016/postD0fix/Scaling.root";*/

  //compare Delta Sagitta Files (error of reweighting)
  TFile *inputHistosFile1, *inputHistosFile2;
  if(doPreTSI){
  inputHistosFile1 = TFile::Open("correctionmaps_Recommendations2016data_recopretsiwithrms_30x30.root");
  //inputHistosFile2 = TFile::Open("correctionmaps_pretsiwithrms_30x30_correctedEp.root");
  inputHistosFile2 = TFile::Open("PreTSI_reweightedToEP.root");

  }
  if(doPred0fix){
  inputHistosFile1 = TFile::Open("correctionmaps_Recommendations2016data_recopred0withrmsv2_30x30.root");
  //inputHistosFile2 = TFile::Open("correctionmaps_pred0withrmsv2_30x30_correctedEp.root");
  inputHistosFile2 = TFile::Open("Pred0fix_reweightedToEP.root");

  }
  if(doPostd0fix){
  inputHistosFile1 = TFile::Open("correctionmaps_Recommendations2016data_recopostd0withrmsv2_30x30.root");
  //inputHistosFile2 = TFile::Open("correctionmaps_postd0withrmsv2_30x30_correctedEp.root");
  inputHistosFile2 = TFile::Open("Postd0fix_reweightedToEP.root");

  }

  thelist.push_back("d0CorrectionVsEtaPhi");
  thelist.push_back("z0CorrectionVsEtaPhi");
  thelist.push_back("LambdaCorrectionVsEtaPhi");
  
  thelist_sagitta.push_back("d0CorrectionVsEtaPhi");
  thelist_sagitta.push_back("z0CorrectionVsEtaPhi");
  thelist_sagitta.push_back("LambdaCorrectionVsEtaPhi");
  //thelist_sagitta.push_back("LambdaCorrectionVsEtaPhi_reweightedToEP");
  //thelist_sagitta.push_back("FinalCorrections"); //for E/p systematics
  
  theRMSlist.push_back("d0CorrectionVsEta_RMS");
  theRMSlist.push_back("z0CorrectionVsEta_RMS");
  theRMSlist.push_back("LambdaCorrectionVsEta_RMS");
  
  theRMS2Dlist.push_back("d0CorrectionVsEtaPhi_RMS");
  theRMS2Dlist.push_back("z0CorrectionVsEtaPhi_RMS");
  theRMS2Dlist.push_back("LambdaCorrectionVsEtaPhi_RMS");
  
    

  std::vector<std::string> periodsfiles;
  std::vector<std::string> periodsfiles_sagitta;
  
  if(doPreTSI){
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodA_30x30.root.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodB_30x30.root.root");
  }
  
  if(doPred0fix){
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodC_30x30.root.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodD_30x30.root.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodE_30x30.root.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodF1_30x30.root.root");
  }

  bool do_d0z0 = true;
  bool do_sagitta = false;
 
  if(doPostd0fix){
  if(do_d0z0){
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodF2v3_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodGv2_30x30.root");//periodG missing for E/p maps! need to comment out this file to obtain sagitta reweighted maps, and uncomment to obtain d0, z0 maps. (Same number in periodsfiles and periodsfiles_sagitta is needed).
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodIv2_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodKv2_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodLv2_30x30.root");
  }
  if(do_sagitta){
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodF2v3_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodGv2_30x30.root");//periodG missing for E/p maps! need to comment out this to obtain sagitta reweighted maps, and uncomment to obtain d0, z0 maps. (Same number in periodsfiles and periodsfiles_sagitta is needed).
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodIv2_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodKv2_30x30.root");
  periodsfiles.push_back("correctionmaps_Recommendations2016data_recoperiodLv2_30x30.root");
  }
    
  }
  
  if(doPreTSI){
  periodsfiles_sagitta.push_back("./eop_2016/periodA/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodB/Scaling.root");
  }

  if(doPred0fix){
  periodsfiles_sagitta.push_back("./eop_2016/periodC/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodD/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodE/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodF1/Scaling.root");
  }

  if(doPostd0fix){
  //notice that period G is missing in this case
  if(do_sagitta){
  periodsfiles_sagitta.push_back("./eop_2016/periodF2/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodI/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodK/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodL/Scaling.root");
  }
  if(do_d0z0){ //these files are not really used for doing d0, z0! just adding the same number of files to the periodsfiles list!
  periodsfiles_sagitta.push_back("./eop_2016/periodF2/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodF2/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodI/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodK/Scaling.root");
  periodsfiles_sagitta.push_back("./eop_2016/periodL/Scaling.root");
  }
  }
  
  // Compare reference with bin fluctuations of subperiods maps
  //////////////////////////////////////////////////////////////////////////////
  int counter2 = 0;
  for( std::vector<std::string>::const_iterator it2 = periodsfiles.begin(); it2 != periodsfiles.end(); ++it2 ) {
    ++counter2;
    std::cout << "Current file: "<<*it2 << std::endl;
    
    std::string par2 = *it2;
    
    TFile *fin2 = new TFile(periodsfiles.at(counter2-1).c_str());
    TFile *fin2_sagitta = new TFile(periodsfiles_sagitta.at(counter2-1).c_str());
    

    TH2D* hist, *hist3;
    int counter3 = 0;
    for( std::vector<std::string>::const_iterator it3 = thelist_sagitta.begin(); it3 != thelist_sagitta.end(); ++it3 ) {
      ++counter3;
      std::cout << "Current histogram: "<<*it3 << std::endl;
      std::string par3 = *it3;
      
      TFile *fin1 = new TFile(file1.c_str());
      TFile *fin1_sagitta = new TFile(file1_sagitta.c_str());
      
      if( counter3 < 3){
	hist = (TH2D*)fin1->Get(par3.c_str());     
	hist3 = (TH2D*)fin2->Get(par3.c_str());
      }
      
      else{
	hist = (TH2D*)fin1->Get(par3.c_str()); //fin1_sagitta  
	hist3 = (TH2D*)fin2->Get(par3.c_str()); //fin2_sagitta
        //hist->Scale(1000.0);//E/p in GeV^{-1}!
        //hist3->Scale(1000.0); //E/p in GeV^{-1}!
      }
      
      
      std::string histname3 = hist3->GetName();
      cout <<"histname : "<< histname3 <<  endl;
      
      
      
      // Calculate means for the 2D map
      /////////////////////////////////////////////////////////////////////////////
      std::cout << "y bins : "<<  hist3->GetYaxis()->GetNbins()<<  std::endl;
      std::cout << "x bins : "<<  hist3->GetXaxis()->GetNbins()<<  std::endl;
      
      double theMin= hist3->GetBinContent(hist3->GetMinimumBin());
      double theMax = hist3->GetBinContent(hist3->GetMaximumBin());
      double theGap = theMax - theMin;
      theMax = theMax + 0.05*theGap;
      theMin = theMin -0.05*theGap;
      
      TString h_name;
      TString h_name_nom;
      
      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_name.Form("d0Diff_%d",counter2); }
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_name.Form("z0Diff_%d",counter2); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_name.Form("LambdaDiff_%d",counter2);}
      //if(histname3.find("FinalCorrections") != string::npos ) { h_name.Form("LambdaDiff_%d",counter2);}

      
      TH1D* bin =  new TH1D(h_name.Data(),h_name.Data(),100,theMin,theMax);
      
      TString axisname(hist3->GetZaxis()->GetTitle());
      axisname.Prepend("#Delta");
      bin->GetXaxis()->SetTitle(axisname.Data());
      
      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_name.Form("d0CorrectionVsEtaPhi_%d",counter2); }
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_name.Form("z0CorrectionVsEtaPhi_%d",counter2); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_name.Form("LambdaCorrectionVsEtaPhi_%d",counter2);}
      //if(histname3.find("FinalCorrections") != string::npos ) { h_name.Form("LambdaCorrectionVsEtaPhi_%d",counter2);}

      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_name_nom.Form("d0CorrectionVsEtaPhi_%d",counter2); }
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_name_nom.Form("z0CorrectionVsEtaPhi_%d",counter2); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_name_nom.Form("LambdaCorrectionVsEtaPhi_%d",counter2);}
      //if(histname3.find("FinalCorrections") != string::npos ) { h_name_nom.Form("LambdaCorrectionVsEtaPhi_%d",counter2);}
  
            
      TH2D* bin2D =  new TH2D(h_name.Data(),h_name.Data(),
			      hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(),   //hist3 
			      hist->GetNbinsY(), hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax());  //hist3
      
      bin2D->SetStats(kFALSE);
      bin2D->GetZaxis()->SetTitle(axisname.Data());
      bin2D->GetXaxis()->SetTitle(hist3->GetXaxis()->GetTitle());
      bin2D->GetYaxis()->SetTitle(hist3->GetYaxis()->GetTitle());
      
      
      TH2D* bin2D_nom =  new TH2D(h_name_nom.Data(),h_name_nom.Data(),
				  hist->GetNbinsX(), hist->GetXaxis()->GetXmin(), hist->GetXaxis()->GetXmax(), 
				  hist->GetNbinsY(), hist->GetYaxis()->GetXmin(), hist->GetYaxis()->GetXmax());
      
      bin2D_nom->SetStats(kFALSE);
      bin2D_nom->GetZaxis()->SetTitle(hist->GetZaxis()->GetTitle());
      bin2D_nom->GetXaxis()->SetTitle(hist->GetXaxis()->GetTitle());
      bin2D_nom->GetYaxis()->SetTitle(hist->GetYaxis()->GetTitle());
      /*bin2D_nom->GetZaxis()->SetTitle("sagitta [TeV^{-1}]");
      bin2D_nom->GetXaxis()->SetTitle("#eta");
      bin2D_nom->GetYaxis()->SetTitle("#phi");*/
      
           
      
      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { bin2Dvector_d0.push_back(bin2D);}
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) {bin2Dvector_z0.push_back(bin2D); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { bin2Dvector_sagitta.push_back(bin2D);}
      //if(histname3.find("FinalCorrections") != string::npos ) { bin2Dvector_sagitta.push_back(bin2D);}

      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { bin2Dvectornom_d0.push_back(bin2D_nom);}
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) {bin2Dvectornom_z0.push_back(bin2D_nom); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { bin2Dvectornom_sagitta.push_back(bin2D_nom);}
      //if(histname3.find("FinalCorrections") != string::npos ) { bin2Dvectornom_sagitta.push_back(bin2D_nom);}

      float sum1 = 0;
      hist3->Sumw2(); 
      /*double avg = 0; 
      if(histname3.find("d0CorrectionVsEtaPhi") != string::npos ) { avg = avglist.at(0); }
      if(histname3.find("z0CorrectionVsEtaPhi") != string::npos ) { avg = avglist.at(1); }
      if(histname3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { avg = avglist.at(2);}*/
      
      //OLD METHOD -- SYSTEMATICS FROM THE ORIGINAL SAGITTA MAPS (SAME BINNING FOR SUBPERIODS AND EPOCHS). USEFUL FOR D0 AND Z0.
      if(counter3 < 3 ){
	for(int x = 1; x <=  hist3->GetXaxis()->GetNbins(); x++){
	  for(int y = 1; y <=  hist3->GetYaxis()->GetNbins(); y++){
	    if(fDebug){  cout <<  "#ZMUMU## Bin content in bin (" <<x << "," << y<<") : "
			    << hist3->GetBinContent(x,y)
			    << " Value : "<< hist3->GetBinContent(x,y) 
			    //<< " Avg: " << avg
			    << " Value subperiod - value big period: "<<hist3->GetBinContent(x,y)-hist->GetBinContent(x,y) << endl;
	    }
	    double diff_value;
	    diff_value= hist3->GetBinContent(x,y)-hist->GetBinContent(x,y);
	    bin->Fill(diff_value);
	    bin2D->SetBinContent(x,y, diff_value);  
	    bin2D_nom->SetBinContent(x,y, hist->GetBinContent(x,y));          
	  }
	}
      }
      else{
	//NEW METHOD -- SYSTEMATICS FROM THE REWEIGHTED SAGITTA MAPS (DIFFERENT BINNING FOR SUBPERIODS AND EPOCHS)
	for(int x = 1; x <=  hist->GetXaxis()->GetNbins(); x++){//hist
	  double center_x = hist->GetXaxis()->GetBinCenter(x);
	  if(fDebug) cout <<  "#ZMUMU REWEIGHTED## Bin center in eta bin (" <<x << ")" <<   hist->GetXaxis()->GetBinCenter(x) << endl;
	  centerx_list.push_back(center_x); 
	}
 
	
      for(int y = 1; y <=  hist->GetYaxis()->GetNbins(); y++){//hist
        double center_y = hist->GetYaxis()->GetBinCenter(y);
        if(fDebug) cout <<  "#ZMUMU REWEIGHTED## Bin center in phi bin (" <<y << ")" <<   hist->GetYaxis()->GetBinCenter(y) << endl;
        centery_list.push_back(center_y);
      }

                 
      for(int x = 0; x <  hist->GetXaxis()->GetNbins(); x++){//hist
	for(int y = 0; y <  hist->GetYaxis()->GetNbins(); y++){
          //find bins of the 16x16 subperiods sagitta maps corresponding to the centers of the 30x30 sagitta maps
	  double diff_value;
          double bin_x = hist3->GetXaxis()->FindBin(centerx_list.at(x));
          double bin_y = hist3->GetYaxis()->FindBin(centery_list.at(y));
	  diff_value = hist3->GetBinContent(bin_x,bin_y) - hist->GetBinContent(x+1,y+1); 
	  if(fDebug){
	    cout << "CENTERS: " << centerx_list.at(x) << "/" << centery_list.at(y) << endl;                                                                cout <<  "#ZMUMU REWEIGHTED## Bin number in ETA,PHI bin: " << bin_x << "/" << bin_y << endl; 
	    cout << "HIST3 BINX VALUE: " << centerx_list.at(x) << "," << centery_list.at(y) << ": " << hist3->GetBinContent(bin_x,bin_y) << endl; 
	    cout << "HIST VALUE: " << hist->GetXaxis()->GetBinCenter(x+1) << "," << hist->GetYaxis()->GetBinCenter(y+1) << ": " << hist->GetBinContent(x+1,y+1) << endl; 
	  }
	  bin->Fill(diff_value);
	  bin2D->SetBinContent(x+1,y+1, diff_value);  
          bin2D_nom->SetBinContent(x+1,y+1, hist->GetBinContent(x+1,y+1));          
	}
      }
      } 
      
      TCanvas *theCanvas = new TCanvas("theCanvas", "canvas name", 800, 600);
      theCanvas->Divide(1,2);
      theCanvas->cd(1);
      bin->Draw("");
      theCanvas->cd(2);
      bin2D->Draw("colz");
      h_name.Append(".pdf");
      theCanvas->Print(h_name.Data());
      
      
      
    }
  }
  
  TH2D * tempHist = bin2Dvector_d0.at(0);
  TH2D * tempHistnom = bin2Dvectornom_d0.at(0);
  //tempHist->GetZaxis()->SetTitleOffset(1.6);
  
  TH2D* theMaximumDiff_d0 = new TH2D("theMaximumDiff_d0","d_{0} syst. subperiod uncertainty",
				     tempHist->GetNbinsX(), tempHist->GetXaxis()->GetXmin(), tempHist->GetXaxis()->GetXmax(), 
				     tempHist->GetNbinsY(), tempHist->GetYaxis()->GetXmin(), tempHist->GetYaxis()->GetXmax());    
  
  theMaximumDiff_d0->GetXaxis()->SetTitle(tempHist->GetXaxis()->GetTitle());
  theMaximumDiff_d0->GetYaxis()->SetTitle(tempHist->GetYaxis()->GetTitle()); 
  theMaximumDiff_d0->GetZaxis()->SetTitle(tempHist->GetZaxis()->GetTitle());
  theMaximumDiff_d0->GetZaxis()->SetTitleOffset(1.9); 
  theMaximumDiff_d0->GetYaxis()->SetTitleOffset(1.4); 
  
  TH2D* theNominal_d0 = new TH2D("theNominal_d0","",
				 tempHistnom->GetNbinsX(), tempHistnom->GetXaxis()->GetXmin(), tempHistnom->GetXaxis()->GetXmax(), 
				 tempHistnom->GetNbinsY(), tempHistnom->GetYaxis()->GetXmin(), tempHistnom->GetYaxis()->GetXmax());    
  
  theNominal_d0->GetXaxis()->SetTitle(tempHistnom->GetXaxis()->GetTitle());
  theNominal_d0->GetYaxis()->SetTitle(tempHistnom->GetYaxis()->GetTitle()); 
  theNominal_d0->GetZaxis()->SetTitle(tempHistnom->GetZaxis()->GetTitle());
  theNominal_d0->GetZaxis()->SetTitleOffset(1.5); 
  theNominal_d0->GetYaxis()->SetTitleOffset(1.4); 
  
  tempHist = bin2Dvector_z0.at(0);
  tempHistnom = bin2Dvectornom_z0.at(0);
  
  TH2D* theMaximumDiff_z0 = new TH2D("theMaximumDiff_z0","z_{0} syst. subperiod uncertainty",
				      tempHist->GetNbinsX(), tempHist->GetXaxis()->GetXmin(), tempHist->GetXaxis()->GetXmax(), 
				     tempHist->GetNbinsY(), tempHist->GetYaxis()->GetXmin(), tempHist->GetYaxis()->GetXmax());
  
  theMaximumDiff_z0->GetXaxis()->SetTitle(tempHist->GetXaxis()->GetTitle());
  theMaximumDiff_z0->GetYaxis()->SetTitle(tempHist->GetYaxis()->GetTitle()); 
  theMaximumDiff_z0->GetZaxis()->SetTitle(tempHist->GetZaxis()->GetTitle()); 
  theMaximumDiff_z0->GetZaxis()->SetTitleOffset(1.9); 
  theMaximumDiff_z0->GetYaxis()->SetTitleOffset(1.4); 
   
  TH2D* theNominal_z0 = new TH2D("theNominal_z0","",
				 tempHistnom->GetNbinsX(), tempHistnom->GetXaxis()->GetXmin(), tempHistnom->GetXaxis()->GetXmax(), 
				 tempHistnom->GetNbinsY(), tempHistnom->GetYaxis()->GetXmin(), tempHistnom->GetYaxis()->GetXmax());    
  
  theNominal_z0->GetXaxis()->SetTitle(tempHistnom->GetXaxis()->GetTitle());
  theNominal_z0->GetYaxis()->SetTitle(tempHistnom->GetYaxis()->GetTitle()); 
  theNominal_z0->GetZaxis()->SetTitle(tempHistnom->GetZaxis()->GetTitle());
  theNominal_z0->GetZaxis()->SetTitleOffset(1.5); 
  theNominal_z0->GetYaxis()->SetTitleOffset(1.4); 
  
  tempHist = bin2Dvector_sagitta.at(0);
  tempHistnom = bin2Dvectornom_sagitta.at(0);
  
  TH2D* theMaximumDiff_sagitta = new TH2D("theMaximumDiff_sagitta","sagitta syst. subperiod  uncertainty",
					  tempHist->GetNbinsX(), tempHist->GetXaxis()->GetXmin(), tempHist->GetXaxis()->GetXmax(), 
					  tempHist->GetNbinsY(), tempHist->GetYaxis()->GetXmin(), tempHist->GetYaxis()->GetXmax());
  
  
  theMaximumDiff_sagitta->GetXaxis()->SetTitle(tempHist->GetXaxis()->GetTitle());
  theMaximumDiff_sagitta->GetYaxis()->SetTitle(tempHist->GetYaxis()->GetTitle()); 
  theMaximumDiff_sagitta->GetZaxis()->SetTitle(tempHist->GetZaxis()->GetTitle()); 
  theMaximumDiff_sagitta->GetZaxis()->SetTitleOffset(1.5); //1.9 
  theMaximumDiff_sagitta->GetYaxis()->SetTitleOffset(1.4); 
  
  TH2D* theNominal_sagitta = new TH2D("theNominal_sagitta","",
				      tempHistnom->GetNbinsX(), tempHistnom->GetXaxis()->GetXmin(), tempHistnom->GetXaxis()->GetXmax(), 
				      tempHistnom->GetNbinsY(), tempHistnom->GetYaxis()->GetXmin(), tempHistnom->GetYaxis()->GetXmax());    

  theNominal_sagitta->GetXaxis()->SetTitle(tempHistnom->GetXaxis()->GetTitle());
  theNominal_sagitta->GetYaxis()->SetTitle(tempHistnom->GetYaxis()->GetTitle()); 
  theNominal_sagitta->GetZaxis()->SetTitle(tempHistnom->GetZaxis()->GetTitle());
  theNominal_sagitta->GetZaxis()->SetTitleOffset(1.5); 
  theNominal_sagitta->GetYaxis()->SetTitleOffset(1.4); 
  
  theHist1 = (TH2D*)inputHistosFile1->Get("LambdaCorrectionVsEtaPhi");
  theHist2 = (TH2D*)inputHistosFile2->Get("LambdaCorrectionVsEtaPhi_reweightedToEP"); 
  
  
  TFile *fcorr = new TFile("UncertaintiesResult_Postd0fix.root","RECREATE");
    
  TDirectory *cdd0 = fcorr->mkdir("d0");
    
  TDirectory *cdz0 = fcorr->mkdir("z0");
    
  TDirectory *cdsagitta = fcorr->mkdir("sagitta");
    
  
  int counter3 = 0;
  for( std::vector<std::string>::const_iterator it4 = thelist.begin(); it4 != thelist.end(); ++it4 ) {
    ++counter3;
    std::cout << "Current histogram: "<<*it4 << std::endl;
     std::string par3 = *it4;
     par3 = par3 + "_RMS";
     std::string entries = "entries";
     

     
     TFile *fin1 = new TFile(file1.c_str());
     TH2D* histrms_pure = (TH2D*)fin1->Get(par3.c_str());
     histrms_pure->GetZaxis()->SetTitleOffset(1.9);
     TH2D* histentries = (TH2D*)fin1->Get(entries.c_str());
     
     
     if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {histrms_pure->SetTitle("d_{0} stat. uncertainty");}
     if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histrms_pure->SetTitle("z_{0} stat. uncertainty");}
     if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {histrms_pure->SetTitle("sagitta stat. uncertainty");} 
     
     TH2D* histrms = new TH2D("histrms",histrms_pure->GetTitle(),
			      histrms_pure->GetNbinsX(), histrms_pure->GetXaxis()->GetXmin(), histrms_pure->GetXaxis()->GetXmax(), 
			      histrms_pure->GetNbinsY(), histrms_pure->GetYaxis()->GetXmin(), histrms_pure->GetYaxis()->GetXmax());
     
     histrms->GetXaxis()->SetTitle(histrms_pure->GetXaxis()->GetTitle());
     histrms->GetYaxis()->SetTitle(histrms_pure->GetYaxis()->GetTitle()); 
     //histrms->GetZaxis()->SetTitle("error #delta_{sagitta}");
     if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {histrms->GetZaxis()->SetTitle("stat. error d_{0} [mm]");}
     if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histrms->GetZaxis()->SetTitle("stat. error z_{0} [mm]");}
     if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {histrms->GetZaxis()->SetTitle("stat. error #delta_{sagitta} [TeV^{-1}]");} 
     
     histrms->GetYaxis()->SetTitleOffset(1.4);
     histrms->GetZaxis()->SetTitleOffset(2.1);
     
     TH2D* histmax, *histnom;
     if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {histmax = theMaximumDiff_d0;}
     if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) {histmax = theMaximumDiff_z0;}
     if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {histmax = theMaximumDiff_sagitta;}

     
     
     if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {histnom = theNominal_d0;}
     if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) {histnom = theNominal_z0;}
     if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {histnom = theNominal_sagitta;} 
     
     
     
     histmax->SetStats(kFALSE);
     TH2D* theHist, *theNomHist;
     
     
     for (int i=0; i< (int) bin2Dvector_d0.size(); i++) {
       if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {theHist = bin2Dvector_d0.at(i);}
       if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) {theHist = bin2Dvector_z0.at(i);}
       if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {theHist = bin2Dvector_sagitta.at(i);} 
       
       if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {theNomHist = bin2Dvectornom_d0.at(i);}
       if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) {theNomHist = bin2Dvectornom_z0.at(i);}
       if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {theNomHist = bin2Dvectornom_sagitta.at(i);} 
       
       for(int etabin = 1; etabin <=  theHist->GetXaxis()->GetNbins(); etabin++){
	 for(int phibin = 1; phibin <=  theHist->GetYaxis()->GetNbins(); phibin++){
	   if( fabs(theHist-> GetBinContent(etabin, phibin)) > histmax->GetBinContent(etabin, phibin)) histmax->SetBinContent( etabin, phibin, fabs(theHist-> GetBinContent(etabin, phibin))); 
	 }
       }     
       
       for(int etabin = 1; etabin <=  theNomHist->GetXaxis()->GetNbins(); etabin++){
	 for(int phibin = 1; phibin <=  theNomHist->GetYaxis()->GetNbins(); phibin++){
	   histnom->SetBinContent( etabin, phibin, theNomHist->GetBinContent(etabin, phibin)); 
	 }
       }
       
                 
       for(int etabin = 1; etabin <=  histrms->GetXaxis()->GetNbins(); etabin++){
	 for(int phibin = 1; phibin <=  histrms->GetYaxis()->GetNbins(); phibin++){
	   double RMScontent = histrms_pure->GetBinContent(etabin,phibin);
	   double entries = histentries->GetBinContent(etabin,phibin);
	   //cout << "Before dividing over sqrt(n):" << RMScontent << "   with entries: " << entries << endl;                 
	   histrms->SetBinContent(etabin,phibin,RMScontent/sqrt(entries));
	   //cout << "After dividing over sqrt(n):" <<  histrms->GetBinContent(etabin,phibin) << endl;
	   //cout << "-----" << endl;
	   
	 }
       }
       
       
     }
     
     
     
     TString h_title;
     if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s syst. subperiod uncertainty profile #eta", "d_{0}");}
     if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s syst. subperiod uncertainty profile #eta", "z_{0}");}
     if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s syst. subperiod uncertainty profile #eta", "sagitta");}
 
     
      
      TProfile *profile_eta = new TProfile("profile_eta",h_title.Data(),
					   theHist->GetNbinsX(), theHist->GetXaxis()->GetXmin(), theHist->GetXaxis()->GetXmax(), "i");
      
      profile_eta->SetStats(kFALSE);
      profile_eta->GetXaxis()->SetTitle(histmax->GetXaxis()->GetTitle());
      profile_eta->GetYaxis()->SetTitle(histmax->GetZaxis()->GetTitle());
      profile_eta->SetLineWidth(2);
      profile_eta->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_eta->SetMarkerColor(profile_eta->GetLineColor());
      for(int etabin = 1; etabin <=  histmax->GetXaxis()->GetNbins(); etabin++){
	for(int phibin = 1; phibin <=  histmax->GetYaxis()->GetNbins(); phibin++){
	  profile_eta->Fill(histmax->GetXaxis()->GetBinCenter(etabin), histmax->GetBinContent(etabin,phibin));
	  //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	}
      }
      
      TString h_title_phi;
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title_phi.Form("%s syst. subperiod uncertainty profile #phi", "d_{0}");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title_phi.Form("%s syst. subperiod uncertainty profile #phi", "z_{0}");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title_phi.Form("%s syst. subperiod uncertainty profile #phi", "sagitta");}

      TProfile *profile_phi = new TProfile("profile_phi",h_title_phi.Data(),
					   theHist->GetNbinsX(), theHist->GetYaxis()->GetXmin(), theHist->GetYaxis()->GetXmax(), "i");
      
      profile_phi->SetStats(kFALSE);
      profile_phi->GetXaxis()->SetTitle(histmax->GetYaxis()->GetTitle());
      profile_phi->GetYaxis()->SetTitle(histmax->GetZaxis()->GetTitle());
      profile_phi->SetLineWidth(2);
      profile_phi->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_phi->SetMarkerColor(profile_phi->GetLineColor());
      for(int phibin = 1; phibin <=  histmax->GetYaxis()->GetNbins(); phibin++){
	for(int etabin = 1; etabin <=  histmax->GetXaxis()->GetNbins(); etabin++){
	  profile_phi->Fill(histmax->GetYaxis()->GetBinCenter(phibin), histmax->GetBinContent(etabin,phibin));
	  //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	}
      }
      
      
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #eta", "d_{0}");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #eta", "z_{0}");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #eta", "sagitta");}
      
      
      TProfile *profile_eta_nom = new TProfile("profile_eta_nom",h_title.Data(),
					       theNomHist->GetNbinsX(), theNomHist->GetXaxis()->GetXmin(), theNomHist->GetXaxis()->GetXmax(), "i");
      
      profile_eta_nom->SetStats(kFALSE);
      profile_eta_nom->GetXaxis()->SetTitle(histnom->GetXaxis()->GetTitle());
      profile_eta_nom->GetYaxis()->SetTitle(histnom->GetZaxis()->GetTitle());
      profile_eta_nom->SetLineWidth(2);
      profile_eta_nom->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_eta_nom->SetMarkerColor(profile_eta_nom->GetLineColor());
      for(int etabin = 1; etabin <=  histnom->GetXaxis()->GetNbins(); etabin++){
	for(int phibin = 1; phibin <=  histnom->GetYaxis()->GetNbins(); phibin++){
	       profile_eta_nom->Fill(histnom->GetXaxis()->GetBinCenter(etabin), histnom->GetBinContent(etabin,phibin));
               //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	     }
      }
      
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #phi", "d_{0}");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #phi", "z_{0}");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s profile #phi", "sagitta");}
      
      
     TProfile *profile_phi_nom = new TProfile("profile_phi_nom",h_title.Data(),
					      theNomHist->GetNbinsX(), theNomHist->GetYaxis()->GetXmin(), theNomHist->GetYaxis()->GetXmax(), "i");
     
      profile_phi_nom->SetStats(kFALSE);
      profile_phi_nom->GetXaxis()->SetTitle(histnom->GetYaxis()->GetTitle());
      profile_phi_nom->GetYaxis()->SetTitle(histnom->GetZaxis()->GetTitle());
      profile_phi_nom->SetLineWidth(2);
      profile_phi_nom->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_phi_nom->SetMarkerColor(profile_phi_nom->GetLineColor());
      for(int phibin = 1; phibin <=  histnom->GetYaxis()->GetNbins(); phibin++){
	for(int etabin = 1; etabin <=  histnom->GetXaxis()->GetNbins(); etabin++){
	  profile_phi_nom->Fill(histnom->GetYaxis()->GetBinCenter(phibin), histnom->GetBinContent(etabin,phibin));
	  //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	}
      }
      
      
      //extract from *_EtaRMS 
      /*TH1D* theRMS_d0, * theRMS_z0,* theRMS_sagitta;
	int counter4 = 0;
	for( std::vector<std::string>::const_iterator it5 = theRMSlist.begin(); it5 != theRMSlist.end(); ++it5 ) {
	++counter4;
	std::cout << "Current histogram: "<<*it5 << std::endl;
	std::string par3 = *it5;
	TFile *fin4 = new TFile(file1.c_str());
	TH1D* hist4 = (TH1D*)fin4->Get(par3.c_str());
	
	
	if(par3.find("d0CorrectionVsEta_RMS") != string::npos ) theRMS_d0 = hist4;
	if(par3.find("z0CorrectionVsEta_RMS") != string::npos ) theRMS_z0 = hist4;
	if(par3.find("LambdaCorrectionVsEta_RMS") != string::npos ) theRMS_sagitta = hist4;
	}  
	
       
	theRMS_d0->SetStats(kFALSE);
	theRMS_d0->SetTitle("Stat. uncertainty");
	theRMS_d0->GetXaxis()->SetTitle(histmax->GetXaxis()->GetTitle());
	theRMS_d0->GetYaxis()->SetTitle(histmax->GetZaxis()->GetTitle());
	theRMS_d0->SetLineWidth(2);
	theRMS_d0->SetMarkerStyle(20);
	//profile_eta->SetMarkerSize(0.3);
	theRMS_d0->SetMarkerColor(theRMS_d0->GetLineColor());
	
	theRMS_z0->SetStats(kFALSE);
	theRMS_z0->SetTitle("Stat. uncertainty");
	theRMS_z0->GetXaxis()->SetTitle(histmax->GetXaxis()->GetTitle());
	theRMS_z0->GetYaxis()->SetTitle(histmax->GetZaxis()->GetTitle());
	theRMS_z0->SetLineWidth(2);
	theRMS_z0->SetMarkerStyle(20);
	//profile_eta->SetMarkerSize(0.3);
	theRMS_z0->SetMarkerColor(theRMS_z0->GetLineColor());
	
	theRMS_sagitta->SetStats(kFALSE);
	theRMS_sagitta->SetTitle("Stat. uncertainty");
	theRMS_sagitta->GetXaxis()->SetTitle(histmax->GetXaxis()->GetTitle());
	theRMS_sagitta->GetYaxis()->SetTitle(histmax->GetZaxis()->GetTitle());
	theRMS_sagitta->SetLineWidth(2);
	theRMS_sagitta->SetMarkerStyle(20);
	//profile_eta->SetMarkerSize(0.3);
	theRMS_sagitta->SetMarkerColor(theRMS_sagitta->GetLineColor());*/
      
      
      //extract from *_EtaPhiRMS
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #eta", "d_{0}");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #eta", "z_{0}");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #eta", "sagitta");}
      
      TProfile *profile_eta_rms = new TProfile("profile_eta_rms",h_title.Data(),
					       histrms->GetNbinsX(), histrms->GetXaxis()->GetXmin(), histrms->GetXaxis()->GetXmax(), "i");
      
      profile_eta_rms->SetStats(kFALSE);
      profile_eta_rms->GetXaxis()->SetTitle(histrms->GetXaxis()->GetTitle());
      profile_eta_rms->GetYaxis()->SetTitle(histrms->GetZaxis()->GetTitle());
      profile_eta_rms->SetLineWidth(2);
      profile_eta_rms->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_eta_rms->SetMarkerColor(profile_eta_rms->GetLineColor());
      
      
      for(int etabin = 1; etabin <=  histrms->GetXaxis()->GetNbins(); etabin++){
	for(int phibin = 1; phibin <=  histrms->GetYaxis()->GetNbins(); phibin++){
	  profile_eta_rms->Fill(histrms->GetXaxis()->GetBinCenter(etabin), histrms->GetBinContent(etabin,phibin));
	  //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	}
      }
      
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #phi", "d_{0}");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #phi", "z_{0}");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_title.Form("%s stat. uncertainty profile #phi", "sagitta");}
      
      TProfile *profile_phi_rms = new TProfile("profile_phi_rms",h_title.Data(),
                                      histrms->GetNbinsY(), histrms->GetYaxis()->GetXmin(), histrms->GetYaxis()->GetXmax(), "i");
      
      profile_phi_rms->SetStats(kFALSE);
      profile_phi_rms->GetXaxis()->SetTitle(histrms->GetYaxis()->GetTitle());
      profile_phi_rms->GetYaxis()->SetTitle(histrms->GetZaxis()->GetTitle());
      profile_phi_rms->SetLineWidth(2);
      profile_phi_rms->SetMarkerStyle(20);
      //profile_eta->SetMarkerSize(0.3);
      profile_phi_rms->SetMarkerColor(profile_phi_rms->GetLineColor());
      
      for(int phibin = 1; phibin <=  histrms->GetYaxis()->GetNbins(); phibin++){
	for(int etabin = 1; etabin <=  histrms->GetXaxis()->GetNbins(); etabin++){
	  profile_phi_rms->Fill(histrms->GetYaxis()->GetBinCenter(phibin), histrms->GetBinContent(etabin,phibin));
	  //cout << "Bin Center: " << theHist->GetXaxis()->GetBinCenter(etabin) << " BinContent : " << theHist->GetBinContent(etabin,phibin) << endl;
	}
      }
      
      TH2F* histDiff = new TH2F("histDiff","#delta_{sagitta} difference",
				theHist1->GetXaxis()->GetNbins(), theHist1->GetXaxis()->GetXmin(), theHist1->GetXaxis()->GetXmax(),  
				theHist1->GetYaxis()->GetNbins(), theHist1->GetYaxis()->GetXmin(), theHist1->GetYaxis()->GetXmax());  
      histDiff->SetXTitle(theHist2->GetXaxis()->GetTitle());
      histDiff->SetYTitle(theHist2->GetYaxis()->GetTitle());
      //theHist2->SetTitle("theNominal_sagitta");
      histDiff->GetZaxis()->SetTitleOffset(1.6);
      for (int etabin=1; etabin <= theHist1->GetNbinsX(); etabin++) {
	for (int phibin=1; phibin <= theHist1->GetNbinsY(); phibin++) {
          if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) histDiff->SetBinContent(etabin,phibin,0.);
          if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) histDiff->SetBinContent(etabin,phibin,0.);
          if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) histDiff->SetBinContent(etabin, phibin, theHist1->GetBinContent(etabin, phibin) - theHist2->GetBinContent(etabin, phibin)); 
	}
      }
      
      
      TH2D* histerror = new TH2D("histerror","histerror",
				 histrms->GetNbinsX(), histrms->GetXaxis()->GetXmin(), histrms->GetXaxis()->GetXmax(), 
				 histrms->GetNbinsY(), histrms->GetYaxis()->GetXmin(), histrms->GetYaxis()->GetXmax());   
      double rms , max, ep = 0;
      for(int etabin = 1; etabin <=  histrms->GetXaxis()->GetNbins(); etabin++){
	for(int phibin = 1; phibin <=  histrms->GetYaxis()->GetNbins(); phibin++){
	  rms = histrms->GetBinContent(etabin,phibin);
	  max = histmax->GetBinContent(etabin,phibin);

	  if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) ep = 0.018;//ep = histDiff->GetBinContent(etabin,phibin);
          if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) ep = 0.0;
          if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) ep = 0.0;

	  histerror->SetBinContent(etabin, phibin, sqrt( pow(rms,2) + pow(max,2) + pow(ep,2) ));
	  //cout << "Hist error: " << histerror->GetBinContent(etabin,phibin) << "=" << rms << "+" << max << "+" << ep << endl;
	}
      }
      
      TH1 *h_eta = new TH1F("h1", "", 30, -2.5, 2.5);
      TH1 *h_sys_eta = new TH1F("h1_sys_eta", ";#eta;Value", 30, -2.5, 2.5);
      TH1 *h_sys_eta_ep = new TH1F("h1_sys_eta_ep", ";#eta;Value", 30, -2.5, 2.5);
      TH1 *h_tot_eta = new TH1F("h1_tot_eta", ";#eta;Value", 30, -2.5, 2.5);

      TH1 *h_phi = new TH1F("h1", "", 30, -3.14, 3.14);
      TH1 *h_sys_phi = new TH1F("h1_sys_phi", ";#phi;Value", 30, -3.14, 3.14);
      TH1 *h_sys_phi_ep = new TH1F("h1_sys_phi_ep", ";#phi;Value", 30, -3.14, 3.14);
      TH1 *h_tot_phi = new TH1F("h1_tot_phi", ";#phi;Value", 30, -3.14, 3.14);
      
      
    for(int ibin=0; ibin<30; ibin++) {
      double value_eta = profile_eta_nom->GetBinContent(ibin+1);
      double stat_eta =  profile_eta_rms->GetBinContent(ibin+1);
      double sys_eta = profile_eta->GetBinContent(ibin+1);
      double sys_eta_ep = 0.015;//histDiff->GetBinContent(1,1); //same value for all bins in the e/p error
      double value_phi = profile_phi_nom->GetBinContent(ibin+1);
      double stat_phi =  profile_phi_rms->GetBinContent(ibin+1);
      double sys_phi = profile_phi->GetBinContent(ibin+1);
      double sys_phi_ep = 0.015;//histDiff->GetBinContent(1,1); //same value for all bins in the e/p error
      
      h_eta->SetBinContent(ibin+1, value_eta );
      h_eta->SetBinError(ibin+1, stat_eta);
      h_phi->SetBinContent(ibin+1, value_phi );
      h_phi->SetBinError(ibin+1, stat_phi);
      
      h_sys_eta->SetBinContent(ibin+1, value_eta );
      h_sys_eta_ep->SetBinContent(ibin+1, value_eta );
      h_tot_eta->SetBinContent(ibin+1, value_eta );
      h_sys_phi->SetBinContent(ibin+1, value_phi );
      h_tot_phi->SetBinContent(ibin+1, value_phi );
      h_sys_phi_ep->SetBinContent(ibin+1, value_phi );
      
      h_sys_eta->SetBinError(ibin+1, sys_eta);
      h_sys_eta_ep->SetBinError(ibin+1, sys_eta_ep);
      h_tot_eta->SetBinError(ibin+1, sqrt( stat_eta*stat_eta + sys_eta*sys_eta + sys_eta_ep*sys_eta_ep) );
      
      h_sys_phi->SetBinError(ibin+1, sys_phi);
      h_sys_phi_ep->SetBinError(ibin+1, sys_phi_ep);
      h_tot_phi->SetBinError(ibin+1, sqrt( stat_phi*stat_phi + sys_phi*sys_phi + sys_phi_ep*sys_phi_ep) );
      
 
    }
    
    TCanvas *theEtaErrorsCanvas = new TCanvas("theEtaErrorsCanvas", "canvas name", 1000, 800);
    
    h_tot_eta->SetFillColor(kAzure);
    h_tot_eta->SetFillStyle(1001);
    h_tot_eta->SetStats(kFALSE);
    h_tot_eta->GetYaxis()->SetTitleOffset(1.6);
    if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(0.015); h_tot_eta->SetMinimum(-0.015); h_tot_eta->GetYaxis()->SetTitle(Form("%s [mm]", "d_{0}"));};
    if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(0.14); h_tot_eta->SetMinimum(-0.14); h_tot_eta->GetYaxis()->SetTitle(Form("%s [mm]", "z_{0}"));}
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(1.6); h_tot_eta->SetMinimum(-1.6); h_tot_eta->GetYaxis()->SetTitle(Form("%s [TeV^{-1}]", "sagitta"));}
    h_tot_eta->Draw("e2");
    h_sys_eta->SetFillColor(kGray);
    h_sys_eta->SetFillStyle(1001);
    h_sys_eta->Draw("e2,same");
    h_sys_eta_ep->SetFillColor(kGreen);
    h_sys_eta_ep->SetFillStyle(1001);
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {h_sys_eta_ep->Draw("e2,same");}
    
    h_eta->Draw("pe,same");
    
    gPad->RedrawAxis();
    
    TLegend *lgd = new TLegend(0.2, 0.8, 0.7, 0.85);
    lgd->AddEntry(h_eta, "Stat", "pe");
    lgd->AddEntry(h_sys_eta, "Sys", "f");
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {lgd->AddEntry(h_sys_eta_ep, "E/p","f");}
    lgd->AddEntry(h_tot_eta, "Total", "f");
    lgd->SetBorderSize(0);
    lgd->SetFillStyle(0);
    lgd->SetTextFont(42);
    lgd->Draw();
    lgd->SetNColumns(3);
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {lgd->SetNColumns(4);}
    
    TString plotname;
    plotname = par3.c_str();
    plotname.Append(".png");
    plotname.Prepend("ErrorEta_");
    //theEtaErrorsCanvas->Print(plotname.Data());
    
    TCanvas *thePhiErrorsCanvas = new TCanvas("thePhiErrorsCanvas", "canvas name", 1000, 800);
    
    h_tot_phi->SetFillColor(kAzure);
    h_tot_phi->SetFillStyle(1001);
    h_tot_phi->SetStats(kFALSE);
    h_tot_phi->GetYaxis()->SetTitleOffset(1.6);
    if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(0.015); h_tot_phi->SetMinimum(-0.015); h_tot_phi->GetYaxis()->SetTitle(Form("%s [mm]", "d_{0}"));};
    if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(0.14); h_tot_phi->SetMinimum(-0.14); h_tot_phi->GetYaxis()->SetTitle(Form("%s [mm]", "z_{0}"));}
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(1.6); h_tot_phi->SetMinimum(-1.6); h_tot_phi->GetYaxis()->SetTitle(Form("%s [TeV^{-1}]", "sagitta"));}
    
    h_tot_phi->Draw("e2");
    h_sys_phi->SetFillColor(kGray);
    h_sys_phi->SetFillStyle(1001);
    h_sys_phi->Draw("e2,same");
    h_sys_phi_ep->SetFillColor(kGreen);
    h_sys_phi_ep->SetFillStyle(1001);
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {h_sys_phi_ep->Draw("e2,same");}

    h_phi->Draw("pe,same");
    
    gPad->RedrawAxis();
    
    TLegend *lgd_phi = new TLegend(0.2, 0.8, 0.7, 0.85);
    lgd_phi->AddEntry(h_phi, "Stat", "pe");
    lgd_phi->AddEntry(h_sys_phi, "Sys", "f");
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {lgd_phi->AddEntry(h_sys_phi_ep, "E/p","f");}
    lgd_phi->AddEntry(h_tot_phi, "Total", "f");
    lgd_phi->SetBorderSize(0);
    lgd_phi->SetFillStyle(0);
    lgd_phi->SetTextFont(42);
    lgd_phi->SetNColumns(3);
    if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {lgd_phi->SetNColumns(4);}
    
    lgd_phi->Draw();
    
    
    plotname = par3.c_str();
    plotname.Append(".png");
    plotname.Prepend("ErrorPhi_");
    //thePhiErrorsCanvas->Print(plotname.Data());
    
    
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {  cdd0->cd(); /*histnom->Write();*/ histerror->Write("theUncertainty_d0");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { cdz0->cd(); /*histnom->Write();*/ histerror->Write("theUncertainty_z0");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { cdsagitta->cd(); /*theHist2->Write("theNominal_sagitta");*/histerror->Write("theUncertainty_sagitta");}
      
      //theRMSCanvas --individual plot of 2D maps
      TCanvas *theRMSCanvas = new TCanvas("theRMSCanvas", "canvas name", 800, 600); //1600,1600
      theRMSCanvas->Divide(1,1);
      gStyle->SetPalette(1);
      
      theRMSCanvas->cd(1);
      double theMaximum = histnom->GetMaximum(); cout << "histnom MAXIMUM: " << histnom->GetMaximum() << endl;
      double theMinimum = histnom->GetMinimum(); cout << "histnom MINIMUM: " << histnom->GetMinimum() << endl;
      double theMaximumprof_eta = profile_eta_nom->GetMaximum();
      double theMinimumprof_eta = profile_eta_nom->GetMinimum();
      double theMaximumprof_phi = profile_phi_nom->GetMaximum();
      double theMinimumprof_phi = profile_phi_nom->GetMinimum();
      if (fabs(theMinimum) > fabs(theMaximum) ) theMaximum = fabs(theMinimum);
      histnom->SetMaximum(theMaximum);
      histnom->SetMinimum(-theMaximum);
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(0.015); histnom->SetMinimum(-0.015);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(0.2); histnom->SetMinimum(-0.2);}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(1.6); histnom->SetMinimum(-1.6);}
      
      histnom->SetStats(kFALSE);
      histnom->SetContour(99);
      //histnom->Draw("colz");
      /*if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histnom->Draw("colz");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histnom->Draw("colz");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histnom->Draw("colz");}*/

      SetPaletteColors->Draw();
      
      //theRMSCanvas->cd(2); cout << "histrms MAXIMUM: " << histrms->GetMaximum() << endl; cout << "histrms MINIMUM: " << histrms->GetMinimum() << endl;
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histrms->SetMaximum(0.00075); histrms->SetMinimum(0.);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histrms->SetMaximum(0.01); histrms->SetMinimum(0.);}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histrms->SetMaximum(0.075); histrms->SetMinimum(0.);}
      
      histrms->SetContour(99);
      //histrms->Draw("colz");

      /*if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histrms->Draw("colz");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histrms->Draw("colz");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histrms->Draw("colz");}*/

      gPad->SetTickx();
      gPad->SetTicky();
      
      //theRMSCanvas->cd(3);      
      theMaximum = histmax->GetMaximum(); cout << "histmax MAXIMUM: " << histmax->GetMaximum() << endl; cout << "histmax MINIMUM: " << histmax->GetMinimum() << endl;
      if(histrms->GetMaximum() > theMaximum) theMaximum = histrms->GetMaximum();     
      //histmax->SetMaximum(theMaximum);
      histmax->SetMinimum(0.);
      //histrms->SetMaximum(theMaximum);
      histrms->SetMinimum(0.);
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histmax->SetMaximum(0.003); histmax->SetMinimum(0.);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histmax->SetMaximum(0.04); histmax->SetMinimum(0.);}
      //if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histmax->SetMaximum(0.3); histmax->SetMinimum(0.);}
      
      histrms->SetStats(kFALSE);
      histmax->SetContour(99);
      //histmax->Draw("colz");

      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histmax->Draw("colz");}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histmax->Draw("colz");}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histmax->Draw("colz");}

      SetPaletteColors->Draw();
      
      TString filename = par3.c_str();
      filename.Append(".png");
      filename.Prepend("Delta_");
      theRMSCanvas->Print(filename.Data());
        
      //theCanvas3 -- 2D bias maps, and stat. and sys. size
      TCanvas *theRMSCanvas3 = new TCanvas("theRMSCanvas3", "canvas name", 800, 1800); 
      theRMSCanvas3->Divide(1,3);
      gStyle->SetPalette(1);

      theRMSCanvas3->cd(1);
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(0.015); histnom->SetMinimum(-0.015);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(0.2); histnom->SetMinimum(-0.2);}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { histnom->SetMaximum(1.6); histnom->SetMinimum(-1.6);}
      histnom->SetStats(kFALSE);
      histnom->SetContour(99);
      histnom->Draw("colz");
      
      theRMSCanvas3->cd(2);
      h_tot_eta->SetFillColor(kAzure);
      h_tot_eta->SetFillStyle(1001);
      h_tot_eta->SetStats(kFALSE);
      h_tot_eta->GetYaxis()->SetTitleOffset(1.6);
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(0.015); h_tot_eta->SetMinimum(-0.015); h_tot_eta->GetYaxis()->SetTitle(Form("%s [mm]", "d_{0}"));};
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(0.14); h_tot_eta->SetMinimum(-0.14); h_tot_eta->GetYaxis()->SetTitle(Form("%s [mm]", "z_{0}"));}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_tot_eta->SetMaximum(1.6); h_tot_eta->SetMinimum(-1.6); h_tot_eta->GetYaxis()->SetTitle(Form("%s [TeV^{-1}]", "sagitta"));}
      h_tot_eta->Draw("e2");
      h_sys_eta->SetFillColor(kGray);
      h_sys_eta->SetFillStyle(1001);
      h_sys_eta->Draw("e2,same");
      h_sys_eta_ep->SetFillColor(kGreen);
      h_sys_eta_ep->SetFillStyle(1001);
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {h_sys_eta_ep->Draw("e2,same");}
      
      h_eta->Draw("pe,same");
    
      gPad->RedrawAxis();
    
      lgd->Draw();
     
            
      theRMSCanvas3->cd(3);

      h_tot_phi->SetFillColor(kAzure);
      h_tot_phi->SetFillStyle(1001);
      h_tot_phi->SetStats(kFALSE);
      h_tot_phi->GetYaxis()->SetTitleOffset(1.6);
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(0.015); h_tot_phi->SetMinimum(-0.015); h_tot_phi->GetYaxis()->SetTitle(Form("%s [mm]", "d_{0}"));};
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(0.14); h_tot_phi->SetMinimum(-0.14); h_tot_phi->GetYaxis()->SetTitle(Form("%s [mm]", "z_{0}"));}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { h_tot_phi->SetMaximum(1.6); h_tot_phi->SetMinimum(-1.6); h_tot_phi->GetYaxis()->SetTitle(Form("%s [TeV^{-1}]", "sagitta"));}
    
      h_tot_phi->Draw("e2");
      h_sys_phi->SetFillColor(kGray);
      h_sys_phi->SetFillStyle(1001);
      h_sys_phi->Draw("e2,same");
      h_sys_phi_ep->SetFillColor(kGreen);
      h_sys_phi_ep->SetFillStyle(1001);
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) {h_sys_phi_ep->Draw("e2,same");}

      h_phi->Draw("pe,same");
    
      gPad->RedrawAxis();
    
      lgd_phi->Draw();
 
      TString filename3 = par3.c_str();
      filename3.Append(".png");
      filename3.Prepend("Projections_");
      theRMSCanvas3->Print(filename3.Data());

      //the RMSCanvas2 --profiles in eta and phi
      TCanvas *theRMSCanvas2 = new TCanvas("theRMSCanvas2", "canvas name", 2400, 1200); 
      theRMSCanvas2->Divide(3,2);
      theRMSCanvas2->cd(1);
      profile_eta_nom->SetMaximum(theMaximum);
      profile_eta_nom->SetMinimum(-theMaximum);
      profile_eta_nom->SetMaximum(histnom->GetMaximum());
      profile_eta_nom->SetMinimum(histnom->GetMinimum());
      
      profile_eta_nom->Draw();
      
      theRMSCanvas2->cd(2);
      profile_eta_rms->SetMinimum(0.);
      //theMaximum = profile_eta_rms->GetMaximum();
      theMaximum = profile_eta->GetMaximum();
      //theMaximum = 1.1*theMaximum;
      profile_eta_rms->SetMaximum(theMaximum);
      profile_eta_rms->SetMaximum(profile_eta->GetMaximum()); //rms
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { profile_eta_rms->SetMaximum(0.00075);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { profile_eta_rms->SetMaximum(0.01);}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { profile_eta_rms->SetMaximum(0.075);}
      profile_eta_rms->Draw();
      gPad->SetTickx();
      gPad->SetTicky();
      
      theRMSCanvas2->cd(3);
      profile_eta_rms->SetMinimum(0.);     
      //theMaximum = profile_eta_rms->GetMaximum();
      theMaximum = profile_eta->GetMaximum();
      theMaximum = 1.1*theMaximum; //1.1
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) {theMaximum = 1.6*theMaximum;} 
      profile_eta->SetMaximum(theMaximum);
      profile_eta->SetMaximum(histmax->GetMaximum());
      if(par3.find("d0CorrectionVsEtaPhi") != string::npos ) { profile_eta->SetMaximum(0.003);}
      if(par3.find("z0CorrectionVsEtaPhi") != string::npos ) { profile_eta->SetMaximum(0.04);}
      if(par3.find("LambdaCorrectionVsEtaPhi") != string::npos ) { profile_eta->SetMaximum(0.3);}
      profile_eta->SetMinimum(0.);
      profile_eta->Draw();


      theRMSCanvas2->cd(4);
      profile_phi_nom->SetMaximum(theMaximum);
      profile_phi_nom->SetMinimum(-theMaximum);
      profile_phi_nom->SetMaximum(histnom->GetMaximum());
      profile_phi_nom->SetMinimum(histnom->GetMinimum());

      profile_phi_nom->Draw();
      gPad->SetTickx();
      gPad->SetTicky();


      theRMSCanvas2->cd(5);
      profile_phi_rms->SetMinimum(0.);
      profile_phi_rms->SetMaximum(theMaximum);
      profile_phi_rms->SetMaximum(profile_eta_rms->GetMaximum()); //rms
      profile_phi_rms->Draw();
      gPad->SetTickx();
      gPad->SetTicky();

             
      theRMSCanvas2->cd(6);
      profile_phi->SetMaximum(theMaximum);
      profile_phi->SetMaximum(profile_eta->GetMaximum());
      profile_phi->SetMinimum(0.);
      profile_phi->Draw();
      gPad->SetTickx();
      gPad->SetTicky();

      TString filename2 = par3.c_str();
      filename2.Append(".png");
      filename2.Prepend("Profiles_");
      theRMSCanvas2->Print(filename2.Data());
      
  }
  
  fcorr->Close();           
  
  
}

void CreateColors(){
  const Int_t NRGBs = 3;
  const Int_t NCont = 50;
  Double_t stops[NRGBs] = {0.33, 0.66, 1.00 };
  Double_t red[NRGBs] = { 0.10, 1.00, 1.00 };
  Double_t green[NRGBs] = {0.90, 1.00, 0.05 };
  Double_t blue[NRGBs] = {0.10, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue,NCont);
  gStyle->SetNumberContours(NCont); 
  
  return;
}
