/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iomanip>
#include <fstream>

#include "SiDigiAnalysis/SiDigiAnalysis.h"
#include "GaudiKernel/MsgStream.h"
// Don't know where Amg is defined, but don't seem to need to include a file for it
#include "PixelReadoutGeometry/PixelDetectorManager.h"
#include "SCT_ReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "ReadoutGeometryBase/SiLocalPosition.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "TTree.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include <Eigen/Geometry>


#include "SCT_ReadoutGeometry/StripBoxDesign.h"

using namespace std;
using namespace InDetDD;
//::SiLocalPosition;

SiDigiAnalysis::SiDigiAnalysis(const std::string &name, ISvcLocator *pSvcLocator):
 AthAlgorithm(name, pSvcLocator),
 m_storeGate("StoreGateSvc", name),
 m_detStore("DetectorStore", name),
 m_histSvc("THistSvc", name) {
    ATH_MSG_DEBUG("Create SiDigiAnalysis");
    declareProperty("doRDO", m_doRDO = true);
    declareProperty("doTruth", m_doTruth = true);
    declareProperty("PixelRDO_Container", m_pixelRDO_ContainerName = "PixelRDOs");
    declareProperty("StripRDO_Container", m_stripRDO_ContainerName = "SCT_RDOs");
    declareProperty("TruthEventContainer", m_truthEventContainerName = "TruthEvent");
}

StatusCode SiDigiAnalysis::initialize() {
//
//    Get a StoreGate-handle for access to the Event Store
//
    CHECK(m_storeGate.retrieve());
//
//    Get Detector Store
//
    CHECK(m_detStore.retrieve());
//
//    Get a handle on the NTuple and histogramming service
//
    CHECK(m_histSvc.retrieve());
    m_outputTree = new TTree("digTree", "digTree");
    CHECK(m_histSvc->regTree("/DIG_NTUPLE/digTree", m_outputTree));
//
//    Get Pixel Detector Manager           
//
    CHECK(m_detStore->retrieve(m_pixelManager, "Pixel"));
    ATH_MSG_INFO("Found Pixel Detector Manager. Layout is " << m_pixelManager->getLayout());
//
//    Get SCT Detector Manager
//
    CHECK(m_detStore->retrieve(m_SCT_Manager, "SCT"));
    ATH_MSG_INFO("Found SCT_Manager. Layout is " << m_SCT_Manager->getLayout());
//
//    Get SCT ID helper:
//    When things go wrong with identifiers, it is still good to have the ID helper. So instead of
//    Getting id of each RDO, then element of this id, then m_sctIdHelper for this element, we just
//    get the helper from the first element managed, and use that for ever.
//
    SiDetectorElementCollection::const_iterator firstEl = m_SCT_Manager->getDetectorElementBegin();
    if (*firstEl) {
        m_sctIdHelper = dynamic_cast<const SCT_ID *> ((*firstEl)->getIdHelper());
        if (m_sctIdHelper) cout << "Got SCT_ID helper " << hex << m_sctIdHelper << "\n";
    }
    else {
        ATH_MSG_FATAL("Unable to get SCT ID helper from SCT_DetectorManager\n");
        return StatusCode::FAILURE;
    }

    cout << "wafer_hash_max is " << m_sctIdHelper->wafer_hash_max() << endl;
//
//    Add branches and leaves to the tree 
//
    m_outputTree->Branch("NPixelRDO", &m_nPixelRDO, "NPixelRDO/i");
    m_outputTree->Branch("PixelRDO_BarrelEndcap", &m_pixelRDO_BarrelEndcap);
    m_outputTree->Branch("PixelRDO_Layer", &m_pixelRDO_Layer);
    m_outputTree->Branch("PixelRDO_Phi", &m_pixelRDO_Phi);
    m_outputTree->Branch("PixelRDO_Eta", &m_pixelRDO_Eta);
    m_outputTree->Branch("PixelRDO_GlobalX", &m_pixelRDO_GlobalX);
    m_outputTree->Branch("PixelRDO_GlobalY", &m_pixelRDO_GlobalY);
    m_outputTree->Branch("PixelRDO_GlobalZ", &m_pixelRDO_GlobalZ);
    m_outputTree->Branch("PixelRDO_LocalX", &m_pixelRDO_LocalX);
    m_outputTree->Branch("PixelRDO_LocalY", &m_pixelRDO_LocalY);
    m_outputTree->Branch("PixelRDO_LocalZ", &m_pixelRDO_LocalZ);
    m_outputTree->Branch("PixelRDO_ToT", &m_pixelRDO_ToT);
    m_outputTree->Branch("PixelRDO_BCID", &m_pixelRDO_BCID);

    m_outputTree->Branch("NStripRDO", &m_nStripRDO, "NStripRDO/i");
    m_outputTree->Branch("StripRDO_BarrelEndcap", &m_stripRDO_BarrelEndcap);
    m_outputTree->Branch("StripRDO_Layer", &m_stripRDO_Layer);
    m_outputTree->Branch("StripRDO_Phi", &m_stripRDO_Phi);
    m_outputTree->Branch("StripRDO_Eta", &m_stripRDO_Eta);
    m_outputTree->Branch("StripRDO_Side", &m_stripRDO_Side);
    m_outputTree->Branch("StripRDO_Row", &m_stripRDO_Row);
    m_outputTree->Branch("StripRDO_Strip", &m_stripRDO_Strip);

    m_outputTree->Branch("StripRDO_size", &m_stripRDO_size);

    m_outputTree->Branch("StripRDO_LocalX", &m_stripRDO_LocalX);
    m_outputTree->Branch("StripRDO_LocalY", &m_stripRDO_LocalY);
    m_outputTree->Branch("StripRDO_LocalZ", &m_stripRDO_LocalZ);

    m_outputTree->Branch("StripRDO_x0", &m_stripRDO_x0);
    m_outputTree->Branch("StripRDO_y0", &m_stripRDO_y0);
    m_outputTree->Branch("StripRDO_z0", &m_stripRDO_z0);

    m_outputTree->Branch("StripRDO_x1", &m_stripRDO_x1);
    m_outputTree->Branch("StripRDO_y1", &m_stripRDO_y1);
    m_outputTree->Branch("StripRDO_z1", &m_stripRDO_z1);

    m_outputTree->Branch("NTruth", &m_nTruth, "NTruth/i");
    m_outputTree->Branch("TruthBarcode", &m_truthBarcode);
    m_outputTree->Branch("TruthPdgId", &m_truthPdgId);
    m_outputTree->Branch("TruthStatus", &m_truthStatus);
    m_outputTree->Branch("TruthPx", &m_truthPx);
    m_outputTree->Branch("TruthPy", &m_truthPy);
    m_outputTree->Branch("TruthPz", &m_truthPz);
    m_outputTree->Branch("TruthVx", &m_truthVx);
    m_outputTree->Branch("TruthVy", &m_truthVy);
    m_outputTree->Branch("TruthVz", &m_truthVz);

    clear();

    return StatusCode::SUCCESS;
}

