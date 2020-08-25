#define DRM_cxx

#include "DRM.h"
#include "DRM_Transf.cxx"
#include "TLorentzRotation.h"
#include "TLorentzVector.h"
#include "TVector3.h"

int main(int argc, char*argv[]){

  if(argc<3){
    std::cout<<" DRM: merge Powheg and Prophecy LHE files! "<<std::endl;
    std::cout<<" Usage:  ./DRM [Options]"<<std::endl;
    std::cout<<"         Nedeed options:"<<std::endl;
    std::cout<<"               --inPowheg [Powheg lhe file]"<<std::endl;
    std::cout<<"               --inProphecy [Profecy4f lhe file]"<<std::endl;
    std::cout<<"               --outLHE [Outupt lhe file]"<<std::endl;
    std::cout<<"         Additional options:"<<std::endl;
    std::cout<<"               --debug [Enable debug mode]"<<std::endl;
    exit(0);
  }
  std::string Powheg, Prophecy, Out;
  bool Debug=false;
  for(int a=1; a<argc; a++){
    if(!strcmp(argv[a],"--inPowheg")){
      Powheg = argv[a+1];
    }
    else if(!strcmp(argv[a],"--inProphecy")){
      Prophecy = argv[a+1];
    }
    else if(!strcmp(argv[a],"--outLHE")){
      Out = argv[a+1];
    }
    else if(!strcmp(argv[a],"--debug")){
      Debug=true;
    }
  }

  DRM Run;
  Run.SetIO(Powheg,Prophecy,Out,Debug);
  Run.Merge();

  return 0;
  
}

DRM::DRM() : m_PHEvent(false), m_Debug(false) { 
}

DRM::~DRM(){
}


void DRM::SetIO(std::string powheg,
		std::string prophecy,
		std::string outlhe,
		bool debug){
  
  m_InPowheg   = powheg;
  m_InProphecy = prophecy;
  m_OutLHE     = outlhe;
  m_PHEvent    = false;
  m_Debug      = debug;
  
}

