/*
        Copyright (C) 2019-2022 CERN for the benefit of the ATLAS collaboration
*/
// Author: Neza Ribaric <neza.ribaric@cern.ch>

/***************************************************************************
                                                                                                 InDetAdaptiveMultiSecVtxFinderTool.cxx  -
 Description
                                                                                                                 -------------------
                begin   : 01-12-2019
                authors : Neza Ribaric ( Lancaster University )
                information : Tool for Secondary Vertex Finding using AdaptiveMultivertexFitter and InDetTrackSelection
 ***************************************************************************/

#include "Root/InDetAdaptiveMultiSecVtxFinderTool.h"

#include <map>
#include <utility>
#include <vector>

#include "AthContainers/DataVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include "TrkLinks/LinkToXAODTrackParticle.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkParticleBase/LinkToTrackParticleBase.h"
#include "TrkParticleBase/TrackParticleBase.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkTrack/LinkToTrack.h"
#include "TrkTrack/Track.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkVertexFitterInterfaces/IVertexFitter.h"
#include "TrkVertexFitterInterfaces/IVertexLinearizedTrackFactory.h"
#include "TrkVertexSeedFinderUtils/IMode3dFinder.h"
#include "VxMultiVertex/MVFVxTrackAtVertex.h"
#include "VxMultiVertex/MvfFitInfo.h"
#include "VxMultiVertex/TrackToVtxLink.h"
#include "VxVertex/RecVertex.h"
#include "VxVertex/Vertex.h"
#include "VxVertex/VxTrackAtVertex.h"
#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleAuxContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODTracking/VertexContainer.h"

namespace InDet {

    InDetAdaptiveMultiSecVtxFinderTool::InDetAdaptiveMultiSecVtxFinderTool(const std::string& t, const std::string& n,
                                                                           const IInterface* p) :
        base_class(t, n, p) {}

    StatusCode InDetAdaptiveMultiSecVtxFinderTool::initialize() {
        /* Get the right vertex fitting tool from ToolSvc */
        ATH_CHECK(m_VertexFitter.retrieve());
        ATH_CHECK(m_SeedFinder.retrieve());
        ATH_CHECK(m_LinearizedTrackFactory.retrieve());
        ATH_CHECK(m_ImpactPoint3dEstimator.retrieve());
        ATH_CHECK(m_trkFilter.retrieve());

        m_seedperigees = new std::vector<Trk::TrackParameters*>();

        ATH_MSG_DEBUG("Initialization successful");
        return StatusCode::SUCCESS;
    }

    namespace {
        struct xAODVertex_pair {
            double first;
            xAOD::Vertex* second;
            xAODVertex_pair(double p1, xAOD::Vertex* p2) : first(p1), second(p2) {}
            bool operator<(const xAODVertex_pair& other) const { return first > other.first; }
        };
    }  // anonymous namespace

    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiSecVtxFinderTool::findVertex(
        const xAOD::TrackParticleContainer* trackParticles) {
        ATH_MSG_DEBUG(" Number of input tracks before track selection: " << trackParticles->size());

        m_evtNum++;

        std::vector<Trk::ITrackLink*> selectedTracks;
        bool selectionPassed;
        xAOD::TrackParticle::Decorator<bool> trackPass("TrackPassedSelection");
        m_trkdefiPars.clear();

        for (const xAOD::TrackParticle* itr : *trackParticles) {
            xAOD::Vertex null;
            null.makePrivateStore();
            null.setPosition(Amg::Vector3D(0, 0, 0));
            AmgSymMatrix(3) vertexError;
            vertexError.setZero();
            null.setCovariancePosition(vertexError);
            selectionPassed = static_cast<bool>(m_trkFilter->accept(*itr, &null));
            if (selectionPassed) selectionPassed = static_cast<bool>(m_trkFilter->accept(*itr, &null));

            if (selectionPassed) {
                trackPass(*itr) = true;

                Amg::VectorX par = (itr)->definingParameters();
                par[0] = (itr)->hitPattern();
                m_trkdefiPars.push_back(par);

                ElementLink<xAOD::TrackParticleContainer> link;
                link.setElement(itr);
                Trk::LinkToXAODTrackParticle* linkTT = new Trk::LinkToXAODTrackParticle(link);

                linkTT->setStorableObject(*trackParticles);
                selectedTracks.push_back(linkTT);
            }
        }

        ATH_MSG_DEBUG("Of " << trackParticles->size() << " tracks " << selectedTracks.size() << " survived the preselection.");

        std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> returnContainers = doVertexing(selectedTracks);

        return returnContainers;
    }

