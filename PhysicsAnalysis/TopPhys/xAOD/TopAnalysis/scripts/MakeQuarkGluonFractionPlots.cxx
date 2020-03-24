//  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
//  ===============================================================================
//   Macro to compute the gluon initiated jets fraction in your sample/selection
//   To run the macro: .x MakeQuarkGluonFractionPlots.cxx+("config.txt")
//  ===============================================================================
#include "MakeQuarkGluonFractionPlots.h"

MakeQuarkGluonFractionPlots::MakeQuarkGluonFractionPlots(std::string filename): 
  m_doGluVsFlavour("lightC"),
  m_channel({}),
  m_detailedConfig(false),
  m_OutputFile("FlavourComposition"),
  m_doPS(false){

  //Suppressing info/error/... ROOT messages
  gErrorIgnoreLevel = 5000;
  TH1::AddDirectory(kFALSE);
  gStyle->SetPaintTextFormat(".3f");
  gROOT->SetBatch(kTRUE);
  printf("\n %s\n MakeQuarkGluonFractionPlots::MakeQuarkGluonFractionPlots\n %s\n",std::string(75,'=').c_str(),std::string(75,'=').c_str());
  //----------------------------
  // reading configuration file
  //----------------------------
  readXMLConfigFile(filename);
  printXMLConfigFile();
  //-----------------------------------------------------------------------------------------------
  // plotting the 2D input histograms in a ps file and storing them in a vector and
  // compute the gluon fraction for nominal and systematic samples
  //-----------------------------------------------------------------------------------------------
  std::string psfilename;
  TCanvas c1;
  //Maps to store histograms
  std::map<std::string, TH2D*> h_input;
  std::vector< std::map<std::string, TH2D*> > h_input_1P;
  std::vector< std::map<std::string, TH2D*> > h_input_1PVar;
  std::vector< std::map<std::string, TH2D*> > h_input_2P;
  std::vector< std::map<std::string, TH2D*> > h_input_2PUp;
  std::vector< std::map<std::string, TH2D*> > h_input_2PDown;
  printf("\n %s\n Retrieving histograms\n %s\n",std::string(75,'%').c_str(),std::string(75,'%').c_str());
  if(m_mergechannels){
    psfilename = "PlotsGluonFraction_AllChannels.ps";
    if(m_doPS) c1.Print((psfilename+"[").c_str());
    bool createMap = true;
    for(unsigned int c=0; c<m_channel.size(); ++c){
      std::string append = "nominal_";
      DumpToMap(h_input, m_configNominal.path, m_channel.at(c), m_configNominal.folder, append, createMap);
      createMap = false;
    }
    if(m_doPS) drawhistos(h_input, psfilename);
    for(unsigned int v=0; v<m_config1PointVec.size(); ++v){
      createMap = true;
      std::map<std::string, TH2D*> map_temp;
      for(unsigned int c=0; c<m_channel.size(); ++c){
	std::string append = "central1P_"+std::to_string(v)+"_";
	DumpToMap(map_temp, m_config1PointVec.at(v).central, m_channel.at(c), m_config1PointVec.at(v).folder_c, append, createMap);
	createMap = false;
      }
      h_input_1P.push_back(map_temp);
      if(m_doPS) drawhistos(h_input_1P.at(v), psfilename);
    }
    for(unsigned int v=0; v<m_config1PointVec.size(); ++v){
      createMap = true;
      std::map<std::string, TH2D*> map_temp;
      for(unsigned int c=0; c<m_channel.size(); ++c){
	std::string append = "variation1P_"+std::to_string(v)+"_";
	DumpToMap(map_temp, m_config1PointVec.at(v).variation, m_channel.at(c), m_config1PointVec.at(v).folder_v, append, createMap);
	createMap = false;
      }
      h_input_1PVar.push_back(map_temp);
      if(m_doPS) drawhistos(h_input_1PVar.at(v), psfilename);
    }
    for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
      createMap = true;
      std::map<std::string, TH2D*> map_temp;
      for(unsigned int c=0; c<m_channel.size(); ++c){
	std::string append = "central2P_"+std::to_string(v)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).central, m_channel.at(c), m_config2PointVec.at(v).folder_c, append, createMap);
	createMap = false;
      }
      h_input_2P.push_back(map_temp);
      if(m_doPS) drawhistos(h_input_2P.at(v), psfilename);
    }
    for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
      createMap = true;
      std::map<std::string, TH2D*> map_temp;
      for(unsigned int c=0; c<m_channel.size(); ++c){
	std::string append = "variation2PUp_"+std::to_string(v)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).variation_up, m_channel.at(c), m_config2PointVec.at(v).folder_up, append, createMap);
	createMap = false;
      }
      h_input_2PUp.push_back(map_temp);
      if(m_doPS) drawhistos(h_input_2PUp.at(v), psfilename);
    }
    for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
      createMap = true;
      std::map<std::string, TH2D*> map_temp;
      for(unsigned int c=0; c<m_channel.size(); ++c){
	std::string append = "variation2PDown_"+std::to_string(v)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).variation_down, m_channel.at(c), m_config2PointVec.at(v).folder_down, append, createMap);
	createMap = false;
      }
      h_input_2PDown.push_back(map_temp);
      if(m_doPS) drawhistos(h_input_2PDown.at(v), psfilename);
    }
    CreateQGFFile("AllChannels",h_input,h_input_1P,h_input_1PVar,h_input_2P,h_input_2PUp,h_input_2PDown);
  }
  else{
    for(unsigned int c=0; c<m_channel.size(); ++c){
      psfilename = "PlotsGluonFraction_" + m_channel.at(c) +".ps";
      if(m_doPS) c1.Print((psfilename+"[").c_str());
      bool createMap = true;
      std::string append = "nominal_";
      
      //Clear the input map 
      h_input.clear();
      h_input_1P.clear();
      h_input_1PVar.clear();
      h_input_2P.clear();
      h_input_2PUp.clear();
      h_input_2PDown.clear();
      
      DumpToMap(h_input, m_configNominal.path, m_channel.at(c), m_configNominal.folder, append, createMap);
      if(m_doPS) drawhistos(h_input, psfilename);
      
      for(unsigned int v=0; v<m_config1PointVec.size(); ++v){
	std::map<std::string, TH2D*> map_temp;
	std::string append = "central1P_"+std::to_string(v)+"_"+m_channel.at(c)+"_";
	DumpToMap(map_temp, m_config1PointVec.at(v).central, m_channel.at(c), m_config1PointVec.at(v).folder_c, append, createMap);
	h_input_1P.push_back(map_temp);
	if(m_doPS) drawhistos(h_input_1P.at(v), psfilename);
      }
      for(unsigned int v=0; v<m_config1PointVec.size(); ++v){
	std::map<std::string, TH2D*> map_temp;
	std::string append = "variation1P_"+std::to_string(v)+"_"+m_channel.at(c)+"_";
	DumpToMap(map_temp, m_config1PointVec.at(v).variation, m_channel.at(c), m_config1PointVec.at(v).folder_v, append, createMap);
	h_input_1PVar.push_back(map_temp);
	if(m_doPS) drawhistos(h_input_1PVar.at(v), psfilename);
      }
      for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
	std::map<std::string, TH2D*> map_temp;
	std::string append = "central2P_"+std::to_string(v)+"_"+m_channel.at(c)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).central, m_channel.at(c), m_config2PointVec.at(v).folder_c, append, createMap);
	h_input_2P.push_back(map_temp);
	if(m_doPS) drawhistos(h_input_2P.at(v), psfilename);
      }
      for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
	std::map<std::string, TH2D*> map_temp;
	std::string append = "variation2PUp_"+std::to_string(v)+"_"+m_channel.at(c)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).variation_up, m_channel.at(c), m_config2PointVec.at(v).folder_up, append, createMap);
	h_input_2PUp.push_back(map_temp);
	if(m_doPS) drawhistos(h_input_2PUp.at(v), psfilename);
      }
      for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
	std::map<std::string, TH2D*> map_temp;
	std::string append = "variation2PDown_"+std::to_string(v)+"_"+m_channel.at(c)+"_";
	DumpToMap(map_temp, m_config2PointVec.at(v).variation_down, m_channel.at(c), m_config2PointVec.at(v).folder_down, append, createMap);
	h_input_2PDown.push_back(map_temp);
	if(m_doPS) drawhistos(h_input_2PDown.at(v), psfilename);
      }
      CreateQGFFile(m_channel.at(c),h_input,h_input_1P,h_input_1PVar,h_input_2P,h_input_2PUp,h_input_2PDown);
    }
  }
}
/*********************************************
 * Drawing methods
 * *******************************************/
