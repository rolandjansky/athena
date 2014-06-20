/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_MBTSLVL2FILLERTOOL_H
#define TRIGMBD3PDMAKER_MBTSLVL2FILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

class TrigT2MbtsBitsContainer;

/** 
 * A class to dump the persistent form that was in use from early 2010 data taking.
 */

namespace TrigMbD3PD {

typedef TrigT2MbtsBitsContainer MbtsLvl2FillerObjectType;
typedef D3PD::BlockFillerTool<MbtsLvl2FillerObjectType> MbtsLvl2FillerBase;

class MbtsLvl2FillerTool
  : public MbtsLvl2FillerBase
{
public:
  MbtsLvl2FillerTool (const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const MbtsLvl2FillerObjectType& p);

private:

  void clearData();

  /* NTuple variables **/
  unsigned int *m_nElements;
  std::vector<float> *m_triggerEnergies;
  unsigned int *m_trigger_ntimes;
  std::vector<float>* m_triggerTimes;

}; // class MbtsLvl2FillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_MBTSLVL2FILLERTOOL_H