    std::pair<xAOD::VertexContainer*, xAOD::VertexAuxContainer*> InDetAdaptiveMultiSecVtxFinderTool::doVertexing(
        const std::vector<Trk::ITrackLink*>& trackVector) {
        xAOD::VertexContainer* theVertexContainer = new xAOD::VertexContainer;
        xAOD::VertexAuxContainer* theVertexAuxContainer = new xAOD::VertexAuxContainer;
        theVertexContainer->setStore(theVertexAuxContainer);

        /// prepare decorators to hold multi vertex fit information
        static const xAOD::Vertex::Decorator<Trk::MvfFitInfo*> MvfFitInfo("MvfFitInfo");
        static const xAOD::Vertex::Decorator<bool> isInitialized("isInitialized");
        static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV("VTAV");

        std::vector<xAODVertex_pair> myxAODVertices;

        std::vector<Trk::TrackToVtxLink*> myTrackToVtxLinks;

        /// place all tracks in the origTrack vector and initially all in the seedTrack vector
        std::vector<Trk::ITrackLink*> origTracks = trackVector;
        std::vector<Trk::ITrackLink*> seedTracks = trackVector;

        // create a map between ITrackLink* and TrackToVtxLink*
        std::map<Trk::ITrackLink*, Trk::TrackToVtxLink*> TrackLinkOf;

        // fill vector of track parameters used for fitting
        //  create map

        for (Trk::ITrackLink* trkIter : origTracks) {
            Trk::TrackToVtxLink* newTrkToVtxLink(new Trk::TrackToVtxLink(new std::vector<xAOD::Vertex*>));

            TrackLinkOf[trkIter] = newTrkToVtxLink;
            myTrackToVtxLinks.push_back(newTrkToVtxLink);
        }

        int iteration = 0;
        unsigned int seedtracknumber = seedTracks.size();

        do {
            if (seedtracknumber == 0) { ATH_MSG_DEBUG("New iteration. No tracks available after track selection for seeding."); }

            iteration += 1;
            ATH_MSG_DEBUG("Iteration number " << iteration << " and tracks left for seeding " << seedtracknumber);

            std::vector<const Trk::TrackParameters*> perigeeList;

            for (const Trk::ITrackLink* seedtrkAtVtxIter : seedTracks) { perigeeList.push_back((seedtrkAtVtxIter)->parameters()); }

            ATH_MSG_DEBUG("Going to seed finder");

            std::unique_ptr<Trk::IMode3dInfo> info;
            Amg::Vector3D seedVertex;
            seedVertex = m_SeedFinder->findSeed(m_privtx.x(), m_privtx.y(), info, perigeeList);

            ATH_MSG_DEBUG("Found seed at x: " << seedVertex.x() << " at y: " << seedVertex.y() << " at z: " << seedVertex.z());

            xAOD::Vertex* seededxAODVertex = new xAOD::Vertex;
            theVertexContainer->push_back(seededxAODVertex);
            seededxAODVertex->setPosition(seedVertex);
            Amg::MatrixX looseConstraintCovariance(3, 3);
            looseConstraintCovariance.setIdentity();
            looseConstraintCovariance = looseConstraintCovariance * 1e+8;
            seededxAODVertex->setCovariancePosition(looseConstraintCovariance);
            seededxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
            seededxAODVertex->setVertexType(xAOD::VxType::NotSpecified);

            if (seedVertex.z() == 0) {
                ATH_MSG_DEBUG("No good seed found: no further vertices in event");
                ATH_MSG_DEBUG("Number of input tracks: " << perigeeList.size() << ", but no good seed returned");
                break;
            }

            xAOD::Vertex* constraintVertex = nullptr;
            looseConstraintCovariance.setIdentity();
            looseConstraintCovariance = looseConstraintCovariance * 1e+8;
            constraintVertex = new xAOD::Vertex();
            constraintVertex->makePrivateStore();
            constraintVertex->setPosition(seedVertex);
            constraintVertex->setCovariancePosition(looseConstraintCovariance);
            constraintVertex->setFitQuality(0., -3.);
            constraintVertex->setVertexType(xAOD::VxType::NotSpecified);

            xAOD::Vertex* actualCandidate = new xAOD::Vertex;
            actualCandidate->makePrivateStore();
            actualCandidate->setVertexType(xAOD::VxType::NotSpecified);

            MvfFitInfo(*actualCandidate) =
                new Trk::MvfFitInfo(constraintVertex, new Amg::Vector3D(seedVertex), new Amg::Vector3D(seedVertex));
            isInitialized(*actualCandidate) = false;
            std::vector<Trk::VxTrackAtVertex*> vectorOfTracks(0);
            VTAV(*actualCandidate) = vectorOfTracks;

            for (Trk::ITrackLink* trkIter : origTracks) {
                // now fill perigeesToFit list of track parameters from origTracks
                float doe = findCompatibleTracks(seedVertex, trkIter);
                if (doe < m_significanceCutSeeding) {
                    Trk::TrackToVtxLink* actualLink = TrackLinkOf[trkIter];
                    std::vector<xAOD::Vertex*>* actualvtxlink = actualLink->vertices();
                    // adding vertex to candidates of track
                    actualvtxlink->push_back(actualCandidate);
                    VTAV(*actualCandidate).push_back(new Trk::MVFVxTrackAtVertex((trkIter)->clone(), actualLink));
                }
            }

            ATH_MSG_DEBUG(" Considering n. " << VTAV(*actualCandidate).size() << " tracks for the fit. ");
            if (actualCandidate != nullptr) {
                if (VTAV(*actualCandidate).size() < 2) {
                    ATH_MSG_DEBUG("No tracks found near seed, while at least two tracks were expected.");

                    if (VTAV.isAvailable(*actualCandidate)) {
                        for (auto tav : VTAV(*actualCandidate)) {
                            if (tav == nullptr) continue;

                            (static_cast<Trk::MVFVxTrackAtVertex*>(tav))->setLinkToVertices(nullptr);
                            delete tav;
                            tav = nullptr;
                        }
                        VTAV(*actualCandidate).clear();
                    }
                    if (MvfFitInfo.isAvailable(*actualCandidate) && MvfFitInfo(*actualCandidate) != nullptr) {
                        delete MvfFitInfo(*actualCandidate);
                        MvfFitInfo(*actualCandidate) = nullptr;
                    }
                    delete actualCandidate;
                    actualCandidate = nullptr;

                    break;
                }
            }
            ATH_MSG_DEBUG("Going to fitter.");

            m_VertexFitter->addVtxTofit(actualCandidate);

            ATH_MSG_DEBUG("Deleting tracks with really good fit to vertex from seeding tracks.");
            int nFound = removeTracksFromSeeds(actualCandidate, seedTracks);

            ATH_MSG_DEBUG("Found and deleted " << nFound << " tracks from seeding tracks.");
            if (nFound == 0) {
                ATH_MSG_DEBUG("All tracks used for fitting came from fiting tracks, removing closest from seeding.");
                // all the tracks used for the fit came from fitting track list
                //-> so remove the closest track to seed from seeding, otherwise you'll keep finding the same seed position

                removeClosestTrack(seedVertex, seedTracks, nFound);
            }

            if (nFound == 0) {
                ATH_MSG_DEBUG("You still have not removed any tracks from seeds! Aborting.");
                break;
            }

            ATH_MSG_DEBUG("Checking goodness of fit.");
            m_goodVertex = checkFit(actualCandidate);

            if (!m_goodVertex) {
                ATH_MSG_DEBUG("Bad vertex, deleting the vertex and clearing all pointers");

                seededxAODVertex->setVertexType(xAOD::VxType::KinkVtx);

                if (actualCandidate) {
                    if (VTAV.isAvailable(*actualCandidate)) {
                        for (auto tav : VTAV(*actualCandidate)) {
                            if (tav == nullptr) continue;

                            (static_cast<Trk::MVFVxTrackAtVertex*>(tav))->setLinkToVertices(nullptr);
                            delete tav;
                            tav = nullptr;
                        }
                        VTAV(*actualCandidate).clear();
                    }
                    if (MvfFitInfo.isAvailable(*actualCandidate) && MvfFitInfo(*actualCandidate) != nullptr) {
                        delete MvfFitInfo(*actualCandidate);
                        MvfFitInfo(*actualCandidate) = nullptr;
                    }

                    delete actualCandidate;
                    actualCandidate = nullptr;
                }

            } else {
                ATH_MSG_DEBUG("I have found a good vertex!");

                seededxAODVertex->setVertexType(xAOD::VxType::NoVtx);
                actualCandidate->setVertexType(xAOD::VxType::SecVtx);
                myxAODVertices.emplace_back(0, actualCandidate);
            }
            seedtracknumber = seedTracks.size();
        } while (seedTracks.size() > 1 && iteration < m_maxIterations);

        if (iteration >= m_maxIterations) {
            ATH_MSG_DEBUG("Maximum number of iterations ("
                          << m_maxIterations << ") reached; to reconstruct more vertices, set maxIterations to a higher value.");
        }

        ATH_MSG_DEBUG("Secondary vertex finding complete with " << iteration << " iterations and " << myxAODVertices.size()
                                                                << " vertices found.");

        for (const xAODVertex_pair& vtxIter : myxAODVertices) {
            xAOD::Vertex* fittedVert = vtxIter.second;

            xAOD::Vertex* cand = new xAOD::Vertex;
            theVertexContainer->push_back(cand);
            cand->setPosition(fittedVert->position());
            cand->setCovariancePosition(fittedVert->covariancePosition());
            cand->setFitQuality(fittedVert->chiSquared(), fittedVert->numberDoF());
            cand->setVertexType(xAOD::VxType::SecVtx);

            std::vector<Trk::VxTrackAtVertex>* tracksOfVertex = &(cand->vxTrackAtVertex());
            tracksOfVertex->clear();

            for (Trk::VxTrackAtVertex* MVFtrkIter : VTAV(*fittedVert)) {
                if ((*MVFtrkIter).initialPerigee()) { (*MVFtrkIter).setPerigeeAtVertex(((*MVFtrkIter).initialPerigee())->clone()); }
                tracksOfVertex->push_back(*MVFtrkIter);
            }
        }

        for (const xAODVertex_pair& vtxIter : myxAODVertices) {
            xAOD::Vertex* cand = vtxIter.second;

            for (Trk::VxTrackAtVertex* MVFtrkIter : VTAV(*cand)) {
                (static_cast<Trk::MVFVxTrackAtVertex*>(MVFtrkIter))->setLinkToVertices(nullptr);
                delete MVFtrkIter;
                MVFtrkIter = 0;
            }

            delete MvfFitInfo(*cand);
        }

        ATH_MSG_DEBUG("Looping over vertex container");

        for (xAOD::Vertex* vxIter : *theVertexContainer) {
            std::vector<Trk::VxTrackAtVertex>* myVxTracksAtVtx = &((vxIter)->vxTrackAtVertex());
            if (!myVxTracksAtVtx) continue;

            for (Trk::VxTrackAtVertex& tracksIter : *myVxTracksAtVtx) {
                Trk::LinkToXAODTrackParticle* linkToXAODTP = nullptr;
                Trk::ITrackLink* tmpLink = (tracksIter).trackOrParticleLink();
                if (tmpLink->type() == Trk::ITrackLink::ToxAODTrackParticle) {
                    linkToXAODTP = static_cast<Trk::LinkToXAODTrackParticle*>(tmpLink);
                }

                if (linkToXAODTP) { (vxIter)->addTrackAtVertex(*linkToXAODTP, (tracksIter).weight()); }
            }

            int ntrk = myVxTracksAtVtx->size();
            if (ntrk == 2) {
                ATH_MSG_DEBUG("Could do a V0 search");

                bool isV0 = V0check(getVertexMomenta(vxIter), (&(*vxIter))->position());
                if (isV0) {
                    ATH_MSG_DEBUG("Labeling as V0");
                    (vxIter)->setVertexType(xAOD::VxType::V0Vtx);
                }
            }
        }

        // delete all TrackToVtxLink objects
        for (Trk::TrackToVtxLink* iterator : myTrackToVtxLinks) { delete iterator; }

        if (!theVertexContainer->empty()) {
            xAOD::Vertex* secVtx = theVertexContainer->front();
            if (!secVtx->vxTrackAtVertex().empty()) {
                secVtx->setVertexType(xAOD::VxType::SecVtx);
                xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
                theVertexContainer->push_back(dummyxAODVertex);  // have to add vertex to container here first so it can use its aux store
                dummyxAODVertex->setPosition(secVtx->position());
                dummyxAODVertex->setCovariancePosition(secVtx->covariancePosition());
                dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
                dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
            } else {
                secVtx->setVertexType(xAOD::VxType::NoVtx);
            }
        }

        else if (theVertexContainer->empty()) {
            xAOD::Vertex* dummyxAODVertex = new xAOD::Vertex;
            theVertexContainer->push_back(dummyxAODVertex);  // have to add vertex to container here first so it can use its aux store
            dummyxAODVertex->setPosition(Amg::Vector3D(0, 0, 0));
            Amg::MatrixX looseConstraintCovariance(3, 3);
            looseConstraintCovariance.setIdentity();
            looseConstraintCovariance = looseConstraintCovariance * 1e+8;
            dummyxAODVertex->setCovariancePosition(looseConstraintCovariance);
            dummyxAODVertex->vxTrackAtVertex() = std::vector<Trk::VxTrackAtVertex>();
            dummyxAODVertex->setVertexType(xAOD::VxType::NoVtx);
        }

        int noVtx = 0;
        int kinkVtx = 0;
        int notSpec = 0;
        int secVtx = 0;
        int V0vtx = 0;
        for (unsigned int i = 0; i < theVertexContainer->size(); i++) {
            xAOD::VxType::VertexType vtxType;
            vtxType = static_cast<xAOD::VxType::VertexType>((*theVertexContainer)[i]->vertexType());
            switch (vtxType) {
                case xAOD::VxType::NoVtx: noVtx++; break;
                case xAOD::VxType::KinkVtx: kinkVtx++; break;
                case xAOD::VxType::NotSpecified: notSpec++; break;
                case xAOD::VxType::V0Vtx: V0vtx++; break;
                case xAOD::VxType::SecVtx: secVtx++; break;
                default: ATH_MSG_DEBUG("Unfamiliar vertex type");
            }

            ATH_MSG_DEBUG(" Vtx: " << i << " x= " << (*theVertexContainer)[i]->position().x() << " y= "
                                   << (*theVertexContainer)[i]->position().y() << " z= " << (*theVertexContainer)[i]->position().z()
                                   << " ntracks= " << (*theVertexContainer)[i]->vxTrackAtVertex().size() << " chi2= "
                                   << (*theVertexContainer)[i]->chiSquared() << " #dof = " << (*theVertexContainer)[i]->numberDoF());
        }

        ATH_MSG_DEBUG("Done finding " << theVertexContainer->size() << " vertices and cleaning the container.");
        ATH_MSG_DEBUG("Seeds good/bad/all : " << noVtx << "/" << kinkVtx << "/" << notSpec);
        ATH_MSG_DEBUG("'Good' secondaries : " << secVtx << " and V0: " << V0vtx);

        return std::make_pair(theVertexContainer, theVertexAuxContainer);
    }

