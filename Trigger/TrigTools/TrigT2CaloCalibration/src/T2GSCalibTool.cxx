/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     T2GSCalibTool.cxx
// PACKAGE:  Trigger/TrigTools/TrigT2CaloCalibration
//
// AUTHOR:   N. Anjos <nuno.anjos@cern.ch> 
//
//   Tool to apply a Glogbal Sequential Calibration for L2 trigger jets. 
//   It is based in JetCalibTools/JetPropertyInvertResponseTool.
//   Correction factors are defined through jobO.
//  
// ********************************************************************

#include "TrigT2CaloCalibration/T2GSCalibTool.h"
#include <cmath>


T2GSCalibTool::T2GSCalibTool(const std::string& t, const std::string& n,
			       const IInterface*  p ) : AthAlgTool(t,n,p),	m_log(msgSvc(), n)	
{
 
  declareInterface<IT2HadCalibTool>(this);

  declareConstant("CalibConstants",     m_calibConstants);
  declareProperty("EtaBins",            m_etaBins);
  declareProperty("EtaMin",             m_etaMin);
  declareProperty("EtaMax",             m_etaMax);
  declareProperty("EtaGapMin",          m_etaGapMin = 0);
  declareProperty("EtaGapMax",          m_etaGapMax = 0);
  declareProperty("PtBins",             m_ptBins);
  declareProperty("PtMin",              m_ptMin = 10.*1000);
  declareProperty("PtMax",              m_ptMax =  2.*1000*1000);
  declareProperty("JetPropertyBins",    m_propertyBins);

}

 
T2GSCalibTool::~T2GSCalibTool() {
}


StatusCode T2GSCalibTool::setProperty (const std::string& propname,
                                             const std::string& value){
  StatusCode sc = AthAlgTool::setProperty (propname, value);
  if (sc.isFailure())
    return sc;
  sc=CaloRec::ToolWithConstantsMixin::setProperty (propname, value);
  if (sc.isFailure())
    return sc;
  return StatusCode::SUCCESS;  
}

StatusCode T2GSCalibTool::setProperty (const Property& p){
  StatusCode sc=AthAlgTool::setProperty (p);
  if (sc.isFailure())
    return sc;
  sc=CaloRec::ToolWithConstantsMixin::setProperty (p);
  if (sc.isFailure())
    return sc;
  return StatusCode::SUCCESS;
}   


StatusCode T2GSCalibTool::initialize()
{
  // Initialize MsgStream
  m_log.setLevel(outputLevel());
  return StatusCode::SUCCESS;  
}


StatusCode T2GSCalibTool::finalize()
{
 return StatusCode::SUCCESS;
}


double T2GSCalibTool::c_energy(double fLayer,double jetEnergy,double eta)
{
  double et        = jetEnergy/cosh(eta);
  double response  = CalculateCorrection(fLayer, et, eta);
  return response;
}


double T2GSCalibTool::c_febenergy(double fLayer,double jetEnergy,double eta)
{
  return c_energy(fLayer,jetEnergy,eta);
}


double T2GSCalibTool::CalculateCorrection(double jetProperty, 
					  double pT, double eta)
{
  // Say hello
  if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE 
      << "In JetPropertyInvertResponseTool::CalculateCorrection" << endreq;

  int eta_index = -1;  
  double corr   = 0;

  if(eta<m_etaMin || eta>m_etaMax 
     || (eta>m_etaGapMin && eta<m_etaGapMax)
     || pT<m_ptMin || pT>m_ptMax) return 1.;

  // This correction can be asymmetric in eta, so go across the whole range
  if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE 
      << "Looking up jet eta position in table" << endreq;
  for(unsigned int i=0; i < m_etaBins.size() - 1; i++) {
    if (eta < m_etaBins.at(0)) eta_index = 0;
    else if (eta >= m_etaBins.at(m_etaBins.size()-1)) eta_index = m_etaBins.size();
    else if (eta >= m_etaBins.at(i) && eta < m_etaBins.at(i+1)) {
      eta_index = i+1;
      break;
    }
  }
      
  // Checking if the correction can be interpolated or extrapolated
  if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "Checking if pT and fraction are in the range of the correction" << endreq;
  // if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << " m_ptbins size"<< m_ptBins.size()  << "   " << m_propertyBins.size() << endreq;
  // if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << " eta_index"<< eta_index << "   " << m_ptBins[eta_index].size()-1 << endreq;

  double maxX = (m_ptBins[eta_index][m_ptBins[eta_index].size()-1]
		 +m_ptBins[eta_index][m_ptBins[eta_index].size()-2])/2;
  double maxY = (m_propertyBins[eta_index][m_propertyBins[eta_index].size()-1]
		 +m_propertyBins[eta_index][m_propertyBins[eta_index].size()-2])/2;

  double minX = m_ptBins[eta_index][0];
  double minY = m_propertyBins[eta_index][0];

  if(pT<minX || jetProperty<minY) return 1.;

  bool interp = !(pT>maxX || jetProperty>maxY);

  corr = interp 
    ? interpolate(pT, jetProperty, 
		  m_ptBins[eta_index], 
		  m_propertyBins[eta_index], 
		  m_calibConstants[eta_index]) 
    : 0;
  if(corr == 0){
    corr = extrapolate(pT, jetProperty, 
		       m_ptBins[eta_index], 
		       m_propertyBins[eta_index], 
		       m_calibConstants[eta_index]);


  }
  if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "Obtained correction value" << endreq;
  
  // Print out some info for debugging
  if(outputLevel() <= MSG::DEBUG) m_log << MSG::DEBUG 
                                        << " eta: "            << eta 
                                        << " index: "          << eta_index 
                                        << " property value: " << jetProperty 
//                                        << " property: "       << m_propertyName
                                        << " response = "      << corr
                                        << endreq;
  
  return corr;
}


