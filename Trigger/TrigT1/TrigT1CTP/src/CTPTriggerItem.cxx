/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigConfL1Data/TriggerItem.h"

#include "TrigT1CTP/CTPTriggerItem.h"

namespace LVL1CTP {

  CTPTriggerItem::CTPTriggerItem( const TrigConf::TriggerItem* parent, int prescale )
    : m_parent( parent ), m_prescale( prescale ) {

    m_counter = 0;
    m_position = -1;

  }

  CTPTriggerItem::~CTPTriggerItem() {

  }

  const TrigConf::TriggerItem* CTPTriggerItem::item() const {

    return m_parent;

  }

  int CTPTriggerItem::prescale() const {

    return m_prescale;

  }

  int CTPTriggerItem::prescaleCounter() const {

    return m_counter;

  }

  void CTPTriggerItem::setPrescaleCounter( int counter ) {

    if( counter > m_prescale ) m_counter = 1;
    else m_counter = counter;
    return;

  }

  int CTPTriggerItem::itemPos() const {

    return m_position;

  }

  void CTPTriggerItem::setItemPos( int position ) {

    m_position = position;
    return;

  }

} // namespace LVL1CTP