    void InDetAdaptiveMultiSecVtxFinderTool::setPrimaryVertexPosition(double vx, double vy, double vz) {
        m_privtx = Amg::Vector3D(vx, vy, vz);
    }

    StatusCode InDetAdaptiveMultiSecVtxFinderTool::finalize() { return StatusCode::SUCCESS; }

    void InDetAdaptiveMultiSecVtxFinderTool::countTracksAndNdf(xAOD::Vertex* myxAODVertex, float& ndf, int& ntrk) const {
        ndf = -3.0;
        ntrk = 0;

        if (myxAODVertex) {
            ndf = myxAODVertex->numberDoF();

            static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV("VTAV");

            for (Trk::VxTrackAtVertex* trkAtVtxIter : VTAV(*myxAODVertex)) {
                if ((trkAtVtxIter)->weight() > m_minWghtAtVtx) { ntrk += 1; }
            }
        }
    }

    const std::vector<Amg::Vector3D> InDetAdaptiveMultiSecVtxFinderTool::getVertexMomenta(xAOD::Vertex* myxAODVertex) const {
        std::vector<Amg::Vector3D> TrkAtVtxMomenta;

        std::vector<Trk::VxTrackAtVertex>* tracksAtVertex = &(myxAODVertex->vxTrackAtVertex());

        ATH_MSG_DEBUG(" getVertexMomenta ... #Tracks associated at vertex : " << tracksAtVertex->size());

        for (Trk::VxTrackAtVertex tracksAtVertexIter : *tracksAtVertex) {
            if ((tracksAtVertexIter).weight() <= m_minWghtAtVtx) continue;
            {
                const Trk::TrackParameters* sv_perigee = (tracksAtVertexIter).perigeeAtVertex();
                if (!sv_perigee) {
                    ATH_MSG_DEBUG("perigeeAtVertex not available!!");
                    continue;
                }

                double qp = 1. / (std::fabs(sv_perigee->parameters()[Trk::qOverP]));
                double theta = sv_perigee->parameters()[Trk::theta];
                double phi = sv_perigee->parameters()[Trk::phi];

                TrkAtVtxMomenta.push_back(Amg::Vector3D(qp * sin(theta) * cos(phi), qp * sin(theta) * sin(phi), qp * cos(theta)));
            }
        }

        return TrkAtVtxMomenta;
    }

