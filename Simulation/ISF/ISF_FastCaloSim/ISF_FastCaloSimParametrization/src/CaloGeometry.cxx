/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloGeometry.h"
#include <TTree.h>
#include <TVector2.h>
#include <TRandom.h>
#include "CaloDetDescr/CaloDetDescrElement.h"
//#include "TMVA/Tools.h"
//#include "TMVA/Factory.h"

using namespace std;

const int CaloGeometry::MAX_SAMPLING =24; //number of calorimeter layers/samplings

Identifier CaloGeometry::m_debug_identify;
bool CaloGeometry::m_debug=false;

CaloGeometryLookup::CaloGeometryLookup(int ind):m_xy_grid_adjustment_factor(0.75),m_index(ind)
{
  m_mineta=+10000;
  m_maxeta=-10000;
  m_minphi=+10000;
  m_maxphi=-10000;

  m_mineta_raw=+10000;
  m_maxeta_raw=-10000;
  m_minphi_raw=+10000;
  m_maxphi_raw=-10000;

  m_mindeta=10000;
  m_mindphi=10000;
  
  m_mineta_correction=+10000;
  m_maxeta_correction=-10000;
  m_minphi_correction=+10000;
  m_maxphi_correction=-10000;
}

CaloGeometryLookup::~CaloGeometryLookup()
{
}

bool CaloGeometryLookup::has_overlap(CaloGeometryLookup* ref)
{
  if(m_cells.size()==0) return false;
  for(t_cellmap::iterator ic=m_cells.begin();ic!=m_cells.end();++ic) {
    CaloDetDescrElement* cell=ic->second;
    if(ref->IsCompatible(cell)) return true;
  }
  return false;
}

void CaloGeometryLookup::merge_into_ref(CaloGeometryLookup* ref)
{
  for(t_cellmap::iterator ic=m_cells.begin();ic!=m_cells.end();++ic) {
    CaloDetDescrElement* cell=ic->second;
    ref->add(cell);
  }
}


bool CaloGeometryLookup::IsCompatible(CaloDetDescrElement* cell)
{
  if(m_cells.size()==0) return true;
  t_cellmap::iterator ic=m_cells.begin();
  CaloDetDescrElement* refcell=ic->second;
  int sampling=refcell->getSampling();
  if(cell->getSampling()!=sampling) return false;
  if(cell->eta_raw()*refcell->eta_raw()<0) return false;
  if(sampling<21) { // keep only cells reasonable close in eta to each other
    if(TMath::Abs(cell->deta()-refcell->deta())>0.001) return false;
    if(TMath::Abs(cell->dphi()-refcell->dphi())>0.001) return false;
    
    if( cell->eta()<mineta()-2.1*cell->deta() ) return false;  
    if( cell->eta()>maxeta()+2.1*cell->deta() ) return false;  

    double delta_eta=(cell->eta_raw()-refcell->eta_raw())/cell->deta();
    double delta_phi=(cell->phi_raw()-refcell->phi_raw())/cell->dphi();
    if(TMath::Abs(delta_eta-TMath::Nint(delta_eta)) > 0.01) return false;
    if(TMath::Abs(delta_phi-TMath::Nint(delta_phi)) > 0.01) return false;

    /*
    cout<<"Compatible: s="<<cell->getSampling()<<"~"<<refcell->getSampling()<<"; ";
    cout<<"eta="<<cell->eta_raw()<<","<<refcell->eta_raw()<<"; ";
    cout<<"phi="<<cell->phi_raw()<<","<<refcell->phi_raw()<<"; ";
    cout<<"deta="<<cell->deta()<<"~"<<refcell->deta()<<"; ";
    cout<<"dphi="<<cell->dphi()<<"~"<<refcell->dphi()<<";";
    cout<<"mineta="<<mineta()<<", maxeta="<<maxeta()<<"; ";
    cout<<endl;
    */
  } else {
    //FCal is not sufficiently regular to use submaps with regular mapping
    return true;
    /*
    if(TMath::Abs(cell->dx()-refcell->dx())>0.001) return false;
    if(TMath::Abs(cell->dy()-refcell->dy())>0.001) return false;
    if( cell->x()<minx()-20*cell->dx() ) return false;
    if( cell->x()>maxx()+20*cell->dx() ) return false;
    if( cell->y()<miny()-20*cell->dy() ) return false;
    if( cell->y()>maxy()+20*cell->dy() ) return false;

    double delta_x=(cell->x_raw()-refcell->x_raw())/cell->dx();
    double delta_y=(cell->y_raw()-refcell->y_raw())/cell->dy();
    if(TMath::Abs(delta_x-TMath::Nint(delta_x)) > 0.01) return false;
    if(TMath::Abs(delta_y-TMath::Nint(delta_y)) > 0.01) return false;
    */
  }  

  return true;  
}

