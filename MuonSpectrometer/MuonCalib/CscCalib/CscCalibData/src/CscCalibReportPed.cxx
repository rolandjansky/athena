/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TH1I.h"
#include "TH2F.h"

#include "CscCalibData/CscCalibReportPed.h"
#include "DataModel/DataVector.h"
#include <string>

/* default constructor */
CscCalibReportPed::CscCalibReportPed() : 
    pedAmpHists(NULL),
    sampHists(NULL),
    bitHists(NULL),
    bitCorrelation(NULL),
    onlineTHoldTests(NULL)
{ }

/* full constructor */
CscCalibReportPed::CscCalibReportPed(std::string label) :  
    CscCalibReportBase::CscCalibReportBase(label),
    pedAmpHists(NULL),
    sampHists(NULL),
    bitHists(NULL),
    bitCorrelation(NULL),
    onlineTHoldTests(NULL)
{ }

CscCalibReportPed::~CscCalibReportPed()
{
    delete pedAmpHists;
    delete bitHists;
    delete bitCorrelation;
    delete onlineTHoldTests;
}

void CscCalibReportPed::setPedAmpHists(DataVector<TH1I> * somePedAmpHists)
{
    pedAmpHists = somePedAmpHists;
}
        
void CscCalibReportPed::setSampHists(DataVector< DataVector<TH1I> > * someSampHists) 
{
  sampHists = someSampHists;
}

void CscCalibReportPed::setBitHists(DataVector<TH1I> * someBitHists)
{
    bitHists = someBitHists;
}

void CscCalibReportPed::setBitCorrelation(DataVector<TH2F> * somebitCorrelation)
{
    bitCorrelation = somebitCorrelation;
}

const DataVector<TH1I> * CscCalibReportPed::getPedAmpHists() const
{
    return pedAmpHists;
}

const DataVector<DataVector<TH1I> > * CscCalibReportPed:: getSampHists() const
{
  return sampHists;
}

const DataVector<TH1I> * CscCalibReportPed::getBitHists() const
{
    return bitHists;
}

const DataVector<TH2F> * CscCalibReportPed::getBitCorrelation() const
{
    return bitCorrelation;
}

//**setOnlineTHoldTests*///
void CscCalibReportPed::setOnlineTHoldTests(std::vector<int> * onlineTests){
  onlineTHoldTests = onlineTests;
}

/**setOnlineTholdTests - contains number of times a channel's sample went above online threshold*/
const std::vector<int> * CscCalibReportPed::getOnlineTHoldTests() const{
  return onlineTHoldTests;
}
