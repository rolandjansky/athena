/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileDigitsToNtuple.h
// Author   : 
// Created  : 
//
// DESCRIPTION
// 
//    To create Digits Ntuple file from TileHitContainer
//
// Properties (JobOption Parameters):
//
//    TileDigitsContainer         string   key value of Digits in TDS 
//    NtupleLoc                   string   pathname of ntuple file
//    NtupleID                    string   ID of ntuple
//
// BUGS:
//  
// History:
//  
//  
//****************************************************************************
#ifndef TILEDIGITSTONTUPLE_H
#define TILEDIGITSTONTUPLE_H

#include "GaudiKernel/NTuple.h"
#include "AthenaBaseComps/AthAlgorithm.h"

class TileID;
class TileHWID;
class TileTBID;

#include <string>

class TileDigitsToNtuple : public AthAlgorithm {
 public:
  //Constructor
  TileDigitsToNtuple(const std::string name, ISvcLocator* pSvcLocator);
  
  //Destructor 
  virtual ~TileDigitsToNtuple();                         
  
  //Gaudi Hooks
  StatusCode initialize();    
  StatusCode execute();
  StatusCode finalize();
  
 private:

  const TileID* m_tileID;
  const TileHWID* m_tileHWID;
  const TileTBID* m_tileTBID;

  NTuple::Tuple* m_ntuplePtr;
  std::string m_ntupleID;
  std::string m_ntupleLoc;
  bool m_commitNtuple;

  
  NTuple::Item<short> m_nChannel;

  NTuple::Array<short> m_ros;
  NTuple::Array<short> m_drawer;
  NTuple::Array<short> m_channel;
  NTuple::Array<short> m_gain;

  NTuple::Array<short> m_section;
  NTuple::Array<short> m_side;
  NTuple::Array<short> m_phi;
  NTuple::Array<short> m_eta;
  NTuple::Array<short> m_sample;
  NTuple::Array<short> m_pmt;
  NTuple::Array<short> m_adc;

  NTuple::Matrix<short> m_samples;
  
  std::string m_digitsContainer;
  std::string m_infoName;
  int m_saveMaxChannels;
  int m_nSamples;
  bool m_saveAll;
  bool m_saveE4prAndMBTS;
};

#endif
