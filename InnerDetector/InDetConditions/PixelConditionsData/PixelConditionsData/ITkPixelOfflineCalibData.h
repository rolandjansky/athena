/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ITkPixelOfflineCalibData.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ITKPIXELOFFLINECALIBDATA_H
#define ITKPIXELOFFLINECALIBDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
#include "PixelConditionsData/ITkPixelClusterErrorData.h"
#include <memory>

/** @class ITk::PixelOfflineCalibData

    The ITk::PixelOfflineCalibData is a class that designed to hold the
    data used by ITk pixel offline algorithms.

*/

namespace ITk
{

class PixelOfflineCalibData
{
public:
  /** Constructor:*/
  PixelOfflineCalibData()
    : m_clusterErrorData(std::make_unique<PixelClusterErrorData>()) {}
  PixelOfflineCalibData(const PixelOfflineCalibData& rhs);
  PixelOfflineCalibData& operator=(const PixelOfflineCalibData& rhs);

  /** default destructor */
  ~PixelOfflineCalibData () = default;

  bool update(const PixelClusterErrorData& idat);

  // get the pointer to pixel cluster error data
  PixelClusterErrorData *getClusterErrorData();
  const PixelClusterErrorData *getClusterErrorData() const;

  std::vector<float> getConstants() const;
  void setConstants(const std::vector<float> &constants);

  void dump();

private:
  std::unique_ptr<PixelClusterErrorData> m_clusterErrorData{};
};


inline PixelOfflineCalibData::PixelOfflineCalibData(const PixelOfflineCalibData& rhs) {
  m_clusterErrorData.reset(new PixelClusterErrorData(*rhs.m_clusterErrorData));
}

inline PixelOfflineCalibData& PixelOfflineCalibData::operator=(const PixelOfflineCalibData& rhs) {
  if (this != &rhs) {
    m_clusterErrorData.reset(new PixelClusterErrorData(*rhs.m_clusterErrorData));
  }
  return (*this);
}

inline bool PixelOfflineCalibData::update(const PixelClusterErrorData& idat) {
  *m_clusterErrorData = idat;
  return true;
}

inline PixelClusterErrorData* PixelOfflineCalibData::getClusterErrorData() {
  return m_clusterErrorData.get();
}

inline const PixelClusterErrorData* PixelOfflineCalibData::getClusterErrorData() const {
  return m_clusterErrorData.get();
}

} // namespace ITk


CLASS_DEF( ITk::PixelOfflineCalibData , 114268426 , 1 )
CLASS_DEF( CondCont<ITk::PixelOfflineCalibData> , 183220670 , 1 )

#endif
