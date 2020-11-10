/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////
//
// Filename : TileCorrelation.cxx
//
//  Author   : Valencia TileCal group, cristobal.cuenca@cern.ch,
//  Mantained by Ximo Poveda, jpoveda@cern.ch
// 
//  Created  : May, 2004
//  Moved to TileRecUtils on Jan'05
//
////////////////////////////////////////

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "PathResolver/PathResolver.h"

#include "TileRecUtils/TileCorrelation.h"
#include "TileConditions/TileInfo.h"
#include "TileIdentifier/TileHWID.h"
#include "TileCalibBlobObjs/TileCalibUtils.h"


#include "CLHEP/Matrix/Matrix.h"
//#include "TileConditions/TilePulseShapes.h"

#include <cmath>
#include <iostream>
#include <iomanip>

//using CLHEP::HepMatrix;


////////////////////////////////////////
TileCorrelation::TileCorrelation()
  : AthMessaging (Athena::getMessageSvc(), "TileCorrelation")
  , m_SS()
  , m_S()
  , m_R()
  , m_corr()
  , m_corrSum()
  , m_corrSum2()
  , m_nCorr(0.0)
  , m_N()
  , m_jEntry(0) 
  , m_lag(0)
  , m_nPairs()
  , m_nD(0.0)
  , m_S1()
  , m_S2()
  , m_S11()
  , m_S12()
  , m_S22()
{
}


////////////////////////////////////////
TileCorrelation::~TileCorrelation() {
}

////////////////////////////////////////
void TileCorrelation::setCorrelationZero(int dignum) {

  ATH_MSG_DEBUG("SetCorrelationZero");

  for (int ros = 0; ros < 4; ++ros)
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer)
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
          m_N[ros][drawer][channel][gain] = 0;
          //N[ros][drawer][channel][gain]=0;
          for (int i = 0; i < dignum; ++i) {
            m_S[ros][drawer][channel][gain][i] = 0.;
            for (int j = 0; j < dignum; ++j) {
              m_SS[ros][drawer][channel][gain][i][j] = 0.;
              m_R[ros][drawer][channel][gain][i][j] = 0.;
            }
          }

          for (m_lag = 0; m_lag < 9; ++m_lag) {
            m_S1[ros][drawer][channel][gain][m_lag] = 0.;
            m_S2[ros][drawer][channel][gain][m_lag] = 0.;
            m_S11[ros][drawer][channel][gain][m_lag] = 0.;
            m_S12[ros][drawer][channel][gain][m_lag] = 0.;
            m_S22[ros][drawer][channel][gain][m_lag] = 0.;
            m_nPairs[ros][drawer][channel][gain][m_lag] = 0;
            m_corrSum[ros][drawer][channel][gain][m_lag] = 0.;
            m_corrSum2[ros][drawer][channel][gain][m_lag] = 0.;
          }
        }

  for (m_lag = 0; m_lag < 9; ++m_lag) {
    m_corr[m_lag] = 0.;
  }

}

////////////////////////////////////////
void TileCorrelation::setCorrelationDelta(int dignum) {

  ATH_MSG_DEBUG("SetCorrelationDelta");

  for (int ros = 0; ros < 4; ++ros)
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer)
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
          m_N[ros][drawer][channel][gain] = 1;
          for (int i = 0; i < dignum; ++i)
            for (int j = 0; j < dignum; ++j)
              if (i == j)
                m_R[ros][drawer][channel][gain][i][j] = 1.;
              else
                m_R[ros][drawer][channel][gain][i][j] = 0.;
        }
}

////////////////////////////////////////
void TileCorrelation::sum(std::vector<double> &digits, int ros, int drawer, int channel, int gain, int &dignum) {

  ATH_MSG_VERBOSE("Sum");

  double N_d = 0.;
  dignum = digits.size();

  m_N[ros][drawer][channel][gain]++;
  N_d = double(m_N[ros][drawer][channel][gain]);
  for (int i = 0; i < dignum; ++i) {
    m_S[ros][drawer][channel][gain][i] += digits[i];
    for (int j = 0; j < dignum; ++j)
      m_SS[ros][drawer][channel][gain][i][j] += digits[i] * digits[j];
    //for (int j=0;j<i+1;j++) SS[ros][drawer][channel][gain][i][j]+=digits[i]*digits[j];
  }

  ATH_MSG_DEBUG(" Sum, ros=" << ros
                << " drawer=" << drawer
                << " channel=" << channel
                << " gain=" << gain
                << " N=" << m_N[ros][drawer][channel][gain]
                << " Sum[1]=" << m_S[ros][drawer][channel][gain][1]
                << " Sum[2]=" << m_S[ros][drawer][channel][gain][2]
                << " Sum[1][2]=" << m_SS[ros][drawer][channel][gain][1][2]
                << " Sum[1][1]=" << m_SS[ros][drawer][channel][gain][1][1]
                << " Sum[2][2]=" << m_SS[ros][drawer][channel][gain][2][2]
                << " B[1][2]=" << m_SS[ros][drawer][channel][gain][1][2] / N_d
                                  - m_S[ros][drawer][channel][gain][1] / N_d * m_S[ros][drawer][channel][gain][2] / N_d
                << " Correlation[1][2]="  << (N_d * m_SS[ros][drawer][channel][gain][1][2]
                                              - m_S[ros][drawer][channel][gain][1] * m_S[ros][drawer][channel][gain][2])
                                                  / sqrt((N_d * m_SS[ros][drawer][channel][gain][1][1]
                                                            - m_S[ros][drawer][channel][gain][1]
                                                                * m_S[ros][drawer][channel][gain][1])
                                                                * (N_d * m_SS[ros][drawer][channel][gain][2][2]
                                                                  - m_S[ros][drawer][channel][gain][2]
                                                                     * m_S[ros][drawer][channel][gain][2]))
             );

}

