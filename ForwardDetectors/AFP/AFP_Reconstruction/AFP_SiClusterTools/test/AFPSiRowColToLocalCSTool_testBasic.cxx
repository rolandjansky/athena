/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include<iostream>

#include<TString.h>

#include "TestTools/initGaudi.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGate.h"
#include "StoreGate/StoreGateSvc.h"


#include "xAODForward/AFPSiHitsCluster.h"
#include "xAODForward/AFPSiHitsClusterContainer.h"
#include "xAODForward/AFPSiHitsClusterAuxContainer.h"

#include "AFP_SiClusterTools/AFPSiClusterBasicObj.h"
#include "AFP_SiClusterTools/IAFPSiRowColToLocalCSTool.h"

#include "TestUtils.h"

StoreGateSvc* pStore(nullptr);

using namespace std;


StatusCode testFullTransform (IAFPSiRowColToLocalCSTool* rowColTool)
{
  BEGIN_TEST("Testing Pixels");

  const int stationID = 2;
  //  const int layerID = 1;

  const float horizID = 335;
  const float vertID = 2;
  const float charge = 3000;
  const float algID = 4;
  AFPSiClusterBasicObj basicCluster (horizID, vertID, charge, algID);
  REPORT_AND_CONTINUE("horizIDVertID coordinates " << basicCluster.horizID()<<" "<<basicCluster.vertID());

  TString clustersContainerName = "AFPSiHitsClusterContainer";
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
  clusterContainer->setStore(clusterAuxContainer);

  for (int i = 0; i < 4; ++i) {
    xAOD::AFPSiHitsCluster* xAODCluster = rowColTool->newXAODLocal(stationID, i, basicCluster, clusterContainer);
    REPORT_AND_CONTINUE("Local coordinates " << i<<": "<< 
			xAODCluster->xLocal()<<" +- "<<xAODCluster->xLocalErr()
			<<"\t"<<xAODCluster->yLocal()<<" +- "<<xAODCluster->yLocalErr()
			<<"\t"<<xAODCluster->zLocal()<<" +- "<<xAODCluster->zLocalErr()
			);
  }

  // if (xAODCluster->xLocal() != horizID*scaleX)
  //   REPORT_AND_STOP("Wrong X scaling");

  // if (xAODCluster->yLocal() != vertID*scaleY)
  //   REPORT_AND_STOP("Wrong Y scaling");


  END_TEST;
}

StatusCode testScaling (IAFPSiRowColToLocalCSTool* rowColTool)
{
  BEGIN_TEST("Testing Scaling");

  const int stationID = 0;
  const int layerID = 0;

  const float scaleX = 20;
  const float scaleY = 30;
  const float scaleZ = 1;
  rowColTool->transformations()[stationID][layerID] = HepGeom::Scale3D(scaleX, scaleY, scaleZ);

  const float horizID = 1;
  const float vertID = 2;
  const float charge = 3000;
  const float algID = 4;
  AFPSiClusterBasicObj basicCluster (horizID, vertID, charge, algID);
  REPORT_AND_CONTINUE("horizIDVertID coordinates " << basicCluster.horizID()<<" "<<basicCluster.vertID());

  TString clustersContainerName = "AFPSiHitsClusterContainer";
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
  clusterContainer->setStore(clusterAuxContainer);

  xAOD::AFPSiHitsCluster* xAODCluster = rowColTool->newXAODLocal(stationID, layerID, basicCluster, clusterContainer);

  if (xAODCluster->xLocal() != horizID*scaleX)
    REPORT_AND_STOP("Wrong X scaling");

  if (xAODCluster->yLocal() != vertID*scaleY)
    REPORT_AND_STOP("Wrong Y scaling");

  REPORT_AND_CONTINUE("Local coordinates " << xAODCluster->xLocal()<<" "<<xAODCluster->yLocal()<<" "<<xAODCluster->zLocal());

  END_TEST;
}

