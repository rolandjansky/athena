/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_CTPTRIGGERITEM_H
#define TRIGT1CTP_CTPTRIGGERITEM_H

// Forward include(s):
namespace TrigConf {
  class TriggerItem;
}

namespace LVL1CTP {

  /**
   *
   *   @short Class storing information helping to make the CTP decision
   *
   *          The trigger configuration classes can't contain all the information
   *          that would make creating the CTP decision simple, so I define a few
   *          additional classes, like this one. One such object always belongs to
   *          exactly one TrigConf::TriggerItem object. 
   *          It contains the prescale (value and counter) and (for historic reasons)
   *          the item position.
   *
   *     @see TrigConf::TriggerItem
   *     @see LVL1CTP::ItemMap
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: CTPTriggerItem.h,v 1.3 2007-06-15 16:15:59 efeld Exp $
   *
   */

  class CTPTriggerItem {

  public:
    /// constructor setting trigger item and corresponding prescale
    CTPTriggerItem( const TrigConf::TriggerItem* parent, int prescale );
    /// default destructor
    ~CTPTriggerItem();

    /// Returns pointer to associated TrigConf::TriggerItem object
    const TrigConf::TriggerItem* item() const;

    // Accessor functions:
    int prescale() const;                      //!< get prescale value

    int prescaleCounter() const;               //!< get counter for prescale
    void setPrescaleCounter( int counter );    //!< set counter for prescale

    unsigned int itemPos() const;              //!< get item position as in CTP hardware
    void setItemPos( unsigned int position );           //!< set item position as in CTP hardware

  private:
    const TrigConf::TriggerItem* m_parent;     //!< pointer to associated TrigConf::TriggerItem object
    const int m_prescale;                      //!< prescale value
    int m_counter;                             //!< counter for prescale
    unsigned int m_position;                   //!< item position in CTP hardware

  }; // class CTPTriggerItem

} // namespace LVL1CTP

#endif // TRIGT1CTP_CTPTRIGGERITEM_H
