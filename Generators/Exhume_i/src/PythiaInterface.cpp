/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "Exhume_i/PythiaInterface.h"
#include "Exhume_i/PythiaRecord.h"

extern"C"{
  void my_pythia_init_();
}

////////////////////////////////////////////////////////////////////////////////
Exhume::PythiaInterface::PythiaInterface(CrossSection& _xsecn){
  xsecn = &_xsecn;
  StableHeavyObject=false;
  //initialise pythia and allow resonance decays.
  my_pythia_init_();
  pyinre_();
  std::string no_string;
  SuperSymmetry(no_string);//sets all default supersymmetry
  //SetParticleMasses as defined in CrossSection (Exhume class)
  //Think about changing the input for CrossSection.
  pydat2_.pmas[0][24] = xsecn->GetHiggsMass();
  pydat2_.pmas[0][22] = xsecn->GetZMass();
  pydat2_.pmas[0][23] = xsecn->GetWMass();
  pydat2_.pmas[0][5]  = xsecn->GetTopMass();
  pydat2_.pmas[0][12] = xsecn->GetMuonMass();
  pydat2_.pmas[0][14] = xsecn->GetTauMass();
  pydat2_.pmas[0][4]  = xsecn->GetBottomMass();
  pydat2_.pmas[0][3]  = xsecn->GetCharmMass();
  pydat2_.pmas[0][2]  = xsecn->GetStrangeMass();
  pydat2_.pmas[0][1]  = 0.0;
  pydat2_.pmas[0][0]  = 0.0;

  NumberSubProcessParticles = 0;
}

Exhume::PythiaInterface::~PythiaInterface(){
}

void Exhume::PythiaInterface::SuperSymmetry(std::string parameters_){
					    
////					    = "default"){
  pymssm_.imss[1]=1;//allow MSSM normal mode

  if(!parameters_.size()==0){//read in some SUSY parameters
    std::cout<<"  Pythia Supersymmetry Updated Using File "<<parameters_<<
      std::endl<<std::endl;
    std::cout<<"\tReading Pythia Values......"<<std::endl<<std::endl;
    std::fstream input_;
    input_.open(parameters_.c_str(),std::ios::in);
    if(input_.is_open()){
      while(! input_.eof()){
	std::string block_;	int array_position_,i_value_;
	double d_value_;
	input_>>block_;
	input_>>array_position_;
	

	if( block_ == "imss" && array_position_ >= 0 && array_position_ < 100 ){
	  input_>>i_value_;
	  pymssm_.imss[array_position_]=i_value_;
	  std::cout<<"\timss\t"<<array_position_
		   <<"\t"<<pymssm_.imss[array_position_]<<std::endl;
	}
	else if( block_ == "rmss" && array_position_ >= 0 && array_position_ < 100 ){
	  input_>>d_value_;
	  pymssm_.rmss[array_position_]=d_value_;
	  std::cout<<"\trmss\t"<<array_position_
		   <<"\t"<<pymssm_.rmss[array_position_]<<std::endl;
	}
	else{
	  std::cerr<<"  Unknown SUSY Parameter in File"<<std::endl;
	}
      }
      input_.close();
    }
    else{
      std::cout<<"  Error opening SUSY FILE"<<std::endl<<std::endl;
    }

  }
  else{
    std::cout<<"  Pythia SuperSymmetry Set Up Using Default Values"<<std::endl
	     <<std::endl;;
  }
 
  //redo all widths.
  pymsin_();
  pyinre_();//not sure if this needs to be done again....and in which order??
  return;
}


void Exhume::PythiaInterface::Hadronise(){

  InsertProtons();
  SetPartonRecord();
  InsertPartons();
  switch(NumberSubProcessParticles){
  case 1:
    SingleResonance();
    break;
  case 2:
    TwoParticles();
    break;
  case 3:
    ThreeParticles();
    break;
  default:
    std::cout<<"To many subprocess particles. Email the authors"<<std::endl;
    break;

  }
  int one=1;
  pyhepc_(one);
  return;
}

void Exhume::PythiaInterface::InsertProtons(){
  Exhume::Particle p1 = xsecn->GetProton1();
  Exhume::Particle p2 = xsecn->GetProton2();
  double e_,theta_,phi_;
  int nn;
  //for now.....until later
  int p1Id=p1.id;
  int p2Id=p2.id;
  //
  //Put in the protons that interact
  /*
  e_ = 7000.0;
  phi_= 0.0;
  theta_ = 0.0;
  nn=1;
  py1ent_(nn, p1Id, e_, theta_, phi_);
  pyjets_.k[0][nn-1]=1;

  theta_ = PI;
  nn=2;
  py1ent_(nn, p1Id, e_, theta_, phi_);
  pyjets_.k[0][nn-1]=1;
*/
  //And the outgoing ones;
  e_ = p1.p.e();
  theta_ = p1.p.theta();
  phi_ = p1.p.phi();
  nn =1;
  py1ent_(nn, p1Id, e_, theta_, phi_);
  pyjets_.k[0][nn-1]=1;
  

  e_ = p2.p.e();
  theta_ = p2.p.theta();
  phi_ = p2.p.phi();
  nn = 2;
  py1ent_(nn, p2Id, e_, theta_, phi_);
  pyjets_.k[0][nn-1]=1;
  return;
}