void CaloGeometryLookup::add(CaloDetDescrElement* cell)
{
  if(cell->getSampling()<21) {
    m_deta.add(cell->deta());
    m_dphi.add(cell->dphi());
    m_mindeta=TMath::Min(cell->deta(),m_mindeta);
    m_mindphi=TMath::Min(cell->dphi(),m_mindphi);

    m_eta_correction.add(cell->eta_raw()-cell->eta());
    m_phi_correction.add(cell->phi_raw()-cell->phi());
    m_mineta_correction=TMath::Min(cell->eta_raw()-cell->eta() , m_mineta_correction);
    m_maxeta_correction=TMath::Max(cell->eta_raw()-cell->eta() , m_maxeta_correction);
    m_minphi_correction=TMath::Min(cell->phi_raw()-cell->phi() , m_minphi_correction);
    m_maxphi_correction=TMath::Max(cell->phi_raw()-cell->phi() , m_maxphi_correction);

    m_mineta=TMath::Min(cell->eta()-cell->deta()/2,m_mineta);
    m_maxeta=TMath::Max(cell->eta()+cell->deta()/2,m_maxeta);
    m_minphi=TMath::Min(cell->phi()-cell->dphi()/2,m_minphi);
    m_maxphi=TMath::Max(cell->phi()+cell->dphi()/2,m_maxphi);

    m_mineta_raw=TMath::Min(cell->eta_raw()-cell->deta()/2,m_mineta_raw);
    m_maxeta_raw=TMath::Max(cell->eta_raw()+cell->deta()/2,m_maxeta_raw);
    m_minphi_raw=TMath::Min(cell->phi_raw()-cell->dphi()/2,m_minphi_raw);
    m_maxphi_raw=TMath::Max(cell->phi_raw()+cell->dphi()/2,m_maxphi_raw);
  } else {
    m_deta.add(cell->dx());
    m_dphi.add(cell->dy());
    m_mindeta=TMath::Min(cell->dx(),m_mindeta);
    m_mindphi=TMath::Min(cell->dy(),m_mindphi);

    m_eta_correction.add(cell->x_raw()-cell->x());
    m_phi_correction.add(cell->y_raw()-cell->y());
    m_mineta_correction=TMath::Min(cell->x_raw()-cell->x() , m_mineta_correction);
    m_maxeta_correction=TMath::Max(cell->x_raw()-cell->x() , m_maxeta_correction);
    m_minphi_correction=TMath::Min(cell->y_raw()-cell->y() , m_minphi_correction);
    m_maxphi_correction=TMath::Max(cell->y_raw()-cell->y() , m_maxphi_correction);

    m_mineta=TMath::Min(cell->x()-cell->dx()/2,m_mineta);
    m_maxeta=TMath::Max(cell->x()+cell->dx()/2,m_maxeta);
    m_minphi=TMath::Min(cell->y()-cell->dy()/2,m_minphi);
    m_maxphi=TMath::Max(cell->y()+cell->dy()/2,m_maxphi);

    m_mineta_raw=TMath::Min(cell->x_raw()-cell->dx()/2,m_mineta_raw);
    m_maxeta_raw=TMath::Max(cell->x_raw()+cell->dx()/2,m_maxeta_raw);
    m_minphi_raw=TMath::Min(cell->y_raw()-cell->dy()/2,m_minphi_raw);
    m_maxphi_raw=TMath::Max(cell->y_raw()+cell->dy()/2,m_maxphi_raw);
  }  
  m_cells[cell->identify()]=cell;
}

bool CaloGeometryLookup::index_range_adjust(int& ieta,int& iphi)
{
  while(iphi<0) {iphi+=m_cell_grid_phi;};
  while(iphi>=m_cell_grid_phi) {iphi-=m_cell_grid_phi;};
  if(ieta<0) {
    ieta=0;
    return false;
  }
  if(ieta>=m_cell_grid_eta) {
    ieta=m_cell_grid_eta-1;
    return false;
  }
  return true;
}

