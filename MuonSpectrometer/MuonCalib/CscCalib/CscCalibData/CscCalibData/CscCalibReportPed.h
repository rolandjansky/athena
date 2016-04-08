/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CSCCALIBDATA_CSCCALIBREPORTPED_H
#define CSCCALIBDATA_CSCCALIBREPORTPED_H

/**************************************************************************
Package: MuonSpectrometer/MuonCalib/CscCalib/CscCalibData
Name: CscCalibReportPed.h
Author: Caleb Parnell-Lampen
Date & Place: July 4, 2008, University of Arizona

Base class to hold info for a class. Mostly an interface class for which 
different types of reports can be derived from. The reports are meant to 
include details about the calibration process, as opposed to the 
CscCalibResults which contain just the simple results of the calibration.
 ****************************************************************************/
#include "CscCalibData/CscCalibReportBase.h"
#include "DataModel/DataVector.h"
#include "TH1.h"
#include "TH2.h"

class CscCalibReportPed : public CscCalibReportBase
{
  private:
    //Pedestal amplitude histograms
    DataVector<TH1I> * m_pedAmpHists;
    DataVector< DataVector<TH1I> > * m_sampHists;
    DataVector<TH1I> * m_bitHists;
    DataVector<TH2F> * m_bitCorrelation;
    std::vector<int> * m_onlineTHoldTests;

  public:

    /* default constructor */
    CscCalibReportPed();

    /* full constructor */
    CscCalibReportPed(std::string label);


    ~CscCalibReportPed();

    /** Set the pedAmpHist vector.  
      @PARAM somePedAmpHists - Pointer to DataVector of pedestal histograms. Takes ownership.
     */
    void setPedAmpHists(DataVector<TH1I> * somePedAmpHists);

    /** Set the sample histogram vector.  
      @PARAM someSampHists - Pointer to DataVector of pedestal histograms. Takes ownership.
     */

    void setSampHists(DataVector<DataVector<TH1I> >* someSampHists);

    /** Set the pedAmpHist vector.  
      @PARAM somePedAmpHists - Pointer to DataVector of pedestal histograms. Takes ownership.
     */
    void setBitHists(DataVector<TH1I> * somePedAmpHists);

    void setBitCorrelation(DataVector<TH2F> * somebitCorrelation);




    /**Retrieve pedestal amplitude histogram vector*/
    const DataVector<TH1I> * getPedAmpHists() const;

    /**Retrieve pedestal  sample amplitude histogram vector*/
    const DataVector<DataVector<TH1I> >* getSampHists() const;


    /**Retrive bit histogram vector*/
    const DataVector<TH1I> * getBitHists() const;        

    const DataVector<TH2F> *getBitCorrelation() const;

    //**setOnlineTHoldTests*///
    void setOnlineTHoldTests(std::vector<int> * onlineTests);

    /**setOnlineTholdTests - contains number of times a channel's sample went above online threshold*/
    const std::vector<int> * getOnlineTHoldTests() const;

};

#endif
