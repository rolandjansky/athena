{
  // gStyle->SetOptStat(0);
 gROOT->Reset();
 gROOT->LoadMacro("./MakeContoursPretty.C");
 set_plot_style();
 std::string path = "/Users/shenkel/scripts/atlasstyle-00-03-05/";

 gROOT->LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasStyle.C");
 gROOT->LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasUtils.C");
 gROOT->LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasLabels.C");
 
 bool fDebug = false;
 bool writeToFile = false; 
 // Get maps
 TCanvas c2;
 c2->Divide(2,2);
 //////////////////////////////////////////////////////////////////////////////////////////////
 std::string file1 = "correctionmaps_HighGran_IBLon_NoGRL_INDET_2015_datareproAll25ns_correctedEp.root"; std::string par1 = "LambdaCorrectionVsEtaPhi";
 std::string file2 = "Scaling_All.root"; std::string par2 = "FinalCorrections";
 std::string output = "correctionmaps_HighGran_IBLon_NoGRL_INDET_2015_datareproAll25ns_correctedEp.root";
 
 TFile *fin1 = new TFile(file1.c_str());
 TFile *fin2 = new TFile(file2.c_str());
 
 hzmumu = (TH2D*)fin1.Get(par1.c_str());
  hep = (TH2D*)fin2.Get(par2.c_str());
 // hep->Scale(1000); //From GeV to TeV
 
 

 //Plot maps
 /////////////////////////////////////////////////////////////////////////////////////////////
 TCanvas c;
 c->Divide(3,3);
 c->cd(1);
 hzmumu->Draw("colz");
 c2->cd(1);
 hzmumu->Draw("colz");
 c->cd(4);
 hep->Draw("colz");
 
 // Calculate means for the 2D map
 //////////////////////////////////////////////////////////////////////////////
 std::cout << "y bins : "<<  hzmumu->GetYaxis()->GetNbins()<<  std::endl;
 std::cout << "x bins : "<<  hzmumu->GetXaxis()->GetNbins()<<  std::endl;
 //  cout << "<Mean> "<< hzmumu->GetName() << " in  <#mu> : " <<   (hzmumu->GetMean(1)+hzmumu->GetMean(2))/2 << endl;

  
 TH1D* bin =  new TH1D("bin","bin",1000000,hzmumu->GetYaxis()->GetXmin(),hzmumu->GetYaxis()->GetXmax());
 float sum1 = 0;
 hzmumu->Sumw2(); 
 for(int x = 1; x <=  hzmumu->GetXaxis()->GetNbins(); x++){
   for(int y = 1; y <=  hzmumu->GetYaxis()->GetNbins(); y++){
     bin->Fill(hzmumu->GetBinContent(x,y));
     sum1+=hzmumu->GetBinContent(x,y);
     }
 }
 float avg_zmumu  = bin->GetMean(); //sum1/(hzmumu->GetXaxis()->GetNbins() * hzmumu->GetYaxis()->GetNbins());//bin->GetMean();
 cout << "Mean of Zmumu  : " << sum1/(hzmumu->GetXaxis()->GetNbins() * hzmumu->GetYaxis()->GetNbins()) << endl;
 cout << "<mu> zmumu (taken to calculate) : " << bin->GetMean()<< endl;
 c2->cd(2);bin->Draw();
 bin->Reset();
 std::cout << bin->GetEntries() << std::endl;

 
 // Extract mean from E/P
 //////////////////////////////////////////////////////////////////////////////
 float sum2 = 0;
 hep->Sumw2();
 TH1D* bin2 =  new TH1D("bin2","bin2",1000000,hep->GetYaxis()->GetXmin(),hep->GetYaxis()->GetXmax());
 for(int x = 1; x <=  hep->GetXaxis()->GetNbins(); x++){
   for(int y = 1; y <=  hep->GetYaxis()->GetNbins(); y++){
     bin2->Fill(hep->GetBinContent(x,y));
     sum2 +=hep->GetBinContent(x,y);
   }
 }
 float avg_ep = bin2->GetMean();//sum2/(hep->GetXaxis()->GetNbins() * hep->GetYaxis()->GetNbins());//bin2->GetMean();
 cout << "Mean in EP  : " <<sum2/(hep->GetXaxis()->GetNbins() * hep->GetYaxis()->GetNbins())<< endl;
 cout << "<mu> ep  (taken to calculate)  : " << bin2->GetMean()<< endl;
 //bin2->Reset();
 std::cout << bin2->GetEntries() << std::endl;
 
 

 
 //Add difference
 //////////////////////////////////////////////////////////////////////////////
 hzmumu_new = (TH2F*)hzmumu->Clone();
 hzmumu_new->Sumw2();
 float avgdiff = avg_ep-avg_zmumu;
 cout << "AVG difference: " <<avgdiff << endl;
 for(int i = 1; i <=hzmumu_new->GetSize() ; ++i){
   hzmumu_new->AddBinContent(i, avgdiff);
 }
 hzmumu_new->SetName("LambdaCorrectionVsEtaPhi_reweightedToEP");
 cout << "NAME : "<< hzmumu_new->GetName() << endl;


 // Write new map to file
 /////////////////////////
 if(writeToFile){
   fout = TFile::Open(Form("%s",output.c_str()),"UPDATE");
   hzmumu_new->Write();
   fout->Delete("LambdaCorrectionVsEtaPhi;1");
   cout << "Updated file : "<< output.c_str() << endl;
   fout->Close();
 }
 
 
//X check
 //////////////////////////////////////////////////////////////////////////////
 float sum = 0;
 for(int x = 1; x <=  hzmumu_new->GetXaxis()->GetNbins(); x++){
   for(int y = 1; y <=  hzmumu_new->GetYaxis()->GetNbins(); y++){
     if(fDebug){  cout <<  "#ZMUMU_NEW## Bin content in bin (" <<x << "," << y<<") : "
	  << hzmumu_new->GetBinContent(x,y)
	  << " before : "<< hzmumu->GetBinContent(x,y)
	  << " Difference : "<<hzmumu_new->GetBinContent(x,y)-hzmumu->GetBinContent(x,y)
	  << " avg : "<< avgdiff <<endl;
     }
     bin2->Fill(hzmumu_new->GetBinContent(x,y));
           sum +=hzmumu_new->GetBinContent(x,y);
   }
 }
 float avg_zmumu_new  = sum/(hzmumu_new->GetXaxis()->GetNbins() * hzmumu_new->GetYaxis()->GetNbins());
 cout << "Zmumu_New : "<< avg_zmumu_new <<  endl;
 cout << "Zmumu_New -> <#mu> : "<< bin2->GetMean() <<  endl;
 c2->cd(4);bin2->Draw();
 bin2->Reset();


 

 //Plot projections
 //////////////////////////////////////////////////////////////////////////////
 c.cd(2);
 projY_zmumu = (TH1D*) hzmumu.ProjectionY("eoh1projY_h1_zmumu");
 projY_zmumu->Draw();
 
 c.cd(3);
 projX_zmumu = (TH1D*) hzmumu.ProjectionX("eoh1projX_h1_zmumu");
 projX_zmumu->Draw();

 c.cd(5);
 projY_hep = (TH1D*) hep.ProjectionY("eoh1projY_h1_hep");
 projY_hep->Draw();
 
 c.cd(6);
 projX_hep = (TH1D*) hep.ProjectionX("eoh1projX_h1_hep");
 projX_hep->Draw();

 c.cd(7);
 hzmumu_new->Draw("colz");

 c.cd(8);
 projY_zmumu_new = (TH1D*) hzmumu_new.ProjectionY("eoh1projY_h1_zmumu");
 projY_zmumu_new->Draw();
 
 c.cd(9);
 projX_zmumu_new = (TH1D*) hzmumu_new.ProjectionX("eoh1projX_h1_zmumu");
 projX_zmumu_new->Draw();
 // Plot new map
 ///////////////////////////////////////////////////////////////////////////////

 c2.cd(3);
 hzmumu_new->Draw("colz");
 
 

}


}//end for loop