    bool InDetAdaptiveMultiSecVtxFinderTool::checkFit(xAOD::Vertex* actualCandidate) const {
        m_ntracks = 0;
        m_ndf = -3.;
        m_ndf = actualCandidate->numberDoF();

        static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV("VTAV");

        for (Trk::VxTrackAtVertex* trkAtVtxIter : VTAV(*actualCandidate)) {
            if ((trkAtVtxIter)->weight() > m_minWghtAtVtx) { m_ntracks += 1; }
        }

        m_goodVertex = actualCandidate != 0 && m_ndf > 0 && m_ntracks >= 2;

        ATH_MSG_DEBUG(" xAOD::Vertex : " << (actualCandidate != 0 ? 1 : 0) << ",  #dof = " << m_ndf
                                         << ",  #tracks (weight>0.01) = " << m_ntracks);

        return m_goodVertex;
    }

    int InDetAdaptiveMultiSecVtxFinderTool::removeTracksFromSeeds(xAOD::Vertex* actualCandidate,
                                                                  std::vector<Trk::ITrackLink*>& seedTracks) const {
        static const xAOD::Vertex::Decorator<std::vector<Trk::VxTrackAtVertex*>> VTAV("VTAV");

        std::vector<Trk::ITrackLink*>::iterator seedBegin = seedTracks.begin();
        std::vector<Trk::ITrackLink*>::iterator seedEnd = seedTracks.end();

        m_goodVertex = checkFit(actualCandidate);

        int nFound = 0;

        for (Trk::VxTrackAtVertex* trkAtVtxIter : VTAV(*actualCandidate)) {
            // delete the pointer to this vertex if the vertex was bad
            if (!m_goodVertex) { (static_cast<Trk::MVFVxTrackAtVertex*>(trkAtVtxIter))->linkToVertices()->vertices()->pop_back(); }

            std::vector<Trk::ITrackLink*>::iterator foundTrack = seedEnd;
            for (std::vector<Trk::ITrackLink*>::iterator seedtrkiter = seedBegin; seedtrkiter != seedEnd; ++seedtrkiter) {
                if ((*seedtrkiter)->parameters() == (trkAtVtxIter)->trackOrParticleLink()->parameters() &&
                    (trkAtVtxIter)->weight() > m_minWghtAtVtx) {
                    foundTrack = seedtrkiter;
                }
            }

            if (foundTrack != seedEnd) {
                seedTracks.erase(foundTrack);

                nFound += 1;

                seedBegin = seedTracks.begin();
                seedEnd = seedTracks.end();
            }
        }

        return nFound;
    }

