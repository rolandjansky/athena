/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/// PROJECTS
#include "MM_Digitization/MM_StripsResponseSimulation.h"
#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/MM_StripResponse.h"
#include "PathResolver/PathResolver.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandGeneral.h"
#include "CLHEP/Random/RandGamma.h"

#include<map>
#include<algorithm>

using namespace std;

/*

To Do List:

Check Lorentz Angle is in correct direction...

*/

namespace {
    static constexpr float electronsToFC = 1./6241.;
    static constexpr float polyaTheta = 1.765;
}

/*******************************************************************************/
MM_StripsResponseSimulation::MM_StripsResponseSimulation():

	// Variables that should be set externally (MM_DigitizationTool)
	m_qThreshold(0),                 // 0.001
	m_transverseDiffusionSigma(0),   // 0.360/10.
	m_longitudinalDiffusionSigma(0), // 0.190/10.
	m_crossTalk1(0),                 // 0.1
	m_crossTalk2(0),                 // 0.03
	m_driftGapWidth(0),              // 5.128
	m_driftVelocity(0),              // 0.047

	// Other variables
	m_avalancheGain(0),
	m_maxPrimaryIons(300),
	m_interactionDensityMean(0),  //   16.15 interactions per 5 mm traversed
	m_interactionDensitySigma(0),  //   Spread in this number.

	// Function Pointers
	m_lorentzAngleFunction(0),
	m_engine(0),
	m_writeOutputFile(false),
	m_writeEventDisplays(false),
	m_outputFile(0)
	{
	}
/*******************************************************************************/
void MM_StripsResponseSimulation::initHistos()
{
	if(m_writeOutputFile){
		m_outputFile = new TFile("MM_StripsResponse_Plots.root","RECREATE");
		if(m_outputFile) m_outputFile->cd();

		m_mapOfHistograms["nInteractions"] = new TH1F("nInteractions","nInteractions",200,0,200);
		m_mapOfHistograms["nElectrons"] = new TH1F("nElectrons","nElectrons",200,0,200);
		m_mapOfHistograms["lorentzAngle"] = new TH1F("lorentzAngle","lorentzAngle",100,0,3);
		m_mapOfHistograms["effectiveCharge"] = new TH1F("effectiveCharge","effectiveCharge",100,0,1e5);
		m_mapOfHistograms["effectiveNElectrons"] = new TH1F("effectiveNElectrons","effectiveNElectrons",200,0,200);
		m_mapOfHistograms["totalEffectiveCharge"] = new TH1F("totalEffectiveCharge","totalEffectiveCharge",400,0,200);
		m_mapOf2DHistograms["totalEffectiveChargeVsTheta"] = new TH2F("totalEffectiveChargeVsTheta","totalEffectiveChargeVsTheta",17,0,35,300,0,300);

		m_mapOf2DHistograms["pathLengthVsTheta"] = new TH2F("pathLengthVsTheta","pathLengthVsTheta",  100,0,10,  100,-3,3);
		m_mapOf2DHistograms["pathLengthVsAlpha"] = new TH2F("pathLengthVsAlpha","pathLengthVsAlpha",  100,0,10,  100,-3,3);

		m_mapOf2DHistograms["lorentzAngleVsTheta"] = new TH2F("lorentzAngleVsTheta","lorentzAngleVsTheta",  100,-3,3,  100,-3,3);
		m_mapOf2DHistograms["lorentzAngleVsBy"]    = new TH2F("lorentzAngleVsBy","lorentzAngleVsBy",  100,-3,3,  100,-2.,2.);

		m_mapOf2DHistograms["deltaLorentzAngleVsByForPosTheta"]    = new TH2F("deltaLorentzAngleVsByForPosTheta","deltaLorentzAngleVsByForPosTheta",  100,-3,3,  100,-2.,2.);
		m_mapOf2DHistograms["deltaLorentzAngleVsByForNegTheta"]    = new TH2F("deltaLorentzAngleVsByForNegTheta","deltaLorentzAngleVsByForNegTheta",  100,-3,3,  100,-2.,2.);

		m_mapOf2DHistograms["driftDistanceVsDriftTime"]            = new TH2F("driftDistanceVsDriftTime","driftDistanceVsDriftTime",  100,0,10,  100,0,200);
	}
}
/*******************************************************************************/
void MM_StripsResponseSimulation::writeHistos()
{

	if(m_outputFile){
		m_outputFile->cd();
		m_outputFile->Write();
	}

}
/*******************************************************************************/
void MM_StripsResponseSimulation::clearValues()
{
}

