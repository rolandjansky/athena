/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Overlaying RDOs from two different events for InDet subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
//
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

#ifndef CSCOVERLAY_CSCOVERLAY_H
#define CSCOVERLAY_CSCOVERLAY_H


#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "MuonRDO/CscRawDataContainer.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "AthenaKernel/IAthRNGSvc.h"

#include <vector>
#include <string>
#include <map>

class CscIdHelper;

namespace CLHEP {
  class HepRandomEngine;
}

class CscOverlay : public AthAlgorithm {
public:

  CscOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:

  /// @brief Overlay signal on the background container and record to the output one
  StatusCode overlayContainer(const CscRawDataContainer *bkgContainer,
                              const CscRawDataContainer *signalContainer,
                              CscRawDataContainer *outputContainer);

  /// @brief Copy CscRawDataCollection, optionally only copy properties
  std::unique_ptr<CscRawDataCollection> copyCollection(const CscRawDataCollection *collection,
                                                       bool propertiesOnly = false) const;

  /// @brief In case of overlap merge signal and background collections
  void mergeCollections(const CscRawDataCollection *bkgCollection,
                        const CscRawDataCollection *signalCollection,
                        CscRawDataCollection *outputCollection,
                        CLHEP::HepRandomEngine *rndmEngine);

  /** get the data in one SPU of a chamber */
  void spuData( const CscRawDataCollection * coll, const uint16_t spuID, std::vector<const CscRawData*>& data);

  /** data in one gas lauer */
  uint32_t stripData ( const std::vector<const CscRawData*>& data,
                       const unsigned int numSamples,
                       std::map< int,std::vector<uint16_t> >& samples,
                       uint32_t& hash,
                       const uint16_t spuID,
                       const int gasLayer, bool isdata);

  /** do the overlay - summing the ADC samples on one plane if there is overlap
      between zero bias data and simulation. If there is no overlap, simply
      copy the data */
  std::vector<CscRawData*> overlay( const std::map< int,std::vector<uint16_t> >& sigSamples,
                                    const std::map< int,std::vector<uint16_t> >& ovlSamples,
                                    const uint32_t address,
                                    const uint16_t spuID,
                                    const uint16_t collId,
                                    const uint32_t hash,
                                    CLHEP::HepRandomEngine *rndmEngine);

  //Whether the data needs to be fliped by 49-strip for bug#56002
  bool needtoflip(const int address) const;

  // ----------------------------------------------------------------

  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  SG::ReadHandleKey<CscRawDataContainer> m_bkgInputKey{this,"BkgInputKey","OriginalEvent_SG+CSCRDO",""};
  SG::ReadHandleKey<CscRawDataContainer> m_signalInputKey{this,"SignalInputKey","BkgEvent_0_SG+CSCRDO",""};
  SG::WriteHandleKey<CscRawDataContainer> m_outputKey{this,"OutputKey","StoreGateSvc+CSCRDO",""};


  const CscIdHelper   * m_cscHelper{nullptr};
  ToolHandle<ICscCalibTool> m_cscCalibTool{this, "CalibTool", "CscCalibTool", ""};
  PublicToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool{this, "CscRdoDecoderTool", "Muon::CscRDO_Decoder", ""};

  ServiceHandle <IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service"};      // Random number service
};

#endif/*CSCOVERLAY_CSCOVERLAY_H*/
