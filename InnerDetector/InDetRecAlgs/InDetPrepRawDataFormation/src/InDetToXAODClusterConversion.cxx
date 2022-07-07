/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetPrepRawDataFormation/InDetToXAODClusterConversion.h"

#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/PixelClusterCollection.h"
#include "InDetPrepRawData/SCT_ClusterCollection.h"

#include "StoreGate/WriteHandle.h"
#include "StoreGate/ReadHandle.h"

#include "xAODMeasurementBase/UncalibratedMeasurement.h"
#include "xAODInDetMeasurement/PixelClusterAuxContainer.h"
#include "xAODInDetMeasurement/StripClusterAuxContainer.h"

#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "SCT_ReadoutGeometry/StripStereoAnnulusDesign.h"

#include "AthContainers/DataVector.h"
#include <iterator>

using namespace InDet;

// Constructor with parameters:
InDetToXAODClusterConversion::InDetToXAODClusterConversion(const std::string &name, ISvcLocator *pSvcLocator) :
AthReentrantAlgorithm(name, pSvcLocator)
{}


//-----------------------------------------------------------------------------
// Initialize method:
StatusCode InDetToXAODClusterConversion::initialize(){

    ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
    ATH_CHECK(detStore()->retrieve(m_stripID,"SCT_ID"));

    ATH_CHECK( m_pixelDetEleCollKey.initialize() );
    ATH_CHECK( m_stripDetEleCollKey.initialize() );

    ATH_CHECK( m_inputPixelClusterContainerKey.initialize() );
    ATH_CHECK( m_inputStripClusterContainerKey.initialize() );
    ATH_CHECK( m_outputPixelClusterContainerKey.initialize() );
    ATH_CHECK( m_outputStripClusterContainerKey.initialize() );

    ATH_MSG_DEBUG( "Initialize done !" );
    return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
//
StatusCode InDetToXAODClusterConversion::execute(const EventContext& ctx) const {

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
    const InDetDD::SiDetectorElementCollection* pixElements(*pixelDetEleHandle);
    if (not pixelDetEleHandle.isValid() or pixElements==nullptr) {
        ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
        return StatusCode::FAILURE;
    }

    SG::WriteHandle<xAOD::PixelClusterContainer> outputPixelClusterContainer(m_outputPixelClusterContainerKey, ctx);
    ATH_CHECK( outputPixelClusterContainer.record (std::make_unique<xAOD::PixelClusterContainer>(),
                                                   std::make_unique<xAOD::PixelClusterAuxContainer>()) );
    ATH_MSG_DEBUG( "Recorded xAOD::PixelClusterContainer with key: " << m_outputPixelClusterContainerKey.key()  );

    SG::ReadHandle<InDet::PixelClusterContainer> inputPixelClusterContainer(m_inputPixelClusterContainerKey, ctx);

    for (const auto clusterCollection : *inputPixelClusterContainer) {
        if (!clusterCollection) continue;
        for(const auto theCluster : *clusterCollection)  {
            Identifier clusterId = theCluster->identify();

            const InDetDD::SiDetectorElement *element=pixElements->getDetectorElement(m_pixelID->wafer_hash(m_pixelID->wafer_id(clusterId)));
            if ( element==nullptr ) {
                ATH_MSG_FATAL( "Invalid pixel detector element for cluster identifier " << clusterId );
                return StatusCode::FAILURE;
            }
            IdentifierHash idHash = element->identifyHash();

            auto localPos = theCluster->localPosition();
            auto localCov = theCluster->localCovariance();

            Eigen::Matrix<float,2,1> localPosition(localPos.x(), localPos.y());

            Eigen::Matrix<float,2,2> localCovariance;
            localCovariance.setZero();
            localCovariance(0, 0) = localCov(0, 0);
            localCovariance(1, 1) = localCov(1, 1);

            auto globalPos = theCluster->globalPosition();
            Eigen::Matrix<float, 3, 1> globalPosition(globalPos.x(), globalPos.y(), globalPos.z());

            auto RDOs = theCluster->rdoList();
            auto ToTs = theCluster->totList();
            auto charges = theCluster->chargeList();
            auto width = theCluster->width();
            auto omegaX = theCluster->omegax();
            auto omegaY = theCluster->omegay();
            auto isSplit = theCluster->isSplit();
            auto splitProbability1 = theCluster->splitProbability1();
            auto splitProbability2 = theCluster->splitProbability2();

            xAOD::PixelCluster * pixelCl = new xAOD::PixelCluster();
            outputPixelClusterContainer->push_back(pixelCl);

            pixelCl->setMeasurement<2>(idHash, localPosition, localCovariance);
            pixelCl->setRDOlist(RDOs);
            pixelCl->globalPosition() = globalPosition;
            pixelCl->setToTlist(ToTs);
            pixelCl->setChargelist(charges);
            pixelCl->setLVL1A(theCluster->LVL1A());
            pixelCl->setChannelsInPhiEta(width.colRow()[0], width.colRow()[1]);
            pixelCl->setOmegas(omegaX, omegaY);
            pixelCl->setIsSplit(isSplit);
            pixelCl->setSplitProbabilities(splitProbability1, splitProbability2);

            // Create auxiliary branches accessors
            const SG::AuxElement::Accessor< ElementLink< InDet::PixelClusterCollection > > pixelLinkAcc("pixelClusterLink");
            ElementLink<InDet::PixelClusterCollection> pixelLink(theCluster, *clusterCollection);
            pixelLinkAcc( *pixelCl ) = pixelLink;

        }
    }

    ATH_MSG_DEBUG("xAOD::PixelClusterContainer with size: " << outputPixelClusterContainer->size());

    SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> stripDetEleHandle(m_stripDetEleCollKey, ctx);
    const InDetDD::SiDetectorElementCollection* stripElements(*stripDetEleHandle);
    if (not stripDetEleHandle.isValid() or stripElements==nullptr) {
        ATH_MSG_FATAL(m_stripDetEleCollKey.fullKey() << " is not available.");
        return StatusCode::FAILURE;
    }

    SG::WriteHandle<xAOD::StripClusterContainer> outputStripClusterContainer(m_outputStripClusterContainerKey, ctx);
    ATH_CHECK( outputStripClusterContainer.record (std::make_unique<xAOD::StripClusterContainer>(),
                                                   std::make_unique<xAOD::StripClusterAuxContainer>()) );
    ATH_MSG_DEBUG( "Recorded xAOD::StripClusterContainer with key: " << m_outputStripClusterContainerKey.key()  );

    SG::ReadHandle<InDet::SCT_ClusterContainer> inputStripClusterContainer(m_inputStripClusterContainerKey, ctx);

    for (const auto clusterCollection : *inputStripClusterContainer) {
        if (!clusterCollection) continue;
        for(const auto theCluster : *clusterCollection)  {
            Identifier clusterId = theCluster->identify();

            const InDetDD::SiDetectorElement *element=stripElements->getDetectorElement(m_stripID->wafer_hash(m_stripID->wafer_id(clusterId)));
            if ( element==nullptr ) {
                ATH_MSG_FATAL( "Invalid strip detector element for cluster with identifier " << clusterId );
                return StatusCode::FAILURE;
            }

            IdentifierHash idHash = element->identifyHash();

            auto localPos = theCluster->localPosition();
            auto localCov = theCluster->localCovariance();

            Eigen::Matrix<float,1,1> localPosition;
            Eigen::Matrix<float,1,1> localCovariance;
            localCovariance.setZero();

            if (element->isBarrel()) {
                localPosition(0, 0) = localPos.x();
                localCovariance(0, 0) = element->phiPitch()*element->phiPitch()/12.;
            } else {
                InDetDD::SiCellId cellId = element->cellIdFromIdentifier(clusterId);
                const InDetDD::StripStereoAnnulusDesign *design = dynamic_cast<const InDetDD::StripStereoAnnulusDesign *>(&element->design());
                if ( design==nullptr ) {
                    ATH_MSG_FATAL( "Invalid strip annulus design for module with identifier/identifierHash " << element->identify() << "/" << idHash);
                    return StatusCode::FAILURE;
                }
                InDetDD::SiLocalPosition localInPolar = design->localPositionOfCellPC(cellId);
                localPosition(0, 0) = localInPolar.xPhi();
                localCovariance(0, 0) = design->phiPitchPhi()*design->phiPitchPhi()/12.;
            }

            auto globalPos = theCluster->globalPosition();
            Eigen::Matrix<float, 3, 1> globalPosition(globalPos.x(), globalPos.y(), globalPos.z());

            auto RDOs = theCluster->rdoList();
            auto width = theCluster->width();

            xAOD::StripCluster * stripCl = new xAOD::StripCluster();
            outputStripClusterContainer->push_back(stripCl);
            stripCl->setMeasurement<1>(idHash, localPosition, localCovariance);
            stripCl->setRDOlist(RDOs);
            stripCl->globalPosition() = globalPosition;
            stripCl->setChannelsInPhi(width.colRow()[0]);
            stripCl->setHitsInThirdTimeBin(theCluster->hitsInThirdTimeBin());

            // Create auxiliary branches accessors
            const SG::AuxElement::Accessor< ElementLink< InDet::SCT_ClusterCollection > > stripLinkAcc("sctClusterLink");
            ElementLink<InDet::SCT_ClusterCollection> stripLink(theCluster, *clusterCollection);
            stripLinkAcc( *stripCl ) = stripLink;
        }
    }

    ATH_MSG_DEBUG("xAOD::StripClusterContainer with size: " << outputStripClusterContainer->size());

    return StatusCode::SUCCESS;
  }


