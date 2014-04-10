/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// SCT_Cluster.h
//   Header file for class SCT_Cluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class derived from SiCluster to given a different class type
// as a trick to get the convertors working. Does not really 
// externd SiCluster.
///////////////////////////////////////////////////////////////////
// Version 1.0 23/07/2004 John Baines
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_SCT_CLUSTER_H
#define TRKPREPRAWDATA_SCT_CLUSTER_H

// Base class
#include "InDetPrepRawData/SiCluster.h"

#include <vector>

class Identifier;
class MsgStream;

namespace InDetDD
{
  class SiDetectorElement;
}

namespace InDet{

class SiWidth;

class SCT_Cluster : public SiCluster {
 public:
  /** Public, Copy, operator=, constructor*/
  SCT_Cluster();
  SCT_Cluster(const SCT_Cluster &);
  SCT_Cluster &operator=(const SCT_Cluster &);

  SCT_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX* locErrMat
              );

    /** dump information about the PRD object. */
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** dump information about the PRD object. */
    virtual std::ostream& dump( std::ostream& stream) const;

};

 MsgStream&    operator << (MsgStream& stream,    const SCT_Cluster& prd);
 std::ostream& operator << (std::ostream& stream, const SCT_Cluster& prd);


}
#endif // TRKPREPRAWDATA_SCT_CLUSTER_H
