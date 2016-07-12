/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonPatternNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibNtuple/NtupleSafeCast.h"

#include "MuonCalibEventBase/MuonCalibPattern.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MuonPatternNtupleBranch::MuonPatternNtupleBranch(std::string branchName) : m_branchName(branchName), 
    branchesInit(false), m_first(true), index(0) {}

  bool MuonPatternNtupleBranch::fillBranch(const MuonCalibPattern &pat, const PatInfo& /* info (not used) */) {
    // check if branches were initialized
    if( !branchesInit ){
      //     std::cout << "MuonPatternNtupleBranch::fillBranch "
      //	<< " ERROR <branches where not initialized>"
      //	<<  std::endl;
      return false;    
    }

    // check if index is within range 
    if( index >= m_blockSize || index < 0 ){
      if (m_first == true) {
	//std::cout << "MuonPatternNtupleBranch::fillBranch  " 
	//  << " ERROR <index out of range, pat not added to ntuple> "
	//		  <<  index << std::endl;
	m_first = false;
      }
      return false;
    }
        
    // copy values 
    chi2[index]    = NtupleSafeCast(pat.chi2());
    z0[index]      = NtupleSafeCast(pat.z0());
    r0[index]      = NtupleSafeCast(pat.r0());
    invP[index]    = NtupleSafeCast(pat.invP());
    phi[index]     = NtupleSafeCast(pat.phi());
    theta[index]   = NtupleSafeCast(pat.theta());
    nSegments[index] = pat.muonSegments();
    nmdt[index] = pat.nmdtHits();
    nrpc[index] = pat.nrpcHits();
    ncsc[index] = pat.ncscHits();
    ntgc[index] = pat.ntgcHits();

    // increment pat index
    ++index;

    return true;
  }  //end MuonPatternNtupleBranch::fillBranch

  bool MuonPatternNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      //   std::cout << "MuonPatternNtupleBranch::createBranch: <got invalid tree pointer> " 
      //	<< std::endl;
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nPatterns";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "chi2",  &chi2,  array_size + "/F" );
    branchCreator.createBranch( tree, "z0",    &z0,    array_size + "/F" );
    branchCreator.createBranch( tree, "r0",    &r0,    array_size + "/F" );
    branchCreator.createBranch( tree, "invP",  &invP,  array_size + "/F" );
    branchCreator.createBranch( tree, "phi",   &phi,   array_size + "/F" );
    branchCreator.createBranch( tree, "theta", &theta, array_size + "/F" );
    branchCreator.createBranch( tree, "nSegments", &nSegments, array_size + "/I" );
    branchCreator.createBranch( tree, "nmdt",  &nmdt,  array_size + "/I" );
    branchCreator.createBranch( tree, "nrpc",  &nrpc,  array_size + "/I" );
    branchCreator.createBranch( tree, "ncsc",  &ncsc,  array_size + "/I" );
    branchCreator.createBranch( tree, "ntgc",  &ntgc,  array_size + "/I" );

    branchesInit = true;

    // reset branch
    reset();
  
    return true;
  }  //end MuonPatternNtupleBranch::createBranch

}  //namespace MuonCalib
