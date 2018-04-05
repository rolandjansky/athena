#ifndef GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H
#define GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"

#include "GeomACTS/IExtrapolationTool.h"

namespace Acts {

class ExtrapolationCode;
class Surface;
class BoundaryCheck;
class ExtrapolationEngine;
class ITrackingGeometrySvc;

class ExtrapolationTool : public IExtrapolationTool
{

public:
  virtual StatusCode initialize() override;

  ExtrapolationTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  virtual ExtrapolationCode
  extrapolate(ExCellCharged&       ecCharged,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const;

  virtual ExtrapolationCode
  extrapolate(ExCellNeutral&       ecNeutral,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const;

private:

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;

  std::unique_ptr<Acts::ExtrapolationEngine> m_exEngine;

};

}


#endif
