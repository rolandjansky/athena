/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
// -*- C++ -*-
/**********************************************************************************
 * @Project: HLT
 * @Package: TrigSteeringEvent
 * @class  : Lvl1Result
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

#ifndef TRIGSTEERINGEVENT_Lvl1Result_H
#define TRIGSTEERINGEVENT_Lvl1Result_H

#include <stdint.h>
#include "xAODCore/CLASS_DEF.h"
#include "TrigSteeringEvent/Lvl1Item.h"

namespace LVL1CTP {

  class Lvl1Result
  {

  public:

    // default constructor
    Lvl1Result(bool config = false) : m_configured(config) { }

    bool isConfigured() const { return m_configured; }  //!< is LVL1 configured ?
    bool isAccepted() const;                            //!< final LVL1 decision && isConfigured

    bool anyActiveL1ItemAfterVeto() const;              //!< any LVL1 item passed after prescale, veto ?

    bool isPassedBeforePrescale(unsigned int item) const; //!< raw L1 item
    bool isPassedAfterPrescale(unsigned int item) const;  //!< after prescale
    bool isPassedAfterVeto(unsigned int item) const;      //!< final L1 decision for this item
    bool isPassedRaw(unsigned int item) const;            //!< same as isPassedBeforePrescale

    bool isPrescaled(unsigned int item) const;            //!< item isPassedBeforePrescale and _not_ isPassedAfterPrescale
    bool isVeto(unsigned int item) const;               //!< item isPassedAfterPrescale  and _not_ isPassedAfterVeto

    unsigned int nItems() const { return m_l1_itemsTAV.size()*32; }

    const std::vector<uint32_t>& itemsPassed()          const { return m_l1_itemsTAV; }
    const std::vector<uint32_t>& itemsBeforePrescale()  const { return m_l1_itemsTBP; }
    const std::vector<uint32_t>& itemsAfterPrescale()   const { return m_l1_itemsTAP; }
    const std::vector<uint32_t>& itemsAfterVeto()       const { return m_l1_itemsTAV; }
    //    const std::vector<uint32_t>& itemsPrescaled() const { return m_l1_itemsPrescaled; }

    std::vector<uint32_t>& itemsPassed()           { return m_l1_itemsTAV; }
    std::vector<uint32_t>& itemsBeforePrescale()   { return m_l1_itemsTBP; }
    std::vector<uint32_t>& itemsAfterPrescale()    { return m_l1_itemsTAP; }
    std::vector<uint32_t>& itemsAfterVeto()        { return m_l1_itemsTAV; }


  private:

    bool m_configured;

    /* max 256 LVL1 items => 8 words */
    //    std::vector<uint32_t> m_l1_items;            //!< Trigger level 1, Lvl1 items (=chains)
    //    std::vector<uint32_t> m_l1_itemsPrescaled;   //!< Trigger level 1, Lvl1 items (=chains) after prescale
    std::vector<uint32_t> m_l1_itemsTBP;            //!< Trigger (level 1 items) Before Prescale
    std::vector<uint32_t> m_l1_itemsTAP;            //!< Trigger (level 1 items) After Prescale
    std::vector<uint32_t> m_l1_itemsTAV;            //!< Trigger (level 1 items) After Veto (==final L1 decision)
  };

} // end of namespace


CLASS_DEF( LVL1CTP::Lvl1Result, 21091893, 1)

#endif