////////////////////////////////////////
void TileCorrelation::calculateCorrelation(int dignum) {

  ATH_MSG_DEBUG("CalcCorrelation");

  for (int ros = 0; ros < 4; ++ros)
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer)
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
          double N_d = double(m_N[ros][drawer][channel][gain]);
          if (N_d > 0.)
            ATH_MSG_VERBOSE(" CalcCorrelation, ros=" << ros
                            << " drawer=" << drawer
                            << " channel=" << channel
                            << " gain=" << gain
                            << " N_d=" << N_d);

          for (int i = 0; i < dignum; ++i)
            //	      for (int j=0;j<i+1;j++)
            for (int j = 0; j < dignum; ++j) {
              if (N_d > 0.) {
                //		      std::cout<<"b  i="<<i<<" j="<<j<<std::endl
                // 			  <<" R[ros][drawer][channel][gain][i][j]="<<R[ros][drawer][channel][gain][i][j]<<std::endl
                // 			  <<" N_d="<<N_d<<std::endl
                // 			  <<" S[ros][drawer][channel][gain][i]="<<S[ros][drawer][channel][gain][i]<<std::endl
                // 			  <<" S[ros][drawer][channel][gain][j]="<<S[ros][drawer][channel][gain][j]<<std::endl
                // 			  <<" SS[ros][drawer][channel][gain][i][j]="<<SS[ros][drawer][channel][gain][i][j]<<std::endl
                // 			  <<" SS[ros][drawer][channel][gain][i][i]="<<SS[ros][drawer][channel][gain][i][i]<<std::endl
                // 			  <<" SS[ros][drawer][channel][gain][j][j]="<<SS[ros][drawer][channel][gain][j][j]<<std::endl
                //<<" S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]="<<S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]<<std::endl
                //<<" N_d*SS[ros][drawer][channel][gain][i][j]="<<N_d*SS[ros][drawer][channel][gain][i][j]<<std::endl
                // 			  <<" N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]="<<N_d*SS[ros][drawer][channel][gain][i][j]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][j]<<std::endl
                // 			  <<" N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j])="<<N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]<<std::endl
                // 			  <<" N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i]="<<N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i]<<std::endl
                // 			  <<" sqrt((N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*(N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]))="<< sqrt((N_d*SS[ros][drawer][channel][gain][i][i]-S[ros][drawer][channel][gain][i]*S[ros][drawer][channel][gain][i])*(N_d*SS[ros][drawer][channel][gain][j][j]-S[ros][drawer][channel][gain][j]*S[ros][drawer][channel][gain][j]))
                //			  <<std::endl;
                m_R[ros][drawer][channel][gain][i][j] = (N_d * m_SS[ros][drawer][channel][gain][i][j]
                    - m_S[ros][drawer][channel][gain][i] * m_S[ros][drawer][channel][gain][j])
                    / sqrt(
                        (N_d * m_SS[ros][drawer][channel][gain][i][i]
                            - m_S[ros][drawer][channel][gain][i] * m_S[ros][drawer][channel][gain][i])
                            * (N_d * m_SS[ros][drawer][channel][gain][j][j]
                                - m_S[ros][drawer][channel][gain][j] * m_S[ros][drawer][channel][gain][j]));
                //		      std::cout<<"R[ros][drawer][channel][gain][i][j]="<<R[ros][drawer][channel][gain][i][j]<<std::endl;
              } else
                m_R[ros][drawer][channel][gain][i][j] = -1234.;
            }
        }
}

