/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration     
 */

#include "InDetSecVxFinderTool/JetFitterMultiStageFit.h"

#include "VxJetVertex/VxJetCandidate.h"
#include "VxJetVertex/VxVertexOnJetAxis.h"
#include "VxJetVertex/VxClusteringTable.h"
#include "VxJetVertex/PairOfVxVertexOnJetAxis.h"
#include "VxVertex/VxTrackAtVertex.h"


using namespace InDet;

/*
 * For the Hackathon, an example of a new algorithm tool
 */


JetFitterMultiStageFit::JetFitterMultiStageFit(const std::string &t, const std::string &n, const IInterface *p)
        : AthAlgTool(t, n, p)
{
    // constructor, declare all necessary things
    declareInterface< JetFitterMultiStageFit >(this);
}


JetFitterMultiStageFit::~JetFitterMultiStageFit() {}

StatusCode JetFitterMultiStageFit::initialize() {

    StatusCode sc = AlgTool::initialize();
    if (sc.isFailure()) {
        msg(MSG::ERROR) << " Unable to initialize the AlgTool" << endmsg;
        return sc;
    }

    sc =  m_helper.retrieve();
    if(sc.isFailure()) 	  {
        msg(MSG::ERROR) << " Unable to retrieve "<<m_helper<<endmsg;
        return StatusCode::FAILURE;
    }else msg(MSG::INFO) << "JetFitter Helper retrieved"<<endmsg;

    sc = m_initializationHelper.retrieve();
    if(sc.isFailure()) 	  {
        msg(MSG::ERROR) << " Unable to retrieve "<<m_initializationHelper<<endmsg;
        return StatusCode::FAILURE;
    }else msg(MSG::INFO) << "JetFitter Initialization Helper retrieved"<<endmsg;

    sc = m_routines.retrieve();
    if(sc.isFailure())
    {
        msg(MSG::ERROR) << " Unable to retrieve the JetFitter routines"<<m_routines<<endmsg;
        return StatusCode::FAILURE;
    }
    else msg(MSG::INFO) << "JetFitter Routines class retrieved"<<endmsg;

    if (m_jetFitterUtils.retrieve().isFailure())
    {
        msg(MSG::ERROR) << "Could not find JetFitterUtils tool." << endmsg;
        return StatusCode::FAILURE;
    }
    else msg(MSG::INFO) << " JetFitterUtils retrieved" << endmsg;

    return StatusCode::SUCCESS;
}

StatusCode JetFitterMultiStageFit::finalize() {
    msg(MSG::INFO) << "Finalize successful" << endmsg;
    return StatusCode::SUCCESS;
}

