/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This class builds one Si Crystal to be placed in one module
//
// The information on which layer to build is retrieved by the Geometry Manager
//
// Contained by: PixelModule_log
//
// Contains: nothing
//
#include "PixelGeoModel/GeoPixelSiCrystal.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
#include "InDetReadoutGeometry/SiCommonItems.h"
#include "InDetReadoutGeometry/InDetDD_Defs.h"

#include <vector>

using namespace InDetDD;

GeoPixelSiCrystal::GeoPixelSiCrystal(bool isBLayer, bool isModule3D) 
  : m_logVolume(0)
{
  // 
  //Builds the design for this crystal
  m_isBLayer = isBLayer;
  m_isModule3D = isModule3D;
  //SiDetectorDesign::Axis etaAxis   = SiDetectorDesign::zAxis;
  //SiDetectorDesign::Axis phiAxis   = SiDetectorDesign::yAxis;
  //SiDetectorDesign::Axis depthAxis = SiDetectorDesign::xAxis;

  // Dimensions
  double thickness = gmt_mgr->PixelBoardThickness(m_isModule3D);
  double length = gmt_mgr->PixelBoardLength(m_isModule3D);
  double width = gmt_mgr->PixelBoardWidth(m_isModule3D);

  int circuitsPhi = gmt_mgr->DesignCircuitsPhi(m_isModule3D); // Warning col/row naming opposite to chip
  int circuitsEta = gmt_mgr->DesignCircuitsEta(m_isModule3D); // Warning col/row naming opposite to chip
  int cellRowPerCirc = gmt_mgr->DesignCellRowsPerCircuit(m_isModule3D);
  int cellColPerCirc = gmt_mgr->DesignCellColumnsPerCircuit(m_isModule3D);
  int diodeRowPerCirc = gmt_mgr->DesignDiodeRowsPerCircuit(m_isModule3D);
  int diodeColPerCirc = gmt_mgr->DesignDiodeColumnsPerCircuit(m_isModule3D);
  int readoutSide = gmt_mgr->DesignReadoutSide(m_isModule3D);

  double etaPitchLongEnd =  gmt_mgr->DesignPitchZLongEnd(m_isModule3D);
  double etaPitchLong =  gmt_mgr->DesignPitchZLong(m_isModule3D);
  double phiPitch = gmt_mgr->DesignPitchRP(m_isModule3D);
  double etaPitch = gmt_mgr->DesignPitchZ(m_isModule3D);


//   std::cout<<"thickness = "<<thickness<<std::endl;
//   std::cout<<"length = "<<length<<std::endl;
//     std::cout<<"width = "<<width<<std::endl;

//   std::cout<<"circuitsPhi = "<<circuitsPhi<<std::endl;
//   std::cout<<"circuitsEta = "<<circuitsEta<<std::endl;
//   std::cout<<"cellRowPerCirc = "<<cellRowPerCirc<<std::endl;
//   std::cout<<"cellColPerCirc = "<<cellColPerCirc<<std::endl;
//   std::cout<<"diodeRowPerCirc = "<<diodeRowPerCirc<<std::endl;
//   std::cout<<"diodeColPerCirc = "<<diodeColPerCirc<<std::endl;
//   std::cout<<"readoutSide = "<<readoutSide<<std::endl;
  
//   std::cout<<"etaPitchLongEnd = "<<etaPitchLongEnd<<std::endl;
//   std::cout<<"etaPitchLong =  "<<etaPitchLong<<std::endl;
//   std::cout<<"phiPitch = "<<phiPitch<<std::endl;
//   std::cout<<"etaPitch = "<<etaPitch<<std::endl;

  PixelDiodeMatrix * fullMatrix = makeMatrix(phiPitch, etaPitch, etaPitchLong, etaPitchLongEnd,
					     circuitsPhi, circuitsEta, diodeRowPerCirc, diodeColPerCirc);
 
  PixelModuleDesign *p_barrelDesign2 = new PixelModuleDesign(thickness,
							     circuitsPhi,
							     circuitsEta,
							     cellColPerCirc,
							     cellRowPerCirc,
							     diodeColPerCirc,
							     diodeRowPerCirc,
							     fullMatrix,
							     InDetDD::electrons,
							     readoutSide);

  // Multiple connections (ganged pixels)
  if (gmt_mgr->NumberOfEmptyRows() > 0) {
    int minRow = gmt_mgr->EmptyRows(0);
    int maxRow = minRow;
 

    for (int iConnect = 0; iConnect < gmt_mgr->NumberOfEmptyRows(); iConnect++){
      minRow = std::min(minRow, gmt_mgr->EmptyRows(iConnect));
      minRow = std::min(minRow, gmt_mgr->EmptyRowConnections(iConnect));
      maxRow = std::max(maxRow, gmt_mgr->EmptyRows(iConnect));
      maxRow = std::max(maxRow, gmt_mgr->EmptyRowConnections(iConnect));
    }
  
    std::vector <int> connections(maxRow-minRow+1);
    
    // We fill them all with a one to one correspondence first.
    for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
      connections[iRow] = iRow +  minRow;
    }
    
    // Now make the connections.
    for (int iConnect = 0; iConnect < gmt_mgr->NumberOfEmptyRows(); iConnect++){
      connections[gmt_mgr->EmptyRows(iConnect)-minRow] = gmt_mgr->EmptyRowConnections(iConnect);
    }
    
    //std::cout << "MinRow = " << minRow << std::endl;
    //std::cout << "MaxRow = " << maxRow << std::endl;
    //for (unsigned int iRow = 0; iRow < connections.size(); iRow++){
    //  std::cout << iRow << " " << connections[iRow] << std::endl;
    //}

    p_barrelDesign2->addMultipleRowConnection(minRow, connections);

  } else {
    // No empty rows.
    if (!gmt_mgr->slhc() && !gmt_mgr->ibl()) {
      gmt_mgr->msg(MSG::WARNING) << "GeoPixelSiCrystal: No ganged pixels" << endmsg;
    } else {
      if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) << "GeoPixelSiCrystal: No ganged pixels" << endmsg;
    }
  }
  
  // Check that the active area seems reasonable
