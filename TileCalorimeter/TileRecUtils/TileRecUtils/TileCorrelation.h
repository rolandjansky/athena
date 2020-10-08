/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************************************************
#ifndef TILERECUTILS_TILECORRELATION_H
#define TILERECUTILS_TILECORRELATION_H

// Filename : TileCorrelation.h
// Author   : C Cuenca
// Created  : May 2004

// DESCRIPTION
// Implementation of a class for correlation among digits calculation.

// ************************************************************************************

#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaKernel/getMessageSvc.h"

class TileHWID;

#include <vector>
#include <string>
#include <fstream>


class TileCorrelation
  : public AthMessaging
{
  public:
    TileCorrelation();
    ~TileCorrelation();
    void setCorrelationZero(int dignum);
    void setCorrelationDelta(int dignum);

    void sum(std::vector<double> &digits, int ros, int drawer, int channel, int gain, int &dignum);

    void runningCorrelation(std::vector<double> &digits, int ros, int drawer, int channel, int gain,
        int &dignum, int chthres);

    void calculateCorrelation(int dignum);
    void calculateRunningCorrelation(int dignum, int chthres, bool is7to9);
    void printCorrelation(int dignum);
    void saveCorrelationSumm(bool deltaCorrelation, std::string correlationSummOptFilterFile,
        const TileHWID *tileHWID, int dignum);

    void saveCorrelationMatrix(bool deltaCorrelation, std::string correlationMatrixOptFilterFile,
        const TileHWID *tileHWID, int dignum);

    void calculateWeights(bool deltaCorrelation, std::vector<double> shapeFormLG, std::vector<double> shapeFormHG,
        std::vector<double> shapeFormDerivativeLG, std::vector<double> shapeFormDerivativeHG, std::string aiLoOptFilterFile,
        std::string biLoOptFilterFile, std::string aiHiOptFilterFile, std::string biHiOptFilterFile,
        const TileHWID *tileHWID, int dignum);
  
    void buildPulseShape(std::vector<double> &pulseShape, std::vector<double> &pulseShapeY,
        std::vector<double> &pulseShapeT, int dignum);


  private:
    //double SS(9,9);
    double m_SS[4][64][48][2][9][9];
    double m_S[4][64][48][2][9];
    double m_R[4][64][48][2][9][9];
    //int N_events[4][64][48][2];

    double m_corr[9];
    double m_corrSum[4][64][48][2][9];
    double m_corrSum2[4][64][48][2][9];
    double m_nCorr;
    int m_N[4][64][48][2];
    int m_jEntry;
    int m_lag;
    int m_nPairs[4][64][48][2][9];
    double m_nD;
    double m_S1[4][64][48][2][9];
    double m_S2[4][64][48][2][9];
    double m_S11[4][64][48][2][9];
    double m_S12[4][64][48][2][9];
    double m_S22[4][64][48][2][9];
};


#endif // TILERECUTILS_TILECORRELATION_H
