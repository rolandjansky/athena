/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 *  ElectronicsResponse.cxx
 *  MC for micromegas athena integration
 *
 **/
//~ 
/// PROJECTS
#include "GaudiKernel/MsgStream.h"
#include "MM_Digitization/ElectronicsResponse.h"
 
/*******************************************************************************/
ElectronicsResponse::ElectronicsResponse()
 : tMinFromIntegration(-1),
   tminFromIntegrationAboveThreshold(-1),
   tMinFromFirstPeak(-1)
{
  //stripObject = 0;
  stripObject = new StripsResponse();
  m_StripResponse_qThreshold = stripObject->get_qThreshold();
  m_StripResponse_driftGap = stripObject->get_driftGap();
  m_StripResponse_driftVelocity = stripObject->get_driftVelocity();
//  if(stripObject) delete stripObject;
//  clearValues();
  alpha = 2.5;
  RC = 20.0;
  electronicsThreshold = (m_StripResponse_qThreshold * ( TMath::Power(alpha,alpha)*TMath::Exp(-alpha)) ) ;
       
  //---------------------------------------------------------
   
  intFn = new TF1("intFn","[3]*TMath::Power((x-[2])/[1],[0])*TMath::Exp(-(x-[2])/[1])", 0, 3*( m_StripResponse_driftGap/m_StripResponse_driftVelocity ) );
  intFn->SetParameter( 0, alpha);
  intFn->SetParameter( 1, RC);
}
/*******************************************************************************/
void ElectronicsResponse::clearValues()
{
  tMinFromIntegration = -1;
  tminFromIntegrationAboveThreshold = -1;
  tMinFromFirstPeak = -1;
  tStripElectronicsAbThr.clear();
  qStripElectronics.clear();
  nStripElectronics.clear();
}
/*******************************************************************************/
MmDigitToolOutput ElectronicsResponse::GetResponceFrom(const MmElectronicsToolInput & digiInput)
{
  clearValues();
  bnlResponceFunction(digiInput.NumberOfStripsPos(), digiInput.chipCharge(), digiInput.chipTime() );
  /// ToDo: include loop for calculating Trigger study vars
  // MmDigitToolOutput(bool hitWasEff, std::vector <int> strpos, std::vector<float> time, std::vector<int> charge, int strTrig, float strTimeTrig ):     
  MmDigitToolOutput tmp(true, nStripElectronics, tStripElectronicsAbThr, qStripElectronics, 5, 0.3);
  return tmp;
}
/*******************************************************************************/
void ElectronicsResponse::bnlResponceFunction(const vector <int> & numberofStrip, const vector <float> & qStrip, const vector <float> & tStrip){ 

  //int minStrip=1000000, maxStrip=-10000000;
  //tStripElectronicsAbThr.clear();
  //nStripElectronics.clear();
  //qStripElectronics.clear();

  //stripObject = new StripsResponse();
  //cout << "stripObject->get_qThreshold()" << stripObject->get_qThreshold() << endl;
  //cout << "m_StripResponse_driftGap" << m_StripResponse_driftGap << endl;
  //cout << "m_StripResponse_driftVelocity" << m_StripResponse_driftVelocity << endl;
  
    
  for (unsigned int ii = 0; ii < numberofStrip.size(); ii++) {
    
    //   cout << "----------for strip number " << numberofStrip.at(ii) << "---------" << endl;
    //cout << "time max: " << 3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity) << endl;
    //cout << "charge at (0,50,100,200): " << intFn->Eval(0,0,0) << ", " << intFn->Eval(50,0,0) << ", " << intFn->Eval(100,0,0) << ", " << intFn->Eval(200,0,0) << endl;
    
    //find min and max times for each strip:
    
    //maxChargeThisStrip = intFn->GetMaximum(0, 3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity), 1.0E-10, 100, false);
    //maxChargeThisStrip = intFn->GetMaximum(50,300);
    //maxChargeThisStrip = intFn->GetMaximum(tStrip.at(ii),3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity) );
    

    //cout << "maxChargeThisStrip(with tmin = tstrip): " << maxChargeThisStrip << endl;
    
    double maxChargeThisStrip = 0;
    double maxChargeLeftNeighbor = 0;
    double maxChargeRightNeighbor = 0;
  
    Athena::MsgStreamMember log("ElectronicsResponse::bnlResponceFunction"); 
    // find the maximum charge:
    if ( ii > 0 ) {
      log << MSG::DEBUG << "for Left neighbor:   tStrip.at(ii-1) "<< tStrip.at(ii-1) << " qStrip.at(ii-1) " << qStrip.at(ii-1) << endreq;    
      intFn->SetParameter( 2, tStrip.at(ii-1));
      intFn->SetParameter( 3, qStrip.at(ii-1));
      maxChargeLeftNeighbor = intFn->GetMaximum(tStrip.at(ii-1),3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));
    }
 
    if ( ii+1 < numberofStrip.size() ) {
      log << MSG::DEBUG << "for Right neighbor:   tStrip.at(ii+1) "<< tStrip.at(ii+1) << " qStrip.at(ii+1) " << qStrip.at(ii+1) << endreq;
      intFn->SetParameter( 2, tStrip.at(ii+1));
      intFn->SetParameter( 3, qStrip.at(ii+1));
      maxChargeRightNeighbor = intFn->GetMaximum(tStrip.at(ii+1),3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));    
    }

    if ( ii > 0 ) {
      log << MSG::DEBUG << "for This strip:    tStrip.at(ii) "<< tStrip.at(ii) << " qStrip.at(ii) " << qStrip.at(ii) << endreq;
      intFn->SetParameter( 2, tStrip.at(ii));
      intFn->SetParameter( 3, qStrip.at(ii));
      maxChargeThisStrip = intFn->GetMaximum(tStrip.at(ii),3*(m_StripResponse_driftGap/m_StripResponse_driftVelocity));
    }
    
    log << MSG::DEBUG << "Check if a strip or its neighbors were above electronics threshold (" << electronicsThreshold << "):   maxChargeLeftNeighbor: " << maxChargeLeftNeighbor << ", maxChargeRightNeighbor: " << maxChargeRightNeighbor << ", maxChargeThisStrip: " << maxChargeThisStrip << endreq; 
     
    // Look at strip if it or its neighbor was above threshold:
    if ( maxChargeLeftNeighbor > electronicsThreshold || maxChargeRightNeighbor > electronicsThreshold || maxChargeThisStrip > electronicsThreshold ) {
	
      intFn->SetParameter( 2, tStrip.at(ii));
      intFn->SetParameter( 3, qStrip.at(ii));

      float localPeak = 0;
      float localPeakt = 0;
      float localPeakq = 0;

      float stepSize = 0.1; //ns(?) step size corresponding to VMM discriminator

      for (int jj = tStrip.at(ii); jj < (3*( m_StripResponse_driftGap/m_StripResponse_driftVelocity))/stepSize; jj++) {
      
      	//cout << "start looking for local peak: " << endl;
	
	
	float thisStep = jj*stepSize;
	float nextStep = (jj+1)*stepSize;
	float oneAfterStep = (jj+2)*stepSize;
	
	//cout << "intFn->Eval(thisStep,0,0): " << intFn->Eval(thisStep,0,0) << endl;
	//cout << "intFn->Derivative(thisStep,0,0.001): " << intFn->Derivative(thisStep,0,0.001) << endl;

	if (localPeak == 0 ) {  //haven't found a local peak yet
	
	  //cout << "intFn->Eval(nextStep,0,0): " << intFn->Eval(nextStep,0,0) << endl;
	  //cout << "intFn->Derivative(nextStep,0,0.001): " << intFn->Derivative(nextStep,0,0.001) << endl;
	 
	  //check if the charge for the next points is less than the current step and the derivative of the first point is positive or 0 and the next point is negative:
	  if ( ( intFn->Eval(thisStep,0,0) < intFn->Eval(nextStep,0,0) ) && ( intFn->Eval(nextStep,0,0) > intFn->Eval(oneAfterStep,0,0) ) &&
	       (intFn->Derivative(thisStep,0,0.001) > 0 && intFn->Derivative(oneAfterStep,0,0.001) < 0 ) ) {
	    // && (intFn->Derivative(thisStep,0,0.001)>0 && intFn->Derivative(nextStep,0,0.001)<=0 )
	    // && (intFn->Eval(oneAfterStep,0,0) < intFn->Eval(nextStep,0,0))
	  

	    localPeak = 1;  
	    localPeakt = nextStep;
	    localPeakq = intFn->Eval(nextStep,0,0);
	    
	    log << MSG::DEBUG << "found a peak!    for strip number: " << numberofStrip.at(ii) << " at time: " << nextStep << " with charge: " << intFn->Eval(nextStep,0,0) << endreq;
	    
	    nStripElectronics.push_back(numberofStrip.at(ii));
	    tStripElectronicsAbThr.push_back(localPeakt);
	    qStripElectronics.push_back(localPeakq);
            	   
	  }
	}
      }
    }
  }

  //---------------------------------------------------------
  //   /// DUMMY:
  //   tMinFromFirstPeak = tMinFromIntegration;
    
  //   nStripElectronics.push_back(ii);
  //   tStripElectronicsAbThr.push_back(tMinFromFirstPeak);
  //   if (maxOfIntegration>=electronicsThreshold) qStripElectronics.push_back(maxOfIntegration);
  //   else qStripElectronics.push_back(-50000.);
  // }
  // tMinFromFirstPeak = 50000.;
  // for (size_t j = 0;j<tStripElectronicsAbThr.size();j++){
  //   if (tMinFromFirstPeak>tStripElectronicsAbThr.at(j)) tMinFromFirstPeak = tStripElectronicsAbThr.at(j);
  // }
  //delete spectrum;
}///end of bnl responce function
/*******************************************************************************/
ElectronicsResponse::~ElectronicsResponse()
{ 
  if (stripObject) delete stripObject;
  if (intFn) delete intFn;
  clearValues();
}
/*******************************************************************************/
