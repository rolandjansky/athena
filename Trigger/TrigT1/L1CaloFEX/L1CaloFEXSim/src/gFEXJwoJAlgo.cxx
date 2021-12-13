/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJwoJAlg - Jets without jets algorithm for gFEX
//                              -------------------
//     begin                : 10 08 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#include <cmath>
#include <vector>

#include "L1CaloFEXSim/gFEXJwoJAlgo.h"
#include "L1CaloFEXSim/gFEXJwoJTOB.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gTower.h"

namespace LVL1 {

  // default constructor for persistency
gFEXJwoJAlgo::gFEXJwoJAlgo(const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type, name, parent)
  {
    declareInterface<IgFEXJwoJAlgo>(this);
  }


StatusCode gFEXJwoJAlgo::initialize(){

  return StatusCode::SUCCESS;

}


void gFEXJwoJAlgo::setAlgoConstant(unsigned int aFPGA_A, unsigned int bFPGA_A,
                                   unsigned int aFPGA_B, unsigned int bFPGA_B,
                                   int gXE_seedThrA, int gXE_seedThrB) {
  m_aFPGA_A = aFPGA_A;
  m_bFPGA_A = bFPGA_A;
  m_aFPGA_B = aFPGA_B;
  m_bFPGA_B = bFPGA_B;
  m_gBlockthresholdA = gXE_seedThrA;
  m_gBlockthresholdB = gXE_seedThrB;

}



std::vector<std::unique_ptr<gFEXJwoJTOB>> gFEXJwoJAlgo::jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                                 std::array<uint32_t, 4> & outTOB) {


  // find gBlocks
  gTowersCentral gBLKA;
  gBlockAB(Atwr, gBLKA);


  gTowersCentral gBLKB;
  gBlockAB(Btwr, gBLKB);

  //FPGA A observables
  unsigned int A_MHT_x = 0x0;
  unsigned int A_MHT_y = 0x0;
  unsigned int A_MST_x = 0x0;
  unsigned int A_MST_y = 0x0;
  unsigned int A_MET_x = 0x0;
  unsigned int A_MET_y = 0x0;

  unsigned int A_sumEt = 0x0;

  //FPGA B observables
  unsigned int B_MHT_x = 0x0;
  unsigned int B_MHT_y = 0x0;
  unsigned int B_MST_x = 0x0;
  unsigned int B_MST_y = 0x0;
  unsigned int B_MET_x = 0x0;
  unsigned int B_MET_y = 0x0;

  unsigned int B_sumEt = 0x0;

  //Global observables
  unsigned int MET_x = 0x0;
  unsigned int MET_y = 0x0;
  unsigned int MET = 0x0;

  unsigned int total_sumEt = 0x0;
  unsigned int MHT_x = 0x0;
  unsigned int MHT_y = 0x0;
  unsigned int MST_x = 0x0;
  unsigned int MST_y = 0x0;


  metFPGA(Atwr, gBLKA, m_gBlockthresholdA, A_MHT_x, A_MHT_y, A_MST_x, A_MST_y, A_MET_x, A_MET_y);
  metFPGA(Btwr, gBLKB, m_gBlockthresholdB, B_MHT_x, B_MHT_y, B_MST_x, B_MST_y, B_MET_x, B_MET_y);

  metTotal(A_MET_x, A_MET_y, B_MET_x, B_MET_y, MET_x, MET_y, MET);

  sumEtFPGA(Atwr, A_sumEt);
  sumEtFPGA(Btwr, B_sumEt);

  sumEt(A_sumEt, B_sumEt, total_sumEt);
  sumEt(A_MHT_x, B_MHT_x, MHT_x);
  sumEt(A_MHT_y, B_MHT_y, MHT_y);
  sumEt(A_MST_x, B_MST_x, MST_x);
  sumEt(A_MST_y, B_MST_y, MST_y);

  //Define a vector to be filled with all the TOBs of one event
  std::vector<std::unique_ptr<gFEXJwoJTOB>> tobs_v;
  tobs_v.resize(4);


  // fill in TOBs
  // The order of the TOBs is given according to the TOB ID (TODO: check how it's done in fw)

  // First TOB is (MET, SumEt)
  outTOB[0] = total_sumEt; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[0] = outTOB[0] | (MET  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (total_sumEt != 0) outTOB[0] = outTOB[0] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET != 0) outTOB[0] = outTOB[0] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[0] = outTOB[0] | (1  &  0x0000001F) << 26;//TOB ID is 1 for scalar values (5 bits starting at 26)

// Second TOB is (MET_x, MET_y)
  outTOB[1] = MET_y; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[1] = outTOB[1] | (MET_x  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (MET_y != 0) outTOB[1] = outTOB[1] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MET_x != 0) outTOB[1] = outTOB[1] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[1] = outTOB[1] | (2  &  0x0000001F) << 26;//TOB ID is 2 for MET_x, MET_y (5 bits starting at 26)

// Third TOB is hard components (MHT_x, MHT_y)
  outTOB[2] = MHT_y; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[2] = outTOB[2] | (MHT_x  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (MHT_y != 0) outTOB[2] = outTOB[2] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MHT_x != 0) outTOB[2] = outTOB[2] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[2] = outTOB[2] | (3  &  0x0000001F) << 26;//TOB ID is 3 for hard components (5 bits starting at 26)

  // Fourth TOB is hard components (MST_x, MST_y)
  outTOB[3] = MST_y; //set the Quantity2 to the corresponding slot (LSB)
  outTOB[3] = outTOB[3] | (MST_x  &  0x00000FFF) << 12;//Quantity 1 (in bit number 12)
  if (MST_y != 0) outTOB[3] = outTOB[3] | 0x00000001 << 24;//Status bit for Quantity 2 (0 if quantity is null)
  if (MST_x != 0) outTOB[3] = outTOB[3] | 0x00000001 << 25;//Status bit for Quantity 1 (0 if quantity is null)
  outTOB[3] = outTOB[3] | (4  &  0x0000001F) << 26;//TOB ID is 4 for soft components (5 bits starting at 26)


  tobs_v[0] = std::make_unique<gFEXJwoJTOB>();
  tobs_v[0]->setWord(outTOB[0]);
  tobs_v[0]->setQuantity1(MET);
  tobs_v[0]->setQuantity2(total_sumEt);
  tobs_v[0]->setSaturation(0); //Always 0 for now, need a threshold?
  tobs_v[0]->setTobID(1);
  if( MET != 0 ) tobs_v[0]->setStatus1(1);
  else tobs_v[0]->setStatus1(0);
  if(total_sumEt!= 0) tobs_v[0]->setStatus2(1);
  else tobs_v[0]->setStatus2(0);

  tobs_v[1] = std::make_unique<gFEXJwoJTOB>();
  tobs_v[1]->setWord(outTOB[1]);
  tobs_v[1]->setQuantity1(MET_x);
  tobs_v[1]->setQuantity2(MET_y);
  tobs_v[1]->setSaturation(0); //Always 0 for now, need a threshold?
  tobs_v[1]->setTobID(2);
  if( MET_x != 0 ) tobs_v[1]->setStatus1(1);
  else tobs_v[1]->setStatus1(0);
  if(MET_y!= 0) tobs_v[1]->setStatus2(1);
  else tobs_v[1]->setStatus2(0);

  tobs_v[2] = std::make_unique<gFEXJwoJTOB>();
  tobs_v[2]->setWord(outTOB[2]);
  tobs_v[2]->setQuantity1(MHT_x);
  tobs_v[2]->setQuantity2(MHT_y);
  tobs_v[2]->setSaturation(0); //Always 0 for now, need a threshold?
  tobs_v[2]->setTobID(3);
  if( MHT_x != 0 ) tobs_v[2]->setStatus1(1);
  else tobs_v[2]->setStatus1(0);
  if(MHT_y!= 0) tobs_v[2]->setStatus2(1);
  else tobs_v[2]->setStatus2(0);

  tobs_v[3] = std::make_unique<gFEXJwoJTOB>();
  tobs_v[3]->setWord(outTOB[3]);
  tobs_v[3]->setQuantity1(MST_x);
  tobs_v[3]->setQuantity2(MST_y);
  tobs_v[3]->setSaturation(0); //Always 0 for now, need a threshold?
  tobs_v[3]->setTobID(4);
  if( MST_x != 0 ) tobs_v[3]->setStatus1(1);
  else tobs_v[2]->setStatus1(0);
  if(MST_y!= 0) tobs_v[3]->setStatus2(1);
  else tobs_v[3]->setStatus2(0);


  return tobs_v;

}