StatusCode testTranslation (IAFPSiRowColToLocalCSTool* rowColTool)
{
  BEGIN_TEST("Testing Translation");

  const int stationID = 0;
  const int layerID = 0;

  const float shiftX = 10;
  const float shiftY = 100;
  const float shiftZ = 1000;
  rowColTool->transformations()[stationID][layerID] = HepGeom::Translate3D(shiftX, shiftY, shiftZ);

  const float horizID = 1;
  const float vertID = 2;
  const float charge = 3000;
  const float algID = 4;
  AFPSiClusterBasicObj basicCluster (horizID, vertID, charge, algID);

  TString clustersContainerName = "AFPSiHitsClusterContainer";
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
  clusterContainer->setStore(clusterAuxContainer);

  xAOD::AFPSiHitsCluster* xAODCluster = rowColTool->newXAODLocal(0, 0, basicCluster, clusterContainer);

  if (xAODCluster->xLocal() != (horizID + shiftX))
    REPORT_AND_STOP("Wrong X scaling and translation");
  
  if (xAODCluster->yLocal() != (vertID + shiftY))
    REPORT_AND_STOP("Wrong Y scaling and translation");


  REPORT_AND_CONTINUE("Local coordinates " << xAODCluster->xLocal()<<" "<<xAODCluster->yLocal()<<" "<<xAODCluster->zLocal());

  END_TEST;
}

StatusCode testScalingAndTranslation (IAFPSiRowColToLocalCSTool* rowColTool)
{
  BEGIN_TEST("Testing Scaling and Translation");

  const int stationID = 0;
  const int layerID = 0;

  const float scaleX = 20;
  const float scaleY = 30;
  const float scaleZ = 1;
  const HepGeom::Transform3D scale = HepGeom::Scale3D(scaleX, scaleY, scaleZ);

  const float shiftX = 10;
  const float shiftY = 100;
  const float shiftZ = 1000;
  const  HepGeom::Transform3D shift =  HepGeom::Translate3D(shiftX, shiftY, shiftZ);
  rowColTool->transformations()[stationID][layerID] = shift*scale;

  const float horizID = 1;
  const float vertID = 2;
  const float charge = 3000;
  const float algID = 4;
  AFPSiClusterBasicObj basicCluster (horizID, vertID, charge, algID);
  REPORT_AND_CONTINUE("horizIDVertID coordinates " << basicCluster.horizID()<<" "<<basicCluster.vertID());

  TString clustersContainerName = "AFPSiHitsClusterContainer";
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
  clusterContainer->setStore(clusterAuxContainer);

  xAOD::AFPSiHitsCluster* xAODCluster = rowColTool->newXAODLocal(stationID, layerID, basicCluster, clusterContainer);

  if (xAODCluster->xLocal() != (horizID*scaleX) + shiftX)
    REPORT_AND_STOP("Wrong X scaling and translation");

  if (xAODCluster->yLocal() != (vertID*scaleY) + shiftY)
    REPORT_AND_STOP("Wrong Y scaling and translation");

  REPORT_AND_CONTINUE("Local coordinates " << xAODCluster->xLocal()<<" "<<xAODCluster->yLocal()<<" "<<xAODCluster->zLocal());

  END_TEST;
}