////////////////////////////////////////
void TileCorrelation::runningCorrelation(std::vector<double> &digits, int ros, int drawer, int channel, int gain,
    int &dignum, int chthres) {

  ATH_MSG_VERBOSE("RunningCorrelation");

  dignum = digits.size();

  //chthres=10;
  //update sums
  m_N[ros][drawer][channel][gain]++;
  m_jEntry = m_N[ros][drawer][channel][gain];
  m_nD = double(m_jEntry);

  if (ros == 1 && drawer == 1 && channel == 0 && gain == 1)
    ATH_MSG_INFO("Computing RunningCorrelation for jentry=" << m_jEntry);

  for (m_lag = 1; m_lag < dignum; ++m_lag) {
    for (int i = 0; i < dignum - m_lag; ++i) {
      m_S1[ros][drawer][channel][gain][m_lag - 1] += digits[i];
      m_S2[ros][drawer][channel][gain][m_lag - 1] += digits[i + m_lag];
      m_S12[ros][drawer][channel][gain][m_lag - 1] += digits[i] * digits[i + m_lag];
      m_S11[ros][drawer][channel][gain][m_lag - 1] += digits[i] * digits[i];
      m_S22[ros][drawer][channel][gain][m_lag - 1] += digits[i + m_lag] * digits[i + m_lag];
      m_nPairs[ros][drawer][channel][gain][m_lag - 1]++;
    }
    if (m_lag == 1 && ros == 1 && drawer == 1 && channel == 0 && gain == 1)
      ATH_MSG_VERBOSE(" jentry=" << m_jEntry
                       << " N=" << m_nPairs[ros][drawer][channel][gain][m_lag - 1]
                       << " S1=" << m_S1[ros][drawer][channel][gain][m_lag - 1]);


    if (m_jEntry > chthres) {
      m_nCorr = double(m_nPairs[ros][drawer][channel][gain][m_lag - 1]);
      m_corr[m_lag - 1] = (m_nCorr * m_S12[ros][drawer][channel][gain][m_lag - 1]
          - m_S1[ros][drawer][channel][gain][m_lag - 1] * m_S2[ros][drawer][channel][gain][m_lag - 1])
          / sqrt(
              (m_nCorr * m_S11[ros][drawer][channel][gain][m_lag - 1]
                  - m_S1[ros][drawer][channel][gain][m_lag - 1] * m_S1[ros][drawer][channel][gain][m_lag - 1])
                  * (m_nCorr * m_S22[ros][drawer][channel][gain][m_lag - 1]
                      - m_S2[ros][drawer][channel][gain][m_lag - 1] * m_S2[ros][drawer][channel][gain][m_lag - 1]));

      if (m_lag == 1 && ros == 1 && drawer == 1 && channel == 0 && gain == 1)
        ATH_MSG_DEBUG(" corr=" << m_corr[m_lag - 1]
                       << " corr_sum=" << m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                       << " corr_sum_sq=" << m_corrSum2[ros][drawer][channel][gain][m_lag - 1]);


      m_corrSum[ros][drawer][channel][gain][m_lag - 1] += m_corr[m_lag - 1];
      m_corrSum2[ros][drawer][channel][gain][m_lag - 1] += m_corr[m_lag - 1] * m_corr[m_lag - 1];
      //  	  corr_mean=corr_sum[lag-1]/(chthres-jentry);
      //  	  corr_RMS=sqrt(corr_sum_sq[lag-1]*(chthres-jentry)-corr_sum[lag-1]*corr_sum[lag-1])/(chthres-jentry)

      if (m_lag == 1 && ros == 1 && drawer == 1 && channel == 0 && gain == 1)
        ATH_MSG_DEBUG(" jentry=" << m_jEntry
                      << " jentry-chthres=" << m_jEntry - chthres
                      << " lag=1, ros=1, drawer=1, channel=0, gain=1"
                      << " corr=" << m_corr[m_lag - 1]
//                    <<" corr_mean="<<corr_sum[lag-1]/(jentry-chthres)
                      << " sum corr_mean=" << m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                      << " corr_mean=" << m_corrSum[ros][drawer][channel][gain][m_lag - 1] / (m_jEntry - chthres)
//                    <<" RMS="<<sqrt(corr_sum_sq[lag-1]*(jentry-chthres)-corr_sum[lag-1]*corr_sum[lag-1])/(jentry-chthres)
                      << " sum RMS=" << m_corrSum2[ros][drawer][channel][gain][m_lag - 1]
                      << " RMS=" << sqrt( m_corrSum2[ros][drawer][channel][gain][m_lag - 1] * (m_jEntry - chthres)
                                          - m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                                             * m_corrSum[ros][drawer][channel][gain][m_lag - 1]) / (m_jEntry - chthres));

    }
  }
}