void DRM::Merge(){

  Print(" Opening Powheg LHE file ... ");
  if( !FileExists(m_InPowheg) ){
    std::cerr << "Input Powheg LHE not Found!"
	      << " Aborting ... " << std::endl;
    return;
  }
  LHEF::Reader ReadH( m_InPowheg.c_str() );
  
  Print(" Opening Profecy4f LHE file ... ");
  if( !FileExists(m_InProphecy) ){
    std::cerr << "Input Prophecy4f LHE not Found!"
	      << " Aborting ... " << std::endl;
    return;
  }
  LHEF::Reader Read4f( m_InProphecy.c_str() );
  
  Print(" Opening Out LHE file ... ");
  LHEF::Writer WriteLHE( m_OutLHE.c_str() );
  
  WriteLHE.headerBlock() << ReadH.headerBlock;
  WriteLHE.initComments() << ReadH.initComments;
  WriteLHE.heprup = ReadH.heprup;
  WriteLHE.init();

  long neve = 0;
  
  /*Read each event and write them out again*/
  while( ReadH.readEvent() ){
    if( !Read4f.readEvent() ){
      std::cout << "Still Powheg events but no more Prophecy4f"
		<< "events! Wrting out LHE file ... " << std::endl;
      break;
    }
    
    ++neve;
    if(neve%1000==0){
      Print("Events processed so far ",neve);
    }
    if( ReadH.outsideBlock.length() )
      std::cout << ReadH.outsideBlock;
    
    int nup_org=ReadH.hepeup.NUP;

    for(int nup=0; nup<nup_org; nup++){
      if(ReadH.hepeup.idup(nup)==m_HiggsID)
	ReadH.hepeup.istup(nup)=2;
    }
    WriteLHE.hepeup = ReadH.hepeup;

    double P4_Z[5][2] = {{0}};
    double P4_l[5][5];
    long ID_l[4] = {-999, -999, -999, -999};
    double Pph[5] = {-999., -999., -999., -999., -999.};
    double daughter2[5][5];
    
    for(int i=0; i<nup_org; i++){
      if(ReadH.hepeup.idup(i)==m_HiggsID) {
	TLorentzVector Higgs( ReadH.hepeup.pup(i,0),
			      ReadH.hepeup.pup(i,1),
			      ReadH.hepeup.pup(i,2),
			      ReadH.hepeup.pup(i,3) );
	
	TVector3 boostvec=Higgs.BoostVector();
	TLorentzRotation boost(-boostvec);
	TLorentzRotation backboost(boostvec);
	
	/* boost to Higgs rest frame */
	TLorentzVector r_Higgs(Higgs);
	r_Higgs*=boost;
	
	/* Here add daughter particles to Higgs in rest
	   boost back with backboost */
	WriteLHE.hepeup.resize(nup_org+Read4f.hepeup.NUP);
	
	TLorentzVector sum_daughter;
	TLorentzVector sum_daughter_rest;
	TLorentzVector sum_daughter_rest_init;
	
	for(int k=0; k<Read4f.hepeup.NUP; k++){
	  TLorentzVector daughter(Read4f.hepeup.pup(k,0),
				  Read4f.hepeup.pup(k,1),
				  Read4f.hepeup.pup(k,2),
				  Read4f.hepeup.pup(k,3));
	  sum_daughter_rest_init+=daughter;
	}
	
	for(int j=0; j<Read4f.hepeup.NUP; j++){
	  TLorentzVector daughter(Read4f.hepeup.pup(j,0),
				  Read4f.hepeup.pup(j,1),
				  Read4f.hepeup.pup(j,2),
				  Read4f.hepeup.pup(j,3));
	  sum_daughter_rest+=daughter;
	  
	  daughter*=backboost;
	  daughter*=Higgs.M()/sum_daughter_rest_init.M();
	  
	  /* POWHEG event with Higgs off-mass shell */
	  m_PHEvent=IsPHevent(Higgs,sum_daughter_rest_init);
	  if( m_PHEvent ){
	    break;
	  }
	  
	  sum_daughter+=daughter;
	  
	  if( std::abs(Read4f.hepeup.idup(j))==m_ElectronID ||
	      std::abs(Read4f.hepeup.idup(j))==m_MuonID      ){
	    
	    ID_l[j]    = Read4f.hepeup.idup(j);
	    P4_l[0][j] = daughter.Px();
	    P4_l[1][j] = daughter.Py();
	    P4_l[2][j] = daughter.Pz();
	    P4_l[3][j] = daughter.E();
	    
	  }		
	  else if( std::abs(Read4f.hepeup.idup(j))==m_PhotonID ){
	    Pph[0] = daughter.Px();
	    Pph[1] = daughter.Py();
	    Pph[2] = daughter.Pz();
	    Pph[3] = daughter.E();
	    Pph[4] =0.0;
	  }
	}			
	
	for(int n=0; n<4; n++){
	  if( std::abs(ID_l[n])==m_ElectronID ||
	      std::abs(ID_l[n])==m_MuonID  ) {
	    
	    P4_Z[n][0] = P4_l[n][0] + P4_l[n][1];
	    P4_Z[n][1] = P4_l[n][2] + P4_l[n][3];
	  }
	}
	
	double mass1, mass2;
	P4_Z[4][0] = std::sqrt(P4_Z[3][0] * P4_Z[3][0] - P4_Z[0][0] * P4_Z[0][0] - P4_Z[1][0] * P4_Z[1][0] - P4_Z[2][0] * P4_Z[2][0]);
	P4_Z[4][1] = std::sqrt(P4_Z[3][1] * P4_Z[3][1] - P4_Z[0][1] * P4_Z[0][1] - P4_Z[1][1] * P4_Z[1][1] - P4_Z[2][1] * P4_Z[2][1]);
	mass1=SetParticleMass(ID_l).first;
	mass2=SetParticleMass(ID_l).second;
	
	double p1[5], p2[5], pZ1[5], p3[5], p4[5], pZ2[5];
	for(int u=0; u<5; u++){
	  
	  pZ1[u] = P4_Z[u][0];
	  p1[u]  = P4_l[u][0];
	  p2[u]  = P4_l[u][1];
	  pZ2[u] = P4_Z[u][1];
	  p3[u]  = P4_l[u][2];
	  p4[u]  = P4_l[u][3];
	  
	}
	
	p1[4] = 0.0;
	p2[4] = 0.0;
	p3[4] = 0.0;
	p4[4] = 0.0;
	
	/* Give masses to the leptons using Alpgen/FHerwig routines */
	rescms(pZ1, p1, p2, mass1, mass1);
	rescms(pZ2, p3, p4, mass2, mass2);

	for(int d =0; d<5; d++){
	  daughter2[0][d] = p1[d];
	  daughter2[1][d] = p2[d];
	  daughter2[2][d] = p3[d];
	  daughter2[3][d] = p4[d];
	  daughter2[4][d] = Pph[d];
	}
	break;
      }
    }

    /* Write LHE */
    if(m_PHEvent){
      WriteLHE.hepeup = ReadH.hepeup;
    }
    else{
      
      for(int jp=0; jp<Read4f.hepeup.NUP; jp++){
	
        WriteLHE.hepeup.idup(nup_org+jp) =Read4f.hepeup.idup(jp);
        WriteLHE.hepeup.istup(nup_org+jp)=Read4f.hepeup.istup(jp);
        if( Read4f.hepeup.icolup1(jp)==0 &&
	    Read4f.hepeup.icolup2(jp)==0  ) {
          WriteLHE.hepeup.icolup1(nup_org+jp)=0;
          WriteLHE.hepeup.icolup2(nup_org+jp)=0;
        }
	else {
          WriteLHE.hepeup.icolup1(nup_org+jp)=Read4f.hepeup.icolup1(jp)+100;
          WriteLHE.hepeup.icolup2(nup_org+jp)=Read4f.hepeup.icolup2(jp)+100;
        }  
        WriteLHE.hepeup.mothup1(nup_org+jp)=3;//i-1;
	WriteLHE.hepeup.mothup2(nup_org+jp)=3;//i-1;
        WriteLHE.hepeup.pup(nup_org+jp,0)=daughter2[jp][0];
        WriteLHE.hepeup.pup(nup_org+jp,1)=daughter2[jp][1];
        WriteLHE.hepeup.pup(nup_org+jp,2)=daughter2[jp][2];
        WriteLHE.hepeup.pup(nup_org+jp,3)=daughter2[jp][3];
        WriteLHE.hepeup.pup(nup_org+jp,4)=daughter2[jp][4];
        WriteLHE.hepeup.vtimup(nup_org+jp)=Read4f.hepeup.vtimup(jp);
        WriteLHE.hepeup.spinup(nup_org+jp)=Read4f.hepeup.spinup(jp);
      }
    }
    WriteLHE.eventComments() << ReadH.eventComments;
    WriteLHE.writeEvent();
  }
}

