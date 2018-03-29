/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiRowColToLocalCSTool.cxx
 * @author Grzegorz Gach <gach@agh.edu.pl>
 * @date   2017-04-23
 * 
 * @brief  Implementation file of AFPSiRowColToLocalCSTool
 * 
 */

// STL includes
#include <list>

#include "xAODForward/AFPSiHit.h"
#include "xAODForward/AFPSiHitContainer.h"

#include "AFPSiRowColToLocalCSTool.h"
#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"

#include "CLHEP/Units/SystemOfUnits.h"

AFPSiRowColToLocalCSTool::AFPSiRowColToLocalCSTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : ::AthAlgTool(type, name, parent) 
{
  declareInterface<IAFPSiRowColToLocalCSTool>(this);

  declareProperty ("numberOfLayersInStations", m_numberOfLayersInStations, "The size of the vector sets number of stations. Each element defines number of pixel layers in the station.");
  declareProperty ("transformations", m_transformationsInit, "Initialisation of the transformation matrices (4x3). It is not full transformation only part. Must be defined simultaneously with transformationsErr.");
  declareProperty ("transformationsErr", m_transformationsErrInit, "Initialisation of the error transformation matrices (4x3). It is not full transformation only part. Must be defined simultaneously with transformations.");

  declareProperty ("pixelHorizSize", m_pixelHorizSize = 0.05, "Size of the pixel in horizontal direction when mounted (default = 0.05 mm)");
  declareProperty ("pixelVertSize", m_pixelVertSize = 0.25, "Size of the pixel in vertical direction when mounted (default = 0.25 mm)");
}

void AFPSiRowColToLocalCSTool::initTransformationMatricesSize (std::list<std::vector< std::vector<ROOT::Math::Transform3D> >* >& matrices, const std::vector<int>& layersInStations)
{
  const int numberOfStations = layersInStations.size();
  std::list<std::vector< std::vector<ROOT::Math::Transform3D> >::iterator> iterators;

  // initialise number of stations in each set and prepare iterators
  for (std::vector< std::vector<ROOT::Math::Transform3D> >* theMatrix : matrices) {
    theMatrix->resize(numberOfStations);
    iterators.push_back(theMatrix->begin());
  }

  // initialise number of layers in each station
  for (const int layersN : layersInStations)
    for (std::vector< std::vector<ROOT::Math::Transform3D> >::iterator& matrixIter : iterators)
      (*(matrixIter++)) = std::vector<ROOT::Math::Transform3D> (layersN);
}