////////////////////////////////////////
void TileCorrelation::calculateRunningCorrelation(int dignum, int chthres, bool is7to9) {

  ATH_MSG_VERBOSE("CalcRunningCorrelation");

  for (int ros = 0; ros < 4; ++ros)
    for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer)
      for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
        for (unsigned int gain = 0; TileCalibUtils::MAX_GAIN < 2; ++gain) {
          m_jEntry = m_N[ros][drawer][channel][gain];
          m_nCorr = double(m_jEntry - chthres);

          if (m_jEntry > 0) {
            if (is7to9 && dignum == 7) {
              for (int i = 0; i < 9; ++i)
                m_R[ros][drawer][channel][gain][i][i] = 1.;

              for (m_lag = 1; m_lag < 9; ++m_lag)
                for (int i = 0; i < 9 - m_lag; ++i) {
                  if (m_lag < 7) {
                    m_R[ros][drawer][channel][gain][i][i + m_lag] = m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                        / m_nCorr;
                    m_R[ros][drawer][channel][gain][i + m_lag][i] = m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                        / m_nCorr;
                  } else {
                    m_R[ros][drawer][channel][gain][i][i + m_lag] = 0.;
                    m_R[ros][drawer][channel][gain][i + m_lag][i] = 0.;
                  }

                  if (-1. > m_R[ros][drawer][channel][gain][i][i + m_lag]
                      || m_R[ros][drawer][channel][gain][i][i + m_lag] > 1.)
                    m_R[ros][drawer][channel][gain][i][i + m_lag] = 0.;
                  if (-1. > m_R[ros][drawer][channel][gain][i + m_lag][i]
                      || m_R[ros][drawer][channel][gain][i + m_lag][i] > 1.)
                    m_R[ros][drawer][channel][gain][i + m_lag][i] = 0.;

                }
            } else {
              for (int i = 0; i < dignum; i++)
                m_R[ros][drawer][channel][gain][i][i] = 1.;

              for (m_lag = 1; m_lag < dignum; ++m_lag)
                for (int i = 0; i < dignum - m_lag; ++i) {
                  m_R[ros][drawer][channel][gain][i][i + m_lag] = m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                      / m_nCorr;
                  m_R[ros][drawer][channel][gain][i + m_lag][i] = m_corrSum[ros][drawer][channel][gain][m_lag - 1]
                      / m_nCorr;
                  if (-1. > m_R[ros][drawer][channel][gain][i][i + m_lag]
                      || m_R[ros][drawer][channel][gain][i][i + m_lag] > 1.)
                    m_R[ros][drawer][channel][gain][i][i + m_lag] = 0.;
                  if (-1. > m_R[ros][drawer][channel][gain][i + m_lag][i]
                      || m_R[ros][drawer][channel][gain][i + m_lag][i] > 1.)
                    m_R[ros][drawer][channel][gain][i + m_lag][i] = 0.;
                }
            }
          }
        }
}

////////////////////////////////////////
void TileCorrelation::printCorrelation(int dignum) {

  std::cout << " TileCorrelation::PrintCorrelation()..." << std::endl;
  for (int ros = 0; ros < 1; ++ros) {
    std::cout << " ros=" << ros << std::endl;
    for (int drawer = 31; drawer < 32; ++drawer) {
      std::cout << " drawer=" << drawer << std::endl;
      for (int channel = 17; channel < 24; ++channel) {
        std::cout << " channel=" << channel << std::endl;
        for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
          std::cout << " gain=" << gain << std::endl;
          for (int i = 0; i < dignum; ++i) {
            for (int j = 0; j < dignum; ++j) {
              std::cout << " " << m_R[ros][drawer][channel][gain][i][j];
            }
            std::cout << std::endl;
          }
          std::cout << std::endl;
        }
      }
    }
  }

}

////////////////////////////////////////
void TileCorrelation::saveCorrelationSumm(bool deltaCorrelation, std::string correlationSummOptFilterFile,
    const TileHWID *tileHWID, int dignum) {

  ATH_MSG_DEBUG("SaveCorrelationSumm");

  CLHEP::HepMatrix correlation(dignum, 1, 0);

  std::fstream *correlationFile = new std::fstream(correlationSummOptFilterFile.c_str(), std::fstream::out);
  if (correlationFile->is_open()) ATH_MSG_INFO(correlationSummOptFilterFile << " file open");

  if (deltaCorrelation) {
    //      for (int i=0;i<dignum;i++)
    for (int j = 0; j < dignum; ++j) {
      int i = 0;
      if (m_R[0][0][0][0][i][j] > -100000. && m_R[0][0][0][0][i][j] < 100000.)
        correlation[i][j] = m_R[0][0][0][0][i][j];
      else
        correlation[i][j] = 0.0;
    }

    *correlationFile << correlation.T() << std::endl;
  } else {
    for (int ros = 0; ros < 4; ++ros)
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer) {
        int frag = tileHWID->frag(ros + 1, drawer);
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
          for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
            ATH_MSG_VERBOSE("ros " << ros
                            << "  drawer " << drawer << MSG::hex
                            << "  frag0x " << frag << MSG::dec
                            << "  channel " << channel
                            << "  gain " << gain
                            << "  N " << m_N[ros][drawer][channel][gain]);

            if (m_N[ros][drawer][channel][gain] > 0) {
              //for (int i=0;i<dignum;i++)
              for (int j = 0; j < dignum; ++j) {
                int i = 0;
                if (m_R[ros][drawer][channel][gain][i][j] > -100000. && m_R[ros][drawer][channel][gain][i][j] < 100000.)
                  correlation[i][j] = m_R[ros][drawer][channel][gain][i][j];
                else
                  correlation[i][j] = 0.0;
              }

              *correlationFile << "ros " << ros
                               << "  drawer " << drawer << std::hex
                               << "  frag0x " << frag << std::dec
                               << "  channel " << channel
                               << "  gain " << gain
                               << "  N " << m_N[ros][drawer][channel][gain]
                               << correlation.T();
              //				    <<M_correlation.T()<<std::endl;
              // 		for (int i=0;i<dignum;i++)
              // 		  for (int j=0;j<dignum;j++)
              // 		      *f_correlation<<R[ros][drawer][channel][gain][i][j]<< M_correlation[i][j]<<std::endl;

            }
          }
      }
  }
  correlationFile->close();
}

