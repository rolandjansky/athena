/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//  ===============================================================================
//   Macro to compute the gluon initiated jets fraction in your sample/selection
//   Required input from AnalysisTop: JetFlavorPlots directory
//   To run the macro: .x MakeQuarkGluonFractionPlots.cxx+("config.txt")
//  ===============================================================================

#include "MakeQuarkGluonFractionPlots.h"


MakeQuarkGluonFractionPlots::MakeQuarkGluonFractionPlots(std::string filename): 
  m_inputfile(""),
  m_doGluVsFlavour("lightC"),
  m_systGenerator({}),
  m_systDiffGenerator(1.),
  m_systFragmentation({}),
  m_systDiffFragmentation(1.),
  m_systRadiation({}),
  m_systDiffRadiation(1.),
  m_channel({}),
  m_doLoose(false),
  m_detailedConfig(false),
  m_OutputFile("FlavourComposition"),
  m_doPS(false){
  
  //Suppressing info/error/... ROOT messages
  gErrorIgnoreLevel = 5000;

  std::cout<<" In MakeQuarkGluonFractionPlots::MakeQuarkGluonFractionPlots "<<std::endl;
    
  //----------------------------
  // reading configuration file
  //----------------------------
  readConfigFile(filename);
  
  //-----------------------------------------------------------------------------------------------
  // plotting the 2D input histograms in a ps file and storing them in a vector and
  // compute the gluon fraction for nominal and systematic samples
  //-----------------------------------------------------------------------------------------------
  //vector to store the input histograms created in AT
  std::vector<std::vector<TH2D*>> inputhistos;
  // vector to store the gluon fraction histograms. It can be calculated in different ways:
  // light: computed talking into account light quarks only (f=g/g+lq)
  // lightC: computed talking into account light quarks and c-quark jets (baseline f=g/(g+lq+cq))
  // lightCB: computed talking into account light quarks , c-quar and b-quark jets (f=g/(g+lq+cq+bq)
  std::vector<std::vector<TH2D*>> fractionhistos;
  
  //Systematics
  //vector to store the input histograms created in AT and the gluon fraction
  std::vector<std::vector<TH2D*>> inputhistosSyst;
  std::vector<std::vector<TH2D*>> fractionhistosSyst;
  //vector to store the fraction uncertainty for systematic sample
  std::vector<std::vector<TH2D*>> fractionUnchistosPerSyst;
  //vector to store the fraction uncertainty for all systematic variations
  std::vector<std::vector<std::vector<TH2D*>>> fractionUnchistos; //
  //vector to store the total uncertainty
  std::vector<std::vector<TH2D*>>fractionTotalUnc;


  //loop over channels
  for(size_t i = 0; i < m_channel.size() ; i++){
    
    std::cout<<""<<std::endl;
    std::cout<<" ============================================================================"<<std::endl;
    std::cout<<"  Computing the gluon fraction in "<< m_channel.at(i)<<" channel"<<std::endl;   
    std::cout<<" ============================================================================"<<std::endl;
       
    //create root file to store all intermediate histograms
    std::string hitosrootfilename ="Allhistos_" +  m_OutputFile;
    TFile *hitosrootfile  = new TFile(hitosrootfilename.c_str(),"recreate");
    
    //Creating ps file to plot all flavour plots/channel
    std::string psfilename = "PlotsGluonFraction_" + m_channel.at(i) +".ps";
    TCanvas c1;
    c1.Print((psfilename+"[").c_str());
    
    //Define the common pre-name for the histograms
    std::string prehistname ;
    
    if(!m_inputfile.empty()) {
      std::cout<<" Calculating the gluon fraction for the signal"<< std::endl;   
      inputhistos.push_back(vec2Dhist(m_inputfile, m_channel.at(i), m_channel.at(i)));

      fractionhistos.push_back(computeQuarkGluonFraction (inputhistos.at(i), m_doGluVsFlavour, m_channel.at(i)));
      if(m_doPS) drawhistos(inputhistos.at(i), fractionhistos.at(i), psfilename, hitosrootfile);
    }
    

    //Systematics:  
    std::vector< std::vector<std::string> > systSample;
    std::vector<double> systDiff;
    std::vector<std::string> systLabel;

    // Checking how many systematic variations we have and storing them in a vector
    if(!m_systGenerator.empty()) {
      systSample.push_back(m_systGenerator);
      systDiff.push_back(m_systDiffGenerator);
      systLabel.push_back("GeneratorSyst");}
    
    if(!m_systFragmentation.empty()) {
      systSample.push_back(m_systFragmentation);
      systDiff.push_back(m_systDiffFragmentation);
      systLabel.push_back("FragmentationSyst");}
    
    if(!m_systRadiation.empty()){
      systSample.push_back(m_systRadiation);
      systDiff.push_back(m_systDiffRadiation);
      systLabel.push_back("RadiationSyst");}
    
    // clear the vectors
    fractionUnchistosPerSyst.clear();

    if( !systSample.empty() ){
      
      for (std::vector< std::vector<std::string> >::iterator it = systSample.begin() ; it != systSample.end(); ++it){
    	auto index = std::distance(systSample.begin(), it);
	std::cout<<" Calculating the gluon fraction for "<< systLabel.at(index) << std::endl;
	
	// loop over samples for each systematic: var1 and var2
	for(size_t j = 0; j < (*it).size() ; j++){
	  prehistname = m_channel.at(i) + "_" + systLabel.at(index) + std::to_string(j);
	  inputhistosSyst.push_back( vec2Dhist( ((*it).at(j)).c_str(), m_channel.at(i), prehistname));
	  fractionhistosSyst.push_back( computeQuarkGluonFraction (inputhistosSyst.at(j) , m_doGluVsFlavour, prehistname));
	  if(m_doPS) drawhistos(inputhistosSyst.at(j), fractionhistosSyst.at(j), psfilename, hitosrootfile);
	}
	
	prehistname = m_channel.at(i) + "_" + systLabel.at(index);
	fractionUnchistosPerSyst.push_back(subtracthist(fractionhistosSyst, systDiff.at(index), prehistname));
	if(m_doPS) addhistos ( fractionUnchistosPerSyst.at(index), psfilename, hitosrootfile);
	index++;
	
	//clear the vectors  
	inputhistosSyst.clear();
	fractionhistosSyst.clear();
      }
      fractionUnchistos.push_back(fractionUnchistosPerSyst); 
      
      //-----------------------------------------------------------
      // compute the uncertainty histograms
      //-----------------------------------------------------------
      std::cout<<" Computing the systematic uncertainty "<<std::endl;
      fractionTotalUnc.push_back(computeQuarkGluonFractionUnc(fractionUnchistos.at(i), m_channel.at(i)));
      if(m_doPS) addhistos ( fractionTotalUnc.at(i), psfilename, hitosrootfile);
      
    }//end systematics block
    else std::cout<<" WARNING: no systematic variation found! "<<std::endl;

    
    //------------------------------------------------------------------
    // create output file to store fraction histograms in a root file
    //------------------------------------------------------------------
    std::cout<<" Storing q-g fraction and uncertainty histograms in a root file "<<std::endl;
    std::string outputName = m_channel.at(i) + "_" + m_OutputFile;
    if(!systSample.empty())
      createOutputFile(outputName, fractionhistos.at(i), fractionTotalUnc.at(i));
    else
      createOutputFile(outputName, fractionhistos.at(i), {});
    
    //close ps file
    c1.Print((psfilename+"]").c_str());
  }
}