// Draw histograms from a map
void MakeQuarkGluonFractionPlots::drawhistos(std::map<std::string, TH2D*> histos, std::string psfilename){
  TCanvas c1;
  c1.SetLogx();
  for (std::map<std::string, TH2D*>::iterator it = histos.begin() ; it != histos.end(); ++it){
    TH2D *h = it->second;
    if(h->Integral()==0.) continue;
    h->Draw("COLZtext");
    c1.Print(psfilename.c_str());
    
  }
}
// Draw histograms from a vector
void MakeQuarkGluonFractionPlots::drawhistos(std::vector<TH2D*> histos, std::string psfilename){
  TCanvas c1;
  c1.SetLogx();
  for (std::vector<TH2D*>::iterator it = histos.begin() ; it != histos.end(); ++it){
    TH2D *h = *it;
    if(h->Integral()==0.) continue;
    h->Draw("COLZtext");
    c1.Print(psfilename.c_str());
    
  }
}
//Write histograms to file (nominal and uncertainties, if presents)
void MakeQuarkGluonFractionPlots::createOutputFile(std::string filename, std::vector<TH2D*> histos, std::vector<TH2D*> histosUnc){
  TFile *fout   = new TFile(filename.c_str(),"recreate");
  fout->cd();
  for (std::vector<TH2D*>::iterator it = histos.begin() ; it != histos.end(); ++it) (*it)->Write();
  for (std::vector<TH2D*>::iterator it = histosUnc.begin() ; it != histosUnc.end(); ++it) { if((*it)!= NULL)  (*it)->Write();}
  fout->Close();
  std::cout<<" Histograms are saved in "<<filename<<std::endl;
}
/**************************************************************
 * QGF Evaluation - related methods
 * ************************************************************/