StatusCode SiDigiAnalysis::execute() {

    if (m_doRDO) {
        const DataHandle<PixelRDO_Container> pixelRDO_Container;
        ATH_MSG_DEBUG("Searching for container " << m_pixelRDO_ContainerName);
        CHECK(m_storeGate->retrieve(pixelRDO_Container, m_pixelRDO_ContainerName));
            ATH_MSG_DEBUG("Total size of PixelRDO_Containers is " << pixelRDO_Container->size());
            PixelRDO_Container::const_iterator pixelContainerItr;
            for (pixelContainerItr=pixelRDO_Container->begin(); pixelContainerItr!=pixelRDO_Container->end(); ++pixelContainerItr) {
                const DataVector<PixelRDORawData>* rdoCollection = *pixelContainerItr;
                ATH_MSG_DEBUG("Pixel RDO size is " << rdoCollection->size());
                InDetRawDataCollection<PixelRDORawData>::const_iterator collectionItr;
                for (collectionItr=rdoCollection->begin(); collectionItr!=rdoCollection->end(); ++collectionItr) {
                    const PixelRDORawData *rdoData = *collectionItr;
                    Identifier rdoId = rdoData->identify();
                    ATH_MSG_DEBUG("Got rdoId " << rdoId);
                    const SiDetectorElement *element = m_pixelManager->getDetectorElement(rdoId);
                    if (!element) {
                        cout << "Pixel DetectorElement not found!\n";
                        continue;
                    }
                    SiLocalPosition localPos = element->localPositionOfCell(rdoId);
                    Amg::Vector3D globalPos = element->globalPosition(localPos);

                    m_pixelIdHelper = dynamic_cast<const PixelID *>(element->getIdHelper());
	  
                    m_nPixelRDO++;

                    m_pixelRDO_BarrelEndcap.push_back(m_pixelIdHelper->barrel_ec(rdoId));
                    m_pixelRDO_Layer.push_back(m_pixelIdHelper->layer_disk(rdoId));
                    m_pixelRDO_Phi.push_back(m_pixelIdHelper->phi_module(rdoId));
                    m_pixelRDO_Eta.push_back(m_pixelIdHelper->eta_module(rdoId));
                    m_pixelRDO_GlobalX.push_back(globalPos[Amg::x]);
                    m_pixelRDO_GlobalY.push_back(globalPos[Amg::y]);
                    m_pixelRDO_GlobalZ.push_back(globalPos[Amg::z]);
                    m_pixelRDO_LocalX.push_back(localPos.xEta());
                    m_pixelRDO_LocalY.push_back(localPos.xPhi());
                    m_pixelRDO_LocalZ.push_back(localPos.xDepth());
                    m_pixelRDO_ToT.push_back(rdoData->getToT());
                    m_pixelRDO_BCID.push_back(rdoData->getBCID());
                }
            }
        ATH_MSG_DEBUG("Found " << m_nPixelRDO << " pixel digis in event");

        const DataHandle<SCT_RDO_Container> stripRDO_Container;
        ATH_MSG_DEBUG("Searching for container " << m_stripRDO_ContainerName);
        CHECK(m_storeGate->retrieve(stripRDO_Container, m_stripRDO_ContainerName));

        ATH_MSG_DEBUG("Total size of strip_RDO_Container is " << stripRDO_Container->size());
        SCT_RDO_Container::const_iterator stripContainerItr;
        for (stripContainerItr = stripRDO_Container->begin(); stripContainerItr != stripRDO_Container->end(); 
             stripContainerItr++) {
            const DataVector<SCT_RDORawData> *rdoCollection = *stripContainerItr;
            DataVector<SCT_RDORawData>::const_iterator collectionItr;
            for (collectionItr = rdoCollection->begin(); collectionItr != rdoCollection->end(); collectionItr++) {
                const SCT_RDORawData *rdoData = *collectionItr;
                Identifier id = rdoData->identify();
                int bec = m_sctIdHelper->barrel_ec(id);
                int lay = m_sctIdHelper->layer_disk(id);
                int phi = m_sctIdHelper->phi_module(id);
                int eta = m_sctIdHelper->eta_module(id);
                int sid = m_sctIdHelper->side(id);
                int row = m_sctIdHelper->row(id);
                int str = m_sctIdHelper->strip(id);
                Identifier waferId = m_sctIdHelper->wafer_id(id);
                IdentifierHash idHash =  m_sctIdHelper->wafer_hash(waferId);

                m_stripRDO_BarrelEndcap.push_back(bec);
                m_stripRDO_Layer.push_back(lay);
                m_stripRDO_Phi.push_back(phi);
                m_stripRDO_Eta.push_back(eta);
                m_stripRDO_Side.push_back(sid);
                m_stripRDO_Row.push_back(row);
                m_stripRDO_Strip.push_back(str);

                m_nStripRDO++;
                m_stripRDO_size.push_back(rdoData->getGroupSize());

                const SiDetectorElement *element = m_SCT_Manager->getDetectorElement(id);
                if (element) {
                    // Lorentz-corrected position
                    Amg::Vector2D localPos = element->localPositionOfCell(id);
//                    if (bec != 0) {
//                      cout << "Local x, y (should be phi, eta) = " << localPos.x() << ", " << localPos.y() << endl;
//                    }
                    m_stripRDO_LocalX.push_back(localPos.x());
                    m_stripRDO_LocalY.push_back(localPos.y());
                    m_stripRDO_LocalZ.push_back(0.0);

                    // SiDetectorElement::endsOfStrip gives results in *global* frame
                    // get centre of cell, in Trk frame, not Lorentz corrected
                    SiCellId cellId = element->cellIdFromIdentifier(id);
                    localPos = element->design().localPositionOfCell(cellId);
                    SiLocalPosition silp(localPos[1], localPos[0]);
                    const StripBoxDesign *box = dynamic_cast <const StripBoxDesign *> (&(element->design()));
                    if (box) {
                        if (!(box->inActiveArea(silp, false))) {
                            cerr << "Local position of cell is not in active area!\n";
                            cerr << "id  = " << hex << id << "; waferId = " << waferId << dec << "; Hash = " << idHash << "\n";
                            cerr << "bec = " << bec << "; lay = " << lay << "; phi = " << phi << "; eta = " << eta << 
                                    "; sid = " << sid << "; row = " << row << "; str = " << str << endl;
                            cerr << localPos << endl;
                            cerr << "__________________________________\n";
                        }
                    }
                    std::pair<Amg::Vector3D, Amg::Vector3D> endsOfStrip = element->endsOfStrip(localPos);
/*
cout << "global 1st end x, y, z = " << endsOfStrip.first.x() << ", " << endsOfStrip.first.y() << ", " << 
                                       endsOfStrip.first.z() << endl;
cout << "global 2nd end x, y, z = " << endsOfStrip.second.x() << ", " << endsOfStrip.second.y() << ", " << 
                                       endsOfStrip.second.z() << endl;
*/
                    m_stripRDO_x0.push_back(endsOfStrip.first.x());
                    m_stripRDO_y0.push_back(endsOfStrip.first.y());
                    m_stripRDO_z0.push_back(endsOfStrip.first.z());

                    m_stripRDO_x1.push_back(endsOfStrip.second.x());
                    m_stripRDO_y1.push_back(endsOfStrip.second.y());
                    m_stripRDO_z1.push_back(endsOfStrip.second.z());

                }
                else {
                    std::cout << "Strip element not found!\n";
                    cout << "id  = " << hex << id << "; waferId = " << waferId << dec << "; Hash = " << idHash << "\n";
                    cout << "bec = "   << bec << "; lay = " << lay << "; phi = " << phi << "; eta = " << eta << 
                            "; sid = " << sid << "; row = " << row << "; str = " << str << endl;

                    Identifier builtWaferId = m_sctIdHelper->wafer_id(bec, lay, phi, eta, sid);
                    cout << "Wafer Id rebuilt from bec/... is " << hex << builtWaferId << dec;
                    if (builtWaferId.is_valid()) {
                        cout << " - Valid!\n";
                    }
                    else {
                        cout << " - Invalid!\n";
                    }

                    IdentifierHash builtHashId = m_sctIdHelper->wafer_hash(builtWaferId);
                    cout << "Hash from built wafer_id: " << builtHashId;
                    if (builtHashId.is_valid()) {
                        cout << " - Valid!\n";
                    }
                    else {
                        cout << " - Invalid!\n";
                    }
                }
            } // end of loop over rdoCollection
        } // end of loop over stripRDO_Container
    }

    if (m_doTruth) {
        const McEventCollection* mcCollptr;
        StatusCode sc = m_storeGate->retrieve(mcCollptr, m_truthEventContainerName);
        if( sc.isFailure() ) {
            ATH_MSG_WARNING("Could not find McEventCollection " << m_truthEventContainerName);
        } 
        else {
            ATH_MSG_DEBUG("Found main McEventCollection " << m_truthEventContainerName);
            ATH_MSG_DEBUG("Size of McEventCollection is " << mcCollptr->size());
            for (McEventCollection::const_iterator itr = mcCollptr->begin(); itr != mcCollptr->end(); ++itr) {
                ATH_MSG_DEBUG(m_truthEventContainerName << " McEventCollection size = " << (*itr)->particles_size());
                HepMC::GenEvent::particle_const_iterator Part;
                for (Part = (*itr)->particles_begin(); Part != (*itr)->particles_end(); ++Part) {
                    m_nTruth++;
                    m_truthBarcode.push_back((*Part)->barcode());
                    m_truthPdgId.push_back((*Part)->pdg_id());
                    m_truthStatus.push_back((*Part)->status());
                    m_truthPx.push_back((*Part)->momentum().px());
                    m_truthPy.push_back((*Part)->momentum().py());
                    m_truthPz.push_back((*Part)->momentum().pz());

                    m_truthVx.push_back((*Part)->production_vertex()->point3d().x());
                    m_truthVy.push_back((*Part)->production_vertex()->point3d().y());
                    m_truthVz.push_back((*Part)->production_vertex()->point3d().z());

                    // m_truthCharge.push_back(pdt->find((*Part)->pdg_id())->charge());
                } // loop over single McEventCollection
            } // loop over McEventCollections
        } // found McEventCollection
    }

    m_outputTree->Fill();

    clear();
 
    return StatusCode::SUCCESS;
}