////////////////////////////////////////
void TileCorrelation::saveCorrelationMatrix(bool deltaCorrelation, std::string correlationMatrixOptFilterFile,
    const TileHWID *tileHWID, int dignum) {

  ATH_MSG_DEBUG("SaveCorrelationMatrix");

  CLHEP::HepMatrix correlation(dignum, dignum, 0);

  std::fstream *correlationFile = new std::fstream(correlationMatrixOptFilterFile.c_str(), std::fstream::out);
  if (correlationFile->is_open()) ATH_MSG_INFO(correlationMatrixOptFilterFile << " file open");

  if (deltaCorrelation) {
    for (int i = 0; i < dignum; ++i)
      for (int j = 0; j < dignum; ++j) {
        if (m_R[0][0][0][0][i][j] > -100000. && m_R[0][0][0][0][i][j] < 100000.)
          correlation[i][j] = m_R[0][0][0][0][i][j];
        else
          correlation[i][j] = 0.0;
      }

    *correlationFile << correlation << std::endl;
  } else {
    for (int ros = 0; ros < 4; ++ros)
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer) {
        int frag = tileHWID->frag(ros + 1, drawer);
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
          for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain) {
            ATH_MSG_VERBOSE("ros " << ros
                            << "  drawer " << drawer << MSG::hex
                            << "  frag0x " << frag << MSG::dec
                            << "  channel " << channel
                            << "  gain " << gain
                            << "  N " << m_N[ros][drawer][channel][gain]);


            if (m_N[ros][drawer][channel][gain] > 0) {
              for (int i = 0; i < dignum; ++i)
                for (int j = 0; j < dignum; ++j) {
                  if (m_R[ros][drawer][channel][gain][i][j] > -100000.
                      && m_R[ros][drawer][channel][gain][i][j] < 100000.)
                    correlation[i][j] = m_R[ros][drawer][channel][gain][i][j];
                  else
                    correlation[i][j] = 0.0;
                }

              *correlationFile << "ros " << ros
                               << "  drawer " << drawer << std::hex
                               << "  frag0x " << frag << std::dec
                               << "  channel " << channel
                               << "  gain " << gain
                               << "  N " << m_N[ros][drawer][channel][gain]
                               << correlation << std::endl;
              // 		for (int i=0;i<dignum;i++)
              // 		  for (int j=0;j<dignum;j++)
              // 		      *f_correlation<<R[ros][drawer][channel][gain][i][j]<< M_correlation[i][j]<<std::endl;

            }
          }
      }
  }
  correlationFile->close();
}