    float InDetAdaptiveMultiSecVtxFinderTool::findCompatibleTracks(Amg::Vector3D& seedVertex, Trk::ITrackLink* trkIter) const {
        double distance = 0.;

        try {
            std::unique_ptr<Trk::PlaneSurface> mySurface =
                m_ImpactPoint3dEstimator->Estimate3dIP((trkIter)->parameters(), &seedVertex, distance);
            ATH_MSG_VERBOSE(" ImpactPoint3dEstimator done ");
        } catch (error::ImpactPoint3dEstimatorProblem err) {
            ATH_MSG_DEBUG(" ImpactPoint3dEstimator failed to find minimum distance between track and vertex seed: " << err.p);
        }

        if (distance < 0) { ATH_MSG_DEBUG(" Distance between track and seed vtx is negative: " << distance); }

        const Trk::TrackParameters* myPerigee = ((trkIter)->parameters());
        double doe = 99999999.9;
        double error = 0.;

        if (myPerigee && myPerigee->covariance()) {
            error = std::sqrt((*myPerigee->covariance())(Trk::d0, Trk::d0) + (*myPerigee->covariance())(Trk::z0, Trk::z0));
        }  // end of the security check

        if (error == 0.) {
            ATH_MSG_ERROR(" Error is zero! " << distance);
            error = 1.;
        }

        doe = distance / error;

        ATH_MSG_VERBOSE("Distance between track and seed vtx: " << distance << " d/s(d) = " << distance / error << " err " << error);

        return doe;
    }