StatusCode AFPSiRowColToLocalCSTool::initialize() 
{
  typedef std::vector< std::vector<ROOT::Math::Transform3D> >  TransformVec_t;

  std::list<TransformVec_t*> matricesForLayersInit;
  matricesForLayersInit.push_back(&m_transformations);
  matricesForLayersInit.push_back(&m_transformationsErr);

  // --- initialise size of vector of transformations for each layer in station ---
  if (m_numberOfLayersInStations.size() == 0) {
    // no job options configuration - use default values
    m_numberOfLayersInStations.resize(4, 4); // set number of stations to 4
    //    for (int& layersInStation : m_numberOfLayersInStations) layersInStation = 4;
  }

  initTransformationMatricesSize(matricesForLayersInit, m_numberOfLayersInStations);

  // count total number of layers
  unsigned int totalLayersN = 0;
  for (const int layersN : m_numberOfLayersInStations)
    totalLayersN += layersN;
  
  // --- initialise transformations for each layer in station ---
  bool useDefualtTransformation = true;
  if (m_transformationsInit.size() > 0 && m_transformationsErrInit.size() > 0) {
    // check if the number of arguments is correct for (4x3) matrices
    if (m_transformationsInit.size() == totalLayersN*4*3) {
      if (m_transformationsErrInit.size() == totalLayersN*4*3) {

	std::vector<double>::const_iterator valueIter = m_transformationsInit.begin();
	for (std::vector<ROOT::Math::Transform3D>& station : m_transformations)
	  for (ROOT::Math::Transform3D& layerTransform : station) {
	    layerTransform.SetComponents(valueIter, valueIter+12);
	    valueIter += 12;
	  }

	std::vector<double>::const_iterator valueErrIter = m_transformationsErrInit.begin();
	for (std::vector<ROOT::Math::Transform3D>& station : m_transformationsErr)
	  for (ROOT::Math::Transform3D& layerTransform : station) {
	    layerTransform.SetComponents(valueErrIter, valueErrIter+12);
	    valueErrIter += 12;
	  }
      
	useDefualtTransformation = false;
      }
      else
	ATH_MSG_WARNING("Incorrect number of elements in transformationErr vector in job options. Should be "<<totalLayersN<<"x4x3="<<totalLayersN*4*3<<". Is "<<m_transformationsErrInit.size()<<".");
    } // close if (m_transformationsInit.size() == totalLayersN*4*3)
    else
      ATH_MSG_WARNING("Incorrect number of elements in transformation vector in job options. Should be "<<totalLayersN<<"x4x3="<<totalLayersN*4*3<<". Is "<<m_transformationsInit.size()<<".");
  } // close if (m_transformationsInit.size() && m_transformationsErrInit.size())
  else
    ATH_MSG_INFO("In job options m_transformationsInit and m_transformationsErrInit are not defined simultaneously, so default transformations will be used.");

  // if there were problems with job options scaling or none was defined use default values
  if (useDefualtTransformation) {
    const ROOT::Math::Transform3D rotation (ROOT::Math::RotationY (-0.2443461*CLHEP::rad)); // rotate by 14 degrees

    const float layerSpacingZ = 9.*CLHEP::mm;

    // station 0
    m_transformationsErr[0][0] = rotation;
    m_transformationsErr[0][1] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[0][2] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[0][3] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));

    m_transformations[0][0] = ROOT::Math::Translation3D(0, 0, 0)*m_transformationsErr[0][0];
    m_transformations[0][1] = ROOT::Math::Translation3D(0, 0, 0)*ROOT::Math::Translation3D(0, 0, layerSpacingZ)*m_transformationsErr[0][1];
    m_transformations[0][2] = ROOT::Math::Translation3D(12.5*CLHEP::micrometer, -45*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 2.*layerSpacingZ)*m_transformationsErr[0][2];
    m_transformations[0][3] = ROOT::Math::Translation3D(0, 0, 0)*ROOT::Math::Translation3D(0, 0, 3.*layerSpacingZ)*m_transformationsErr[0][3];

    // station 1
    m_transformationsErr[1][0] = rotation;
    m_transformationsErr[1][1] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[1][2] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[1][3] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));

    m_transformations[1][0] = ROOT::Math::Translation3D(0, 0, 0)*m_transformationsErr[1][0];
    m_transformations[1][1] = ROOT::Math::Translation3D(12.5*CLHEP::micrometer, -150*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, layerSpacingZ)*m_transformationsErr[1][1];
    m_transformations[1][2] = ROOT::Math::Translation3D(24*CLHEP::micrometer, 60*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 2.*layerSpacingZ)*m_transformationsErr[1][2];
    m_transformations[1][3] = ROOT::Math::Translation3D(24*CLHEP::micrometer, 50*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 3.*layerSpacingZ)*m_transformationsErr[1][3];

    // station 2
    m_transformationsErr[2][0] = rotation;
    m_transformationsErr[2][1] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[2][2] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[2][3] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));

    m_transformations[2][0] = ROOT::Math::Translation3D(0, 0, 0)*m_transformationsErr[2][0];
    m_transformations[2][1] = ROOT::Math::Translation3D(-70*CLHEP::micrometer, -170*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, layerSpacingZ)*m_transformationsErr[2][1];
    m_transformations[2][2] = ROOT::Math::Translation3D(-115*CLHEP::micrometer, -125*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 2.*layerSpacingZ)*m_transformationsErr[2][2];
    m_transformations[2][3] = ROOT::Math::Translation3D(-185*CLHEP::micrometer, -25*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 3.*layerSpacingZ)*m_transformationsErr[2][3];

    // station 3
    m_transformationsErr[3][0] = rotation;
    m_transformationsErr[3][1] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[3][2] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));
    m_transformationsErr[3][3] = ROOT::Math::Transform3D(ROOT::Math::RotationY(-0.2443461*CLHEP::rad));

    m_transformations[3][0] = ROOT::Math::Translation3D(0, 0, 0)*m_transformationsErr[3][0];
    m_transformations[3][1] = ROOT::Math::Translation3D(-45*CLHEP::micrometer, 175*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, layerSpacingZ)*m_transformationsErr[3][1];
    m_transformations[3][2] = ROOT::Math::Translation3D(-100*CLHEP::micrometer, 140*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 2.*layerSpacingZ)*m_transformationsErr[3][2];
    m_transformations[3][3] = ROOT::Math::Translation3D(-140*CLHEP::micrometer, -15*CLHEP::micrometer, 0)*ROOT::Math::Translation3D(0, 0, 3.*layerSpacingZ)*m_transformationsErr[3][3];
  }

  int stationCounter(0);
  ATH_MSG_INFO("The following transformation matrices will be used");
  for (const std::vector<ROOT::Math::Transform3D>& station : m_transformations) {
    int layersCounter (0);
    ATH_MSG_INFO("Station "<<stationCounter++);
    for (const ROOT::Math::Transform3D& layerTrans : station)
      ATH_MSG_INFO ("Layer "<<layersCounter++<<" "<<layerTrans);
  } // close for (stations)

  stationCounter = 0;
  ATH_MSG_INFO("The following transformation matrices for errors will be used");
  for (const std::vector<ROOT::Math::Transform3D>& station : m_transformationsErr) {
    int layersCounter (0);
    ATH_MSG_INFO("Station "<<stationCounter++);
    for (const ROOT::Math::Transform3D& layerTrans : station)
      ATH_MSG_INFO ("Layer "<<layersCounter++<<" "<<layerTrans);
  } // close for (stations)
      
  return StatusCode::SUCCESS;
}