//   std::cout<< "GeoPixelSiCrystal: Active area vs sensor size. Sensor: " 
// 	   << width/CLHEP::mm << " x " << length/CLHEP::mm << ", Active: " 
// 	   << gmt_mgr->DesignRPActiveArea(m_isModule3D)/CLHEP::mm << " x " << gmt_mgr->DesignZActiveArea(m_isModule3D)/CLHEP::mm 
// 	   << std::endl;
  if ( (gmt_mgr->DesignRPActiveArea(m_isModule3D) > width) ||
       (gmt_mgr->DesignZActiveArea(m_isModule3D) >  length) || 
       (width - gmt_mgr->DesignRPActiveArea(m_isModule3D) > 4 * CLHEP::mm) || 
       (length - gmt_mgr->DesignZActiveArea(m_isModule3D) > 4 * CLHEP::mm) ) { 
    gmt_mgr->msg(MSG::WARNING) << "GeoPixelSiCrystal: Active area not consistent with sensor size. Sensor: " 
			       << width/CLHEP::mm << " x " << length/CLHEP::mm << ", Active: " 
			       << gmt_mgr->DesignRPActiveArea(m_isModule3D)/CLHEP::mm << " x " << gmt_mgr->DesignZActiveArea(m_isModule3D)/CLHEP::mm 
			       << endmsg;
  } else {
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) 
      << "GeoPixelSiCrystal: Sensor: "  
      << width/CLHEP::mm << " x " << length/CLHEP::mm << ", Active: " 
      << gmt_mgr->DesignRPActiveArea(m_isModule3D)/CLHEP::mm << " x " << gmt_mgr->DesignZActiveArea(m_isModule3D)/CLHEP::mm 
      << endmsg;		       
  }


  m_design = p_barrelDesign2;

  DDmgr->addDesign(m_design);
  
  std::string matName = gmt_mgr->getMaterialName("Sensor");
  const GeoMaterial* siMat = mat_mgr->getMaterial(matName);
  const GeoBox* siBox = new GeoBox(thickness/2.,width/2.,length/2.);
  std::string logname;
  // There is not a strong need to give the blayer a different name but leave it for now. 
  if(m_isBLayer) logname = "siBLayLog";
  else logname = "siLog";
  m_logVolume = new GeoLogVol(logname,siBox,siMat);

}
GeoVPhysVol* GeoPixelSiCrystal::Build() {


  GeoFullPhysVol* siPhys = new GeoFullPhysVol(m_logVolume);

  //
  // Add this to the list of detector elements:
  //
  int brl_ec=0;

  // Build the Identifier for the silicon:
  //
  if(gmt_mgr->isBarrel() ) brl_ec = 0;
  if(gmt_mgr->isEndcap() ) brl_ec = 2*gmt_mgr->GetSide();
  const PixelID * idHelper = gmt_mgr->getIdHelper();
  Identifier idwafer;
  idwafer = idHelper->wafer_id(brl_ec,gmt_mgr->GetLD(),gmt_mgr->Phi(),gmt_mgr->Eta());
 
  _id=idwafer;
  SiDetectorElement * element = new SiDetectorElement(idwafer, m_design, siPhys, gmt_mgr->commonItems());
  
  
  // add the element to the manager
  DDmgr->addDetectorElement(element);
  return siPhys;
}
 

