/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/** @class BCMOverlay

  BCM overlay algorithm
      @author  Jakob Novak <jakob.novak@cern.ch>

*/

#ifndef INDETOVERLAY_BCMOVERLAY_H
#define INDETOVERLAY_BCMOVERLAY_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "InDetBCM_RawData/BCM_RDO_Container.h"

struct BCM_Pulse {
  BCM_Pulse(unsigned int p_, unsigned int w_) {p = p_; w = w_;};
  unsigned int p;
  unsigned int w;
};

class BCMOverlay : public AthReentrantAlgorithm
{
public:

  BCMOverlay(const std::string &name, ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

  StatusCode overlayContainer(const EventContext& ctx,
                              const BCM_RDO_Container *bkgContainer, 
                              const BCM_RDO_Container *signalContainer, 
                              BCM_RDO_Container *outputContainer) const;

private:

  std::unique_ptr<BCM_RawData> mergeChannel(const BCM_RawData *bkgRDO, 
                                            const BCM_RawData *signalRDO) const;

  void overlayPulses(std::vector<std::unique_ptr<BCM_Pulse>>& merged_pulses) const;

  std::pair<BCM_Pulse*, BCM_Pulse*> timeOrder(BCM_Pulse* pulse1, BCM_Pulse* pulse2) const;

  static bool compare(const std::unique_ptr<BCM_Pulse>& a, const std::unique_ptr<BCM_Pulse>& b);

  Gaudi::Property<bool> m_dataOverlay{ this, "isDataOverlay", false, "Flag to switch between MC and data overlay" };
  Gaudi::Property<bool> m_storeAllBCID{ this, "storeAllBCID", false, "Store all BCID" };

  SG::ReadHandleKey<BCM_RDO_Container> m_bkgInputKey{ this, "BkgInputKey", "Bkg_BCM_RDOs", "ReadHandleKey for Background Input BCM_RDO_Container" };
  SG::ReadHandleKey<BCM_RDO_Container> m_signalInputKey{ this, "SignalInputKey", "Sig_BCM_RDOs", "ReadHandleKey for Signal Input BCM_RDO_Container" };
  SG::WriteHandleKey<BCM_RDO_Container> m_outputKey{ this, "OutputKey", "BCM_RDOs", "WriteHandleKey for Output BCM_RDO_Container" };

};

#endif // INDETOVERLAY_BCMOVERLAY_H
