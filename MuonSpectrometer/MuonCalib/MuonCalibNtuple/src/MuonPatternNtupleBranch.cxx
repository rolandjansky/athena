/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/MuonPatternNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibNtuple/NtupleSafeCast.h"

#include "MuonCalibEventBase/MuonCalibPattern.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  MuonPatternNtupleBranch::MuonPatternNtupleBranch(std::string branchName) : m_branchName(branchName), 
    m_branchesInit(false), m_first(true), m_index(0) {}

  bool MuonPatternNtupleBranch::fillBranch(const MuonCalibPattern &pat, const PatInfo& /* info (not used) */) {
    // check if branches were initialized
    if( !m_branchesInit ){
      return false;    
    }

    // check if index is within range 
    if( m_index >= m_blockSize || m_index < 0 ){
      if (m_first == true) {
	m_first = false;
      }
      return false;
    }
        
    // copy values 
    m_chi2[m_index]    = NtupleSafeCast(pat.chi2());
    m_z0[m_index]      = NtupleSafeCast(pat.z0());
    m_r0[m_index]      = NtupleSafeCast(pat.r0());
    m_invP[m_index]    = NtupleSafeCast(pat.invP());
    m_phi[m_index]     = NtupleSafeCast(pat.phi());
    m_theta[m_index]   = NtupleSafeCast(pat.theta());
    m_nSegments[m_index] = pat.muonSegments();
    m_nmdt[m_index] = pat.nmdtHits();
    m_nrpc[m_index] = pat.nrpcHits();
    m_ncsc[m_index] = pat.ncscHits();
    m_ntgc[m_index] = pat.ntgcHits();

    // increment pat index
    ++m_index;

    return true;
  }  //end MuonPatternNtupleBranch::fillBranch

  bool MuonPatternNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree ){
      return false;
    }

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name = "nPatterns";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name, &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "chi2",  &m_chi2,  array_size + "/F" );
    branchCreator.createBranch( tree, "z0",    &m_z0,    array_size + "/F" );
    branchCreator.createBranch( tree, "r0",    &m_r0,    array_size + "/F" );
    branchCreator.createBranch( tree, "invP",  &m_invP,  array_size + "/F" );
    branchCreator.createBranch( tree, "phi",   &m_phi,   array_size + "/F" );
    branchCreator.createBranch( tree, "theta", &m_theta, array_size + "/F" );
    branchCreator.createBranch( tree, "nSegments", &m_nSegments, array_size + "/I" );
    branchCreator.createBranch( tree, "nmdt",  &m_nmdt,  array_size + "/I" );
    branchCreator.createBranch( tree, "nrpc",  &m_nrpc,  array_size + "/I" );
    branchCreator.createBranch( tree, "ncsc",  &m_ncsc,  array_size + "/I" );
    branchCreator.createBranch( tree, "ntgc",  &m_ntgc,  array_size + "/I" );

    m_branchesInit = true;

    // reset branch
    reset();
  
    return true;
  }  //end MuonPatternNtupleBranch::createBranch

}  //namespace MuonCalib