void MakeQuarkGluonFractionPlots::DumpToMap(std::map<std::string, TH2D*> &h_map, std::string filename, std::string channel, std::string folder, std::string keyname, bool createMap){
  bool newMap = createMap;
  bool istxt = ( (filename.find(".txt") + 4 ) == filename.length());
  if(istxt){
    std::string f_name;
    ifstream reader;
    reader.open (filename);
    while(!reader.eof()){
      getline(reader,f_name);
      if(f_name.compare("")==0) continue;
      DumpFileToMap(h_map, f_name, channel, folder, keyname, newMap);
      newMap = false;
    }
    reader.close();
  }
  else
    DumpFileToMap(h_map, filename, channel, folder, keyname, newMap);
}
void MakeQuarkGluonFractionPlots::DumpFileToMap(std::map<std::string, TH2D*> &h_map, std::string filename, std::string channel, std::string folder, std::string keyname, bool createMap){
  TFile *f_in=TFile::Open(filename.c_str());
  std::string dirname = channel + "/"+folder;
  TDirectory* dir = f_in->GetDirectory(dirname.c_str());
  if(!dir) {
    std::cout<<" ERROR: directory "<<dirname<<" not found in file "<<filename<<"! Exiting.\n";
    abort();
  }
  TIter next(dir->GetListOfKeys());
  TKey *key;
  int counter = 0;
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH2D")) continue;
    TH2D *h = (TH2D*)key->ReadObj();
    if(createMap){
      std::string histoname = keyname + std::to_string(counter);
      ++counter;
      std::string orig_name = h->GetName();
      h->SetName(histoname.c_str());
      std::string titlename = keyname + "_" + orig_name;
      h->SetTitle(titlename.c_str());
      h->SetStats(kTRUE); // Activate or desactivate the statistics box            
      gStyle->SetStatY(0.95); // Set y-position (fraction of pad size)            
      gStyle->SetStatX(0.89); // Set x-position (fraction of pad size)            
      h_map.insert( std::pair<std::string, TH2D* >( orig_name, h ) );
    }
    else{
      h_map.at(h->GetName())->Add(h);
    }
  }
}
//QGF File creation
void MakeQuarkGluonFractionPlots::CreateQGFFile(std::string prename, std::map<std::string, TH2D*> h_input,std::vector< std::map<std::string, TH2D*> > h_input_1P,std::vector< std::map<std::string, TH2D*> > h_input_1PVar,std::vector< std::map<std::string, TH2D*> > h_input_2P,std::vector< std::map<std::string, TH2D*> > h_input_2PUp,std::vector< std::map<std::string, TH2D*> > h_input_2PDown){
  //Since I am here, all histograms have already been retrieved!
  printf("\n %s\n Computing the gluon fraction in %s channel\n %s\n",std::string(75,'%').c_str(),prename.c_str(),std::string(75,'%').c_str());
  std::string outputName = prename + "_" + m_OutputFile;  
  //Creating ps file to plot all flavour plots/channel
  std::string psfilename = "PlotsGluonFraction_" + prename +".ps";
  TCanvas c1;
  if(m_doPS) c1.Print((psfilename+"[").c_str());
  //Nominal
  printf("\n %s\n Computing QGF (nominal): \n %s\n",std::string(75,'=').c_str(),std::string(75,'=').c_str());
  std::vector<TH2D*> fractionhistos=computeQuarkGluonFraction(h_input, m_doGluVsFlavour, prename);
  if(m_doPS) drawhistos(fractionhistos, psfilename);
  //Systematics:  
  std::vector<std::vector<TH2D*>> h_fractionUnc1PSyst;
  std::vector<std::vector<TH2D*>> h_fractionUnc2PSyst_up;
  std::vector<std::vector<TH2D*>> h_fractionUnc2PSyst_down;
  printf("\n %s\n Computing QGF (%s): \n %s\n",std::string(75,'%').c_str(),"1 point systematics",std::string(75,'%').c_str());
  for(unsigned int v=0; v<m_config1PointVec.size(); ++v){
    std::string prehistname="central_1PSystematic" + std::to_string(v) ;
    std::string prehistnameVar="variation_1PSystematic" + std::to_string(v) ;//std::to_string(v) + "variation_1PSystematic";
    printf("\n %s\n Computing QGF (%s): \n %s\n",std::string(75,'=').c_str(),m_config1PointVec.at(v).name.c_str(),std::string(75,'=').c_str());
    std::vector<TH2D*> qgf_nominal = computeQuarkGluonFraction(h_input_1P.at(v), m_doGluVsFlavour, prehistname);
    if(m_doPS) drawhistos(qgf_nominal, psfilename);
    std::vector<TH2D*> qgf_variation = computeQuarkGluonFraction(h_input_1PVar.at(v), m_doGluVsFlavour, prehistnameVar);
    if(m_doPS) drawhistos(qgf_variation, psfilename);
    prehistname=std::to_string(v) + "_1PSystematic_delta";
    h_fractionUnc1PSyst.push_back(getDelta(qgf_nominal, qgf_variation, m_config1PointVec.at(v).scalefactor, prehistname)); //nominal-var
    if(m_doPS) drawhistos(h_fractionUnc1PSyst.at(v), psfilename);
  }
  printf("\n %s\n Computing QGF (%s): \n %s\n",std::string(75,'%').c_str(),"2 point systematics",std::string(75,'%').c_str());
  for(unsigned int v=0; v<m_config2PointVec.size(); ++v){
    std::string prehistname=std::to_string(v) + "_2PSystematic";
    printf("\n %s\n Computing QGF (%s): \n %s\n",std::string(75,'=').c_str(),m_config2PointVec.at(v).name.c_str(),std::string(75,'=').c_str());
    std::vector<TH2D*> qgf_nominal = computeQuarkGluonFraction(h_input_2P.at(v), m_doGluVsFlavour, prehistname);
    if(m_doPS) drawhistos(qgf_nominal, psfilename);
    std::vector<TH2D*> qgf_variationUp = computeQuarkGluonFraction(h_input_2PUp.at(v), m_doGluVsFlavour, prehistname);
    if(m_doPS) drawhistos(qgf_variationUp, psfilename);
    prehistname=std::to_string(v) + "_2PSystematic_deltaUp";
    h_fractionUnc2PSyst_up.push_back(getDelta(qgf_variationUp, qgf_nominal, m_config2PointVec.at(v).scalefactor, prehistname)); //up-nominal
    if(m_doPS) drawhistos(h_fractionUnc2PSyst_up.at(v), psfilename);
    
    std::vector<TH2D*> qgf_variationDown = computeQuarkGluonFraction(h_input_2PDown.at(v), m_doGluVsFlavour, prehistname);
    if(m_doPS) drawhistos(qgf_variationDown, psfilename);
    prehistname=std::to_string(v) + "_2PSystematic_deltaDown";
    h_fractionUnc2PSyst_down.push_back(getDelta(qgf_nominal, qgf_variationDown, m_config2PointVec.at(v).scalefactor, prehistname)); //up-nominal
    if(m_doPS) drawhistos(h_fractionUnc2PSyst_down.at(v), psfilename);

  }
  //-----------------------------------------------------------
  // compute the uncertainty histograms
  //-----------------------------------------------------------
  printf("\n %s\n Computing the total systematic uncertainty: \n %s\n",std::string(75,'=').c_str(),std::string(75,'=').c_str());
  std::vector<TH2D*> fractionTotalUnc = evaluateQGFUncertaity(h_fractionUnc1PSyst,h_fractionUnc2PSyst_up,h_fractionUnc2PSyst_down,prename);
  if(m_doPS) drawhistos( fractionTotalUnc, psfilename);
  //------------------------------------------------------------------
  // create output file to store fraction histograms in a root file
  //------------------------------------------------------------------
  printf("\n %s\n Writing output file: \n %s\n",std::string(75,'%').c_str(),std::string(75,'%').c_str());
  if(m_config1PointVec.size()+m_config2PointVec.size())
    createOutputFile(outputName, fractionhistos, fractionTotalUnc);
  else
    createOutputFile(outputName, fractionhistos, {});
  //close ps file
  c1.Print((psfilename+"]").c_str());
}
// Evaluate Delta
std::vector<TH2D*> MakeQuarkGluonFractionPlots::getDelta (std::vector<TH2D*> h_nom, std::vector<TH2D*> h_var, double scale, std::string prehistname){
  std::vector<TH2D*> QuarkGluonDiffFractionhistos;
  if(h_nom.size() != h_var.size()) std::cout<<"ERROR: Systematic variation don't have the same number of histograms!\n";
  std::string JetCollection = extractJetCollectionName((h_nom.at(0))->GetName());
  for(size_t i = 0; i<h_nom.size(); i++){
    //return diff*(nominal-varied)
    std::string njets = extractJetNumber(h_nom.at(i)->GetName());
    std::string histname =  prehistname + "_gluonFractionError_" + njets + "_" + JetCollection;    
    TH2D *hist_diff;
    hist_diff =  (TH2D*)((h_nom.at(i))->Clone(histname.c_str()));
    hist_diff->Add((TH2D*)(h_var.at(i)), -1);
    hist_diff->Scale(scale);
    hist_diff->SetTitle(hist_diff->GetName());
    hist_diff->SetStats(kFALSE);
    QuarkGluonDiffFractionhistos.push_back(hist_diff);
  }
  return QuarkGluonDiffFractionhistos;
}

