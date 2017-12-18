/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Dear emacs, this is -*-c++-*-

/** 
 * @file
 *
 * Overlaying RDOs from two different events for InDet subdetectors.
 *
 * @author Andrei Gaponenko, 2006-2009
 */

#ifndef INDETOVERLAY_H
#define INDETOVERLAY_H

#include <string>

#include "GaudiKernel/ServiceHandle.h"

#include "IDC_OverlayBase/IDC_OverlayBase.h"

class StoreGateSvc;
class SCT_ID;

class InDetOverlay : public IDC_OverlayBase {
public:
  
  InDetOverlay(const std::string &name,ISvcLocator *pSvcLocator);
  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  // SCT collection merging needs an ID helper.  Instead of retrieving
  // it thousands of times per event in the collection merging code,
  // do it once in InDetOverlay initialization and make available here
  const SCT_ID *get_sct_id() const { return m_sct_id; }

  enum SCT_numbers {NextBC=0, CurrentBC=1, PreviousBC=2, AnyBC=3, NumberOfBCs=3, NumberOfBitSets=4,
                    NumberOfStrips=768,
                    MCSource=0, DataSource=1, NumberOfSources=2};

private:

  ServiceHandle<StoreGateSvc> m_detStore;
  const SCT_ID *m_sct_id;

  // ----------------------------------------------------------------
  //! jO controllable properties.
  //! "Main" containers are read, have data from "overlay" containers added,
  //! and written out with the original SG keys.
  bool m_do_TRT, m_do_TRT_background;
  std::string m_mainInputTRT_Name;
  std::string m_overlayInputTRT_Name;

  bool m_do_SCT, m_do_SCT_background;
  std::string m_mainInputSCT_Name;
  std::string m_overlayInputSCT_Name;

  bool m_do_Pixel, m_do_Pixel_background;
  std::string m_mainInputPixel_Name;
  std::string m_overlayInputPixel_Name;

};

#endif/*INDETOVERLAY_H*/
