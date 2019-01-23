/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "DBM_Module.h"

#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoBox.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetReadoutGeometry/PixelDiodeMatrix.h"
using namespace InDetDD;

DBM_Module::DBM_Module() {

  double thickness = 0.5;

  int circuitsPhi = 1;
  int circuitsEta = 1;
  int cellRowPerCirc = 80;
  int cellColPerCirc = 336;
  int diodeRowPerCirc = 80;
  int diodeColPerCirc = 336;
  int readoutSide = 1;

  double etaPitchLongEnd =  0.05;
  double etaPitchLong =  0.05;
  double phiPitch = 0.25;
  double etaPitch = 0.05;

  PixelDiodeMatrix * fullMatrix = makeMatrix(phiPitch, etaPitch, etaPitchLong, etaPitchLongEnd,
					     circuitsPhi, circuitsEta, diodeRowPerCirc, diodeColPerCirc);
 
  PixelModuleDesign *p_dbmdesign = new PixelModuleDesign(thickness,
							     circuitsPhi,
							     circuitsEta,
							     cellColPerCirc,
							     cellRowPerCirc,
							     diodeColPerCirc,
							     diodeRowPerCirc,
							     fullMatrix,
							     InDetDD::electrons,
							     readoutSide);

  m_design = p_dbmdesign;

  m_DDmgr->addDesign(m_design);


}

