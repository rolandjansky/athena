/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef T0CALIBRATIONOUTPUT_H
#define T0CALIBRATIONOUTPUT_H

#include "MdtCalibInterfaces/IMdtCalibrationOutput.h"
#include "MuonCalibStandAloneBase/NtupleStationId.h"
#include <map>

namespace MuonCalib{
class MdtTubeFitContainer;
/** 
@class T0CalibrationOutput
class for the communication of the results of T0 calibration algorithms 
*/
class T0CalibrationOutput : public IMdtCalibrationOutput {
 public:
  /** constructor (from a pointer to an MdtTubeFitContainer) */
  T0CalibrationOutput( MdtTubeFitContainer* t0Vec ) : 
  IMdtCalibrationOutput("T0CalibrationOutput"), m_tubeConstants(t0Vec) {}
  T0CalibrationOutput(const std::map<NtupleStationId, MdtTubeFitContainer*> & t0Map ) : 
  IMdtCalibrationOutput("T0CalibrationOutput"), m_tubeConstants(NULL), m_tubeConstants_map(t0Map) {}

  /** @return the pointer to the MdtTubeFitContainer with the fit results */
  MdtTubeFitContainer* t0s() const { return m_tubeConstants; }
  std::map<NtupleStationId, MdtTubeFitContainer*> & GetMap()
  	{
	return m_tubeConstants_map;
	}
  const std::map<NtupleStationId, MdtTubeFitContainer*> & GetMap() const
  	{
	return m_tubeConstants_map;
	}

 private:
  // pointer to a MdtTubeFitContainer instance
  MdtTubeFitContainer* m_tubeConstants;
  std::map<NtupleStationId, MdtTubeFitContainer*> m_tubeConstants_map;
  
};
}
#endif
