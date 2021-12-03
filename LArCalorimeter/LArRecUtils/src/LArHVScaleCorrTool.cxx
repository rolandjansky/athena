/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArHVScaleCorrTool.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>

using CLHEP::microsecond;
using CLHEP::second;

LArHVScaleCorrTool::LArHVScaleCorrTool(const CaloCell_ID* calocell_id, MsgStream& msg, 
				       const std::vector<std::string>& fixHVStrings) :
  m_larem_id(calocell_id->em_idHelper()),
  m_larhec_id(calocell_id->hec_idHelper()),
  m_larfcal_id(calocell_id->fcal_idHelper()),
  m_T0(90.371)
{
  buildFixHVList(fixHVStrings, msg);
}

  
float LArHVScaleCorrTool::getHVScale(const CaloDetDescrElement* calodde,  const voltageCell_t& hvlist,
                                     MsgStream& msg) const {

  double d=.2;  // nominal gap  in cm
  double nominal=2000.;  // nominal HV  in Volts
  double T=88.;  // temperature  in Kelvin
  
  bool isbarrelEM=false;
  bool isFCAL1=false;
  
  unsigned int subdet=99;
  unsigned int layer=99;

  const Identifier offid=calodde->identify();
  const float eta_raw = calodde->eta_raw();
  const float phi_raw = calodde->phi_raw();


  // EM calorimeter
  if (m_larem_id->is_lar_em(offid)) {
    // barrel
    if (abs(m_larem_id->barrel_ec(offid))==1) {
      subdet=0;
      layer = m_larem_id->sampling(offid);
      // EMB
      if (m_larem_id->sampling(offid)>0) {
	d=0.209;
	nominal=2000.;
	T=88.37;
	isbarrelEM=true;
      }
      // EMBPS
      else {
	nominal = 2000.;
	T = 88.37;
	const int ieta=m_larem_id->eta(offid);
	if (ieta>=0 && ieta<16)        d = 0.196; //cm
	else if (ieta>=16 && ieta<32)  d = 0.193; //cm
	else if (ieta>=32 && ieta<48)  d = 0.2; //cm
	else  d = 0.190; //cm
      }
    }
    // endcap
    else {
      subdet=1;
      layer = m_larem_id->sampling(offid);
      // End-Cap pre-sampler
      if (abs(m_larem_id->barrel_ec(offid))==2 && m_larem_id->sampling(offid)==0){
	T = 88.65; // temperature remainder Calorimeter
	nominal = -2000.;
	d = 0.2; 
      }
      //EMEC      
      else {
	T = 88.65;
	float aeta_raw = std::fabs(eta_raw);
	double Zsamp;
	if ( m_larem_id->sampling(offid)==1 && m_larem_id->region(offid)>=0 )  Zsamp = 3760.; //mm
	else if ( m_larem_id->sampling(offid)==2 && m_larem_id->region(offid)<=1 )  Zsamp = 3880.; //mm
	else if(aeta_raw< 2.) Zsamp=4200.-40.*(aeta_raw-1.5);
	else Zsamp = 4180. - 160.*(aeta_raw-2.);
	nominal = EMEC_nominal(aeta_raw);
	d = EMEC_gap(aeta_raw, Zsamp)*0.1; //cm
      }//end EMEC
    }//end endcap
  }//end is-em
  // Forward Calorimeter
  else if ( m_larfcal_id->is_lar_fcal(offid)) {
    subdet=3;
    layer = m_larfcal_id->module(offid);
    T = 88.65;
    if (m_larfcal_id->module(offid)==1){ 
      d =0.0269; //cm
      nominal = 250.;
      isFCAL1=true;
    }
    else  if (m_larfcal_id->module(offid)==2){
      d =0.0376;//cm
      nominal = 375.;
    }
    else { 
      d =0.0508;//cm
      nominal = 500.;
    }
  }
  // Hadronic Calorimeter
  else if ( m_larhec_id->is_lar_hec(offid)) {
    subdet=2;
    layer = m_larhec_id->sampling(offid);
    T = 88.65;
    nominal = 1800.;
    d = 0.196;//cm
  }
  T = T - m_T0;

  const double E_nominal = champ_e(nominal,d);
  
  // dump values
  bool notfound=false;
  for (unsigned int i=0;i<hvlist.size();i++) {
    if (hvlist[i].hv<-10000) notfound=true;
  }

  if (notfound) {
    msg << MSG::WARNING << " At least one HV value not found in database for cell " << m_larem_id->show_to_string(offid) << endmsg;
  }

  double mycorr=0.;
  double mynorm=0.;
  for (unsigned int i=0;i<hvlist.size();i++) {
    double E = champ_e(hvlist[i].hv,d);
    
    // dont correct if E is very close to E nominal to avoid small glitches
    if (std::fabs(E_nominal)>1e-3) {
      const double deviation = std::fabs((E-E_nominal)/E_nominal);
      if (deviation<1e-3) E = E_nominal;
    }

    // barrel accordion
    if (isbarrelEM) {
      const double corr = this->Scale_barrel(hvlist[i].hv)*hvlist[i].weight;
      mycorr += corr;
    }
    //FCAL module 1
    else if (isFCAL1) {
      const double corr = this->Scale_FCAL1(hvlist[i].hv) * hvlist[i].weight;
      mycorr+=corr;
    }
    // other subdetectors
    else {
      const double corr = Respo(E,E_nominal,T)*hvlist[i].weight;
      mycorr+= corr;
    }
    mynorm += hvlist[i].weight;
  }//end loop over hvlist
  

  if (mycorr>1e-2) mycorr = mynorm/mycorr;
  else mycorr=1.;
  
  // Add protection against correction factor > 10
  if (mycorr>10.) {
    msg << MSG::DEBUG << "Correction factor > 10, ignore it... for cell " <<  m_larem_id->show_to_string(offid) << " " << mycorr << endmsg;;
    mycorr=1.;
  }   

  //Overwrite with hardcoded jobOption numbers if requested
  for (unsigned int ii=0;ii<m_HVfix.size();ii++) {
    if (subdet == m_HVfix[ii].subdet && layer >= m_HVfix[ii].layer_min && layer <= m_HVfix[ii].layer_max &&
	eta_raw >= m_HVfix[ii].eta_min && eta_raw < m_HVfix[ii].eta_max &&
	phi_raw >= m_HVfix[ii].phi_min && phi_raw < m_HVfix[ii].phi_max ) {
      msg << MSG::DEBUG << "Overwrite correction for cell " <<  m_larem_id->show_to_string(offid) << " " << m_HVfix[ii].corr << endmsg;;
      mycorr = m_HVfix[ii].corr;
    }
  }
    
  return mycorr;
}