bool DRM::IsPHevent(TLorentzVector higgs,
		    TLorentzVector sum_daugh_rest_init){

  if( std::abs(higgs.M()-sum_daugh_rest_init.M())>m_DeltaM ){
    std::cout << "POWHEG event with Higgs off-mass shell: "
	      << higgs.M() << " GeV" <<std::endl;
    return true;
  }
  return false;
  
}

std::pair<double,double> DRM::SetParticleMass(long *ID){
  
  double mass1=0.0, mass2=0.0;
  std::pair<double,double> Pmass;

  int NP=4;
  for(int idx=0; idx<NP; idx++){
    long id = ID[idx];
    if( std::abs(id)==m_NeutrinoEl ||
	std::abs(id)==m_NeutrinoMu ||
	std::abs(id)==m_NeutrinoTau ){
      mass1 = 0.0;
      mass2 = 0.0;
    }
    else if( std::abs(id)==m_ElectronID ){
      if(idx==0) mass1 = m_ElectronMass;
      if(idx==2) mass2 = m_ElectronMass;
    }
    else if( std::abs(id)==m_MuonID ){
      if(idx==0) mass1 = m_MuonMass;
      if(idx==2) mass2 = m_MuonMass;
    }
    else if( std::abs(id)==m_TauID ){
      if (idx==0) mass1 = m_TauMass;
      if (idx==2) mass2 = m_TauMass;
    }
    else {
      mass1 = 0.0;
      mass2 = 0.0;
    }
  }
  Pmass.first=mass1;
  Pmass.second=mass2;
  
  return Pmass;
  
}

bool DRM::FileExists(std::string filename){
  
  std::ifstream ifile(filename.c_str());
  if( ifile.good() ){
    ifile.close();
    return true;
  }
  ifile.close();
  return false;

}

void DRM::Print(std::string field){
  if(m_Debug){
    std::cout<<"DEBUG:: "+field<<std::endl;
  }
}

void DRM::Print(std::string field,
		int value){

  if(m_Debug){
    std::cout<<"DEBUG:: "<<value<<" "+field<<std::endl;
  }
  
}
