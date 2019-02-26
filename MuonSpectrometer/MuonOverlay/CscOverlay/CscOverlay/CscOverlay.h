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

#include <string>

#include "GaudiKernel/ToolHandle.h"
#include "MuonOverlayBase/MuonOverlayBase.h"
#include "MuonDigToolInterfaces/IMuonDigitizationTool.h"
#include "PileUpTools/IPileUpTool.h"

#include "MuonRDO/CscRawDataContainer.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "AthenaKernel/IAthRNGSvc.h"

#include <vector>
#include <map>

class CscIdHelper;

namespace std { template<typename _Tp> class auto_ptr; }

namespace CLHEP {
  class HepRandomEngine;
}

class CscOverlay : public MuonOverlayBase  {
public:

  CscOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  /** given 2 container of data - zero bias real data and the simulation,
      do the merging */
  void overlayContainer(const CscRawDataContainer* main, const CscRawDataContainer* overlay);
  void overlayContainer(std::auto_ptr<CscRawDataContainer>& data, const CscRawDataContainer* mc) {
    this->overlayContainer(data.get(), mc);
  }

  /** if the 2 container do overlay,
      loop over the container and do the overlap collection by collection */
  void mergeCollections(CscRawDataCollection *out_coll, const CscRawDataCollection *orig_coll,
                        const CscRawDataCollection *ovl_coll, CLHEP::HepRandomEngine* rndmEngine);

private:

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
  SG::ReadHandleKey<CscRawDataContainer> m_inputDataRDOKey{this,"InputDataRDOKey","OriginalEvent_SG+CSCRDO",""};
  SG::ReadHandleKey<CscRawDataContainer> m_inputOverlayRDOKey{this,"InputOverlayRDOKey","BkgEvent_0_SG+CSCRDO",""};
  SG::WriteHandleKey<CscRawDataContainer> m_outputContainerKey{this,"OutputContainerKey","StoreGateSvc+CSCRDO",""};


  const CscIdHelper   * m_cscHelper{nullptr};
  ToolHandle<ICscCalibTool> m_cscCalibTool{this, "CalibTool", "CscCalibTool", ""};
  ToolHandle<IPileUpTool> m_digTool{this, "DigitizationTool", "CscDigitizationTool", ""};
  ToolHandle<IMuonDigitizationTool> m_rdoTool2{this, "MakeRDOTool2", "CscDigitToCscRDOTool2", ""};
  ToolHandle<IMuonDigitizationTool> m_rdoTool4{this, "MakeRDOTool4", "CscDigitToCscRDOTool4", ""};
  PublicToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool{this, "CscRdoDecoderTool", "Muon::CscRDO_Decoder", ""};

  ServiceHandle <IAthRNGSvc> m_rndmSvc{this, "RndmSvc", "AthRNGSvc", "Random Number Service"};      // Random number service
};

#endif/*CSCOVERLAY_CSCOVERLAY_H*/
