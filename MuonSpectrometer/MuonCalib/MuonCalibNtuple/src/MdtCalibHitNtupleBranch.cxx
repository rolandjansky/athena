/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MdtCalibHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MdtCalibHitBase.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MdtCalibHitNtupleBranch::MdtCalibHitNtupleBranch(std::string branchName) : 
    m_branchName(branchName), m_branchesInit(false), m_index(0) 
  {}

  bool MdtCalibHitNtupleBranch::fillBranch(const MdtCalibHitBase &hit, const int segmentIndex) {
    // check if branches where initialized
    if( !m_branchesInit ) {
      return false;    
    }

    // check if index is out of range 
    if( m_index >= m_blockSize || m_index < 0 ){
      return false;
    }

    // copy values 
    m_segIndex[m_index]          = segmentIndex;
    m_id[m_index]                = (hit.identify()).getIdInt();
    m_tdc[m_index]               = hit.tdcCount();
    m_adc[m_index]               = hit.adcCount();
    m_t[m_index]                 = hit.driftTime();
    m_r[m_index]                 = hit.driftRadius();  
    m_dr[m_index]                = hit.sigmaDriftRadius(); 
    m_rTrk[m_index]              = hit.signedDistanceToTrack();
    m_drTrk[m_index]             = hit.sigmaDistanceToTrack();
    m_resi[m_index]              = hit.radialResidual();
    m_distRO[m_index]            = hit.distanceToReadout();
    m_slewTime[m_index]          = hit.slewingTime();
    m_lorTime[m_index]           = hit.bFieldTime();
    m_propTime[m_index]          = hit.propagationTime();
    m_tof[m_index]               = hit.timeOfFlight();
    m_bFieldPerp[m_index]        = hit.bFieldPerp();
    m_bFieldPara[m_index]        = hit.bFieldPara();
    m_temperature[m_index]       = hit.temperature();
    m_projSag[m_index]           = hit.projSag();
    m_posX[m_index]              = hit.localPosition().x();
    m_posY[m_index]              = hit.localPosition().y();
    m_posZ[m_index]              = hit.localPosition().z();
    m_gPosX[m_index]             = hit.globalPosition().x();
    m_gPosY[m_index]             = hit.globalPosition().y();
    m_gPosZ[m_index]             = hit.globalPosition().z();
    m_closesApproachX [m_index]  = hit.localPointOfClosestApproach().x();
    m_closesApproachY [m_index]  = hit.localPointOfClosestApproach().y();
    m_closesApproachZ [m_index]  = hit.localPointOfClosestApproach().z();
    m_gClosesApproachX [m_index] = hit.globalPointOfClosestApproach().x();
    m_gClosesApproachY [m_index] = hit.globalPointOfClosestApproach().y();
    m_gClosesApproachZ [m_index] = hit.globalPointOfClosestApproach().z();
    m_tube_t0[m_index]           = hit.tubeT0();
    m_tube_adccal[m_index]       = hit.tubeAdcCal();
    m_temperature_time[m_index]  = hit.TemperatureTime();
    m_wire_sag_time[m_index]     = hit.WiresagTime();
    m_xtwin[m_index]             = hit.xtwin();
    m_segment_t0_applied[m_index]= hit.segmentT0Applied();
    // increment hit index
    ++m_index;
  
    return true;
  }  //end MdtCalibHitNtupleBranch::fillBranch

  bool MdtCalibHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name ="nMdt";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "segIndex",         &m_segIndex,          array_size + "/I" );
    branchCreator.createBranch( tree, "id",               &m_id,                array_size + "/I" );
    branchCreator.createBranch( tree, "tdc",              &m_tdc,               array_size + "/I" );
    branchCreator.createBranch( tree, "adc",              &m_adc,               array_size + "/I" );
    branchCreator.createBranch( tree, "t",                &m_t,                 array_size + "/F" );
    branchCreator.createBranch( tree, "r",                &m_r,                 array_size + "/F" );
    branchCreator.createBranch( tree, "dr",               &m_dr,                array_size + "/F" );
    branchCreator.createBranch( tree, "rTrk",             &m_rTrk,              array_size + "/F" );
    branchCreator.createBranch( tree, "drTrk",            &m_drTrk,             array_size + "/F" );
    branchCreator.createBranch( tree, "resi",             &m_resi,              array_size + "/F" );
    branchCreator.createBranch( tree, "distRO",           &m_distRO,            array_size + "/F" );
    branchCreator.createBranch( tree, "slewTime",         &m_slewTime,          array_size + "/F" );
    branchCreator.createBranch( tree, "lorTime",          &m_lorTime,           array_size + "/F" );
    branchCreator.createBranch( tree, "propTime",         &m_propTime,          array_size + "/F" );
    branchCreator.createBranch( tree, "tof",              &m_tof,               array_size + "/F" );
    branchCreator.createBranch( tree, "bFieldPerp",       &m_bFieldPerp,        array_size + "/F" );
    branchCreator.createBranch( tree, "bFieldPara",       &m_bFieldPara,        array_size + "/F" );
    branchCreator.createBranch( tree, "temperature",      &m_temperature,       array_size + "/F" );
    branchCreator.createBranch( tree, "projSag",          &m_projSag,           array_size + "/F" );
    branchCreator.createBranch( tree, "posX",             &m_posX,              array_size + "/F" );
    branchCreator.createBranch( tree, "posY",             &m_posY,              array_size + "/F" );
    branchCreator.createBranch( tree, "posZ",             &m_posZ,              array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",            &m_gPosX,             array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",            &m_gPosY,             array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",            &m_gPosZ,             array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachX",  &m_closesApproachX,   array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachY",  &m_closesApproachY,   array_size + "/F" );
    branchCreator.createBranch( tree, "closesApproachZ",  &m_closesApproachZ,   array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachX", &m_gClosesApproachX,  array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachY", &m_gClosesApproachY,  array_size + "/F" );
    branchCreator.createBranch( tree, "gClosesApproachZ", &m_gClosesApproachZ,  array_size + "/F" );
    branchCreator.createBranch( tree, "temperature_time", &m_temperature_time,  array_size + "/F" );
    branchCreator.createBranch( tree, "wire_sag_time",    &m_wire_sag_time,     array_size + "/F" );
    branchCreator.createBranch( tree, "tube_t0",          &m_tube_t0,           array_size + "/F" );
    branchCreator.createBranch( tree, "tube_adccal",      &m_tube_adccal,       array_size + "/F" );
    branchCreator.createBranch( tree, "xtwin",            &m_xtwin,             array_size + "/F" );
    branchCreator.createBranch( tree, "segmentT0Applied", &m_segment_t0_applied,array_size + "/O" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end MdtCalibHitNtupleBranch::createBranch

}   //namespace MuonCalib