//Evaluate uncertainty
std::vector<TH2D*>  MakeQuarkGluonFractionPlots::evaluateQGFUncertaity(std::vector<std::vector<TH2D*> > f_1P, std::vector<std::vector<TH2D*> > f_2PUp, std::vector<std::vector<TH2D*> > f_2PDown, std::string channel){
  std::vector<TH2D*> histUnc;
  TH2D *h_tmp;
  std::string h_tmp_name;
  if(f_1P.size()!=0){
    h_tmp = (TH2D*)((f_1P.at(0)).at(0))->Clone("TH2_tmp_TotalUnc");
    h_tmp_name = ((f_1P.at(0)).at(0))->GetName();
  }
  else{
    h_tmp = (TH2D*)((f_2PUp.at(0)).at(0))->Clone("TH2_tmp_TotalUnc");
    h_tmp_name = ((f_2PUp.at(0)).at(0))->GetName();
  }
  std::string JetCollection = extractJetCollectionName(h_tmp_name);
  // create a new uncertainty histogram (take the bin and axis from the histograms)
  int binX = h_tmp->GetXaxis()->GetNbins();
  int binY = h_tmp->GetYaxis()->GetNbins();
  h_tmp->Reset();
  h_tmp->Sumw2();
  //Get squared 1P systematics histograms
  std::vector<TH2D*> squared_histos1P;
  if(f_1P.size()!=0){
    for(unsigned int h=0; h<f_1P.at(0).size(); ++h){
		
      std::string h_tmp_name = ((f_1P.at(0)).at(h))->GetName();
      std::string h_new_name("TH2_tmp_1P_TotalUnc_"+std::to_string(h));
      TH2D *h_tmp = (TH2D*)((f_1P.at(0)).at(h))->Clone(h_new_name.c_str());
      h_tmp->Reset();
      h_tmp->Sumw2();
      for(int x = 0;  x <= binX; ++x){
	for(int y = 0; y <= binY; ++y){
	  int bin = h_tmp->GetBin(x,y);
	  float sq_err = 0.;
	  for(unsigned int hs=0; hs<f_1P.size(); ++hs){
	    sq_err+=pow(((f_1P.at(hs)).at(h))->GetBinContent(bin),2);
	  }
	      
	  h_tmp->SetBinContent(bin,sq_err);
	}
      }
      squared_histos1P.push_back(h_tmp);
    }
  }
  //Get squared 2P systematics (symmetrized) histograms
  std::vector<TH2D*> squared_histos2P;
  if(f_2PUp.size()!=0){
    for(unsigned int h=0; h<f_2PUp.at(0).size(); ++h){
      std::string h_tmp_name = ((f_2PUp.at(0)).at(h))->GetName();
      std::string h_new_name("TH2_tmp_2P_TotalUnc_"+std::to_string(h));
      TH2D *h_tmp = (TH2D*)((f_2PUp.at(0)).at(h))->Clone(h_new_name.c_str());
      h_tmp->Reset();
      h_tmp->Sumw2();
      for(int x = 0;  x <= binX; ++x){
	for(int y = 0; y <= binY; ++y){
	  int bin = h_tmp->GetBin(x,y);
	  float sq_err = 0.;
	  float sq_err_up = 0.;
	  float sq_err_down = 0.;
	  for(unsigned int hs=0; hs<f_2PUp.size(); ++hs){
	    float err_up   = ((f_2PUp.at(hs)).at(h))->GetBinContent(bin);
	    float err_down = ((f_2PDown.at(hs)).at(h))->GetBinContent(bin);
	    /* 4 different cases:
	     * (+,-): no action 
	     * (-,+): swap
	     * (+,+): (max(+,+),0)
	     * (-,-): (0,min(-,-))
	     */
	    bool isneg_up   = std::signbit(err_up);
	    bool isneg_down = std::signbit(err_down);
	    if((isneg_up)&&(!isneg_down)){
	      float tmp_swap = err_up;
	      err_up = err_down;
	      err_down = tmp_swap;
	    } 
	    else if((!isneg_up)&&(!isneg_down)){
	      err_up   = std::max(err_up,err_down);
	      err_down = 0.;  
	    }
	    else if((isneg_up)&&(isneg_down)){
	      err_down = std::min(err_up,err_down);
	      err_up   = 0.;  
	    }
	    sq_err_up+=pow(err_up,2);
	    sq_err_down+=pow(err_down,2);
	  }
	  //symmetrize error
	  sq_err=pow((sqrt(sq_err_down)+sqrt(sq_err_up))/2.,2);
	  h_tmp->SetBinContent(bin,sq_err);
	}
      }
      squared_histos2P.push_back(h_tmp);
    }
  }
  //Filling empty final histograms (just to retrieve correct binning
  std::vector<TH2D*> final_histos;

  if(f_1P.size()!=0){
    for(unsigned int h=0; h<f_1P.at(0).size(); ++h){
      std::string h_tmp_name = ((f_1P.at(0)).at(h))->GetName();
      std::string JetCollection = extractJetCollectionName(h_tmp_name);
      std::string njets = extractJetNumber(h_tmp_name);
      std::string histoname = channel + "_gluonFractionError_"+ njets + JetCollection;
      TH2D *h_tmp = (TH2D*)((f_1P.at(0)).at(h))->Clone(histoname.c_str());
      h_tmp->Reset();
      h_tmp->SetTitle(histoname.c_str());
      h_tmp->Sumw2();
      final_histos.push_back(h_tmp);
    }
  }
  else{
    for(unsigned int h=0; h<f_2PUp.at(0).size(); ++h){
      std::string h_tmp_name = ((f_2PUp.at(0)).at(h))->GetName();
      std::string JetCollection = extractJetCollectionName(h_tmp_name);
      std::string njets = extractJetNumber(h_tmp_name);
      std::string histoname = channel + "_gluonFractionError_"+ njets + "_" + JetCollection;
      TH2D *h_tmp = (TH2D*)((f_2PUp.at(0)).at(h))->Clone(histoname.c_str());
      h_tmp->Reset();
      h_tmp->SetTitle(histoname.c_str());
      h_tmp->Sumw2();
      final_histos.push_back(h_tmp);
    }
  }
  //Filling final histograms
  for(unsigned int h=0; h<final_histos.size(); ++h){
    for(int x = 0;  x <= binX; ++x){
      for(int y = 0; y <= binY; ++y){
	int bin = h_tmp->GetBin(x,y);
	float content = 0.;
         
	if((squared_histos1P.size())!=0){
	  content+=(squared_histos1P.at(h))->GetBinContent(bin);
	}
	if((squared_histos2P.size())!=0){
	  content+=(squared_histos2P.at(h))->GetBinContent(bin);
	}
	content = sqrt(content);
	final_histos.at(h)->SetBinContent(bin,content);
      }
    }
  }
  return final_histos;
}
// Compute quark-gluon composition
std::vector<TH2D*> MakeQuarkGluonFractionPlots::computeQuarkGluonFraction (std::map< std::string, TH2D* > inputhistos, std::string quarkflavour, std::string prename){
  std::vector<TH2D*> QuarkGluonFractionhistos;
  std::string jetCollection = extractJetCollectionName(inputhistos.begin()->first);
  int njets =  (inputhistos.size()-6)/5; //FIXME this can be potentially wrong
  if(!m_detailedConfig){
    std::string name_gluons = "gluon_jets_"; name_gluons += jetCollection.c_str();
    std::string name_quarks = "quark_jets_"; name_quarks += jetCollection.c_str();
    TH2D *hist_g = inputhistos[name_gluons];
    TH2D *hist_q = inputhistos[name_quarks];
    //Ratio is f=g/(g+q)
    std::string name_fraction = prename + "_gluonFraction_" + jetCollection;
    TH2D *hist_ratio = (TH2D*)hist_g->Clone(name_fraction.c_str());
    hist_ratio->SetTitle(hist_ratio->GetName());
    hist_ratio->SetStats(kFALSE);
    TH2D *hist_summ = (TH2D*)hist_g->Clone("hist_summ"); //denominator
    hist_summ->Add(hist_q);
    hist_ratio->Divide(hist_summ);
    QuarkGluonFractionhistos.push_back(hist_ratio);
  }
  else{
    for (int nj = -1; nj < njets ; nj++) { //nj = -1 mean inclusive case
      std::string name_gluons   = "gluon_jets_";
      std::string name_lquarks  = "lightquark_jets_";
      std::string name_cquarks  = "cquark_jets_";
      std::string name_bquarks  = "bquark_jets_";
      std::string name_other    = "other_jets_";
      std::string name_fraction = prename + "_gluonFraction_";
      if (nj==-1){
	name_gluons   += jetCollection;
	name_lquarks  += jetCollection;
	name_cquarks  += jetCollection;
	name_bquarks  += jetCollection;
	name_other    += jetCollection;
	name_fraction += jetCollection;
      }
      else{
	std::string name_append="njet" + std::to_string(nj) + "_" + jetCollection;
	name_gluons   += name_append;
	name_lquarks  += name_append;
	name_cquarks  += name_append;
	name_bquarks  += name_append;
	name_other    += name_append;
	name_fraction += name_append;
      }
      TH2D *hist_g = inputhistos[name_gluons];
      TH2D *hist_lj = inputhistos[name_lquarks];
      TH2D *hist_cj = inputhistos[name_cquarks];
      TH2D *hist_bj = inputhistos[name_bquarks];
      TH2D *hist_oth = inputhistos[name_other];
      //Ratio is f=g/(g+lj[light]+cj[lightC]+bj[lightCB])
      TH2D *hist_ratio = (TH2D*)hist_g->Clone(name_fraction.c_str());
      hist_ratio->SetTitle(hist_ratio->GetName());
      hist_ratio->SetStats(kFALSE);
      std::string name_summ =  "hist_summ_" + std::to_string(nj); //denominator
      TH2D *hist_summ = (TH2D*)hist_g->Clone(name_summ.c_str());
      hist_summ->Add(hist_lj);
      if(quarkflavour.compare("light") == 0){
	hist_ratio->Divide(hist_summ);}
      else if (quarkflavour.compare("lightC") == 0){
	hist_summ->Add(hist_cj);
	hist_ratio->Divide(hist_summ);}
      else if(quarkflavour.compare("lightCB") == 0){
	hist_summ->Add(hist_cj);
	hist_summ->Add(hist_bj);
	hist_ratio->Divide(hist_summ);}
      else
	std::cout<< " WARNING: no flavor known to calculate the gluon fraction" << std::endl;
      QuarkGluonFractionhistos.push_back(hist_ratio);
    }
  }
  return QuarkGluonFractionhistos;
}
// Extract jet collection name
std::string MakeQuarkGluonFractionPlots::extractJetCollectionName(std::string histoname){
  if(histoname.find("EMPFlow")!=std::string::npos) return "AntiKt4EMPFlow";
  else if(histoname.find("EMTopo")!=std::string::npos) return "AntiKt4EMTopo";
  else if(histoname.find("LCTopo")!=std::string::npos) return "AntiKt4LCTopo";
  else std::cout<<"ERROR: jet collection not found"<<std::endl;
  return NULL;
}
// Extract number of jets
std::string MakeQuarkGluonFractionPlots::extractJetNumber(std::string histoname){
  std::string njets;
  std::size_t found = histoname.find("njet");
  if (found!=std::string::npos){
    std::size_t first= histoname.find("njet");
    std::size_t last = histoname.find("_AntiKt");
    njets = histoname.substr(first,(last-first));
  }
  return njets;
}
/**************************************************************
 * Configuration - related methods
 * ************************************************************/
