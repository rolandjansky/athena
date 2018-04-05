#include "GaudiKernel/IInterface.h"

#include "ACTS/Extrapolation/ExtrapolationCell.hpp" // for excell and ecode
//#include "ACTS/EventData/NeutralParameters.hpp"
//#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp" // for the parameters
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Surfaces/BoundaryCheck.hpp"
#include "GeomACTS/IExtrapolationTool.h"
#include "GeomACTS/ExtrapolationTool.h"

#include <iostream>

Acts::ExtrapolationTool::ExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : IExtrapolationTool(type, name, parent)
{

}

Acts::ExtrapolationCode
Acts::ExtrapolationTool::extrapolate(Acts::ExCellCharged&       ecCharged,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const 
{

  std::cout << "HALLO CHARGED" << std::endl;

  return ExtrapolationCode(ExtrapolationCode::Unset);

}



Acts::ExtrapolationCode
Acts::ExtrapolationTool::extrapolate(Acts::ExCellNeutral&       ecCharged,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const 
{

  std::cout << "HALLO NEUTRAL" << std::endl;
  return ExtrapolationCode(ExtrapolationCode::Unset);

}