    void InDetAdaptiveMultiSecVtxFinderTool::removeClosestTrack(Amg::Vector3D& seedVertex, std::vector<Trk::ITrackLink*>& seedTracks,
                                                                int& nFound) const {
        const Trk::ITrackLink* nearestTrack = nullptr;
        double dist = 1e8;

        for (Trk::ITrackLink* trkIter : seedTracks) {
            double distance = 0.;
            try {
                std::unique_ptr<Trk::PlaneSurface> mySurface =
                    m_ImpactPoint3dEstimator->Estimate3dIP((trkIter)->parameters(), &seedVertex, distance);
            } catch (error::ImpactPoint3dEstimatorProblem err) {
                ATH_MSG_DEBUG(" ImpactPoint3dEstimator failed to find minimum distance between this track and vertex seed: " << err.p);
            }
            ATH_MSG_DEBUG("Seed to track dist: " << distance);
            if (distance < 0) { ATH_MSG_DEBUG("Distance was negative!"); }

            if (distance > 0 && !nearestTrack) {
                dist = distance;
                nearestTrack = trkIter;
            }
            if (distance > 0 && distance < dist) {
                dist = distance;
                nearestTrack = trkIter;
            }
        }
        if (nearestTrack) {
            ATH_MSG_DEBUG("Found closest track to seed and deleting.");
            std::vector<Trk::ITrackLink*>::iterator seedBegin = seedTracks.begin();
            std::vector<Trk::ITrackLink*>::iterator seedEnd = seedTracks.end();
            nFound += 1;
            std::vector<Trk::ITrackLink*>::iterator foundTrack = std::find(seedBegin, seedEnd, nearestTrack);
            if (foundTrack != seedEnd) {
                seedTracks.erase(foundTrack);
                seedBegin = seedTracks.begin();
                seedEnd = seedTracks.end();
            } else {
                ATH_MSG_DEBUG("The nearest track was not found!");
            }
        } else {
            ATH_MSG_DEBUG("What else can I try?");
        }
    }

