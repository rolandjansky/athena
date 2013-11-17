// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_OBJECT_MATCHING_TRAIT_DEFS_H
#define TRIG_OBJECT_MATCHING_TRAIT_DEFS_H

#include "DataModel/DataVector.h"

#include "TrigObjectMatching/TrigMuonEFInfoDistanceFunctor.h"
#include "TrigObjectMatching/ClusterDistanceFunctor.h"

namespace TrigMatch {

    // Here we define the trait classes.  The basic problem is
    // this:
    //
    // Some classes are attached to the navigation as basic objects
    // for example - MuonFeature
    // Other classes are attached to the navigation as containers of
    // objects for example - egamma's are attached as egammaContainers
    // Still other classes are not attached at all, but can be gotten
    // from the ancestor method in the TDT, such as the L1 items
    // We need to make this complexity invisible to the user, so they
    // can just call whatever they want to match to and not worry
    // about how it interacts with the navigation
    // We solve this with traits classes (thanks to Scott Snyder for
    // the suggestion) which tell us whether or not a class is a
    // container and if it is, allows us to map from the class to
    // the container class and similarly for ancest style objects

    struct DirectAttached {}; // identifies objects as attached directly
    struct AncestorAttached {}; // identifies objects as attached via ancest

    // by default, we assume a class is attached directly
    template<typename T>
    struct ClassTraits {
        typedef DirectAttached type;
    };

}
    
// now we specialize for the container classes

// This macro specializes the definition of the
// ContainerForClass trait class
#define SPECIALIZE_CONTAINER_FOR_CLASS(x,y) \
    namespace TrigMatch { \
        template<> \
        struct ClassTraits<x> { \
            typedef y type; \
        }; \
    }

// This macro does the forward declaration
// for objects without a namespace
#define DECLARE_ATTACHED_CONTAINER(x,y) \
    class x; \
    class y; \
    SPECIALIZE_CONTAINER_FOR_CLASS(x,y) 

// This macro does the forward declaration
// for objects with the same namespace
// as the container
#define DECLARE_ATTACHED_CONTAINER_NAMESPACE(a,x,y) \
    namespace a { \
        class x; \
        class y; \
    } \
    SPECIALIZE_CONTAINER_FOR_CLASS(a::x,a::y) 

// This macro does the forward declaration
// for objects with a namespace, but not namespace
// on the container
#define DECLARE_ATTACHED_CONTAINER_OBJECT_NAMESPACE(a,x,y) \
    namespace a { \
        class x; \
    } \
    class y; \
    SPECIALIZE_CONTAINER_FOR_CLASS(a::x,y) 

// This macro does the forward declaration
// for objects with a namespace, and containers
// that are a typedef
#define DECLARE_ATTACHED_CONTAINER_TYPEDEF(a, x, y) \
    namespace a { \
        class x; \
    } \
    SPECIALZE_CONTAINER_FOR_CLASS(a::x, y)

    DECLARE_ATTACHED_CONTAINER(CaloCell, CaloCellContainer)
    DECLARE_ATTACHED_CONTAINER(TrigPhoton, TrigPhotonContainer)
    DECLARE_ATTACHED_CONTAINER(TrigElectron, TrigElectronContainer)
    DECLARE_ATTACHED_CONTAINER(TrigL2Bjet, TrigL2BjetContainer)
    DECLARE_ATTACHED_CONTAINER(CosmicMuon, CosmicMuonCollection)
    DECLARE_ATTACHED_CONTAINER(MdtTrackSegment, MdtTrackSegmentCollection)
    DECLARE_ATTACHED_CONTAINER(TrigL2Bphys, TrigL2BphysContainer)
    DECLARE_ATTACHED_CONTAINER(TrigInDetTrack, TrigInDetTrackCollection)
    DECLARE_ATTACHED_CONTAINER(TrigVertex, TrigVertexCollection)
    DECLARE_ATTACHED_CONTAINER(CaloCluster, CaloClusterContainer)
    DECLARE_ATTACHED_CONTAINER(TrigMuonEF, TrigMuonEFContainer)
    DECLARE_ATTACHED_CONTAINER(TrigMuonEFInfo, TrigMuonEFInfoContainer)
    DECLARE_ATTACHED_CONTAINER(CaloShower, CaloShowerContainer)
    DECLARE_ATTACHED_CONTAINER(egamma, egammaContainer)
    DECLARE_ATTACHED_CONTAINER_NAMESPACE(Rec, TrackParticle, TrackParticleContainer)
    DECLARE_ATTACHED_CONTAINER_NAMESPACE(Analysis, TauDetails, TauDetailsContainer)
    DECLARE_ATTACHED_CONTAINER(TrigEFBjet, TrigEFBjetContainer)
    DECLARE_ATTACHED_CONTAINER(TrigEFBphys, TrigEFBphysContainer)
    DECLARE_ATTACHED_CONTAINER_NAMESPACE(Analysis, TauJet, TauJetContainer)
    DECLARE_ATTACHED_CONTAINER(Jet, JetCollection)
    DECLARE_ATTACHED_CONTAINER(CaloTower, CaloTowerContainer)
    DECLARE_ATTACHED_CONTAINER(egDetail, egDetailContainer)
