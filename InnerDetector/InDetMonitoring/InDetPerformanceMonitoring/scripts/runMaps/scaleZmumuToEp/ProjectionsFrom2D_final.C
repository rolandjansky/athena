{
 // gStyle->SetOptStat(0);
 gROOT->Reset();
 gROOT->LoadMacro("./MakeContoursPretty.C");

 gROOT.LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasStyle.C");
 gROOT.LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasUtils.C");
 gROOT.LoadMacro("/Users/shenkel/scripts/atlasstyle-00-03-05/AtlasLabels.C");
 
 
 
 SetAtlasStyle();
 set_plot_style();
 gStyle->SetOptTitle(0);
 gStyle->SetOptStat(0);
 
 bool fDebug = kFALSE;
 bool ep = false;

 bool writeToFile = true;


 // Get maps
 //////////////////////////////////////////////////////////////////////////////////////////////
 
 std::vector<std::string> list;

 if(!ep){
       std::string file1 = "correctionmaps_data_2016_AC.root"; std::string output = "data_2016_AC_40x40";
       list.push_back("d0CorrectionVsEtaPhi");
       list.push_back("z0CorrectionVsEtaPhi");
       list.push_back("LambdaCorrectionVsEtaPhi");
       //	 list.push_back("LambdaCorrectionVsEtaPhi_reweightedToEP");
   
 }else{
   std::string file1 = "Scaling_All.root"; std::string output = "output_All2015_25ns_runs_Epmaps";
   //std::string file1 = "Scaling_D-F.root"; std::string output = "output_StablePeriodD-F_Epmaps";
   //   std::string file1 = "Scaling_G-J.root"; std::string output = "output_UnstablePeriodG-J_Epmaps";
   list.push_back("FinalCorrections");
 }
 int width = 800;
 int height = 500;
 
 TCanvas*c  = new TCanvas("c", "canvas", width, height);
 // c->Divide(2,1);
 TCanvas*c2  = new TCanvas("c2", "canvas2", width, height);
 // c->Divide(2,1);
 TCanvas*c3  = new TCanvas("c3", "canvas3", width, height);
 TCanvas*c4  = new TCanvas("c4", "canvas4", width, height);
 
 TCanvas*c_map  = new TCanvas("c_map", "canvas", width, height);

 TCanvas*c5  = new TCanvas("c5", "canvas5", width+200, height+500);
 c5->Divide(3,3,0.0001,0.0001);

 
 
 int counter = 0;
 for( std::vector<std::string>::const_iterator it = list.begin(); it != list.end(); ++it ) {
   ++counter;
   std::cout << "Current : "<<*it << std::endl;
   
   std::string par1 = *it;
   
   TFile *fin1 = new TFile(file1.c_str());
   TH2* hist = (TH2*)fin1.Get(par1.c_str());
   
   
   //Plot maps
   /////////////////////////////////////////////////////////////////////////////////////////////
      
   int fnEtaBins = hist->GetXaxis()->GetNbins();
   int fnPhiBins = hist->GetYaxis()->GetNbins();
   double eta_bound = 2.5;
   double phi_bound = 3.1415;
   int num_bins = 1;
   float temp_val_mu = 0;
   float temp_val_sig = 0;
   TGaxis::SetMaxDigits(4);
   std::string histname = hist->GetName();
   cout <<"histname : "<< histname <<  endl;
   

   if(histname.find("d0CorrectionVsEtaPhi") != string::npos ){
     hist->Scale(1000.);hist->GetZaxis()->SetTitle("#delta_{d_{0}} [#mum]");
   }

   if(histname.find("FinalCorrections") != string::npos ){
     hist->Scale(1000.);hist->GetZaxis()->SetTitle("#delta_{sagitta} [TeV^{-1}] (E/p)");
     //     fnEtaBins = 16;
   }

   TH1D* graph1  =	new TH1D(Form("%s_proj_eta_mean",hist->GetName()),Form(";#eta; Mean %s",hist->GetZaxis()->GetTitle()), fnEtaBins,-eta_bound,eta_bound);
   TH1D* graph2  =	new TH1D(Form("%s_proj_eta_RMS",hist->GetName()),Form(";#eta; RMS %s",hist->GetZaxis()->GetTitle()), fnEtaBins,-eta_bound,eta_bound);   

   TH1D* graph3  =	new TH1D(Form("%s_proj_phi_mean",hist->GetName()),Form(";#phi; Mean %s",hist->GetZaxis()->GetTitle()), fnPhiBins,-phi_bound,phi_bound);
   TH1D* graph4  =	new TH1D(Form("%s_proj_phi_RMS",hist->GetName()),Form(";#phi; RMS %s",hist->GetZaxis()->GetTitle()), fnPhiBins,-phi_bound,phi_bound);   

   
   TPaletteAxis *palette = (TPaletteAxis*)hist->GetListOfFunctions()->FindObject("palette");
   if(histname.find("d0CorrectionVsEtaPhi") != string::npos ) TH1D* bin =  new TH1D("bin","bin",100000000,-500,500);//hist->GetZaxis()->GetZmin(),hist->GetZaxis()->GetXmax());
   if(histname.find("z0CorrectionVsEtaPhi") != string::npos ) TH1D* bin =  new TH1D("bin","bin",10000000,-.5,.5);//hist->GetZaxis()->GetZmin(),hist->GetZaxis()->GetXmax());
   if(histname.find("LambdaCorrectionVsEtaPhi") != string::npos ) TH1D* bin =  new TH1D("bin","bin",10000000,-3.5,3.5);//hist->GetZaxis()->GetZmin(),hist->GetZaxis()->GetXmax());
   if(histname.find("FinalCorrections") != string::npos )  TH1D* bin =  new TH1D("bin","bin",10000000,-3.5,3.5);//hist->GetZaxis()->GetZmin(),hist->GetZaxis()->GetXmax());
   
   
   
   for (int i = 1; i <=hist->GetNbinsX(); ++i) {
     double value_x = (hist->GetXaxis()->GetBinLowEdge(i) + hist->GetXaxis()->GetBinUpEdge(i+num_bins-1))/2;
     for(int j=1 ;  j <= hist->GetNbinsY(); ++j){
       bin->Fill(hist->GetBinContent(i,j));
     }
     cout << ">>> mean : "<< bin->GetMean() << "  >>> RMS : "<< bin->GetRMS() << endl;
     graph1->SetBinContent(i,bin->GetMean());
     graph1->SetBinError(i,bin->GetMeanError());
     graph2->SetBinContent(i,bin->GetRMS());
     graph2->SetBinError(i,bin->GetRMSError());     
      // if(i ==3 ){
      //    bin->Draw();
      //    cout << " output check";
      //    getline(cin, input);
      //    }
     bin->Reset();
     std::cout << bin->GetEntries() << std::endl;
   }

   for (int i = 1; i <=hist->GetNbinsX(); ++i) {
     double value_y = (hist->GetYaxis()->GetBinLowEdge(i) + hist->GetYaxis()->GetBinUpEdge(i+num_bins-1))/2;
     for(int j=1 ;  j <= hist->GetNbinsY(); ++j){
       bin->Fill(hist->GetBinContent(j,i));
     }
     cout << ">>> mean : "<< bin->GetMean() << "  >>> RMS : "<< bin->GetRMS() << endl;
     graph3->SetBinContent(i,bin->GetMean());
     graph3->SetBinError(i,bin->GetMeanError());
     graph4->SetBinContent(i,bin->GetRMS());
     graph4->SetBinError(i,bin->GetRMSError());     
      // if(i ==3 ){
      //    bin->Draw();
      //    cout << " output check";
      //    getline(cin, input);
      //    }
     bin->Reset();
     std::cout << bin->GetEntries() << std::endl;
   }
   

   if(histname.find("d0CorrectionVsEtaPhi") != string::npos ){
     hist->GetZaxis()->SetRangeUser(-10.,10.0);
     hist->GetYaxis()->SetTitleOffset(0.7);
     hist->GetYaxis()->SetTitle("#phi [rad]");
     graph1->GetYaxis()->SetRangeUser(-12.,12.);
     graph2->GetYaxis()->SetRangeUser(0.,8.);
     graph3->GetYaxis()->SetRangeUser(-12.,12.);
     graph4->GetYaxis()->SetRangeUser(0.,8.);

   }
   if(histname.find("LambdaCorrectionVsEtaPhi") != string::npos ){
     hist->GetZaxis()->SetRangeUser(-2.2,2.2);
     hist->GetYaxis()->SetTitleOffset(0.7);
     hist->GetYaxis()->SetTitle("#phi [rad]");
     graph1->GetYaxis()->SetRangeUser(-1.,1.);
     graph2->GetYaxis()->SetRangeUser(0.,1.5);
     graph3->GetYaxis()->SetRangeUser(-1.,1.);
     graph4->GetYaxis()->SetRangeUser(0.,1.5);
   }
   
   if(histname.find("z0CorrectionVsEtaPhi") != string::npos ){
     hist->GetZaxis()->SetRangeUser(-.26,.26);
     hist->GetYaxis()->SetTitleOffset(0.7);
     hist->GetYaxis()->SetTitle("#phi [rad]");
     graph1->GetYaxis()->SetRangeUser(-.02,.12);
     graph2->GetYaxis()->SetRangeUser(.0,.12);
     graph3->GetYaxis()->SetRangeUser(-.02,.12);
     graph4->GetYaxis()->SetRangeUser(0.,.12);

   }
   if(histname.find("FinalCorrections") != string::npos ){
     hist->GetZaxis()->SetRangeUser(-2.2,2.2);
     hist->GetYaxis()->SetTitleOffset(0.7);
     hist->GetYaxis()->SetTitle("#phi [rad]");
     graph1->GetYaxis()->SetRangeUser(-1.,1.);
     graph2->GetYaxis()->SetRangeUser(0.,1.5);
   }

   // TString ext = "pdf";
   c_map->cd();
   hist->GetZaxis()->SetTitleOffset(0.68);
   hist->Draw("colz");
   ATLASLabel(0.52,0.28,"Internal");
   myText(0.52,0.23,kBlack,"Data 2016, #sqrt{s} = 13 TeV");
   c_map->Update();
   c_map->SaveAs(Form("%s/Maps_%s.pdf",output.c_str(),hist->GetName()));
   
   
   
   c->cd();
   graph1->Draw();

   c3->cd();
   graph3->Draw();
   
   ATLASLabel(0.25,0.88,"Internal");
   myText(0.25,0.83,kBlack,"Data 2016, #sqrt{s} = 13 TeV");
   c->SaveAs(Form("%s/Proj_%s_eta_mean.pdf",output.c_str(),hist->GetName()));
   c3->SaveAs(Form("%s/Proj_%s_phi_mean.pdf",output.c_str(),hist->GetName()));
   
   c2->cd();
   graph2->Draw();
   ATLASLabel(0.25,0.88,"Internal");
   myText(0.25,0.83,kBlack,"Data 2016, #sqrt{s} = 13 TeV");

   c4->cd();
   graph4->Draw();

   c2->SaveAs(Form("%s/Proj_%s_eta_RMS.pdf",output.c_str(),hist->GetName()));
   c4->SaveAs(Form("%s/Proj_%s_phi_RMS.pdf",output.c_str(),hist->GetName()));


   //Draw common canvas
   c5->cd(1); hist->Draw("colz");
   c5->cd(2); graph3->Draw();
   c5->cd(3); graph4->Draw();
   c5->cd(4); graph1->Draw();
   c5->cd(7); graph2->Draw();
 

   c5->SaveAs(Form("%s/Comp_%s_mean.pdf",output.c_str(),hist->GetName()));
 if(writeToFile){
   fout = TFile::Open(Form("%s/Projections.root",output.c_str()),"UPDATE");
   graph1->Write();
   graph2->Write();
   graph3->Write();
   graph4->Write();
   fout->Delete("LambdaCorrectionVsEtaPhi;1");
   cout << "Updated file : "<< output.c_str() << endl;
   fout->Close();
 }
 
 


   graph1->Reset();
   graph2->Reset();
   bin->Reset();
 
   // delete hist;
 }//end for loop
}
