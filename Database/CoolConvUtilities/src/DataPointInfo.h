/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COOLCONVUTILS_DATAPOINTINFO_H
#define COOLCONVUTILS_DATAPOINTINFO_H
// DataPointInfo.h - simple class to hold info about DCS datapoint represented
// in COOL
// Richard Hawkings, started 11/1/06

#include <string>
#include "CoolKernel/RecordSpecification.h"

class DataPointInfo {
 public:
  DataPointInfo(const std::string folder, const std::string column,
  const int channel,const cool::RecordSpecification& atrspec);
  const std::string folder() const;
  const std::string column() const;
  const int channel() const;
  const cool::IRecordSpecification& atrspec() const;

 private:
  std::string m_folder;
  std::string m_column;
  int m_channel;
  const cool::RecordSpecification m_atrspec;
};

inline const std::string DataPointInfo::folder() const {return m_folder;}
inline const std::string DataPointInfo::column() const {return m_column;}
inline const int DataPointInfo::channel() const { return m_channel;}
inline const cool::IRecordSpecification& DataPointInfo::atrspec() const 
   { return m_atrspec;}

#endif // COOLCONVUTILS_DATAPOINTINFO_H
