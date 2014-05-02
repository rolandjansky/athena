/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRAWCONDITIONS_LARSHAPEP2
#define LARRAWCONDITIONS_LARSHAPEP2

#include "LArElecCalib/ILArShape.h"
#include "LArRawConditions/LAr2DWaveBase.h"
#include <vector> 

class LArCompactSubsetChannelProxy;

/** c-struct reproducing the structure of the persistent data
 * @author M. Delmastro
 * @version  0-0-1 , 13/06/2008 */

class LArShapeP2
  : public LAr2DWaveBase
{
public:      

  typedef ILArShape::ShapeRef_t ShapeRef_t;

  LArShapeP2() {}
  LArShapeP2(float timeOffset,
             float timeBinWidth,
	     const std::vector<std::vector<float> >& vShape,
	     const std::vector<std::vector<float> >& vShapeDer);
  LArShapeP2(float timeOffset,
             float timeBinWidth,
             unsigned int nPhases,
             unsigned int nSamples,
             const std::vector<float>& shape,
             const std::vector<float>& shapeDer,
             unsigned int index);
  LArShapeP2(const LArCompactSubsetChannelProxy& other);

  size_t shapeSize() const { return waveSize(0); }
  size_t shapeDerSize() const { return waveSize(1); }

  ShapeRef_t shape (size_t tbin) const { return wave(0, tbin); }
  ShapeRef_t shapeDer (size_t tbin) const { return wave(1, tbin); }
};



/**
 * When storing these data in a LArConditionsSubset, use LArCompactSubset.
 */

#include "LArRawConditions/LArConditionsSubset.h"     
#include "LArRawConditions/LArCompactSubset.h"     


template <>
class LArConditionsSubsetTraits<LArShapeP2>
{
public:
  typedef unsigned int  FebId; 
  typedef LArCompactSubsetChannelProxy        Reference;
  typedef LArCompactSubsetChannelProxy        ConstReference;
  typedef LArCompactSubsetChannelPointer      Pointer;
  typedef LArCompactSubsetChannelPointer      ConstPointer;
  typedef LArCompactSubsetChannelVector       ChannelVector; 
  typedef LArCompactSubsetChannelVectorPointer  ChannelVectorPointer;
  typedef LArCompactSubsetFebPair             FebPair;
  typedef FebPair                             FebPairReference;
  typedef LArCompactSubsetVector              SubsetVector;

  static ConstReference empty()
  {
    return ConstReference();
  }
};


#endif 