StatusCode testRotation (IAFPSiRowColToLocalCSTool* rowColTool)
{
  BEGIN_TEST("Testing Rotation X");

  const float horizIDX = 0;
  const float vertIDX = 1;
  const float charge = 3000;
  const float algID = 4;
  AFPSiClusterBasicObj basicCluster (horizIDX, vertIDX, charge, algID);
  REPORT_AND_CONTINUE("horizIDVertID coordinates " << basicCluster.horizID()<<" "<<basicCluster.vertID());

  TString clustersContainerName = "AFPSiHitsClusterContainer";
  xAOD::AFPSiHitsClusterContainer* clusterContainer = new xAOD::AFPSiHitsClusterContainer();
  xAOD::AFPSiHitsClusterAuxContainer *clusterAuxContainer = new xAOD::AFPSiHitsClusterAuxContainer();
  clusterContainer->setStore(clusterAuxContainer);

  const int stationID = 0;
  const int layerID = 0;

  // rotation X
  const float angleX = CLHEP::pi/2.;
  const HepGeom::Transform3D rotateX = HepGeom::RotateX3D(angleX);
  rowColTool->transformations()[stationID][layerID] = rotateX;

  xAOD::AFPSiHitsCluster* xRotated = rowColTool->newXAODLocal(0, 0, basicCluster, clusterContainer);

  REPORT_AND_CONTINUE("Local coordinates X rotation: " << xRotated->xLocal()<<" "<<xRotated->yLocal()<<" "<<xRotated->zLocal());

  // rotation Y
  const float horizIDY = 1;
  const float vertIDY = 0;
  basicCluster.setHorizID(horizIDY); basicCluster.setVertID(vertIDY);

  //  const float angleY = CLHEP::pi/2.;
  const float angleY = -14.*CLHEP::pi/180.;
  const HepGeom::Transform3D rotateY = HepGeom::RotateY3D(angleY);
  rowColTool->transformations()[stationID][layerID] = rotateY;

  xAOD::AFPSiHitsCluster* yRotated = rowColTool->newXAODLocal(0, 0, basicCluster, clusterContainer);

  REPORT_AND_CONTINUE("Local coordinates Y rotation: " << yRotated->xLocal()<<" "<<yRotated->yLocal()<<" "<<yRotated->zLocal());

  // rotation Z
  const float angleZ = CLHEP::pi/2.;
  const HepGeom::Transform3D rotateZ = HepGeom::RotateZ3D(angleZ);
  rowColTool->transformations()[stationID][layerID] = rotateZ;

  xAOD::AFPSiHitsCluster* zRotated = rowColTool->newXAODLocal(0, 0, basicCluster, clusterContainer);

  REPORT_AND_CONTINUE("Local coordinates Z rotation: " << zRotated->xLocal()<<" "<<zRotated->yLocal()<<" "<<zRotated->zLocal());

  END_TEST;
}

int main() 
{
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("test.txt",  pSvcLoc)) {
    cerr << "ERROR This test can not be run" << endl;
    return 0;
  }
  assert(pSvcLoc);
  MsgStream log(Athena::getMessageSvc(), "AFPSSiRowColToLocalCSTool_testBasic");
  msglog = &log;

  if( pSvcLoc->service("StoreGateSvc", pStore, true).isSuccess() ) {
    *msglog << MSG::DEBUG << "SG pointer: " << pStore << endmsg;
  } else {
    ABORT( "ERROR no SG available" );
  }

  IToolSvc* toolSvc;
  if( pSvcLoc->service("ToolSvc", toolSvc, true).isSuccess()  ) {
    log << MSG::DEBUG << "ToolSvc pointer: " << toolSvc << endmsg;
  } else 
    ABORT ( "no ToolSvc available" );

  
  IAlgTool* algTool = nullptr;
  IAFPSiRowColToLocalCSTool* rowColTool = nullptr;
  if ( toolSvc->retrieveTool("AFPSiRowColToLocalCSTool", algTool).isSuccess() ) {
    log << MSG::DEBUG << "OK navigation tool retrieved" << endmsg;
    rowColTool = dynamic_cast< IAFPSiRowColToLocalCSTool*>(algTool);
    if ( rowColTool ) {
      log << MSG::DEBUG << "OK tool casted" << endmsg;    
    } else 
      ABORT( "tool cast failed" );    

  } else  ABORT("tool NOT retrieved" );

  // if (testScaling(rowColTool).isFailure())
  //   ABORT("");

  // if (testTranslation(rowColTool).isFailure())
  //   ABORT("");

  // if (testScalingAndTranslation(rowColTool).isFailure())
  //   ABORT("");

  // if (testRotation(rowColTool).isFailure())
  //   ABORT("");

  if (testFullTransform(rowColTool).isFailure())
    ABORT("");
    
  REPORT_AND_CONTINUE( "END all went fine" );
  return 0;
}
