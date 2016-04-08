/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CaloRingsDefs.h 659538 2015-04-08 17:29:18Z wsfreund $ 
#ifndef XAODCALORINGS_CALORINGSDEF_H
#define XAODCALORINGS_CALORINGSDEF_H

namespace Ringer {

/**
 * @brief the joint calorimeter sections.
 *
 * - EM stands for all eletromagnetic layers (included Pre-Sampler);
 * - HAD stands for all hadronic calorimeter layers.
 *
 **/
enum CalJointSection {
  EM,
  HAD,
  NJointSections,
  UnknownJointSection
};

/**
 * @brief the joint calorimeter layers.
 *
 * - PS stands for  pre-sampler barrel and endcap;
 *
 * - EM1 stands for the barrel and endcap of the first eletromagnetic
 *   calorimeter layer.
 * - EM2 stands for the barrel and endcap of the second eletromagnetic
 *   calorimeter layer.
 * - EM3 stands for the barrel and endcap of the third eletromagnetic
 *   calorimeter layer.
 *
 * - HAD1 stands for the first hadronic calorimeter layer of the TileCal
 *   barrel and extended barrel.
 * - HAD2 stands for the second hadronic calorimeter layer of the TileCal
 *   barrel and extended barrel.
 * - HAD3 stands for the third hadronic calorimeter layer of the TileCal
 *   barrel and extended barrel.
 *
 **/
enum CalJointLayer {
  PS,
  EM1,
  EM2,
  EM3,
  HAD1,
  HAD2,
  HAD3,
  NJointLayers,
  UnknownJointLayer
};

} // namespace Ringer

#endif // XAODCALORINGS_CALORINGSDEF_H
