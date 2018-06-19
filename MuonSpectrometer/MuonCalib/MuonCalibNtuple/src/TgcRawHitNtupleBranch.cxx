/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/TgcRawHitNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawTgcHit.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  TgcRawHitNtupleBranch::TgcRawHitNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false)
   , m_index(-1), m_index_Curr(-1), m_index_Prev(-1), m_index_Next(-1)
  {}

  bool TgcRawHitNtupleBranch::fillBranch(const MuonCalibRawTgcHit &hit) {
    // check if branches were initialized
    if( !m_branchesInit )
      return false;    
    
    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 )
      return false;

    //    Trk::GlobalPosition global = hit.globalP
    m_num_Prev[m_index] = m_index_Prev;
    m_num_Curr[m_index] = m_index_Curr;
    m_num_Next[m_index] = m_index_Next;
 
    // copy values 
    m_occupancy[m_index]  = hit.occupancy();
    m_id[m_index]         = hit.identify().getIdInt();
    m_station[m_index]    = hit.station();
    m_eta[m_index]        = hit.eta();
    m_phi[m_index]        = hit.phi();
    m_gasGap[m_index]     = hit.gasGap();
    m_isStrip[m_index]    = hit.isStrip();
    m_channel[m_index]    = hit.channel();
    m_shortWidth[m_index] = hit.shortWidth();
    m_longWidth[m_index]  = hit.longWidth();
    m_width[m_index]      = hit.width() ;
    m_length[m_index]     = hit.length() ;
    m_gPosX[m_index]      = hit.globalPosition().x();
    m_gPosY[m_index]      = hit.globalPosition().y();
    m_gPosZ[m_index]      = hit.globalPosition().z();
    m_bcTag[m_index] = hit.bcTag();
    ++m_index;

    if(     hit.bcTag()==1) ++m_index_Prev;
    else if(hit.bcTag()==2) ++m_index_Curr;
    else if(hit.bcTag()==3) ++m_index_Next;

    return true;
  }  //end TgcRawHitNtupleBranch::fillBranch

  bool TgcRawHitNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree )
      return false;
    

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name      ="nRTgc";
    std::string index_name_Prev ="nRTgc_Prev";
    std::string index_name_Next ="nRTgc_Next";
    std::string index_name_All  ="nRTgc_All_Bunch";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name,      &m_index_Curr, "/I");
    branchCreator.createBranch( tree, index_name_Prev, &m_index_Prev, "/I");
    branchCreator.createBranch( tree, index_name_Next, &m_index_Next, "/I");
    branchCreator.createBranch( tree, index_name_All,  &m_index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name_All + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "bcTag",      &m_bcTag,      array_size + "/I" );
    branchCreator.createBranch( tree, "num_Prev",   &m_num_Prev,   array_size + "/I" );
    branchCreator.createBranch( tree, "num_Curr",   &m_num_Curr,   array_size + "/I" );
    branchCreator.createBranch( tree, "num_Next",   &m_num_Next,   array_size + "/I" );

    branchCreator.createBranch( tree, "occupancy",  &m_occupancy,  array_size + "/I" );
    branchCreator.createBranch( tree, "id",         &m_id,         array_size + "/I" );
    branchCreator.createBranch( tree, "station",    &m_station,    array_size + "/I" );
    branchCreator.createBranch( tree, "eta",        &m_eta,        array_size + "/I" );
    branchCreator.createBranch( tree, "phi",        &m_phi,        array_size + "/I" );
    branchCreator.createBranch( tree, "gasGap",     &m_gasGap,     array_size + "/I" );
    branchCreator.createBranch( tree, "isStrip",    &m_isStrip,    array_size + "/I" );
    branchCreator.createBranch( tree, "channel",    &m_channel,    array_size + "/I" );
    branchCreator.createBranch( tree, "shortWidth", &m_shortWidth, array_size + "/F" );
    branchCreator.createBranch( tree, "longWidth",  &m_longWidth,  array_size + "/F" );
    branchCreator.createBranch( tree, "width",      &m_width,      array_size + "/F" );
    branchCreator.createBranch( tree, "length",     &m_length,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",      &m_gPosX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",      &m_gPosY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",      &m_gPosZ,      array_size + "/F" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end TgcRawHitNtupleBranch::createBranch

}  //namespace MuonCalib
