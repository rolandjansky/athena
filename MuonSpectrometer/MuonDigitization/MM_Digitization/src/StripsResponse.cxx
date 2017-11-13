/*
Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
*  StripsResponse.cxx
*  MC for micromegas athena integration
*
**/

/// PROJECTS
#include "MM_Digitization/StripsResponse.h"
#include "GaudiKernel/MsgStream.h"
#include "MM_Digitization/MM_IonizationCluster.h"
#include "MM_Digitization/MM_StripResponse.h"
#include "MM_Digitization/GarfieldGas.h"
#include "PathResolver/PathResolver.h"

#include<map>
#include<algorithm>

using namespace std;


/*

To Do List:

Make variable names reasonable and follow ATLAS conventions
Implement Gaussian Smearing of Mean Free Path for Ineraction Rate
Check Lorentz Angle is in correct direction...
Implement Proper Polya for Gain
Check double gaussian for transverse diffusion. Is it even implemented?

*/




/*******************************************************************************/
StripsResponse::StripsResponse():
m_qThreshold(0.001),
m_transverseDiffusionSigma(0.360/10.),
m_longitudinalDiffusionSigma(0.190/10.),
m_pitch(0.500),
m_avalancheGain(1.16e4),
m_polyaFunction(0),
// conv_gaus(0),
m_driftGapWidth(5.128),
m_driftVelocity(0.047),
// nstrip(0),
// temp_polya(0),
// polya_theta(0),
// numberOfElectrons(0),
// dimClusters(0),
m_maxPrimaryIons(300),
// pt(0),
// xx(0),
// xxDiffussion(0),
// yy(0),
// yyDiffussion(0),
// primaryion(0),
m_interactionProbabilityMean(5./16.15),  // 5 mm per 16.15 interactions
m_interactionProbabilitySigma(5./4.04)   //   Spread in this number.
// totalelectrons(0),
// gRandom_loc(0),
// randomNum(0),
// m_electricFieldZ(600.)
{
	clearValues();
	initializationFrom();
}
/*******************************************************************************/
// void StripsResponse::loadGasFile(std::string fileName){
// 	Athena::MsgStreamMember log("StripsResponse::loadGasFile");
// 	std::string fileWithPath = PathResolver::find_file (fileName, "CALIBPATH");
// 	if (fileWithPath == "") {
// 		log << MSG::FATAL << "StripResponse::loadGasFile(): Could not find file " << fileName << endmsg;
// 		exit(1);
// 	}
// }
/*******************************************************************************/
void StripsResponse::initHistos()
{
// For validation purposes only...
}
/*******************************************************************************/
void StripsResponse::writeHistos()
{}
/*******************************************************************************/
void StripsResponse::initFunctions()
{
	m_polyaFunction = new TF1("polyaFit","[2]*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,50000);
	m_polyaFunction->SetParameters(1.765, m_avalancheGain, 104.9);

	m_lorentzAngleFunction = new TF1("lorentzAngleFunction","[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x",0,2);
	m_lorentzAngleFunction->SetParameters(0,58.87, -2.983, -10.62, 2.818);

	// conv_gaus  = new TF1("conv_gaus","1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);
	m_longitudinalDiffusionFunction = new TF1("longdiff","gaus", 0., 5.);
	m_transverseDiffusionFunction = new TF1("transdiff", "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);

	m_interactionProbabilityFunction = new TF1("interactionProbabilityFunction","gaus", 0, 1000);
	m_interactionProbabilityFunction->SetParameters( m_interactionProbabilityMean, m_interactionProbabilitySigma );

	Athena::MsgStreamMember log("StripsResponse::initFunctions");

	log << MSG::DEBUG << "StripsResponse::initFunctions DONE" << endmsg;

}
/*******************************************************************************/
void StripsResponse::clearValues()
{

	if (m_polyaFunction                  != 0 ) delete m_polyaFunction                  ;
	if (m_lorentzAngleFunction           != 0 ) delete m_lorentzAngleFunction           ;
	if (m_longitudinalDiffusionFunction  != 0 ) delete m_longitudinalDiffusionFunction  ;
	if (m_transverseDiffusionFunction    != 0 ) delete m_transverseDiffusionFunction    ;
	if (m_interactionProbabilityFunction != 0 ) delete m_interactionProbabilityFunction ;

}

/*******************************************************************************/
void StripsResponse::initializationFrom()
{
	m_crossTalk1=0.0;
	m_crossTalk2=0.0;

	initHistos ();
	initFunctions();

	Athena::MsgStreamMember log("StripsResponse::initializationFrom");
	log << MSG::DEBUG << "StripsResponse::initializationFrom set values" << endmsg;
}

/*******************************************************************************/
MmStripToolOutput StripsResponse::GetResponseFrom(const MmDigitToolInput & digiInput)
{
	Athena::MsgStreamMember log("StripsResponse::GetResponseFrom");
	log << MSG::DEBUG << "\t \t StripsResponse::GetResponseFrom start " << endmsg;

	IonizationClusters.clear();
	finalNumberofStrip.clear();
	finalqStrip.clear();
	finaltStrip.clear();

	log << MSG::DEBUG << "\t \t StripsResponse::calculateResponseFrom call whichStrips " << endmsg;

	whichStrips(digiInput.positionWithinStrip(), digiInput.stripIDLocal(), digiInput.incomingAngle(), digiInput.stripMaxID(), digiInput);

	log << MSG::DEBUG << "\t \t StripsResponse::GetResponseFrom MmDigitToolOutput create " << endmsg;

	MmStripToolOutput tmpStripToolOutput(finalNumberofStrip, finalqStrip, finaltStrip); // T.Saito

	return tmpStripToolOutput;
}

//__________________________________________________________________________________________________
//==================================================================================================
// calculate the strips that got fired, charge and time. Assume any angle thetaD....
//==================================================================================================
void StripsResponse::whichStrips(const float & hitx, const int & stripID, const float & thetaD, const int & stripMaxID, const MmDigitToolInput & digiInput)
{

	Athena::MsgStreamMember msglog("StripsResponse::whichStrips");
	msglog << MSG::DEBUG << "\t \t StripsResponse::whichStrips start " << endmsg;

	float eventTime = digiInput.eventTime();
	float theta = thetaD*TMath::Pi()/180.0;

	// dimClusters=7000; //dimClusters=total number of collisions

	// m_interactionProbabilityMean = 10./26.;  //  24 e/8 mm or 23 e/10mm (as it has been used here) according to Sauli's paper, for Argon
	// float tmpPathLength = m_interactionProbabilityFunction->GetRandom();

	// this lmean should be drawn from a gaussian each time....

	int nPrimaryIons = 0;

	// Now generate the primary ionization colisions, Poisson distributed
	TRandom3 randomNum(0);

	randomNum.SetSeed((int)fabs(hitx*10000));

	// float tmpRandomNumber = randomNum.Uniform();

	float pathLengthTraveled = m_interactionProbabilityFunction->GetRandom() * -1. * log( randomNum.Uniform() );
	float pathLength         = m_driftGapWidth/TMath::Cos(theta);


	while (pathLengthTraveled < pathLength){

		MM_IonizationCluster IonizationCluster(hitx, pathLengthTraveled*sin(theta), pathLengthTraveled*cos(theta));
		IonizationCluster.createElectrons(&randomNum);
		//    IonizationCluster.diffuseElectrons(longitudinalDiffusSigma, diffusSigma, randomNum);
		//---
		TVector2 initialPosition = IonizationCluster.getIonizationStart();
		for (auto& Electron : IonizationCluster.getElectrons()){

			m_longitudinalDiffusionFunction->SetParameters(1.0, initialPosition.Y(), initialPosition.Y()*m_longitudinalDiffusionSigma);

			if ( m_longitudinalDiffusionSigma == 0 || m_transverseDiffusionSigma == 0) {

				m_transverseDiffusionFunction->SetParameters( initialPosition.Y()*m_transverseDiffusionSigma , 0.0);

			} else {

				m_transverseDiffusionFunction->SetParameters( initialPosition.Y()*m_transverseDiffusionSigma , 1.0);

			}
			Electron->setOffsetPosition(m_transverseDiffusionFunction->GetRandom(), m_longitudinalDiffusionFunction->GetRandom());
		}
		//---

		Amg::Vector3D b = digiInput.magneticField() * 1000.;

		msglog << MSG::WARNING << "StripsResponse::See which is the largest?? x,y,z " << b.x() << " " << b.y() << " " << b.z() << endmsg;
		float lorentzAngle = m_lorentzAngleFunction->Eval( b.x() );

		// NEED TO REIMPLEMENT USING THE LORENTZ ANGLE UP HERE....
		// IonizationCluster.propagateElectrons(vx, -vz, m_driftVelocity);

		//---
		for (auto& Electron : IonizationCluster.getElectrons()){
			Electron->setCharge( m_polyaFunction->GetRandom() );
			// Add eventTime in Electron time
			Electron->setTime(Electron->getTime() + eventTime);
		}
		//---
		IonizationClusters.push_back(IonizationCluster);

		pathLengthTraveled +=  m_interactionProbabilityFunction->GetRandom() * -1. * log( randomNum.Uniform() );

		nPrimaryIons++;
		if (nPrimaryIons >= m_maxPrimaryIons) break; //don't create more than "MaxPrimaryIons" along a track....

	} //end of clusters loop

	float timeresolution = 0.01; //ns

	MM_StripResponse StripResponse(IonizationClusters, timeresolution, m_pitch, stripID, stripMaxID);
	StripResponse.timeOrderElectrons();
	StripResponse.calculateTimeSeries(thetaD, digiInput.gasgap());
	StripResponse.simulateCrossTalk( m_crossTalk1,  m_crossTalk2);
	StripResponse.calculateSummaries( m_qThreshold );
	//Connect the output with the rest of the existing code
	finalNumberofStrip = StripResponse.getStripVec();
	finalqStrip = StripResponse.getTotalChargeVec();
	finaltStrip = StripResponse.getTimeThresholdVec();
	tStripElectronicsAbThr = StripResponse.getTimeMaxChargeVec();
	qStripElectronics = StripResponse.getMaxChargeVec();

} // end of whichStrips()

/*******************************************************************************/
StripsResponse::~StripsResponse()
{
	clearValues();
}
/*******************************************************************************/
