/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include <sstream>

#define protected public
#include "JetAnalysisEDM/Particle.h"
#include "JetAnalysisEDM/EventObject.h"
#include "JetAnalysisEDM/EventManager.h"

#include "TFile.h"
#include "TChain.h"



using namespace std;
using namespace JetAnalysisEDM;

TTree* ptree = 0;

typedef Long64_t Index;
typedef string Name;
typedef std::vector<Name> NameList;

typedef std::vector<string> NameList;



int dumpjets(Index ient1 =0, Index nent =1, int detail =2, int maxobj =0,
             string cname ="jet_CamKt12LCTopoSplitFilteredmassFraction67minSplitR0_") {
  const string myname = "dumpjets: ";
  const double mevtogev = 0.001;
  int count = 0;

  Index ient = 0;

  Name tname = "physics";
  ptree = dynamic_cast<TTree*>(gDirectory->Get(tname.c_str()));
  if ( ptree == 0 ) {
    cout << myname << "Input tree not found: " << tname << endl;
    return -1;
  }
  Index maxent = ptree->GetEntries();
  cout << myname << "Input tree: " << ptree->GetName() << endl;
  string svar = cname + "E:" + cname + "m:" + cname + "pt:" + cname + "m";
  cout << myname << "Tree entry count: " << maxent << endl;
  cout << myname << "Event scan:" << endl;

  const ParticleContainer jets(cname, ient, ptree);
  EventObject event(ient, ptree);

  bool showscan = detail > 2;

  NameList fnames;
  NameList inames;


  Index ient2 = ient1 + nent;
  for ( ient=ient1; ient<ient2; ++ient ) {
    cout << "\nEntry " << ient << endl;
    //cout << "  Run " << event.get<Index>("RunNumber")
    //     << ", Event " << event.get<Index>("EventNumber") << endl;
    cout << "  Run " << event.RunNumber() << ", Event " << event.EventNumber() << endl;
    int njet = jets.size();
    cout << "  Jet multiplicity: " << njet << endl;
    cout<< " *****    "<<endl;
    cout << jets.eta.fInBranch << endl;
    cout<< " *****    "<<endl;

    string line = "-------------------------------------------------------------------------";
    int wname = 60;
    if ( detail > 0 ) {
      Index njetshow = njet;
      if ( (maxobj > 0) && (maxobj < njet) ) njetshow = maxobj;
      ParticleContainer::const_iterator itjet = jets.begin();
      cout << " njetshow "<< njetshow << endl;
      for ( Index ijet=0; ijet<njetshow; ++ijet ) {
        const Particle* pjet = *itjet++;
        if ( detail > 1 ) cout << line << endl;
        // One line summary.
        ostringstream ssjetline;
        ssjetline << "pT ="  << setw(6) << std::fixed << setprecision(1) << mevtogev*pjet->pt() << " GeV";
        ssjetline << ", m =" << setw(6) << std::fixed << setprecision(1) << mevtogev*pjet->m() << " GeV";
        ssjetline << ", eta =" << setw(5) << std::fixed << setprecision(2) << pjet->eta();;
        ssjetline << ", phi =" << setw(5) << std::fixed << setprecision(2) << pjet->phi();;
        cout << "    Jet " << ijet << ": " << ssjetline.str() << endl;
        // All properties.
        if ( detail > 1 ) {
          if ( fnames.size() == 0 ) fnames = pjet->getAttributeKeys<float>();
          cout << "    Jet has " << fnames.size() << " float attributes:" << endl;
          for ( NameList::const_iterator inam=fnames.begin(); inam!=fnames.end(); ++inam ) {
            Name name = *inam;
            cout << setw(wname) << name << ":" << setw(12) << std::fixed << setprecision(3) << pjet->get<float>(name) << endl;
            //Particle newjet(*pjet);
            //cout << setw(10) << name << ":" << setw(8) << setprecision(3) << newjet.getF(name) << endl;
          }
          if ( inames.size() == 0 ) inames = pjet->getAttributeKeys<int>();
          cout << "    Jet has " << inames.size() << " int attributes:" << endl;
          for ( NameList::const_iterator inam=inames.begin(); inam!=inames.end(); ++inam ) {
            Name name = *inam;
            cout << setw(wname) << name << ":" << setw(12) << std::fixed << setprecision(3) << pjet->get<int>(name) << endl;
          }
        }
      }
      if ( detail > 1 ) cout << line << endl;
    }
    if ( showscan ) {
      ostringstream sssel;
      sssel << "Entry$==" << ient;
      string ssel = sssel.str();
      ptree->Scan(svar.c_str(), ssel.c_str());
    }
    count += njet;
  }

  return count;
}


