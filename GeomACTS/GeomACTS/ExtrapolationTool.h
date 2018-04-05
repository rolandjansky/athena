#ifndef GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H
#define GEOMACTS_ACTS_EXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"

#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"

#include "GeomACTS/IExtrapolationTool.h"

namespace Acts {

class ExtrapolationCode;
class Surface;
class BoundaryCheck;

class ExtrapolationTool : public IExtrapolationTool
{

public:
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

};

}


#endif
