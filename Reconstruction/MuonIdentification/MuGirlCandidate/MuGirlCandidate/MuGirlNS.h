/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUGIRL_MUGIRLNS_H
#define MUGIRL_MUGIRLNS_H

#include "GeoPrimitives/GeoPrimitives.h"
#include <string>
#include <vector>
#include <map>
#include <set>

#ifndef M_TWOPI
#define M_TWOPI (2.0 * M_PI)
#endif

//<S>==================================================
// stauCandidate dependencies
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonSegment/MuonSegment.h"
//end of stauCandidate dependencies
//<S>==================================================

namespace Trk
{
    class PrepRawData;
    class RIO_OnTrack;
}

namespace MuGirlNS
{
    class Intersection;
    class Station;
    class Chamber;
    class Hit;
    class Segment;
    class BBox;

    typedef BBox Road;
    typedef std::vector<Intersection*> IntersectionList;
    typedef std::vector<Station*> StationList;
    typedef std::vector<Chamber*> ChamberList;
    typedef std::map<Station*, ChamberList> StationChamberMap;  /**< A map of chambers index by Station */
    typedef std::vector<const Trk::PrepRawData*> PrepDataList;
    typedef std::vector<const Trk::RIO_OnTrack*> RIOList;   /**< A list of RIO_OnTrack objects */
    typedef std::vector<Hit*> HitList;          /**< A vector of hits */
    typedef std::vector<Segment*> SegmentList;

   enum RegionType
    {
        BARREL_REGION,
        ENDCAP_REGION,
        REGIONS
    };

    inline std::string RegionTypeName(RegionType eReg)
    {
        switch (eReg)
        {
        case BARREL_REGION: return "Barrel";
        case ENDCAP_REGION: return "Endcap";
        default: return std::string();
        }
    }
    enum DistanceType
    {
        INNER_DIST,
        EE_DIST,
        MIDDLE_DIST,
        OUTER_DIST,
        DISTANCES
    };

    inline std::string DistanceTypeName(DistanceType eDist)
    {
        switch (eDist)
        {
        case INNER_DIST:  return "Inner";
        case EE_DIST:     return "EE";
        case MIDDLE_DIST: return "Middle";
        case OUTER_DIST:  return "Outer";
        default: return std::string();
        }
    }

    inline DistanceType PreviousDistance(DistanceType eDist)
    {
        return (eDist == OUTER_DIST ? MIDDLE_DIST : INNER_DIST);
    }

    enum TechnologyType
    {
        MDT_TECH,
        RPC_TECH,
        TGC_TECH,
        CSC_TECH,
        STGC_TECH,
        MM_TECH,
        ID_TECH,
        COMB_TECH,
        ALL_TECHNOLOGIES,
        TECHNOLOGIES = ID_TECH
    };

    inline std::string TechnologyTypeName(TechnologyType eTech)
    {
        switch (eTech)
        {
        case MDT_TECH:   return "MDT";
        case RPC_TECH:   return "RPC";
        case TGC_TECH:   return "TGC";
        case CSC_TECH:   return "CSC";
        case STGC_TECH:  return "sTGC";
        case MM_TECH:    return "MM";
        case ID_TECH:    return "ID";
        case COMB_TECH:  return "COMBINED";
        default: return std::string();
        }
    }

    enum IntersectionType
    {
        CRUDE_INTERSECTION,         /**< Intersection of an ID track with a detector Region */
        FINE_INTERSECTION,          /**< Intersection of an ID track with a chamber surface */
        FIT_INTERSECTION,           /**< Intersection generated from a segment built from hits */
        EVALUATED_INTERSECTION,     /**< Intersection of a fit with a chamber surface */
        COMBINED_INTERSECTION,      /**< Computed from all infomation in a distance/region combination */
        INTERSECTIONS               /**< Number of intersection types */
    };

    inline std::string IntersectionTypeName(IntersectionType eIsect)
    {
        switch (eIsect)
        {
        case CRUDE_INTERSECTION:     return "Crude";
        case FINE_INTERSECTION:      return "Fine";
        case FIT_INTERSECTION:       return "Fit";
        case EVALUATED_INTERSECTION: return "Evaluated";
        case COMBINED_INTERSECTION:  return "Combined";
        default: return std::string();
        }
    }

    enum SegmentType
    {
        HOUGH_SEGMENT,
        LINEAR_SEGMENT,
        SEGMENTS
    };

    inline std::string SegmentTypeName(SegmentType eSeg)
    {
        switch (eSeg)
        {
        case HOUGH_SEGMENT:  return "Hough";
        case LINEAR_SEGMENT: return "Linear";
        default: return std::string();
        }
    }

    //<S>==================================================================
    // stauCandidate definitions
    #define SPEEDOFLIGHT 0.299792458    //[m/ns]
    #define RPCINVSIGVEL 4.8        //[ns/m]
    #define STEPINBETA 0.01         //The step size in the loop
    #define TDC2NS 0.78125          //1tdc=0.78125ns

    typedef std::vector<const PrepDataList*> PrepDataLists;
    typedef std::vector<RIOList*> RIOLists;
    typedef std::vector<std::vector<const Muon::MuonClusterOnTrack*> > Clusters;
    typedef std::vector<const Muon::MuonSegment*> stauSegments;

    struct stauMDTIteration
    {
        int numOfHits;
        int numOfSegments;
        int region;
        int distance;
        Amg::Vector3D globalPosition;
        Amg::Vector3D globalDirection;
        double beta;
        double chi2;
        int segmentNumber;
    };
    typedef std::vector < stauMDTIteration* > stauMDTIterations;

    struct stauMDTCombinedBeta
    {
        double beta;
        int numOfHits;
        int numOfSegments;
        bool moreThanOneSegment;
        int region;
        double combinedChi2;
    };
    typedef std::vector < stauMDTCombinedBeta* > stauMDTCombinedBetas;

    struct stauRPCHit
    {
        int distance;
        double beta;
        bool isEta;
    };
    typedef std::vector < stauRPCHit* > stauRPCHits;
    //end of stauCandidate definitions
    //<S>==================================================================
}

#endif //MUGIRL_MUGIRLNS_H
