/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <TMinuit.h>
#include "TopEventReconstructionTools/TtresNeutrinoBuilder.h"

//_________________________________________________________________________________________________
void delta2_fcn(Int_t&, Double_t*, Double_t&, Double_t*, Int_t);

//_________________________________________________________________________________________________
TtresNeutrinoBuilder::TtresNeutrinoBuilder(std::string units){
  m_debug = 0;
  if     (units=="MeV")m_Units = 1000.;
  else if(units=="GeV")m_Units = 1.;
  else std::cerr << "WARNING in NeutrinoBuilder: units different from GeV or MeV" << std::endl;
}

//_________________________________________________________________________________________________
TtresNeutrinoBuilder::~TtresNeutrinoBuilder(){
  if(m_debug>0) std::cout << "in destructor " << std::endl;
}

//_________________________________________________________________________________________________                                                         
TtresNeutrinoBuilder::TtresNeutrinoBuilder(const TtresNeutrinoBuilder& other) {                                                                                     
  m_debug = other.m_debug;
}

//_________________________________________________________________________________________________                                                         
TtresNeutrinoBuilder& TtresNeutrinoBuilder::operator=(const TtresNeutrinoBuilder& other) {                                                                           
  if(this != &other) {
    m_debug = other.m_debug;
  }
  return *this;
}

//_________________________________________________________________________________________________
// JDM - fix compiler warnings
void delta2_fcn(Int_t& /*npar*/, Double_t* /*grad*/, Double_t& f, Double_t* par, Int_t /*iflag*/){
    
  Double_t delta2 = 0;
  Double_t alpha = par[0];
  Double_t r = par[1];
  Double_t dphi = par[2];
  Double_t l_pt = par[3];
  Double_t l_m = par[4];
  Double_t n_px = par[5];
  Double_t n_py = par[6];
  r /= sqrt(l_pt * l_pt + l_m * l_m) - l_pt * cos(dphi + alpha);
  TLorentzVector *neut = new TLorentzVector(n_px, n_py, 0., 0.);
  neut->SetE(neut->P());
  
  TLorentzVector *neut_new = new TLorentzVector(r * neut->P() * cos(neut->Phi() + alpha), r * neut->P() * sin(neut->Phi() + alpha), 0., 0.);
  neut_new->SetE(neut_new->P());
  
  delta2 = pow((neut_new->Px() - neut->Px()), 2)  + pow((neut_new->Py() - neut->Py()), 2);
  r *= sqrt(l_pt * l_pt + l_m * l_m) - l_pt * cos(dphi + alpha);
  delete neut;
  delete neut_new;
  f = delta2;
}

//_________________________________________________________________________________________________
double TtresNeutrinoBuilder::fitAlpha(const TLorentzVector* L, const Double_t met, const Double_t metphi){
  
  // initialize
  double m_mWpdg = 80.4 * m_Units;
  Double_t pxNu = met * cos(metphi);
  Double_t pyNu = met * sin(metphi);
  Double_t ptNu = met;
  
  TMinuit *fit = new TMinuit(7);
  fit->SetFCN(delta2_fcn);
  int ierr = 0;
  double arglist[1] = {-1};
  fit->mnexcm("SET PRIN",arglist,1,ierr);

  // Initialise the parameters
  std::string par_name[7] = {"alpha", "r", "dphi", "l_pt", "l_m", "n_px", "n_py"};
  Double_t par_ival[7] = {0., (m_mWpdg * m_mWpdg - L->M() * L->M()) / (2 * ptNu), metphi - L->Phi(), L->Pt(), L->M(), pxNu, pyNu};
  Double_t par_step[7] = {0.1, 0., 0., 0., 0., 0., 0.};
  Double_t par_min[7] = {-3.15, 0., -3.15, 0., 0., -10000., -10000.};
  Double_t par_max[7] = {3.15, 1., 3.15, 10000., 80., 10000., 10000.};
  for (Int_t i = 0; i < 7; i++){
    fit->DefineParameter(i,par_name[i].c_str(),par_ival[i],par_step[i],par_min[i],par_max[i]);
    if (i != 0){
      fit->FixParameter(i);
    }
  }
  fit->SetPrintLevel(-1);
  fit->Migrad();
  Double_t a, e_a;
  Int_t ret = fit->GetParameter(0, a, e_a);
  delete fit;
  if (ret > 0){
    return a;
  }
  else {
    std::cout << "Error in fit of alpha for met correction" << std::endl;
    return 0.;
  }
  
}


// In case of negative discriminant, decrease the MET
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_Scaling(const TLorentzVector* L, const TLorentzVector* MET){
return this->candidatesFromWMass_Scaling(L, MET->Px(), MET->Py());
}