StatusCode AFPSiRowColToLocalCSTool::finalize() {
  return StatusCode::SUCCESS;
}

void AFPSiRowColToLocalCSTool::fillXAOD (const int stationID, const int layerID, const ROOT::Math::XYZPoint& position, const ROOT::Math::XYZPoint& positionError, xAOD::AFPSiHitsCluster* xAODCluster)
{
    xAODCluster->setStationID(stationID);
    xAODCluster->setPixelLayerID(layerID);

    xAODCluster->setXLocal(position.x());
    xAODCluster->setXLocalErr(positionError.x());

    xAODCluster->setYLocal(position.y());
    xAODCluster->setYLocalErr(positionError.y());

    xAODCluster->setZLocal(position.z());
    xAODCluster->setZLocalErr(positionError.z());
}

xAOD::AFPSiHitsCluster* AFPSiRowColToLocalCSTool::newXAODLocal (const int stationID, const int layerID, const AFPSiClusterBasicObj& cluster, xAOD::AFPSiHitsClusterContainer* xAODContainer)
{
  const int halfColumns = 168;
  const int halfRows = 40;
  ROOT::Math::XYZPoint horizVertID ((cluster.horizID()-halfColumns)*m_pixelHorizSize, (cluster.vertID() - halfRows)*m_pixelVertSize, 0);
  ROOT::Math::XYZPoint horizVertIDErr (cluster.horizIDErr()*m_pixelHorizSize, cluster.vertIDErr()*m_pixelVertSize, 0);

  try {  
    ROOT::Math::XYZPoint localCS = m_transformations.at(stationID).at(layerID)*horizVertID;
    ROOT::Math::XYZPoint localCSError = m_transformationsErr.at(stationID).at(layerID)*horizVertIDErr;

    xAOD::AFPSiHitsCluster* xAODCluster = new xAOD::AFPSiHitsCluster;
    xAODContainer->push_back (xAODCluster);

    fillXAOD (stationID, layerID, localCS, localCSError, xAODCluster);
    xAODCluster->setDepositedCharge(cluster.charge());
    xAODCluster->setNHits(cluster.hits().size());

#ifndef NDEBUG
      if (msg().level() <= MSG::DEBUG) {
    	msg() << MSG::DEBUG<<"Cluster info: station="<<stationID<<" layer="<<layerID<<" horizID="<<cluster.horizID()<<" vertID="<<cluster.vertID()<<"\tcoordinates: x="<<xAODCluster->xLocal()<<" y="<<xAODCluster->yLocal()<<endmsg;
    	msg() << MSG::DEBUG<<"hits: ";	
    	for (const xAOD::AFPSiHit* theHit : cluster.hits() )
    	  msg()<<MSG::DEBUG<<"("<<theHit->pixelHorizID()<<", "<<theHit->pixelVertID()<<")\t";
    	msg()<<MSG::DEBUG<<endmsg;
      }
#endif

    return xAODCluster;
  } // close try
  catch (const std::out_of_range& outOfRange) {
    ATH_MSG_WARNING("Transformation matrix for station or pixel ID outside expected range. Aborting transformation to xAOD.");
    return nullptr;
  }
}

void AFPSiRowColToLocalCSTool::fillXAODContainer (const int stationID, const int layerID, const std::list<AFPSiClusterBasicObj>& clusters, xAOD::AFPSiHitsClusterContainer* xAODContainer)
{
  for (const AFPSiClusterBasicObj& theCluster : clusters)
    newXAODLocal (stationID, layerID, theCluster, xAODContainer);
}