void gFEXJwoJAlgo::gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum){

  int rows = twrs.size();
  int cols = twrs[0].size();

  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      // zero jet sum here
      gBlkSum[irow][jcolumn] = 0;
      int krowUp = (irow + 1)%32;
      int krowDn = (irow - 1 +32)%32;
      if( (jcolumn == 0) || (jcolumn == 6) ) {
        //left edge case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn+1] + twrs[krowUp][jcolumn+1] + twrs[krowDn][jcolumn+1];
            } else if( (jcolumn == 5) || (jcolumn == 11)) {
        //  right edge case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn-1] + twrs[krowUp][jcolumn-1] + twrs[krowDn][jcolumn-1];
            } else{
        // normal case
        gBlkSum[irow][jcolumn] =
          twrs[irow][jcolumn]   + twrs[krowUp][jcolumn]   + twrs[krowDn][jcolumn]   +
          twrs[irow][jcolumn-1] + twrs[krowUp][jcolumn-1] + twrs[krowDn][jcolumn-1] +
          twrs[irow][jcolumn+1] + twrs[krowUp][jcolumn+1] + twrs[krowDn][jcolumn+1];
        }
    }
  }

}


void gFEXJwoJAlgo::metFPGA(gTowersCentral twrs, gTowersCentral & gBlkSum, int gBlockthreshold,
                           unsigned int & MHT_x, unsigned int & MHT_y,
                           unsigned int & MST_x, unsigned int & MST_y,
                           unsigned int & MET_x, unsigned int & MET_y){

  int rows = twrs.size();
  int cols = twrs[0].size();
  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      if(gBlkSum[irow][jcolumn] > gBlockthreshold){
        MHT_x += (twrs[irow][jcolumn])*cosLUT(irow, 5, 6);
        MHT_y += (twrs[irow][jcolumn])*sinLUT(irow, 5, 6);
      }
      else{
       MST_x += (twrs[irow][jcolumn])*cosLUT(irow, 5, 6);
       MST_y += (twrs[irow][jcolumn])*sinLUT(irow, 5, 6);
      }
    }
  }
  MET_x = m_aFPGA_A * MHT_x + m_bFPGA_A * MST_x;
  MET_y = m_aFPGA_B * MHT_y + m_bFPGA_B * MST_y;


}