void CaloGeometryLookup::post_process()
{
  if(size()==0) return;
  t_cellmap::iterator ic=m_cells.begin();
  CaloDetDescrElement* refcell=ic->second;
  int sampling=refcell->getSampling();
  if(sampling<21) {
    double rneta=neta_double()-neta();
    double rnphi=nphi_double()-nphi();
    if(TMath::Abs(rneta)>0.05 || TMath::Abs(rnphi)>0.05) {
      cout<<"ERROR: can't map cells into a regular grid for sampling "<<sampling<<", map index "<<index()<<endl;
      return;
    }
    m_cell_grid_eta=neta();
    m_cell_grid_phi=nphi();
    m_deta_double=deta();
    m_dphi_double=dphi();
  } else {
    double rnx=nx_double()-nx();
    double rny=ny_double()-ny();
    if(TMath::Abs(rnx)>0.05 || TMath::Abs(rny)>0.05) {
      cout<<"Grid: Sampling "<<sampling<<"_"<<index()<<": mapping cells into a regular grid, although cells don't fit well"<<endl;
    }

    m_cell_grid_eta=TMath::Nint(TMath::Ceil(nx_double()/m_xy_grid_adjustment_factor));
    m_cell_grid_phi=TMath::Nint(TMath::Ceil(ny_double()/m_xy_grid_adjustment_factor));
    m_deta_double=mindx()*m_xy_grid_adjustment_factor;
    m_dphi_double=mindy()*m_xy_grid_adjustment_factor;
  }
  
  m_cell_grid.resize(m_cell_grid_eta); 
  for(int ieta=0;ieta<m_cell_grid_eta;++ieta) {
    m_cell_grid[ieta].resize(m_cell_grid_phi,0); 
  }  
  
  for(ic=m_cells.begin();ic!=m_cells.end();++ic) {
    refcell=ic->second;
    int ieta,iphi;
    if(sampling<21) {
      ieta=raw_eta_position_to_index(refcell->eta_raw());
      iphi=raw_phi_position_to_index(refcell->phi_raw());
    } else {
      ieta=raw_eta_position_to_index(refcell->x_raw());
      iphi=raw_phi_position_to_index(refcell->y_raw());
    }
    if(index_range_adjust(ieta,iphi)) {
      if(m_cell_grid[ieta][iphi]) {
        cout<<"Grid: Sampling "<<sampling<<"_"<<index()<<": Already cell found at pos ("<<ieta<<","<<iphi<<"): id="
            <<m_cell_grid[ieta][iphi]->identify()<<"->"<<refcell->identify()<<endl;
        cout<<"    x="<<m_cells[m_cell_grid[ieta][iphi]->identify()]->x_raw()<<" -> "<<refcell->x_raw();
        cout<<" , y="<<m_cells[m_cell_grid[ieta][iphi]->identify()]->y_raw()<<" -> "<<refcell->y_raw();
        cout<<" mindx="<<m_deta_double<<" mindy="<<m_dphi_double<<endl;
        cout<<endl;
      }      
      m_cell_grid[ieta][iphi]=refcell;
    } else {
      cout<<"Grid: Sampling "<<sampling<<"_"<<index()<<": Cell at pos ("<<ieta<<","<<iphi<<"): id="
          <<refcell->identify()<<" outside eta range"<<endl;
    }
  }

  int ncells=0;
  int nempty=0;
  for(int ieta=0;ieta<m_cell_grid_eta;++ieta) {
    for(int iphi=0;iphi<m_cell_grid_phi;++iphi) {
      if(!m_cell_grid[ieta][iphi]) {
        ++nempty;
        //cout<<"Sampling "<<sampling<<"_"<<index()<<": No cell at pos ("<<ieta<<","<<iphi<<")"<<endl;
      } else {
        ++ncells;
      }
    }
  }  
  cout<<"Grid: Sampling "<<sampling<<"_"<<index()<<": "<<ncells<<"/"<<size()<<" cells filled, "<<nempty<<" empty grid positions"<<endl;
}

float CaloGeometryLookup::calculate_distance_eta_phi(CaloDetDescrElement* DDE,float eta,float phi,float& dist_eta0,float& dist_phi0)
{
  dist_eta0=(eta - DDE->eta())/m_deta_double;
  dist_phi0=(TVector2::Phi_mpi_pi(phi - DDE->phi()))/m_dphi_double;
  float abs_dist_eta0=TMath::Abs(dist_eta0);
  float abs_dist_phi0=TMath::Abs(dist_phi0);
  return TMath::Max(abs_dist_eta0,abs_dist_phi0)-0.5;
}

