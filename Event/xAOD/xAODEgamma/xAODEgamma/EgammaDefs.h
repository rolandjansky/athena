// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EgammaDefs.h 775814 2016-09-29 12:14:38Z ludovica $
#ifndef XAODEGAMMA_EGAMMADEFS_H
#define XAODEGAMMA_EGAMMADEFS_H

extern "C" {
 #include <stdint.h>
 }


/// Namespace holding all the xAOD EDM classes
namespace xAOD {

  namespace EgammaParameters {
     /// for the Authors of the egamma objects
    /** @brief Default author */
    const uint16_t AuthorUnknown=0x0;
    /** @brief Object Reconstructed by standard cluster-based algorithm. Surely not Photon ==> Electron */
    const uint16_t AuthorElectron=0x1;
    /** @brief Electron reconstructed by the track-based algorithm, here for compatibility with old AOD format*/
    const uint16_t AuthorSofte =0x2;
    /** @brief Object Reconstructed by standard cluster-based algorithm. Surely not Electron ==> Photon */
    const uint16_t AuthorPhoton=0x4;
    /** @brief Electron reconstructed by the Forward cluster-based algorithm */
    const uint16_t AuthorFwdElectron=0x8;
    /** @brief Object Reconstructed by standard cluster-based algorithm. Ambiguous ==> can be either Electron or Photon */
    const uint16_t AuthorAmbiguous=0x10;
    /** @brief trigger Electrons */
    const uint16_t AuthorTrigElectron=0x20;
    /** @brief trigger Photons */
    const uint16_t AuthorTrigPhoton=0x40;
    /** @brief Photon reconstructed by SW CaloTopo35 seeded clusters */
    const uint16_t AuthorCaloTopo35=0x80;
    /** @brief not an actual author, but used as default in some functions */
    const uint16_t AuthorALL = 0xFFFF;

  //
  // for object quality flags
  //

  enum BitDefOQ {
    /** @brief Dead High Voltage in the EM Presampler */
    DeadHVPS = 0,
    /** @brief Dead High Voltage in the EM strips, middle and back affecting the core of the cluster*/
    DeadHVS1S2S3Core = 1,
    /** @brief Dead High Voltage in the EM strips, middle and back affecting the edge of the cluster*/
    DeadHVS1S2S3Edge = 2,

    /** @brief Non Nominal High Voltage in the EM Presampler */
    NonNominalHVPS = 3,
   /** @brief Non Nominal High Voltage in the EM strips, middle and back*/
    NonNominalHVS1S2S3 = 4,

    /** @brief Missing FEB in the core of the cluster */
    MissingFEBCellCore = 5,
    /** @brief Missing FEB in the edge of the cluster */
    MissingFEBCellEdgePS = 6,
    /** @brief Missing FEB in the edge of the cluster */
    MissingFEBCellEdgeS1 = 7,
    /** @brief Missing FEB in the edge of the cluster */
    MissingFEBCellEdgeS2 = 8,
    /** @brief Missing FEB in the edge of the cluster */
    MissingFEBCellEdgeS3 = 9,

    /** @brief Masked cell in the core of the cluster */
    MaskedCellCore = 10,
    /** @brief Masked presampler cell in the edge of the cluster */
    MaskedCellEdgePS = 11,
    /** @brief Masked strip cell in the edge of the cluster */
    MaskedCellEdgeS1 = 12,
    /** @brief Masked middle cell in the edge of the cluster */
    MaskedCellEdgeS2 = 13,
    /** @brief Masked back cell in the edge of the cluster */
    MaskedCellEdgeS3 = 14,
   /** @brief Missing FEB or masked cell in S1 core (corresponding to the 8 strips in front of the core of the cluster)*/
    BadS1Core = 15,
    /** @brief Sporadic noisy cell in the core of the cluster */
    SporadicNoiseLowQCore = 16,
    /** @brief Sporadic noisy cell in the edge of the cluster */
    SporadicNoiseLowQEdge = 17,
    /** @brief High quality factor cell in the core of the cluster */
    HighQCore = 18,
    /** @brief High quality factor cell in the edge of the cluster */
    HighQEdge = 19,
    /** @brief Affected cell in the core of the cluster */
    AffectedCellCore = 20,
    /** @brief Affected presampler cell in the edge of the cluster */
    AffectedCellEdgePS = 21,
    /** @brief Affected strip cell in the edge of the cluster */
    AffectedCellEdgeS1 = 22,
    /** @brief Affected middle cell in the edge of the cluster */
    AffectedCellEdgeS2 = 23,
    /** @brief Affected back cell in the edge of the cluster */
    AffectedCellEdgeS3 = 24,
    /** @brief High quality factor cell in the HEC */
    HECHighQ = 25,
    /** @brief Out of time cell */
    OutTime =26,
    /** @brief Cleaning based on LArQ factor--> sum[ E(Q>4000)/E]*/
    LArQCleaning=27,
    /** @brief Dead cell in the Tile Calorimeter S0 affecting the hadronic leakage calculation*/
    DeadCellTileS0=28,
    /** @brief Dead cell in the Tile Calorimeter S1,2,3 affecting the hadronic leakage calculation*/
    DeadCellTileS1S2=29,
    /** @brief High R_cell ----> Energy of the most energetic cell / total energy of the cluster */
    HighRcell =30
  };

  /** @brief */

    const uint32_t BADCLUSELECTRON =
    0x1 << MaskedCellCore |
    0x1 << MissingFEBCellCore |
    0x1 << MissingFEBCellEdgeS1 |
    0x1 << MissingFEBCellEdgeS2 |
    0x1 << DeadHVS1S2S3Core |
    0x1 << DeadHVS1S2S3Edge ;

    const uint32_t BADCLUSPHOTON =
    0x1 << MaskedCellCore |
    0x1 << MissingFEBCellCore |
    0x1 << MissingFEBCellEdgeS1 |
    0x1 << MissingFEBCellEdgeS2 |
    0x1 << DeadHVS1S2S3Core |
    0x1 << DeadHVS1S2S3Edge |
    0x1 << BadS1Core;

    const uint32_t ALLOQ= 0xFFFFFFFF;


    struct ROOT6_NamespaceAutoloadHook{};
  }
}

#endif // XAODEGAMMA_EGAMMADEFS_H
