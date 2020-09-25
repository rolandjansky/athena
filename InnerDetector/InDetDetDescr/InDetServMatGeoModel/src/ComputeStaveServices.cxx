/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetServMatGeoModel/ComputeStaveServices.h"

#include "InDetServMatGeoModel/CcawUtils.h"

#include <iostream>
using namespace std;


StaveServices ComputeStaveServices::compute( DetType::Type type, DetType::Part part, int layerNumber,
					     int nModulesPerStave, int nChipsPerModule,
                                             MsgStream& msg) const

{
  msg << MSG::DEBUG << "Computing services for " << DetType::name( type, part) << " layer " << layerNumber 
		  << " with " << nModulesPerStave << " modulesPerStave and " 
		  << nChipsPerModule << " chipsPerModule" << endmsg;
  
  // The input parameters to the calculation of services per stave.
  // FIXME: They must be eventually stored in the DB, and not hard-wired in the code

  const int HV_grouping = 4;  // FIXME: hard-wired number
  const int nDCS_Pixel = 10;  // FIXME: hard-wired number
  const int nDCS_Strip = 10;  // FIXME: hard-wired number
  const int max_SP_group_size = 8;  // FIXME: hard-wired number

  // HV part
  int nHV = nModulesPerStave / HV_grouping;
  if (nModulesPerStave % HV_grouping != 0) ++nHV;

  // DCS part
  int nDCS;
  if ( type == DetType::Pixel) {
    nDCS = nDCS_Pixel;  // FIXME
  }
  else {
    nDCS = nDCS_Strip;
  }

  // Data part
  int Data_grouping = 10;  // FIXME: hard-wired number
  if (layerNumber > 2) Data_grouping = 20;  // FIXME: hard-wired number
  int nChips = nModulesPerStave * nChipsPerModule;
  int nData = nChips/Data_grouping;
  if (nChips % Data_grouping != 0) ++nData;

  // LV part
  double chipCurrent = 0.5; // FIXME: hard-wired number
  double chipVoltage = 2.4; // FIXME: hard-wired number
  double poweringLoss = 0.2; // FIXME: hard-wired number
  double lossInCable = 0.1; // FIXME: hard-wired number
  double cableLen = 3.0; // FIXME: hard-wired number

  double moduleCurrent = chipCurrent * nChipsPerModule; // chips are powered in parallel inside a pixel module
  double moduleVoltage = chipVoltage;

  int nGroups = 1;
  int nModules = nModulesPerStave;

  if (nModulesPerStave > max_SP_group_size) {
    nGroups = nModulesPerStave / max_SP_group_size;
    if (nModulesPerStave % max_SP_group_size != 0) nGroups++;
    nModules =  max_SP_group_size;
    msg << MSG::DEBUG  << "Using " << nGroups << " powering groups with " << max_SP_group_size << " modules each" << endl;
  }

  int lvGauge = computeLVGaugeSerial( type, part, layerNumber, 
				      nModules, moduleCurrent, moduleVoltage,
				      poweringLoss, lossInCable, cableLen,
                                      msg);

  std::vector<int> lvg( nGroups, lvGauge);
  return StaveServices( type, part, layerNumber, nHV, nDCS, nData, lvg);
}
/*
int computeLVGauge( int nModules, double moduleCurrent, double moduleVoltage, int poweringScheme, 
		    double poweringLoss, double lossInCable, double cableLen)
{
  // poweringSheme: 0 for serial, 1 for DC-DC
  if (poweringScheme == 0) { 
    // serial powering
    double cableCurrent = moduleCurrent;
    double staveVoltage = moduleVoltage * nModules / (1.-poweringLoss);
    double stavePower = cableCurrent*staveVoltage;
    double cablePowerDissipation = stavePower * lossInCable;
    double cableResistivity = cablePowerDissipation / (cableCurrent*cableCurrent);
    double resistivityPerMeter = cableResistivity / (2*cableLen);

  }
}
*/
int ComputeStaveServices::computeLVGaugeSerial( DetType::Type type, DetType::Part part, int layerNumber,
						int nModules, double moduleCurrent, double moduleVoltage,
						double poweringLoss, double lossInCable, double cableLen,
                                                MsgStream& msg) const
{
  /*
  cout << "computeLVGaugeSerial: nModules = " << nModules
       << " moduleCurrent " << moduleCurrent
       << " moduleVoltage " << moduleVoltage
       << " poweringLoss " << poweringLoss
       << " lossInCable " << lossInCable
       << " cableLen " << cableLen 
       << endl;
  */
  // serial powering
  double cableCurrent = moduleCurrent;
  double staveVoltage = moduleVoltage * nModules / (1.-poweringLoss);
  double stavePower = cableCurrent*staveVoltage;
  double cablePowerDissipation = stavePower * lossInCable;
  if (cablePowerDissipation < 2.) cablePowerDissipation = 2.;  // we can always accept 2 W power loss in cable

  double cableResistivity = cablePowerDissipation / (cableCurrent*cableCurrent);
  double resistivityPerMeter = cableResistivity / (2*cableLen);

  int gauge = ccaw::closestResistivityCcawGauge( resistivityPerMeter);

  msg << MSG::DEBUG  << DetType::name( type, part) << " layer " << layerNumber
                     << " SP loop power " << stavePower << " [W], current " << moduleCurrent 
    // << " [A], cablePowerLoss " << cablePowerDissipation 
		  << " [W], desired resistivity " << resistivityPerMeter 
		  << " [Ohm/m], closest gauge " << gauge << endl;

  return gauge;
}