const CaloDetDescrElement* CaloGeometryLookup::getDDE(float eta,float phi,float* distance,int* steps) 
{
  float dist;
  CaloDetDescrElement* bestDDE=0;
  if(!distance) distance=&dist;
  *distance=+10000000;
  int intsteps=0;
  if(!steps) steps=&intsteps;
  
  float best_eta_corr=m_eta_correction;
  float best_phi_corr=m_phi_correction;
  
  float raw_eta=eta+best_eta_corr;
  float raw_phi=phi+best_phi_corr;

  int ieta=raw_eta_position_to_index(raw_eta);
  int iphi=raw_phi_position_to_index(raw_phi);
  index_range_adjust(ieta,iphi);
  
  CaloDetDescrElement* newDDE=m_cell_grid[ieta][iphi];
  float bestdist=+10000000;
  ++(*steps);
  int nsearch=0;
  while(newDDE && nsearch<3) {
    float dist_eta0,dist_phi0;
    *distance=calculate_distance_eta_phi(newDDE,eta,phi,dist_eta0,dist_phi0);
    bestDDE=newDDE;
    bestdist=*distance;

    if(CaloGeometry::m_debug || CaloGeometry::m_debug_identify==newDDE->identify()) {
      cout<<"CaloGeometryLookup::getDDE, search iteration "<<nsearch<<endl;
      cout<<"cell id="<<newDDE->identify()<<" steps "<<*steps<<" steps, eta="<<eta<<" phi="<<phi<<" dist="<<*distance<<" cell_grid_eta="<<cell_grid_eta()<<" cell_grid_phi="<<cell_grid_phi()<<" m_deta_double="<<m_deta_double<<" m_dphi_double="<<m_dphi_double<<" 1st_eta_corr="<<best_eta_corr<<" 1st_phi_corr="<<best_phi_corr<<endl;
      cout<<"  input sampling="<<newDDE->getSampling()<<" eta="<<newDDE->eta()<<" eta_raw="<<newDDE->eta_raw()<<" deta="<<newDDE->deta()<<" ("<<(newDDE->eta_raw()-newDDE->eta())/newDDE->deta()<<") phi="<<newDDE->phi()<<" phi_raw="<<newDDE->phi_raw()<<" dphi="<<newDDE->dphi()<<" ("<<(newDDE->phi_raw()-newDDE->phi())/newDDE->dphi()<<")"<<endl;
      cout<<"  ieta="<<ieta<<" iphi="<<iphi<<endl;
      cout<<"  dist_eta0="<<dist_eta0<<" ,dist_phi0="<<dist_phi0<<endl;
    }

    if(*distance<0) return newDDE;
    //correct ieta and iphi by the observed difference to the hit cell
    ieta+=TMath::Nint(dist_eta0);
    iphi+=TMath::Nint(dist_phi0);
    index_range_adjust(ieta,iphi);
    CaloDetDescrElement* oldDDE=newDDE;
    newDDE=m_cell_grid[ieta][iphi];
    ++(*steps);
    ++nsearch;
    if(oldDDE==newDDE) break;
  } 
  if(CaloGeometry::m_debug && !newDDE) {
    cout<<"CaloGeometryLookup::getDDE, direct search ieta="<<ieta<<" iphi="<<iphi<<" is empty"<<endl;
  }
  float minieta=ieta+TMath::Nint(TMath::Floor(m_mineta_correction/cell_grid_eta()));
  float maxieta=ieta+TMath::Nint(TMath::Ceil(m_maxeta_correction/cell_grid_eta()));
  float miniphi=iphi+TMath::Nint(TMath::Floor(m_minphi_correction/cell_grid_phi()));
  float maxiphi=iphi+TMath::Nint(TMath::Ceil(m_maxphi_correction/cell_grid_phi()));
  if(minieta<0) minieta=0;
  if(maxieta>=m_cell_grid_eta) maxieta=m_cell_grid_eta-1;
  for(int iieta=minieta;iieta<=maxieta;++iieta) {
    for(int iiphi=miniphi;iiphi<=maxiphi;++iiphi) {
      ieta=iieta;
      iphi=iiphi;
      index_range_adjust(ieta,iphi);
      newDDE=m_cell_grid[ieta][iphi];
      ++(*steps);
      if(newDDE) {
        float dist_eta0,dist_phi0;
        *distance=calculate_distance_eta_phi(newDDE,eta,phi,dist_eta0,dist_phi0);

        if(CaloGeometry::m_debug || CaloGeometry::m_debug_identify==newDDE->identify()) {
          cout<<"CaloGeometryLookup::getDDE, windows search! minieta="<<m_mineta_correction/cell_grid_eta()<<" maxieta="<<m_maxeta_correction/cell_grid_eta()<<" miniphi="<<m_minphi_correction/cell_grid_phi()<<" maxiphi="<<m_maxphi_correction/cell_grid_phi()<<endl;
          cout<<"cell id="<<newDDE->identify()<<" steps "<<*steps<<" steps, eta="<<eta<<" phi="<<phi<<" dist="<<*distance<<endl;
          cout<<"  input sampling="<<newDDE->getSampling()<<" eta="<<newDDE->eta()<<" eta_raw="<<newDDE->eta_raw()<<" deta="<<newDDE->deta()<<" ("<<(newDDE->eta_raw()-newDDE->eta())/newDDE->deta()<<") phi="<<newDDE->phi()<<" phi_raw="<<newDDE->phi_raw()<<" dphi="<<newDDE->dphi()<<" ("<<(newDDE->phi_raw()-newDDE->phi())/newDDE->dphi()<<")"<<endl;
          cout<<"  ieta="<<ieta<<" iphi="<<iphi<<endl;
          cout<<"  dist_eta0="<<dist_eta0<<" ,dist_phi0="<<dist_phi0<<endl;
        }

        if(*distance<0) return newDDE;
        if(*distance<bestdist) {
          bestDDE=newDDE;
          bestdist=*distance;
        }
      } else if(CaloGeometry::m_debug) {
        cout<<"CaloGeometryLookup::getDDE, windows search ieta="<<ieta<<" iphi="<<iphi<<" is empty"<<endl;
      }
    }
  }
  *distance=bestdist;
  return bestDDE;
}

