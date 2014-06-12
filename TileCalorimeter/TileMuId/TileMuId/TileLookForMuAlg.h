/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//****************************************************************************
// Filename : TileLookForMuAlg.h
// Author   : G Usai 
// Created  : Jun 2003
//
// DESCRIPTION
// 
//  
// Properties (JobOption Parameters):
//
//
// BUGS:
//  
// History:
//      
//   
//****************************************************************************
#ifndef  TILEMUID_TILELOOKFORMUALG_H
#define  TILEMUID_TILELOOKFORMUALG_H 

// includes 
#include "AthenaBaseComps/AthAlgorithm.h"

// C++ STL includes
#include  <vector> 
#include <string>

//Forward declaration
class TileID;

class TileLookForMuAlg: public AthAlgorithm {
  public:
    // constructor 
    TileLookForMuAlg(const std::string& name, ISvcLocator* pSvcLocator);
    // destructor 
    ~TileLookForMuAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    //static std::string TileLookForMuAlg_stream;

  private:

    const TileID* m_tileID;

    // define a  numeration  scheme for the  cells
    enum N_CELLS_SAMP {
      N_CELLS_D = 13, N_CELLS_BC = 30, N_CELLS_A = 30
    };
    enum N_OF_MODULES {
      N_MODULES = 64
    };
    double etaD[N_CELLS_D];
    double etaBC[N_CELLS_BC];
    double etaA[N_CELLS_A];
    const int nMuMax;
    // and patterns search from D to BC and  from BC to A layers   
    std::vector<int> fromDtoBC;
    std::vector<int> fromBCtoA;
    //   std::map<double,unsigned int> eta2map
    // lower thresholds for the 3 sample
    double loThrA;
    double loThrBC;
    double loThrD;
    double loThrITC;
    // std::vector<double>  hiThrD; 
    std::vector<double> hiThrD;
    std::vector<double> hiThrBC;
    std::vector<double> hiThrA;
    std::string m_cellContainer; // Cell container names 
    std::string m_tileTagContainer;
//std::string m_CellCollName;

};
#endif // TILEMUID_TILELOOKFORMUALG_H
