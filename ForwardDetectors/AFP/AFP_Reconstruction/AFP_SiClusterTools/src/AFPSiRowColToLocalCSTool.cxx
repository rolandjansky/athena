/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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

#include "AFP_SiClusterTools/AFPSiRowColToLocalCSTool.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "AFP_Geometry/AFP_constants.h"

AFPSiRowColToLocalCSTool::AFPSiRowColToLocalCSTool(const std::string &type,
                                             const std::string &name,
                                             const IInterface *parent)
    : base_class(type, name, parent)
{
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


  initTransformationMatricesSize(matricesForLayersInit, m_numberOfLayersInStations);

  // count total number of layers
  unsigned int totalLayersN = 0;
  for (const int layersN : m_numberOfLayersInStations)
    totalLayersN += layersN;
  
  // --- initialise transformations for each layer in station ---
  bool useDefualtTransformation = true;
  if (!m_transformationsInit.empty() && !m_transformationsErrInit.empty()) {
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
        ATH_MSG_WARNING("Incorrect number of elements in transformationErr vector in job options. Should be "<<totalLayersN<<"x4x3="<<totalLayersN*4*3<<". Is "<<m_transformationsErrInit.size()<<". Will use default transformation.");
    } // close if (m_transformationsInit.size() == totalLayersN*4*3)
    else
      ATH_MSG_WARNING("Incorrect number of elements in transformation vector in job options. Should be "<<totalLayersN<<"x4x3="<<totalLayersN*4*3<<". Is "<<m_transformationsInit.size()<<". Will use default transformation.");
  } // close if (m_transformationsInit.size() && m_transformationsErrInit.size())
  else
    ATH_MSG_INFO("In job options m_transformationsInit and m_transformationsErrInit are not defined simultaneously, so default transformations will be used.");

  // if there were problems with job options scaling or none was defined use default values
  if (useDefualtTransformation) {
    //---->Extra brackets are for vexing parse issues under clang14<---
    const ROOT::Math::Transform3D rotationA((ROOT::Math::RotationY(AFP_CONSTANTS::SiT_Plate_rot_xz))); // rotate by +14 degrees
    const ROOT::Math::Transform3D rotationC(ROOT::Math::RotationY(-1.*AFP_CONSTANTS::SiT_Plate_rot_xz)); // rotate by -14 degrees
    
    const ROOT::Math::Translation3D translationA0(0, 0, 0.);
    const ROOT::Math::Translation3D translationA1(0, 0, 1.*AFP_CONSTANTS::SiT_Plate_zsep);
    const ROOT::Math::Translation3D translationA2(0, 0, 2.*AFP_CONSTANTS::SiT_Plate_zsep);
    const ROOT::Math::Translation3D translationA3(0, 0, 3.*AFP_CONSTANTS::SiT_Plate_zsep);
    
    const ROOT::Math::Translation3D translationC0(0, 0, 0.);
    const ROOT::Math::Translation3D translationC1(0, 0,-1.*AFP_CONSTANTS::SiT_Plate_zsep);
    const ROOT::Math::Translation3D translationC2(0, 0,-2.*AFP_CONSTANTS::SiT_Plate_zsep);
    const ROOT::Math::Translation3D translationC3(0, 0,-3.*AFP_CONSTANTS::SiT_Plate_zsep);
    
    // station 0
    m_transformationsErr[0][0] = rotationA;
    m_transformationsErr[0][1] = rotationA;
    m_transformationsErr[0][2] = rotationA;
    m_transformationsErr[0][3] = rotationA;

    m_transformations[0][0] = translationA0*m_transformationsErr[0][0];
    m_transformations[0][1] = translationA1*m_transformationsErr[0][1];
    m_transformations[0][2] = translationA2*m_transformationsErr[0][2];
    m_transformations[0][3] = translationA3*m_transformationsErr[0][3];

    // station 1
    m_transformationsErr[1][0] = rotationA;
    m_transformationsErr[1][1] = rotationA;
    m_transformationsErr[1][2] = rotationA;
    m_transformationsErr[1][3] = rotationA;

    m_transformations[1][0] = translationA0*m_transformationsErr[1][0];
    m_transformations[1][1] = translationA1*m_transformationsErr[1][1];
    m_transformations[1][2] = translationA2*m_transformationsErr[1][2];
    m_transformations[1][3] = translationA3*m_transformationsErr[1][3];

    // station 2
    m_transformationsErr[2][0] = rotationC;
    m_transformationsErr[2][1] = rotationC;
    m_transformationsErr[2][2] = rotationC;
    m_transformationsErr[2][3] = rotationC;

    m_transformations[2][0] = translationC0*m_transformationsErr[2][0];
    m_transformations[2][1] = translationC1*m_transformationsErr[2][1];
    m_transformations[2][2] = translationC2*m_transformationsErr[2][2];
    m_transformations[2][3] = translationC3*m_transformationsErr[2][3];

    // station 3
    m_transformationsErr[3][0] = rotationC;
    m_transformationsErr[3][1] = rotationC;
    m_transformationsErr[3][2] = rotationC;
    m_transformationsErr[3][3] = rotationC;

    m_transformations[3][0] = translationC0*m_transformationsErr[3][0];
    m_transformations[3][1] = translationC1*m_transformationsErr[3][1];
    m_transformations[3][2] = translationC2*m_transformationsErr[3][2];
    m_transformations[3][3] = translationC3*m_transformationsErr[3][3];
  }

  int stationCounter(0);
  ATH_MSG_DEBUG("The following transformation matrices will be used");
  for (const std::vector<ROOT::Math::Transform3D>& station : m_transformations) {
    int layersCounter (0);
    ATH_MSG_DEBUG("Station "<<stationCounter++);
    for (const ROOT::Math::Transform3D& layerTrans : station)
      ATH_MSG_DEBUG ("Layer "<<layersCounter++<<" "<<layerTrans);
  } // close for (stations)

  stationCounter = 0;
  ATH_MSG_DEBUG("The following transformation matrices for errors will be used");
  for (const std::vector<ROOT::Math::Transform3D>& station : m_transformationsErr) {
    int layersCounter (0);
    ATH_MSG_DEBUG("Station "<<stationCounter++);
    for (const ROOT::Math::Transform3D& layerTrans : station)
      ATH_MSG_DEBUG ("Layer "<<layersCounter++<<" "<<layerTrans);
  } // close for (stations)
      
  return StatusCode::SUCCESS;
}