Trk::VxJetCandidate* JetFitterMultiStageFit::doTwoStageFit(const Trk::RecVertex & primaryVertex,
                                                        const TLorentzVector & jetMomentum,
                                                        const std::vector<const Trk::ITrackLink*> & firstInputTracks,
                                                        const std::vector<const Trk::ITrackLink*> & secondInputTracks,
                                                        const Amg::Vector3D & vtxSeedDirection) const {

    msg(MSG::VERBOSE) << " entered findSecVertex(). Applying JetFitter finding to the found sets of tracks and performing clustering (pattern recognition)  " << endmsg;

    Amg::Vector3D myDirection(jetMomentum.X(),jetMomentum.Y(),jetMomentum.Z());

    std::vector<std::vector<const Trk::ITrackLink*> > bunchesOfTracks; // vector of vector of tracks

    std::vector<const Trk::ITrackLink*> tracksToAdd;

    std::vector<const Trk::ITrackLink*>::const_iterator tracks2Begin=firstInputTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator tracks2End=firstInputTracks.end();
    for (std::vector<const Trk::ITrackLink*>::const_iterator tracks2Iter=tracks2Begin;
         tracks2Iter!=tracks2End;++tracks2Iter) {
        //msg(MSG::VERBOSE) <<" adding track to fit " << endmsg;
        tracksToAdd.push_back(*tracks2Iter);
    }

    bunchesOfTracks.push_back(tracksToAdd);
    tracksToAdd.clear();

    std::vector<const Trk::ITrackLink*>::const_iterator tracks3Begin=secondInputTracks.begin();
    std::vector<const Trk::ITrackLink*>::const_iterator tracks3End=secondInputTracks.end();
    for (std::vector<const Trk::ITrackLink*>::const_iterator tracks3Iter=tracks3Begin;
         tracks3Iter!=tracks3End;++tracks3Iter) {
        if (msgLvl(MSG::VERBOSE)) msg() <<" adding track to fit " << endmsg;
        tracksToAdd.push_back(*tracks3Iter);
    }

    if (tracksToAdd.size()!=0)
    {
        bunchesOfTracks.push_back(tracksToAdd);
    }
    tracksToAdd.clear();

    //now it just uses these bunches...
    //now I have just to make sure that no clustering is done at first iteration
    //while it needs to be done at second iteration (there will be only two iterations)


    std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesBegin=bunchesOfTracks.begin();
    std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesEnd=bunchesOfTracks.end();

    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddBegin;
    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddEnd;
    std::vector<const Trk::ITrackLink*>::const_iterator tracksToAddIter;


    Trk::VxJetCandidate* myJetCandidate=0;

    for (std::vector<std::vector<const Trk::ITrackLink*> >::const_iterator BunchesIter=BunchesBegin;
         BunchesIter!=BunchesEnd;++BunchesIter) {

        if (BunchesIter == BunchesBegin) { // this simply means we are only using tracksToUseInFirstFit
            msg(MSG::VERBOSE) << " initial fit with  " << (*BunchesIter).size() << " tracks " << endmsg;
            myJetCandidate = m_initializationHelper->initializeJetCandidate(*BunchesIter, &primaryVertex, &myDirection,
                                                                            &vtxSeedDirection);
            m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
            if ((*BunchesIter).size() > 0) {
                doTheFit(myJetCandidate, true);
                // Im confused, didnt the comment above say no clustering done in first iteration?
                // yet performClustering = true in the call above??
            }
        }

        // second stage using all tracks, why is it done this way with an if-else in a for loop...
        else {
            msg(MSG::VERBOSE) <<" other fit with " << (*BunchesIter).size() << " tracks " << endmsg;
            std::vector<Trk::VxVertexOnJetAxis*> setOfVertices=myJetCandidate->getVerticesOnJetAxis();
            std::vector<Trk::VxTrackAtVertex*>* setOfTracks=myJetCandidate->vxTrackAtVertex();
            tracksToAddBegin=(*BunchesIter).begin();
            tracksToAddEnd=(*BunchesIter).end();

            for (tracksToAddIter=tracksToAddBegin;tracksToAddIter!=tracksToAddEnd;++tracksToAddIter) {
                std::vector<Trk::VxTrackAtVertex*> temp_vector_tracksAtVertex;
                Trk::VxTrackAtVertex* newVxTrack=new Trk::VxTrackAtVertex((*tracksToAddIter)->clone());
                temp_vector_tracksAtVertex.push_back(newVxTrack);
                setOfTracks->push_back(newVxTrack);
                //add the new tracks to the candidate's track collection
                setOfVertices.push_back(new Trk::VxVertexOnJetAxis(temp_vector_tracksAtVertex));
                //add new vertex with all the *BunchesIter tracks attached to it
            }

            msg(MSG::VERBOSE) <<" new overall number of tracks (vertices?) to fit : " << setOfVertices.size() << endmsg;
            myJetCandidate->setVerticesOnJetAxis(setOfVertices);
            m_initializationHelper->updateTrackNumbering(myJetCandidate);
            //question: should this be done???
            m_routines->initializeToMinDistancesToJetAxis(myJetCandidate);
            // we re-initialize, is this wise? We've merged vertices in the previous iteration...
            doTheFit(myJetCandidate);
        }
    }



    ATH_MSG_DEBUG(" returning jet candidate");
    return myJetCandidate;
}