void MakeQuarkGluonFractionPlots::printXMLConfigFile(){
  printf("\n %s\n Extracting configuration: \n %s\n",std::string(75,'=').c_str(),std::string(75,'=').c_str());
  std::cout<<" Nominal input file: "<<m_configNominal.path<<std::endl;
  checkFile(m_configNominal.path);
  std::cout<<" Nominal folder: "<<m_configNominal.folder<<std::endl;
  std::cout<<" The gluon fraction will be calculated for the following channels: \n";
  for(unsigned int i=0; i<m_channel.size(); ++i)
    std::cout<<"\t"<<i<<") "<<m_channel.at(i)<<std::endl;
  if(m_detailedConfig) std::cout<<" It will be calculated for different jet multiplicities.\n";
  if(m_mergechannels) std::cout<<" The different channels will be merged.\n";
  std::cout<<" The gluon fraction will be calculated taking into account: "<< m_doGluVsFlavour<< " jets\n";
  std::cout<<"\n\n Systematic samples:\n";
  std::cout<<"\t 1 PointSystematic samples:\n";
  for(unsigned int i=0; i<m_config1PointVec.size();++i){
    std::cout<<"\t("<<i<<") "<<m_config1PointVec.at(i).name<<"\n";
    std::cout<<"\t\tCentral file: "<<m_config1PointVec.at(i).central<<std::endl;
    checkFile(m_config1PointVec.at(i).central);
    std::cout<<"\t\tCentral folder: "<<m_config1PointVec.at(i).folder_c<<std::endl;
    std::cout<<"\t\tVariated file: "<<m_config1PointVec.at(i).variation<<std::endl;
    checkFile(m_config1PointVec.at(i).variation);
    std::cout<<"\t\tVariated folder: "<<m_config1PointVec.at(i).folder_v<<std::endl;
    std::cout<<"\t\tScalefactor to apply: "<<m_config1PointVec.at(i).scalefactor<<std::endl;
  }
  std::cout<<"\t 2 PointSystematic samples:\n";
  for(unsigned int i=0; i<m_config2PointVec.size();++i){
    std::cout<<"\t("<<i<<") "<<m_config2PointVec.at(i).name<<"\n";
    std::cout<<"\t\tCentral file: "<<m_config2PointVec.at(i).central<<std::endl;
    checkFile(m_config2PointVec.at(i).central);
    std::cout<<"\t\tCentral folder : "<<m_config2PointVec.at(i).folder_c<<std::endl;
    std::cout<<"\t\tUp-variated file: "<<m_config2PointVec.at(i).variation_up<<std::endl;
    checkFile(m_config2PointVec.at(i).variation_up);
    std::cout<<"\t\tUp-variated folder: "<<m_config2PointVec.at(i).folder_up<<std::endl;
    std::cout<<"\t\tDown-variated file: "<<m_config2PointVec.at(i).variation_down<<std::endl;
    checkFile(m_config2PointVec.at(i).variation_down);
    std::cout<<"\t\tDown-variated folder: "<<m_config2PointVec.at(i).folder_down<<std::endl;
    std::cout<<"\t\tScalefactor to apply: "<<m_config2PointVec.at(i).scalefactor<<std::endl;
  }
  std::cout<<"\n\n The histograms will be stored inside [channel]_"<<m_OutputFile<<std::endl;
  if (m_doPS) std::cout<<" All plots will be stored in PS files "<<std::endl;
  printf("\n %s\n Configuration Extracted\n %s\n",std::string(75,'=').c_str(),std::string(75,'=').c_str());
}