////////////////////////////////////////////////////////////////////////////////
// Read configuration file
////////////////////////////////////////////////////////////////////////////////
void MakeQuarkGluonFractionPlots::readConfigFile(std::string configfile){
  
  std::ifstream fin(configfile);
  std::string line;
  std::istringstream stream;
  
  while (std::getline(fin, line)) {
    stream.str(line.substr(line.find("=")+1));
    if(line.find("#") != std::string::npos) continue;
    if (line.find("INPUTFILE") != std::string::npos) stream >> m_inputfile;
    if (line.find("SYSTFILE_GEN1") != std::string::npos) m_systGenerator.push_back(stream.str());
    if (line.find("SYSTFILE_GEN2") != std::string::npos) m_systGenerator.push_back(stream.str());
    if (line.find("SYSTDIFF_GEN") != std::string::npos) stream >> m_systDiffGenerator;
    if (line.find("SYSTFILE_FRAG1") != std::string::npos) m_systFragmentation.push_back(stream.str());
    if (line.find("SYSTFILE_FRAG2") != std::string::npos) m_systFragmentation.push_back(stream.str());
    if (line.find("SYSTDIFF_FRAG") != std::string::npos) stream >> m_systDiffFragmentation;
    if (line.find("SYSTFILE_RAD1") != std::string::npos) m_systRadiation.push_back(stream.str());
    if (line.find("SYSTFILE_RAD2") != std::string::npos) m_systRadiation.push_back(stream.str());
    if (line.find("SYSTDIFF_RAD") != std::string::npos) stream >> m_systDiffRadiation;
    if (line.find("CHANNEL") != std::string::npos) {
      std::string channel;
      char separator = ':';
      while(std::getline(stream, channel, separator)) 
	m_channel.push_back(channel);
    }
     if (line.find("DOLOOSE") != std::string::npos) m_doLoose = true ;
    if (line.find("DETAILED") != std::string::npos) m_detailedConfig = true ;
    if (line.find("GLUON_VS_FLAVOUR") != std::string::npos) stream >> m_doGluVsFlavour;
    if (line.find("OUTPUTFILE") != std::string::npos) stream >> m_OutputFile;
    if (line.find("DO_PS") != std::string::npos) m_doPS = true ;
    stream.clear();
  }
  
  std::cout << "\n"; 
  std::cout<<" ============================================================================"<<std::endl;
  std::cout<<" Extracting configuration:"<<std::endl;
  std::cout<<" ============================================================================"<<std::endl;
  std::cout << "\n";
  
  if(!m_inputfile.empty()) {
    std::cout<<" Input file readed from: "<<m_inputfile<<std::endl;
    if (!checkFile(m_inputfile)){
      std::cout<<" ERROR: input file doens't exist "<<std::endl;
      exit (EXIT_FAILURE);
    }
  }
  else std::cout<<" ERROR: no input sample provided "<<std::endl;
  std::cout << "\n";
  
  std::cout<<" The gluon fraction will be calculated for the following channels: "<<std::endl;
  for (size_t i=0;i<m_channel.size();i++){ std::cout<< "\t - channel: "<< m_channel.at(i)<< std::endl;}
  if(m_detailedConfig) std::cout<<" and it will be calculated for different jet multiplicities "<<std::endl;
  std::cout << "\n";

  std::cout<<" The gluon fraction will be calculated taking into account: "<< m_doGluVsFlavour<< " jets" <<std::endl;
  std::cout << "\n";
  
  std::cout<<" Systematic samples: "<<std::endl;
  std::cout<<"\t* GENERATOR systematic: "<<std::endl;
  if(m_systGenerator.empty()) std::cout<<"\t WARNING: Generator systematic not found "<<std::endl;
  else if (!checkFile(m_systGenerator.at(0))){
    std::cout<<"\t\t WARNING: Generator systematic var1 not found "<<std::endl;
    if (!checkFile(m_systGenerator.at(1))) std::cout<<"\t\t WARNING: Generator systematic var2 not found "<<std::endl;
    std::cout<< "\t WARNING: Generatort systematic will not be taken into account"<<std::endl;
    m_systGenerator.clear();
  }
  else {
    std::cout<<"\t - var1:"<< m_systGenerator.at(0)<< std::endl;
    std::cout<<"\t - var2:"<< m_systGenerator.at(1)<< std::endl;
    if(m_systDiffGenerator == 1) std::cout<<"\t - full difference taken as a systematic variation"<< std::endl;
    else std::cout<<"\t - difference of "<< m_systDiffGenerator<<" taken as systematic variation"<< std::endl;
  }
  
  std::cout<<"\t* FRAGMENTATION systematic: "<<std::endl;
  if(m_systFragmentation.empty()) std::cout<<" \t WARNING: Fragmentation systematic not found "<<std::endl;
  else if(!checkFile(m_systFragmentation.at(0))){
    std::cout<<"\t\t WARNING: Fragmentation systematic var1 not found "<<std::endl;
    if (!checkFile(m_systFragmentation.at(1))) std::cout<<"\t\t WARNING: Fragmentation systematic var2 not found "<<std::endl;
    std::cout<< "\t WARNING: Fragmentation systematic will not be taken into account"<<std::endl;
      m_systFragmentation.clear();
  }
  else{
    std::cout<<"\t - var2:"<< m_systFragmentation.at(1)<< std::endl;
    std::cout<<"\t - var1:"<< m_systFragmentation.at(0)<< std::endl;
    if(m_systDiffFragmentation==1) std::cout<<"\t - full difference taken as a systematic variation"<< std::endl;
    else std::cout<<"\t - difference of "<< m_systDiffFragmentation<<" taken as systematic variation"<< std::endl;
  }
  
  std::cout<<"\t* RADIATION systematic: "<<std::endl;
  if(m_systRadiation.empty()) std::cout<<" \t WARNING: Radiation systematic not found "<<std::endl;
  else if(!checkFile(m_systRadiation.at(0))){
    
    std::cout<<"\t\t WARNING: Radiation systematic var1 not found "<<std::endl;
    if (!checkFile(m_systRadiation.at(1))) std::cout<<"\t\t WARNING: Radiation systematic var1 not found "<<std::endl;
    std::cout<< "\t WARNING: Radiation systematic will not be taken into account"<<std::endl;
    m_systRadiation.clear();
  }
  else{
    std::cout<<"\t - var1:"<< m_systRadiation.at(0)<< std::endl;
    std::cout<<"\t - var2:"<< m_systRadiation.at(1)<< std::endl;
    if(m_systDiffRadiation==1) std::cout<<"           - full difference taken as a systematic variation"<< std::endl;
    else std::cout<<"           - difference of "<< m_systDiffRadiation<<" taken as systematic variation"<< std::endl;
  }
  std::cout << "\n";
  if (m_doPS) std::cout<<" All plots will be stored in PS files "<<std::endl;
}

