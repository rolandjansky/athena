/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelServicesTool/ConvertDynStaveServices.h"

#include "PixelServicesTool/StaveDynServices.h"
#include "PixelServicesTool/SvcCcawUtils.h"

#include <sstream>

ServiceDynMaterial 
ConvertDynStaveServices::convertStaveServices( const StaveDynServices& stServ) const
{
  std::string name = constructName( stServ.type(), stServ.part(), stServ.layer());
  ServiceDynMaterial result( name);

  std::string hvCableName = getHvCableName( stServ.type(), stServ.part(), stServ.layer());
  result.addEntry( hvCableName, stServ.numHV(), true);

  for (int i=0; i< stServ.numLvTypes(); i++) {
    int gauge = stServ.lvGauge(i);
    std::string lvConductorName = getLvConductorName( stServ.type(), stServ.part(), stServ.layer());
    std::string lvInsulatorName = getLvInsulatorName( stServ.type(), stServ.part(), stServ.layer());
    // two cables of each type assumed; if a thicker cable should be split in several thinner ones for engineering reasons
    // this should not change the amount of material significantly.
    result.addEntry( lvConductorName, 2, true, ccaw::conductorWeight(gauge));
    result.addEntry( lvInsulatorName, 2, true, ccaw::insulatorWeight(gauge));
  }

  std::string dcsCableName = getDcsCableName( stServ.type(), stServ.part(), stServ.layer());
  result.addEntry( dcsCableName, stServ.numDCS(), true);

  std::string dataCableName = getDataCableName( stServ.type(), stServ.part(), stServ.layer());
  result.addEntry( dataCableName, stServ.numData(), true);

  // Add shielding and sleeving for thw whole bundle, for bundle diameter 6 mm
  result.addEntry( "ShieldingSleeve", 1, true);
  result.addEntry( "ProtectionSleeve", 1, true);

  return result;
}

inline double area (double R) {return M_PI*R*R;}

inline double pipeWeight( double R, double thick) {
  double Ti_dens = 4.54e3; // kg/m3
  return (area(R+thick)-area(R))*Ti_dens;
}


void ConvertDynStaveServices::addCooling( DetTypeDyn::Type type, DetTypeDyn::Part part, int layerNumber,
				       int nStaves, ServiceDynMaterial& mat) 
{
  int manifoldFactor = 1;

  double pipesPerStave;
  if (layerNumber < 2 && type == DetTypeDyn::Pixel && part == DetTypeDyn::Barrel)  pipesPerStave = 1;
  else pipesPerStave = 2;

  double dInlets = nStaves * pipesPerStave / 2. / manifoldFactor;
  int nInlets = int(dInlets);
  if (dInlets - int(dInlets) > 0.001) nInlets++; // smallest int larger than dInlets

  // Fixme: Hard-wired numbers, only valid if no manifolding
  double inletR = 1.e-3;
  double outletR = 1.e-3;
  if ( type != DetTypeDyn::Pixel) outletR = 1.5e-3;

  mat.addEntry( "std::Titanium", nInlets, true, pipeWeight( inletR, 1.e-4) + pipeWeight( outletR, 1.e-4));

  double collantDensity = 0.6e3; // kg/m3, approx for 60% liquid fraction at -40, ignore liquid fraction diff between inlet and outlet
  mat.addEntry( "pix::CO2_Liquid", nInlets, true, (area(inletR)+area(outletR)) * collantDensity);
}



std::string ConvertDynStaveServices::constructName( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer) const
{
  std::string name = DetTypeDyn::name(type, part) + "Services_L";
  std::ostringstream os;
  os << layer;
  return name + os.str();
}


std::string ConvertDynStaveServices::getHvCableName( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/, int /*layer*/) const
{
  // FIXME: should come from DB?
  return "TwistedPair_awg36"; // for all strip and pixel
}
std::string ConvertDynStaveServices::getDcsCableName( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/, int /*layer*/) const
{
  // FIXME: should come from DB?
  return "ScreenedTwistP_36"; // for all strip and pixel
}
std::string ConvertDynStaveServices::getDataCableName( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/, int /*layer*/) const
{
  // FIXME: should come from DB?
  return "Twinax"; // for all strip and pixel
}

std::string ConvertDynStaveServices::getLvConductorName( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/, int /*layer*/) const
{
  // FIXME: should come from DB?
  return "CCAW";
}
std::string ConvertDynStaveServices::getLvInsulatorName( DetTypeDyn::Type /*type*/, DetTypeDyn::Part /*part*/, int /*layer*/) const
{
  // FIXME: should come from DB?
  return "Polyetherimide";
}
