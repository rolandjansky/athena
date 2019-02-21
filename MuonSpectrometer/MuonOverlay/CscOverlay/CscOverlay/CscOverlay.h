/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Overlaying RDOs from two different events for InDet subdetectors.
//
// Andrei Gaponenko <agaponenko@lbl.gov>, 2006, 2007
//
// Ketevi A. Assamagan <ketevi@bnl.gov>, March 2008

#ifndef CSCOVERLAY_H
#define CSCOVERLAY_H

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonOverlayBase/MuonOverlayBase.h"

#include "MuonRDO/CscRawDataContainer.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"

#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"

#include <vector>
#include <map>

class CscIdHelper;
class IMuonDigitizationTool;

namespace std { template<typename _Tp> class auto_ptr; }

class CscOverlay : public MuonOverlayBase  {
public:
  
  CscOverlay(const std::string &name,ISvcLocator *pSvcLocator);

  /** Framework implemenrtation for the event loop */  
  virtual StatusCode overlayInitialize();
  virtual StatusCode overlayExecute();
  virtual StatusCode overlayFinalize();

  /** given 2 container of data - zero bias real data and the simulation,
      do the merging */
  void overlayContainer(CscRawDataContainer* main, const CscRawDataContainer* overlay);
  void overlayContainer(std::auto_ptr<CscRawDataContainer>& data, const CscRawDataContainer* mc) {
    this->overlayContainer(data.get(), mc);
  }

  /** if the 2 container do overlay, 
      loop over the container and do the overlap collection by collection */
  void mergeCollections(CscRawDataCollection *out_coll, const CscRawDataCollection *orig_coll, 
                                   const CscRawDataCollection *ovl_coll);
  
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
                        const uint32_t hash );

  //Whether the data needs to be fliped by 49-strip for bug#56002
  bool needtoflip(const int address) const;

  // ----------------------------------------------------------------

  ServiceHandle<StoreGateSvc> m_storeGateTemp;
 
  // jO controllable properties.
  // "Main" containers are read, have data from "overlay" containers added,
  // and written out with the original SG keys.
  std::string m_mainInputCSC_Name;
  std::string m_overlayInputCSC_Name;

  const CscIdHelper   * m_cscHelper;
  ToolHandle<ICscCalibTool> m_cscCalibTool;
  ToolHandle<IMuonDigitizationTool> m_digTool;
  ToolHandle<IMuonDigitizationTool> m_rdoTool2;
  ToolHandle<IMuonDigitizationTool> m_rdoTool4;
  ToolHandle<Muon::ICSC_RDO_Decoder> m_cscRdoDecoderTool;
  bool m_isByteStream;

  ServiceHandle <IAtRndmGenSvc> m_rndmSvc;      // Random number service
  CLHEP::HepRandomEngine *m_rndmEngine;    // Random number engine used - not init in SiDigitization
  std::string m_rndmEngineName;// name of random engine

};

#endif/*CScOVERLAY_H*/
