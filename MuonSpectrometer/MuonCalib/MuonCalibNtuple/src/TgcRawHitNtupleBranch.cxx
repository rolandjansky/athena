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

  TgcRawHitNtupleBranch::TgcRawHitNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false)
   , index(-1), index_Curr(-1), index_Prev(-1), index_Next(-1)
  {}

  bool TgcRawHitNtupleBranch::fillBranch(const MuonCalibRawTgcHit &hit) {
    // check if branches where initialized
    if( !branchesInit )
      return false;    
    
    // check if index not out of range 
    if( index >= m_blockSize || index < 0 )
      return false;

    //    Trk::GlobalPosition global = hit.globalP
    num_Prev[index] = index_Prev;
    num_Curr[index] = index_Curr;
    num_Next[index] = index_Next;
 
    // copy values 
    occupancy[index]  = hit.occupancy();
    id[index]         = hit.identify().getIdInt();
    station[index]    = hit.station();
    eta[index]        = hit.eta();
    phi[index]        = hit.phi();
    gasGap[index]     = hit.gasGap();
    isStrip[index]    = hit.isStrip();
    channel[index]    = hit.channel();
    shortWidth[index] = hit.shortWidth();
    longWidth[index]  = hit.longWidth();
    width[index]      = hit.width() ;
    length[index]     = hit.length() ;
    gPosX[index]      = hit.globalPosition().x();
    gPosY[index]      = hit.globalPosition().y();
    gPosZ[index]      = hit.globalPosition().z();
    bcTag[index] = hit.bcTag();
    ++index;

    if(     hit.bcTag()==1) ++index_Prev;
    else if(hit.bcTag()==2) ++index_Curr;
    else if(hit.bcTag()==3) ++index_Next;

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
    branchCreator.createBranch( tree, index_name,      &index_Curr, "/I");
    branchCreator.createBranch( tree, index_name_Prev, &index_Prev, "/I");
    branchCreator.createBranch( tree, index_name_Next, &index_Next, "/I");
    branchCreator.createBranch( tree, index_name_All,  &index, "/I");

    // all entries of same size, the number of hits in the event
    std::string array_size( std::string("[") + m_branchName + index_name_All + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "bcTag",      &bcTag,      array_size + "/I" );
    branchCreator.createBranch( tree, "num_Prev",   &num_Prev,   array_size + "/I" );
    branchCreator.createBranch( tree, "num_Curr",   &num_Curr,   array_size + "/I" );
    branchCreator.createBranch( tree, "num_Next",   &num_Next,   array_size + "/I" );

    branchCreator.createBranch( tree, "occupancy",  &occupancy,  array_size + "/I" );
    branchCreator.createBranch( tree, "id",         &id,         array_size + "/I" );
    branchCreator.createBranch( tree, "station",    &station,    array_size + "/I" );
    branchCreator.createBranch( tree, "eta",        &eta,        array_size + "/I" );
    branchCreator.createBranch( tree, "phi",        &phi,        array_size + "/I" );
    branchCreator.createBranch( tree, "gasGap",     &gasGap,     array_size + "/I" );
    branchCreator.createBranch( tree, "isStrip",    &isStrip,    array_size + "/I" );
    branchCreator.createBranch( tree, "channel",    &channel,    array_size + "/I" );
    branchCreator.createBranch( tree, "shortWidth", &shortWidth, array_size + "/F" );
    branchCreator.createBranch( tree, "longWidth",  &longWidth,  array_size + "/F" );
    branchCreator.createBranch( tree, "width",      &width,      array_size + "/F" );
    branchCreator.createBranch( tree, "length",     &length,     array_size + "/F" );
    branchCreator.createBranch( tree, "gPosX",      &gPosX,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosY",      &gPosY,      array_size + "/F" );
    branchCreator.createBranch( tree, "gPosZ",      &gPosZ,      array_size + "/F" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end TgcRawHitNtupleBranch::createBranch

}  //namespace MuonCalib
