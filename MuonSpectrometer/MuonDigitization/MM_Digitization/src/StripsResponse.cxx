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
	m_driftGapWidth(5.128),
	m_driftVelocity(0.047),
	m_maxPrimaryIons(300),
	m_interactionDensityMean(10./26.),  // 5 mm per 16.15 interactions
	// m_interactionDensityMean(5./16.15),  // 5 mm per 16.15 interactions
	m_interactionDensitySigma(0.001),   //   Spread in this number.
	// m_interactionDensitySigma(5./4.04)   //   Spread in this number.
	m_polyaFunction(0),
	m_lorentzAngleFunction(0),
	m_longitudinalDiffusionFunction(0),
	m_transverseDiffusionFunction(0),
	m_interactionDensityFunction(0),
	m_random(0)
	{
	// clearValues();
	// initializate();
	}
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
	// m_polyaFunction = new TF1("m_polyaFunction","[2]*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,50000);
	// m_polyaFunction->SetParameters(1.765, m_avalancheGain, 104.9);


	m_interactionDensityFunction = new TF1("interactionDensityFunction","gaus", 0., 10.);
	m_interactionDensityFunction->SetParameter(0, 1.0); // normalization
    m_interactionDensityFunction->SetParameter(1, 16.15 / 5.); // mean
    m_interactionDensityFunction->SetParameter(2, 4.04 / 5.); // width
	// m_interactionDensityFunction->SetParameters( 1., m_interactionDensityMean, m_interactionDensitySigma );

	m_polyaFunction = new TF1("m_polyaFunction","[2]*(TMath::Power([0]+1,[0]+1)/TMath::Gamma([0]+1))*TMath::Power(x/[1],[0])*TMath::Exp(-([0]+1)*x/[1])",0,50000);
    m_polyaFunction->SetParameter(0, 1.765); // polya theta
    m_polyaFunction->SetParameter(1, 1.16e4); // mean gain
    m_polyaFunction->SetParameter(2, 104.9); // constant

	m_lorentzAngleFunction = new TF1("lorentzAngleFunction","[0] + [1]*x + [2]*x*x + [3]*x*x*x + [4]*x*x*x*x",0,2);
	m_lorentzAngleFunction->SetParameters(0,58.87, -2.983, -10.62, 2.818);

	// conv_gaus  = new TF1("conv_gaus","1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);
	m_longitudinalDiffusionFunction = new TF1("longdiff","gaus", 0., 5.);
	m_transverseDiffusionFunction = new TF1("transdiff", "1.*TMath::Exp(-TMath::Power(x,2.)/(2.*[0]*[0])) + 0.001*TMath::Exp(-TMath::Power(x,2)/(2.*[1]*[1]))", -1., 1.);

	m_random = new TRandom3(0);


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
	if (m_interactionDensityFunction     != 0 ) delete m_interactionDensityFunction     ;
	if (m_random                         != 0 ) delete m_random                         ;

}

/*******************************************************************************/
void StripsResponse::initialize()
{

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

	int nPrimaryIons = 0;

	m_random->SetSeed((int)fabs(hitx*10000));


	Amg::Vector3D b = digiInput.magneticField() * 1000.;

	// Still need to understand which sign is which... But I think this is correct...

	float lorentzAngle = ( b.y()>0. ?  1.  :  -1. ) * m_lorentzAngleFunction->Eval( fabs( b.y() ) ) * TMath::DegToRad() ; // in radians

	msglog << MSG::DEBUG << "StripsResponse::lorentzAngle vs theta: " << lorentzAngle << " " << theta << endmsg;



	msglog << MSG::DEBUG << "StripResponse::function pointer points to " << m_interactionDensityFunction << endmsg;

	float pathLengthTraveled = ( 1. / m_interactionDensityFunction->GetRandom() ) * -1. * log( m_random->Uniform() );

	float pathLength         = m_driftGapWidth/TMath::Cos(theta);

	while (pathLengthTraveled < pathLength){

		MM_IonizationCluster IonizationCluster(hitx, pathLengthTraveled*sin(theta), pathLengthTraveled*cos(theta));
		IonizationCluster.createElectrons(m_random);
		//    IonizationCluster.diffuseElectrons(longitudinalDiffusSigma, diffusSigma, m_random);
		//---
		TVector2 initialPosition = IonizationCluster.getIonizationStart();

		msglog << MSG::DEBUG << "StripResponse:: New interaction starting at x,y, pathLengthTraveled: " << initialPosition.X() << " " << initialPosition.Y() << " " << pathLengthTraveled<< endmsg;


		for (auto& Electron : IonizationCluster.getElectrons()){

			m_longitudinalDiffusionFunction->SetParameters(1.0, initialPosition.Y(), initialPosition.Y()*m_longitudinalDiffusionSigma);

			if ( m_longitudinalDiffusionSigma == 0 || m_transverseDiffusionSigma == 0) {

				m_transverseDiffusionFunction->SetParameters( initialPosition.Y()*m_transverseDiffusionSigma , 0.0);

			} else {

				m_transverseDiffusionFunction->SetParameters( initialPosition.Y()*m_transverseDiffusionSigma , 1.0);

			}

			Electron->setOffsetPosition(m_transverseDiffusionFunction->GetRandom(), m_longitudinalDiffusionFunction->GetRandom());

		}

		// NEED TO REIMPLEMENT USING THE LORENTZ ANGLE UP HERE....
		IonizationCluster.propagateElectrons( lorentzAngle , m_driftVelocity);

		//---
		for (auto& Electron : IonizationCluster.getElectrons()){

			float effectiveCharge = m_polyaFunction->GetRandom();

			Electron->setCharge( effectiveCharge );
			msglog << MSG::DEBUG << "StripsResponse::Electron's effective charge is  " << effectiveCharge << endmsg;

			// Add eventTime in Electron time
			Electron->setTime(Electron->getTime() + eventTime);
		}
		//---
		IonizationClusters.push_back(IonizationCluster);

		pathLengthTraveled +=  (1. / m_interactionDensityFunction->GetRandom() ) * -1. * log( m_random->Uniform() );

		msglog << MSG::DEBUG << "StripResponse:: path length traveled: " << pathLengthTraveled << endmsg;

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


	// clearValues();

} // end of whichStrips()

/*******************************************************************************/
StripsResponse::~StripsResponse()
{
	clearValues();
}
/*******************************************************************************/
