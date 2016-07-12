/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCalibNtuple/TgcRawCoinNtupleBranch.h"
#include "MuonCalibNtuple/NtupleBranchCreator.h"
#include "MuonCalibEventBase/MuonCalibRawTgcCoin.h"
#include "MuonCalibIdentifier/MuonFixedId.h"

#include "TTree.h"

#include <iostream>

namespace MuonCalib {

  TgcRawCoinNtupleBranch::TgcRawCoinNtupleBranch(std::string branchName) : m_branchName(branchName), branchesInit(false), 
    index(0), index_Curr(0), index_Prev(0), index_Next(0), index_tracklet(0), index_highpt(0), index_sl(0), 
    index_tracklet_Curr(0), index_tracklet_Prev(0), index_tracklet_Next(0), index_highpt_Curr(0), index_highpt_Prev(0), 
    index_highpt_Next(0), index_sl_Curr(0), index_sl_Prev(0), index_sl_Next(0)
  {}

  bool TgcRawCoinNtupleBranch::fillBranch(const MuonCalibRawTgcCoin& coin) {
    // check if branches where initialized
    if( !branchesInit )
      return false;    
    
    // check if index not out of range 
    if( index >= m_blockSize || index < 0 )
      return false;

    num_tracklet_Prev[index] = index_tracklet_Prev;
    num_tracklet_Curr[index] = index_tracklet_Curr;
    num_tracklet_Next[index] = index_tracklet_Next;
    num_highpt_Prev[index] = index_highpt_Prev;
    num_highpt_Curr[index] = index_highpt_Curr;
    num_highpt_Next[index] = index_highpt_Next;
    num_sl_Prev[index] = index_sl_Prev;
    num_sl_Curr[index] = index_sl_Curr;
    num_sl_Next[index] = index_sl_Next;
 
    // copy values 
    if(coin.type()==0){
      bcTag_tracklet[index_tracklet] = coin.bcTag();

      id_tracklet[index_tracklet] = coin.identify().getIdInt();
      gPosInX_tracklet[index_tracklet] = coin.gPosIn().x();
      gPosInY_tracklet[index_tracklet] = coin.gPosIn().y();
      gPosInZ_tracklet[index_tracklet] = coin.gPosIn().z();
      gPosOutX_tracklet[index_tracklet] = coin.gPosOut().x();
      gPosOutY_tracklet[index_tracklet] = coin.gPosOut().y();
      gPosOutZ_tracklet[index_tracklet] = coin.gPosOut().z();
      eta_tracklet[index_tracklet] = coin.eta();
      phi_tracklet[index_tracklet] = coin.phi();
      sector_tracklet[index_tracklet] = coin.sector();
      isForward_tracklet[index_tracklet] = coin.isForward();
      isStrip_tracklet[index_tracklet] = coin.isStrip();
      trackletId_tracklet[index_tracklet] = coin.trackletId();
      widthIn_tracklet[index_tracklet] = coin.widthIn();
      widthOut_tracklet[index_tracklet] = coin.widthOut();
      delta_tracklet[index_tracklet] = coin.delta();
      sub_tracklet[index_tracklet] = coin.sub();

      ++index_tracklet;
    }
    if(coin.type()==1){
      bcTag_highpt[index_highpt] = coin.bcTag();

      id_highpt[index_highpt] = coin.identify().getIdInt();
      gPosInX_highpt[index_highpt] = coin.gPosIn().x();
      gPosInY_highpt[index_highpt] = coin.gPosIn().y();
      gPosInZ_highpt[index_highpt] = coin.gPosIn().z();
      gPosOutX_highpt[index_highpt] = coin.gPosOut().x();
      gPosOutY_highpt[index_highpt] = coin.gPosOut().y();
      gPosOutZ_highpt[index_highpt] = coin.gPosOut().z();
      eta_highpt[index_highpt] = coin.eta();
      phi_highpt[index_highpt] = coin.phi();
      sector_highpt[index_highpt] = coin.sector();
      isForward_highpt[index_highpt] = coin.isForward();
      isStrip_highpt[index_highpt] = coin.isStrip();
      trackletId_highpt[index_highpt] = coin.trackletId();
      widthIn_highpt[index_highpt] = coin.widthIn();
      widthOut_highpt[index_highpt] = coin.widthOut();
      delta_highpt[index_highpt] = coin.delta();
      sub_highpt[index_highpt] = coin.sub();

      ++index_highpt;
    }
    if(coin.type()==2){
      bcTag_sl[index_sl] = coin.bcTag();

      id_sl[index_sl] = coin.identify().getIdInt();
      gPosOutX_sl[index_sl] = coin.gPosOut().x();
      gPosOutY_sl[index_sl] = coin.gPosOut().y();
      gPosOutZ_sl[index_sl] = coin.gPosOut().z();
      eta_sl[index_sl] = coin.eta();
      phi_sl[index_sl] = coin.phi();
      sector_sl[index_sl] = coin.sector();
      isForward_sl[index_sl] = coin.isForward();
      trackletId_sl[index_sl] = coin.trackletId();
      trackletIdStrip_sl[index_sl] = coin.trackletIdStrip();
      widthR_sl[index_sl] = coin.widthR();
      widthPhi_sl[index_sl] = coin.widthPhi();
      roi_sl[index_sl] = coin.roi();
      pt_sl[index_sl] = coin.pt();
      isPositiveDeltaR_sl[index_sl] = coin.isPositiveDeltaR();

      ++index_sl;
    }

    if(coin.bcTag()==1){
      type_Prev[index_Prev] = coin.type(); ++index_Prev;
      if(coin.type()==0){ type_bcTag[index] = 1; ++index_tracklet_Prev; }
      if(coin.type()==1){ type_bcTag[index] = 2; ++index_highpt_Prev;   }
      if(coin.type()==2){ type_bcTag[index] = 3; ++index_sl_Prev;       }
    }
    if(coin.bcTag()==2){
      type[index_Curr] = coin.type(); ++index_Curr;
      if(coin.type()==0){ type_bcTag[index] = 4; ++index_tracklet_Curr; }
      if(coin.type()==1){ type_bcTag[index] = 5; ++index_highpt_Curr;   }
      if(coin.type()==2){ type_bcTag[index] = 6; ++index_sl_Curr;       }
    }
    if(coin.bcTag()==3){
      type_Next[index_Next] = coin.type(); ++index_Next;
      if(coin.type()==0){ type_bcTag[index] = 7; ++index_tracklet_Next; }
      if(coin.type()==1){ type_bcTag[index] = 8; ++index_highpt_Next;   }
      if(coin.type()==2){ type_bcTag[index] = 9; ++index_sl_Next;       }
    }
    
    index++;
    
    return true;
  }  //end TgcRawCoinNtupleBranch::fillBranch

