/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERECUTILS_TILERAWCHANNELBUILDERQIEFILTER_H
#define TILERECUTILS_TILERAWCHANNELBUILDERQIEFILTER_H

//////////////////////////////////////////////////////////////////////
//
//     Author:
//     Alexander.Paramonov@cern.ch
//
//     Created:
//    05 February 2016
//
//  	 File Name:
//     TileRawChannelBuilderQIEFilter.h
//
//    Description:
//		The code calculates time and energy/charge of QIE pulses
//
//////////////////////////////////////////////////////////////////////

// Tile includes
#include "TileRecUtils/TileRawChannelBuilder.h"
#include "TileConditions/ITileCondToolOfc.h"
#include "TileConditions/TileCondToolOfcCool.h"
#include "TileConditions/TileCondToolTiming.h"
#include "TileConditions/TileCondToolNoiseSample.h"

#include <vector>
#include <string>

/**
 *
 * @class TileRawChannelBuilderQIEFilter
 * @brief Reconstructs Tile digitized pulses (ie, computes amplitude and time) for the QIE front-end
 *
 */

class TileRawChannelBuilderQIEFilter: public TileRawChannelBuilder {
  public:

    TileRawChannelBuilderQIEFilter(const std::string& type, const std::string& name, const IInterface *parent); //!< Constructor
    ~TileRawChannelBuilderQIEFilter(); //!< Destructor

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

    //!< Callback added to handle Data-driven GeoModel initialization
    //virtual StatusCode geoInit(IOVSVC_CALLBACK_ARGS);

    //ToolHandle<TileCondToolTiming> m_tileToolTiming;
    //ToolHandle<ITileCondToolOfc> m_tileCondToolOfc;
    //ToolHandle<TileCondToolOfcCool> m_tileCondToolOfcCool;
    //ToolHandle<TileCondToolNoiseSample> m_tileToolNoiseSample; //!< tool which provides noise values

    //!< Applies OF algorithm
    float filter(int ros, int drawer, int channel, float &amplitude, float &time);
    int findMaxDigitPosition();  //!< Finds maximum digit position in the pulse
    //!< Sets pedestal estimation for OF1
    //float setPedestal(int ros, int drawer, int channel, int gain);
    //!< Apply the number of iterations needed for reconstruction by calling the Filter method
    //int iterate(int ros, int drawer, int channel, int gain, double &pedestal, double &amplitude, double &time,
    //    double &chi2);
    //!< Computes A,time,ped using OF. If iterations are required, the Iterator method is used
    float compute(int ros, int drawer, int channel, float &amplitude, float &time);

//    void BuildPulseShape(std::vector<double> &m_pulseShape, std::vector<double> &m_pulseShapeX
//        , std::vector<double> &m_pulseShapeT, int dignum, MsgStream &log); //!< Builds pulse shapes

    //void ofc2int(int nDigits, double* w_off, short* w_int, short& scale); // convert weights to dsp short int format

    //int m_maxIterations; //!< maximum number of iteration to perform
    int m_pedestalMode;  //!< pedestal mode to use
    //bool m_confTB;       //!< use testbeam configuration
    //double m_timeForConvergence; //!< minimum time difference to quit iteration procedure
   // bool m_of2;    //!< bool variable for OF method: true=> OF2;  false=> OF1
    //bool m_minus1Iter;   //!< bool variable for whether to apply -1 iteration (initial phase guess)
   // bool m_correctAmplitude; //!< If true, resulting amplitude is corrected when using weights for tau=0 without iteration

    //bool m_bestPhase; // if true, use best phase from COOL DB in "fixed phase" mode (i.e., no iterations)
    //bool m_ofcFromCool; // if true, take OFCs from DB (no on-fly calculations)
    //bool m_emulateDsp; // if true, emulate DSP reconstruction algorithm
    int m_nSignal; //!< internal counters
    //int m_nNegative;  //!< internal counters
    //int m_nCenter; //!< internal counters
    int m_nConst;  //!< internal counters

    int m_nSamples; //!< number of samples in the data
    //int m_t0SamplePosition;  //!< position of peak sample = (m_nSamples-1)/2
    //float m_maxTime; //!< max allowed time = 25*(m_nSamples-1)/2
    //float m_minTime; //!< min allowed time = -25*(m_nSamples-1)/2

//    std::vector<double> m_LpulseShape_cis;  //!< vector for low gain/CIS pulse shape
//    std::vector<double> m_HpulseShape_cis;  //!< vector for high gain/CIS pulse shape
//    std::vector<double> m_LpulseShape_phys; //!< vector for low gain/Physics pulse shape
//    std::vector<double> m_HpulseShape_phys; //!< vector for high gain/Physics pulse shape
//
//    std::vector<double> m_LdpulseShape_cis;  //!< vector for low gain/CIS pulse derivative
//    std::vector<double> m_HdpulseShape_cis;  //!< vector for high gain/CIS pulse derivative
//    std::vector<double> m_LdpulseShape_phys; //!< vector for low gain/Physics pulse derivative
//    std::vector<double> m_HdpulseShape_phys; //!< vector for high gain/Physics pulse derivative

    std::vector<float> m_digits;
};

#endif
