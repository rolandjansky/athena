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
    m_pedAmpHists(NULL),
    m_sampHists(NULL),
    m_bitHists(NULL),
    m_bitCorrelation(NULL),
    m_onlineTHoldTests(NULL)
{ }

/* full constructor */
CscCalibReportPed::CscCalibReportPed(std::string label) :  
    CscCalibReportBase::CscCalibReportBase(label),
    m_pedAmpHists(NULL),
    m_sampHists(NULL),
    m_bitHists(NULL),
    m_bitCorrelation(NULL),
    m_onlineTHoldTests(NULL)
{ }

CscCalibReportPed::~CscCalibReportPed()
{
    delete m_pedAmpHists;
    delete m_bitHists;
    delete m_bitCorrelation;
    delete m_onlineTHoldTests;
}

void CscCalibReportPed::setPedAmpHists(DataVector<TH1I> * somePedAmpHists)
{
    m_pedAmpHists = somePedAmpHists;
}
        
void CscCalibReportPed::setSampHists(DataVector< DataVector<TH1I> > * someSampHists) 
{
  m_sampHists = someSampHists;
}

void CscCalibReportPed::setBitHists(DataVector<TH1I> * someBitHists)
{
    m_bitHists = someBitHists;
}

void CscCalibReportPed::setBitCorrelation(DataVector<TH2F> * somebitCorrelation)
{
    m_bitCorrelation = somebitCorrelation;
}

const DataVector<TH1I> * CscCalibReportPed::getPedAmpHists() const
{
    return m_pedAmpHists;
}

const DataVector<DataVector<TH1I> > * CscCalibReportPed:: getSampHists() const
{
  return m_sampHists;
}

const DataVector<TH1I> * CscCalibReportPed::getBitHists() const
{
    return m_bitHists;
}

const DataVector<TH2F> * CscCalibReportPed::getBitCorrelation() const
{
    return m_bitCorrelation;
}

//**setOnlineTHoldTests*///
void CscCalibReportPed::setOnlineTHoldTests(std::vector<int> * onlineTests){
  m_onlineTHoldTests = onlineTests;
}

/**setOnlineTholdTests - contains number of times a channel's sample went above online threshold*/
const std::vector<int> * CscCalibReportPed::getOnlineTHoldTests() const{
  return m_onlineTHoldTests;
}
