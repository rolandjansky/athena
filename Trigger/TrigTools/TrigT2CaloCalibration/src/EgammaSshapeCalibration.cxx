/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EgammaSshapeCalibration.h"
//#include "TrigCaloEvent/TrigEMCluster.h"
#include "xAODTrigCalo/TrigEMCluster.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloDetDescr/CaloDetDescriptor.h"
#include "CaloClusterCorrection/interpolate.h"
#include <math.h>
#include <iomanip>

//====================================================================
// initialize
//====================================================================
StatusCode EgammaSshapeCalibration::initialize(){
  m_log = new MsgStream(AthAlgTool::msgSvc(), name() ); 
  CHECK(base_class::initialize());
  (*m_log) << MSG::DEBUG << "Initialize Tool : " << name() << endmsg;
  return StatusCode::SUCCESS;
}

//====================================================================
// finalize
//====================================================================
StatusCode EgammaSshapeCalibration::finalize(){
  delete m_log;
  return StatusCode::SUCCESS;  
}

//====================================================================
// EgammaSshapeCalibration::makeCorrection
//====================================================================
void EgammaSshapeCalibration::makeCorrection(xAOD::TrigEMCluster* clus,
					     const void *arg){
  
  if(arg!=0){
    const CaloDetDescrElement *caloDDE = (const CaloDetDescrElement*) arg;

    bool isRange_barrel = m_isRange_barrel();

#ifndef NDEBUG    
    (*m_log) << MSG::DEBUG << "caloDDE->descriptor()->is_lar_em_barrel() = " 
	     << caloDDE->descriptor()->is_lar_em_barrel() << endmsg;
    (*m_log) << MSG::DEBUG << "caloDDE->descriptor()->is_lar_em_endcap() = " 
	     << caloDDE->descriptor()->is_lar_em_endcap() << endmsg;

    (*m_log) << MSG::DEBUG << "m_isRange_barrel=" << isRange_barrel << endmsg;

    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::PreSamplerB) = " 
	     << clus->energy(CaloSampling::PreSamplerB) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EMB1)        = " 
	     << clus->energy(CaloSampling::EMB1) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EMB2)        = " 
	     << clus->energy(CaloSampling::EMB2) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EMB3)        = " 
	     << clus->energy(CaloSampling::EMB3) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::PreSamplerE) = " 
	     << clus->energy(CaloSampling::PreSamplerE) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EME1)        = " 
	     << clus->energy(CaloSampling::EME1) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EME2)        = " 
	     << clus->energy(CaloSampling::EME2) << endmsg;
    (*m_log) << MSG::DEBUG << "clus->energy(CaloSampling::EME3)        = " 
	     << clus->energy(CaloSampling::EME3) << endmsg;

    if((caloDDE->descriptor()->is_lar_em_barrel() &&  isRange_barrel) ||
       (caloDDE->descriptor()->is_lar_em_endcap() && !isRange_barrel))
      (*m_log) << MSG::DEBUG 
	       << "[GOOD]: seedCell location and selected eta range agree" << endmsg;
    else if((caloDDE->descriptor()->is_lar_em_barrel() && !isRange_barrel) ||
	    (caloDDE->descriptor()->is_lar_em_endcap() &&  isRange_barrel))
      (*m_log) << MSG::DEBUG 
	       << "[BAD]: seedCell location and selected eta range disagree !!" << endmsg;
#endif
  
    // check if seedCell is in barrel or end-cap for correct range selection
    if((caloDDE->descriptor()->is_lar_em_barrel() && !isRange_barrel) ||
       (caloDDE->descriptor()->is_lar_em_endcap() &&  isRange_barrel))
      return;
    
    double eta      = clus->eta();     // cluster position in eta     
    double elt_eta  = caloDDE->eta();  // seedCell position in eta
    double elt_deta = caloDDE->deta(); // seedCell width in eta

    // Find u, the normalized displacement of the cluster within the cell
    // in the range -1...1, with 0 being at the center.
    double u = 2*(eta-elt_eta)/elt_deta; // position within cell
    
    // cluster position in eta without accounting for alignment corrections
    double elt_eta_raw = clus->rawEta(); 
    
    if(elt_eta_raw<0.) u=-u;
   
    // Sanity-check of valid u-range...
    if(u>1) 
      u=1;
    else if(u<-1)   
      u=-1;                 
   
    // The eta adjusted for any shift between the actual and nominal coordinates.    
    double aeta = fabs(eta - elt_eta + elt_eta_raw);

#ifndef NDEBUG    
    (*m_log) << MSG::DEBUG << "eta         = " << eta         << endmsg;
    (*m_log) << MSG::DEBUG << "elt_eta     = " << elt_eta     << endmsg;
    (*m_log) << MSG::DEBUG << "elt_deta    = " << elt_deta    << endmsg;
    (*m_log) << MSG::DEBUG << "elt_eta_raw = " << elt_eta_raw << endmsg;
    (*m_log) << MSG::DEBUG << "u           = " << u           << endmsg; 
    (*m_log) << MSG::DEBUG << "aeta        = " << aeta        << endmsg; 
