/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>


#include "JetAnalysisEDM/Particle.h"
#include "JetAnalysisEDM/EventObject.h"
#include "JetAnalysisEDM/KinematicUtils.h"

#include "TFile.h"




using namespace std;
using namespace JetAnalysisEDM;

void f1() {
 
  
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
  const ParticleContainer jetCont("jet_AntiKt10LCTopo_", eventIndex);
  // connect it to the input tree.
  jetCont.readFrom(tree);
  // Alternatively : const ParticleContainer jetCont("jet_AntiKt10LCTopo_", eventIndex, tree);
  // **********************************************



  // **********************************************
  // EventObject
  // **********************************************
  // define an EventObject. 
  EventObject event( eventIndex , tree);

  // One can also directly define a simple variable 
  //   WARNING : this is less safe, will return meanigless value if branch name is mispelled
  VarHandle< std::vector<float>  > muon_pt( "mu_pt", &eventIndex);    
  muon_pt.readFrom( tree );
  // **********************************************

  

  // **********************************************
  // Write out 
  // **********************************************
  // Open an output file:
  TFile* ofile = TFile::Open( "output_test.root", "RECREATE" );
  ofile->cd();
  
  // Create an output tree:
  TTree* otree = new TTree( "bla", "Selected MuctpiD3PD tree" );
  otree->SetAutoSave( 100000000 );
  TTree::SetBranchStyle( 1 );
  otree->SetDirectory( ofile );
  
  // Write out some of the input container content
  jetCont.setOutputVar(true, "^c.*"); // This selects only the variables starting with 'c'
  // declare which tree to write in. This automatically selects the kinematic vars to be written out (eta,pt,phi,E) 
  jetCont.writeTo( otree );    
  // **********************************************



  // **********************************************
  // Create a new output container.
  // **********************************************
  ParticleContainer myjets( "myjets_",eventIndex);
  // Don't link it to the input file. We will build and fill it from scratch
  // We want to save it. As it is entirely new, we must declare in advance wich branch we want to save
  myjets.declareOutputVar<float>("Tau2"); // declare 1 branch.
  myjets.declareOutputVarAsIn(&jetCont, "^c.*"); // declare the same branch as in jetCont which start with 'c'
  // declare which tree to write in. This must come AFTER declaration of branches to be written out.
  myjets.writeTo( otree);
  // **********************************************



  // **********************************************
  // Constituents access. 
  // **********************************************
  //  It is mandatory to define the source of the constituents
  const ParticleContainer clust("cl_", eventIndex);
  // one can customize the Branch name of the kinematic variables
  clust.declareKinematicVar("pt", "eta", "phi", "E_em"); // (my ntuple doesn't have an "E" variable)
  // in this case, readFrom, must come *after*
  clust.readFrom(tree);

  // IMPORTANT declare clust as the source of constituents, give the variable index name.
  jetCont.declareConstituents(&clust, "constit_index");
  
  

  // **********************************************
  // Event Loop
  // **********************************************
  for( eventIndex = 0; eventIndex < tree->GetEntries(); ++eventIndex ) {
    std::cout << "Event  "<< eventIndex<< "  N jet = "<< jetCont.size()  << std::endl;
    
    // -----------------
    // ask the event object for *any* d3pd variable
    int runnumber = event.RunNumber();
    float mu = event.get<float>("averageIntPerXing");
    const std::vector<float> & v0_z = event.get<std::vector<float>&>("v0_z");
    std::cout<< runnumber<< " mu="<< mu << " v0_z.size()="<< v0_z.size() << std::endl;

    for(size_t i=0; i< jetCont.size(); i++){
      // retrieve particle.
      const Particle * jet = jetCont[i];
      if( jet->pt() > 20000 ){
        // -----------------
        // access dynamic attributes :
        float tau1 = jet->get<float>("Tau1"); 
                
        // -----------------
        // Create a new particle. 
        // The clone() link the created particle to it's source, so they share the dynamic attributes.
        Particle *selJet = jet->clone();
        
        // -----------------
        // This will internally access jet's Tau1 since selJet is a fresh object
        float clone_tau1= selJet->get<float>("Tau1");
        if( tau1 != clone_tau1) std::cout << "Error !"<< std::endl;
        // Set a value to the new particle. This Does NOT change the source (jet)
        selJet->set<float>("Tau1", clone_tau1*2.);
        // Next call to selJet->get<float>("Tau1") will return the modified value.

        // -----------------
        // Associate a particle :
        selJet->set<Particle*>("closestNeighb", selJet); // (stupid example...)

        // -----------------
        // This is a LorentzVector, we can use kinematics operations :
        (*selJet) *= 1.2; // best calibration ever.
        
        // -----------------
        // Now add the selJet to the output collection (this will do things smartly, including copying all 
        // necassary attributes from input collection to the output collection).
        //  NOTE : myjets OWNS its content: it will automatically delete what's needed when needed. Don't delete selJet yourself
        myjets.push_back(selJet); 

        // -----------------
        // Access the constituents. This implies the source container was declared, see above.
        ParticleContainer constit;
        jet->fillConstituents(constit);
        // constit is now filled ...
      }


      // -----------------
      // An efficient filtering mechanism.
      // Build a selector and add selection criteria
      ParticleSelector sel;
      sel.addAtomicSel(SelectPt< Above >( 10000 ) ); 
      sel.addAtomicSel(SelectEta< Below >(0.3) );
      sel.addAtomicSel(SelectAttribute< float, Above >("Tau1",0.2) );
      // (equivalent to pt>1000 && eta<0.3 && Tau1>0.2)
      // do the selection :
      sel.filter(jetCont, myjets); // by default, this also clears myjets before filling it.      

    }
    
    
    cout << "--------------------------------"<<endl;

    //-------------------
    // As we declared branches to be written out, make sure they have been read 
    // before filling the output tree. 
    jetCont.readAllActive(); 
    // Fill the output Tree
    otree->Fill();
  }


  file->Close();
  delete file;

  otree->Write();
  ofile->Close();
  
}