PixelDiodeMatrix *  GeoPixelSiCrystal::makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
						  int circuitsPhi, int circuitsEta, int diodeRowPerCirc, int diodeColPerCirc)
{
  // There are several different cases. Not all are used at the time of wrtiting the code but I
  // have tried to consider all possible cases for completeness. 
  //
  // end cell : middle cells : between chip 
  // --------------------------------------
  // long:normal:long (standard ATLAS case)
  // normal:normal:normal
  // normal:normal:long (> 2 chips)
  // normal:normal:long (2 chips)
  // end:normal:long    (not likely)
  // end:normal:normal  (not likely)
  // end:normal:end  (if single chip)

  PixelDiodeMatrix * fullMatrix = 0;
  
  if (etaPitchLongEnd == etaPitchLong && etaPitchLong != etaPitch) {
    // long:normal:long (standard ATLAS case)
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (long:normal:long, Standard ATLAS case)" << endmsg;

    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * singleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							    bigCell, 
							    normalCell, 
							    diodeColPerCirc-2,
							    bigCell);

    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							0, singleChipRow, circuitsEta, 0);

    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitchLong && (etaPitchLong == etaPitch || circuitsEta == 1)) {
    // normal:normal:normal
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:normal)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							0, normalCell, circuitsEta*diodeColPerCirc, 0);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitch &&  etaPitchLong != etaPitch && circuitsEta > 2) {
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:long, > 2 chips)" << endmsg;
    // normal:normal:long: > 2 chips
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 0, 
								 normalCell, 
								 diodeColPerCirc-1,
								 bigCell);
    PixelDiodeMatrix * middleSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								  bigCell, 
								  normalCell, 
								  diodeColPerCirc-2,
								  bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-1,
								 0);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, middleSingleChipRow, circuitsEta-2, upperSingleChipRow);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitch &&  etaPitchLong != etaPitch && circuitsEta == 2) {
    // normal:normal:long: 2 chips (current SLHC case)
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (normal:normal:long, 2 chips)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 0, 
								 normalCell, 
								 diodeColPerCirc-1,
								 bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-1,
								 0);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, upperSingleChipRow, 1, 0);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (circuitsEta == 1 ||  (etaPitchLongEnd != etaPitch &&  etaPitchLong == etaPitch )){ // etaPitchLongEnd != etaPitch at this stage
    // end:normal:end  (for single chip)
    // end:normal:normal  (not likely)
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (end:normal:end, single chips or end:normal:normal)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLongEnd); 
    
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							    bigCell, 
							    normalCell, 
							    circuitsEta*diodeColPerCirc-2,
							    bigCell);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else {
    // end:normal:long    (not likely)
    if (gmt_mgr->msgLvl(MSG::DEBUG)) gmt_mgr->msg(MSG::DEBUG) <<  "GeoPixelSiCrystal: Making matrix (end:normal:long)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * bigCell = new PixelDiodeMatrix(phiPitch, etaPitchLong); 
    PixelDiodeMatrix * endCell = new PixelDiodeMatrix(phiPitch, etaPitchLongEnd); 
    
    PixelDiodeMatrix * lowerSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 endCell, 
								 normalCell, 
								 diodeColPerCirc-2,
								 bigCell);
    PixelDiodeMatrix * middleSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								  bigCell, 
								  normalCell, 
								  diodeColPerCirc-2,
								  bigCell);
    PixelDiodeMatrix * upperSingleChipRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
								 bigCell, 
								 normalCell, 
								 diodeColPerCirc-2,
								 endCell);
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							lowerSingleChipRow, middleSingleChipRow, circuitsEta-2, upperSingleChipRow);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
    
  }

  return fullMatrix;
}
