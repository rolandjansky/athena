/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibData/CscCalibReportBase.h"
#include <string>

using namespace std;

CscCalibReportBase::CscCalibReportBase()
{}

CscCalibReportBase::CscCalibReportBase(std::string aLabel) : 
    m_label(aLabel)
{}

CscCalibReportBase::~CscCalibReportBase()
{}

void CscCalibReportBase::setLabel( const string & aLabel)
{
    m_label = aLabel;
}

string CscCalibReportBase::getLabel() const
{
    return m_label;
}