void MakeQuarkGluonFractionPlots::AbortXMLDecode(std::string value){
  std::cout<<"ERROR! Unknown attribute: "<<value<<". Aborting!\n";
  abort();
}

bool MakeQuarkGluonFractionPlots::decodeFloat(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, float &value){
  if(strncmp(xml.GetAttrName(attr),match.c_str(),100)!=0) return false;
  value=std::stof(xml.GetAttrValue(attr));
  return true;
}

bool MakeQuarkGluonFractionPlots::decodeString(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, std::string &value){
  if(strncmp(xml.GetAttrName(attr),match.c_str(),100)!=0) return false;
  value=xml.GetAttrValue(attr);
  return true;
}

bool MakeQuarkGluonFractionPlots::decodeBool(TXMLEngine xml, XMLAttrPointer_t attr, std::string match, bool &value){
  if(strncmp(xml.GetAttrName(attr),match.c_str(),100)!=0) return false;
  if(strncmp(xml.GetAttrValue(attr),"true",100)==0)
    value=true;
  else if(strncmp(xml.GetAttrValue(attr),"false",100)==0)
    value=false;
  else{
    std::cout<<"ERROR! Unknown attribute value: "<<xml.GetAttrValue(attr)<<". Choose between true or false! Exiting!\n";
    abort();
  }
  return true;
}

