/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGT1CTP_RESULTBUILDER_H
#define TRIGT1CTP_RESULTBUILDER_H

// local include(s):
#include "TrigT1CTP/MsgLogger.h"
#include "TrigT1CTP/ISpecialTrigger.h"

#include <stdint.h>

// For handling different CTP versions:
#include "CTPfragment/CTPdataformatVersion.h"

// Forward declaration(s):
//class CTPdataformatVersion;
namespace TrigConf {
  class CTPConfig;
}
class CTP_RDO;

namespace LVL1CTP {

  // Forward declaration(s) in local namespace:
  class CTPSLink;
  class ThresholdMap;
  class ItemMap;

  /**
   *
   *   @short Class used to create the CTP output (RDO, SLink and TriggerInfo)
   *
   *          This object recieves the extracted information, and
   *          creates the CTP's output to the DAQ system (CTP_RDO) and
   *          to the RoIBuilder (LVL1CTP::CTP_SLink). It also creates
   *          the extended LVL1 ID and the trigger type stored in the
   *          LVL1 part of the TriggerInfo used in the EventInfo
   *          class.
   *
   *  @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   *  @author Wolfgang Ehrenfeld <Wolfgang.Ehrenfeld@desy.de>
   *
   * @version \$Id: ResultBuilder.h,v 1.13 2008-06-10 13:49:28 efeld Exp $
   *
   */

  class ResultBuilder {

  public:
    
    //! constructor setting configuration, decision, items and internal triggers
    ResultBuilder( unsigned int ctpVersionNumber, const TrigConf::CTPConfig* ctpConfig, const ThresholdMap* decisionMap, const ItemMap* itemMap, const InternalTriggerMap* internalTrigger, const uint32_t readoutWindow = 1);
    //! default destructor
    ~ResultBuilder();

    //! build RoI result (LVL1CTP::CTPSLink)
    const CTPSLink* constructRoIResult() const;     
    //! build RDO result (CTP_RDO)
    const CTP_RDO*  constructRDOResult() const;     

    //! get extendedLevel1ID
    uint32_t extendedLevel1ID() const;

    //! get TIP words
    std::vector<uint32_t> tip() const;

    //! get full trigger result before prescale
    std::vector<uint32_t> triggerResult() const { return tav(); };
    //! get full trigger result before prescale
    std::vector<uint32_t> tbp() const;
    //! get full trigger result after prescale
    std::vector<uint32_t> tap() const;
    //! get full trigger result after veto
    std::vector<uint32_t> tav() const;

    //! get trigger type before prescale
    uint32_t triggerType() const { return ttav(); };
    //! get trigger type before prescale
    uint32_t ttbp() const;
    //! get trigger type after prescale
    uint32_t ttap() const;
    //! get trigger type after veto
    uint32_t ttav() const;

    //! read configuration for trigger offsets
    bool readOffsetConfig(const std::string& filename);

  private:

    enum WrdType { 
      TBP, //!< Trigger Before Prescale
      TAP, //!< Trigger After Prescale 
      TAV  //!< Trigger After Veto
    };

    /** map of items and their offsets (in units of BCs). The item
     *  number is the key, the offset to be applied to that item is
     *  the value.
     */
    typedef std::map<unsigned short, int> OffsetMap;

    /** map of fired items and the item and offset pairs that are
     *  supposed to be realised if the item fired. The fired item
     *  number is the key, the value is the map of items and the
     *  offsets that are supposed to be added to the latter
     *  items. Example: when item no 1 fired, item no 4 is supposed to
     *  be delayed by 3 BCs, item no 6 by 1 BC. This would correspond
     *  to an entry in ChangeRulesMap like : 
     *  < key: 1 | value: <
     *                key: 4 | value: 3; 
     *                key: 6 | value: 1
     *                    >
     *  >.
     *  Note: the offsets are always in units of BCs
     */
    typedef std::map<unsigned short, OffsetMap> ChangeRulesMap;   //!< data type for static trigger offset configuration

