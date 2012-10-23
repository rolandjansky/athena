//  ***************************************************************************
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   Author: John Morris (john.morris@cern.ch)                             *
//  *           Queen Mary University of London                               *
//  *                                                                         *
//  ***************************************************************************/

//
// Pure abstract base class interface to L1CaloTTIdTools
//

#ifndef _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOTTIDTOOLS_H_
#define _TRIGGER_TRIGT1_TRIGT1CALOCALIBTOOLINTERFACES_IL1CALOTTIDTOOLS_H_

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"

#include <map>

class HWIdentifier;
class Identifier;
namespace LVL1{
  class InternalTriggerTower;
}

namespace LVL1{

  static const InterfaceID IID_IL1CaloTTIdTools("LVL1::IL1CaloTTIdTools",1,0);

  class IL1CaloTTIdTools : virtual public IAlgTool{
    public:
      static const InterfaceID& interfaceID();

      virtual ~IL1CaloTTIdTools(){};

      virtual int side     (const HWIdentifier& id) = 0;
      virtual int sampling (const HWIdentifier& id) = 0;
      virtual int region   (const HWIdentifier& id) = 0;
      virtual int eta      (const HWIdentifier& id) = 0;
      virtual int phi      (const HWIdentifier& id) = 0;
      virtual int layer    (const HWIdentifier& id) = 0;
      virtual int PPM_type (const HWIdentifier& id) = 0;

      virtual int module_type(const HWIdentifier& id) = 0;
      virtual int barrel_endcap_fcal(const HWIdentifier& id) = 0;

      // to get eta/phi position from the offline TT Id fields
      virtual double IDeta(const Identifier& id) const = 0;
      virtual double IDphi(const Identifier& id) const = 0;

      virtual LVL1::InternalTriggerTower* findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, const Identifier& towerId, bool bCreate = true) = 0;
      virtual LVL1::InternalTriggerTower* findInternalTriggerTower(std::map<unsigned int, LVL1::InternalTriggerTower*>* pIntTTContainer, double tt_phi, double tt_eta, unsigned int key, bool bCreate = true) = 0;


      virtual double etaWidth(double eta) const = 0;
      virtual double phiWidth(double eta) const = 0;
      virtual int pos_neg_z(double eta) const = 0;
      virtual int regionIndex(double eta) const = 0;
      virtual int etaIndex(double eta) const = 0;
      virtual int phiIndex(double eta, double phi) const = 0;
 
  };

  inline const InterfaceID& IL1CaloTTIdTools::interfaceID(){
    return IID_IL1CaloTTIdTools;
  }    

} // end of namespace
#endif
