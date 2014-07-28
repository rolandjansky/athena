/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ////////////////////////////////////
//                                  //
//  Class: TRT_SimDriftTimeTool     //
//                                  //
//  Author: Thomas Kittelmann       //
//                                  //
//  First Version: January 2006     //
//                                  //
 ////////////////////////////////////

#include "TRT_SimDriftTimeTool.h"

#include "TRT_BarrelDriftTimeData.h"
#include "TRT_ScaledDriftTimeData.h"

#include "GaudiKernel/MsgStream.h"
#include "CLHEP/Units/SystemOfUnits.h"

#include "AthenaKernel/IAtRndmGenSvc.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "GaudiKernel/ServiceHandle.h"

#include "InDetReadoutGeometry/TRT_DetectorManager.h"

TRT_SimDriftTimeTool::TRT_SimDriftTimeTool( const std::string& type,
					    const std::string& name,
					    const IInterface* parent )
  :  AthAlgTool( type, name, parent ),
     m_minDistance( 0*CLHEP::mm ), m_maxDistance( 2.0001*CLHEP::mm ), m_maxField ( -999 ), m_maxFieldSquaredLimit(999), m_invMaxFieldSquared(0.0),
     m_nTabulatedDistances(4000),//4000 means resolution of 0.5micron and 96KB of memory.
     m_invDeltaTabulatedDistances( (m_nTabulatedDistances-1)/(m_maxDistance-m_minDistance) ),
     m_numberGeneratedGaussians(10000),//80KB of memory
     m_runningGaussianIndex(0),
     m_pAtRndmGenSvc ("AtDSFMTGenSvc", name)
{
  // declare my special interface
  declareInterface<ITRT_SimDriftTimeTool>(this);
  declareProperty("RndServ", m_pAtRndmGenSvc, "Random Number Service used in TRT_SimDriftTimeTool" );
}

