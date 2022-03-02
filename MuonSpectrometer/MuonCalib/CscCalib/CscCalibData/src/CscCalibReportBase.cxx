/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "CscCalibData/CscCalibReportBase.h"
#include <string>
#include <utility>

CscCalibReportBase::CscCalibReportBase()
{}

CscCalibReportBase::CscCalibReportBase(std::string aLabel) : 
    m_label(std::move(aLabel))
{}

CscCalibReportBase::~CscCalibReportBase()=default;

void CscCalibReportBase::setLabel(const std::string & aLabel)
{
    m_label = aLabel;
}

std::string CscCalibReportBase::getLabel() const
{
    return m_label;
}
