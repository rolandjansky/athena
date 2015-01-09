/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TRT_DriftCircleValidationNtupleWriter.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "InDetTrackValidation/TRT_DriftCircleValidationNtupleWriter.h"
// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

// Trk
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkEventPrimitives/LocalPosition.h"
#include "TrkEventPrimitives/ParamDefs.h"

#include "GaudiKernel/ListItem.h"

#include "xAODEventInfo/EventInfo.h"

#include "InDetPrepRawData/TRT_DriftCircle.h"

InDet::TRT_DriftCircleValidationNtupleWriter::TRT_DriftCircleValidationNtupleWriter(const std::string& name, ISvcLocator* pSvcLocator):
        AthAlgorithm(name,pSvcLocator),
        m_riocontainer(0),
        //m_inputTrackCollection("Tracks")
        //        m_fullNtupleName("/NTUPLES/FILE1/FitterValidation/TrackStatistics"),
        mjo_riocontainername("TRT_DriftCircles"),
        m_ntupleFileName("/NTUPLES/FILE1"),
        m_ntupleDirName("FitterValidation"),
        m_ntupleTreeName("RIOs"),
        m_nt(0),
        m_rioLoc1(0),
        m_rioSurfaceX(0),
        m_rioSurfaceY(0),
        m_rioSurfaceZ(0)
{
    declareProperty("TRT_DriftCircleContainer", mjo_riocontainername);
    declareProperty("NtupleFileName", m_ntupleFileName);
    declareProperty("NtupleDirectoryName", m_ntupleDirName);
    declareProperty("NtupleTreeName", m_ntupleTreeName);

}

InDet::TRT_DriftCircleValidationNtupleWriter::~TRT_DriftCircleValidationNtupleWriter() {}

StatusCode InDet::TRT_DriftCircleValidationNtupleWriter::initialize() {

    // ---------------------------
    // retrive pointer to THistSvc
    ITHistSvc *tHistSvc;
    StatusCode sc =  service("THistSvc", tHistSvc);
    if ( sc.isFailure() ) {
        ATH_MSG_ERROR( "Unable to retrieve pointer to THistSvc" );
        return sc;
    }

    // ---------------------------
    // create tree and register it to THistSvc
    m_nt = new TTree(TString(m_ntupleTreeName), "TRT_DriftCircles");
    // NB: we must not delete the tree, this is done by THistSvc
    std::string fullNtupleName =  "/"+m_ntupleFileName+"/"+m_ntupleDirName+"/"+m_ntupleTreeName;
    sc = tHistSvc->regTree(fullNtupleName, m_nt);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Unable to register TTree : " << fullNtupleName );
        return sc;
    }
    //-----------------
    // add items
    // event info:
    m_nt->Branch("EventNumber",     &m_eventNumber, "event_number/I");      // Event number
    m_nt->Branch("TRT_nRIOs",       &m_nRIOs,       "Number_of_RIOs/I");    // number of TRT RIOs
    // RIO info:
    m_rioLoc1       = new std::vector<float>();
    m_rioSurfaceX   = new std::vector<float>();
    m_rioSurfaceY   = new std::vector<float>();
    m_rioSurfaceZ   = new std::vector<float>();
    m_nt->Branch("TRT_DriftRadius", &m_rioLoc1); 
    m_nt->Branch("TRT_SurfaceX",    &m_rioSurfaceX); 
    m_nt->Branch("TRT_SurfaceY",    &m_rioSurfaceY); 
    m_nt->Branch("TRT_SurfaceZ",    &m_rioSurfaceZ); 

    return sc;
}

StatusCode InDet::TRT_DriftCircleValidationNtupleWriter::execute() {

    StatusCode sc = StatusCode::SUCCESS;
    // -----------
    // get the container with TRT RIOs
    m_riocontainer = 0;
    sc = evtStore()->retrieve(m_riocontainer, mjo_riocontainername);
    if(sc.isFailure()) {
        ATH_MSG_ERROR("Could not get PrepRawDataContainer");
        return StatusCode::FAILURE;
    }

    //-------------
    // get the event number    
    const xAOD::EventInfo* eventInfo;
    sc = evtStore()->retrieve(eventInfo);
    if (sc.isFailure()) {
        ATH_MSG_ERROR( "Could not retrieve event info" );
    }
    m_eventNumber = eventInfo->eventNumber();
    int RIOindex = 0;
    // reset variables
    m_nRIOs = 0;
    m_rioLoc1->clear();
    m_rioSurfaceX->clear();
    m_rioSurfaceY->clear();
    m_rioSurfaceZ->clear();
     // -----------------------
    // get all the RIO_Collections in the container
    InDet::TRT_DriftCircleContainer::const_iterator containerIterator = m_riocontainer->begin();
    for( ; containerIterator != m_riocontainer->end(); containerIterator++){
        ATH_MSG_DEBUG( "There are "  << (*containerIterator)->size() << " entries in the PrepRawDataCollection" );

        // ---------------------------------
        //get all the RIOs in the collection
        InDet::TRT_DriftCircleCollection::const_iterator rioIterator = (*containerIterator)->begin();
        for (; rioIterator != (*containerIterator)->end(); rioIterator++) {
            // get the surface center of the RIO
            if (!(*rioIterator)) {
                ATH_MSG_WARNING( "NULL pointer to RIO" );
                continue;
            }
            if (!((*rioIterator)->detectorElement())) {
                ATH_MSG_WARNING( "in RIO: NULL pointer to detElement" );
                continue;
            }

            if (m_nRIOs >=500000) {
                ATH_MSG_WARNING( "reached max. number of RIO entries in the ntuple" );
                m_nRIOs = RIOindex;
                m_nt->Fill();
                return StatusCode::SUCCESS;

            }
            const Amg::Vector3D surfaceGlobPos = (*rioIterator)->detectorElement()->center( (*rioIterator)->identify() ) ;
            // fill surface global position
            m_rioSurfaceX->push_back(float(surfaceGlobPos.x()));
            m_rioSurfaceY->push_back(float(surfaceGlobPos.y()));
            m_rioSurfaceZ->push_back(float(surfaceGlobPos.z()));
            // get the local position of the RIO
            const Amg::Vector2D locPos = (*rioIterator)->localPosition();
            // fill local position
            m_rioLoc1->push_back(float(locPos[Trk::locX]));
            // count the RIO
            RIOindex++;
        }
    }
    m_nRIOs = RIOindex;
    m_nt->Fill();
    return StatusCode::SUCCESS;
}

StatusCode InDet::TRT_DriftCircleValidationNtupleWriter::finalize() {
    ATH_MSG_INFO( "TRT_DriftCircleValidationNtupleWriter finalize()" );
    delete m_rioLoc1;
    delete m_rioSurfaceX;
    delete m_rioSurfaceY;
    delete m_rioSurfaceZ;
    m_rioLoc1       = 0;
    m_rioSurfaceX   = 0;
    m_rioSurfaceY   = 0;
    m_rioSurfaceZ   = 0;

    return StatusCode::SUCCESS;
}


