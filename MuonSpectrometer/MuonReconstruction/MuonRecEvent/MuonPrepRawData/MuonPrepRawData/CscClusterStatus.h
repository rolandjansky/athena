/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CscClusterStatus.h
//   Header file for class CscClusterStatus
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement ClusterStatus for Muon - CSC
///////////////////////////////////////////////////////////////////
// Niels van Eldik
///////////////////////////////////////////////////////////////////

#ifndef MUONPREPRAWDATA_CSCCLUSTERSTATUS_H
#define MUONPREPRAWDATA_CSCCLUSTERSTATUS_H

#include <string>

namespace  Muon {
    /** Enum to represent the cluster status - see the specific enum values for more details*/ 
  enum CscClusterStatus {

    /** Clean cluster with precision fit */
    CscStatusUnspoiled =0,

    /** Cluster with non-precision fit. May otherwise be clean. **/
    CscStatusSimple =1,

    // Spoiled cluster list, it should be handled with care in reconstruction
    
    /** Cluster reaches the edge of plane **/
    CscStatusEdge =2,

    /** More than one peak in cluster **/
    CscStatusMultiPeak =3,

    /** Too narrow **/
    CscStatusNarrow =4,

    /** Too wide **/
    CscStatusWide =5,

    /** Skewed, e.g. peak not at center **/
    CscStatusSkewed =6,

    /** Positions from Qrat_left and Qrat_right is not consistent **/
    CscStatusQratInconsistent =7,

    // Parabola interpolation failed in peak strip
    // OR either left or right strip is hot or dead
    CscStatusStripFitFailed = 8,

    // any of left/peak/right strip is saturated...
    CscStatusSaturated = 9,

    /** Clean cluster with precision fit after split cluster **/
    CscStatusSplitUnspoiled =10,

    /** Cluster with non-precision fit after split cluster.
        May otherwise be clean. **/
    CscStatusSplitSimple =11,

    // Spoiled cluster list, it should be handled with care in reconstruction
    
    /** Cluster reaches the edge of plane after split cluster **/
    CscStatusSplitEdge =12,
    
    /** More than one peak in cluster after split cluster**/
    CscStatusSplitMultiPeak =13,

    /** Too narrow after split cluster**/
    CscStatusSplitNarrow =14,

    /** Too wide **/
    CscStatusSplitWide =15,

    /** Skewed, e.g. peak not at center after split cluster **/
    CscStatusSplitSkewed =16,
    
    /** Positions from Qrat_left and Qrat_right is not consistent
        after split cluster **/
    CscStatusSplitQratInconsistent =17,
    
    // Parabola interpolation failed in peak strip
    // OR either left or right strip is hot or dead
    CscStatusSplitStripFitFailed = 18,

    // any of left/peak/right strip is saturated...
    CscStatusSplitSaturated = 19,

    /** Undefined, should not happen, most likely indicates a problem */
    CscStatusUndefined =255
  };

  /** Return a string description of a CSC cluster status flag. */
  inline std::string toString(CscClusterStatus cstat) {
    switch ( cstat ) {
    case CscStatusUnspoiled:         return "unspoiled";
    case CscStatusSimple:            return "simple";
    case CscStatusEdge:              return "edge";
    case CscStatusMultiPeak:         return "multiple peaks";
    case CscStatusNarrow:            return "narrow";
    case CscStatusWide:              return "wide";
    case CscStatusSkewed:            return "skewed";
    case CscStatusQratInconsistent:  return "inconsistent QRAT fits";
    case CscStatusStripFitFailed:    return "stripfit failed";
    case CscStatusSaturated:         return "saturated";

    case CscStatusSplitUnspoiled:    return "unspoiled with split";
    case CscStatusSplitSimple:       return "simple with split";
    case CscStatusSplitEdge:         return "edge with split";
    case CscStatusSplitMultiPeak:    return "multiple peaks with split";
    case CscStatusSplitNarrow:       return "narrow with split";
    case CscStatusSplitWide:         return "wide with split";
    case CscStatusSplitSkewed:       return "skewed with split";
    case CscStatusSplitQratInconsistent:  return "inconsistent QRAT fits with split";
    case CscStatusSplitStripFitFailed:    return "stripfit failed with split";
    case CscStatusSplitSaturated:         return "saturated with split";
    case CscStatusUndefined:         return "undefined";
    }
    return "unknown";
  }

}

#endif
