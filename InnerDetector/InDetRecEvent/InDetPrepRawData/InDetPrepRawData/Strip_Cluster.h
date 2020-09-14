/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// Strip_Cluster.h
//   Header file for class Strip_Cluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class derived from SiCluster to given a different class type
// as a trick to get the convertors working. Does not really 
// externd SiCluster.
///////////////////////////////////////////////////////////////////
// Version 1.0 23/07/2004 John Baines
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_STRIP_CLUSTER_H
#define TRKPREPRAWDATA_STRIP_CLUSTER_H

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

class Strip_Cluster : public SiCluster {
 public:
  /** Public, Copy, operator=, constructor*/
  Strip_Cluster();
  Strip_Cluster(const Strip_Cluster &);
  Strip_Cluster(Strip_Cluster &&);
  Strip_Cluster &operator=(const Strip_Cluster &);
  Strip_Cluster &operator=(Strip_Cluster &&);

  Strip_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                const Amg::MatrixX* locErrMat
              );

  // For use by tp converter.
  Strip_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                std::vector<Identifier>&& rdoList,
                const InDet::SiWidth& width,
                const InDetDD::SiDetectorElement* detEl,
                std::unique_ptr<const Amg::MatrixX> locErrMat
              );

    /** dump information about the PRD object. */
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** dump information about the PRD object. */
    virtual std::ostream& dump( std::ostream& stream) const;

    /** Some information about timing - which strips had 010 and which 011 */
    uint16_t hitsInThirdTimeBin() const;

    void setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin);
 
    int stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const;
    
    
 private:
    uint16_t m_hitsInThirdTimeBin;
    
      

};

 inline uint16_t Strip_Cluster::hitsInThirdTimeBin() const
 {
   return m_hitsInThirdTimeBin;  
 }
 
 inline void Strip_Cluster::setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin) 
 {
   m_hitsInThirdTimeBin = hitsInThirdTimeBin;
 }
 

 inline int Strip_Cluster::stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const {
   return stripNumberWithinCluster <= 16 ? (int)((m_hitsInThirdTimeBin >> stripNumberWithinCluster) & 0x1): 0;
   
 }
 


 MsgStream&    operator << (MsgStream& stream,    const Strip_Cluster& prd);
 std::ostream& operator << (std::ostream& stream, const Strip_Cluster& prd);


}
#endif // TRKPREPRAWDATA_STRIP_CLUSTER_H