////////////////////////////////////////
void TileCorrelation::calculateWeights(bool deltaCorrelation, std::vector<double> shapeFormLG,
    std::vector<double> shapeFormHG, std::vector<double> shapeFormDerivativeLG,
    std::vector<double> shapeFormDerivativeHG, std::string aiLoOptFilterFile, std::string biLoOptFilterFile,
    std::string aiHiOptFilterFile, std::string biHiOptFilterFile, const TileHWID *tileHWID, int dignum) {

  ATH_MSG_DEBUG("CalcWeights");

  CLHEP::HepMatrix correlation(dignum, dignum, 0);
  CLHEP::HepMatrix inverse(dignum, dignum, 0);
  CLHEP::HepMatrix zero(dignum, dignum, 0);
  CLHEP::HepMatrix pulseShape(dignum, 1, 0);
  CLHEP::HepMatrix pulseShapeDerivative(dignum, 1, 0);
  CLHEP::HepMatrix a(dignum, 1, 0);
  CLHEP::HepMatrix b(dignum, 1, 0);

  std::fstream *aiLoFile = new std::fstream(aiLoOptFilterFile.c_str(), std::fstream::out);
  std::fstream *biLoFile = new std::fstream(biLoOptFilterFile.c_str(), std::fstream::out);
  std::fstream *aiHiFile = new std::fstream(aiHiOptFilterFile.c_str(), std::fstream::out);
  std::fstream *biHiFile = new std::fstream(biHiOptFilterFile.c_str(), std::fstream::out);

  //Open Weights files
  if (aiLoFile->is_open() && aiLoFile->is_open() && aiLoFile->is_open() && aiLoFile->is_open()) {
      ATH_MSG_INFO(" Weights files open");
  }  else {
    ATH_MSG_INFO("Weights files didn't open successfully");
  }

  //pulse shape
  //  std::vector<double> new_shapeForm;
  //   double max=0.;
  //   int nmax=0;

  //   for (int i=0; i<int(m_shapeForm.size());i++)
  //     if (m_shapeForm[i]>max)
  //       {
  // 	max=m_shapeForm[i];
  // 	nmax=i;
  //       }
  //   new_shapeForm.resize(dignum*25,0.);

  //   if (lDebug)
  //     log<<MSG::DEBUG<<"m_shapeForm.size()="<<m_shapeForm.size()<<"m_shapeForm nmax="<<nmax<<" new_shapeForm.size()="<<new_shapeForm.size()<<" new_shapeForm nmax="<<dignum*25/2<<endmsg;

  //   for (int i=0;i<dignum*25;i++)
  //     {
  //       if (i<(dignum*25/2-nmax)) new_shapeForm[i]=0.;
  //       if (i>=(dignum*25/2-nmax) && i<(dignum*25/2-nmax+int(m_shapeForm.size()))) new_shapeForm[i]=m_shapeForm[i-dignum*25/2 +nmax];
  //       if (i>=(dignum*25/2-nmax+int(m_shapeForm.size()))) new_shapeForm[i]=0.;
  //     }

  if (msgLvl(MSG::VERBOSE)) {
    msg(MSG::VERBOSE) << "shapeFormLG, shapeFormDerivativeLG, shapeFormHG, shapeFormDerivativeHG" << endmsg;
    for (int i = 0; i < int(shapeFormLG.size()); ++i)
      msg(MSG::VERBOSE) << i << " " << std::setw(18) << std::setprecision(10)
                        << shapeFormLG[i] << " " << std::setw(18) << std::setprecision(10)
                        << shapeFormDerivativeLG[i] << " " << std::setw(18) << std::setprecision(10)
                        << shapeFormHG[i] << " " << std::setw(18) << std::setprecision(10)
                        << shapeFormDerivativeHG[i] << " " << endmsg;
  }
  //if (lDebug) {
  //   log<<MSG::DEBUG<<"m_HshapeForm"<<endmsg;
  //   for(int i=0;i<int(m_HshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_HshapeForm[i]<<endmsg;
  //   log<<MSG::DEBUG<<"m_LdshapeForm"<<endmsg;
  //   for(int i=0;i<int(m_LdshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_LdshapeForm[i]<<endmsg;
  //   log<<MSG::DEBUG<<"shapeFormDerivativeHG"<<endmsg;
  //   for(int i=0;i<int(m_HdshapeForm.size());i++) log<<MSG::DEBUG<<"    "<<i<<" "<<m_HdshapeForm[i]<<endmsg;
  //}

  double Q1, Q2, Q3, Delta;
  int ierr = 0;

  if (deltaCorrelation) {
    for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain)
      for (int pha = -12; pha < 13; ++pha) {
        correlation = zero;
        inverse = zero;

        for (int i = 0; i < dignum; ++i)
          for (int j = 0; j < dignum; ++j)
            correlation[i][j] = m_R[0][0][0][0][i][j];

        inverse = correlation.inverse(ierr);
        if (ierr == 0) {
          for (int i = 0; i < dignum; ++i) {
            if (gain == 0) {
              pulseShape[i][0] = shapeFormLG[i * 25 + 12 + pha];
              pulseShapeDerivative[i][0] = shapeFormDerivativeLG[i * 25 + 12 + pha];
            } else {
              pulseShape[i][0] = shapeFormHG[i * 25 + 12 + pha];
              pulseShapeDerivative[i][0] = shapeFormDerivativeHG[i * 25 + 12 + pha];
            }
          }

          Q1 = ((pulseShape.T()) * inverse * pulseShape).determinant();
          Q2 = ((pulseShapeDerivative.T()) * inverse * pulseShapeDerivative).determinant();
          Q3 = ((pulseShapeDerivative.T()) * inverse * pulseShape).determinant();
          Delta = Q1 * Q2 - Q3 * Q3;

          a = Q2 / Delta * inverse * pulseShape - Q3 / Delta * inverse * pulseShapeDerivative;
          b = Q3 / Delta * inverse * pulseShape - Q1 / Delta * inverse * pulseShapeDerivative;

          if (gain == 0) {
            *aiLoFile << std::setw(6) << pha;
            for (int i = 0; i < dignum; ++i)
              *aiLoFile << std::setw(18) << std::setprecision(10) << a(i + 1, 1);
            *aiLoFile << std::endl;

            *biLoFile << std::setw(6) << pha;
            for (int i = 0; i < dignum; ++i)
              *biLoFile << std::setw(18) << std::setprecision(10) << b(i + 1, 1);
            *biLoFile << std::endl;
          } else {
            *aiHiFile << std::setw(6) << pha;
            for (int i = 0; i < dignum; ++i)
              *aiHiFile << std::setw(18) << std::setprecision(10) << a(i + 1, 1);
            *aiHiFile << std::endl;

            *biHiFile << std::setw(6) << pha;
            for (int i = 0; i < dignum; ++i)
              *biHiFile << std::setw(18) << std::setprecision(10) << b(i + 1, 1);
            *biHiFile << std::endl;
          }
        }
      }
  } else {
    for (int ros = 0; ros < 4; ++ros)
      for (unsigned int drawer = 0; drawer < TileCalibUtils::MAX_DRAWER ; ++drawer) {
        int frag = tileHWID->frag(ros + 1, drawer);
        for (unsigned int channel = 0; channel < TileCalibUtils::MAX_CHAN; ++channel)
          for (unsigned int gain = 0; gain < TileCalibUtils::MAX_GAIN; ++gain)
            if (m_N[ros][drawer][channel][gain] > 0) {
              if (gain == 0) {
                *aiLoFile << "ros " << ros
                          << "  drawer " << drawer << std::hex
                          << "  frag0x " << frag << std::dec
                          << "  channel " << channel
                          << "  N " << m_N[ros][drawer][channel][0] << std::endl;

                *biLoFile << "ros " << ros
                          << "  drawer " << drawer << std::hex
                          << "  frag0x " << frag << std::dec
                          << "  channel " << channel
                          << "  N " << m_N[ros][drawer][channel][0] << std::endl;
              }
              if (gain == 1) {
                *aiHiFile << "ros " << ros
                          << "  drawer " << drawer << std::hex
                          << "  frag0x " << frag << std::dec
                          << "  channel " << channel
                          << "  N " << m_N[ros][drawer][channel][1] << std::endl;

                *biHiFile << "ros " << ros
                          << "  drawer " << drawer << std::hex
                          << "  frag0x " << frag << std::dec
                          << "  channel " << channel
                          << "  N " << m_N[ros][drawer][channel][1] << std::endl;
              }

              for (int pha = -12; pha < 13; ++pha) {
                correlation = zero;
                inverse = zero;

                for (int i = 0; i < dignum; ++i)
                  for (int j = 0; j < dignum; ++j)
                    correlation[i][j] = m_R[ros][drawer][channel][gain][i][j];

                inverse = correlation.inverse(ierr);
                if (ierr == 0) {
                  for (int i = 0; i < dignum; ++i) {
                    if (gain == 0) {
                      pulseShape[i][0] = shapeFormLG[i * 25 + 12 + pha];
                      pulseShapeDerivative[i][0] = shapeFormDerivativeLG[i * 25 + 12 + pha];
                    } else {
                      pulseShape[i][0] = shapeFormHG[i * 25 + 12 + pha];
                      pulseShapeDerivative[i][0] = shapeFormDerivativeHG[i * 25 + 12 + pha];
                    }

                    // 			      PulseShape[i][0]=new_shapeForm[i*25+12+pha];
                    // 			      if ((i*25+12+pha)>0 || (i*25+12+pha)<224)
                    // 				DPulseShape[i][0]=.5*(new_shapeForm[i*25+13+pha]-new_shapeForm[i*25+11+pha]);
                    // 			      else DPulseShape[i][0]=0.;
                  }

                  //HepStd::std::cout<<" correlation "<<Correlation<<Hepstd::endl;
                  //f_weights<<" correlation "<<Correlation<<std::endl;
                  //		    HepStd::std::cout<<" inverse Matrix "<<Correlation.inverse(ierr)<<Hepstd::endl;
                  //HepStd::std::cout<<" inverse Matrix "<<Inverse<<Hepstd::endl;
                  //f_weights<<" inverse Matrix "<<Inverse<<std::endl;
                  //		    HepStd::std::cout<<" Product "<<Inverse*Correlation<<Hepstd::endl;

                  //std::cout<<" Q1 number of columns="<<((PulseShape.T())*Inverse*PulseShape).num_col()
                  //<<" number of rows="<<((PulseShape.T())*Inverse*PulseShape).num_row()<<std::endl;

                  Q1 = ((pulseShape.T()) * inverse * pulseShape).determinant();
                  Q2 = ((pulseShapeDerivative.T()) * inverse * pulseShapeDerivative).determinant();
                  Q3 = ((pulseShapeDerivative.T()) * inverse * pulseShape).determinant();
                  Delta = Q1 * Q2 - Q3 * Q3;

                  //std::cout<<" Coeffs: Q1="<<Q1<<" Q2="<<Q2<<" Q3="<<Q3<<" Delta="<<Delta<<std::endl;
                  a = Q2 / Delta * inverse * pulseShape - Q3 / Delta * inverse * pulseShapeDerivative;
                  b = Q3 / Delta * inverse * pulseShape - Q1 / Delta * inverse * pulseShapeDerivative;

                  //HepStd::std::cout<<" a Weights= "<<a<<Hepstd::endl;
                  //HepStd::std::cout<<" b Weights= "<<b<<Hepstd::endl;

                  if (gain == 0) {
                    *aiLoFile << std::setw(6) << pha;
                    for (int i = 0; i < dignum; ++i)
                      *aiLoFile << std::setw(18) << std::setprecision(10) << a(i + 1, 1);
                    *aiLoFile << std::endl;

                    *biLoFile << std::setw(6) << pha;
                    for (int i = 0; i < dignum; ++i)
                      *biLoFile << std::setw(18) << std::setprecision(10) << b(i + 1, 1);
                    *biLoFile << std::endl;
                  } else {
                    *aiHiFile << std::setw(6) << pha;
                    for (int i = 0; i < dignum; ++i)
                      *aiHiFile << std::setw(18) << std::setprecision(10) << a(i + 1, 1);
                    *aiHiFile << std::endl;

                    *biHiFile << std::setw(6) << pha;
                    for (int i = 0; i < dignum; ++i)
                      *biHiFile << std::setw(18) << std::setprecision(10) << b(i + 1, 1);
                    *biHiFile << std::endl;
                  }
                }
              }
            }
      }
  }

  aiLoFile->close();
  biLoFile->close();
  aiHiFile->close();
  biHiFile->close();
}