GeoVPhysVol* DBM_Module::Build()  
{

  GeoIdentifierTag* diamondTag = new GeoIdentifierTag(400);


    double safety = 0.003*Gaudi::Units::mm;

    //diamond dimension
    double diamond_X = m_gmt_mgr->DBMDiamondX();
    double diamond_Y = m_gmt_mgr->DBMDiamondY();
    double diamond_Z = m_gmt_mgr->DBMDiamondZ();
    double air_gap = m_gmt_mgr->DBMAirGap();
    
    //chip, FEI4 dimension
    double chip_X = m_gmt_mgr->DBMFEI4X();
    double chip_Y = m_gmt_mgr->DBMFEI4Y();
    double chip_thick = m_gmt_mgr->DBMFEI4Z();

    //ceramic dimension
    double substrate_X = m_gmt_mgr->DBMCeramicX();
    double substrate_Y = m_gmt_mgr->DBMCeramicY();
    double substrate_Z = m_gmt_mgr->DBMCeramicZ();
    
    //distances from bottom of the ceramic
    //Hardcoded!
    double bot2Chip = 0.0*Gaudi::Units::mm;
    double bot2Diamond = 1.685*Gaudi::Units::mm;


    //---------------------------------------------
    // Get parameters for alignable Transform
  
    // Position of the corner closest to IP and beamline
    // Hardcoded, so if change then change in GeoPixelEnvelope and DBM_Det too
    double ZToIP = 887.002*Gaudi::Units::mm;
    double RToBeam = 46.678*Gaudi::Units::mm;

    // layer spacing
    double Zspacing = m_gmt_mgr->DBMSpacingZ();
    double Rspacing = m_gmt_mgr->DBMSpacingRadial();
    // gap between V-slide and first main plate
    double layer1Space = m_gmt_mgr->DBMSpace();

    // parameters for rotating the 3-layer unit
    double angle = m_gmt_mgr->DBMAngle(); // telescope tilting angle in degree
    double bracketZ = m_gmt_mgr->DBMBracketZ(); // total thickness of the bracket unit,
    double trapBackY = m_gmt_mgr->DBMTrapezBackY();
    double trapBackShortZ = m_gmt_mgr->DBMTrapezBackShortZ();
    double coolingSidePlateY = m_gmt_mgr->DBMCoolingSidePlateY();
    double brcktLockZ = m_gmt_mgr->DBMBrcktLockZ();
  
    //double lyRadius = sqrt(layerUnitY*layerUnitY/4 + layerUnitZ*layerUnitZ/4);
    //double lyAngle = atan(layerUnitY/layerUnitZ);
    // position of the 3-layer unit's corner closest to the IP and beamline, which is the rotation point
    double layerUnitPos_Y = (trapBackY/cos(angle) - coolingSidePlateY)*cos(angle);
    double layerUnitPos_Z = coolingSidePlateY*sin(angle) + trapBackShortZ + bracketZ - brcktLockZ; 
    //---------------------------------------------



    const GeoMaterial* air = m_mat_mgr->getMaterial("std::Air");
    //first try the Diamond
    const GeoMaterial* diamond = m_mat_mgr->getMaterial("pix::Diamond");
    if(diamond == NULL)
    {
  	diamond = m_mat_mgr->getMaterial("std::Carbon");	
    }
  
    const GeoMaterial* chip_mat = m_mat_mgr->getMaterial("pix::ChipBase");
    /*if(chip == NULL)
    {
  	std::cout << "MATERIAL ERROR: NO CHIPIBL FOUND\n";
  	chip = m_mat_mgr->getMaterial("pix::Chip");
  	if(chip == NULL)
  	     std::cout << "MATERIAL ERROR: NO CHIP FOUND\n";
    }
    else
        std::cout << "MATERIAL WARNING: CHIPIBL FOUND\n";
    */
    
    double max_thick = diamond_Z + air_gap + chip_thick + substrate_Z;
    
    //create a outer box to contain all volumes
    const GeoBox* dbmModuleBox = new GeoBox(substrate_X/2.0 + safety, substrate_Y/2.0 + safety, max_thick/2.0 + safety);
    const GeoLogVol* dbmModuleLog = new GeoLogVol("dbmModuleLog", dbmModuleBox, air);
    GeoPhysVol* dbmModulePhys = new GeoPhysVol(dbmModuleLog);
    
    GeoTrf::Transform3D rm = GeoTrf::RotateZ3D(90.*Gaudi::Units::deg)*GeoTrf::RotateY3D(270.*Gaudi::Units::deg);

    //diamond
    const GeoBox* dbmDiamondBox = new GeoBox(diamond_Z/2.0, diamond_X/2.0, diamond_Y/2.0 );
    const GeoLogVol* dbmDiamondLog = new GeoLogVol("dbmDiamondLog", dbmDiamondBox, diamond);
    GeoFullPhysVol* dbmDiamondPhys = new GeoFullPhysVol(dbmDiamondLog);

    //*******************************
    // Create id and add element
    //*******************************
    const PixelID * idHelper = m_gmt_mgr->getIdHelper();
    int dbmdet = 4*m_gmt_mgr->GetSide();
    Identifier idwafer;
    idwafer = idHelper->wafer_id(dbmdet,m_gmt_mgr->GetLD(),m_gmt_mgr->Phi(),m_gmt_mgr->Eta());

    // if (gmt_mgr->msgLvl(MSG::INFO)) {
    //   gmt_mgr->msg(MSG::INFO) << "BEGIN DBM diamond crystal" << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "check identifier for side="<< dbmdet <<" layer=" << gmt_mgr->GetLD() << " phi=" << gmt_mgr->Phi() << " eta=" << gmt_mgr->Eta() << endmsg; 
    //   gmt_mgr->msg(MSG::INFO) << "is pixel = " << idHelper->is_pixel(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "is barrel = " << idHelper->is_barrel(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "barrelendcap = " << idHelper->barrel_ec(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "layer = " << idHelper->layer_disk(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "phi_module = " << idHelper->phi_module(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "eta module = " << idHelper->eta_module(idwafer) << endmsg;
    //   gmt_mgr->msg(MSG::INFO) << "END DBM diamond crystal" << endmsg;
    //}

    SiDetectorElement * element = new SiDetectorElement(idwafer, m_design, dbmDiamondPhys, m_gmt_mgr->commonItems());
    
    // add the element to the manager
    m_DDmgr->addDetectorElement(element);

    GeoTrf::Translation3D dbmDiamondPos(0, bot2Diamond+diamond_Y/2.0-substrate_Y/2.0, diamond_Z/2.0-max_thick/2.0);
    GeoTransform* xform = new GeoTransform(GeoTrf::Transform3D(dbmDiamondPos*rm));
    //tag = new GeoNameTag("dbmDiamond");
    //dbmModulePhys->add(tag);
    dbmModulePhys->add(diamondTag);
    dbmModulePhys->add(xform);
    dbmModulePhys->add(dbmDiamondPhys);
    
    //FEI4 chip
    const GeoBox* dbmFEI4Box = new GeoBox(chip_thick/2.0, chip_X/2.0, chip_Y/2.0 );
    const GeoLogVol* dbmFEI4Log = new GeoLogVol("dbmWallLogF4", dbmFEI4Box, chip_mat); //should this be "dbmdiamondLog"?
    GeoPhysVol* dbmFEI4Phys = new GeoPhysVol(dbmFEI4Log);
    
    GeoTrf::Translation3D dbmFEI4Pos(0, bot2Chip+chip_Y/2.0-substrate_Y/2.0, max_thick/2.0-substrate_Z-chip_thick/2.0);
    xform = new GeoTransform(GeoTrf::Transform3D(dbmFEI4Pos*rm));
    //tag = new GeoNameTag("dbmFEI4");
    //dbmModulePhys->add(tag);
    dbmModulePhys->add(xform);
    dbmModulePhys->add(dbmFEI4Phys);
    
    //ceramic support
    const GeoBox* dbmSubstBox = new GeoBox(substrate_X/2.0, substrate_Y/2.0, substrate_Z/2.0);
    const GeoMaterial* aluminiumNitride = m_mat_mgr->getMaterialForVolume("pix::DBMCeramic", dbmSubstBox->volume());
    const GeoLogVol* dbmSubstLog = new GeoLogVol("dbmWallLogCe", dbmSubstBox, aluminiumNitride);
    GeoPhysVol* dbmSubstPhys = new GeoPhysVol(dbmSubstLog);

    GeoTrf::Translate3D dbmSubstPos(0, 0, max_thick/2.0-substrate_Z/2.0);
    xform = new GeoTransform(dbmSubstPos);
    //tag = new GeoNameTag("dbmSubstrate");
    //dbmModulePhys->add(tag);
    dbmModulePhys->add(xform);
    dbmModulePhys->add(dbmSubstPhys);
    
    //-----------------------------------------------------
    //Add to alignable transform
    //  DBM has only level 0 alignable transform.
    //  So, a transform w.r.t global position is created.
    //  This mean the alignable pos below should be 
    //  the global position of the sensor

    int layer = m_gmt_mgr->GetLD();
    double sensorPosInModuleCage_Z = layer1Space + layer*Zspacing - (substrate_Z + chip_thick + air_gap + diamond_Z/2.);
    double sensorPosInModuleCage_Y = Rspacing + bot2Diamond + diamond_Y/2.;
    double globPosZ = ZToIP + layerUnitPos_Z + (sensorPosInModuleCage_Z * cos(angle) - sensorPosInModuleCage_Y * sin(angle));
    double globPosY = RToBeam + layerUnitPos_Y + (sensorPosInModuleCage_Z * sin(angle) + sensorPosInModuleCage_Y * cos(angle));

    GeoTrf::RotateX3D rmX10(-10.*Gaudi::Units::deg);
    GeoTrf::Translation3D alignTransformPos(0, globPosY, globPosZ);
    GeoAlignableTransform *xformAlign = new GeoAlignableTransform(GeoTrf::Transform3D(alignTransformPos*rmX10));
    m_DDmgr->addAlignableTransform(0, idwafer, xformAlign, dbmDiamondPhys);
    //-----------------------------------------------------

    return dbmModulePhys;
}


