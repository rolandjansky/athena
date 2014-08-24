// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaConversionFillerTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2009
 * @brief Block filler tool for photon conversion information.
 */


#ifndef EGAMMAD3PDMAKER_EGAMMACONVERSIONFILLERTOOL_H
#define EGAMMAD3PDMAKER_EGAMMACONVERSIONFILLERTOOL_H


#include "D3PDMakerUtils/BlockFillerTool.h"
#include "xAODEgamma/Photon.h"


namespace D3PD {


/**
 * @brief Block filler tool for photon conversion information.
 */
class egammaConversionFillerTool
  : public BlockFillerTool<xAOD::Photon>
{
public:
  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  egammaConversionFillerTool (const std::string& type,
                              const std::string& name,
                              const IInterface* parent);
  

  /// Book variables for this block.
  virtual StatusCode book();


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  virtual StatusCode fill (const xAOD::Photon& p);


private:
  /// Variable: is there a conversion?
  bool* m_hasconv;

  /// Variable: conversion vertex x
  float* m_convvtxx;

  /// Variable: conversion vertex y
  float* m_convvtxy;

  /// Variable: conversion vertex z
  float* m_convvtxz;

  /// Variable: radius of conversion
  float* m_Rconv;

  /// Variable: z of conversion
  float* m_zconv;

  /// Variable: conversion vertex chi2
  float* m_convvtxchi2;

  /// Variable: pt of track1 of conversion
  float* m_pt1conv;

  /// Variable: conversion track 1 number of B layer hits
  uint8_t* m_convtrk1nBLHits;

  /// Variable: conversion track 1 number of pixel hits
  uint8_t* m_convtrk1nPixHits;

  /// Variable: conversion track 1 number of SCT hits
  uint8_t* m_convtrk1nSCTHits;

  /// Variable: conversion track 1 number of TRT hits
  uint8_t* m_convtrk1nTRTHits;

  /// Variable: pt of track2 of conversion
  float* m_pt2conv;

  /// Variable: conversion track 2 number of B layer hits
  uint8_t* m_convtrk2nBLHits;

  /// Variable: conversion track 2 number of pixel hits
  uint8_t* m_convtrk2nPixHits;

  /// Variable: conversion track 2 number of SCT hits
  uint8_t* m_convtrk2nSCTHits;

  /// Variable: conversion track 2 number of TRT hits
  uint8_t* m_convtrk2nTRTHits;

  /// Variable: pt of conversion tracks 1+2
  float* m_ptconv;

  /// Variable: pz of conversion tracks 1+2
  float* m_pzconv;
};


} // namespace D3PD


#endif // not EGAMMAD3PDMAKER_EGAMMACONVERSIONFILLERTOOL_H