void JetFitterMultiStageFit::doTheFit(Trk::VxJetCandidate* myJetCandidate,
              bool performClustering) const {

    int numClusteringLoops=0;
    bool noMoreVerticesToCluster(false);

    do {//regards clustering

        if (msgLvl(MSG::VERBOSE))
            msg() << "InDetImprovedJetFitterVxFinder:      ------>>>>         new cycle of fit" << endmsg;

        int numLoops = 0;
        bool noMoreTracksToDelete(false);
        do {//regards eliminating incompatible tracks...

            m_routines->performTheFit(myJetCandidate, 15, false, 30, 0.001);

            const std::vector<Trk::VxVertexOnJetAxis *> &vertices = myJetCandidate->getVerticesOnJetAxis();

            std::vector<Trk::VxVertexOnJetAxis *>::const_iterator verticesBegin = vertices.begin();
            std::vector<Trk::VxVertexOnJetAxis *>::const_iterator verticesEnd = vertices.end();


            //delete incompatible tracks...
            float max_prob(1.);
            Trk::VxVertexOnJetAxis *worseVertex(0);
            for (std::vector<Trk::VxVertexOnJetAxis *>::const_iterator verticesIter = verticesBegin;
                 verticesIter != verticesEnd; ++verticesIter) {
                if (*verticesIter == 0) {
                    msg(MSG::WARNING)
                            << "One vertex is empy. Problem when trying to delete incompatible vertices. No further vertices deleted."
                            << endmsg;
                } else {
                    const Trk::FitQuality &fitQuality = (*verticesIter)->fitQuality();
                    if (TMath::Prob(fitQuality.chiSquared(), (int) std::floor(fitQuality.numberDoF() + 0.5)) <
                        max_prob) {
                        max_prob = TMath::Prob(fitQuality.chiSquared(), (int) std::floor(fitQuality.numberDoF() + 0.5));
                        worseVertex = *verticesIter;
                    }
                }
            }
            if (max_prob < m_vertexProbCut) {
                if (msgLvl(MSG::DEBUG))
                    msg() << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob
                          << endmsg;
                //	  std::cout << "Deleted vertex " << worseVertex->getNumVertex() << " with probability " << max_prob << std::endl;
                if (worseVertex == myJetCandidate->getPrimaryVertex()) {
                    if (msgLvl(MSG::VERBOSE)) msg() << " It's the primary" << endmsg;
                }

                m_routines->deleteVertexFromJetCandidate(worseVertex, myJetCandidate);

            }
            else {
                noMoreTracksToDelete = true;
                if (msgLvl(MSG::VERBOSE)) msg() << "No tracks to delete: maximum probability is " << max_prob << endmsg;
            }

            numLoops += 1;
        } while (numLoops < m_maxNumDeleteIterations && !(noMoreTracksToDelete));

        if (!performClustering) break;

        // m_useFastClustering is false, so possibly can be removed ?? 
        if (!m_useFastClustering && (int)myJetCandidate->getVerticesOnJetAxis().size()<m_maxTracksForDetailedClustering) {
            m_routines->fillTableWithFullProbOfMerging(myJetCandidate,8,false,10,0.01);
        }

        else {
            m_routines->fillTableWithFastProbOfMerging(myJetCandidate);
        }
        const Trk::VxClusteringTable* clusteringTablePtr(myJetCandidate->getClusteringTable());
        msg(MSG::VERBOSE) << "clustering table retrieved" << endmsg;


        if (clusteringTablePtr==0) {
            msg(MSG::WARNING) << " No Clustering Table while it should have been calculated... no more clustering performed during vertexing " << endmsg;
            noMoreVerticesToCluster=true;
        }
        // why else here? why not just continue in if above?
        else {
            msg(MSG::VERBOSE) <<" clustering table is " << *clusteringTablePtr << endmsg; //suppress this?

            //now iterate over the full map and decide wether you want to do the clustering OR not...
            float probVertex(0.);

            // probVertex is passed by reference, below function modifies it...
            Trk::PairOfVxVertexOnJetAxis pairOfVxVertexOnJetAxis=clusteringTablePtr->getMostCompatibleVertices(probVertex);
            //a PairOfVxVertexOnJetAxis is a std::pair<VxVertexOnJetAxis*,VxVertexOnJetAxis*>

            float probVertexExcludingPrimary(0.);
            Trk::PairOfVxVertexOnJetAxis pairOfVxVertexOnJetAxisExcludingPrimary=clusteringTablePtr->getMostCompatibleVerticesExcludingPrimary(probVertexExcludingPrimary);

            // essentially below ask if the two vals are the same, to a tolerance of 1e-6 ...
            bool firstProbIsWithPrimary= ( fabs(probVertex-probVertexExcludingPrimary)>1e-6 );
            // is there a better way of establishing this?

            // merging vertex with primary (if cond satisfied
            if (probVertex>0.&&probVertex>m_vertexClusteringProbabilityCut&&firstProbIsWithPrimary) {
                msg(MSG::VERBOSE) <<" merging vtx number " << (*pairOfVxVertexOnJetAxis.first).getNumVertex() <<
                                                " and " << (*pairOfVxVertexOnJetAxis.second).getNumVertex() << " (should be PV)." << endmsg;  // what do these numVertex mean? e.g. -10, 0, 1, ...?

                m_helper->mergeVerticesInJetCandidate(*pairOfVxVertexOnJetAxis.first,
                                                      *pairOfVxVertexOnJetAxis.second,
                                                      *myJetCandidate);

                //now you need to update the numbering scheme
                m_initializationHelper->updateTrackNumbering(myJetCandidate);//maybe this should be moved to a lower level...
                continue;
            }

            // determine whether to merge vertices that are compatible, where neither are the primary
            if (probVertexExcludingPrimary>0.) {

                //GP suggested by Marco Battaglia, use vertex mass in order to decide wether to split or not, so derive vertex masses first
                const Trk::VxVertexOnJetAxis *firstVertex = pairOfVxVertexOnJetAxisExcludingPrimary.first;
                const Trk::VxVertexOnJetAxis *secondVertex = pairOfVxVertexOnJetAxisExcludingPrimary.second;

                CLHEP::HepLorentzVector massVector1 = m_jetFitterUtils->fourMomentumAtVertex(*firstVertex);//MeV
                CLHEP::HepLorentzVector massVector2 = m_jetFitterUtils->fourMomentumAtVertex(*secondVertex);//MeV

                CLHEP::HepLorentzVector sumMassVector = massVector1 + massVector2;

                double massTwoVertex = sumMassVector.mag();//MeV

                bool doMerge(false);

                double vertexClusteringProbabilityCutWithMass;

                // surely a better way than below...
                if (massTwoVertex < 1000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass0010;
                } else if (massTwoVertex < 1500.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass1015;
                } else if (massTwoVertex < 2000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass1520;
                } else if (massTwoVertex < 2500.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass2025;
                } else if (massTwoVertex < 3000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass2530;
                } else if (massTwoVertex < 4000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass3040;
                } else if (massTwoVertex < 5000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass4050;
                } else if (massTwoVertex < 6000.) {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass5060;
                } else {
                    vertexClusteringProbabilityCutWithMass = m_vertexClusteringProbabilityCutWithMass6070;
                }

                if (probVertexExcludingPrimary > vertexClusteringProbabilityCutWithMass) {
                    doMerge = true; // why not just have doMerge = probVertexExcludingPrimary > vertexClusteringProbabilityCutWithMass ?
                }

                if (doMerge)
                {

                    msg(MSG::VERBOSE) <<" merging vtx number " << (*pairOfVxVertexOnJetAxis.first).getNumVertex() <<
                                                    " and " << (*pairOfVxVertexOnJetAxis.second).getNumVertex() << " mass merged vertex: " << massTwoVertex << endmsg;

                    m_helper->mergeVerticesInJetCandidate(*pairOfVxVertexOnJetAxisExcludingPrimary.first,
                                                          *pairOfVxVertexOnJetAxisExcludingPrimary.second,
                                                          *myJetCandidate);

                    m_initializationHelper->updateTrackNumbering(myJetCandidate);//maybe this should be moved to a lower level...
                    continue;//go to next cycle, after a succesful merging
                }
            }

            noMoreVerticesToCluster=true;

        }

        numClusteringLoops+=1; // when will this ever be greater than 1?
        // noMoreVerticesToCluster has been set to true by the time this line is reached
        // so numClusteringLoops will equal 1 and thats the end of the while loop (see below)...

    } while (numClusteringLoops<m_maxClusteringIterations&&!(noMoreVerticesToCluster));



}
