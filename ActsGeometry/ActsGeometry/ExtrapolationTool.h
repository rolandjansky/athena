#ifndef GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H
#define GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GaudiKernel/Property.h"

#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"

#include "ActsGeometry/IExtrapolationTool.h"

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

  virtual
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine() const;

private:

  ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle;
  ServiceHandle<Acts::ITrackingGeometrySvc> m_trackingGeometrySvc;

  std::shared_ptr<Acts::ExtrapolationEngine> m_exEngine;

  Gaudi::Property<std::string> m_fieldMode{this, "FieldMode", "ATLAS"};
  Gaudi::Property<std::vector<double>> m_constantFieldVector{this, "ConstantFieldVector", {0, 0, 0}};
  Gaudi::Property<bool> m_matEngineDoELossCorrection{this, "MatEngineDoELossCorrection", true};

};

}


#endif
