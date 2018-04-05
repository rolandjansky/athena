#ifndef GEOMACTS_ACTS_IEXTRAPOLATIONTOOL_H
#define GEOMACTS_ACTS_IEXTRAPOLATIONTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"

namespace Acts {

class ExtrapolationCode;
class Surface;
class BoundaryCheck;

static const InterfaceID IID_IACTSExtrapolationTool("IACTSExtrapolationTool", 1, 0);

class IExtrapolationTool : public AthAlgTool
{

public:

  static const InterfaceID& interfaceID(){return IID_IACTSExtrapolationTool;}

  //DeclareInterfaceID(IExtrapolationTool, 1, 0);
  IExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : AthAlgTool(type, name, parent) {;}
  
  virtual ExtrapolationCode
  extrapolate(ExCellCharged&       ecCharged,
              const Surface*       sf     = 0,
              const BoundaryCheck& bcheck = true) const = 0;

  virtual ExtrapolationCode
  extrapolate(ExCellNeutral&       ecNeutral,
              const Surface*       sf     = 0,
              const BoundaryCheck& bcheck = true) const = 0;

};

}


#endif
