
#include "TrigEgammaEmulationToolMT/TrigEgammaEmulationPrecisionTrackingHypoTool.h"
#include "GaudiKernel/SystemOfUnits.h"


using namespace Trig;

//**********************************************************************

TrigEgammaEmulationPrecisionTrackingHypoTool::TrigEgammaEmulationPrecisionTrackingHypoTool( const std::string& myname )
    : TrigEgammaEmulationBaseHypoTool(myname)
{}



//!==========================================================================


bool TrigEgammaEmulationPrecisionTrackingHypoTool::emulate(   const Trig::TrigData &/*input*/,
                                                          bool &pass ) const
{
  pass = true;
  return true;
 
}

//!==========================================================================