// ****************************************************
// f2 performs the same operations as f1.
// The difference is the use of optimized features to avoid string lookups.
void f2() {
  
  
  // Define one global variable for event index.
  Long64_t eventIndex = 0;

  // define an EventObject (this is a D3PDMakerReader object)
  EventObject event( eventIndex );
  // Define a ParticleContainer, give it the prefix according to the D3PD branches name.
  const ParticleContainer jetCont("jet_AntiKt10LCTopo_",eventIndex);

  // One can also define a simple variable
  VarHandle< std::vector<float>  > muon_pt( "mu_pt",
                                            &eventIndex); 


  
  // Prepare an input file
  TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root" );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );
  if( ! tree ) {
    std::cerr << "Couldn't find TTree!" << std::endl;
    return ;
  }
  
  
  // Connect the JetAnalysisEDM objects to this new TTree:
  event.readFrom( tree );
  jetCont.readFrom( tree);
  muon_pt.readFrom( tree );

  
  // Write out some of the above
  // Open an output file:
  TFile* ofile = TFile::Open( "output_test.root", "RECREATE" );
  ofile->cd();
  
  // Create an output tree:
  TTree* otree = new TTree( "bla", "Selected MuctpiD3PD tree" );
  otree->SetAutoSave( 100000000 );
  TTree::SetBranchStyle( 1 );
  otree->SetDirectory( ofile );

  
  // Write out some of the input container content
  jetCont.setOutputVar(true, "^c.*"); // This selects only the variables starting with 'c'
  // declare which tree to write in. This automatically selects the kinematic vars to be written out (eta,pt,phi,E) 
  jetCont.writeTo( otree );    

  /// Create a new container.
  ParticleContainer myjets( "myjets_", eventIndex);
  // declare it as a subset of a container. 
  // This will allow to use optimized path internally, but it will refuse to add Particle not from jetCont
  myjets.setSubsetOfInputContainer( &jetCont ); 
  // Don't link it to the input file. We will build and fill it from scratch
  // We want to save it. As it is entirely new, we must declare in advance wich branch we want to save
  myjets.declareOutputVar<float>("Tau2"); // declare 1 branch.
  myjets.declareOutputVarAsIn(&jetCont, "^c.*"); // declare the same branch as in jetCont which start with 'c'
  // declare which tree to write in.
  myjets.writeTo( otree);



  // For efficient access to dynamic attribute, we can define an accessor.
  ParticleAccessor<float> jet_tau2  = jetCont.accessor<float>("Tau2");
  // If we want a modifiable accessor :
  NonConstParticleAccessor<float> myjet_tau2  = myjets.accessor_nonconst<float>("Tau2");
  
  for( eventIndex = 0; eventIndex < tree->GetEntries(); ++eventIndex ) {
    std::cout << "Event  "<< eventIndex<< "  N jet = "<< jetCont.size()  << std::endl;
    for(size_t i=0; i< jetCont.size(); i++){
      // retrieve particle.
      const Particle * jet = jetCont[i];
      if( jet->pt() > 20000 ){
        // access dynamic attributes :
        // this is faster than get<float>() (no string lookup, direct array access)
        float tau2 = jet_tau2[jet] ; 


        // Create a new particle. 
        // Ask, myjets to create a new one from our source 'jet' and give back a pointer to it.
        Particle *selJet = myjets.addNewFromSource(jet);
        
        // change the tau2 
        myjet_tau2[selJet] = tau2;

        // This is a LorentzVector, we can use kinematics operations :
        (*selJet) *= 1.2; // best calibration ever.
        
        // selJet is already part of myjets.
      }

    }       
    cout << "--------------------------------"<<endl;

    otree->Fill();
    
    

  }

  file->Close();
  delete file;

  otree->Write();
  ofile->Close();
  
}



int main(int argc, char* argv[]){
  if(argc==1) f1();
  else f2();
    

}
