//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILECONDITIONS_TILEDCSSTATE_H
#define TILECONDITIONS_TILEDCSSTATE_H

#include "TileConditions/Exception.h"

#include <vector>
#include <string>

/**
 * @class TileDCSState
 * @brief Condition object to keep Tile DCS status from DB
 *
 * See <a href="https://twiki.cern.ch/twiki/bin/view/Atlas/TileCalorimeterDCS">
 *      Tile Calorimeter DCS on the twiki
 *     </a>
 */

class TileDCSState {

  public:

   /**
    * @enum TileDCSStatus
    * @brief Describes Tile DCS status
    */
    enum TileDCSStatus {NON_EXISTING = 0, UNKNOWN = 1,
                        OK_DRAWER = 10, OK_KNOWNBADPMT, OK,
                        WARNING_DRAWER = 20, WARNING_KNOWNBADPMT, WARNING,
                        ALERT_DRAWER = 30, ALERT_KNOWNBADPMT, ALERT};

    TileDCSState();
    virtual ~TileDCSState() {};

   /**
    * @brief Return measured HV reported by DCS for given Tile channel
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return measured HV reported by DCS for given Tile channel
    */
    float getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const;

   /**
    * @brief Store measured HV reported by DCS for given Tile channel
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @param hv measured HV reported by DCS for given Tile channel
    */
    void setChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel, float hv);

   /**
    * @brief Return requested HV reported by DCS for given Tile channel
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return requested HV reported by DCS for given Tile channel
    */
    float getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const;

   /**
    * @brief Store requested HV reported by DCS for given Tile channel
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @param hvSet requested HV reported by DCS for given Tile channel
    */
    void setChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel, float hvSet);

   /**
    * @brief Return Tile drawer summary states per LVPS reported by DCS
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @return Tile drawer states reported by DCS
    */
    int getDrawerStates(unsigned int ros,  unsigned int drawer) const;

   /**
    * @brief Store Tile drawer summary states per LVPS reported by DCS
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param states Tile drawer states reported by DCS
    */
    void setDrawerStates(unsigned int ros,  unsigned int drawer, int states);

   /**
    * @brief Return TileDCSstatus for given Tile channel
    *        determined by deviation between measured and requested HV
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return TileDCSstatus for given Tile channel
    *         determined by deviation between measured and requested HV
    */
    TileDCSStatus getDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const;

   /**
    * @brief Store TileDCSstatus for given Tile channel
    *        determined by deviation between measured and requested HV
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @param status TileDCSstatus for given Tile channel
    *                  determined by deviation between measured and requested HV
    */
    void setDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel, TileDCSStatus status);

   /**
    * @brief Return TileDCSstatus for given Tile drawer
    *        determined by summary states per LVPS
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @return TileDCSstatus for given Tile drawer
    */
    TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer) const;

   /**
    * @brief Return TileDCSstatus for given Tile channel
    *        determined by summary states per LVPS
    *        and deviation between measured and requested HV
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return TileDCSstatus for given Tile channel
    */
    TileDCSStatus getDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel) const;

   /**
    * @brief Store TileDCSstatus for given Tile channel
    *        determined by summary states per LVPS
    *        and deviation between measured and requested HV
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @param status TileDCSstatus for given Tile channel
    */
    TileDCSStatus setDCSStatus(unsigned int ros, unsigned int drawer, unsigned int channel, TileDCSStatus status);

   /**
    * @brief Return @c true if given Tile channel considered as bad
    *               by deviation between measured and requested HV
    *               otherwise return false
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return @c TileDCSstatus for given channel
    */
    bool isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const;

   /**
    * @brief Return @c true if given Tile drawer considered as bad
    *               by summary drawer states per LVPS
    *               otherwise return false
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @return @c TileDCSstatus for given channel
    */

    bool isStatusBad (unsigned int ros, unsigned int drawer) const;

   /**
    * @brief Return @c true if given Tile channel considered as bad
    *               by summary drawer states per LVPS
    *               and deviation between measured and requested HV
    *               otherwise return false
    * @param ros Tile ROS number: 1 == LBA, 2 == LBC, 3 == EBA, 4 == EBC
    * @param drawer Tile drawer number in range [0..63]
    * @param channel Tile channel number in range [0..47]
    * @return @c TileDCSstatus for given channel
    */
    bool isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const;

   /**
    * @brief Store good Tile drawer summary states per LVPS as reference
    * @param goodDrawer Tile drawer states considered as good
    */
    void setGoodDrawer(int goodDrawer);

   /**
    * @brief Store vector of warning Tile drawer summary states per LVPS as reference
    * @param warningDrawer vector of Tile drawer states considered as warning
    */
    void setWarningDrawer(const std::vector<int>& warningDrawer);

  private:

    enum TileLimits {
      NUMBER_OF_ROSES = 4, //!< Number of Tile ROSes
      NUMBER_OF_CHANNELS = 48, //!< Number of Tile channels
      NUMBER_OF_HV_CHANNELS = 55, //!< Number of Tile "channels" in DCS HV folder
      NUMBER_OF_HVSET_CHANNELS = 68, //!< Number of Tile "channels" in DCS HVSET folder
      NUMBER_OF_DRAWERS = 64 //!< Number of Tile drawers in one ROS
    };

    void checkDrawer(std::string description, unsigned int ros, unsigned int drawer) const;
    void checkChannel(std::string description, unsigned int ros, unsigned int drawer,
                     unsigned int channel, unsigned int maxChannel) const;

    int m_states[NUMBER_OF_ROSES][NUMBER_OF_DRAWERS];
    float m_hv[NUMBER_OF_ROSES][NUMBER_OF_DRAWERS][NUMBER_OF_HV_CHANNELS];
    float m_hvSet[NUMBER_OF_ROSES][NUMBER_OF_DRAWERS][NUMBER_OF_HVSET_CHANNELS];
    TileDCSStatus m_status[NUMBER_OF_ROSES][NUMBER_OF_DRAWERS][NUMBER_OF_CHANNELS];
    TileDCSStatus m_hvStatus[NUMBER_OF_ROSES][NUMBER_OF_DRAWERS][NUMBER_OF_CHANNELS];

    int m_goodDrawer;
    std::vector<int> m_warningDrawer;
    int m_unknownValue;

};