double T2GSCalibTool::interpolate(double pT, double jetProperty,
				  std::vector<double> ptBins,
				  std::vector<double> propertyBins,
				  CaloRec::Array<2> responseValue)
{
  int pt_low_index = -1;
  int prop_low_index = -1;

  for(unsigned int i=0; i < ptBins.size() - 1; i++) {
    if (pT >= ptBins.at(i) && pT < ptBins.at(i+1)) {
      pt_low_index = (int) i;
      break;
    }
  }

  for(unsigned int i=0; i < propertyBins.size() - 1; i++) {
    if (jetProperty >= propertyBins.at(i) && jetProperty < propertyBins.at(i+1)) {
      prop_low_index = (int) i;
      break;
    }
  }

  if ((outputLevel() <= MSG::VERBOSE) && (pt_low_index == -1)){
    m_log << MSG::VERBOSE << "Problem in interpolation pt_low_index == -1" << endreq;
    return 1;
  }
  if ((outputLevel() <= MSG::VERBOSE) && (prop_low_index == -1)){
    m_log << MSG::VERBOSE << "Problem in interpolation prop_low_index == -1" << endreq;
    return 1;
  }

  //calculate with respect to bin centers, not bin edges

  if(pT<(ptBins[pt_low_index]+ptBins[pt_low_index+1])/2.)
    pt_low_index--;
  if(jetProperty<(propertyBins[prop_low_index]+propertyBins[prop_low_index+1])/2.)
    prop_low_index--;

  if(pt_low_index == (int) ptBins.size()-2 
     && pT>=(ptBins[pt_low_index]+ptBins[pt_low_index+1])/2.)
    pt_low_index++;

  if(prop_low_index == (int) propertyBins.size()-2 
     && jetProperty>=(propertyBins[prop_low_index]+propertyBins[prop_low_index+1])/2.)
    prop_low_index++;


  double response = 1;
  if(pt_low_index != -1 && prop_low_index != -1
     && pt_low_index != (int) ptBins.size()-1 
     && prop_low_index != (int) propertyBins.size()-1){

    //perform bilinear interpolation
    
    double x1, x2, y1, y2, z11, z12, z21, z22;
    
    x1 = (ptBins[pt_low_index]+ptBins[pt_low_index+1])/2.;
    x2 = (ptBins[pt_low_index+1]+ptBins[pt_low_index+2])/2.;

    y1 = (propertyBins[prop_low_index]+propertyBins[prop_low_index+1])/2.;
    y2 = (propertyBins[prop_low_index+1]+propertyBins[prop_low_index+2])/2.;

    z11 = responseValue[pt_low_index][prop_low_index];
    z12 = responseValue[pt_low_index][prop_low_index+1];
    z21 = responseValue[pt_low_index+1][prop_low_index];
    z22 = responseValue[pt_low_index+1][prop_low_index+1];

    response = bilinearInterpolation(x1, y1, x2, y2, z11, z12, z21, z22, pT, jetProperty);
      
  }else if((pt_low_index == -1 || pt_low_index == (int) ptBins.size()-1) &&
	   (prop_low_index == -1 || prop_low_index == (int) propertyBins.size()-1)){

    //No interpolation possible
    //Just return the value of the corresponding bin
    int binx = pt_low_index   == -1 ? 0 : pt_low_index-1;
    int biny = prop_low_index == -1 ? 0 : prop_low_index-1;
    
    response = responseValue[binx][biny];
  }else{

    //interpolation possible in one direction.
    //do linear interpolation
    double x1, x2, z1, z2, x;

    if(pt_low_index == -1 || pt_low_index == (int) ptBins.size()-1){

      x1 = (propertyBins[prop_low_index]+propertyBins[prop_low_index+1])/2.;
      x2 = (propertyBins[prop_low_index+1]+propertyBins[prop_low_index+2])/2.;
      x  = jetProperty;
      
      int binx = pt_low_index   == -1 ? 0 : pt_low_index-1;
      
      z1 = responseValue[binx][prop_low_index];
      z2 = responseValue[binx][prop_low_index+1];

    }else{

      x1 = (ptBins[pt_low_index]  +ptBins[pt_low_index+1])/2.;
      x2 = (ptBins[pt_low_index+1]+ptBins[pt_low_index+2])/2.;
      x  = pT;
      
      int biny = prop_low_index   == -1 ? 0 : prop_low_index;
      
      z1 = responseValue[pt_low_index][biny];
      z2 = responseValue[pt_low_index+1][biny];

    }

    response = linearInterpolation(x1, x2, z1, z2, x);
  }



  //if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "Interpolated value " << pT << " " << pt_low_index << " " << ptBins.at(pt_low_index) << " " << ptBins.at(pt_low_index+1) << " " << jetProperty << " " << prop_low_index << " " << propertyBins.at(prop_low_index) << " " << propertyBins.at(prop_low_index+1) << endreq;
  return response;    

}