// In case of negative discriminant, decrease the MET
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_Scaling(const TLorentzVector* L, const double met_etx, const double met_ety){
  if(m_debug>0) std::cout << "entering NeutrinoBuilder::candidatesFromWMass_Scaling()" << std::endl;
  std::vector<TLorentzVector*> NC;
  this->candidatesFromWMass_Scaling(L, met_etx, met_ety, NC);
  return NC;
}

// In case of negative discriminant, decrease the MET
//_________________________________________________________________________________________________
bool TtresNeutrinoBuilder::candidatesFromWMass_Scaling(const TLorentzVector* L, const double met_etx, const double met_ety, std::vector<TLorentzVector*>& NC){
  
  if(m_debug>0) std::cout << "entering NeutrinoBuilder::candidatesFromWMass_Scaling()" << std::endl;
  // initialize
  double delta;
  double lambda;
  double pxNu;
  double pyNu;
  double ptNu = 21;
  double alpha;
  double beta;
  double gamma;
  double m_mWpdg=80.4 * m_Units;
  int count = 0;
  
  pxNu = met_etx;
  pyNu = met_ety;
  do {
    // solve the quadratic equation
    ++count;
    ptNu = sqrt (pxNu*pxNu + pyNu*pyNu);    
    alpha = pow(m_mWpdg,2)+pow((pxNu+L->Px()),2)+pow((pyNu+L->Py()),2)-pow(L->E(),2);
    
    beta = 0.5 * ( alpha-pow(ptNu,2)+pow(L->Pz(),2) );
    gamma = -( beta*beta - ( pow(L->E(),2)*pow(ptNu,2) ) ) / ( pow(L->E(),2)-pow(L->Pz(),2) );
    lambda = 2*beta*L->Pz() / (pow(L->E(),2)-pow(L->Pz(),2));
    delta = pow(lambda,2)-4*gamma;
    if(m_debug>1) std::cout <<  "count = " << count << "\tptNu = " << ptNu << "\tdelta = " << delta << "\tm_mWpdg = " << m_mWpdg << std::endl;
    // if no real solution, reduce Pt by 0.1 GeV and recompute pxNu & pyNu
    // in consequence
    if (delta < 0){
      if(ptNu > 100. ){
	double Ptmiss = ptNu;
	ptNu -= 100. ;
	pxNu *= ptNu/Ptmiss;
	pyNu *= ptNu/Ptmiss;
      }   
    } // end of case delta is negatively defined
    
  } while( (delta < 0) && (count < 10000) && ( ptNu > 20 *1000));
  if (delta < 0){
    if(m_debug>0) std::cout << "no solution delta still<0" << std::endl;
    return false;
  }
  delta = sqrt(delta);
  // instantiate Neutrino
  double pz = (lambda-delta)/2.0;
  double e  = sqrt(pxNu*pxNu+pyNu*pyNu+pz*pz);
  TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pz,e);
  pz = (lambda+delta)/2.0;
  e  = sqrt(pxNu*pxNu+pyNu*pyNu+pz*pz);
  TLorentzVector *nu2 = new TLorentzVector(pxNu,pyNu,pz,e);
  NC.push_back(nu1);
  NC.push_back(nu2);
  if(m_debug>0) std::cout << "quitting NeutrinoBuilder::candidatesFromWMass_Scaling()" << std::endl;
  return true;
}





// In case of negative discriminant, decrese the MET
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_Rotation(const TLorentzVector* L, const TLorentzVector* MET, const bool useSmallestPz){
 return this->candidatesFromWMass_Rotation(L, MET->Pt(), MET->Phi(), useSmallestPz);
}

// In case of negative discriminant, decrese the MET
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_Rotation(const TLorentzVector* L, const Double_t met, const Double_t metphi, const bool useSmallestPz){
    if(m_debug>0) std::cout << "entering candidatesFromWMassRotation()" << std::endl;

    // initialize
    Double_t m_mWpdg = 80.4 * m_Units;
    Double_t pxNu = met * cos(metphi);
    Double_t pyNu = met * sin(metphi);
    Double_t pzNu = -1000000;
    Double_t ptNu = met;
    Double_t eNu;
    
    std::vector<TLorentzVector*> NC;
    
    Double_t c1 = m_mWpdg * m_mWpdg - L->M() * L->M() + 2 * (L->Px() * pxNu + L->Py() * pyNu);
    Double_t b1 = 2 * L->Pz();
    
    Double_t A = 4 * pow(L->E(), 2) - b1 * b1;
    Double_t B = -2 * c1 * b1;
    Double_t C = 4 * pow(L->E(), 2) * ptNu * ptNu - c1 * c1;
    Double_t discr = B*B - 4*A*C;
    Double_t r = 1;

    Double_t sol1, sol2;
    if (discr > 0){
      sol1 = (-B + sqrt(discr)) / (2*A);
      sol2 = (-B - sqrt(discr)) / (2*A);
    }
    else { 
      Double_t alpha = fitAlpha(L, met, metphi);
      Double_t dphi = metphi - L->Phi();
      r = ( pow(m_mWpdg,2) - pow(L->M(),2) ) / (2 * ptNu * (sqrt(pow(L->Pt(),2) + pow(L->M(),2)) - L->Pt() * cos(dphi + alpha)));
      
      Double_t old_p = ptNu;
      Double_t old_phi = metphi;
      pxNu = r * old_p * cos(old_phi + alpha);
      pyNu = r * old_p * sin(old_phi + alpha);
      ptNu = sqrt (pxNu*pxNu + pyNu*pyNu);
      
      c1 = m_mWpdg * m_mWpdg - pow(L->M(),2) + 2 * (L->Px() * pxNu + L->Py() * pyNu);
      B = -2 * c1 * b1;
      C = 4 * pow(L->E(),2) * ptNu * ptNu - c1 * c1;
      discr = B*B - 4*A*C;
      
      sol1 = -B / (2*A);
      sol2 = -B / (2*A);
    }
    
    if (useSmallestPz){
      
      pzNu = (fabs(sol1) > fabs(sol2)) ? sol2 : sol1;
      
      eNu  = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      NC.push_back(nu1);

    }else{
    
      pzNu = sol1;
      eNu  = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      pzNu = sol2;
      eNu = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu2 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      NC.push_back(nu1);
      NC.push_back(nu2);
      
    }

    if(m_debug>0) std::cout << "quitting NeutrinoBuilder::candidatesFromWMassRotation() : " << NC.size() << std::endl;
    return NC;
}

