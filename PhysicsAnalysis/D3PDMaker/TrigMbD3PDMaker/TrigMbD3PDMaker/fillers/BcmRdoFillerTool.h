/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMBD3PDMAKER_BCMRDOFILLERTOOL_H
#define TRIGMBD3PDMAKER_BCMRDOFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include <vector>

/* for the getter tool **/
#include "D3PDMakerUtils/SGCollectionGetterTool.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"
/************************/

class BCM_RDO_Collection;

namespace TrigMbD3PD {

/* *************************
 * Getter class            *
 * Not needed in principle * 
 ***************************/
class BcmRdoGetterTool
  : public D3PD::SGCollectionGetterTool<BCM_RDO_Container>
{
public:
  BcmRdoGetterTool (const std::string& type,
		    const std::string& name,
		    const IInterface* parent)
    : D3PD::SGCollectionGetterTool<BCM_RDO_Container>
    (type, name, parent) {}
};
/***************************/

typedef BCM_RDO_Collection BcmRdoFillerObjectType;
typedef D3PD::BlockFillerTool<BCM_RDO_Collection> BcmRdoFillerBase;

class BcmRdoFillerTool
  : public BcmRdoFillerBase
{
public:
  BcmRdoFillerTool (const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual StatusCode book();
  virtual StatusCode initialize();

  virtual StatusCode fill (const BcmRdoFillerObjectType& p);

private:

  /* NTuple variables **/
  int* m_bcm_nhits;
  std::vector<int> *m_bcm_channel;
  std::vector<int> *m_bcm_pulse1pos;
  std::vector<int> *m_bcm_pulse1width;
  std::vector<int> *m_bcm_pulse2pos;
  std::vector<int> *m_bcm_pulse2width;
  std::vector<int> *m_bcm_lvl1a;
  std::vector<int> *m_bcm_bcid;
  std::vector<int> *m_bcm_lvl1id;

}; // class BcmRdoFillerTool

} // namespace TrigMbD3PD

#endif // not TRIGMBD3PDMAKER_BCMRDOFILLERTOOL_H