    /** Introduces artificial offsets in the trigger result words
	in order to simulate not timed-in sub detectors.
     */
    void changeTriggerWords(std::vector<uint32_t>& triggerWords) const;
    
    /** Function that introduces artificial offsets in the RoI
     *  result. In order to mimic real data as closely as possible,
     *  one can introduce a window of BCs into the CTP simulation,
     *  just like the real hardware can perform a readout in a window
     *  around the actual L1A. Based on a text configuration file
     *  (cf. data/TriggerOffsets.cfg), the trigger items that fired a
     *  trigger can be shifted relative to each other, as if a
     *  certain trigger input is not correctly timed in. The RoI of
     *  the CTP contains only the BC of the L1A, hence this function
     *  may only shift certain bits out of the trigger pattern of the
     *  RoI.
     */
    void changeRoIResult(std::vector<uint32_t>& roi_vector) const;     

    /** Function that introduces a readout window and artificial
     *  offsets in the RDO result. In order to mimic real data as
     *  closely as possible, one can introduce a window of BCs into
     *  the CTP simulation, just like the real hardware can perform a
     *  readout in a window around the actual L1A. Based on a text
     *  configuration file (cf. data/TriggerOffsets.cfg), the trigger
     *  items that fired a trigger can be shifted relative to each
     *  other, as if a certain trigger input is not correctly timed
     *  in.
     */
    void  changeRDOResult(CTP_RDO*& result) const;     

    //! build thresholds bits of trigger inputs
    uint32_t constructTIPWord( unsigned int wrd_num ) const;
    //! set fired items in result word
    uint32_t constructResultWord( unsigned int wrd_num, WrdType type ) const;

    //! build LVL1 trigger type for result word
    uint32_t constructTriggerType(const std::vector<uint32_t>& triggerWords) const;
    //! build list of fired items and dump to string
    std::vector<std::string> firedItems(const std::vector<uint32_t>& triggerWords) const;
 
    const TrigConf::CTPConfig* m_ctpConfig;
    const ThresholdMap*        m_decisionMap;
    const ItemMap*             m_itemMap;
    const InternalTriggerMap*  m_internalTrigger;
    unsigned int               m_ctpVersionNumber;
    CTPdataformatVersion*      m_ctpDataFormat; 

    MsgLogger                  m_logger;     //!< instance of private message logger

    std::vector<uint32_t> m_tip;             //!< TIP words
    std::vector<uint32_t> m_tbp;             //!< result words for trigger before prescale
    std::vector<uint32_t> m_tap;             //!< result words for trigger after prescale
    std::vector<uint32_t> m_tav;             //!< result words for trigger after veto

    const uint32_t m_readoutWindow;   //!< size of readout window in BCs

    /** 
     * Configuration for static trigger offsets in case mis-timing
     * offsets are artificially added in the simulation 
     */
    ChangeRulesMap m_changeRules;
    /** Active parser for config file, which is read in recursively. */
    bool (LVL1CTP::ResultBuilder::*m_parseLine)(const std::string& line);
    /** config file parser that finds sections */
    bool findSectionParser(const std::string& line);
    /** config file parser that reads StaticOffsetMap section */
    bool staticOffsetParser(const std::string& line);

  }; // class ResultBuilder


  // typedefs

  // generic typedef for easy access to TIP/TBP/TAP/TAV information
  typedef std::vector<uint32_t> (ResultBuilder::*Result)() const;


  // inline functions

  inline std::vector<uint32_t> ResultBuilder::tip() const { return m_tip; }
  inline std::vector<uint32_t> ResultBuilder::tbp() const { return m_tbp; }
  inline std::vector<uint32_t> ResultBuilder::tap() const { return m_tap; }
  inline std::vector<uint32_t> ResultBuilder::tav() const { return m_tav; }

  inline uint32_t ResultBuilder::ttbp() const { return constructTriggerType(tbp()); }
  inline uint32_t ResultBuilder::ttap() const { return constructTriggerType(tap()); }
  inline uint32_t ResultBuilder::ttav() const { return constructTriggerType(tav()); }


} // namespace LVL1CTP

#endif // TRIGT1CTP_RESULTBUILDER_H