////////////////////////////////////////////////////////////////////////////////
// Check if files exist
////////////////////////////////////////////////////////////////////////////////
bool MakeQuarkGluonFractionPlots::checkFile(std::string inputfilename){
  bool file = true;
  TFile f(inputfilename.c_str());
  if (f.IsZombie()) file = false;
  return file;
}

////////////////////////////////////////////////////////////////////////////////
// Store histograms in a vector
////////////////////////////////////////////////////////////////////////////////
std::vector<TH2D*> MakeQuarkGluonFractionPlots::vec2Dhist( std::string filepath, std::string channel, std::string prehistname) {

  std::string dirname = channel + "/JetFlavorPlots";
  if(m_doLoose) dirname = channel + "/JetFlavorPlots_Loose";
  std::vector<TH2D*> myvector;
  
  TFile *f1 = TFile::Open(filepath.c_str());
  TDirectory* dir = f1->GetDirectory(dirname.c_str());
  if(!dir) {
    std::cout<<" EXIT: directory not found!!!! "<<std::endl;
    exit (EXIT_FAILURE);
  }
  TIter next(dir->GetListOfKeys());
  TKey *key;
  
  while ((key = (TKey*)next())) {
    TClass *cl = gROOT->GetClass(key->GetClassName());
    if (!cl->InheritsFrom("TH2D")) continue;
    TH2D *h = (TH2D*)key->ReadObj();
    std::string histoname = prehistname + "_" + h->GetName() ;
    h->SetName(histoname.c_str());
    h->SetTitle(histoname.c_str());
    h->SetStats(kTRUE); // Activate or desactivate the statistics box            
    gStyle->SetStatY(0.95); // Set y-position (fraction of pad size)            
    gStyle->SetStatX(0.89); // Set x-position (fraction of pad size)            
    myvector.push_back(h);
  }
  return myvector;
}