/*
void CaloGeometryLookup::CalculateTransformation()
{
  gROOT->cd();

  TTree* tmap = new TTree( "mapping" , "mapping" );
  
  float eta,phi,Deta_raw,Dphi_raw;
  tmap->Branch("eta", &eta,"eta/F");
  tmap->Branch("phi", &phi,"phi/F");
  tmap->Branch("Deta_raw", &Deta_raw,"Deta_raw/F");
  tmap->Branch("Dphi_raw", &Dphi_raw,"Dphi_raw/F");
  
  if(m_cells.size()==0) return;

  int sampling=0;
  for(t_cellmap::iterator ic=m_cells.begin();ic!=m_cells.end();++ic) {
    CaloDetDescrElement* refcell=ic->second;
    sampling=refcell->getSampling();
    if(sampling<21) {
      eta=refcell->eta();
      phi=refcell->phi();
      Deta_raw=refcell->eta_raw()-eta;
      Dphi_raw=refcell->phi_raw()-phi;
    } else {
      eta=refcell->x();
      phi=refcell->y();
      Deta_raw=refcell->x_raw()-eta;
      Dphi_raw=refcell->y_raw()-phi;
    }  
    tmap->Fill();
    tmap->Fill(); //Fill twice to have all events and training and test tree
  }
  tmap->Print();
  
  TString outfileName( Form("Mapping%d_%d.root",sampling,m_index) );
  TFile* outputFile = new TFile( outfileName, "RECREATE" );
  //TFile* outputFile = new TMemFile( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( Form("Mapping%d_%d.root",sampling,m_index) , outputFile, "!V:!Silent:Color:DrawProgressBar" );

  factory->AddVariable( "eta", "calo eta", "1", 'F' );
  factory->AddVariable( "phi", "calo phi", "1", 'F' );
  factory->AddTarget( "Deta_raw" ); 
  factory->AddTarget( "Dphi_raw" ); 

  factory->AddRegressionTree( tmap );
  
  //factory->PrepareTrainingAndTestTree( "" , Form("nTrain_Regression=%d:nTest_Regression=%d:SplitMode=Random:NormMode=NumEvents:!V",(int)m_cells.size(),(int)m_cells.size()) );
  factory->PrepareTrainingAndTestTree( "" , "nTrain_Regression=0:nTest_Regression=0:SplitMode=Alternate:NormMode=NumEvents:!V" );

  factory->BookMethod( TMVA::Types::kMLP, "MLP", "!H:!V:VarTransform=Norm:NeuronType=tanh:NCycles=20000:HiddenLayers=N+5:TestRate=6:TrainingMethod=BFGS:Sampling=0.3:SamplingEpoch=0.8:ConvergenceImprove=1e-6:ConvergenceTests=15:!UseRegulator" );
  
  cout<<"=== Start trainging ==="<<endl;
  // Train MVAs using the set of training events
  factory->TrainAllMethods();

  cout<<"=== Start testing ==="<<endl;
  // ---- Evaluate all MVAs using the set of test events
  factory->TestAllMethods();

  cout<<"=== Start evaluation ==="<<endl;
  // ----- Evaluate and compare performance of all configured MVAs
  factory->EvaluateAllMethods();    

  outputFile->Close();

  delete factory;
  
  delete tmap;
}
*/

CaloGeometry::CaloGeometry() : m_cells_in_sampling(MAX_SAMPLING),m_cells_in_regions(MAX_SAMPLING)
{
  //TMVA::Tools::Instance();
}

CaloGeometry::~CaloGeometry()
{
}

void CaloGeometry::addcell(CaloDetDescrElement* cell) 
{
  int sampling=cell->getSampling();
  Identifier identify=cell->identify();
  
  m_cells[identify]=cell;
  m_cells_in_sampling[sampling][identify]=cell;
  
  CaloGeometryLookup* lookup=0;
  for(unsigned int i=0;i<m_cells_in_regions[sampling].size();++i) {
    if(m_cells_in_regions[sampling][i]->IsCompatible(cell)) {
      lookup=m_cells_in_regions[sampling][i];
      //cout<<sampling<<":  found compatible map"<<endl;
      break;
    }
  }
  if(!lookup) {
    lookup=new CaloGeometryLookup(m_cells_in_regions[sampling].size());
    m_cells_in_regions[sampling].push_back(lookup);
  }
  lookup->add(cell);
}

