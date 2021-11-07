// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#include "TrigConfL1Data/CTPConfig.h"
#include "L1TopoConfig/L1TopoMenu.h"

/** @brief Run 2 to Run 3 L1 menu converter
 *  Converts Run 2 L1 menu and L1Topo menu into run 3 format and writes out a json file
 * @param ctpConfig pointer to L1 menu
 * @param topoMenu pointer to L1Topo menu (can be empty)
 * @param filename name of the output json file
 * @param writeTmpFile writes out a temporary version of the json file (before filling the L1Menu structure)
 */
void convertRun2L1MenuToRun3(const TrigConf::CTPConfig* ctpConfig, const TXC::L1TopoMenu * topoMenu,
                             const std::string& filename, bool writeTmpFile = false);

/** @brief Run 2 to Run 3 bunchgroup converter
 * @param ctpConfig pointer to L1 menu (which contains the bunchgroups)
 * @param filename name of the output json file
 */
void convertRun2BunchGroupsToRun3(const TrigConf::CTPConfig* ctpConfig, const std::string& filename, bool writeTmpFile = false);

/** @brief Run 2 to Run 3 L1 prescale converter
 * @param ctpConfig pointer to L1 menu (which contains the prescales)
 * @param filename name of the output json file
 */
void convertRun2L1PrescalesToRun3(const TrigConf::CTPConfig* ctpConfig, const std::string& filename, bool writeTmpFile = false);