void dumpCollection(const std::string& fname, const std::string & cname, int eventNum){

  
  TFile* file = TFile::Open(fname.c_str() );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );
  if( ! tree ) {
    std::cerr << "Couldn't find TTree!" << std::endl;
    return ;
  }
  
  EventManager eventM(tree);

  eventM.setEntry( eventNum );

  
  const ParticleContainer * partCont = eventM.retrieve(cname.c_str());
  
  std::cout << std::fixed;
  for(size_t i=0; i<partCont->size(); i++){
    const Particle* p = (*partCont)[i];
    std::cout<< setw(4) << i << setw(12) << setprecision(1) << p->pt() << " ("<< setw(5)<< setprecision(2) << p->eta() <<","<<setw(5)<< p->phi() <<") " <<setprecision(1) << p->m() << std::endl;
  }

  

}


void oneTimeAccess(TTree* tree, Long64_t & eventIndex){
  const ParticleContainer jetCont("jet_AntiKt4LCTopoJets_", eventIndex);
  const ParticleContainer jetCont2("jet_AntiKt4LCTopoJets_", eventIndex);

  // connect it to the input tree.
  jetCont.readFrom(tree);
  jetCont2.readFrom(tree);
  int njet = jetCont.size();
  cout << "  Jet multiplicity: " << njet << endl;
  //ParticleContainer::const_iterator itjet = jetCont.begin();

  string line = "-------------------------------------------------------------------------";
  for(size_t i=0; i< jetCont.size(); i++){
    // retrieve particle.
    const Particle * jet = jetCont[i];
    //NameList fnames = jet->getAttributeKeys<float>();      
    const Particle * jet2 = jetCont2[i];
    if(i<2) std::cout << jet->eta() << "  "<< jet->get<float>("LCJES")<< "  " << jet->get<int>("flavor_assoctrk_n") << std::endl;
    if(i<2) std::cout << jet2->eta() << "  "<< jet2->get<float>("LCJES")<<  "  " << jet2->get<int>("flavor_assoctrk_n") << std::endl;
  }

}

