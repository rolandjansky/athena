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

  TgcRawCoinNtupleBranch::TgcRawCoinNtupleBranch(std::string branchName) : m_branchName(branchName), m_branchesInit(false), 
    m_index(0), m_index_Curr(0), m_index_Prev(0), m_index_Next(0), m_index_tracklet(0), m_index_highpt(0), m_index_sl(0), 
    m_index_tracklet_Curr(0), m_index_tracklet_Prev(0), m_index_tracklet_Next(0), m_index_highpt_Curr(0), m_index_highpt_Prev(0), 
    m_index_highpt_Next(0), m_index_sl_Curr(0), m_index_sl_Prev(0), m_index_sl_Next(0)
  {}

  bool TgcRawCoinNtupleBranch::fillBranch(const MuonCalibRawTgcCoin& coin) {
    // check if branches were initialized
    if( !m_branchesInit )
      return false;    
    
    // check if index not out of range 
    if( m_index >= m_blockSize || m_index < 0 )
      return false;

    m_num_tracklet_Prev[m_index] = m_index_tracklet_Prev;
    m_num_tracklet_Curr[m_index] = m_index_tracklet_Curr;
    m_num_tracklet_Next[m_index] = m_index_tracklet_Next;
    m_num_highpt_Prev[m_index] = m_index_highpt_Prev;
    m_num_highpt_Curr[m_index] = m_index_highpt_Curr;
    m_num_highpt_Next[m_index] = m_index_highpt_Next;
    m_num_sl_Prev[m_index] = m_index_sl_Prev;
    m_num_sl_Curr[m_index] = m_index_sl_Curr;
    m_num_sl_Next[m_index] = m_index_sl_Next;
 
    // copy values 
    if(coin.type()==0){
      m_bcTag_tracklet[m_index_tracklet] = coin.bcTag();

      m_id_tracklet[m_index_tracklet] = coin.identify().getIdInt();
      m_gPosInX_tracklet[m_index_tracklet] = coin.gPosIn().x();
      m_gPosInY_tracklet[m_index_tracklet] = coin.gPosIn().y();
      m_gPosInZ_tracklet[m_index_tracklet] = coin.gPosIn().z();
      m_gPosOutX_tracklet[m_index_tracklet] = coin.gPosOut().x();
      m_gPosOutY_tracklet[m_index_tracklet] = coin.gPosOut().y();
      m_gPosOutZ_tracklet[m_index_tracklet] = coin.gPosOut().z();
      m_eta_tracklet[m_index_tracklet] = coin.eta();
      m_phi_tracklet[m_index_tracklet] = coin.phi();
      m_sector_tracklet[m_index_tracklet] = coin.sector();
      m_isForward_tracklet[m_index_tracklet] = coin.isForward();
      m_isStrip_tracklet[m_index_tracklet] = coin.isStrip();
      m_trackletId_tracklet[m_index_tracklet] = coin.trackletId();
      m_widthIn_tracklet[m_index_tracklet] = coin.widthIn();
      m_widthOut_tracklet[m_index_tracklet] = coin.widthOut();
      m_delta_tracklet[m_index_tracklet] = coin.delta();
      m_sub_tracklet[m_index_tracklet] = coin.sub();

      ++m_index_tracklet;
    }
    if(coin.type()==1){
      m_bcTag_highpt[m_index_highpt] = coin.bcTag();

      m_id_highpt[m_index_highpt] = coin.identify().getIdInt();
      m_gPosInX_highpt[m_index_highpt] = coin.gPosIn().x();
      m_gPosInY_highpt[m_index_highpt] = coin.gPosIn().y();
      m_gPosInZ_highpt[m_index_highpt] = coin.gPosIn().z();
      m_gPosOutX_highpt[m_index_highpt] = coin.gPosOut().x();
      m_gPosOutY_highpt[m_index_highpt] = coin.gPosOut().y();
      m_gPosOutZ_highpt[m_index_highpt] = coin.gPosOut().z();
      m_eta_highpt[m_index_highpt] = coin.eta();
      m_phi_highpt[m_index_highpt] = coin.phi();
      m_sector_highpt[m_index_highpt] = coin.sector();
      m_isForward_highpt[m_index_highpt] = coin.isForward();
      m_isStrip_highpt[m_index_highpt] = coin.isStrip();
      m_trackletId_highpt[m_index_highpt] = coin.trackletId();
      m_widthIn_highpt[m_index_highpt] = coin.widthIn();
      m_widthOut_highpt[m_index_highpt] = coin.widthOut();
      m_delta_highpt[m_index_highpt] = coin.delta();
      m_sub_highpt[m_index_highpt] = coin.sub();

      ++m_index_highpt;
    }
    if(coin.type()==2){
      m_bcTag_sl[m_index_sl] = coin.bcTag();

      m_id_sl[m_index_sl] = coin.identify().getIdInt();
      m_gPosOutX_sl[m_index_sl] = coin.gPosOut().x();
      m_gPosOutY_sl[m_index_sl] = coin.gPosOut().y();
      m_gPosOutZ_sl[m_index_sl] = coin.gPosOut().z();
      m_eta_sl[m_index_sl] = coin.eta();
      m_phi_sl[m_index_sl] = coin.phi();
      m_sector_sl[m_index_sl] = coin.sector();
      m_isForward_sl[m_index_sl] = coin.isForward();
      m_trackletId_sl[m_index_sl] = coin.trackletId();
      m_trackletIdStrip_sl[m_index_sl] = coin.trackletIdStrip();
      m_widthR_sl[m_index_sl] = coin.widthR();
      m_widthPhi_sl[m_index_sl] = coin.widthPhi();
      m_roi_sl[m_index_sl] = coin.roi();
      m_pt_sl[m_index_sl] = coin.pt();
      m_isPositiveDeltaR_sl[m_index_sl] = coin.isPositiveDeltaR();

      ++m_index_sl;
    }

    if(coin.bcTag()==1){
      m_type_Prev[m_index_Prev] = coin.type(); ++m_index_Prev;
      if(coin.type()==0){ m_type_bcTag[m_index] = 1; ++m_index_tracklet_Prev; }
      if(coin.type()==1){ m_type_bcTag[m_index] = 2; ++m_index_highpt_Prev;   }
      if(coin.type()==2){ m_type_bcTag[m_index] = 3; ++m_index_sl_Prev;       }
    }
    if(coin.bcTag()==2){
      m_type[m_index_Curr] = coin.type(); ++m_index_Curr;
      if(coin.type()==0){ m_type_bcTag[m_index] = 4; ++m_index_tracklet_Curr; }
      if(coin.type()==1){ m_type_bcTag[m_index] = 5; ++m_index_highpt_Curr;   }
      if(coin.type()==2){ m_type_bcTag[m_index] = 6; ++m_index_sl_Curr;       }
    }
    if(coin.bcTag()==3){
      m_type_Next[m_index_Next] = coin.type(); ++m_index_Next;
      if(coin.type()==0){ m_type_bcTag[m_index] = 7; ++m_index_tracklet_Next; }
      if(coin.type()==1){ m_type_bcTag[m_index] = 8; ++m_index_highpt_Next;   }
      if(coin.type()==2){ m_type_bcTag[m_index] = 9; ++m_index_sl_Next;       }
    }
    
    m_index++;
    
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
    branchCreator.createBranch( tree, index_name_Curr,          &m_index_Curr,          "/I");
    branchCreator.createBranch( tree, index_name_Prev,          &m_index_Prev,          "/I");
    branchCreator.createBranch( tree, index_name_Next,          &m_index_Next,          "/I");
    branchCreator.createBranch( tree, index_name_tracklet,      &m_index_tracklet,      "/I");
    branchCreator.createBranch( tree, index_name_highpt,        &m_index_highpt,        "/I");
    branchCreator.createBranch( tree, index_name_sl,            &m_index_sl,            "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Curr, &m_index_tracklet_Curr, "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Prev, &m_index_tracklet_Prev, "/I");
    branchCreator.createBranch( tree, index_name_tracklet_Next, &m_index_tracklet_Next, "/I");
    branchCreator.createBranch( tree, index_name_highpt_Curr,   &m_index_highpt_Curr,   "/I");
    branchCreator.createBranch( tree, index_name_highpt_Prev,   &m_index_highpt_Prev,   "/I");
    branchCreator.createBranch( tree, index_name_highpt_Next,   &m_index_highpt_Next,   "/I");
    branchCreator.createBranch( tree, index_name_sl_Curr,       &m_index_sl_Curr,       "/I");
    branchCreator.createBranch( tree, index_name_sl_Prev,       &m_index_sl_Prev,       "/I");
    branchCreator.createBranch( tree, index_name_sl_Next,       &m_index_sl_Next,       "/I");
    branchCreator.createBranch( tree, index_name_All,           &m_index,               "/I");

    // all entries of same size, the number of coins in the event
    std::string array_size_Curr( std::string("[") + m_branchName + index_name_Curr + std::string("]") );
    std::string array_size_Prev( std::string("[") + m_branchName + index_name_Prev + std::string("]") );
    std::string array_size_Next( std::string("[") + m_branchName + index_name_Next + std::string("]") );
    std::string array_size_tracklet( std::string("[") + m_branchName + index_name_tracklet + std::string("]") );
    std::string array_size_highpt( std::string("[") + m_branchName + index_name_highpt + std::string("]") );
    std::string array_size_sl( std::string("[") + m_branchName + index_name_sl + std::string("]") );
    std::string array_size_All( std::string("[") + m_branchName + index_name_All + std::string("]") );

    // create the branches
    branchCreator.createBranch( tree, "bcTag_tracklet",       &m_bcTag_tracklet,       array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "bcTag_highpt",         &m_bcTag_highpt,         array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "bcTag_sl",             &m_bcTag_sl,             array_size_sl + "/I" );
    branchCreator.createBranch( tree, "type",                 &m_type,                 array_size_Curr + "/I" );
    branchCreator.createBranch( tree, "type_Prev",            &m_type_Prev,            array_size_Prev + "/I" );
    branchCreator.createBranch( tree, "type_Next",            &m_type_Next,            array_size_Next + "/I" );
    branchCreator.createBranch( tree, "type_bcTag",           &m_type_bcTag,           array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Prev",    &m_num_tracklet_Prev,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Curr",    &m_num_tracklet_Curr,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_tracklet_Next",    &m_num_tracklet_Next,    array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Prev",      &m_num_highpt_Prev,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Curr",      &m_num_highpt_Curr,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_highpt_Next",      &m_num_highpt_Next,      array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Prev",          &m_num_sl_Prev,          array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Curr",          &m_num_sl_Curr,          array_size_All + "/I");
    branchCreator.createBranch( tree, "num_sl_Next",          &m_num_sl_Next,          array_size_All + "/I");

    branchCreator.createBranch( tree, "id_tracklet",          &m_id_tracklet,          array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "gPosInX_tracklet",     &m_gPosInX_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosInY_tracklet",     &m_gPosInY_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosInZ_tracklet",     &m_gPosInZ_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutX_tracklet",    &m_gPosOutX_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_tracklet",    &m_gPosOutY_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_tracklet",    &m_gPosOutZ_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "eta_tracklet",         &m_eta_tracklet,         array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "phi_tracklet",         &m_phi_tracklet,         array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "sector_tracklet",      &m_sector_tracklet,      array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "isForward_tracklet",   &m_isForward_tracklet,   array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "isStrip_tracklet",     &m_isStrip_tracklet,     array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "trackletId_tracklet",  &m_trackletId_tracklet,  array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "widthIn_tracklet",     &m_widthIn_tracklet,     array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "widthOut_tracklet",    &m_widthOut_tracklet,    array_size_tracklet + "/F" );
    branchCreator.createBranch( tree, "delta_tracklet",       &m_delta_tracklet,       array_size_tracklet + "/I" );
    branchCreator.createBranch( tree, "sub_tracklet",         &m_sub_tracklet,         array_size_tracklet + "/I" );

    branchCreator.createBranch( tree, "id_highpt",            &m_id_highpt,            array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "gPosInX_highpt",       &m_gPosInX_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosInY_highpt",       &m_gPosInY_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosInZ_highpt",       &m_gPosInZ_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutX_highpt",      &m_gPosOutX_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_highpt",      &m_gPosOutY_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_highpt",      &m_gPosOutZ_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "eta_highpt",           &m_eta_highpt,           array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "phi_highpt",           &m_phi_highpt,           array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "sector_highpt",        &m_sector_highpt,        array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "isForward_highpt",     &m_isForward_highpt,     array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "isStrip_highpt",       &m_isStrip_highpt,       array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "trackletId_highpt",    &m_trackletId_highpt,    array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "widthIn_highpt",       &m_widthIn_highpt,       array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "widthOut_highpt",      &m_widthOut_highpt,      array_size_highpt + "/F" );
    branchCreator.createBranch( tree, "delta_highpt",         &m_delta_highpt,         array_size_highpt + "/I" );
    branchCreator.createBranch( tree, "sub_highpt",           &m_sub_highpt,           array_size_highpt + "/I" );

    branchCreator.createBranch( tree, "id_sl",               &m_id_sl,               array_size_sl + "/I" );
    branchCreator.createBranch( tree, "gPosOutX_sl",         &m_gPosOutX_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "gPosOutY_sl",         &m_gPosOutY_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "gPosOutZ_sl",         &m_gPosOutZ_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "eta_sl",              &m_eta_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "phi_sl",              &m_phi_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "sector_sl",           &m_sector_sl,           array_size_sl + "/I" );
    branchCreator.createBranch( tree, "isForward_sl",        &m_isForward_sl,        array_size_sl + "/I" );
    branchCreator.createBranch( tree, "trackletId_sl",       &m_trackletId_sl,       array_size_sl + "/I" );
    branchCreator.createBranch( tree, "trackletIdStrip_sl",  &m_trackletIdStrip_sl,  array_size_sl + "/I" );
    branchCreator.createBranch( tree, "widthR_sl",           &m_widthR_sl,           array_size_sl + "/F" );
    branchCreator.createBranch( tree, "widthPhi_sl",         &m_widthPhi_sl,         array_size_sl + "/F" );
    branchCreator.createBranch( tree, "roi_sl",              &m_roi_sl,              array_size_sl + "/I" );
    branchCreator.createBranch( tree, "pt_sl",               &m_pt_sl,               array_size_sl + "/I" );
    branchCreator.createBranch( tree, "isPositiveDeltaR_sl", &m_isPositiveDeltaR_sl, array_size_sl + "/I" );

    m_branchesInit = true;
  
    // reset branch
    reset();

    return true;
  }  //end TgcRawCoinNtupleBranch::createBranch

}  //namespace MuonCalib