#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

CLASS_DEF(TileDCSState, 125804125, 0)
CONDCONT_MIXED_DEF(TileDCSState, 38603143);

// inlines

inline
float TileDCSState::getChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel) const {

  checkChannel("TileDCSState::getChannelHV()", ros, drawer, channel, NUMBER_OF_HV_CHANNELS);

  return m_hv[ros - 1][drawer][channel];
}

inline
void TileDCSState::setChannelHV(unsigned int ros,  unsigned int drawer,  unsigned int channel, float hv) {

  checkChannel("TileDCSState::setChannelHV()", ros, drawer, channel, NUMBER_OF_HV_CHANNELS);

  m_hv[ros - 1][drawer][channel] = hv;
}

inline
float TileDCSState::getChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel) const {

  checkChannel("TileDCSState::getChannelHVSet()", ros, drawer, channel, NUMBER_OF_HVSET_CHANNELS);

  return m_hvSet[ros - 1][drawer][channel];  // Return the HVSET
}

inline
void TileDCSState::setChannelHVSet(unsigned int ros,  unsigned int drawer,  unsigned int channel, float hvSet) {

  checkChannel("TileDCSState::setChannelHVSet()", ros, drawer, channel, NUMBER_OF_HVSET_CHANNELS);

  m_hvSet[ros - 1][drawer][channel] = hvSet;  // Set the HVSET
}