void gFEXJwoJAlgo::metTotal(unsigned int A_MET_x, unsigned int A_MET_y,
                            unsigned int B_MET_x, unsigned int B_MET_y,
                            unsigned int & MET_x, unsigned int & MET_y, unsigned int & MET){

  MET_x = A_MET_x + B_MET_x;
  MET_y = A_MET_y + B_MET_y;
  MET   = sqrt((MET_x * MET_x) + (MET_y * MET_y));

}


void gFEXJwoJAlgo::sumEtFPGA(gTowersCentral twrs, unsigned int & partial_sumEt ){

  int rows = twrs.size();
  int cols = twrs[0].size();

  for( int irow = 0; irow < rows; irow++ ){
    for(int jcolumn = 0; jcolumn<cols; jcolumn++){
      partial_sumEt += twrs[irow][jcolumn];
    }
  }

}

void gFEXJwoJAlgo::sumEt(unsigned int  A_sumEt, unsigned int  B_sumEt, unsigned int & total_sumEt ){

  total_sumEt = A_sumEt + B_sumEt;

}

//----------------------------------------------------------------------------------
// bitwise simulation of sine LUT in firmware
//----------------------------------------------------------------------------------
unsigned int gFEXJwoJAlgo::sinLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw)
{
  float c = ((float)phiIDX)/pow(2,aw);
  float rad = 2*M_PI*c;
  float rsin = sin(rad);

  return static_cast<unsigned int>(round((pow(2.0,dw-1)-1.0)*rsin));
}

//----------------------------------------------------------------------------------
// bitwise simulation cosine LUT in firmware
//----------------------------------------------------------------------------------
unsigned int gFEXJwoJAlgo::cosLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw)
{
  float c = ((float)phiIDX)/pow(2,aw);
  float rad = 2*M_PI*c;
  float rcos = cos(rad);

  return static_cast<unsigned int>(round((pow(2.0,dw-1)-1.0)*rcos));
}




} // namespace LVL1
