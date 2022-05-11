/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXaltMetAlgo - Noise cut and Rho+RMS algorithm for gFEX MET
//                              -------------------
//     begin                : 31 03 2022
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include <cmath>
#include <vector>

#include "L1CaloFEXSim/gFEXaltMetAlgo.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gTower.h"

namespace LVL1 {

gFEXaltMetAlgo::gFEXaltMetAlgo(const std::string& type, const std::string& name, const IInterface* parent):
base_class(type, name, parent) 
{}

StatusCode gFEXaltMetAlgo::initialize(){

  return StatusCode::SUCCESS;

}


void gFEXaltMetAlgo::setAlgoConstant(std::vector<int>&& A_thr,
                                     std::vector<int>&& B_thr,
                                     const int rhoPlusThr) {
    m_etaThr[0] = std::move(A_thr);
    m_etaThr[1] = std::move(B_thr);
    m_rhoPlusThr = rhoPlusThr;
}



void gFEXaltMetAlgo::altMetAlgo(const gTowersCentral &Atwr, const gTowersCentral &Btwr,
                                std::array<uint32_t, 4> & outTOB) {

  //FPGA A observables
  int A_MET_x_nc = 0x0;
  int A_MET_y_nc = 0x0;
  int A_MET_x_rms = 0x0;
  int A_MET_y_rms = 0x0;

  int A_sumEt_nc = 0x0;
  int A_sumEt_rms = 0x0;

  //FPGA B observables
  int B_MET_x_nc = 0x0;
  int B_MET_y_nc = 0x0;
  int B_MET_x_rms = 0x0;
  int B_MET_y_rms = 0x0;

  int B_sumEt_nc = 0x0;
  int B_sumEt_rms = 0x0;

  //Global observables
  int MET_x_nc = 0x0;
  int MET_y_nc = 0x0;
  int MET_nc = 0x0;
  int MET_x_rms = 0x0;
  int MET_y_rms = 0x0;
  int MET_rms = 0x0;


  int total_sumEt_nc = 0x0;
  int total_sumEt_rms = 0x0;

  metFPGA(Atwr, A_MET_x_nc, A_MET_y_nc, 0);
  metFPGA(Btwr, B_MET_x_nc, B_MET_y_nc, 1);

  metTotal(A_MET_x_nc, A_MET_y_nc, B_MET_x_nc, B_MET_y_nc, MET_x_nc, MET_y_nc, MET_nc);

  int A_rho{get_rho(Atwr)};
  int B_rho{get_rho(Btwr)};
  int A_sigma{3*get_sigma(Atwr, A_rho)};
  int B_sigma{3*get_sigma(Btwr, B_rho)};

  rho_MET(Atwr, A_MET_x_rms, A_MET_y_rms, A_rho, A_sigma);
  rho_MET(Btwr, B_MET_x_rms, B_MET_y_rms, B_rho, B_sigma);

  metTotal(A_MET_x_rms, A_MET_y_rms, B_MET_x_rms, B_MET_y_rms, MET_x_rms, MET_y_rms, MET_rms);

  A_sumEt_nc = sumEtFPGAnc(Atwr, 0);
  B_sumEt_nc = sumEtFPGAnc(Btwr, 1);
  total_sumEt_nc = sumEt(A_sumEt_nc, B_sumEt_nc);
  total_sumEt_nc = total_sumEt_nc/4;

  A_sumEt_rms = sumEtFPGArms(Atwr, A_sigma);
  B_sumEt_rms = sumEtFPGArms(Btwr, B_sigma);
  total_sumEt_rms = sumEt(A_sumEt_rms, B_sumEt_rms);
  total_sumEt_rms = total_sumEt_rms/4;
  //Define a vector to be filled with all the TOBs of one event

    //TOB order
    //  1) MET_x | MET_y    <- ncMET
    //  2) MET_x | MET_y    <- rms
    //  3) MET | sumET      <- ncMET
    //  4) MET | sumET      <- rms

  // fill in TOBs
  // The order of the TOBs is given according to the TOB ID (TODO: check how it's done in fw)

  // First TOB is (MET, SumEt)
  outTOB[0] = (MET_y_nc&  0x00000FFF) << 0; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[0] = outTOB[0] | (MET_x_nc  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (MET_y_nc != 0) outTOB[0] = outTOB[0] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET_x_nc != 0) outTOB[0] = outTOB[0] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[0] = outTOB[0] | (2  &  0x0000001F) << 26;//TOB ID temporary set to 2 according to JwoJ convention (need updates in EDM)

// Second TOB is (MET_x, MET_y)
  outTOB[1] = (MET_y_rms&  0x00000FFF) << 0; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[1] = outTOB[1] | (MET_x_rms  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (MET_y_rms != 0) outTOB[1] = outTOB[1] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET_x_rms != 0) outTOB[1] = outTOB[1] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[1] = outTOB[1] | (2  &  0x0000001F) << 26;//TOB ID temporary set to 2 according to JwoJ convention (need updates in EDM)

// Third TOB is hard components (MHT_x, MHT_y)
  outTOB[2] = (total_sumEt_nc&  0x00000FFF) << 0; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[2] = outTOB[2] | (MET_nc  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (total_sumEt_nc != 0) outTOB[2] = outTOB[2] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET_nc != 0) outTOB[2] = outTOB[2] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[2] = outTOB[2] | (1  &  0x0000001F) << 26;//TOB ID temporary set to 1 according to JwoJ convention (need updates in EDM)

  // Fourth TOB is hard components (MST_x, MST_y)
  outTOB[3] = (total_sumEt_rms&  0x00000FFF) << 0; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[3] = outTOB[3] | (MET_rms  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (total_sumEt_rms != 0) outTOB[3] = outTOB[3] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET_rms != 0) outTOB[3] = outTOB[3] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[3] = outTOB[3] | (1  &  0x0000001F) << 26;//TOB ID temporary set to 1 according to JwoJ convention (need updates in EDM)


}



void gFEXaltMetAlgo::metFPGA(const gTowersCentral &twrs, int & MET_x, int & MET_y, const unsigned short FPGA_NO){

    int rows = twrs.size();
    int cols = twrs[0].size();
    for( int irow = 0; irow < rows; irow++ ){
        for(int jcolumn = 0; jcolumn<cols; jcolumn++){
        bool filter{twrs[irow][jcolumn] > m_etaThr[FPGA_NO][jcolumn]};
        MET_x += filter ? (twrs[irow][jcolumn])*cosLUT(irow, 5) : 0;
        MET_y += filter ? (twrs[irow][jcolumn])*sinLUT(irow, 5) : 0;

    }
  }
}


inline void gFEXaltMetAlgo::metTotal(const int A_MET_x, const int A_MET_y,
                                     const int B_MET_x, const int B_MET_y,
                                     int & MET_x, int & MET_y, int & MET){

  MET_x = A_MET_x + B_MET_x;
  MET_y = A_MET_y + B_MET_y;
  MET   = sqrt((MET_x * MET_x) + (MET_y * MET_y));

}

//Function to calculate rho for the given set of gtowers
int gFEXaltMetAlgo::get_rho(const gTowersCentral &twrs){
    const int rows = twrs.size();
    const int cols = twrs[0].size();
    const int n{rows*cols};
    float rho = 0;
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            rho += twrs[i][j] < m_rhoPlusThr ? twrs[i][j] : 0;
        }
    }
    return rho/n;
}