inline
int TileDCSState::getDrawerStates(unsigned int ros,  unsigned int drawer) const {

  checkDrawer("TileDCSState::getDrawerStates()", ros, drawer);

  return m_states[ros - 1][drawer];
}

inline
void TileDCSState::setDrawerStates(unsigned int ros,  unsigned int drawer, int states) {

  checkDrawer("TileDCSState::setDrawerStates()", ros, drawer);

  m_states[ros - 1][drawer] = states;  // Set the drawer state
}

inline
TileDCSState::TileDCSStatus TileDCSState::getDCSHVStatus(unsigned int ros,
                                                         unsigned int drawer,
                                                         unsigned int channel) const {

  checkChannel("TileDCSState::getDCSHVStatus()", ros, drawer, channel, NUMBER_OF_CHANNELS);

  return m_hvStatus[ros - 1][drawer][channel];
}

inline
void TileDCSState::setDCSHVStatus(unsigned int ros, unsigned int drawer, unsigned int channel,
                                  TileDCSState::TileDCSStatus status) {

  checkChannel("TileDCSState::setDCSHVStatus()", ros, drawer, channel, NUMBER_OF_CHANNELS);

  m_hvStatus[ros - 1][drawer][channel] = status;
}

inline
TileDCSState::TileDCSStatus TileDCSState::getDCSStatus(unsigned int ros,
                                                       unsigned int drawer,
                                                       unsigned int channel) const {

  checkChannel("TileDCSState::getDCSStatus()", ros, drawer, channel, NUMBER_OF_CHANNELS);

  return m_status[ros - 1][drawer][channel]; // final status - the worst of 2 statuses above
}

inline
TileDCSState::TileDCSStatus TileDCSState::setDCSStatus(unsigned int ros,
                                                       unsigned int drawer,
                                                       unsigned int channel,
                                                       TileDCSState::TileDCSStatus status) {

  checkChannel("TileDCSState::setDCSStatus()", ros, drawer, channel, NUMBER_OF_CHANNELS);

  return m_status[ros - 1][drawer][channel] = status; // final status - the worst of 2 statuses above
}

inline
bool TileDCSState::isStatusHVBad(unsigned int ros, unsigned int drawer, unsigned int channel) const {
  return getDCSHVStatus(ros, drawer, channel) > WARNING;
}

inline
bool TileDCSState::isStatusBad (unsigned int ros, unsigned int drawer) const {
  return getDCSStatus(ros, drawer) > WARNING;
}

inline
bool TileDCSState::isStatusBad(unsigned int ros, unsigned int drawer, unsigned int channel) const {

  checkChannel("TileDCSState::isStatusBad()", ros, drawer, channel, NUMBER_OF_CHANNELS);

  return m_status[ros - 1][drawer][channel] > WARNING;
}

inline
void TileDCSState::setGoodDrawer(int goodDrawer) { m_goodDrawer = goodDrawer;}

inline
void TileDCSState::setWarningDrawer(const std::vector<int>& warningDrawer) {
  m_warningDrawer = warningDrawer;
}

inline
void TileDCSState::checkDrawer(std::string description,
                               unsigned int ros, unsigned int drawer) const {

  if (ros > NUMBER_OF_ROSES) {
    throw TileCalib::IndexOutOfRange(description, ros, NUMBER_OF_ROSES);
  }

  if (drawer >= NUMBER_OF_DRAWERS) {
    throw TileCalib::IndexOutOfRange(description, drawer, NUMBER_OF_DRAWERS);
  }

}


inline
void TileDCSState::checkChannel(std::string description,
                                unsigned int ros, unsigned int drawer,
                                unsigned int channel, unsigned int maxChannel) const {

  checkDrawer(description, ros, drawer);

  if (channel >= maxChannel) {
    throw TileCalib::IndexOutOfRange(description, channel, maxChannel);
  }

}

#endif // TILECONDITIONS_TILEDCSSTATE_H