StatusCode SiDigiAnalysis::finalize() {
//
//    Write about all elements to a separate file (B field is initialised at start of run, which happens after initialise)
//
    ofstream elementFile;
    elementFile.open("sensorList.txt");
    elementFile << "hash-id id        bec lay phi eta side L[deg]   phi-axis                  eta-axis             normal-axis " 
     << " hit-(phi, eta, depth)  readoutSide \n\n";

    int nElems = 0;
    for (SiDetectorElementCollection::const_iterator el = m_SCT_Manager->getDetectorElementBegin();
         el <  m_SCT_Manager->getDetectorElementEnd(); ++el) {
        const SiDetectorElement *element = *el;
        Identifier id = element->identify();
        int bec = m_sctIdHelper->barrel_ec(id);
        int lay = m_sctIdHelper->layer_disk(id);
        int phi = m_sctIdHelper->phi_module(id);
        int eta = m_sctIdHelper->eta_module(id);
        int sid = m_sctIdHelper->side(id);
        double tanL = element->getTanLorentzAnglePhi();
        double lorentzAngle = atan(tanL) * 180. / M_PI;
        Amg::Vector3D dirPhi = element->phiAxis();
        Amg::Vector3D dirEta = element->etaAxis();
        Amg::Vector3D dirNor = element->normal();

        elementFile << setw(4) << m_sctIdHelper->wafer_hash(id) << " " << hex << id << dec << " " << setw(2) << 
         bec << " " << setw(1) << lay << " " << setw(2) << phi << " " << setw(3) << eta << " " << setw(1) << sid << " " << 
         setw(7) << setprecision(3) << fixed << lorentzAngle << 
         setprecision(3) << " (" << setw(6) << dirPhi[0] << ", "<< setw(6) << dirPhi[1] << ", "<< setw(6) << dirPhi[2] << ") " <<
                            " (" << setw(6) << dirEta[0] << ", "<< setw(6) << dirEta[1] << ", "<< setw(6) << dirEta[2] << ") " <<
                            " (" << setw(6) << dirNor[0] << ", "<< setw(6) << dirNor[1] << ", "<< setw(6) << dirNor[2] << ") " <<
         setprecision(1) << setw(2) << "(" << element->hitPhiDirection() << ", " << setw(2) << element->hitEtaDirection() << 
                     ", " << setw(2) << element->hitDepthDirection() << ") " << element->design().readoutSide() << 
         endl;
        Identifier tempId = m_sctIdHelper->wafer_id(bec, lay, phi, eta, sid);
        if (!tempId.is_valid()) cout << "Invalid id!\n";
        IdentifierHash idHash =  m_sctIdHelper->wafer_hash(id); if (!idHash.is_valid()) cout << "Invalid hash!\n";
        nElems++;
    }
    cout << "Found " << nElems << " elements\n";

    return StatusCode::SUCCESS;
}

