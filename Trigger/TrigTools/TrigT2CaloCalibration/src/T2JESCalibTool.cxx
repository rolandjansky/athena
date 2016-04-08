/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2JESCalibTool.cxx
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHOR:   N. Anjos <nuno.anjos@cern.ch> 
//
//   Tool to apply a JES calibration for L2 trigger jets. 
//   It is based in JetCalibTools/EtaMassEnergyCorr.
//   Correction factors are defined through jobO.
//  
//   Energy arguments to public function, c_energy, must be specified
//     as transverse energy in MeV.
//
// ********************************************************************

#include "TrigT2CaloCalibration/T2JESCalibTool.h"
#include <cmath>


T2JESCalibTool::T2JESCalibTool(const std::string& t, const std::string& n,
			       const IInterface*  p ) : AthAlgTool(t,n,p),	m_log(msgSvc(), n)	
{
 
 declareInterface<IT2HadCalibTool>(this);
 m_etcut = 10; //GeV
 declareProperty("MinEtCut",m_etcut);
 declareConstant("JES_Factors", m_EnergyCorrConstants);
}

 
T2JESCalibTool::~T2JESCalibTool() {
}


StatusCode T2JESCalibTool::setProperty (const std::string& propname,
                                             const std::string& value){
  StatusCode sc = AthAlgTool::setProperty (propname, value);
  if (sc.isFailure())
    return sc;
  sc=CaloRec::ToolWithConstantsMixin::setProperty (propname, value);
  if (sc.isFailure())
    return sc;
  return StatusCode::SUCCESS;  
}

StatusCode T2JESCalibTool::setProperty (const Property& p){
  StatusCode sc=AthAlgTool::setProperty (p);
  if (sc.isFailure())
    return sc;
  sc=CaloRec::ToolWithConstantsMixin::setProperty (p);
  if (sc.isFailure())
    return sc;
  return StatusCode::SUCCESS;
}   


StatusCode T2JESCalibTool::initialize()
{
  // Initialize MsgStream
  m_log.setLevel(outputLevel());
  return StatusCode::SUCCESS;  
}


StatusCode T2JESCalibTool::finalize()
{
 return StatusCode::SUCCESS;
}


double T2JESCalibTool::c_energy(double EMe,double HADe,double eta)
{
// Internally we must work in GeV
// but the function arguments are specified in MeV
// and the returned value as well in MeV

  double GeV=1000;
  double EMscale_e = (EMe + HADe)/GeV;
  double etf = 1./cosh(eta);
  double EMscale_et = etf * EMscale_e;

  if ( EMscale_et < m_etcut ) {
    if ( outputLevel() <= MSG::DEBUG )
      m_log << MSG::DEBUG << "Cluster transv. energy :" << EMscale_et 
            << ", below threshold :" << m_etcut << endreq;
    return 0.0;
  }

  double JES_weight = GetJES(EMscale_e,eta);
  return JES_weight*EMscale_e*GeV;

}


double T2JESCalibTool::c_febenergy(double EMe,double HADe,double eta)
{
  return c_energy(EMe,HADe,eta);
}


double T2JESCalibTool::GetJES(double jet_e, double jet_eta)
{
  double E = jet_e;
  if ( E < m_etcut*cosh(jet_eta) ) 
    E= m_etcut*cosh(jet_eta);

  // Get the eta bin index
  unsigned ieta = GetEtaBin(jet_eta);

  // Calculate the jet response and then the JES as 1/R
  double R=PolLogE(m_EnergyCorrConstants,ieta,E);
  return 1.0/R;
}


double T2JESCalibTool::PolLogE(const CaloRec::Array<2> &constants, 
                                   unsigned bin, double E)
{
  //up to 7 parameter for the pol-fit
  unsigned Npar=7;
  double logE=log(E);
  double y=0;
  for (unsigned i=0;i<Npar;++i)
    y += constants[bin][i]*pow(logE,i);
  return y;
}

  
int T2JESCalibTool::GetEtaBin(double jet_eta)
{
  //90 eta bins
  int ieta=int(jet_eta*10)+45;
  if (jet_eta<0) ieta-=1;
  if ( ieta <  0 ) return 0;
  if ( ieta > 89 ) return 89;
  return ieta;
}

