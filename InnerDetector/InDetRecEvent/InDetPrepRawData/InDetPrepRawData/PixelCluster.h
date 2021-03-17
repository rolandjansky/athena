/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelCluster.h
//   Header file for class PixelCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class derived from SiCluster to given a different class type
// as a trick to get the convertors working. Does not really 
// extend SiCluster.
///////////////////////////////////////////////////////////////////
// Version 1.0 23/07/2004 John Baines
///////////////////////////////////////////////////////////////////

#ifndef TRKPREPRAWDATA_PIXELCLUSTER_H
#define TRKPREPRAWDATA_PIXELCLUSTER_H

// Base class
#include "InDetPrepRawData/SiCluster.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Identifier/Identifier.h"
#include <vector>

// Pixel splitter information stored in 32 bit
// the information is : bool, prob1, pob2
// 1, 2**15, 2**15, define the mask: 0x7fff
#ifndef SPLITMASK
#define SPLITMASK 0x7fff
#define SPLITPREC 15
#endif

class PixelRDORawData;
class MsgStream;

namespace InDetDD
{
  class SiDetectorElement;
}

namespace InDet{

  class SiWidth;
  
  class PixelCluster final : public SiCluster {
  public:

    /** Public, Copy, operator=, constructor*/
    PixelCluster();
    PixelCluster(const PixelCluster &) = default;
    PixelCluster(PixelCluster &&) = default;
    PixelCluster & operator=(const PixelCluster & rhs) = default;
    PixelCluster & operator=(PixelCluster && rhs) = default;
    

    // legacy constructor with no ToT not LVL1 information
    PixelCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  const std::vector<Identifier>& rdoList,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  const Amg::MatrixX* locErrMat,
                  const float omegax = -1.,
                  const float omegay = -1.,
                  bool split = false,
                  float splitProb1 = 0.,
                  float splitProb2 = 0.
                );

    // constructor if no charge calibration is available
    PixelCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  const std::vector<Identifier>& rdoList,
                  const int lvl1a,
                  const std::vector<int>& totList,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  const Amg::MatrixX* locErrMat,
                  const float omegax = -1.,
                  const float omegay = -1.,
                  bool split = false,
                  float splitProb1 = 0.,
                  float splitProb2 = 0.
                );

    // constructor if charge calibration is available
    PixelCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  const std::vector<Identifier>& rdoList,
                  const int lvl1a,
                  const std::vector<int>& totList,
                  const std::vector<float>& chargeList,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  const Amg::MatrixX* locErrMat,
                  const float omegax = -1.,
                  const float omegay = -1.,
                  bool split = false,
                  float splitProb1 = 0.,
                  float splitProb2 = 0.
                );

    // Constructor for use from tp converter.
    PixelCluster( 
                  const Identifier& RDOId,
                  const Amg::Vector2D& locpos, 
                  std::vector<Identifier>&& rdoList,
                  const int lvl1a,
                  int totalToT,
                  const std::vector<float>& chargeList,
                  float totalCharge,
                  const InDet::SiWidth& width,
                  const InDetDD::SiDetectorElement* detEl,
                  std::unique_ptr<const Amg::MatrixX> locErrMat,
                  const float omegax,
                  const float omegay,
                  int splitInfoRaw
                );
    
    float omegax() const; 
    float omegay() const;
    const std::vector<int>& totList() const;
    int totalToT() const;
    const std::vector<float>& chargeList() const;
    float totalCharge() const;
    /** Energy lost is in the cluster is computed from the total charge
        id a calibration could be used. Just the ToT is returned if no
        calibration is available, but with a negative sign.
        If no ToT or Charge data are available, it gives back 0 */
    float energyLoss() const;
    bool  isFake() const;
    bool  setFake(bool);
    bool  isAmbiguous() const;
    bool  setAmbiguous(bool);
    /** access mehtods for splitting */
    bool  isSplit() const;
    double splitProbability1() const;
    double splitProbability2() const;
    int splitInfoRaw() const;
    
    int   LVL1A() const;
    /** dump information about the PRD object. */
    virtual MsgStream&    dump( MsgStream&    stream) const override final;

    /** dump information about the PRD object. */
    virtual std::ostream& dump( std::ostream& stream) const override final;

    void packSplitInformation(bool split, float prob1, float prob2) ;
 
    void setTooBigToBeSplit(bool a);
    bool tooBigToBeSplit() const;

    void setToTList (std::vector<int>&& totList);


  private:
    float m_omegax;
    float  m_omegay;
    std::vector<int> m_totList;
    int   m_totalToT;
    std::vector<float> m_chargeList;
    float m_totalCharge;
    bool  m_fake;
    bool  m_ambiguous;
    int   m_lvl1;
    unsigned int m_splitInfo;
    bool  m_tooBigToBeSplit;    
  };

 MsgStream&    operator << (MsgStream& stream,    const PixelCluster& prd);
 std::ostream& operator << (std::ostream& stream, const PixelCluster& prd);

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// return omegax:
inline float  PixelCluster::omegax() const
{
	return m_omegax;
}
// return omegay:
inline float  PixelCluster::omegay() const
{
	return m_omegay;
}
// return list of ToT values:
inline const std::vector<int>&  PixelCluster::totList() const
{
	return m_totList;
}
// return total ToT:
inline int  PixelCluster::totalToT() const
{
	return m_totalToT;
}
// return list of Calibrated charge values:
inline const std::vector<float>&  PixelCluster::chargeList() const
{
	return m_chargeList;
}
// return total calibrated charge:
inline float  PixelCluster::totalCharge() const
{
	return m_totalCharge;
}

// return energy loss
inline float  PixelCluster::energyLoss() const
{
  if ( m_totalCharge>0.) return 3.6*Gaudi::Units::eV*m_totalCharge;
	else if (m_totalToT>0) return -((float)m_totalToT);
	else return 0.;
}

// return fake flag
inline bool PixelCluster::isFake() const 
{
  return m_fake;
}
// set fake flag
inline bool PixelCluster::setFake(bool flag) 
{
  return m_fake=flag;
}
// return ambiguous flag
inline bool PixelCluster::isAmbiguous() const 
{
  return m_ambiguous;
}
// pack the split information
inline void PixelCluster::packSplitInformation(bool split, float prob1, float prob2){
  m_splitInfo  =  int(prob1*SPLITMASK);
  m_splitInfo |= (int(prob2*SPLITMASK)<<SPLITPREC);
  m_splitInfo |= (int(split)<<(2*SPLITPREC+1));
}

// return the isSplit flag
inline bool PixelCluster::isSplit() const 
{
  return (m_splitInfo>>(2*SPLITPREC+1));
}

inline double PixelCluster::splitProbability1() const
{
  return ((m_splitInfo) & SPLITMASK )*(1./double(SPLITMASK));
}

inline double PixelCluster::splitProbability2() const
{
  return ((m_splitInfo>>SPLITPREC) & SPLITMASK )*(1./double(SPLITMASK));
}


inline int PixelCluster::splitInfoRaw() const
{
  return m_splitInfo;
}

 
// set ambiguous flag
inline bool PixelCluster::setAmbiguous(bool flag) 
{
  return m_ambiguous=flag;
}

inline int PixelCluster::LVL1A() const
{
  return m_lvl1;
}

inline  void PixelCluster::setTooBigToBeSplit(bool a)
{
  m_tooBigToBeSplit = a;
}

inline  bool PixelCluster::tooBigToBeSplit() const 
{ 
  return m_tooBigToBeSplit;
}


} 
#endif // TRKPREPRAWDATA_PIXELCLUSTER_H
