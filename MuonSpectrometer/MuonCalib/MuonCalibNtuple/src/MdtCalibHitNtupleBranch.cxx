/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MdtCalibHitBase.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MdtCalibHitNtupleBranch::MdtCalibHitNtupleBranch(std::string branchName) : 
    m_branchName(branchName), branchesInit(false), index(0) 
  {}

  bool MdtCalibHitNtupleBranch::fillBranch(const MdtCalibHitBase &hit, const int segmentIndex) {
    // check if branches where initialized
    if( !branchesInit ) {
      //std::cout << "MdtCalibHitNtupleBranch::fillBranch  ERROR <branches where not initialized>"
	//		<<  std::endl;
      return false;    
    }

    // check if index is out of range 
    if( index >= m_blockSize || index < 0 ){
//       //std::cout << "MdtCalibHitNtupleBranch::fillBranch  ERROR <index out of range, hit not added to ntuple> "
// 		<<  index << std::endl;
      return false;
    }

//    //std::cout << "************************************************************************************" << std::endl;
//    //std::cout << "index = " << index << " with segment " << segmentIndex << std::endl;
//    hit.dump(//std::cout) ;
//    //std::cout << "************************************************************************************" << std::endl;

    // copy values 
    segIndex[index]          = segmentIndex;
    id[index]                = (hit.identify()).getIdInt();
    tdc[index]               = hit.tdcCount();
    adc[index]               = hit.adcCount();
    t[index]                 = hit.driftTime();
    r[index]                 = hit.driftRadius();  
    dr[index]                = hit.sigmaDriftRadius(); 
    rTrk[index]              = hit.signedDistanceToTrack();
    drTrk[index]             = hit.sigmaDistanceToTrack();
    resi[index]              = hit.radialResidual();
    distRO[index]            = hit.distanceToReadout();
    slewTime[index]          = hit.slewingTime();
    lorTime[index]           = hit.bFieldTime();
    propTime[index]          = hit.propagationTime();
    tof[index]               = hit.timeOfFlight();
    bFieldPerp[index]        = hit.bFieldPerp();
    bFieldPara[index]        = hit.bFieldPara();
    temperature[index]       = hit.temperature();
    projSag[index]           = hit.projSag();
    posX[index]              = hit.localPosition().x();
    posY[index]              = hit.localPosition().y();
    posZ[index]              = hit.localPosition().z();
    gPosX[index]             = hit.globalPosition().x();
    gPosY[index]             = hit.globalPosition().y();
    gPosZ[index]             = hit.globalPosition().z();
    closesApproachX [index]  = hit.localPointOfClosestApproach().x();
    closesApproachY [index]  = hit.localPointOfClosestApproach().y();
    closesApproachZ [index]  = hit.localPointOfClosestApproach().z();
    gClosesApproachX [index] = hit.globalPointOfClosestApproach().x();
    gClosesApproachY [index] = hit.globalPointOfClosestApproach().y();
    gClosesApproachZ [index] = hit.globalPointOfClosestApproach().z();
    tube_t0[index]           = hit.tubeT0();
    tube_adccal[index]       = hit.tubeAdcCal();
    temperature_time[index]  = hit.TemperatureTime();
    wire_sag_time[index]     = hit.WiresagTime();
    xtwin[index]             = hit.xtwin();
    segment_t0_applied[index]= hit.segmentT0Applied();
    // increment hit index
    ++index;
  
    return true;
  }  //end MdtCalibHitNtupleBranch::fillBranch

  bool MdtCalibHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //std::cout << "MdtCalibHitNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
	//		<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nMdt";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "segIndex",         &segIndex,          array_size + "/I" );
    branchCreator.createBranch( tree, "id",               &id,                array_size + "/I" );
    branchCreator.createBranch( tree, "tdc",              &tdc,               array_size + "/I" );
    branchCreator.createBranch( tree, "adc",              &adc,               array_size + "/I" );
    branchCreator.createBranch( tree, "t",                &t,                 array_size + "/F" );
    branchCreator.createBranch( tree, "r",                &r,                 array_size + "/F" );
    branchCreator.createBranch( tree, "dr",               &dr,                array_size + "/F" );
    branchCreator.createBranch( tree, "rTrk",             &rTrk,              array_size + "/F" );
    branchCreator.createBranch( tree, "drTrk",            &drTrk,             array_size + "/F" );
    branchCreator.createBranch( tree, "resi",             &resi,              array_size + "/F" );
    branchCreator.createBranch( tree, "distRO",           &distRO,            array_size + "/F" );
    branchCreator.createBranch( tree, "slewTime",         &slewTime,          array_size + "/F" );
    branchCreator.createBranch( tree, "lorTime",          &lorTime,           array_size + "/F" );
    branchCreator.createBranch( tree, "propTime",         &propTime,          array_size + "/F" );
    branchCreator.createBranch( tree, "tof",              &tof,               array_size + "/F" );
    branchCreator.createBranch( tree, "bFieldPerp",       &bFieldPerp,        array_size + "/F" );
    branchCreator.createBranch( tree, "bFieldPara",       &bFieldPara,        array_size + "/F" );
    branchCreator.createBranch( tree, "temperature",      &temperature,       array_size + "/F" );
    branchCreator.createBranch( tree, "projSag",          &projSag,           array_size + "/F" );
    branchCreator.createBranch( tree, "posX",             &posX,              array_size + "/F" );
    branchCreator.createBranch( tree, "posY",             &posY,              array_size + "/F" );
    branchCreator.createBranch( tree, "posZ",             &posZ,              array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",            &gPosX,             array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",            &gPosY,             array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",            &gPosZ,             array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachX",  &closesApproachX,   array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachY",  &closesApproachY,   array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachZ",  &closesApproachZ,   array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachX", &gClosesApproachX,  array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachY", &gClosesApproachY,  array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachZ", &gClosesApproachZ,  array_size + "/F" );
    branchCreator.createBranch( tree, "temperature_time", &temperature_time,  array_size + "/F" );
    branchCreator.createBranch( tree, "wire_sag_time",    &wire_sag_time,     array_size + "/F" );
    branchCreator.createBranch( tree, "tube_t0",          &tube_t0,           array_size + "/F" );
    branchCreator.createBranch( tree, "tube_adccal",      &tube_adccal,       array_size + "/F" );
    branchCreator.createBranch( tree, "xtwin",            &xtwin,             array_size + "/F" );
    branchCreator.createBranch( tree, "segmentT0Applied", &segment_t0_applied,array_size + "/O" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end MdtCalibHitNtupleBranch::createBranch

}   //namespace MuonCalib
