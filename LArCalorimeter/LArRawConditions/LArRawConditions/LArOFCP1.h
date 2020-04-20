/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LAROFCP1
#define LARRAWCONDITIONS_LAROFCP1

#include "LArElecCalib/ILArOFC.h"
#include "LArRawConditions/LAr2DWaveBase.h"
#include <vector> 

class LArCompactSubsetChannelProxy;
class LArCompactSubsetConstChannelProxy;

/** c-struct reproducing the structure of the persistent data
 * @author W. Lampl, S. Laplace
 * @version  0-0-1 , 12/12/03 */


class LArOFCP1
  : public LAr2DWaveBase
{
public:

  typedef ILArOFC::OFCRef_t OFCRef_t;

  LArOFCP1() {}
  LArOFCP1(float timeOffset,
           float timeBinWidth,
           const std::vector<std::vector<float> >& vOFC_a,
           const std::vector<std::vector<float> >& vOFC_b);
  LArOFCP1(float timeOffset,
           float timeBinWidth,
           unsigned int nPhases,
           unsigned int nSamples,
           const std::vector<float>& ofc_a,
           const std::vector<float>& ofc_b,
           unsigned int index);
  LArOFCP1(const LArCompactSubsetChannelProxy& other);
  LArOFCP1(const LArCompactSubsetConstChannelProxy& other);

  size_t OFC_aSize() const { return waveSize(0); }
  size_t OFC_bSize() const { return waveSize(1); }

  OFCRef_t OFC_a (size_t tbin) const { return wave(0, tbin); }
  OFCRef_t OFC_b (size_t tbin) const { return wave(1, tbin); }
};


/**
 * When storing these data in a LArConditionsSubset, use LArCompactSubset.
 */

#include "LArRawConditions/LArConditionsSubset.h"     
#include "LArRawConditions/LArCompactSubset.h"     


template <>
class LArConditionsSubsetTraits<LArOFCP1>
{
public:
  typedef unsigned int  FebId; 
  typedef LArCompactSubsetChannelProxy        Reference;
  typedef LArCompactSubsetConstChannelProxy   ConstReference;
  typedef LArCompactSubsetChannelPointer      Pointer;
  typedef LArCompactSubsetChannelPointer      ConstPointer;
  typedef LArCompactSubsetChannelVector       ChannelVector; 
  typedef LArCompactSubsetConstChannelVector  ConstChannelVector; 
  typedef LArCompactSubsetChannelVectorPointer  ChannelVectorPointer;
  typedef LArCompactSubsetFebPair             FebPair;
  typedef FebPair                             FebPairReference;
  typedef LArCompactSubsetVector              SubsetVector;

  static ConstReference empty()
  {
    return ConstReference();
  }


  template <class OTHERIT, class COPIER>
  static void copySubset (OTHERIT otherBeg,
                          OTHERIT otherEnd,
                          SubsetVector& to,
                          COPIER copier)
  {
    SubsetVector::copySubset<LArOFCP1> (otherBeg, otherEnd,
                                        to, copier);
  }
};


#endif 

