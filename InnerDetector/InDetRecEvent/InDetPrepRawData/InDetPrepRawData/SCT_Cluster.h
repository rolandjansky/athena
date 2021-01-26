/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// SCT_Cluster.h
//   Header file for class SCT_Cluster
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

class SCT_Cluster final : public SiCluster {
 public:

  /// Constructor without parameters
  SCT_Cluster();
  /// Copy constructor
  SCT_Cluster(const SCT_Cluster &);
  /// Move constructor
  SCT_Cluster(SCT_Cluster &&);
  /// Assignment operator
  SCT_Cluster &operator=(const SCT_Cluster &);
  /// Move assignment operator
  SCT_Cluster &operator=(SCT_Cluster &&);

  /**
   * Constructor with parameters using pointer of Amg::MatrixX.
   * Last parameter might not be always filled and will be nullptr by default.
   * The others including SiDetectorElement have to be given!
   */
  SCT_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX* locErrMat
              );

  /**
   * Constructor with parameters using unique_ptr of Amg::MatrixX.
   * All parameters have to be given!
   * For use by tp converter.
   */
  SCT_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                std::vector<Identifier>&& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                std::unique_ptr<const Amg::MatrixX> locErrMat
              );

    /// dump information about the PRD object.
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /// dump information about the PRD object.
    virtual std::ostream& dump( std::ostream& stream) const;

    /// Getter method of timing.
    /// Some information about timing - which strips had 010 and which 011 for first 16 strips in a cluster.
    /// Get up to 16 strips.
    uint16_t hitsInThirdTimeBin() const;

    /// Setter method of timing.
    /// Some information about timing - which strips had 010 and which 011 for first 16 strips in a cluster.
    /// Set up to 16 strips
    void setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin);

    /// Getter method of timing.
    /// Some information about timing - which strips had 010 and which 011 for first 16 strips in a cluster.
    /// Get only one strip.
    int stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const;
    
    
 private:
    /// Some information about timing - which strips had 010 and which 011 for first 16 strips in a cluster.
    uint16_t m_hitsInThirdTimeBin;

};

 inline uint16_t SCT_Cluster::hitsInThirdTimeBin() const
 {
   return m_hitsInThirdTimeBin;  
 }
 
 inline void SCT_Cluster::setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin) 
 {
   m_hitsInThirdTimeBin = hitsInThirdTimeBin;
 }
 

 inline int SCT_Cluster::stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const {
   return stripNumberWithinCluster <= 16 ? (int)((m_hitsInThirdTimeBin >> stripNumberWithinCluster) & 0x1): 0;
   
 }
 


 MsgStream&    operator << (MsgStream& stream,    const SCT_Cluster& prd);
 std::ostream& operator << (std::ostream& stream, const SCT_Cluster& prd);


}
#endif // TRKPREPRAWDATA_SCT_CLUSTER_H