StatusCode AFPSiRowColToLocalCSTool::finalize() {
  return StatusCode::SUCCESS;
}

void AFPSiRowColToLocalCSTool::fillXAOD (const int stationID, const int layerID, const ROOT::Math::XYZPoint& position, const ROOT::Math::XYZPoint& positionError, xAOD::AFPSiHitsCluster* xAODCluster) const
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

xAOD::AFPSiHitsCluster* AFPSiRowColToLocalCSTool::newXAODLocal (const int stationID, const int layerID, const AFP::SiLocAlignData& LA, const AFP::SiGlobAlignData& GA, const AFPSiClusterBasicObj& cluster, std::unique_ptr<xAOD::AFPSiHitsClusterContainer>& xAODContainer) const
{
  float flipAC = (stationID<2 ? -1. : +1.);
  
  ROOT::Math::XYZPoint horizVertID ((cluster.horizID()-AFP_CONSTANTS::SiT_Pixel_amount_x)*m_pixelHorizSize, flipAC*cluster.vertID()*m_pixelVertSize, 0);
  ROOT::Math::XYZPoint horizVertIDErr (cluster.horizIDErr()*m_pixelHorizSize, cluster.vertIDErr()*m_pixelVertSize, 0);

  try {
    // these horrible things are needed in order to have compatibility with AFPAnalysisToolbox
    const ROOT::Math::RotationZYX locRotation(flipAC*LA.alpha(), -1.*flipAC*LA.gamma(), LA.beta()); // ATLAS coord.
    const ROOT::Math::Translation3D locTranslation(-1.*LA.yShift(), flipAC*LA.xShift(), flipAC*LA.zShift()); // ATLAS coord.
    const ROOT::Math::Translation3D globTranslation(GA.xShift(), flipAC*GA.yShift(), GA.zShift()); // ATLAS coord.
    
    ROOT::Math::XYZPoint localCS = globTranslation*locTranslation*locRotation*m_transformations.at(stationID).at(layerID)*horizVertID;
	ROOT::Math::XYZPoint localCSError = locRotation*m_transformationsErr.at(stationID).at(layerID)*horizVertIDErr;

    auto xAODCluster = xAODContainer->push_back(std::make_unique<xAOD::AFPSiHitsCluster>());

    fillXAOD (stationID, layerID, localCS, localCSError, xAODCluster);
    xAODCluster->setDepositedCharge(cluster.charge());
    xAODCluster->setNHits(cluster.hits().size());

    ATH_MSG_DEBUG("Cluster info: station="<<stationID<<" layer="<<layerID<<" horizID="<<cluster.horizID()<<" vertID="<<cluster.vertID()<<"\tcoordinates: x="<<xAODCluster->xLocal()<<" y="<<xAODCluster->yLocal());
    ATH_MSG_DEBUG("hits: ");      
    for (const xAOD::AFPSiHit* theHit : cluster.hits() )
      ATH_MSG_DEBUG("("<<theHit->pixelHorizID()<<", "<<theHit->pixelVertID()<<")");

    return xAODCluster;
  } // close try
  catch (const std::out_of_range& outOfRange) {
    ATH_MSG_WARNING("Transformation matrix for station or pixel ID outside expected range. Aborting transformation to xAOD.");
    return nullptr;
  }
}