float LArHVScaleCorrTool::champ_e(float hv, float d) const
{
  float e1;
  if (hv < -3000.){ 
    return -1000.;
  }
  else
    e1 = fabs(hv)/(d*1e3);
  if ( e1 < 0.01 ) e1 = 0.01;
  return e1;
}

float LArHVScaleCorrTool::vdrift(float e, float tempe) const
{	    
  const float T = tempe;                
  static const float P[6] = {-0.01481,-0.0075,0.141,12.4,1.627,0.317};
  if ( e < -999.) return 0.;
  float vd = (P[0]*T+1)*( P[2]*e*log(1+ (P[3]/e)) + P[4]*pow(e,P[5])) + P[1]*T; // vdrift formula walcowialk mm/micro_s
  return vd;
}

float LArHVScaleCorrTool::InvCharge(float e) const
// returns 1/charge from recombination effect
{ 
  float q = 1.;
  if ( e > 2.) q=(1.+0.36/e);
  else if (e>1e-6) q =(1.04+0.25/e);
  return q;
}

float LArHVScaleCorrTool::Respo(float e, float e_nominal,float tempe) const
{ 
  if (e < -999.) return 1.;
  if (e < 0.01) return 0;
  if ( e > e_nominal ) return 1;
  float resp = (InvCharge(e_nominal)*vdrift(e,tempe))/(InvCharge(e)*vdrift(e_nominal,tempe));
  return resp;
}

float LArHVScaleCorrTool::t_drift(float e, float e_nominal, float d, float tempe) const
{
  if ( e < -999.) return (d*1e4)/vdrift(e_nominal, tempe) ;
  if (e > e_nominal ) e = e_nominal;
  return (d*1e4)/vdrift(e, tempe); // ns
}

