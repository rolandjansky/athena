/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// Author: Ketevi A. Assamagan, Woochun Park
/// BNL, April 03 2005

/// algorithm to decode RDO into PrepRawData

#include "CscRdoToCscPrepDataToolCore.h"

#include "EventPrimitives/EventPrimitives.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonRDO/CscRawData.h"
#include "MuonRDO/CscRawDataCollection.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/SurfaceBounds.h"

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

CscRdoToCscPrepDataToolCore::CscRdoToCscPrepDataToolCore(const std::string& type, const std::string& name, const IInterface* parent) :
    base_class(type, name, parent), m_cabling("CSCcablingSvc", name) {
    declareProperty("CSCHashIdOffset", m_cscOffset = 22000);
    declareProperty("DecodeData", m_decodeData = true);
    // DataHandle
    declareProperty("RDOContainer", m_rdoContainerKey = std::string("CSCRDO"), "CscRawDataContainer to retrieve");
    declareProperty("OutputCollection", m_outputCollectionKey = std::string("CSC_Measurements"),
                    "Muon::CscStripPrepDataContainer to record");
}

StatusCode CscRdoToCscPrepDataToolCore::initialize() {
    ATH_CHECK(m_cscCalibTool.retrieve());
    ATH_CHECK(m_cscRdoDecoderTool.retrieve());
    ATH_CHECK(m_idHelperSvc.retrieve());
    ATH_CHECK(m_cabling.retrieve());
    // check if initializing of DataHandle objects success
    ATH_CHECK(m_rdoContainerKey.initialize());
    ATH_CHECK(m_outputCollectionKey.initialize());
    ATH_CHECK(m_muDetMgrKey.initialize());
    return StatusCode::SUCCESS;
}

void CscRdoToCscPrepDataToolCore::printPrepDataImpl(const Muon::CscStripPrepDataContainer* outputCollection) const {
    ATH_MSG_INFO("***************************************************************");
    ATH_MSG_INFO("****** Listing Csc(Strip)PrepData collections content *********");

    if (outputCollection->size() <= 0)
        ATH_MSG_INFO("No Csc(Strip)PrepRawData collections found");
    else {
        ATH_MSG_INFO("Number of Csc(Strip)PrepRawData collections found in this event is " << outputCollection->size());

        int ict = 0;
        int ncoll = 0;
        ATH_MSG_INFO("-------------------------------------------------------------");
        for (IdentifiableContainer<Muon::CscStripPrepDataCollection>::const_iterator icscColl = outputCollection->begin();
             icscColl != outputCollection->end(); ++icscColl) {
            const Muon::CscStripPrepDataCollection* cscColl = *icscColl;

            if (cscColl->size() <= 0) continue;

            ATH_MSG_INFO("PrepData Collection ID " << m_idHelperSvc->cscIdHelper().show_to_string(cscColl->identify())
                                                   << " with size = " << cscColl->size());
            CscStripPrepDataCollection::const_iterator it_cscStripPrepData;
            int icc = 0;
            int iccphi = 0;
            int icceta = 0;
            for (it_cscStripPrepData = cscColl->begin(); it_cscStripPrepData != cscColl->end(); ++it_cscStripPrepData) {
                icc++;
                ict++;
                if (m_idHelperSvc->cscIdHelper().measuresPhi((*it_cscStripPrepData)->identify()))
                    iccphi++;
                else
                    icceta++;

                ATH_MSG_INFO(ict << " in this coll. " << icc
                                 << " prepData id = " << m_idHelperSvc->cscIdHelper().show_to_string((*it_cscStripPrepData)->identify()));
            }
            ncoll++;
            ATH_MSG_INFO("*** Collection " << ncoll << " Summary: " << iccphi << " phi hits / " << icceta << " eta hits ");
            ATH_MSG_INFO("-------------------------------------------------------------");
        }
    }
}

void CscRdoToCscPrepDataToolCore::printInputRdo() const { return; }
