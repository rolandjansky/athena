/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCRAWDATAMONITORING_ITGCRAWDATAMONITORTOOL_H
#define TGCRAWDATAMONITORING_ITGCRAWDATAMONITORTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TgcChamber.h"
#include "TgcHit.h"
#include "GeoPrimitives/GeoPrimitives.h"

class ITgcRawDataMonitorTool: public virtual IAlgTool {
 public:
  virtual ~ITgcRawDataMonitorTool() = default;
  DeclareInterfaceID( ITgcRawDataMonitorTool, 1, 0);

  virtual std::set<std::string> getPassedChambers(const Amg::Vector3D& extVec) const = 0;
  virtual bool getMapIndex(const TGC::TgcHit& tgcHit,
			   int& etamapindex, int& phimapindex, int& globalphimapindex ) const = 0;
  virtual bool getMapIndex(const TGC::TgcChamber& tgcCham, int ilay,
			   int& etamapindex, int& phimapindex, int& globalphimapindex ) const = 0;
};

#endif