////////////////////////////////////////////////////////////////////////////////
// Compute quark-gluon composition
////////////////////////////////////////////////////////////////////////////////

std::vector<TH2D*> MakeQuarkGluonFractionPlots::computeQuarkGluonFraction (std::vector<TH2D*> inputhistos, std::string quarkflavour, std::string prehistname){
  

  std::vector<TH2D*> QuarkGluonFractionhistos;
  std::string jetCollection = extractJetCollectionName((inputhistos.at(0))->GetName());
  
  std::string  name_gluons;
  std::string  name_quarks;
  std::string  name_lquarks;
  std::string  name_cquarks;
  std::string  name_bquarks;
  std::string  name_other;
  TH2D *hist_g;
  TH2D *hist_q;
  TH2D *hist_lj;
  TH2D *hist_cj;
  TH2D *hist_bj;
  TH2D *hist_oth;
  TH2D *hist_summ;
  TH2D *hist_ratio;
  int njets =  (inputhistos.size()-6)/5;
  

  if(!m_detailedConfig){
    
    for (std::vector<TH2D*>::iterator it = inputhistos.begin() ; it != inputhistos.end(); ++it){
      
      name_gluons = prehistname + "_gluon_jets_" + jetCollection.c_str();
      name_quarks = prehistname + "_quark_jets_" + jetCollection.c_str();
            
      if(name_gluons == std::string((*it)->GetName()))
	hist_g = *it;
      if(name_quarks == std::string((*it)->GetName()))
	hist_q = *it;
    }
    
    //Ratio
    std::string name_fraction =  prehistname + "_gluonFraction_" + jetCollection;
    hist_ratio = (TH2D*)hist_g->Clone(name_fraction.c_str());
    hist_ratio->SetTitle(hist_ratio->GetName());
    hist_ratio->SetStats(kFALSE);
    hist_summ = (TH2D*)hist_g->Clone("hist_summ"); //denominator
    hist_summ->Add(hist_q);
    hist_ratio->Divide(hist_summ);
    QuarkGluonFractionhistos.push_back(hist_ratio);
  }
  
  else{
    std::string name_fraction;

    for (int nj = -1; nj < njets ; nj++) { //nj = -1 mean inclusive case
      if (nj==-1){
	name_gluons	 = prehistname + "_gluon_jets_" + jetCollection.c_str();
	name_lquarks	 = prehistname + "_lightquark_jets_" +  jetCollection.c_str();
	name_cquarks	 = prehistname + "_cquark_jets_" + jetCollection.c_str();
	name_bquarks	 = prehistname + "_bquark_jets_" + jetCollection.c_str();
	name_other	 = prehistname + "_other_jets_" + jetCollection.c_str();
	name_fraction    =  prehistname + "_gluonFraction_" + jetCollection;
      }
      else{
	name_gluons	 = prehistname + "_gluon_jets_njet" + std::to_string(nj) +"_"+ jetCollection.c_str();
	name_lquarks	 = prehistname + "_lightquark_jets_njet" + std::to_string(nj) +"_"+ jetCollection.c_str();
	name_cquarks	 = prehistname + "_cquark_jets_njet" + std::to_string(nj) +"_"+ jetCollection.c_str();
	name_bquarks	 = prehistname + "_bquark_jets_njet" + std::to_string(nj) +"_"+ jetCollection.c_str();
	name_other	 = prehistname + "_other_jets_njet" + std::to_string(nj) +"_"+ jetCollection.c_str();
	name_fraction    =  prehistname + "_gluonFraction_njet" + std::to_string(nj) +"_"+ jetCollection;
      }

      for (std::vector<TH2D*>::iterator it = inputhistos.begin() ; it != inputhistos.end(); ++it){
	if(name_gluons == std::string((*it)->GetName()))
	  hist_g = *it;
	if(name_lquarks == std::string((*it)->GetName()))
	  hist_lj = *it;
	if(name_cquarks == std::string((*it)->GetName()))
	  hist_cj = *it;
	if(name_bquarks == std::string((*it)->GetName()))
	  hist_bj = *it;
	if(name_other == std::string((*it)->GetName()))
	  hist_oth = *it;
      }
      
      hist_ratio = (TH2D*)hist_g->Clone(name_fraction.c_str());
      hist_ratio->SetTitle(hist_ratio->GetName());
      hist_ratio->SetStats(kFALSE);
      std::string name_summ =  "hist_summ_" + std::to_string(nj); //denominator
      hist_summ = (TH2D*)hist_g->Clone(name_summ.c_str());
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


////////////////////////////////////////////////////////////////////////////////
// Extract jet collection name
////////////////////////////////////////////////////////////////////////////////
std::string MakeQuarkGluonFractionPlots::extractJetCollectionName(std::string histoname){
  
  if(histoname.find("EMPFlow")!=std::string::npos) return "AntiKt4EMPFlow";
  else if(histoname.find("EMTopo")!=std::string::npos) return "AntiKt4EMTopo";
  else if(histoname.find("LCTopo")!=std::string::npos) return "AntiKt4LCTopo";
  else std::cout<<"ERROR: not jet collection found"<<std::endl;
  return NULL;
}


////////////////////////////////////////////////////////////////////////////////
// Extract jet multiplicity
////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////
// Subtract histograms
////////////////////////////////////////////////////////////////////////////////
std::vector<TH2D*> MakeQuarkGluonFractionPlots::subtracthist (std::vector<std::vector<TH2D*>> systhistos, double diff,  std::string prehistname){
  
  std::vector<TH2D*> QuarkGluonDiffFractionhistos;

  if((systhistos.at(0)).size() !=(systhistos.at(1)).size()) std::cout<<"ERROR: Systematic variation don't have the same number of histograms!"<<std::endl;
  
  std::string JetCollection = extractJetCollectionName(((systhistos.at(0)).at(0))->GetName());
  
  for(size_t i = 0; i<(systhistos.at(0)).size(); i++){
    std::string njets = extractJetNumber(((systhistos.at(0)).at(i))->GetName());
    std::string histname =  prehistname + "_gluonFractionError_" + njets + "_" + JetCollection;    
    
    TH2D *hist_diff;
    hist_diff =  (TH2D*)(((systhistos.at(0)).at(i))->Clone(histname.c_str()));
    hist_diff->Add((TH2D*)((systhistos.at(1)).at(i)), -1);
    hist_diff->Scale(diff);
    hist_diff->SetTitle(hist_diff->GetName());
    hist_diff->SetStats(kFALSE);
    QuarkGluonDiffFractionhistos.push_back(hist_diff);
  }
  return QuarkGluonDiffFractionhistos;
}


////////////////////////////////////////////////////////////////////////////////
// adding systematic in quadrature
////////////////////////////////////////////////////////////////////////////////
std::vector<TH2D*>  MakeQuarkGluonFractionPlots::computeQuarkGluonFractionUnc(vector<vector<TH2D*>> systerror, std::string channel){

  std::vector<TH2D*> histUnc;
  std::string JetCollection = extractJetCollectionName(((systerror.at(0)).at(0))->GetName());
  
  // create a new uncertainty histogram (take the bin and axis from the histograms)
  int  binX = ((systerror.at(0)).at(0))->GetXaxis()->GetNbins();
  int  binY = ((systerror.at(0)).at(0))->GetYaxis()->GetNbins();
  const TArrayD *xbins = ((systerror.at(0)).at(0))->GetXaxis()->GetXbins();
  double binXedges[binX];
  for(int i = 0 ; i <=binX ; i++)
    binXedges[i] = xbins->GetAt(i);
  const TArrayD *ybins = ((systerror.at(0)).at(0))->GetYaxis()->GetXbins();
  double binYedges[binY];
  for(int i = 0 ; i <= binY ; i++) 
    binYedges[i] = ybins->GetAt(i);

  TH2D* hist_unc;
  for(size_t i = 0; i<(systerror.at(0)).size(); i++){
    std::string njets = extractJetNumber(((systerror.at(0)).at(i))->GetName());
    std::string histoname = channel + "_gluonFractionError_"+ njets+ "_AntiKt4EMTopo";
    hist_unc = new TH2D(histoname.c_str(), histoname.c_str(), binX,  binXedges, binY, binYedges);
    hist_unc->Sumw2();
    
    //fill the uncertainty histogram
    for(int j = 0;  j <= binX; j++){
      for(int k = 0; k <= binY; k++){
	//it returns a linearized bin number. 
	Int_t bin = ((systerror.at(0)).at(i))->GetBin(j,k);
	float unc1 = ((systerror.at(0)).at(i))->GetBinContent(bin);
	float unc2 = 0;
	float unc3 = 0;
	if (systerror.size() ==2)  unc2 = ((systerror.at(1)).at(i))->GetBinContent(bin);
	if (systerror.size() ==3)  unc3 = ((systerror.at(2)).at(i))->GetBinContent(bin);
	
	float totalUnc = sqrt(pow(unc1,2)+pow(unc2,2)+pow(unc3,2));
	hist_unc->SetBinContent(bin,totalUnc);
      }
    }
    histUnc.push_back(hist_unc);
  }
  return histUnc;
}



///////////////////////////////////
//  Draw histograms form a vector
///////////////////////////////////
void MakeQuarkGluonFractionPlots::drawhistos(std::vector<TH2D*> histos, std::vector<TH2D*> histosfraction, std::string psfilename, TFile* hitosrootfile) {
  hitosrootfile->cd();
  TCanvas c1;
  c1.SetLogx();

  for (std::vector<TH2D*>::iterator it = histos.begin() ; it != histos.end(); ++it){
    TH2D *h = *it;
    h->Draw("COLZtext");
    c1.Print(psfilename.c_str());
    h->Write(); //storing them in the root file
  }
  for (std::vector<TH2D*>::iterator it = histosfraction.begin() ; it != histosfraction.end(); ++it){
    TH2D *histosfraction = *it;
    histosfraction->Draw("COLZtext");
    c1.Print(psfilename.c_str());
    histosfraction->Write(); //storing them in the root file
  }
  return;
}


///////////////////////////////////
//  Add histograms to a ps file
///////////////////////////////////
void MakeQuarkGluonFractionPlots::addhistos( std::vector<TH2D*> histo, std::string psfilename, TFile* hitosrootfile) {
  hitosrootfile->cd();
  TCanvas c1;
  c1.SetLogx();
  for(size_t i=0; i<histo.size(); i++){
  histo.at(i)->Draw("COLZtext");
  c1.Print(psfilename.c_str());
  histo.at(i)->Write();//storing them in the root file
  }
}


//void MakeQuarkGluonFractionPlots::createOutputFile(std::string filename, std::vector<TH2D*> histos, std::vector<TH2D*> histosUnc){
void MakeQuarkGluonFractionPlots::createOutputFile(std::string filename, std::vector<TH2D*> histos, std::vector<TH2D*> histosUnc){
  TFile *fout   = new TFile(filename.c_str(),"recreate");
  fout->cd();
  for (std::vector<TH2D*>::iterator it = histos.begin() ; it != histos.end(); ++it) (*it)->Write();
  for (std::vector<TH2D*>::iterator it = histosUnc.begin() ; it != histosUnc.end(); ++it) { if((*it)!= NULL)  (*it)->Write();}
  fout->Close();
  std::cout<<" Histograms are saved in "<<filename<<std::endl;
}