////////////////////////////////////////
void TileCorrelation::buildPulseShape(std::vector<double> &pulseShape, std::vector<double> &pulseShapeY,
    std::vector<double> &pulseShapeT, int dignum) {

  ATH_MSG_DEBUG("BuildPulseShape");

  //1: set m_pulseShape
  pulseShape.resize(dignum * 25);
  ATH_MSG_DEBUG("Set dimension of m_pulseShape to dignum*25=" << dignum * 25);

  //2: scan m_pulseShapeT for: tmin, tmax, nt0 and size: m_pulseShapeX[nt0]=1.0;
  int nt0 = 0, size;
  double tmin = 10000., tmax = -10000.;
  size = pulseShapeT.size();
  for (int i = 0; i < size; ++i) {
    if (pulseShapeT[i] < tmin) tmin = pulseShapeT[i];
    if (pulseShapeT[i] > tmax) tmax = pulseShapeT[i];
    if (pulseShapeT[i] == 0) nt0 = i;
  }

  ATH_MSG_DEBUG("pulseShapeY & pulseShapeT size =" << size
                << ", tmin=" << tmin
                << ", tmax=" << tmax
                << " central point=" << nt0
                << " pulseShapeT[nt0]=" << pulseShapeT[nt0]
                << " pulseShapeY[nt0]=" << pulseShapeY[nt0]);


  //3: fill m_pulseShape
  bool exact;
  int nminn, nminp;
  double minn, minp, tdist;
  pulseShape[dignum * 25 / 2] = pulseShapeY[nt0];
  for (int i = 1; i < dignum * 25 / 2 + 1; ++i) {
    // negative times: 0->dignum*25/2    
    if (-i < tmin)
      pulseShape[dignum * 25 / 2 - i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size - 1;
      for (int j = 0; j < nt0 + 1 && !exact; ++j) {
        if (pulseShapeT[j] == double(-i)) {
          pulseShape[dignum * 25 / 2 - i] = pulseShapeY[j];
          exact = true;
        } else {
          tdist = pulseShapeT[j] - double(-i);
          if (tdist < 0. && tdist > minn) {
            minn = tdist;
            nminn = j;
          }
          if (tdist > 0. && tdist < minp) {
            minp = tdist;
            nminp = j;
          }
        }
      }

      if (exact) {
        ATH_MSG_VERBOSE("exact value found for time=" << -i
                        << " pulseShape=" << pulseShape[dignum * 25 / 2 - i]);

      } else {

        ATH_MSG_VERBOSE("exact value NOT found for time=" << -i
                        << " nminn=" << nminn
                        << " pulseShapeT=" << pulseShapeT[nminn]
                        << " pulseShapeY=" << pulseShapeY[nminn] << std::endl
                        << " nminp=" << nminp
                        << " pulseShapeT=" << pulseShapeT[nminp]
                        << " pulseShapeY=" << pulseShapeY[nminp]);


        pulseShape[dignum * 25 / 2 - i] = pulseShapeY[nminn]
            + (pulseShapeY[nminp] - pulseShapeY[nminn]) / (pulseShapeT[nminp] - pulseShapeT[nminn])
                * (-i - pulseShapeT[nminn]);
      }

    }

    // positive times: dignum*25/2->dignum*25
    if (i > tmax)
      pulseShape[dignum * 25 / 2 + i] = 0.;
    else {
      exact = false;
      minn = -10000.;
      minp = 10000.;
      nminn = 0;
      nminp = size;
      for (int j = nt0; j < size && !exact; ++j) {
        if (pulseShapeT[j] == double(i)) {
          pulseShape[dignum * 25 / 2 + i] = pulseShapeY[j];
          exact = true;
        } else {
          tdist = pulseShapeT[j] - double(i);
          if (tdist < 0) if (tdist > minn) {
            minn = tdist;
            nminn = j;
          }
          if (tdist > 0) if (tdist < minp) {
            minp = tdist;
            nminp = j;
          }
        }
      }
      if (exact) {
        ATH_MSG_VERBOSE("exact value found for time=" << i
                        << " pulseShape=" << pulseShape[dignum * 25 / 2 + i]);

      } else {
        ATH_MSG_VERBOSE("exact value NOT found for time=" << i
                        << " nminn=" << nminn
                        << " pulseShapeT=" << pulseShapeT[nminn]
                        << " pulseShapeY=" << pulseShapeY[nminn] << std::endl
                        << " nminp=" << nminp
                        << " pulseShapeT=" << pulseShapeT[nminp]
                        << " pulseShapeY=" << pulseShapeY[nminp]);


        pulseShape[dignum * 25 / 2 + i] = pulseShapeY[nminn]
            + (pulseShapeY[nminp] - pulseShapeY[nminn]) / (pulseShapeT[nminp] - pulseShapeT[nminn])
                * (i - pulseShapeT[nminn]);
      }
    }
  }
}