void CaloGeometry::PrintMapInfo(int i, int j) 
{
  cout<<"  map "<<j<<": "<<m_cells_in_regions[i][j]->size()<<" cells";
  if(i<21) {
    cout<<", "<<m_cells_in_regions[i][j]->cell_grid_eta()<<"*"<<m_cells_in_regions[i][j]->cell_grid_phi();
    cout<<", deta="<<m_cells_in_regions[i][j]->deta().mean()<<"+-"<<m_cells_in_regions[i][j]->deta().rms();
    cout<<", dphi="<<m_cells_in_regions[i][j]->dphi().mean()<<"+-"<<m_cells_in_regions[i][j]->dphi().rms();
    cout<<", mineta="<<m_cells_in_regions[i][j]->mineta()<<", maxeta="<<m_cells_in_regions[i][j]->maxeta();
    cout<<", minphi="<<m_cells_in_regions[i][j]->minphi()<<", maxphi="<<m_cells_in_regions[i][j]->maxphi();
    cout<<endl<<"         ";  
    cout<<", mineta_raw="<<m_cells_in_regions[i][j]->mineta_raw()<<", maxeta_raw="<<m_cells_in_regions[i][j]->maxeta_raw();
    cout<<", minphi_raw="<<m_cells_in_regions[i][j]->minphi_raw()<<", maxphi_raw="<<m_cells_in_regions[i][j]->maxphi_raw();
    cout<<endl;
  } else {
    cout<<", "<<m_cells_in_regions[i][j]->cell_grid_eta()<<"*"<<m_cells_in_regions[i][j]->cell_grid_phi();
    cout<<", dx="<<m_cells_in_regions[i][j]->dx().mean()<<"+-"<<m_cells_in_regions[i][j]->dx().rms();
    cout<<", dy="<<m_cells_in_regions[i][j]->dy().mean()<<"+-"<<m_cells_in_regions[i][j]->dy().rms();
    cout<<", mindx="<<m_cells_in_regions[i][j]->mindx();
    cout<<", mindy="<<m_cells_in_regions[i][j]->mindy();
    cout<<", minx="<<m_cells_in_regions[i][j]->minx()<<", maxx="<<m_cells_in_regions[i][j]->maxx();
    cout<<", miny="<<m_cells_in_regions[i][j]->miny()<<", maxy="<<m_cells_in_regions[i][j]->maxy();
    cout<<endl<<"         ";  
    cout<<", minx_raw="<<m_cells_in_regions[i][j]->minx_raw()<<", maxx_raw="<<m_cells_in_regions[i][j]->maxx_raw();
    cout<<", miny_raw="<<m_cells_in_regions[i][j]->miny_raw()<<", maxy_raw="<<m_cells_in_regions[i][j]->maxy_raw();
    cout<<endl;
  }
}

void CaloGeometry::post_process(int sampling)
{
  cout<<"post processing sampling "<<sampling<<endl;
  bool found_overlap=false;
  for(unsigned int j=0;j<m_cells_in_regions[sampling].size();++j) {
    /*
    cout<<"Sample "<<sampling<<": checking map "<<j<<"/"<<m_cells_in_regions[sampling].size();
    for(unsigned int k=0;k<m_cells_in_regions[sampling].size();++k) {
      cout<<", "<<k<<":"<<m_cells_in_regions[sampling][k]->size();
    }
    cout<<endl;
    */
    for(unsigned int i=j+1;i<m_cells_in_regions[sampling].size();++i) {
      if(m_cells_in_regions[sampling][i]->has_overlap(m_cells_in_regions[sampling][j])) {
        if(!found_overlap) {
          cout<<"Sample "<<sampling<<", starting maps : "<<m_cells_in_regions[sampling].size();
          for(unsigned int k=0;k<m_cells_in_regions[sampling].size();++k) {
            cout<<", "<<k<<":"<<m_cells_in_regions[sampling][k]->size();
          }
          cout<<endl;
        }
        found_overlap=true;
        /*
        cout<<"Sample "<<sampling<<": Found overlap between map "<<j<<" and "<<i<<", "
            <<m_cells_in_regions[sampling].size()<<" total maps"<<endl;
        cout<<"  current configuration map "<<j<<"/"<<m_cells_in_regions[sampling].size();
        for(unsigned int k=0;k<m_cells_in_regions[sampling].size();++k) {
          cout<<", "<<k<<":"<<m_cells_in_regions[sampling][k]->size();
        }
        cout<<endl;

        PrintMapInfo(sampling,j);
        PrintMapInfo(sampling,i);
        */
        
        CaloGeometryLookup* toremove=m_cells_in_regions[sampling][i];
        toremove->merge_into_ref(m_cells_in_regions[sampling][j]);
        
        /*
        cout<<"  New ";
        PrintMapInfo(sampling,j);
        */
        
        for(unsigned int k=i;k<m_cells_in_regions[sampling].size()-1;++k) {
          m_cells_in_regions[sampling][k]=m_cells_in_regions[sampling][k+1];
          m_cells_in_regions[sampling][k]->set_index(k);
        }
        m_cells_in_regions[sampling].resize(m_cells_in_regions[sampling].size()-1);

        /*
        cout<<"  new configuration map "<<j<<"/"<<m_cells_in_regions[sampling].size();
        for(unsigned int k=0;k<m_cells_in_regions[sampling].size();++k) {
          cout<<", "<<k<<":"<<m_cells_in_regions[sampling][k]->size();
        }
        cout<<endl;
        */
        
        --i;
      }
    }
  }
  
  if(found_overlap) {
    cout<<"Sample "<<sampling<<", final maps : "<<m_cells_in_regions[sampling].size();
    for(unsigned int k=0;k<m_cells_in_regions[sampling].size();++k) {
      cout<<", "<<k<<":"<<m_cells_in_regions[sampling][k]->size();
    }
    cout<<endl;
  }

  if(found_overlap) {
    cout<<"Run another round of ";
    post_process(sampling);
  }  
}

