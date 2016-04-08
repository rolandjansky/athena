/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MINBIASD3PD_UNASSOCIATEDHITSFILLERTOOL_H
#define MINBIASD3PD_UNASSOCIATEDHITSFILLERTOOL_H

#include "D3PDMakerUtils/BlockFillerTool.h"
#include "MinBiasPRDAssociation.h"

namespace D3PD {

typedef MinBiasPRDAssociation UnassociatedHitsFillerObjectType;
typedef D3PD::BlockFillerTool<UnassociatedHitsFillerObjectType> UnassociatedHitsFillerBase;

class UnassociatedHitsFillerTool
  : public UnassociatedHitsFillerBase
{
public:
  UnassociatedHitsFillerTool (const std::string& type,
			      const std::string& name,
			      const IInterface* parent);

  virtual StatusCode initialize();
  virtual StatusCode book();

  virtual StatusCode fill (const UnassociatedHitsFillerObjectType& p);

private:

  void clearData();

  /* NTuple variables: **/
  int *m_nPixelUA;
  int *m_nBlayerUA;
  int *m_nPixelBarrelUA;
  int *m_nPixelEndCapAUA;
  int *m_nPixelEndCapCUA;

  int *m_nSCTUA;
  int *m_nSCTBarrelUA;
  int *m_nSCTEndCapAUA;
  int *m_nSCTEndCapCUA;

  int *m_nTRTUA;
  int *m_nTRTBarrelUA;
  int *m_nTRTEndCapAUA;
  int *m_nTRTEndCapCUA;

}; // class UnassociatedHitsFillerTool

} // namespace D3PD

#endif // not MINBIASD3PD_UNASSOCIATEDHITSFILLERTOOL_H
