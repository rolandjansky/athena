/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// DataPointInfo.cxx
// Richard Hawkings, started 11/1/06
// simple class to store info about datapoint represented in COOL

#include "DataPointInfo.h"

DataPointInfo::DataPointInfo(const std::string& folder,const std::string& column,
    const int channel, const cool::RecordSpecification& atrspec) :
  m_folder(folder), m_column(column), m_channel(channel), m_atrspec(atrspec) 
{}