void SiDigiAnalysis::clear() {
// Prepare for next event
    m_nPixelRDO = 0;
    m_pixelRDO_Layer.clear();
    m_pixelRDO_BarrelEndcap.clear();
    m_pixelRDO_Eta.clear();
    m_pixelRDO_Phi.clear();
    m_pixelRDO_GlobalX.clear();
    m_pixelRDO_GlobalY.clear();
    m_pixelRDO_GlobalZ.clear();
    m_pixelRDO_LocalX.clear();
    m_pixelRDO_LocalY.clear();
    m_pixelRDO_LocalZ.clear();
    m_pixelRDO_ToT.clear();
    m_pixelRDO_BCID.clear();

    m_nStripRDO = 0;
    m_stripRDO_BarrelEndcap.clear();
    m_stripRDO_Layer.clear();
    m_stripRDO_Phi.clear();
    m_stripRDO_Eta.clear();
    m_stripRDO_Side.clear();
    m_stripRDO_Row.clear();
    m_stripRDO_Strip.clear();

    m_stripRDO_size.clear();
    m_stripRDO_LocalX.clear();
    m_stripRDO_LocalY.clear();
    m_stripRDO_LocalZ.clear();
    m_stripRDO_x0.clear();
    m_stripRDO_y0.clear();
    m_stripRDO_z0.clear();
    m_stripRDO_x1.clear();
    m_stripRDO_y1.clear();
    m_stripRDO_z1.clear();

    m_nTruth = 0;
    m_truthBarcode.clear();
    m_truthPdgId.clear();
    m_truthStatus.clear();
    m_truthPx.clear();
    m_truthPy.clear();
    m_truthPz.clear();
    m_truthVx.clear();
    m_truthVy.clear();
    m_truthVz.clear();
}
