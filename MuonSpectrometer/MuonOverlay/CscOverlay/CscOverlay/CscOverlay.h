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

  /** Framework implemenrtation for the event loop */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

private:

  /** given 2 container of data - zero bias real data and the simulation,
      do the merging */
  StatusCode overlayContainer(const CscRawDataContainer* main, const CscRawDataContainer* overlay);

  /** if the 2 container do overlay,
      loop over the container and do the overlap collection by collection */
  void mergeCollections(CscRawDataCollection *out_coll, const CscRawDataCollection *orig_coll,
                        const CscRawDataCollection *ovl_coll, CLHEP::HepRandomEngine* rndmEngine);

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

  // Copying CscRawDataCollection properties
  void copyCscRawDataCollectionProperties(const CscRawDataCollection& sourceColl, CscRawDataCollection& outColl) const;

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