/*******************************************************************************/
void MM_StripsResponseSimulation::initialize(CLHEP::HepRandomEngine* rndmEngine)
{
  m_engine = rndmEngine;
	initHistos ();

  // initialization for the random number generator for the number of secondary electrons
  // values are from G. Iakovidis
  
  m_NelectronPropBins = 300;
  std::vector<double> NelectronProb = {65.6,15,6.4,3.5,2.25,1.55,1.05,0.81,0.61,0.49,0.39,0.3,0.25,0.2,0.16,0.12,0.095,0.075,0.063,0.054,0.049,0.045,0.044};
  NelectronProb.reserve(m_NelectronPropBins);
  for(int Nelectron = 23; Nelectron<m_NelectronPropBins; Nelectron++) NelectronProb.push_back(21.6/((Nelectron)*(Nelectron)));

  m_randNelectrons=std::make_unique<CLHEP::RandGeneral>(m_engine,&NelectronProb[0],m_NelectronPropBins,1); // 1 means non-continious random numbers


	ATH_MSG_DEBUG("MM_StripsResponseSimulation::initializationFrom set values");

}

/*******************************************************************************/
MM_StripToolOutput MM_StripsResponseSimulation::GetResponseFrom(const MM_DigitToolInput & digiInput, double gainFraction, double stripPitch)
{

    ATH_MSG_DEBUG("Starting to get response from strips");

	m_IonizationClusters.clear();
	m_finalNumberofStrip.clear();
	m_finalqStrip.clear();
	m_finaltStrip.clear();

	whichStrips(digiInput.positionWithinStrip(),
		digiInput.stripIDLocal(),
		digiInput.incomingAngleXZ(), //degrees
		digiInput.incomingAngleYZ(), //degrees
		digiInput.stripMinID(),
		digiInput.stripMaxID(),
		digiInput,
		gainFraction,
		stripPitch
		);

	ATH_MSG_DEBUG("Creating MmDigitToolOutput object");

	MM_StripToolOutput tmpStripToolOutput(m_finalNumberofStrip, m_finalqStrip, m_finaltStrip);

	return tmpStripToolOutput;

}

