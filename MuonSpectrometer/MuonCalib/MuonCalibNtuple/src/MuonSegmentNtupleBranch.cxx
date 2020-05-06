/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonSegmentNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"

#include "MuonCalibEventBase/MuonCalibSegment.h"

#include "TTree.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <iostream>

namespace MuonCalib {
  
  MuonSegmentNtupleBranch::MuonSegmentNtupleBranch(std::string branchName) : m_branchName(branchName),  m_branchesInit(false), 
    m_first(true), m_index(0)
  {}

  //  bool MuonSegmentNtupleBranch::fillBranch(const MuonCalibSegment &seg, const int patternIndex) {
  bool MuonSegmentNtupleBranch::fillBranch(const MuonCalibSegment &seg) {
    // check if branches were initialized
    if( !m_branchesInit ) {
      return false;
    }

    // check if segment index out of range (m_blockSize is size of segment data arrays) 
    if( m_index >= m_blockSize || m_index < 0 ) {
      if (m_first == true) {
	m_first = false;
      }
      return false;
    }
    
    //Extract the transformation from AmgTransform3D to store derive 6 translation parameters for ntuple
    //These are are the angular and spatial translations from local to global coordinates
    //What is the purpose of storing these translation parameters?
    Amg::Transform3D transform = seg.localToGlobal();
    Amg::RotationMatrix3D rotation = transform.rotation();
    Amg::Vector3D  translation = transform.translation();
        
    double theta, phi, psi;
    
    double rotz = rotation(0,2);
    if( std::abs( rotz ) > 1. ) {
      rotz = rotz > 0. ? 1. : -1.;
    }

    theta = std::asin( -rotz );

    if (rotation(0,1)*rotation(0,1)+rotation(0,0)*rotation(0,0) > 0.0001) {
     psi = std::atan2( rotation(1,2) , rotation(2,2) );
     phi = std::atan2( rotation(0,1) , rotation(0,0) );
    } else {
     phi = 0.;
     psi = std::atan2( rotation(1,0)/std::sin(theta) , rotation(1,1) );
    } 
    
    if( psi < 0 ) psi += 2*CLHEP::pi;
    if( phi < 0 ) phi += 2*CLHEP::pi;

    // copy values 
    m_author[m_index]     = seg.author();
    m_quality[m_index]    = seg.qualityFlag();
    m_chi2[m_index]       = seg.chi2();
    m_fittedT0[m_index]   = seg.hasFittedT0() ? seg.fittedT0() : -99999.;
    m_gPosX[m_index]      = seg.globalPosition().x();
    m_gPosY[m_index]      = seg.globalPosition().y();
    m_gPosZ[m_index]      = seg.globalPosition().z();
    m_gDirX[m_index]      = seg.globalDirection().x();
    m_gDirY[m_index]      = seg.globalDirection().y();
    m_gDirZ[m_index]      = seg.globalDirection().z();
    m_posX[m_index]       = seg.position().x();
    m_posY[m_index]       = seg.position().y();
    m_posZ[m_index]       = seg.position().z();
    m_dirX[m_index]       = seg.direction().x();
    m_dirY[m_index]       = seg.direction().y();
    m_dirZ[m_index]       = seg.direction().z();
    m_nHits[m_index]      = seg.hitsOnTrack();
    m_nMdtHits[m_index]   = seg.mdtHitsOnTrack();
    m_nRpcHits[m_index]   = seg.rpcHitsOnTrack();
    m_nCscHits[m_index]   = seg.cscHitsOnTrack();
    m_nTgcHits[m_index]   = seg.tgcHitsOnTrack();
    m_transPhi[m_index]   = phi;
    m_transTheta[m_index] = theta;
    m_transPsi[m_index]   = psi;
    m_transX[m_index]     = translation.x();
    m_transY[m_index]     = translation.y();
    m_transZ[m_index]     = translation.z();

    // increment segment index
    ++m_index;
  
    return true;
  }  //end MuonSegmentNtupleBranch::fillBranch

  bool MuonSegmentNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ) {
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nSegments";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    //    branchCreator.createBranch( tree, "patIndex",   &m_patIndex,   array_size + "/I" );
    branchCreator.createBranch( tree, "author",     &m_author,     array_size + "/I" );
    branchCreator.createBranch( tree, "quality",    &m_quality,    array_size + "/I" );
    branchCreator.createBranch( tree, "chi2",       &m_chi2,       array_size + "/F" );
    branchCreator.createBranch( tree, "fittedT0",   &m_fittedT0,   array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",      &m_gPosX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",      &m_gPosY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",      &m_gPosZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirX",      &m_gDirX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirY",      &m_gDirY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirZ",      &m_gDirZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "posX",       &m_posX,       array_size + "/F" );
    branchCreator.createBranch( tree, "posY",       &m_posY,       array_size + "/F" );
    branchCreator.createBranch( tree, "posZ",       &m_posZ,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirX",       &m_dirX,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirY",       &m_dirY,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirZ",       &m_dirZ,       array_size + "/F" );
    branchCreator.createBranch( tree, "nHits",      &m_nHits,      array_size + "/I" );
    branchCreator.createBranch( tree, "nMdtHits",   &m_nMdtHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nRpcHits",   &m_nRpcHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nTgcHits",   &m_nTgcHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nCscHits",   &m_nCscHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "transPhi",   &m_transPhi,   array_size + "/D" );
    branchCreator.createBranch( tree, "transTheta", &m_transTheta, array_size + "/D" );
    branchCreator.createBranch( tree, "transPsi",   &m_transPsi,   array_size + "/D" );
    branchCreator.createBranch( tree, "transX",     &m_transX,     array_size + "/D" );
    branchCreator.createBranch( tree, "transY",     &m_transY,     array_size + "/D" );
    branchCreator.createBranch( tree, "transZ",     &m_transZ,     array_size + "/D" );

    m_branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonSegmentNtupleBranch::createBranch

}  //namespace MuonCalib
