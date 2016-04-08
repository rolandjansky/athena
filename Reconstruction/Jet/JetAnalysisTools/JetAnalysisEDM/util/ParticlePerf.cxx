/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <vector>
#include "JetAnalysisEDM/Particle.h"
#include "JetAnalysisEDM/EventObject.h"
#include "JetAnalysisEDM/VarHandle.h"

#include "TFile.h"
#include "TStopwatch.h"



using namespace std;
using namespace JetAnalysisEDM;

#define MACRO_EACH_TYPE( MAC ) MAC( PullMag )  MAC( PullPhi )  MAC( Pull_C00)  MAC( Pull_C01)  MAC( Pull_C10)  MAC( Pull_C11)  MAC( QW )  MAC( SPLIT12 )  MAC( SPLIT23 )  MAC( SPLIT34 )  MAC( ZCUT12 )  MAC( ZCUT23 )  MAC( ZCUT34 )  MAC( jvtx_x)  MAC( jvtx_y)  MAC( jvtx_z)  MAC( jvtxf)  MAC( Tau1 )  MAC( Tau2 )  MAC( emscale_pt)  MAC( emscale_eta)  MAC( emscale_phi)  MAC( emscale_E)  MAC( constscale_pt)  MAC( constscale_eta)  MAC( constscale_phi)  MAC( constscale_E)  MAC( OriginIndex )  MAC( nTrk )  MAC( sumPtTrk )  MAC( EtaOrigin )  MAC( PhiOrigin )  MAC( MOrigin )  MAC( LCJES )  MAC( LCJES_EtaCorr )  MAC( LCJES_MassCorr )

#define MACRO_EACH_TYPE_H1( MAC ) MAC( PullMag )  MAC( PullPhi )  MAC( Pull_C00)  MAC( Pull_C01)  MAC( Pull_C10)  MAC( Pull_C11)  MAC( QW )  MAC( SPLIT12 )  MAC( SPLIT23 )  MAC( SPLIT34 )  MAC( ZCUT12 )  MAC( ZCUT23 )  MAC( ZCUT34 )  MAC( jvtx_x)  MAC( jvtx_y)  MAC( jvtx_z)  MAC( jvtxf)  MAC( Tau1 )  MAC( Tau2 )

#define MACRO_EACH_TYPE_H2( MAC )  MAC( emscale_pt)  MAC( emscale_eta)  MAC( emscale_phi)  MAC( emscale_E)  MAC( constscale_pt)  MAC( constscale_eta)  MAC( constscale_phi)  MAC( constscale_E)  MAC( OriginIndex )  MAC( nTrk )  MAC( sumPtTrk )  MAC( EtaOrigin )  MAC( PhiOrigin )  MAC( MOrigin )  MAC( LCJES )  MAC( LCJES_EtaCorr )  MAC( LCJES_MassCorr )




int main(int argc, char *argv[]) {
  
  cout << argc << endl;
  // Define one global variable for event index.
  Long64_t eventIndex = 0;

  // define an EventObject (this is a D3PDMakerReader object)
  EventObject event( eventIndex );
  // Define a ParticleContainer, give it the prefix according to the D3PD branches name.
  ParticleContainer jetCont("jet_AntiKt10LCTopo_",eventIndex);

  // One can also define a simple variable
  TObject dummyObj; // this is required for implementation details (unnecessary, will be removed)
  // VarHandle< std::vector<float>  > jet_emscale_pt( "jet_AntiKt10LCTopo_emscale_pt",
  //                                                 &eventIndex); 

  
  // Prepare an input file
  TFile* file = TFile::Open("/home/delsart/analysis/topAnalisis/files/NTUP_TOP.01082946._000001_only100.root.1" );
  // Access the D3PD TTree inside of it:
  TTree* tree = dynamic_cast< TTree* >( file->Get( "physics" ) );
  if( ! tree ) {
    std::cerr << "Couldn't find TTree!" << std::endl;
    return 1;
  }
  
  
  // Connect the JetAnalysisEDM objects to this new TTree:
  event.readFrom( tree );
  jetCont.readFrom( tree);

#define declareVar( vname ) VarHandle< std::vector<float>  > jet_##vname( "jet_AntiKt10LCTopo_"#vname, &eventIndex); jet_##vname.readFrom( tree) ;
#define declareAcc( vname ) ParticleAccessor<float> jet_acc_##vname = jetCont.accessor<float>(#vname);

  MACRO_EACH_TYPE( declareVar );
  MACRO_EACH_TYPE_H2( declareAcc );

  
  TStopwatch w;
  w.Start();
  float gt=0;
  int njet=0;
  for( eventIndex = 0; eventIndex < tree->GetEntries(); ++eventIndex ) {
    float ts = 0;
    if(argc==1){
#define readJetVar( vname ) jet_##vname();
      MACRO_EACH_TYPE( readJetVar) ;


      for(int rr=0;rr<5; rr++)
        for(size_t i=0; i< jetCont.size(); i++){
          njet++;

          Particle * jet = jetCont[i];
          float s=jet->Px();



#define sumJetVar( vname ) s+= jet_##vname()[i];
#define sumJetVarDir( vname ) s+= jet_##vname.uncheckedAccess()[i];
          MACRO_EACH_TYPE( sumJetVarDir );
          
          ts +=s;      
        }
    }else{

#define readJetAcc( vname) jet_acc_##vname.readEvent();
      MACRO_EACH_TYPE_H2( readJetAcc) ;

      for(int rr=0;rr<5; rr++)
        for(size_t i=0; i< jetCont.size(); i++){
          Particle * jet = jetCont[i];
          float s=jet->Px();
          njet++;
#define sumJetGetVar( vname ) s+= jet->get<float>( #vname );
#define sumJetAcc( vname ) s+= jet_acc_##vname[ jet ];
#define sumJetAccUnsafe( vname ) s+= jet_acc_##vname.unsafeAccess( *jet );
          //MACRO_EACH_TYPE( sumJetGetVar);
          MACRO_EACH_TYPE_H1( sumJetGetVar);
          MACRO_EACH_TYPE_H2( sumJetAcc);
          //MACRO_EACH_TYPE_H2( sumJetAccUnsafe);

          ts +=s;                

        }
    }
    //cout << ts << endl;
    gt+=ts;
    
      //cout << "--------------------------------"<<endl;        
    

  }
  w.Stop();
  
  if(argc==1) cout <<" Direct access "<< gt<<endl;
  else cout <<" get<>() access "<< endl;
  cout << eventIndex <<" njet "<< njet<< " Total time = "<< w.RealTime() << "  _ "<< w.RealTime()/tree->GetEntries() << endl;
  file->Close();
  delete file;
  
}