float LArHVScaleCorrTool::EMEC_nominal(const float aeta) const
{
  if ( aeta<1.5 ) return 2500.;
  else if (aeta<1.6) return 2300.;
  else if (aeta<1.8 ) return 2100.;
  else if (aeta < 2.0 ) return 1700.;
  else if (aeta < 2.1 ) return 1500.;
  else if (aeta < 2.3 ) return 1250.;
  else if (aeta < 2.5 ) return 1000.;
  else if (aeta < 2.8 ) return 2300.;
  else return 1800.;
}

float LArHVScaleCorrTool::EMEC_gap(const float aeta, float Zeta) const
{
  float EMECg;
  if (aeta<=2.5 ) EMECg = 0.9 +1.9*(  ( Zeta - 40. )/(10*sinh(aeta)) - 60.)/140.;
  else EMECg = 1.8 + 1.3*(  ( Zeta - 40. )/(10*sinh(aeta)) - 30.)/40.;
  return EMECg;
}

float LArHVScaleCorrTool::Scale_FCAL1(const float hv) const
{
  if (hv<5.0) return 0;
  const float R0=-2.612;
  const float alpha=2.336;
  const float beta=0.079;
  const float scale=R0+alpha*pow(hv,beta);
  return scale;
}
  


float LArHVScaleCorrTool::Scale_barrel(const float hv) const
{
   const float hvref[18]={1.,50.,100.,150.,200.,300.,400.,500.,600.,700.,800.,900.,1000.,1200.,1400.,1600.,1800.,2000.};
   const float hvmax = 1998.;
   const float facteur[18]={0.,0.1209,0.2135,0.2829,0.3390,0.4270,0.4961,0.5556,0.6065,0.6527,0.6906,
			    0.7290,0.7626,0.8224,0.8754,0.9190,0.9606,1.};

// 0 HV, returns 0 response
   if (hv<-999.) {
      return 0;
   }
   else if (hv<hvref[0]) {
      float resp=facteur[0];
      return resp;
   }
// 2000 HV, returns response=1
   if (hv>hvmax) {
      float resp=facteur[17];
      return resp;
   }

// intermediate HV, find values between which to extrapolate
   int i1,i2;
   i1=1;
   for (int i=0;i<18;i++) {
    if (hv<hvref[i]) {
       i1=i-1;
       break;
    }
   }
   i2=i1+1;
   float resp=0.;

// if lowHV>=50 V, logarithmic extrapolation
   if (i1>0) {
     const float b=(log(facteur[i2])-log(facteur[i1]))/(log(hvref[i2])-log(hvref[i1]));
     const float a=log(facteur[i2])-b*log(hvref[i2]);
     resp = exp(a+b*log(hv));
   } 
// if between 0 and 50 V, scales linearly
   else {
     resp=facteur[0]*(hv/hvref[0]);
   }
   //std::cout << " hv,i1,i2,resp " << hv << " " << i1 << " " << i2 << " " << resp << std::endl;
   return resp;
}


// *** Build list of correction to hardcode by jobOptions
void LArHVScaleCorrTool::buildFixHVList(const std::vector<std::string>& fixHVStrings,
                                        MsgStream& msg) {

  m_HVfix.clear();
  std::vector<std::string>::const_iterator itrStringID=fixHVStrings.begin();
  for (;itrStringID!=fixHVStrings.end();++itrStringID) {
    std::string theString=*itrStringID;
    std::stringstream is;
    is << theString << std::endl;
  
    unsigned int iDetector,ilayer_min,ilayer_max;
    float eta_min,eta_max,phi_min,phi_max,corr;
    is >> iDetector >> ilayer_min >> ilayer_max >> eta_min >> eta_max >> phi_min >> phi_max >> corr;

    HVfix_t myfix{};
    myfix.subdet = iDetector;
    myfix.layer_min = ilayer_min;
    myfix.layer_max = ilayer_max;
    myfix.eta_min = eta_min;
    myfix.eta_max = eta_max;
    myfix.phi_min = phi_min;
    myfix.phi_max = phi_max;
    myfix.corr = corr;
    m_HVfix.push_back(myfix);
  }

  msg << MSG::INFO << "  Number of regions with overwritten HV corrections from jobOptions " << m_HVfix.size() << endmsg;

  return;
}



