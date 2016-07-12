/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  
  MuonSegmentNtupleBranch::MuonSegmentNtupleBranch(std::string branchName) : m_branchName(branchName),  branchesInit(false), 
    m_first(true), index(0)
  {}

  //  bool MuonSegmentNtupleBranch::fillBranch(const MuonCalibSegment &seg, const int patternIndex) {
  bool MuonSegmentNtupleBranch::fillBranch(const MuonCalibSegment &seg) {
    // check if branches where initialized
    if( !branchesInit ) {
      //std::cout << "MuonSegmentNtupleBranch::fillBranch  ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;
    }

    // check if segment index out of range (m_blockSize is size of segment data arrays) 
    if( index >= m_blockSize || index < 0 ) {
      if (m_first == true) {
 	//std::cout << "MuonSegmentNtupleBranch::fillBranch  ERROR <index out of range, seg not added to ntuple> "
	//  << index << std::endl;
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
//       std::cout << "NEW ERROR rotz " << rotation.xz() << std::endl;
      rotz = rotz > 0. ? 1. : -1.;
    }

    theta = asin( -rotz );

    if (rotation(0,1)*rotation(0,1)+rotation(0,0)*rotation(0,0) > 0.0001) {
     psi = atan2( rotation(1,2) , rotation(2,2) );
     phi = atan2( rotation(0,1) , rotation(0,0) );
    } else {
     phi = 0.;
     psi = atan2( rotation(1,0)/sin(theta) , rotation(1,1) );
//     std::cout << " Approximate matrix " << std::endl;
    } 

    //Maybe this is old code for the translations 
    // double sinPhi   = sin( phi   ), cosPhi   = cos( phi   );
    // double sinTheta = sin( theta ), cosTheta = cos( theta );
    // double sinPsi   = sin( psi   ), cosPsi   = cos( psi   );
    
    // double rxx = cosTheta * cosPhi;  
    // double rxy = cosTheta * sinPhi;
    // double rxz = -sinTheta;   
      
    // double ryx = sinPsi*sinTheta*cosPhi - cosPsi*sinPhi;
    // double ryy = sinPsi*sinTheta*sinPhi + cosPsi*cosPhi;
    // double ryz = cosTheta*sinPsi;
      
    // double rzx = cosPsi*sinTheta*cosPhi + sinPsi*sinPhi;
    // double rzy = cosPsi*sinTheta*sinPhi - sinPsi*cosPhi;
    // double rzz = cosTheta*cosPsi;
    
    // Amg::Vector3D colx( rxx ,ryx, rzx );
    // Amg::Vector3D coly( rxy, ryy, rzy );
    // Amg::Vector3D colz( rxz, ryz, rzz );
    
    // CLHEP::HepRotation rotation_test( colx, coly, colz );
    
    if( psi < 0 ) psi += 2*CLHEP::pi;
    if( phi < 0 ) phi += 2*CLHEP::pi;

    // copy values 
    //    patIndex[index]   = patternIndex;
    author[index]     = seg.author();
    quality[index]    = seg.qualityFlag();
    chi2[index]       = seg.chi2();
    fittedT0[index]   = seg.hasFittedT0() ? seg.fittedT0() : -99999.;
    gPosX[index]      = seg.globalPosition().x();
    gPosY[index]      = seg.globalPosition().y();
    gPosZ[index]      = seg.globalPosition().z();
    gDirX[index]      = seg.globalDirection().x();
    gDirY[index]      = seg.globalDirection().y();
    gDirZ[index]      = seg.globalDirection().z();
    posX[index]       = seg.position().x();
    posY[index]       = seg.position().y();
    posZ[index]       = seg.position().z();
    dirX[index]       = seg.direction().x();
    dirY[index]       = seg.direction().y();
    dirZ[index]       = seg.direction().z();
    nHits[index]      = seg.hitsOnTrack();
    nMdtHits[index]   = seg.mdtHitsOnTrack();
    nRpcHits[index]   = seg.rpcHitsOnTrack();
    nCscHits[index]   = seg.cscHitsOnTrack();
    nTgcHits[index]   = seg.tgcHitsOnTrack();
    transPhi[index]   = phi;
    transTheta[index] = theta;
    transPsi[index]   = psi;
    transX[index]     = translation.x();
    transY[index]     = translation.y();
    transZ[index]     = translation.z();

    // increment segment index
    ++index;
  
    return true;
  }  //end MuonSegmentNtupleBranch::fillBranch

  bool MuonSegmentNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ) {
      //   std::cout << "MuonSegmentNtupleBranch::createBranch  ERROR <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nSegments";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    //    branchCreator.createBranch( tree, "patIndex",   &patIndex,   array_size + "/I" );
    branchCreator.createBranch( tree, "author",     &author,     array_size + "/I" );
    branchCreator.createBranch( tree, "quality",    &quality,    array_size + "/I" );
    branchCreator.createBranch( tree, "chi2",       &chi2,       array_size + "/F" );
    branchCreator.createBranch( tree, "fittedT0",   &fittedT0,   array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",      &gPosX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",      &gPosY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",      &gPosZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirX",      &gDirX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirY",      &gDirY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gDirZ",      &gDirZ,      array_size + "/F" );
    branchCreator.createBranch( tree, "posX",       &posX,       array_size + "/F" );
    branchCreator.createBranch( tree, "posY",       &posY,       array_size + "/F" );
    branchCreator.createBranch( tree, "posZ",       &posZ,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirX",       &dirX,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirY",       &dirY,       array_size + "/F" );
    branchCreator.createBranch( tree, "dirZ",       &dirZ,       array_size + "/F" );
    branchCreator.createBranch( tree, "nHits",      &nHits,      array_size + "/I" );
    branchCreator.createBranch( tree, "nMdtHits",   &nMdtHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nRpcHits",   &nRpcHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nTgcHits",   &nTgcHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "nCscHits",   &nCscHits,   array_size + "/I" );
    branchCreator.createBranch( tree, "transPhi",   &transPhi,   array_size + "/D" );
    branchCreator.createBranch( tree, "transTheta", &transTheta, array_size + "/D" );
    branchCreator.createBranch( tree, "transPsi",   &transPsi,   array_size + "/D" );
    branchCreator.createBranch( tree, "transX",     &transX,     array_size + "/D" );
    branchCreator.createBranch( tree, "transY",     &transY,     array_size + "/D" );
    branchCreator.createBranch( tree, "transZ",     &transZ,     array_size + "/D" );

    branchesInit = true;

    // reset branch
    reset();

    return true;
  }  //end MuonSegmentNtupleBranch::createBranch

}  //namespace MuonCalib
