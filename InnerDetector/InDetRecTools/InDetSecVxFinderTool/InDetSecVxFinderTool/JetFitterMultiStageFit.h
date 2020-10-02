/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration         
*/

/*
 * This tooltakes the tracks divided into those for a first fit, and additional tracks for a second
 * It initializes a jet candidate, which consists of vertices with tracks fitted to them
 * It runs the full JetFitter fitting algorithm, that is the merge cluster and kalman fit
 * It returns the JetFitter output info
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkTrackLink/ITrackLink.h"
#include "TLorentzVector.h"

#include "TrkJetVxFitter/JetFitterRoutines.h"
#include "TrkJetVxFitter/JetFitterHelper.h"
#include "TrkJetVxFitter/JetFitterInitializationHelper.h"
#include "InDetSecVxFinderTool/InDetJetFitterUtils.h"

namespace Trk {
    class VxJetCandidate;
    class RecVertex;
}

namespace InDet {


static const InterfaceID IID_JetFitterMultiStageFit("JetFitterMultiStageFit", 1, 0);

class JetFitterMultiStageFit : public AthAlgTool {
public:

    static const InterfaceID &interfaceID() {
        return IID_JetFitterMultiStageFit;
    }

    StatusCode initialize();

    StatusCode finalize();

    JetFitterMultiStageFit(const std::string &t, const std::string &n, const IInterface *p);

    ~JetFitterMultiStageFit();

    /*
     * Method for performing the two stage fit
     */
    Trk::VxJetCandidate* doTwoStageFit( const Trk::RecVertex & primaryVertex,
                                        const TLorentzVector & jetMomentum,
                                        const std::vector<const Trk::ITrackLink*> & firstInputTracks,
                                        const std::vector<const Trk::ITrackLink*> & secondInputTracks,
                                        const Amg::Vector3D & vtxSeedDirection) const;


private:
    /*
     * Method which runs the merge procedure, at each cycle using the kalman filter
     */
    void doTheFit(Trk::VxJetCandidate* myJetCandidate,
                  bool performClustering=true) const;

    ToolHandle< Trk::JetFitterInitializationHelper > m_initializationHelper {this,"JetFitterInitializationHelper","Trk::JetFitterInitializationHelper",""};
    ToolHandle< Trk::JetFitterHelper > m_helper {this,"JetFitterHelper","Trk::JetFitterHelper",""};
    ToolHandle< Trk::JetFitterRoutines > m_routines {this,"JetFitterRoutines","Trk::JetFitterRoutines",""};
    ToolHandle< InDet::InDetJetFitterUtils > m_jetFitterUtils {this,"InDetJetFitterUtils","InDet::InDetJetFitterUtils/InDetJetFitterUtils",""};

    Gaudi::Property< int > m_maxNumDeleteIterations {this,"MaxNumDeleteIterations",30,""};
    Gaudi::Property< double > m_vertexProbCut {this,"VertexProbCut",0.001,""};
    Gaudi::Property< int > m_maxClusteringIterations {this,"MaxClusteringIterations",30,""};

    Gaudi::Property< bool > m_useFastClustering {this,"UseFastClustering",false,""};

    // to avoid combinatoric slow down, limit on max tracks for detailed clustering
    Gaudi::Property< int > m_maxTracksForDetailedClustering {this,"maxTracksForDetailedClustering",25,""};

    Gaudi::Property< double > m_vertexClusteringProbabilityCut {this,"VertexClusteringProbabilityCut",0.005,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass {this,"VertexClusteringProbabilityCutWithMass",0.05,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass0010 {this,"VertexClusteringProbabilityCutWithMass0010",0.002,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass1015 {this,"VertexClusteringProbabilityCutWithMass1015",0.002,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass1520 {this,"VertexClusteringProbabilityCutWithMass1520",0.050,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass2025 {this,"VertexClusteringProbabilityCutWithMass2025",0.100,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass2530 {this,"VertexClusteringProbabilityCutWithMass2530",0.200,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass3040 {this,"VertexClusteringProbabilityCutWithMass3040",0.500,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass4050 {this,"VertexClusteringProbabilityCutWithMass4050",0.700,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass5060 {this,"VertexClusteringProbabilityCutWithMass5060",0.900,""};
    Gaudi::Property< double > m_vertexClusteringProbabilityCutWithMass6070 {this,"VertexClusteringProbabilityCutWithMass6070",0.900,""};

};

}
