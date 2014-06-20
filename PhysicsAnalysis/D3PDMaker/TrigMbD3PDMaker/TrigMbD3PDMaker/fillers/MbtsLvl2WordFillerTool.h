/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_MBTSLVL2WORDFILLERTOOL_H
#define TRIGMBD3PDMAKER_MBTSLVL2WORDFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"

class TrigT2MbtsBitsContainer;

/**
 * A class to dump the older version of the persistent class, used
 * during 2009 and early 2010.
 */

namespace TrigMbD3PD {
  
  typedef TrigT2MbtsBitsContainer MbtsLvl2WordFillerObjectType;
  typedef D3PD::BlockFillerTool<MbtsLvl2WordFillerObjectType> MbtsLvl2WordFillerBase;
  
  class MbtsLvl2WordFillerTool  : public MbtsLvl2WordFillerBase {
  public:
    MbtsLvl2WordFillerTool (const std::string& type,
			    const std::string& name,
			    const IInterface* parent);
    
    virtual StatusCode initialize();
    virtual StatusCode book();
    
    virtual StatusCode fill (const MbtsLvl2WordFillerObjectType& p);
    
  private:
    
    void clearData();
    
    /* NTuple variables **/
    unsigned int *m_nElements;
    unsigned int *m_mbtsWord;
    unsigned int *m_trigger_ntimes;
    std::vector<float>* m_triggerTimes;
    
  }; // class MbtsLvl2WordFillerTool
  
} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_MBTSLVL2WORDFILLERTOOL_H