void MakeQuarkGluonFractionPlots::readXMLConfigFile(std::string configfile){
  TXMLEngine xml;
  // Now try to parse xml file
  XMLDocPointer_t xmldoc = xml.ParseFile(configfile.c_str());
  if (xmldoc==0) {
    std::cout<<"ERROR! No valid xml configuration file provided! Aborting!\n";
    abort();
  }
  // Access to main node
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);
  // Loop through all nodes and subnodes
  readXMLNode(xml, mainnode);
  // Release memory before exit
  xml.FreeDoc(xmldoc);
}
void MakeQuarkGluonFractionPlots::readXMLNode(TXMLEngine xml, XMLNodePointer_t node){
  if(strncmp(xml.GetNodeName(node),"nominal",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    while (attr!=0) {
      bool pass=0;
      pass+=decodeString(xml, attr, "path", m_configNominal.path);
      pass+=decodeString(xml, attr, "folder", m_configNominal.folder);
      if(!pass) AbortXMLDecode(xml.GetAttrName(attr));
      attr = xml.GetNextAttr(attr);
    }
  }
  else if(strncmp(xml.GetNodeName(node),"systematic1point",100)==0){
    config_1PointSyst temp_config1Point;
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    while (attr!=0) {
      bool pass=0;
      pass+=decodeString(xml, attr, "name", temp_config1Point.name);
      pass+=decodeString(xml, attr, "central", temp_config1Point.central);
      pass+=decodeString(xml, attr, "variation", temp_config1Point.variation);
      pass+=decodeFloat(xml, attr, "scalefactor", temp_config1Point.scalefactor);
      pass+=decodeString(xml, attr, "folder_central", temp_config1Point.folder_c);
      pass+=decodeString(xml, attr, "folder_variation", temp_config1Point.folder_v);
      if(!pass) AbortXMLDecode(xml.GetAttrName(attr));
      attr = xml.GetNextAttr(attr);
    }
    m_config1PointVec.push_back(temp_config1Point);
  }
  else if(strncmp(xml.GetNodeName(node),"systematic2point",100)==0){
    config_2PointSyst temp_config2Point;
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    while (attr!=0) {
      bool pass=0;
      pass+=decodeString(xml, attr, "name", temp_config2Point.name);
      pass+=decodeString(xml, attr, "central", temp_config2Point.central);
      pass+=decodeString(xml, attr, "variation_up", temp_config2Point.variation_up);
      pass+=decodeString(xml, attr, "variation_down", temp_config2Point.variation_down);
      pass+=decodeFloat(xml, attr, "scalefactor", temp_config2Point.scalefactor);
      pass+=decodeString(xml, attr, "folder_central", temp_config2Point.folder_c);
      pass+=decodeString(xml, attr, "folder_up", temp_config2Point.folder_up);
      pass+=decodeString(xml, attr, "folder_down", temp_config2Point.folder_down);
      if(!pass) AbortXMLDecode(xml.GetAttrName(attr));
      attr = xml.GetNextAttr(attr);
    }
    m_config2PointVec.push_back(temp_config2Point);
    std::cout<<"systematic2point\n";
  }
  else if(strncmp(xml.GetNodeName(node),"output",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    if(!decodeString(xml, attr, "path", m_OutputFile)) AbortXMLDecode(xml.GetAttrName(attr));
  }
  else if(strncmp(xml.GetNodeName(node),"channel",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    std::string tmp_channel="";
    if(!decodeString(xml, attr, "name", tmp_channel)) AbortXMLDecode(xml.GetAttrName(attr));
    m_channel.push_back(tmp_channel);
  }
  else if(strncmp(xml.GetNodeName(node),"mergechannels",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    if(!decodeBool(xml, attr, "value", m_mergechannels)) AbortXMLDecode(xml.GetAttrName(attr));
  }
  else if(strncmp(xml.GetNodeName(node),"detailed",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    if(!decodeBool(xml, attr, "value", m_detailedConfig)) AbortXMLDecode(xml.GetAttrName(attr));
  }
  else if(strncmp(xml.GetNodeName(node),"gluon_vs_flavour",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    if(!decodeString(xml, attr, "value",   m_doGluVsFlavour)) AbortXMLDecode(xml.GetAttrName(attr));
    if(strncmp(xml.GetAttrValue(attr),"light"  ,100)
       *strncmp(xml.GetAttrValue(attr),"lightC" ,100)
       *strncmp(xml.GetAttrValue(attr),"lightCB",100)) 
      AbortXMLDecode(xml.GetAttrName(attr));
  }
  else if(strncmp(xml.GetNodeName(node),"do_ps",100)==0){
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    if(!decodeBool(xml, attr, "value", m_doPS)) AbortXMLDecode(xml.GetAttrName(attr));
  }
  //Loop over child nodes
  XMLNodePointer_t child = xml.GetChild(node);
  while (child!=0) {
    readXMLNode(xml, child);
    child = xml.GetNext(child);
  }
}
void MakeQuarkGluonFractionPlots::checkFile(std::string inputfilename){
  bool istxt = ( (inputfilename.find(".txt") + 4 ) == inputfilename.length());
  if(istxt){
    std::string f_name;
    ifstream reader;
    reader.open (inputfilename);
    while(!reader.eof()){
      getline(reader,f_name);
      if(f_name.compare("")==0) continue;
      TFile *f = TFile::Open(f_name.c_str());
      if (f->IsZombie()){
	std::cout<<"File "<<f_name<<" from list "<<inputfilename<<" not found! Please check against your configuration file! Aborting|\n";
	abort();
      }
    }
    reader.close();
  }
  else{
    TFile f(inputfilename.c_str());
    if (f.IsZombie()){
      std::cout<<"File "<<inputfilename<<" not found! Please check against your configuration file! Aborting|\n";
      abort();
    }
  }
}