//__________________________________________________________________________________________________
//==================================================================================================
// calculate the strips that got fired, charge and time. Assume any angle thetaDegrees....
//==================================================================================================
void MM_StripsResponseSimulation::whichStrips( const float & hitx,
											const int & stripID,
											const float & incidentAngleXZ,
											const float & incidentAngleYZ,
											const int & stripMinID,
											const int & stripMaxID,
											const MM_DigitToolInput & digiInput,
											double gainFraction,
											double stripPitch)
{

	ATH_MSG_DEBUG("Starting to calculate strips that got fired");

	float eventTime = digiInput.eventTime();
	float theta = incidentAngleXZ * M_PI/180.0; // Important for path length and strip distribution
	float alpha = incidentAngleYZ * M_PI/180.0; // Important for path length


	int nPrimaryIons = 0;

	Amg::Vector3D b = digiInput.magneticField() * 1000.;

	// Still need to understand which sign is which... But I think this is correct...

	float lorentzAngle = ( b.y()>0. ?  1.  :  -1. ) * m_lorentzAngleFunction->Eval( fabs( b.y() ) ) * TMath::DegToRad() ; // in radians
	if (m_writeOutputFile) {
		m_mapOf2DHistograms["lorentzAngleVsTheta"]->Fill(lorentzAngle,theta);
		m_mapOf2DHistograms["lorentzAngleVsBy"]->Fill(lorentzAngle,b.y());
	}

	ATH_MSG_DEBUG("LorentzAngle vs theta: " <<lorentzAngle <<" " <<theta);

	float pathLengthTraveled = getPathLengthTraveled(); 

	float pathLength  = m_driftGapWidth/TMath::Cos(theta); // Increasing path length based on XZ angle
	pathLength       /= TMath::Cos(alpha);                 // Further increasing path length for YZ angle
    
	if (m_writeOutputFile) {
		m_mapOf2DHistograms["pathLengthVsTheta"]->Fill(pathLength, theta);
		m_mapOf2DHistograms["pathLengthVsAlpha"]->Fill(pathLength, alpha);
	
		if (theta>0) m_mapOf2DHistograms["deltaLorentzAngleVsByForPosTheta"]->Fill(  lorentzAngle-theta , b.y() );
		else         m_mapOf2DHistograms["deltaLorentzAngleVsByForNegTheta"]->Fill(  lorentzAngle-theta , b.y() );
	}
	while (pathLengthTraveled < pathLength){

		// N.B. Needs correction from alpha angle still...
		std::unique_ptr<MM_IonizationCluster> IonizationCluster = std::make_unique<MM_IonizationCluster>(hitx, pathLengthTraveled*sin(theta), pathLengthTraveled*cos(theta));
		
    int nElectrons = m_randNelectrons->shoot(m_engine) * m_NelectronPropBins + 1; // +1 since first entry in electron probability vector corresponds to 1 electron and not to 0 electrons 
    IonizationCluster -> createElectrons(nElectrons);

		TVector2 initialPosition = IonizationCluster->getIonizationStart();

		ATH_MSG_DEBUG("New interaction starting at x,y, pathLengthTraveled: "
			<< initialPosition.X()
			<< " "
			<< initialPosition.Y()
			<< " "
			<< pathLengthTraveled
			);

        for (auto& Electron : IonizationCluster->getElectrons()) {
            Electron->setOffsetPosition(getTransverseDiffusion(initialPosition.Y()) , getLongitudinalDiffusion(initialPosition.Y()) );
        }

		IonizationCluster->propagateElectrons( lorentzAngle , m_driftVelocity );


		int tmpEffectiveNElectrons = 0;

		for (auto& Electron : IonizationCluster->getElectrons()){

			float effectiveCharge = getEffectiveCharge() * gainFraction;

			Electron->setCharge( effectiveCharge );

			ATH_MSG_DEBUG("Electron's effective charge is  "<< effectiveCharge);

			if (m_writeOutputFile) {
				m_mapOfHistograms["effectiveCharge"]->Fill( effectiveCharge );
				m_mapOf2DHistograms["driftDistanceVsDriftTime"]->Fill(Electron->getOffsetPosition().Mod(), Electron->getTime() );
			}
			// Add eventTime in Electron time
			Electron->setTime(Electron->getTime() + eventTime);

			tmpEffectiveNElectrons+= effectiveCharge;
		}

		if(m_writeOutputFile)	m_mapOfHistograms["effectiveNElectrons"]->Fill( tmpEffectiveNElectrons / m_avalancheGain);

		//---
		m_IonizationClusters.push_back(std::move(IonizationCluster));

		pathLengthTraveled +=  getPathLengthTraveled();

		ATH_MSG_DEBUG("Path length traveled: " << pathLengthTraveled);

		nPrimaryIons++;
		if (nPrimaryIons >= m_maxPrimaryIons) break; //don't create more than "MaxPrimaryIons" along a track....

	} //end of clusters loop

	float timeresolution = 0.01; //ns

	MM_StripResponse stripResponseObject(m_IonizationClusters, timeresolution, stripPitch, stripID, stripMinID, stripMaxID);
	stripResponseObject.timeOrderElectrons();
	stripResponseObject.calculateTimeSeries(incidentAngleXZ, digiInput.gasgap());
	stripResponseObject.simulateCrossTalk( m_crossTalk1,  m_crossTalk2);
	stripResponseObject.calculateSummaries( m_qThreshold );

	//Connect the output with the rest of the existing code
	//
	m_finalNumberofStrip     = stripResponseObject.getStripVec();
	m_finalqStrip            = stripResponseObject.getTotalChargeVec();
	m_finaltStrip            = stripResponseObject.getTimeThresholdVec();
	m_tStripElectronicsAbThr = stripResponseObject.getTimeMaxChargeVec();
	m_qStripElectronics      = stripResponseObject.getMaxChargeVec();



    // Output diagnostic hists and graphs
    //
    if (m_writeOutputFile) {
        m_mapOfHistograms["nInteractions"]->Fill(nPrimaryIons);
        m_mapOfHistograms["nElectrons"]->Fill(stripResponseObject.getNElectrons());
        m_mapOfHistograms["totalEffectiveCharge"]->Fill(stripResponseObject.getTotalCharge() * electronsToFC);
        m_mapOf2DHistograms["totalEffectiveChargeVsTheta"]->Fill(std::abs(incidentAngleXZ),
                                                       stripResponseObject.getTotalCharge() * electronsToFC);
        ATH_MSG_DEBUG("incidentAngleXZ" << incidentAngleXZ << "charge [fC]"
                      << stripResponseObject.getTotalCharge() / 6241.);
        m_mapOfHistograms["lorentzAngle"]->Fill(lorentzAngle);
    }

	if(m_writeEventDisplays){
		if(m_outputFile) m_outputFile->cd();
		TGraph grIonizationXZ( m_IonizationClusters.size() );
		for (int iIonization=0; iIonization <  (int) m_IonizationClusters.size(); iIonization++) {
			TVector2 ionizationPosition( m_IonizationClusters.at(iIonization)->getIonizationStart() );
			grIonizationXZ.SetPoint( iIonization, ionizationPosition.X(), ionizationPosition.Y() );
		}
		grIonizationXZ.Write("ionizationXZ");

		TGraph grElectronsXZ( stripResponseObject.getNElectrons() );
		int iElectron = 0;
		for (const auto & electron: stripResponseObject.getElectrons()){
			grElectronsXZ.SetPoint( iElectron, electron->getX(), electron->getY() );
			iElectron++;
		}
		grElectronsXZ.Write("electronsXZ");
	}

} // end of whichStrips()