void test(){

  // Prepare an input file **********************************************
  TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root" );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );
  if( ! tree ) {
    std::cerr << "Couldn't find TTree!" << std::endl;
    return ;
  }
 
  // **********************************************
  // Define one global variable for event index.
  Long64_t eventIndex = 0;
  // **********************************************


  // **********************************************
  // ParticleContainer
  // **********************************************
  // Define a ParticleContainer, give it the prefix according to the D3PD branches name.
  const ParticleContainer ak10Cont("jet_AntiKt10LCTopo_", eventIndex);
  // connect it to the input tree.
  ak10Cont.readFrom(tree);
  const ParticleContainer ak4Cont("jet_AntiKt4LCTopoJets_", eventIndex, tree);

  const ParticleContainer clCont("cl_", eventIndex);
  clCont.declareKinematicVar("pt", "eta", "phi", "E_em");
  clCont.forceInputMass(0.);
  clCont.readFrom(tree);

  // **********************************************

  // define an EventObject. 
  EventObject event( eventIndex , tree);

  // Write out some of the above
  // Open an output file:
  TFile* ofile = TFile::Open( "testFile.root", "RECREATE" );
  ofile->cd();

  // Create an output tree:
  TTree* otree = new TTree( "physics", "Selected MuctpiD3PD tree" );
  otree->SetAutoSave( 100000000 );
  TTree::SetBranchStyle( 1 );
  otree->SetDirectory( ofile );

  
  // Write out some of the input container content
  ak10Cont.setOutputVar(true, "^.*"); // This selects only the variables starting with 'c'
  // declare which tree to write in. This automatically selects the kinematic vars to be written out (eta,pt,phi,E) 
  ak10Cont.writeTo( otree );    

  // Write out some of the input container content
  ak4Cont.setOutputVar(true, "^.*"); // This selects only the variables starting with 'c'
  // declare which tree to write in. This automatically selects the kinematic vars to be written out (eta,pt,phi,E) 
  ak4Cont.writeTo( otree );    
  clCont.writeTo( otree );    

  event.declareOutputVar<float>("actualIntPerXing");
  event.declareOutputVar<int>("cl_n");
  event.declareOutputVar<int>("el_n");
  event.declareOutputVar<vector<float> >("el_pt");

  event.writeTo( otree) ;

    Particle firstJet;

    firstJet.writeTo(otree, "singlejet_");


  // **********************************************
  // Event Loop
  // **********************************************
  for( eventIndex = 0; eventIndex < tree->GetEntries(); ++eventIndex ) {
    std::cout << "Event  "<< eventIndex<< "  N jet = "<< ak10Cont.size()  << std::endl;

    // for(size_t i=0; i< ak10Cont.size(); i++){
    //   // retrieve particle.
    //   const Particle * jet = ak10Cont[i];
    //   if(i<2) std::cout << jet->pt() << "  "<< jet->get<float>("emscale_eta")<<std::endl;
    // }

    firstJet = *ak10Cont[0]; // copy 4 momentum and link


    ak10Cont.readAllActive();
    ak4Cont.readAllActive();
    clCont.readAllActive();
    event.readAllActive();

    otree->Fill();

  }
  file->Close();
  delete file;

  otree->Write();
  ofile->Close();


}

void testDumpJet(){
   
  //TFile* file = TFile::Open("/home/delsart/analysis/d3pdreaderTest/TestFiles/NTUP_COMMON.01317700._000001_only5.root.1" );
  //TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root" );
  // Access the D3PD TTree inside of it:
  //TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );
  
  
  
  dumpjets(0,1,1,0,"jet_AntiKt4LCTopo");
  std::cout << " -----------\n"<<std::endl;
  std::cout << " -----------\n"<<std::endl;
  //dumpjets(3,2,2,1,"jet_AntiKt4LCTopoJets");

}

void testEventManager(){
  TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root" );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );

  EventManager event(tree);

  event.setEntry(0);
  ParticleContainer * tmp = new ParticleContainer("AApart");
  event.record( "AApart", tmp );

  cout << tmp << " // "  << event.retrieve("AApart") << endl;

  event.setEntry(1);
  cout << tmp<< "  // " << event.retrieve("AApart") << endl;

    

  // const ParticleContainer * jetContI = event.retrieve("jet_AntiKt4LCTopoJets");
  // while( event.nextEntry() ){
  //   const ParticleContainer * jetCont = event.retrieve("jet_AntiKt4LCTopoJets");
  //   cout << event.currentEntry() << " EventNumber "<< event.EventNumber() << "  njet="<< jetCont->size()  << "  "<< jetCont << " / "<< jetContI << endl;
  //   cout << " jet 0 pt="<< jetCont->operator[](0)->pt() << endl;
    
  // }
  
}
void testoneTimeAccess(){

  TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root" );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );

  Long64_t index = 0;
  oneTimeAccess(tree, index);
  index= 4;
  std::vector<float> v(3000);
  oneTimeAccess(tree, index);
}

void testEventObject(){
  Long64_t index = 0;
  JetAnalysisEDM::EventObject o(index,0);

}

int main(int argc, char *argv[]) {

  //dumpCollection( argv[1], argv[2], 3);
  //testDumpJet();
  //testoneTimeAccess();
  testEventManager();
  //test();
  //  testEventObject();
  
}