#endif
    
    // Find the appropriate region 
    int region_ndx=-1;
    const CxxUtils::Array<2> regions = m_regions();
    unsigned int nreg = regions.size();

    // find correct region
    for (unsigned int i=0; i<nreg; i++) {
      if(aeta>=regions[i][REG_LO] && aeta<regions[i][REG_HI]){
	region_ndx=i;
	break;
      }
    }

    // Sanity-check for region index...
    if(region_ndx<0){
      return;
    }

    // In a few regions, the fit was done using a cell size different
    // from what we actually have.  Need to recalculate u in this case.
    if(std::abs(regions[region_ndx][REG_CELLSIZE]-elt_deta) > 1e-3) {
      float cellsize = regions[region_ndx][REG_CELLSIZE];
      u = fmod(aeta,cellsize)/cellsize*2 - 1;
    }

    float energy = clus->energy();

    Builder *builder = new Builder(*this, aeta, u, region_ndx);
    
    // Calculate the correction for each energy.
    const CxxUtils::Array<1> energies = m_energies();
    unsigned int n_energies = energies.size();
    unsigned int shape[] = {n_energies, 2};
    CaloRec::WritableArrayData<2> corrtab (shape);
    
    // If we're outside the range of the table, we'll just be using the
    // value at the end (no extrapolation).  We only need to calculate
    // that one point in that case.
    unsigned int beg = 0;
    unsigned int end = n_energies;
    if(energy <= energies[0]) // ok. energies also in MeV
      end = 1;
    else if(energy >= energies[n_energies-1])// ok. energies also in MeV
      beg = n_energies-1;
    
    // Build the table.
    int n_good = 0;
    for (unsigned int i=beg; i<end; i++)
      docalc(i, *builder, energies, corrtab, n_good); 
    
    // If we only evaluated one point, but it wasn't good, keep
    // searching until we find a good one.
    while (n_good==0 && beg>0) {
      --beg;
      docalc (beg, *builder, energies, corrtab, n_good);
    }
    while (n_good == 0 && end<n_energies) {
      docalc(end, *builder, energies, corrtab, n_good);
      ++end;
    }
    
    // Now interpolate in energy.
    // But if we're outside of the range of the table, just use the value
    // at the end (don't extrapolate).
    
    float offs=0;
    
    if(n_good==0){ // No good energies --- return a null correction.
      offs=0;  
    }
    else if(n_good==1){ // Only one good energy --- nothing to do but to use it.
      offs=corrtab[0][1]; 
    } 
    else if(energy<=corrtab[0][0]){ // Off the low end of the table --- return the first value.
      offs=corrtab[0][1];
    }
    else if(energy>=corrtab[n_good-1][0]) { // Off the high end of the table --- return the last value.
      offs=corrtab[n_good-1][1];
    }
    else{ // Do the interpolation.
      offs = CaloClusterCorr::interpolate(corrtab, 
					  energy, 
					  m_energy_degree(),
					  1, 
					  CaloRec::Array<1>(), 
					  n_good);
    }
    
    if (eta<0) 
      offs=-offs;

    clus->setEta(eta+offs);  

#ifndef NDEBUG     
    (*m_log) << MSG::DEBUG << "Before correction : " << eta << endmsg;
    (*m_log) << MSG::DEBUG << "offset =" << offs << endmsg;
    (*m_log) << MSG::DEBUG << "After correction : " << eta+offs << endmsg;
#endif
    
    delete builder;
  }
}

//====================================================================
// EgammaSshapeCalibration::docalc
//====================================================================
inline void EgammaSshapeCalibration::docalc (int i,
					     const CaloClusterCorrectionCommon::TableBuilder& builder,
					     const CaloRec::Array<1>& energies,
					     CaloRec::WritableArray<2>& corrtab,
					     int& n_good) {
  corrtab[n_good][0] = energies[i];
  bool good = false;
  corrtab[n_good][1] = builder.calculate (i, good);
  if (good)
    ++n_good;
}


//====================================================================
//
// Builder constructor
//
//====================================================================
EgammaSshapeCalibration::Builder::Builder (const EgammaSshapeCalibration& corr,
					   double aeta,
					   double u,
					   int region_ndx)
  : m_corr(corr),
    m_aeta(aeta),
    m_u(u),
    m_region_ndx(region_ndx),
    m_form (m_corr.m_regions()[region_ndx][REG_FORM])
{ }