double T2GSCalibTool::extrapolate(double pT, double jetProperty,
				  std::vector<double> ptBins,
				  std::vector<double> propertyBins,
				  CaloRec::Array<2> responseValue)
{
  //find closest point and take that one
  int indx = -1;
  int indy = -1;
  double tempDR = 1e7;
  double dR = 1e7;

  double maxX = (ptBins[ptBins.size()-1]+ptBins[ptBins.size()-2])/2.;
  double maxY = (propertyBins[propertyBins.size()-1]+propertyBins[propertyBins.size()-2])/2;

  double minX = ptBins[0];
  double minY = propertyBins[0];

  double extrapolated = 1;

  for(int i = 0; i<(int) ptBins.size()-1; i++){
    for(int j = 0; j<(int) propertyBins.size()-1; j++){
      double xs, ys;	
      xs = (ptBins[i+1]+ptBins[i])/2;
      ys = (propertyBins[j+1]+propertyBins[j])/2;
      //distance calculated differently for different cases
      //find closest on the lower part
      if(pT>=maxX){
	tempDR=jetProperty-ys > 0 ? jetProperty-ys : 1e7;
      }else if(jetProperty>=maxY){
	tempDR= pT-xs > 0 ? pT-xs : 1e7;
      }else{
	tempDR = sqrt(pow((pT-xs)/(maxX-minX),2)
		      +pow((jetProperty-ys)/(maxY-minY),2));
      }
      if(tempDR<dR){
	dR = tempDR;
	indx = i;
	indy = j;
      }
    }
  }

  if(pT>=maxX) indx = ptBins.size()-2;
  if(jetProperty>=maxY) indy = propertyBins.size()-2;

  if(indx == -1) indx = 0;
  if(indy == -1) indy = 0;

  if(indx == (int) ptBins.size()-2 && indy == (int) propertyBins.size()-2){
    extrapolated = responseValue[indx][indy];
  }else{
    double x1, x2, z1, z2, x;
    if(indx == (int) ptBins.size()-2){
      x1 = (propertyBins[indy]  +propertyBins[indy+1])/2;
      x2 = (propertyBins[indy+1]+propertyBins[indy+2])/2;
      x  = jetProperty;
      z1 = responseValue[indx][indy];
      z2 = responseValue[indx][indy+1];
    }else{
      x1 = (ptBins[indx]  +ptBins[indx+1])/2;
      x2 = (ptBins[indx+1]+ptBins[indx+2])/2;
      x  = pT;
      z1 = responseValue[indx][indy];
      z2 = responseValue[indx+1][indy];
    }
    extrapolated = linearInterpolation(x1, x2, z1, z2, x);
  }
  
  if (outputLevel() <= MSG::VERBOSE) m_log << MSG::VERBOSE << "Extrapolated correction value " << extrapolated << " " << indx << " " << indy << " " << jetProperty << endreq;
  
  return extrapolated;

}


double T2GSCalibTool::bilinearInterpolation(double x1, double y1, 
					    double x2, double y2, 
					    double z11,double z12,
					    double z21,double z22,
					    double x, double y)
{
  double zInt1 = z11+(z21-z11)*(x-x1)/(x2-x1);
  double zInt2 = z12+(z22-z12)*(x-x1)/(x2-x1);

  double zInt  = zInt1+(zInt2-zInt1)*(y-y1)/(y2-y1);
  return zInt;
}


double T2GSCalibTool::linearInterpolation(double x1, double x2, 
					  double z1,double z2,
					  double x)
{
  return z1+(z2-z1)*(x-x1)/(x2-x1);
}