const CaloDetDescrElement* CaloGeometry::getDDE(Identifier identify) 
{
  return m_cells[identify];
}

const CaloDetDescrElement* CaloGeometry::getDDE(int sampling,float eta,float phi,float* distance,int* steps) 
{
  if(sampling<0) return 0;
  if(sampling>=MAX_SAMPLING) return 0;
  if(m_cells_in_regions[sampling].size()==0) return 0;
  
  float dist;
  const CaloDetDescrElement* bestDDE=0;
  if(!distance) distance=&dist;
  *distance=+10000000;
  int intsteps;
  int beststeps;
  if(steps) beststeps=(*steps);
   else beststeps=0;
  
  if(sampling<21) {
    for(int skip_range_check=0;skip_range_check<=1;++skip_range_check) {
      for(unsigned int j=0;j<m_cells_in_regions[sampling].size();++j) {
        if(!skip_range_check) {
          if(eta<m_cells_in_regions[sampling][j]->mineta()) continue;
          if(eta>m_cells_in_regions[sampling][j]->maxeta()) continue;
        }  
        if(steps) intsteps=(*steps);
         else intsteps=0;
        if(m_debug) {
          cout<<"CaloGeometry::getDDE : check map"<<j<<" skip_range_check="<<skip_range_check<<endl;
        }
        float newdist;
        const CaloDetDescrElement* newDDE=m_cells_in_regions[sampling][j]->getDDE(eta,phi,&newdist,&intsteps);
        if(m_debug) {
          cout<<"CaloGeometry::getDDE : map"<<j<<" dist="<<newdist<<" best dist="<<*distance<<" steps="<<intsteps<<endl;
        }
        if(newdist<*distance) {
          bestDDE=newDDE;
          *distance=newdist;
          if(steps) beststeps=intsteps;
          if(newdist<-0.1) break; //stop, we are well within the hit cell
        }
      }
      if(bestDDE) break;
    }
  } else {
    for(int skip_range_check=0;skip_range_check<=1;++skip_range_check) {
      for(unsigned int j=0;j<m_cells_in_regions[sampling].size();++j) {
        if(!skip_range_check) {
          if(eta<m_cells_in_regions[sampling][j]->minx()) continue;
          if(eta>m_cells_in_regions[sampling][j]->maxx()) continue;
          if(phi<m_cells_in_regions[sampling][j]->miny()) continue;
          if(phi>m_cells_in_regions[sampling][j]->maxy()) continue;
        }  
        if(steps) intsteps=*steps;
         else intsteps=0;
        if(m_debug) {
          cout<<"CaloGeometry::getDDE : check map"<<j<<" skip_range_check="<<skip_range_check<<endl;
        }
        float newdist;
        const CaloDetDescrElement* newDDE=m_cells_in_regions[sampling][j]->getDDE(eta,phi,&newdist,&intsteps);
        if(m_debug) {
          cout<<"CaloGeometry::getDDE : map"<<j<<" dist="<<newdist<<" best dist="<<*distance<<" steps="<<intsteps<<endl;
        }
        if(newdist<*distance) {
          bestDDE=newDDE;
          *distance=newdist;
          if(steps) beststeps=intsteps;
          if(newdist<-0.1) break; //stop, we are well within the hit cell
        }
      }
      if(bestDDE) break;
    }  
  }
  if(steps) *steps=beststeps;
  return bestDDE;
}

bool CaloGeometry::PostProcessGeometry()
{
  for(int i=0;i<MAX_SAMPLING;++i) {
    post_process(i);
    for(unsigned int j=0;j<m_cells_in_regions[i].size();++j) {
      m_cells_in_regions[i][j]->post_process();
    }
    //if(i>=21) break;
  } 
  
  /*
  cout<<"all : "<<m_cells.size()<<endl;
  for(int sampling=0;sampling<MAX_SAMPLING;++sampling) {
    cout<<"cells sampling "<<sampling<<" : "<<m_cells_in_sampling[sampling].size()<<" cells";
    cout<<", "<<m_cells_in_regions[sampling].size()<<" lookup map(s)"<<endl;
    for(unsigned int j=0;j<m_cells_in_regions[sampling].size();++j) {
      PrintMapInfo(sampling,j);
      //break;
    }
    //if(i>0) break;
  } 
  */ 

  return true;
}