    bool InDetAdaptiveMultiSecVtxFinderTool::V0check(const std::vector<Amg::Vector3D>& momenta, const Amg::Vector3D& posi) const {
        int ntrk = momenta.size();

        if (ntrk < 2) {
            ATH_MSG_DEBUG(" ntrk < 2 , Meaningless to test mass ");
            return false;
        }

        std::vector<double> Pv(ntrk);
        double vx = 0., vy = 0., vz = 0., eK0 = 0.;
        double pi2 = 139.57018 * 139.57018;  // Pion in MeV

        for (int t = 0; t < ntrk; t++) {
            Amg::Vector3D trk = momenta[t];

            vz += trk.z();
            vx += trk.x();
            vy += trk.y();
            Pv[t] = trk.x() * trk.x() + trk.y() * trk.y() + trk.z() * trk.z();
            eK0 += std::sqrt(Pv[t] + pi2);
        }

        double mnt2 = vx * vx + vy * vy + vz * vz;
        double mass = eK0 * eK0 - mnt2;
        mass = 0.001 * (std::sqrt(std::abs(mass)));

        Amg::Vector3D vdif = posi - m_privtx;
        Amg::Vector3D vmoment = Amg::Vector3D(vx, vy, vz);

        double modir = vmoment.dot(vdif) / std::sqrt(mnt2);

        // borrowed from InnerDetector/InDetRecAlgs/InDetV0Finder/InDetV0FinderTool
        double a0z = (vdif + vmoment * vmoment.dot(vdif) / (mnt2 + 0.00001)).z();
        double Rxy = vdif.perp();

        ATH_MSG_DEBUG(" V0kine : a0z = " << a0z << " Rxy = " << Rxy << " direction  " << modir);

        if (ntrk != 2) {
            ATH_MSG_DEBUG(" ntrk != 2 , Meaningless to test V0 ");
            return false;
        }

        if (a0z > 15. || Rxy > 500.) { return false; }

        // 1 eV^(-1) of time = hbar / eV = 6.582173*10^(-16) second,  for energy-time in natural unit
        //  double planck = 6.582173 ;

        double eGam = std::sqrt(Pv[0] + 0.511 * 0.511) + std::sqrt(Pv[1] + 0.511 * 0.511);
        double mGam = eGam * eGam - mnt2;

        double prtn2 = 938.27205 * 938.27205;
        double eLam = Pv[0] > Pv[1] ? std::sqrt(Pv[0] + prtn2) + std::sqrt(Pv[1] + pi2) : std::sqrt(Pv[0] + pi2) + std::sqrt(Pv[1] + prtn2);
        double mLam = eLam * eLam - mnt2;

        ATH_MSG_DEBUG(" V0 masses : " << mass << " " << std::sqrt(std::abs(mGam)) << " " << std::sqrt(std::abs(mLam)));

        return ((fabs(mass - 497.614) < 100.)                                 // K short
                || (mGam > 0 && std::sqrt(mGam) < 40.)                        // gamma conversion ;
                || (mLam > 0 && std::abs(std::sqrt(mLam) - 1115.683) < 200.)  //  Lambda
        );
    }

}  // end namespace InDet