void Exhume::PythiaInterface::InsertPartons(){
  double e_, px_, py_, pz_, mm_;
  int id_;
  int nn_,nnc_;
  nn_ = 2; //Protons already in on lines 1,2 
  nnc_ = nn_ - 1;//C++ array indexing;
  for(std::vector< Exhume::Particle >::iterator itr = SubProcessRecord.begin();
      itr!=SubProcessRecord.end();itr++){
    nn_++;//every time input->increase line number;
    nnc_++;
    e_ = itr->p.e();
    px_ = itr->p.px();
    py_ = itr->p.py();
    pz_ = itr->p.pz();
    mm_ = itr->p.m();

    id_ = itr->id;

    for( int j=0;j<5;j++){
      pyjets_.k[j][nnc_]=0;
      pyjets_.p[j][nnc_]=0.0;
      pyjets_.v[j][nnc_]=0.0;
    }

    pyjets_.k[0][nnc_]=1; //states there is an undecayed particle/parton,
    pyjets_.k[1][nnc_]=id_;//sets the PDG type

    pyjets_.p[0][nnc_]=px_;
    pyjets_.p[1][nnc_]=py_;
    pyjets_.p[2][nnc_]=pz_;
    pyjets_.p[3][nnc_]=e_;
    pyjets_.p[4][nnc_]=mm_;
    
    pyjets_.n = nn_;
    
  
  }
  return;
}

void Exhume::PythiaInterface::SingleResonance(){
  int _ResonanceLine=3;
  pyresd_(_ResonanceLine);
  pyexec_();
  return;

}

void Exhume::PythiaInterface::TwoParticles(){
  bool _Coloured=true;//most probable in exclusive diffraction
  if(SubProcessRecord[0].Colour==0 && SubProcessRecord[0].AntiColour==0 &&
     SubProcessRecord[1].Colour==0 && SubProcessRecord[1].AntiColour==0){
    _Coloured=false;
  }
  if(_Coloured){
    //Assume pair
    int _NJoin=2;
    int* _IJoin = (int*) calloc(_NJoin, sizeof(int) );
    _IJoin[0]=3;
    _IJoin[1]=4;
    //pyjoin_(_NJoin,_IJoin);
    //string set
    //std::cout<<"3\t"<<pyjets_.k[3][2]<<"\t"<<pyjets_.k[4][2]<<"\t"<<
    //  "4\t"<<pyjets_.k[3][3]<<"\t"<<pyjets_.k[4][3]<<std::endl;
    //this line assumes that things are pair produced i.e no baryon non-cons
    pyjoin_(_NJoin,_IJoin);
    if((abs(SubProcessRecord[0].id) >= 0 && abs(SubProcessRecord[0].id) <= 5) 
       || abs(SubProcessRecord[0].id)==21){
      //pyjoin_(_NJoin,_IJoin);
      int _four=4;
      int _three=3;
      double _Qmax= xsecn->GetSqrtsHat();
      pyshow_(_three,_four,_Qmax);
    //then showered if low mass object
    }
    else if(!StableHeavyObject){
      int _three=3;
      int _four=4;
      pyresd_(_three);
      pyresd_(_four);
      
    }
  }
  pyexec_();
  return;

}

void Exhume::PythiaInterface::ThreeParticles(){
  //will be updated when 3 particle f.s is ready as untestable until then
  return;
}

void Exhume::PythiaInterface::SetPartonRecord(){
  SubProcessRecord=xsecn->GetPartons();
  NumberSubProcessParticles=SubProcessRecord.size();
  return;
}

double Exhume::PythiaInterface::SetHiggsDecay(int _id){
  
  for(int _ii=209;_ii<226;_ii++){
    pydat3_.mdme[0][_ii]=0;//set all decays to off
  }
  
  double BR;

  switch(_id){
  case 3:
    //strange
    pydat3_.mdme[0][211]=1;
    BR=1;
    break;
  case 4: 
    //charm
    pydat3_.mdme[0][212]=1;
    BR=1;
    break;
  case 5: 
    //bottom
    pydat3_.mdme[0][213]=1;
    BR=1;
    break;
  case 6: 
    //top
    pydat3_.mdme[0][214]=1;
    BR=1;
    break;
  case 13: 
    //muon
    pydat3_.mdme[0][218]=1;
    BR=1;
    break;
  case 15: 
    //tau
    pydat3_.mdme[0][219]=1;
    BR=1;
    break;
  case 21:
    //gluon
    pydat3_.mdme[0][221]=1;
    BR=1;
    break;
  case 22: 
    //photon
    pydat3_.mdme[0][222]=1;
    BR=1;
    break;
  case 23: 
    //Z
    pydat3_.mdme[0][224]=1;
    BR=1;
    break;
  case 24: 
    //W
    pydat3_.mdme[0][225]=1;
    BR=1;
    break;
    
  default:
    std::cout<<"   Unrecognised PDG code for Higgs Decay"<<std::endl;
    for(int alldecay = 209; alldecay<226; alldecay++){
      pydat3_.mdme[0][alldecay] = 1;      
    }
    BR=1;
    break;
    
  }

  return(BR);
}

double Exhume::PythiaInterface::SetWDecay(int _id){
  double BR=1;
  //to avoid compiler warnings
  _id = _id;
  
  return(BR);
  
}

void Exhume::PythiaInterface::SetTauDecay(bool _x){
  if(_x){
    pydat3_.mdcy[0][14]=1;
  }
  else{
    pydat3_.mdcy[0][14]=0;
  }
  return;
}
void Exhume::PythiaInterface::SetQEDRadiationFromLeptons(double _x){
  pydat1_.parj[89]=_x;
  return;
}