//Function calculates standard deviation of the gtowers
int gFEXaltMetAlgo::get_sigma(const gTowersCentral &twrs, const int rho) {

  int rows = twrs.size();
  int cols = twrs[0].size();
  const int n{rows*cols};
  int sigma = 0;
  for(int i = 0; i < rows; ++i) {
    for(int j = 0; j < cols; ++j) {
      const int diff{twrs[i][j]-rho};
      sigma += twrs[i][j] < m_rhoPlusThr ? diff*diff: 0;
    }  
  }    

  return static_cast<int>(std::sqrt(sigma * 1. / n));

}



void gFEXaltMetAlgo::rho_MET(const gTowersCentral &twrs, int & MET_x, int & MET_y, const int rho, const int sigma) {

    int rows = twrs.size();
    int cols = twrs[0].size();
    for( int irow = 0; irow < rows; irow++ ){
        for(int jcolumn = 0; jcolumn < cols; jcolumn++){
            const int ET_gTower_sub{(twrs[irow][jcolumn] - rho) & 0xFFF};
            const bool filter{ET_gTower_sub > sigma && !(ET_gTower_sub & 0x800)};
            MET_x += filter ? (ET_gTower_sub)*cosLUT(irow, 5) : 0;
            MET_y += filter ? (ET_gTower_sub)*sinLUT(irow, 5) : 0;

        }
    }
}

int gFEXaltMetAlgo::sumEtFPGAnc(const gTowersCentral &twrs, const unsigned short FPGA_NO){

    int partial_sumEt = 0;
    const int rows = twrs.size();
    const int cols = twrs[0].size();
    for( int irow = 0; irow < rows; irow++ ){
        for(int jcolumn = 0; jcolumn<cols; jcolumn++){
            partial_sumEt += twrs[irow][jcolumn] > m_etaThr[FPGA_NO][jcolumn] ? twrs[irow][jcolumn] : 0;
        }
    }
    return partial_sumEt;
}

int gFEXaltMetAlgo::sumEtFPGArms(const gTowersCentral &twrs, const int sigma){

    int partial_sumEt = 0;
    const int rows = twrs.size();
    const int cols = twrs[0].size();
    for(int i{0}; i < rows; ++i){
        for(int j{0}; j < cols; ++j) {
            partial_sumEt += twrs[i][j] > sigma ? twrs[i][j] : 0;
        }
    }
    return partial_sumEt;
}


int gFEXaltMetAlgo::sumEt(const int A_sumEt, const int B_sumEt){

  return A_sumEt + B_sumEt;

}


//----------------------------------------------------------------------------------
// bitwise simulation of sine LUT in firmware
//----------------------------------------------------------------------------------
float gFEXaltMetAlgo::sinLUT(const unsigned int phiIDX, const unsigned int aw)
{
  float c = ((float)phiIDX)/std::pow(2,aw);
  float rad = (2*M_PI) *c;
  float rsin = std::sin(rad);
  return rsin;
}


//----------------------------------------------------------------------------------
// bitwise simulation cosine LUT in firmware
//----------------------------------------------------------------------------------
float gFEXaltMetAlgo::cosLUT(const unsigned int phiIDX, const unsigned int aw)
{
  float c = ((float)phiIDX)/std::pow(2,aw);
  float rad = (2*M_PI) *c;
  float rcos = std::cos(rad);
  return rcos;
}




} // namespace LVL1