PixelDiodeMatrix *  DBM_Module::makeMatrix(double phiPitch, double etaPitch, double etaPitchLong, double etaPitchLongEnd,
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
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) <<  "DBMModule: Making matrix (long:normal:long, Standard ATLAS case)" << endmsg;

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
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) <<  "DBMModule: Making matrix (normal:normal:normal)" << endmsg;
    PixelDiodeMatrix * normalCell = new PixelDiodeMatrix(phiPitch, etaPitch); 
    PixelDiodeMatrix * singleRow = new PixelDiodeMatrix(PixelDiodeMatrix::etaDir,
							0, normalCell, circuitsEta*diodeColPerCirc, 0);
    fullMatrix = new PixelDiodeMatrix(PixelDiodeMatrix::phiDir,
				      0, singleRow, circuitsPhi*diodeRowPerCirc, 0);
  } else if (etaPitchLongEnd == etaPitch &&  etaPitchLong != etaPitch && circuitsEta > 2) {
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) <<  "DBMModule: Making matrix (normal:normal:long, > 2 chips)" << endmsg;
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
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) <<  "DBMModule: Making matrix (normal:normal:long, 2 chips)" << endmsg;
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
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG) <<  "DBMModule: Making matrix (end:normal:end, single chips or end:normal:normal)" << endmsg;
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
    if (m_gmt_mgr->msgLvl(MSG::DEBUG)) m_gmt_mgr->msg(MSG::DEBUG)<<  "DBMModule: Making matrix (end:normal:long)" << endmsg;
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