  bool TgcRawCoinNtupleBranch::createBranch(TTree *tree) {
    // check if pointer is valid
    if( !tree )
      return false;    

    // helper class to create branches in trees
    NtupleBranchCreator branchCreator(m_branchName);

    std::string index_name_Curr ="nRTgc";
    std::string index_name_Prev ="nRTgc_Prev";
    std::string index_name_Next ="nRTgc_Next";
    std::string index_name_tracklet ="nRTgc_tracklet";
    std::string index_name_highpt ="nRTgc_highpt";
    std::string index_name_sl ="nRTgc_sl";
    std::string index_name_tracklet_Curr ="nRTgcTracklet";
    std::string index_name_tracklet_Prev ="nRTgcTracklet_Prev";
    std::string index_name_tracklet_Next ="nRTgcTracklet_Next";
    std::string index_name_highpt_Curr ="nRTgcHighPt";
    std::string index_name_highpt_Prev ="nRTgcHighPt_Prev";
    std::string index_name_highpt_Next ="nRTgcHighPt_Next";
    std::string index_name_sl_Curr ="nRTgcSl";
    std::string index_name_sl_Prev ="nRTgcSl_Prev";
    std::string index_name_sl_Next ="nRTgcSl_Next";
    std::string index_name_All ="nRTgc_All";

    // create a branch for every data member
    branchCreator.createBranch( tree, index_name_Curr,          &index_Curr,          "/I");
    branchCreator.createBranch( tree, index_name_Prev,          &index_Prev,          "/I");
    branchCreator.createBranch( tree, index_name_Next,          &index_Next,          "/I");
    branchCreator.createBranch( tree, index_name_tracklet,      &index_tracklet,      "/I");
    branchCreator.createBranch( tree, index_name_highpt,        &index_highpt,        "/I");
    branchCreator.createBranch( tree, index_name_sl,            &index_sl,            "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Curr, &index_tracklet_Curr, "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Prev, &index_tracklet_Prev, "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Next, &index_tracklet_Next, "/I");
    branchCreator.createBranch( tree, index_name_highpt_Curr,   &index_highpt_Curr,   "/I");
    branchCreator.createBranch( tree, index_name_highpt_Prev,   &index_highpt_Prev,   "/I");
    branchCreator.createBranch( tree, index_name_highpt_Next,   &index_highpt_Next,   "/I");
    branchCreator.createBranch( tree, index_name_sl_Curr,       &index_sl_Curr,       "/I");
    branchCreator.createBranch( tree, index_name_sl_Prev,       &index_sl_Prev,       "/I");
    branchCreator.createBranch( tree, index_name_sl_Next,       &index_sl_Next,       "/I");
    branchCreator.createBranch( tree, index_name_All,           &index,               "/I");

    // all entries of same size, the number of coins in the event
    std::string array_size_Curr( std::string("[") + m_branchName + index_name_Curr + std::string("]") );
    std::string array_size_Prev( std::string("[") + m_branchName + index_name_Prev + std::string("]") );
    std::string array_size_Next( std::string("[") + m_branchName + index_name_Next + std::string("]") );
    std::string array_size_tracklet( std::string("[") + m_branchName + index_name_tracklet + std::string("]") );
    std::string array_size_highpt( std::string("[") + m_branchName + index_name_highpt + std::string("]") );
    std::string array_size_sl( std::string("[") + m_branchName + index_name_sl + std::string("]") );
    std::string array_size_All( std::string("[") + m_branchName + index_name_All + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "bcTag_tracklet",       &bcTag_tracklet,       array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "bcTag_highpt",         &bcTag_highpt,         array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "bcTag_sl",             &bcTag_sl,             array_size_sl + "/I" );
    branchCreator.createBranch( tree, "type",                 &type,                 array_size_Curr + "/I" );
    branchCreator.createBranch( tree, "type_Prev",            &type_Prev,            array_size_Prev + "/I" );
    branchCreator.createBranch( tree, "type_Next",            &type_Next,            array_size_Next + "/I" );
    branchCreator.createBranch( tree, "type_bcTag",           &type_bcTag,           array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Prev",    &num_tracklet_Prev,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Curr",    &num_tracklet_Curr,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Next",    &num_tracklet_Next,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Prev",      &num_highpt_Prev,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Curr",      &num_highpt_Curr,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Next",      &num_highpt_Next,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Prev",          &num_sl_Prev,          array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Curr",          &num_sl_Curr,          array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Next",          &num_sl_Next,          array_size_All + "/I");

    branchCreator.createBranch( tree, "id_tracklet",          &id_tracklet,          array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "gPosInX_tracklet",     &gPosInX_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosInY_tracklet",     &gPosInY_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosInZ_tracklet",     &gPosInZ_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutX_tracklet",    &gPosOutX_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_tracklet",    &gPosOutY_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_tracklet",    &gPosOutZ_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "eta_tracklet",         &eta_tracklet,         array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "phi_tracklet",         &phi_tracklet,         array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "sector_tracklet",      &sector_tracklet,      array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "isForward_tracklet",   &isForward_tracklet,   array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "isStrip_tracklet",     &isStrip_tracklet,     array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "trackletId_tracklet",  &trackletId_tracklet,  array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "widthIn_tracklet",     &widthIn_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "widthOut_tracklet",    &widthOut_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "delta_tracklet",       &delta_tracklet,       array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "sub_tracklet",         &sub_tracklet,         array_size_tracklet + "/I" );

    branchCreator.createBranch( tree, "id_highpt",            &id_highpt,            array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "gPosInX_highpt",       &gPosInX_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosInY_highpt",       &gPosInY_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosInZ_highpt",       &gPosInZ_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutX_highpt",      &gPosOutX_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_highpt",      &gPosOutY_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_highpt",      &gPosOutZ_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "eta_highpt",           &eta_highpt,           array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "phi_highpt",           &phi_highpt,           array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "sector_highpt",        &sector_highpt,        array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "isForward_highpt",     &isForward_highpt,     array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "isStrip_highpt",       &isStrip_highpt,       array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "trackletId_highpt",    &trackletId_highpt,    array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "widthIn_highpt",       &widthIn_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "widthOut_highpt",      &widthOut_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "delta_highpt",         &delta_highpt,         array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "sub_highpt",           &sub_highpt,           array_size_highpt + "/I" );

    branchCreator.createBranch( tree, "id_sl",               &id_sl,               array_size_sl + "/I" );
    branchCreator.createBranch( tree, "gPosOutX_sl",         &gPosOutX_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_sl",         &gPosOutY_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_sl",         &gPosOutZ_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "eta_sl",              &eta_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "phi_sl",              &phi_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "sector_sl",           &sector_sl,           array_size_sl + "/I" );
    branchCreator.createBranch( tree, "isForward_sl",        &isForward_sl,        array_size_sl + "/I" );
    branchCreator.createBranch( tree, "trackletId_sl",       &trackletId_sl,       array_size_sl + "/I" );
    branchCreator.createBranch( tree, "trackletIdStrip_sl",  &trackletIdStrip_sl,  array_size_sl + "/I" );
    branchCreator.createBranch( tree, "widthR_sl",           &widthR_sl,           array_size_sl + "/F" );
    branchCreator.createBranch( tree, "widthPhi_sl",         &widthPhi_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "roi_sl",              &roi_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "pt_sl",               &pt_sl,               array_size_sl + "/I" );
    branchCreator.createBranch( tree, "isPositiveDeltaR_sl", &isPositiveDeltaR_sl, array_size_sl + "/I" );

    branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end TgcRawCoinNtupleBranch::createBranch

}  //namespace MuonCalib