//    DECLARE_ATTACHED_CONTAINER_TYPEDEF(Trk, Track, DataVector<Trk::Track>)
    DECLARE_ATTACHED_CONTAINER_OBJECT_NAMESPACE(Trk, VxCandidate, VxContainer)

#undef SPECIALIZE_CONTAINER_FOR_CLASS
#undef DECLARE_ATTACHED_CONTAINER
#undef DECLARE_ATTACHED_CONTAINER_NAMESPACE
#undef DECLARE_ATTACHED_CONTAINER_TYPEDEF
#undef DECLARE_ATTACHED_CONTAINER_OBJECT_NAMESPACE

// now we set up the traits for the L1 items

// this macro setups a template specialization
// for the L1 objects
#define DECLARE_L1_TRIGGER_OBJECT(x) \
    class x; \
    namespace TrigMatch { \
        template<> \
        struct ClassTraits<x> { \
            typedef AncestorAttached type; \
        }; \
    }

    DECLARE_L1_TRIGGER_OBJECT(Muon_ROI)
    DECLARE_L1_TRIGGER_OBJECT(Jet_ROI)
    DECLARE_L1_TRIGGER_OBJECT(EmTau_ROI)

#undef DECLARE_L1_TRIGGER_OBJECT

// We can use a similar solution to set the default metrics for various types
// of objects

namespace TrigMatch {

    // by default, we assume a class uses the DeltaRDistanceFunctor
    template<typename T, typename U>
    struct MetricTraits {
        typedef DeltaRDistanceFunctor<T,U> type;
    };

}

// this macro is how we add a new delta metric for a type
// DECLARE_DEFAULT_METRIC(x,y,z) sets y as the default left
// metric for x, and z as the default right metric for x
#define DECLARE_DEFAULT_METRIC(x, y, z) \
    class x; \
    namespace TrigMatch { \
        template<typename U> \
        struct MetricTraits<x,U> { \
            typedef y<U> type; \
        }; \
        template<typename U> \
        struct MetricTraits<U,x> {\
            typedef z<U> type; \
        }; \
    }

// this macro is used to set a default metric for a pair
// This sets z as the default metric for pairs x,y
#define DECLARE_DEFAULT_METRIC_PAIR(x,y,z) \
    namespace TrigMatch { \
        template<> \
        struct MetricTraits<x,y> { \
            typedef z<x,y> type; \
        }; \
        template<> \
        struct MetricTraits<y,x> { \
            typedef z<y,x> type; \
        }; \
    }

// this macro is used to set a default metric for an
// object with itself
#define DECLARE_DEFALUT_METRIC_SINGLE(x,z) \
    namespace TrigMatch { \
        template<> \
        struct MetricTraits<x,x> { \
            typedef z<x,x> type; \
        }; \
    }
        
class TrigMuonEFInfo;
class egamma;
class EmTau_ROI;
class TrigElectron;

namespace Analysis {
    class Electron;
    class Photon;
}

DECLARE_DEFAULT_METRIC(TrigMuonEFInfo, TrigMuonEFInfoDistanceFunctor, TrigMuonEFInfoDistanceFunctorRight)
namespace TrigMatch { template<> struct MetricTraits< TrigMuonEFInfo, TrigMuonEFInfo > { typedef TrigMuonEFInfoDistanceFunctorBoth type; }; }
//DECLARE_DEFAULT_METRIC_SINGLE(egamma, ClusterDistanceFunctor)
namespace TrigMatch { template<> struct MetricTraits<egamma,egamma> { typedef ClusterDistanceFunctor<egamma,egamma> type; }; }
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Electron, egamma, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Photon, egamma, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(egamma, TrigElectron, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Electron, TrigElectron, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Photon, TrigElectron, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(egamma, EmTau_ROI, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Electron, EmTau_ROI, ClusterDistanceFunctor)
DECLARE_DEFAULT_METRIC_PAIR(Analysis::Photon, EmTau_ROI, ClusterDistanceFunctor)
    
#undef DECLARE_DEFAULT_METRIC
#undef DECLARE_DEFAULT_METRIC_PAIR
#undef DECLARE_DEFAULT_METRIC_SINGLE

#endif
