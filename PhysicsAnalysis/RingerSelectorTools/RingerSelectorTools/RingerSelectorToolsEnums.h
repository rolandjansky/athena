/*
f  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RINGERSELECTORTOOLS_RINGERSELECTORTOOLSENUMS_H
#define RINGERSELECTORTOOLS_RINGERSELECTORTOOLSENUMS_H

/**
 *
 * WARNING: Do not change the order of the enumerations for backward files
 * compatibility!!
 *
 * This means that if a new value is needed, add it to the end (before the Nxxx
 * enumeration) of the enumeration, instead of adding it to any place in the
 * enumeration.
 *
 * After adding a enumeration type, also add it to the methods in the
 * RingerSelectorToolsDefs.cxx file.
 *
 **/

/**
 * @brief Namespace dedicated for Ringer utilities
 **/
namespace Ringer {


/**
 * @brief the possible procedure longitudinal segmentation types.
 *
 * - NoSegmentation stands for a unique representation for all data. This may
 *   mean that the procedure:
 *
 *    o is not applied for a pattern segment;
 *
 *    o has a specialized implemented approach that works in distinct depths of
 *    the segmented information.
 *
 *    o cannot be applied in a segmentation approach. This is the case whenever
 *    the procedure is not applied in the first layer or if the procedure is
 *    the application of a threshold. In another words, only the first
 *    PreProcessor and the first discriminator may have access to segmented
 *    information, the later only if its pre-processing chain does not affect
 *    the output segmentation.
 *
 * - TrackCalSegmentation will segmentate track (if information
 *   available) from calorimeter.
 *
 * - TrackCalPatTypeSegmentation will segmentate track (if information
 *   available) from calorimeter, the latter also being segmented between the
 *   standard calorimeter patterns and rings.
 *
 * - TrackCalJointSections will segmentate track (if information available)
 *   from calorimeter. Besides, the calorimeter information will be segmentated
 *   through CalJointSections (see xAODCaloRings/CaloRingsDefs.h).
 *
 * - TrackCalJointLayers will segmentate track (if information available) from
 *   calorimeter. Besides, the calorimeter information will be segmentated
 *   through CalJointLayers (see xAODCaloRings/CaloRingsDefs.h).
 *
 * If adding a new value on this enumeration, add it to the numberOfSegments
 * function on RingerSelectorToolsDefs.cxx
 **/
enum SegmentationType {
  NoSegmentation = 0,
  TrackCalSegmentation,
  TrackCalPatTypeSegmentation,
  TrackCalJointSections,
  TrackCalJointLayers,
  NSegmentations
};

/**
 * @brief Specify eta position dependency
 *
 * This specifies whether the discriminator will operate only for a small
 * (bin) eta region or will operate for the full analysis region (generally the
 * ATLAS precision region).
 *
 **/
enum EtaDependency {
  EtaIndependent = 0,
  EtaDependent,
  NEtaDependencies
};

/**
 * @brief Specify et position dependency
 *
 * This specifies whether the discriminator will operate only for a small
 * (bin) Et region or will operate for the full analysis region (generally the
 * ATLAS precision region).
 *
 **/
enum EtDependency {
  EtIndependent = 0,
  EtDependent,
  NEtDependencies
};

/**
 * @brief the cut requirement
 **/
enum Requirement {
  Loose_CutID_Pd = 0, // Same detection probability as CutID Loose
  Medium_CutID_Pd,    // Same detection probability as CutID Medium
  Tight_CutID_Pd,     // Same detection probability as CutID Tight
  Loose_CutID_Pf,     // Same false alarm probability as CutID Loose
  Medium_CutID_Pf,    // Same false alarm probability as CutID Medium
  Tight_CutID_Pf,     // Same false alarm probability as CutID Tight
  Loose_LH_Pd,        // Same detection probability as LH Loose
  Medium_LH_Pd,       // Same detection probability as LH Medium
  Tight_LH_Pd,        // Same detection probability as LH Tight
  Loose_LH_Pf,        // Same false alarm probability as LH Loose
  Medium_LH_Pf,       // Same false alarm probability as LH Medium
  Tight_LH_Pf,        // Same false alarm probability as LH Tight
  Medium_MaxSP,       // Maximum SP-product
  Loose,              // Same as LooseCutIDPd
  Medium,             // Same as MediumSP
  Tight,              // Same as TightCutIDPf
  NoCut,              // Run to retrieve discriminators output, but no cut is applied
  NRequirements       // Number of Requirements available
};

} // namespace Ringer

#endif // RINGERSELECTORTOOLS_RINGERSELECTORTOOLSENUMS_H

