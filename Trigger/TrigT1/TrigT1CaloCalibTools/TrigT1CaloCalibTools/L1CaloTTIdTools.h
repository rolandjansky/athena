/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOTTIDTOOLS_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLS_L1CALOTTIDTOOLS_H_

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TTOnlineID.h"

#include "TrigT1CaloUtils/InternalTriggerTower.h"
#include "TrigT1CaloUtils/TriggerTowerKey.h"

#include "TrigT1CaloCalibToolInterfaces/IL1CaloTTIdTools.h"

#include <iostream>
#include <cmath>

/**
  * AlgTool that implement several methods
  * used to produced the On/Off and On/Attr
  * trigger tower Id maps.
  *
  *  @author Damien Prieur <damien.prieur@cern.ch>
  *  @author John Morris <john.morris@cern.ch>
  */

namespace LVL1{

  class L1CaloTTIdTools : virtual public IL1CaloTTIdTools,public AthAlgTool
  {
  public:
  // constructor
    L1CaloTTIdTools(const std::string& type, const std::string& name, const IInterface* parent );
    virtual ~L1CaloTTIdTools();

    virtual StatusCode initialize();
    virtual StatusCode finalize();

  // from harcoded algorithms
      int side     (const HWIdentifier& id);
      int sampling (const HWIdentifier& id);
      int region   (const HWIdentifier& id);
      int eta      (const HWIdentifier& id);
      int phi      (const HWIdentifier& id);
      int layer    (const HWIdentifier& id);
      int PPM_type (const HWIdentifier& id);

      int module_type(const HWIdentifier& id);
      int barrel_endcap_fcal(const HWIdentifier& id);

  // to get eta/phi position from the offline TT Id fields
      double IDeta(const Identifier& id) const;
      double IDphi(const Identifier& id) const;

      LVL1::InternalTriggerTower* findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, const Identifier& towerId, bool bCreate = true);
      LVL1::InternalTriggerTower* findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, double tt_phi, double tt_eta, unsigned int key, bool bCreate = true);


      double etaWidth(double eta) const;
      double phiWidth(double eta) const;
      int pos_neg_z(double eta) const;
      int regionIndex(double eta) const;
      int etaIndex(double eta) const;
      int phiIndex(double eta, double phi) const;

  private:

      StoreGateSvc* m_detStore;

      const TTOnlineID*  m_l1ttonlineHelper;
      const CaloLVL1_ID* m_l1ttofflineHelper;

      bool m_bInitialized;


      static const double BASEDETA;
      static const double BASEDPHI;
      static const double FCALDETA;

      static const double ETAMAXREGION0;
      static const double ETAMAXREGION1;
      static const double ETAMAXREGION2;
      static const double ETAMAXREGION3;

      static const double ETAMIN;
      static const double PHIMIN;

  };
} // end of namespace
#endif
