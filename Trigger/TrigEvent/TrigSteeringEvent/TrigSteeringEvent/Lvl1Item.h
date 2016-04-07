/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
// -*- C++ -*-
/**********************************************************************************
 * @Project: HLT
 * @Package: TrigSteeringEvent
 * @Class  : Lvl1Result
 *
 * @brief container for compact version of the results of all three trigger levels
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Ricardo Goncalo <Jose.Goncalo@cern.ch>    - Royal Holloway, U. of London
 *
 * File and Version Information:
 * $Id: Lvl1Result.h,v 1.8 2008-05-15 19:17:06 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERINGEVENT_Lvl1Item_H
#define TRIGSTEERINGEVENT_Lvl1Item_H

#include <stdint.h>
#include <string>
#include <vector>
#include "xAODCore/CLASS_DEF.h"

namespace LVL1CTP {

  /**
   * \brief
   *
   */

  class Lvl1Item {

  public:

    Lvl1Item( std::string n, unsigned int hash, bool passBP = false,
              bool passAP = true, bool passAV = true, float factor = 1) :
      m_name(n), m_hashId(hash), m_passBP(passBP), m_passAP(passAP), m_passAV(passAV),
      m_prescaleFactor(factor) {}

    std::string  name()   const { return m_name; }
    unsigned int hashId() const { return m_hashId; }

    bool isPassedBeforePrescale() const    { return m_passBP; }
    bool isPassedAfterPrescale()  const    { return m_passAP; }
    bool isPassedAfterVeto()      const    { return m_passAV; }

    bool isPassed()      const    { return isPassedAfterVeto(); }

    bool isPrescaled() const { return isPassedBeforePrescale() && ! isPassedAfterPrescale(); }
    bool isVeto() const      { return isPassedAfterPrescale()  && ! isPassedAfterVeto();     }

    float prescaleFactor() const { return m_prescaleFactor; }

    void setName  (std::string name) { m_name = name; }
    void setHashId(unsigned int hash) { m_hashId = hash; }
    void setPrescaleFactor(float factor) { m_prescaleFactor = factor; }

  private:

    std::string m_name;
    unsigned int m_hashId;
    bool m_passBP, m_passAP, m_passAV;
    float m_prescaleFactor;
  };

} // end of namespace

#endif