// In case of negative discriminant, use the real part
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_RealPart(const TLorentzVector* L, const TLorentzVector* MET, const bool useSmallestPz){
   return this->candidatesFromWMass_RealPart(L, MET->Pt(), MET->Phi(), useSmallestPz);
}
// In case of negative discriminant, use the real part
//_________________________________________________________________________________________________
std::vector<TLorentzVector*> TtresNeutrinoBuilder::candidatesFromWMass_RealPart(const TLorentzVector* L, Double_t met, Double_t metphi, const bool useSmallestPz){
    if(m_debug>0) std::cout << "entering candidatesFromWMass_RealPart()" << std::endl;

    // initialize
    Double_t m_mWpdg = 80.4 * m_Units;
    Double_t pxNu = met * cos(metphi);
    Double_t pyNu = met * sin(metphi);
    Double_t pzNu = -1000000;
    Double_t ptNu = met;
    Double_t eNu;
    
    std::vector<TLorentzVector*> NC;
    
    Double_t c1 = m_mWpdg * m_mWpdg - L->M() * L->M() + 2 * (L->Px() * pxNu + L->Py() * pyNu);
    Double_t b1 = 2 * L->Pz();
    
    Double_t A = 4 * pow(L->E(), 2) - b1 * b1;
    Double_t B = -2 * c1 * b1;
    Double_t C = 4 * pow(L->E(), 2) * ptNu * ptNu - c1 * c1;
    Double_t discr = B*B - 4*A*C;

    Double_t sol1, sol2;
    if (discr > 0){
      sol1 = (-B + sqrt(discr)) / (2*A);
      sol2 = (-B - sqrt(discr)) / (2*A);
    } 
    //if discr<0
    else {
      pzNu = -B / (2*A);
      
      eNu  = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      NC.push_back(nu1);
      return NC;
    }
    
    if (useSmallestPz){
      
      pzNu = (fabs(sol1) > fabs(sol2)) ? sol2 : sol1;
      
      eNu  = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      NC.push_back(nu1);

    }else{
    
      pzNu = sol1;
      eNu  = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu1 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      pzNu = sol2;
      eNu = sqrt(pxNu*pxNu + pyNu*pyNu + pzNu*pzNu);
      TLorentzVector *nu2 = new TLorentzVector(pxNu,pyNu,pzNu,eNu);
      NC.push_back(nu1);
      NC.push_back(nu2);
      
    }

    if(m_debug>0) std::cout << "quitting NeutrinoBuilder::candidatesFromWMass_RealPart() : " << NC.size() << std::endl;
    return NC;
}

Double_t TtresNeutrinoBuilder::getDiscriminant(const TLorentzVector* L, Double_t met, Double_t metphi){

    // initialize
    Double_t m_mWpdg = 80.4 * m_Units;
    Double_t pxNu = met * cos(metphi);
    Double_t pyNu = met * sin(metphi);
    Double_t ptNu = met;
    
    Double_t c1 = m_mWpdg * m_mWpdg - L->M() * L->M() + 2 * (L->Px() * pxNu + L->Py() * pyNu);
    Double_t b1 = 2 * L->Pz();
    
    Double_t A = 4 * pow(L->E(), 2) - b1 * b1;
    Double_t B = -2 * c1 * b1;
    Double_t C = 4 * pow(L->E(), 2) * ptNu * ptNu - c1 * c1;
    Double_t discr = B*B - 4*A*C;
    return discr;
}