float MM_StripsResponseSimulation::generateTransverseDiffusion(float posY) {
    // this is a helper function used in getTransverseDiffusion, generating double gaussian distributions

    // avoid division by zero in calculation of scale if ypos is 0
    // also protect against negative values of ypos which should never happen
    if (posY <= 0) posY = 0.001;

    // need to scale weigths since initial distributions were not normalized
    double scale = 0.001/(posY*m_transverseDiffusionSigma);

    double uni = CLHEP::RandFlat::shoot(m_engine,0, 1.0+scale);
    if (uni < scale) return CLHEP::RandGaussZiggurat::shoot(m_engine,0.0, 1.0);
    return CLHEP::RandGaussZiggurat::shoot(m_engine,0., m_transverseDiffusionSigma * posY);
}


float MM_StripsResponseSimulation::getTransverseDiffusion(float posY) {
    // the random numbers are generate from the following function:
    // "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))"
    // in the range from -1 to 1
    // Since sampling from a TF1 where its parameters are changed for every random number is slow,
    // the following approach is used to generate random numbers from the mixture of two Gaussian:
    // https://stats.stackexchange.com/questions/226834/sampling-from-a-mixture-of-two-gamma-distributions/226837#226837
    // this approach seems to be around 20000 times faster

    // if one of the diffusions is off, the tail is not present
    if (m_longitudinalDiffusionSigma == 0 || m_transverseDiffusionSigma == 0) {
        float tmp =  CLHEP::RandGaussZiggurat::shoot(m_engine,0.0, posY*m_transverseDiffusionSigma);
        // limit random number to be -1 < x < 1
        while (std::abs(tmp) > 1.) tmp =  CLHEP::RandGaussZiggurat::shoot(m_engine,0.0, posY*m_transverseDiffusionSigma);
        return tmp;
    }
    float tmp = generateTransverseDiffusion(posY);
    while (std::abs(tmp) > 1.) {tmp = generateTransverseDiffusion(posY);}

    return tmp;
  }

float MM_StripsResponseSimulation::getLongitudinalDiffusion(float posY) {
  float tmp = CLHEP::RandGaussZiggurat::shoot(m_engine,0.0, posY*m_longitudinalDiffusionSigma);
  // We only want random numbers between -5 and 5
  while (std::abs(tmp) > 5) { tmp = CLHEP::RandGaussZiggurat::shoot(m_engine,0.0, posY*m_longitudinalDiffusionSigma); }
  return tmp;
}

float  MM_StripsResponseSimulation::getEffectiveCharge() {
        // charge fluctuatation is described by Polya function
    	  //m_polyaFunction = new TF1("m_polyaFunction","(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,50000);
       // m_polyaFunction->SetParameter(0, 1.765); // polya theta
       // m_polyaFunction->SetParameter(1, m_avalancheGain ); // mean gain
       
       // To calculate the gain from polya distibution we replace "alpha = 1+theta and beta = 1+theta/mean" in gamma PDF. With this gamma PDF gives us same sampling values as we get from polya PDF.
       // Idea taken from MR 40547

        return CLHEP::RandGamma::shoot(m_engine, 1. + polyaTheta, (1. + polyaTheta)/m_avalancheGain);    

}

float MM_StripsResponseSimulation::getPathLengthTraveled() {

	  // Probability of having an interaction (per unit length traversed) is sampled from a gaussian provided by G. Iakovidis
    float rndGaus = CLHEP::RandGaussZiggurat::shoot(m_engine,m_interactionDensityMean, m_interactionDensitySigma);


    // gaussian random number should be in the range from 0 to 10
    while (rndGaus < 0. || rndGaus > 10.) {rndGaus = CLHEP::RandGaussZiggurat::shoot(m_engine,m_interactionDensityMean, m_interactionDensitySigma);}

    return  ( 1. / rndGaus) * -1. * log( CLHEP::RandFlat::shoot(m_engine) );
}




/*******************************************************************************/
MM_StripsResponseSimulation::~MM_StripsResponseSimulation()
{
	if(m_outputFile){
		writeHistos();
		delete m_outputFile;
	}

}
/*******************************************************************************/
