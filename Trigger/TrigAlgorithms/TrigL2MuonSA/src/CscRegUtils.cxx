/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "CscRegUtils.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include <cmath>


namespace TrigL2MuonSA{


CscRegDict :: CscRegDict(const std::string &type, const std::string &name, const IInterface *parent)
  : AthAlgTool(type,name,parent),
    m_util()
{
}


StatusCode CscRegDict :: initialize(){

  int i=0;

  //small sector
  const double SPwid = 0.32369;
  const double Setamin = 1.97667;
  const double Setamax = 2.76781;
  const double SDisplace = (7441.+7518.)/2.;//7428.3;
  const double SAtanNormal = 0.20223129856437;

  //Cside Small
  for (int phi=0; phi<8; ++phi) {
    m_reg_dict[i].etaMin=(-1)*Setamax;
    m_reg_dict[i].etaMax=(-1)*Setamin;
    m_reg_dict[i].phiMin=PhiConv( 0.125*M_PI+ phi*0.25*M_PI - 0.5*SPwid );
    m_reg_dict[i].phiMax=PhiConv( 0.125*M_PI+ phi*0.25*M_PI + 0.5*SPwid );
    m_reg_dict[i].phiCen= m_reg_dict[i].phiMod =PhiConv( 0.125*M_PI+ phi*0.25*M_PI );
    m_reg_dict[i].Displacement=SDisplace;
    m_reg_dict[i].idealAtanNormal= m_reg_dict[i].actualAtanNormal =M_PI-SAtanNormal;
    m_reg_dict[i].posCorrectionMinusR=0.;
    m_reg_dict[i].posCorrectionPlusR=0.;
    ATH_MSG_DEBUG( "CscRegDict: hash= " << i << " StationName=" << stationName(i) << " StationEta=" << stationEta(i) << " StationPhi=" << stationPhi(i)
		   << " eta:["  << m_reg_dict[i].etaMin << "," << m_reg_dict[i].etaMax << "]"
		   << " phi:[" << m_reg_dict[i].phiMin << "," << m_reg_dict[i].phiMax << "]"
		   << " theta:[" << m_util.calc_theta(m_reg_dict[i].etaMin) << "," << m_util.calc_theta(m_reg_dict[i].etaMax) << "]");
    ++i;
  }
  //Aside Small
  for (int phi=0; phi<8; ++phi) {
    m_reg_dict[i].etaMin=Setamin;
    m_reg_dict[i].etaMax=Setamax;
    m_reg_dict[i].phiMin=PhiConv( 0.125*M_PI+ phi*0.25*M_PI - 0.5*SPwid );
    m_reg_dict[i].phiMax=PhiConv( 0.125*M_PI+ phi*0.25*M_PI + 0.5*SPwid );
    m_reg_dict[i].phiCen= m_reg_dict[i].phiMod =PhiConv( 0.125*M_PI+ phi*0.25*M_PI );
    m_reg_dict[i].Displacement=SDisplace;
    m_reg_dict[i].idealAtanNormal= m_reg_dict[i].actualAtanNormal =SAtanNormal;
    m_reg_dict[i].posCorrectionMinusR=0.;
    m_reg_dict[i].posCorrectionPlusR=0.;
    ATH_MSG_DEBUG( "CscRegDict: hash= " << i << " StationName=" << stationName(i) << " StationEta=" << stationEta(i) << " StationPhi=" << stationPhi(i)
		   << " eta:["  << m_reg_dict[i].etaMin << "," << m_reg_dict[i].etaMax << "]"
		   << " phi:[" << m_reg_dict[i].phiMin << "," << m_reg_dict[i].phiMax << "]"
		   << " theta:[" << m_util.calc_theta(m_reg_dict[i].etaMin) << "," << m_util.calc_theta(m_reg_dict[i].etaMax) << "]");
    ++i;
  }


  //large sector
  const double LPwid = 0.514507;
  const double Letamin = 2.01471;
  const double Letamax = 2.75914;
  const double LDisplace = (7800.+7880.)/2.;//7789.6;
  const double LAtanNormal = 0.20223129856437;

  //Cside Large
  for (int phi=0; phi<8; ++phi) {
    m_reg_dict[i].etaMin=(-1)*Letamax;
    m_reg_dict[i].etaMax=(-1)*Letamin;
    m_reg_dict[i].phiMin=PhiConv( phi*0.25*M_PI - 0.5*LPwid );
    m_reg_dict[i].phiMax=PhiConv( phi*0.25*M_PI + 0.5*LPwid );
    m_reg_dict[i].phiCen= m_reg_dict[i].phiMod =PhiConv( phi*0.25*M_PI );
    m_reg_dict[i].Displacement=LDisplace;
    m_reg_dict[i].idealAtanNormal= m_reg_dict[i].actualAtanNormal =M_PI-LAtanNormal;
    m_reg_dict[i].posCorrectionMinusR=0.;
    m_reg_dict[i].posCorrectionPlusR=0.;
    ATH_MSG_DEBUG( "CscRegDict: hash= " << i << " StationName=" << stationName(i) << " StationEta=" << stationEta(i) << " StationPhi=" << stationPhi(i)
		   << " eta:["  << m_reg_dict[i].etaMin << "," << m_reg_dict[i].etaMax << "]"
		   << " phi:[" << m_reg_dict[i].phiMin << "," << m_reg_dict[i].phiMax << "]"
		   << " theta:[" << m_util.calc_theta(m_reg_dict[i].etaMin) << "," << m_util.calc_theta(m_reg_dict[i].etaMax) << "]");
    ++i;
  }
    //Aside Large
  for (int phi=0; phi<8; ++phi) {
    m_reg_dict[i].etaMin=Letamin;
    m_reg_dict[i].etaMax=Letamax;
    m_reg_dict[i].phiMin=PhiConv( phi*0.25*M_PI - 0.5*LPwid );
    m_reg_dict[i].phiMax=PhiConv( phi*0.25*M_PI + 0.5*LPwid );
    m_reg_dict[i].phiCen= m_reg_dict[i].phiMod =PhiConv( phi*0.25*M_PI );
    m_reg_dict[i].Displacement=LDisplace;
    m_reg_dict[i].idealAtanNormal= m_reg_dict[i].actualAtanNormal =LAtanNormal;
    m_reg_dict[i].posCorrectionMinusR=0.;
    m_reg_dict[i].posCorrectionPlusR=0.;
    ATH_MSG_DEBUG( "CscRegDict: hash= " << i << " StationName=" << stationName(i) << " StationEta=" << stationEta(i) << " StationPhi=" << stationPhi(i)
		   << " eta:["  << m_reg_dict[i].etaMin << "," << m_reg_dict[i].etaMax << "]"
		   << " phi:[" << m_reg_dict[i].phiMin << "," << m_reg_dict[i].phiMax << "]"
		   << " theta:[" << m_util.calc_theta(m_reg_dict[i].etaMin) << "," << m_util.calc_theta(m_reg_dict[i].etaMax) << "]");
    ++i;
  }
  /*
  ATH_MSG_DEBUG( "m_isMC=" << m_isMC );
  if (!m_isMC){
    initializePosCorrectionParameters();
    initializeDictionaryForData();
  }
  */  
  initializeHashDictionary();


  return StatusCode::SUCCESS;
}


int CscRegDict :: get_hash(int stationname, int stationeta, int stationphi){

  int sname, seta, sphi;
  if (stationname == 50 || stationname == 51)  sname = stationname-50;
  else {  ATH_MSG_DEBUG( "stationname is out of [50,51]");
    return 999;
  }
  if (stationeta == 1 || stationeta ==-1) seta = ((stationeta == 1)? 1:0);
  else {  ATH_MSG_DEBUG( "stationeta is not 1 nor -1" );
    return 999;
  }
  if (stationphi >=1  || stationeta <=8) sphi = stationphi-1;
  else {  ATH_MSG_DEBUG( "stationeta is out of [1,8]" );
    return 999;
  }
  
  return m_module_hashes[sname][seta][sphi];
}




double CscRegDict :: PhiConv(double phi){
  
  if (phi>M_PI) {
    phi -= 2*M_PI;
  }
  
  return phi;
}


ReturnCode CscRegDict :: initializeHashDictionary(){
  
  int hash = 0;
  for (int sname=0; sname<2; ++sname) {
    for (int seta=0; seta<2; ++seta) {
      for (int sphi=0; sphi<8; ++sphi) {
        m_module_hashes[sname][seta][sphi] = hash;
        ATH_MSG_DEBUG( "module_hash:" << hash << " StationName:" << sname+50 << " StationEta:" << (seta ? 1 : -1) << " StationPhi:" << sphi+1 );
        ++hash;
      }
    }
  }
  return 0;
}



Amg::Vector3D CscRegDict::nomalVector(int module){
    
  double phi=m_reg_dict[module].phiCen;
  double theta=m_reg_dict[module].idealAtanNormal;
  Amg::Vector3D nomvec( sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
  
  return nomvec;
  
}
  
  
double CscRegDict  :: posCorrectionR(int module, int charge/*0 or 1*/){
  
  if (0==charge) return m_reg_dict[module].posCorrectionMinusR;
  if (1==charge) return m_reg_dict[module].posCorrectionPlusR;
  
  return 0.0;
}
  

double CscRegDict :: posCorrectionZ(int module, int charge/*0 or 1*/){
  
  double tan = this->idealAtanNormal(module);
  
  if (0==charge) return (-1)*tan*( m_reg_dict[module].posCorrectionMinusR );
  if (1==charge) return (-1)*tan*( m_reg_dict[module].posCorrectionPlusR );
  
  return 0.0;
}
  



double UtilTools :: calc_phi(double x, double y){
  
  double /*abs_x=fabs(x),*/ abs_y=fabs(y);
  double abs_sine=abs_y/sqrt(x*x+y*y);
    //std::cout << "convert " << ASin(abssin) << std::endl;
  
  if (x>0 && y>=0) {
    return asin(abs_sine);
  }else if(x<=0 && y>0){
    return M_PI-asin(abs_sine);
  }else if(x<0 && y<=0){
    return M_PI+asin(abs_sine)-2*M_PI;
  }else if(x>=0 && y<0){
    return 2*M_PI-asin(abs_sine)-2*M_PI;
  }else{
      //std::cout<< "UtilTools::calc_phi, sin=cos=0" << std::endl;
    return 0.;
  }
  
}


double UtilTools :: calc_dphi(double phi1, double phi2){
  
  double dphi=phi1-phi2;
  
  if (dphi > M_PI) {
    dphi -= 2*M_PI;
  }else if(dphi < (-1)*M_PI){
    dphi += 2*M_PI;
  }
  
  return dphi;
  
}


double UtilTools :: average_phi(double phi1, double phi2){
  
  double phi = 0.;
  
  if (phi1*phi2<0. && fabs(phi1)>M_PI/2.){
    
    double tmp1 = (phi1>0.) ? phi1 - M_PI : phi1 + M_PI;
    double tmp2 = (phi2>0.) ? phi2 - M_PI : phi2 + M_PI;
    
    double tmp = (tmp1+tmp2)/2.;
    
    phi = (tmp>0.) ? tmp - M_PI : tmp + M_PI;
    
  } else {
    
    phi = (phi1+phi2)/2.;
    
  }
  
  return phi;
}



ReturnCode CscRegDict :: initializeDictionaryForData(){
  
  return ReturnCode::SUCCESS;
}



ReturnCode CscRegDict :: initializePosCorrectionParameters(){
  
  
  return ReturnCode::SUCCESS;
}


}//namespace TrigL2MuonSA
