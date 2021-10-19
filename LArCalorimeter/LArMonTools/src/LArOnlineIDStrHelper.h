/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @class LArOnlineIDStrHelper
 * @author Michel Lefebvre <lefebvre@uvic.ca>
 * @date 09-March-2006
 * @brief Helper class to manipulate strings for LArOnlineID names
 *
 * Helper class to manipulate strings for LArOnlineID names:
 * cryostatEnd, feedthroug, feb, channel, partition names
 * Four different types of names are supported: see enum NameType below */

#ifndef LARMONTOOLS_LARONLINEIDSTRHELPER_H
#define LARMONTOOLS_LARONLINEIDSTRHELPER_H

#include "Identifier/HWIdentifier.h"

#include <vector>
#include <string>
#include <map>

class LArOnlineID;
class TAxis;
class TH2;

//NB: Some methods are templated to support both ROOT and LW hists


class LArOnlineIDStrHelper
{
 public:
  /** @brief enum of name types */
  enum NameType {            //               example for feb id 38008000
    DEFAULT            = 0,  // must be min
    EXPERTS            = 1,  //               BarrelCFT09LFront0
    EXPERTSCOMPACT     = 2,  //               EMC09LF0
    LARONLINEID        = 3,  //               BarrelCFT00Slot02
    LARONLINEIDCOMPACT = 4,  //               [4.4.0.0.0.2]
    BEYONDLASTNAMETYPE = 5   // must be max
  };

  /** @brief constructructor */
  LArOnlineIDStrHelper(const LArOnlineID* pLArOnlineIDHelper,
                       NameType defaultNameType = EXPERTS);

  /** @brief default destructor */
  ~LArOnlineIDStrHelper();

  /** @brief set default name type */
  void setDefaultNameType(NameType defaultNameType);

  /** @brief name type string */
  std::string nameType_str(NameType nameType);

  /** @brief get cryostatEnd name */
  std::string cryostatEnd_str(HWIdentifier id,
                              NameType nameType = DEFAULT);

  /** @brief get feedthrough name, as BarrelCFT09L or BarrelCFT00*/
  std::string feedthrough_str(HWIdentifier id,
                              NameType nameType = DEFAULT,
                              bool noSpaces = true);

  /** @brief get feedthrough only name, as 09L or 00*/
  std::string feedthroughOnly_str(HWIdentifier id,
                                  NameType nameType = DEFAULT);

  /** @brief get feb name */
  std::string feb_str(HWIdentifier id,
                      NameType nameType = DEFAULT,
                      bool noSpaces = true);

  /** @brief get feb only name, as Presampler, or Ps, or 01 */
  std::string febOnly_str(HWIdentifier id,
                          NameType nameType = DEFAULT);

  /** @brief get channel name */
  std::string channel_str(HWIdentifier id,
                          NameType nameType = DEFAULT,
                          bool noSpaces = true);

  /** @brief get feedthrough HWId from feedthrough name (case insensitive)*/
  HWIdentifier feedthrough_id(const std::string& feedthroughName);

  /** @brief get feb HWId from feb name (case insensitive)*/
  HWIdentifier feb_id(const std::string& febName);

  /** @brief set the axis bin labels for a feedthrough axis*/
  // labels are feedthrough expert names associated to the relavant feedthroughs
  // works also for variable bin sizes
  // possible detector strings (not case sensitive)
  //  BARRELA, BARRELC, EMBA (EMA), EMBC (EMC)
  //  ENDCAPA, ENDCAPC, EMECA (ECA), EMECC (ECC), HECA, HECC, FCALA, FCALC
  void setAxisBinLabelsFeedthroughs(TAxis* axis,
                                    const std::string& detector);

  /** @brief set the axis bin labels for an all Febs feb axis*/
  // labels are feedthrough expert names associated to the first relevant feb in a feedthrough
  // works also for variable bin sizes
  // possible detector strings (not case sensitive)
  //  BARRELA, BARRELC, EMBA (EMA), EMBC (EMC)
  //  ENDCAPA, ENDCAPC, EMECA (ECA), EMECC (ECC), HECA, HECC, FCALA, FCALC
  template<class Taxis>
  void setAxisBinLabelsAllFebs(Taxis* axis,
                               const std::string& detector);

  /** @brief set the axis bin labels for a feb axis*/
  // labels are experts compact feb names for the desired feedthrough
  // works also for variable bin sizes
  template<class Taxis>
  void setAxisBinLabelsFebs(Taxis* axis,
                            HWIdentifier feedthroughID);


  template<class THist2D>
  StatusCode definePartitionSummProp(THist2D* hist,bool HWNaming = false);
  template<class THist2D>
  StatusCode definePartitionSummProp2(THist2D* hist,bool HWNaming = false);

  /** @brief test function for this class */
  void test();

 private:

  // preclude the use of the default constructor
  LArOnlineIDStrHelper() {};

  // fill feedthrough experts names vector
  void fillFeedthroughExpertsNames();

  // fill feb experts names vectors
  void fillFebExpertsNames();

  // fill feedthrough HWId map, keyed by feedthrough names (all upper case)
  void fillFeedthroughHWIds();

  // fill feb HWId map, keyed by feb names (all upper case)
  void fillFebHWIds();

  // set axis bin labels with feedthrough names
  // common code for setAxisBinLabelsAllfebs and setAxisBinLabelsFeedthroughs
  template<class Taxis>
  void setAxisBinLabelsFeedthroughNames(Taxis* axis,
                                        const std::string& detector,
                                        bool allFebs = false);

  // integer to string converter
  // width <= 0 means no setw used and no leading zeros
  static std::string int_to_str(int n, int width = 0, bool leadingZeros = false);

  // convert a string to an all upper case string
  static std::string str_to_upper(std::string stringToConvert);

  //FindFixBin which works for both root and LW hists:
  template<class Taxis>
  int findFixBin(Taxis*,const double&) const;

  // service
  const LArOnlineID* m_LArOnlineIDHelper;

  // keep track of default name type
  NameType m_defaultNameType;

  // feedthrough experts name string vector, indexed with feedthrough_hash
  std::vector<std::string> m_feedthroughExpertsNames;      // as 09L

  // feb experts name string vector, indexed with feb_hash
  std::vector<std::string> m_febExpertsNames;
  std::vector<std::string> m_febExpertsCompactNames;

  // feedthrough HWId map, keyex with feedthrough names of all types
  std::map<std::string, HWIdentifier> m_feedthroughHWIds;

  // feb HWId map, keyex with feb names of all types
  std::map<std::string, HWIdentifier> m_febHWIds;

};

#include "LArOnlineIDStrHelper.icc"

#endif