//__________________________________________________________________________
StatusCode TRT_SimDriftTimeTool::initialize() {

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Initializing TRT_SimDriftTimeTool."<<endreq;

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //             Get dig. version through            //
  //             the TRT Detector Manager            //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  // Get the TRT Detector Manager
  const InDetDD::TRT_DetectorManager * detmgr;
  if (StatusCode::SUCCESS != detStore()->retrieve(detmgr,"TRT")) {
    if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Can't get TRT_DetectorManager." <<  endreq;
    return StatusCode::FAILURE;
  } else {
    if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Retrieved TRT_DetectorManager with version "
					    << detmgr->getVersion().majorNum() << endreq;
  }

  m_digversion = detmgr->digitizationVersion();

  if (m_digversion<11) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "digversion < 11 (" << m_digversion << ") is no longer supported. The job will die now :(" <<endreq;
      throw;
  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //              Select DriftTime Data              //
  //            ( based on dig. version )            //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  std::vector<ITRT_DriftTimeData*> pDTData; /// modified by Sasha for Argon

  if (msgLvl(MSG::INFO)) msg(MSG::INFO)  << "Electron Drift Times are based upon specific gas-simulations"<<endreq;

  pDTData.push_back(new TRT_BarrelDriftTimeData(m_digversion,false)); // Xenon straws
  pDTData.push_back(new TRT_BarrelDriftTimeData(m_digversion,true)); //  Argon straws

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //          Make lookup tables of the data         //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  m_maxField = pDTData[0]->MaxTabulatedField();
  m_maxFieldSquaredLimit = 1.1*m_maxField*1.1*m_maxField;
  m_invMaxFieldSquared = 1.0/(m_maxField*m_maxField);

  for (unsigned int i=0; i<pDTData.size(); i++){

    // Tabulate mean drifttimes no and max field, and
    // relative spread in drifttimes:
    // Fill different tables for xenon and argon-based gas mixtures

    std::vector<double> _m_table_of_dist2meanDT_at_noField;
    std::vector<double> _m_table_of_dist2meanDT_at_maxField;
    std::vector<double> _m_table_of_RMSdiffusionToDT;

    _m_table_of_dist2meanDT_at_noField.resize(m_nTabulatedDistances);
    _m_table_of_dist2meanDT_at_maxField.resize(m_nTabulatedDistances);
    _m_table_of_RMSdiffusionToDT.resize(m_nTabulatedDistances);

    for (unsigned int distIndex = 0; distIndex < m_nTabulatedDistances; distIndex++) {

      double distance = ((m_maxDistance-m_minDistance)*distIndex)/(m_nTabulatedDistances-1.0)+m_minDistance;
      if (distIndex == m_nTabulatedDistances-1) distance = m_maxDistance;//to avoid a numerical mistake.

      _m_table_of_dist2meanDT_at_noField[distIndex] = pDTData[i]->DriftTimeAtNoField(distance);
      _m_table_of_dist2meanDT_at_maxField[distIndex] = pDTData[i]->DriftTimeAtMaxField(distance);
      _m_table_of_RMSdiffusionToDT[distIndex] = pDTData[i]->RMSofSpreadRelativeToDriftTime(distance);
      // if ((distIndex - int(distIndex/100.)*100) == 1) msg(MSG::INFO)  << "Sasha: distIndex = "<< distIndex << " pDTData["<< i <<"]->RMSofSpreadRelativeToDriftTime("<<distance<<") = " << pDTData[i]->RMSofSpreadRelativeToDriftTime(distance) << endreq;
    }

    m_table_of_dist2meanDT_at_noField.push_back(_m_table_of_dist2meanDT_at_noField);
    m_table_of_dist2meanDT_at_maxField.push_back(_m_table_of_dist2meanDT_at_maxField);
    m_table_of_RMSdiffusionToDT.push_back(_m_table_of_RMSdiffusionToDT);

  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////
  //     Tabulate Gaussians for diffusion spread     //
  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  // Get the Rndm number service
  if (!m_pAtRndmGenSvc.retrieve().isSuccess()) {
    if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Could not initialize Random Number Service." << endreq;
    return StatusCode::FAILURE;
  };

  //And our own engine with own seeds:
  CLHEP::HepRandomEngine * pHRengine = m_pAtRndmGenSvc->GetEngine("TRT_SimDriftTime");

  // To ensure that all drifttimes are positive we ensure that the
  // numbers are greater than -sup(|tabulatedRMSdiffusionToDT(dist)|)

  for (unsigned int i=0; i<pDTData.size(); i++){

    std::vector<double> _m_generatedGaussians;

    double maxRMSdiff = 0;
    for (double d=0*CLHEP::mm; d<=m_maxDistance; d+=0.1*CLHEP::mm) {
      double abstabval = pDTData[i]->RMSofSpreadRelativeToDriftTime(d);
      if (abstabval<0) abstabval = -abstabval;
      maxRMSdiff = std::max(abstabval,maxRMSdiff );
    }
    double lowercut = (-1/maxRMSdiff)*0.99;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cutting gaussians at "<<lowercut<<" sigma"<<std::endl;
    if (lowercut>-4.0)
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Gauss cut is suspiciously high!"<<std::endl;

    _m_generatedGaussians.resize(m_numberGeneratedGaussians);
    for (unsigned int j = 0 ; j < m_numberGeneratedGaussians; j++)
      do {
        _m_generatedGaussians[j] = CLHEP::RandGaussZiggurat::shoot(pHRengine, 0.0, 1.0);
      } while ( _m_generatedGaussians[j] < lowercut );

    m_generatedGaussians.push_back(_m_generatedGaussians);

    delete pDTData[i];

  }

  /////////////////////////////////////////////////////
  /////////////////////////////////////////////////////

  if (msgLvl(MSG::INFO)) msg(MSG::INFO) << "Initialization completed."<<endreq;
  return StatusCode::SUCCESS;

}

//__________________________________________________________________________
double TRT_SimDriftTimeTool::getAverageDriftTime( const double& dist,
						  const double& effectivefield_squared,
                                                  bool isArgonStraw) const { // last argument added by Sasha for Argon

  // Don't worry; physical checks on "dist" and "effectivefield_squared"
  //  are already performed in TRTProcessingOfStraw.cxx

  // The default isArgonStraw argument in this function gives backward compatibility
  // for older versions of TRT_Digitization where this argument is not given.

  // define straw type (iType):
  // 0 - Xenon
  // 1 - Argon

  int iType = 0; // default is Xenon
  if (m_generatedGaussians.size()==1){ // not ArXeMix regime - depend on m_digversion variable
    iType = 0;
  }
  else{
    if ((m_generatedGaussians.size()==2)&&(isArgonStraw)){
      iType = 1;
    }
  }

  if (effectivefield_squared>m_maxFieldSquaredLimit) { //a little extrapolation is ok
    if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Extrapolation from tabulated field values greater than 10% (at "
						<<std::sqrt(effectivefield_squared)/CLHEP::tesla<<" Tesla)"<<endreq;
  }

  const unsigned int distIndex(static_cast<int>( m_invDeltaTabulatedDistances*(dist-m_minDistance) ));

  if ( distIndex >= m_nTabulatedDistances)
    {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "getAverageDriftTime: Somehow distIndex (" << distIndex << " ) is out of bounds ("
                                              << m_nTabulatedDistances << "). The job will die now." <<endreq;
      throw;
    }

  const double C(m_table_of_dist2meanDT_at_noField[iType][distIndex]);
  const double A((m_table_of_dist2meanDT_at_maxField[iType][distIndex] - C) * m_invMaxFieldSquared);

  return (A * effectivefield_squared) + C;

}

//__________________________________________________________________________
void TRT_SimDriftTimeTool::getNDriftTimes( const unsigned int& N,
					   std::vector<double>& drifttimes,
					   const double& dist,
					   const double& effectivefield_squared,
                                           bool isArgonStraw) { // last argument added by Sasha for Argon

  // The default isArgonStraw argument in this function gives backward compatibility
  // for older versions of TRT_Digitization where this argument is not given.

  //define straw type (iType):
  // 0 - Xenon
  // 1 - Argon

  int iType = 0; // default is Xenon
  if (m_generatedGaussians.size()==1){ // not ArXeMix regime - depend on m_digversion variable
    iType = 0;
  }
  else{
    if ((m_generatedGaussians.size()==2)&&(isArgonStraw)){
      iType = 1;
    }
  }

  const double averageDT(getAverageDriftTime(dist, effectivefield_squared, isArgonStraw)); // last argument added by Sasha for Argon
  const unsigned int distIndex(static_cast<int>( m_invDeltaTabulatedDistances*(dist-m_minDistance) ));
  const double relSpread(m_table_of_RMSdiffusionToDT[iType][distIndex]);

  //To speed up we use Gaussians tabulated at startup. Also, this is not exactly Gaussians, but Gaussians
  //which have a part of their (quite outlying) lower tail cut off (to ensure non-negative drifttimes).
  drifttimes.resize(N);
  for (unsigned int i(0); i<N; ++i) {
    drifttimes[i] = averageDT * ( 1.0 + relSpread * m_generatedGaussians[iType][m_runningGaussianIndex++] );
    if (m_runningGaussianIndex==m_numberGeneratedGaussians)
      {
	m_runningGaussianIndex=0;
      }
  }
  return;
}