void CaloGeometry::Validate()
{
  int ntest=0;
  cout<<"start CaloGeometry::Validate()"<<endl;
  for(t_cellmap::iterator ic=m_cells.begin();ic!=m_cells.end();++ic) {
    CaloDetDescrElement* cell=ic->second;
    int sampling=cell->getSampling();
    if(sampling>=21) continue;

    if(m_debug_identify==cell->identify()) {
      cout<<"CaloGeometry::Validate(), cell "<<ntest<<" id="<<cell->identify()<<endl; 
      m_debug=true;
    }  
    
    const int nrnd=100;
    for(int irnd=0;irnd<nrnd;++irnd) {
      int steps=0;
      float eta=cell->eta()+1.5*(gRandom->Rndm()-0.5)*cell->deta();
      float phi=cell->phi()+1.5*(gRandom->Rndm()-0.5)*cell->dphi();
      float distance;
      const CaloDetDescrElement* foundcell=getDDE(sampling,eta,phi,&distance,&steps);
      if(m_debug && foundcell) {
        cout<<"CaloGeometry::Validate(), irnd="<<irnd<<", foundcell id="<<foundcell->identify()<<", "<<steps<<" steps"<<endl; 
      }  
      if(cell==foundcell) {
        if(steps>3 && distance<-0.01) { 
          cout<<"cell id="<<cell->identify()<<", sampling="<<sampling<<" found in "<<steps<<" steps, dist="<<distance<<" eta="<<eta<<" phi="<<phi<<endl;
          cout<<"  eta="<<cell->eta()<<" eta_raw="<<cell->eta_raw()<<" deta="<<cell->deta()<<" ("<<(cell->eta_raw()-cell->eta())/cell->deta()<<") phi="<<cell->phi()<<" phi_raw="<<cell->phi_raw()<<" dphi="<<cell->dphi()<<" ("<<(cell->phi_raw()-cell->phi())/cell->dphi()<<")"<<endl;
          //if(steps>3 && distance>0.01) return;
        }
      } else {
        if( TMath::Abs( (eta-cell->eta())/cell->deta() )<0.45 && TMath::Abs( (phi-cell->phi())/cell->dphi() )<0.45 ) {
          cout<<"cell id="<<cell->identify()<<" not found! Found instead id="<<foundcell->identify()<<" in "<<steps<<" steps, dist="<<distance<<" eta="<<eta<<" phi="<<phi<<endl;
          cout<<"  input sampling="<<sampling<<" eta="<<cell->eta()<<" eta_raw="<<cell->eta_raw()<<" deta="<<cell->deta()<<" ("<<(cell->eta_raw()-cell->eta())/cell->deta()<<") phi="<<cell->phi()<<" phi_raw="<<cell->phi_raw()<<" dphi="<<cell->dphi()<<" ("<<(cell->phi_raw()-cell->phi())/cell->dphi()<<")"<<endl;
          cout<<" output sampling="<<foundcell->getSampling()<<" eta="<<foundcell->eta()<<" eta_raw="<<foundcell->eta_raw()<<" deta="<<foundcell->deta()<<" ("<<(foundcell->eta_raw()-foundcell->eta())/foundcell->deta()<<") phi="<<foundcell->phi()<<" phi_raw="<<foundcell->phi_raw()<<" dphi="<<foundcell->dphi()<<" ("<<(foundcell->phi_raw()-foundcell->phi())/cell->dphi()<<")"<<endl;
          return;
        }  
        if(!foundcell) {
          cout<<"nothing found close to cell id="<<cell->identify()<<" in "<<steps<<" steps, dist="<<distance<<" eta="<<eta<<" phi="<<phi<<endl;
          cout<<"  input sampling="<<sampling<<" eta="<<cell->eta()<<" eta_raw="<<cell->eta_raw()<<" deta="<<cell->deta()<<" ("<<(cell->eta_raw()-cell->eta())/cell->deta()<<") phi="<<cell->phi()<<" phi_raw="<<cell->phi_raw()<<" dphi="<<cell->dphi()<<" ("<<(cell->phi_raw()-cell->phi())/cell->dphi()<<")"<<endl;
          return;
        }
      }
      //if(ntest>60000) break;
    }  
    m_debug=false;
    if(ntest%25000==0) cout<<"Validate cell "<<ntest<<" with "<<nrnd<<" random hits"<<endl;
    ++ntest;
  }
  cout<<"end CaloGeometry::Validate()"<<endl;
}

