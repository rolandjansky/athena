/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file SCT_CalibBsErrorTool.cxx
 * Implementation file for the SCT_CalibBsErrorTool class
 * @author Shaun Roe
 **/

#include "SCT_CalibBsErrorTool.h"
#include "SCT_CalibUtilities.h"
#include "SCT_CalibNumbers.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

// RAW data access
#include "InDetRawData/InDetRawDataCLASS_DEF.h"
#include "InDetRawData/SCT3_RawData.h"

#include "GaudiKernel/ITHistSvc.h"

#include "TH1I.h"

#include <set>

using namespace std;
using namespace SCT_CalibAlgs;

static const string pathRoot{"/BSErrors/"};
static const string detectorPaths[]{"SCTEC/", "SCTB/","SCTEA/"};
static const string detectorNames[]{"negativeEndcap", "barrel", "positiveEndcap"};
static const int n_BSErrorType{15};
static const int firstBSErrorType{0};
static const int lastBSErrorType{14};

SCT_CalibBsErrorTool::SCT_CalibBsErrorTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent),
  m_detStore{"DetectorStore", name},
  m_evtStore{"StoreGateSvc", name},
  m_pSCTHelper{0},
  m_scterr_bec{0},
  m_scterr_layer{0},
  m_scterr_eta{0},
  m_scterr_phi{0},
  m_scterr_side{0},
  m_scterr_type{0},
  m_maxHash{0}
{
}

StatusCode
SCT_CalibBsErrorTool::initialize() {
  ATH_CHECK(service("THistSvc", m_thistSvc));
  ATH_CHECK(m_detStore->retrieve(m_pSCTHelper, "SCT_ID"));
  ATH_CHECK(m_bytestreamErrorsTool.retrieve());

  m_maxHash = m_pSCTHelper->wafer_hash_max();
  m_waferItrBegin = m_pSCTHelper->wafer_begin();
  m_waferItrEnd = m_pSCTHelper->wafer_end();

  return StatusCode::SUCCESS;
}

StatusCode
SCT_CalibBsErrorTool::finalize() {
  ATH_MSG_INFO("Finalize of SCT_CalibBsErrorTool");

  return StatusCode::SUCCESS;
}

bool
SCT_CalibBsErrorTool::book() {
  bool result{true};
  m_phistoVector.clear();
  string histoName{pathRoot+"GENERAL/"};
  //histogram for numbers of events
  m_numberOfEventsHisto = new TH1I{"events", "Events", 1, 0.5, 1.5};
  if (m_thistSvc->regHist(histoName.c_str(), m_numberOfEventsHisto ).isFailure()) {
    ATH_MSG_ERROR("Error in booking BSErrors histogram");
  }
  //--- BSErrors for each wafer
  SCT_ID::const_id_iterator waferItr{m_waferItrBegin};
  SCT_ID::const_id_iterator waferItrE{m_waferItrEnd};
  for (; waferItr not_eq waferItrE; ++waferItr) {
    Identifier waferId{*waferItr};
    const int bec{m_pSCTHelper->barrel_ec( waferId )};
    const string formattedPosition{formatPosition(waferId, m_pSCTHelper)};
    std::string histotitle{string{"SCT "} + detectorNames[bec2Index(bec)] + string{" BSErrors : plane "} + formattedPosition};
    const std::string name{pathRoot+detectorPaths[bec2Index(m_pSCTHelper->barrel_ec(waferId))] + formattedPosition};
    TH1F* hitmapHisto_tmp{new TH1F{TString{formattedPosition}, TString{histotitle}, n_BSErrorType, firstBSErrorType-0.5, lastBSErrorType+0.5}};
    if (m_thistSvc->regHist( name.c_str(), hitmapHisto_tmp ).isFailure()) ATH_MSG_ERROR("Error in booking BSErrors histogram");
    m_phistoVector.push_back( hitmapHisto_tmp );
  }
  return result;
}

bool
SCT_CalibBsErrorTool::read(const std::string& fileName) {
  ATH_MSG_ERROR("Reding BsError histograms from " << fileName.c_str() << " is not supported!");
  return false;
}

bool
SCT_CalibBsErrorTool::fill(const bool fromData) {
  if (fromData) {
    return fillFromData();
  }
  bool result{true};
  //--- Number of event
  m_numberOfEventsHisto->Fill( 1 );
  //--- Fill BSErrors
  const int maxEntry{static_cast<int>(m_scterr_type->size())};
  for (int i{0}; i != maxEntry; ++i ) {
    int bec{(*m_scterr_bec)[i]};
    int layer{(*m_scterr_layer)[i]};
    int phi{(*m_scterr_phi)[i]};
    int eta{(*m_scterr_eta)[i]};
    int side{(*m_scterr_side)[i]};
    int type{(*m_scterr_type)[i]};
    Identifier waferId{m_pSCTHelper->wafer_id( bec, layer, phi, eta, side )};
    fillBsErrorsForWafer(waferId, type);
  }
  return result;
}

bool
SCT_CalibBsErrorTool::fillFromData() {
  bool result{true};
  //--- Number of event
  m_numberOfEventsHisto->Fill( 1 );
  //--- Loop over BSErrors
  for (int type{0}; type < SCT_ByteStreamErrors::NUM_ERROR_TYPES; ++type) {
    const std::set<IdentifierHash>* errorSet{m_bytestreamErrorsTool->getErrorSet(type)};
    if ( errorSet != 0 ) {
      std::set<IdentifierHash>::const_iterator it{errorSet->begin()};
      std::set<IdentifierHash>::const_iterator itE{errorSet->end()};
      for ( ; it != itE; ++it ) {
        Identifier waferId{m_pSCTHelper->wafer_id(*it)};
        fillBsErrorsForWafer(waferId, type);
      }
    }
  }
  return result;
}

void
SCT_CalibBsErrorTool::fillBsErrorsForWafer(const Identifier& waferId, const int type) {
  int iWaferHash{static_cast<int>(m_pSCTHelper->wafer_hash(waferId))};
  const string osWafer{formatPosition(waferId, m_pSCTHelper,".")};
  //--- Protection for wrong waferID
  if ( iWaferHash < 0 || iWaferHash >= m_maxHash ) {
    ATH_MSG_WARNING("WaferHash " << iWaferHash << " is out of range : [ bec.layer.eta.phi.side, BSErrorType ] = [ " << osWafer << ", " << type << " ]");
  } else {
    ATH_MSG_DEBUG("BSError : [ bec.layer.eta.phi.side, Type ] = [ " << osWafer<< ", " << type << " ]");
    m_phistoVector[ iWaferHash ]->Fill( type );
  }
}
