/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDEROPTFILTER_H
#define TILERECUTILS_TILERAWCHANNELBUILDEROPTFILTER_H

//////////////////////////////////////////////////////////////////////
//
//     Tilecal Valencia, Ximo.Poveda@cern.ch. June 2007
//
//     TileRawChannelBuilderOptFilter.h
//
//     implementation of the Optimal Filtering based on Lagrange multipliers
//       for energy/time reconstruction in TileCal 
//
//////////////////////////////////////////////////////////////////////

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/TileOptFilterWeights.h"
#include "TileConditions/TilePulseShapes.h"

#include <vector>
#include <string>

/**
 *
 * @class TileRawChannelBuilderOptFilter
 * @brief Reconstructs Tile digitized pulses (ie, computes amplitude, time and pedestal) as a linear combination of the samples
 *
 * This class implements a energy reconstruction method known as Optimal
 * Filtering. It takes as input the digital samples from the digitizer boards
 * in the front-end electronics and outputs the amplitude, time and pedestal
 * of the pulse. Full details and fundaments of the method can be found in
 * the ATL-TILECAL-2005-001 note. Two different versions of the algorithms
 * are currently used: OF1 (with 2 parameters: amplitude and time) and OF2
 * (with 3 parameters (amplitude, time and pedestal).
 */
class TileRawChannelBuilderOptFilter: public TileRawChannelBuilder {
  public:

    TileRawChannelBuilderOptFilter(const std::string& type, const std::string& name,
        const IInterface *parent); //!< Constructor
    ~TileRawChannelBuilderOptFilter(); //!< Destructor

    // virtual methods
    virtual StatusCode initialize(); //!< Initialize method
    //virtual StatusCode execute();
    virtual StatusCode finalize(); //!< Finalize method

    // Inherited from TileRawChannelBuilder
    virtual TileRawChannel* rawChannel(const TileDigits* digits);

    /**
     * AlgTool InterfaceID
     */
    static const InterfaceID& interfaceID();

  private:
    //!< Applies OF algorithm
    double filter(int ros, int drawer, int channel, int &gain, double &pedestal, double &amplitude, double &time);
    float findMaxDigitsDifference(); //!< Computes maximum difference between digits
    float findMaxDigit(); //!< Finds maximum digit value in the pulse
    int findMaxDigitPosition();  //!< Finds maximum digit position in the pulse
    bool are3FF(); //!< Checks that all the samples are 0x3FF (as sent by the DSP when no data arrives)
    float getPedestal(); //!< Gets pedestal estimation for OF1
    //!< Apply the number of iterations needed for reconstruction by calling the Filter method
    int iterate(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time,
        double &chi2);
    //!< Computes A,time,ped using OF. If iterations are required, the Iterator method is used
    double compute(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time,
        int phase);
    void buildPulseShape(std::vector<double> &pulseShape, std::vector<double> &pulseShapeY,
        std::vector<double> &pulseShapeT, int dignum); //!< Builds pulse shapes

    int m_maxIterations; //!< maximum number of iteration to perform
    int m_pedestalMode;  //!< pedestal mode to use
    bool m_confTB;       //!< use testbeam configuration
    double m_timeForConvergence; //!< minimum time difference to quit iteration procedure
    bool m_of2;    //!< bool variable for OF method: true=> OF2;  false=> OF1
    bool m_minus1Iter;   //!< bool variable for whether to apply -1 iteration (initial phase guess)
    bool m_correctAmplitude; //!< If true, resulting amplitude is corrected when using weights for tau=0 without iteration
    bool m_correctTimeNI; //!< If true, resulting time is corrected when using method  without iteration

    int m_nSignal; //!< internal counters
    int m_nNegative;  //!< internal counters
    int m_nCenter; //!< internal counters

    int m_nSamples;   //!< number of samples in the data
    int m_t0SamplePosition;  //!< position of peak sample = (m_nSamp-1)/2
    double m_maxTime; //!< max allowed time = 25*(m_nSamp-1)/2
    double m_minTime; //!< min allowed time = -25*(m_nSamp-1)/2

    /*
     double a_phys_simp[2][9][25], b_phys_simp[2][9][25];
     double a_cis_simp[2][7][25], b_cis_simp[2][7][25];
     double a_phys[4][64][48][2][9][25], b_phys[4][64][48][2][9][25];
     double a_cis[4][64][48][2][7][25], b_cis[4][64][48][2][7][25];
     */
    std::vector<double> m_pulseShapeCisLG;  //!< vector for low gain/CIS pulse shape
    std::vector<double> m_pulseShapeCisHG;  //!< vector for high gain/CIS pulse shape
    std::vector<double> m_pulseShapePhysLG; //!< vector for low gain/Physics pulse shape
    std::vector<double> m_pulseShapePhysHG; //!< vector for high gain/Physics pulse shape

    std::vector<double> m_pulseShapeDerivativeCisLG;  //!< vector for low gain/CIS pulse derivative
    std::vector<double> m_pulseShapeDerivativeCisHG;  //!< vector for high gain/CIS pulse derivative
    std::vector<double> m_pulseShapeDerivativePhysLG; //!< vector for low gain/Physics pulse derivative
    std::vector<double> m_pulseShapeDerivativePhysHG; //!< vector for high gain/Physics pulse derivative

    TilePulseShapesStruct* m_pulseShapes;       //!< structure for pulse shapes

    TileOptFilterWeightsStruct *m_weights;      //!< structure for OF weights
    //  TileOptFilterCorrelationStruct *m_correla;   //!< structure for pulse shapes

    std::vector<float> m_digits;
};

#endif