//====================================================================
// EgammaSshapeCalibration::Builder::calculate
//====================================================================
float EgammaSshapeCalibration::Builder::calculate(int energy_ndx, 
						  bool& good) const {

  // Find the proper array of coefficients.
  CaloRec::Array<2> coef = m_corr.m_correction()[energy_ndx][m_region_ndx];
  
  // If we don't have coefficients for this energy/region, skip it.
  if(coef[0].end()[-1]==0) {
    good = false;
    return 0;
  }

  // Which functional form to use?
  int form;
  CxxUtils::Array<2> forms = m_corr.m_forms();
  if (forms.size(0) != 0 && forms.size(1) != 0)
    form = forms[m_region_ndx][energy_ndx];
  else
    form = m_form;


 // Evaluate the correction!
  good = true;
  float ret = 0;
  switch (form) {
  case 0:
    ret = calc0 (m_aeta, m_u, coef);
    break;
  case 3:
    ret = calc3 (m_aeta, m_u, coef);
    break;
  case 4:
    ret = calc4 (m_aeta, m_u, coef);
    break;
  case 5:
    ret = calc5 (m_aeta, m_u, coef);
    break;
  case 10:
    ret = calc10 (m_aeta, m_u, coef);
    break;
  case 11:
    ret = calc11 (m_aeta, m_u, coef);
    break;
  default:
    abort();
  }

  return ret;
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc0
//====================================================================
float EgammaSshapeCalibration::Builder::calc0 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 5;
  double pars[NPAR];

  unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
      double out = xcoef[0];
      for (unsigned int j=1; j<degree; j++)
	out = out * aeta + xcoef[j];
      pars[i] = out;
  }

  double c = (u>=0) ? pars[2] : pars[3];
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[4]);
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc3
//====================================================================
float EgammaSshapeCalibration::Builder::calc3 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 4;
  double pars[NPAR];

  unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
    double out = xcoef[0];
    for (unsigned int j=1; j<degree; j++)
      out = out * aeta + xcoef[j];
    pars[i] = out;
  }
  
  double c = pars[2];
  if (u < 0)
    c = -c - 2*std::atan(pars[1]);
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[3]);
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc4
//====================================================================
float EgammaSshapeCalibration::Builder::calc4 (float aeta,
                                       float u,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 3;
  double pars[NPAR];

  unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
    double out = xcoef[0];
    for (unsigned int j=1; j<degree; j++)
      out = out * aeta + xcoef[j];
    pars[i] = out;
  }
  
  double b = std::max ((double)pars[1], 1e-5);
  double atanb = std::atan(b);
  double sq = std::sqrt (b/atanb - 1);
  double den = (sq/b*atanb - std::atan(sq));
  return  pars[0]* ((- std::atan (b*u) + u*atanb) / den +
                    pars[2]*(1-std::abs(u)));
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc5
//====================================================================
float EgammaSshapeCalibration::Builder::calc5 (float aeta,
                                       float /*u*/,
                                       const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 1;
  double pars[NPAR];

  //  poly_eval (aeta, coef, NPAR, pars);
 unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
      double out = xcoef[0];
      for (unsigned int j=1; j<degree; j++)
	out = out * aeta + xcoef[j];
      pars[i] = out;
  }

  return pars[0];
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc10
//====================================================================
float EgammaSshapeCalibration::Builder::calc10 (float aeta,
                                        float u,
                                        const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 7;
  double pars[NPAR];
  
  unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
    double out = xcoef[0];
    for (unsigned int j=1; j<degree; j++)
      out = out * aeta + xcoef[j];
    pars[i] = out;
  }

  double c = pars[2];
  if (u < 0)
    c = -c - 2*std::atan(pars[1]);

  double offs = pars[0] * (atan (pars[1] * u) + c*u + pars[3]) +
    pars[4] * std::cos (u * pars[5] * M_PI + pars[6]);
  if (u > 0.9 || u < -0.9) {
    double xdiff = pars[4] * (std::cos (pars[5] * M_PI + pars[6]) -
                              std::cos (- pars[5] * M_PI + pars[6]));
    if (u > 0.9)
      offs -= (u-0.9)/0.1 * xdiff/2;
    else
      offs += (-0.9-u)/0.1 * xdiff/2;
  }
  return offs;
}

//====================================================================
// EgammaSshapeCalibration::Builder::calc11
//====================================================================
float EgammaSshapeCalibration::Builder::calc11 (float aeta,
                                        float u,
                                        const CaloRec::Array<2>& coef) const
{
  const unsigned int NPAR = 8;
  double pars[NPAR];

 unsigned int degree = coef.size(1);
  for (unsigned int i=0; i < NPAR; i++) {
    CaloRec::Array<1> xcoef = coef[i];      
      double out = xcoef[0];
      for (unsigned int j=1; j<degree; j++)
	out = out * aeta + xcoef[j];
      pars[i] = out;
  }

  double c = (u>=0) ? pars[2] : pars[3];
  return pars[0] * (std::atan (pars[1] * u) + c*u + pars[4])
    + pars[5] * std::cos (u * pars[6] * M_PI + pars[7]);
}

