/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETREADOUTGEOMETRY_TRT_DETELEMENTCONTAINER_H
#define INDETREADOUTGEOMETRY_TRT_DETELEMENTCONTAINER_H

#include "TRT_DetElementCollection.h"
#include "TRT_Numerology.h"

#include <vector>

namespace InDetDD {

class TRT_BaseElement;
class TRT_BarrelElement;
class TRT_EndcapElement;
class TRT_Numerology;

/// Class to hold different TRT detector elements structures. 

class TRT_DetElementContainer{

 public:  

  TRT_DetElementContainer();
  ~TRT_DetElementContainer();
  // disable implicit copy constructor
  TRT_DetElementContainer(const TRT_DetElementContainer& other) = delete;
  // disable implicit assignment operator
  void operator = (const TRT_DetElementContainer& other) = delete;

  void setNumerology(const TRT_Numerology* mynum);

  void addBarrelElement(TRT_BarrelElement *element);

  void addEndcapElement(TRT_EndcapElement *element);

  const TRT_DetElementCollection* getElements() const;

  const TRT_Numerology* getTRTNumerology() const;

  const TRT_EndcapElement *getEndcapDetElement(unsigned int positive,                                                                                     
					    unsigned int wheelIndex,                                                            
					    unsigned int strawLayerIndex,                                                     
					    unsigned int phiIndex) const;
  const TRT_BarrelElement *getBarrelDetElement(unsigned int positive,
					    unsigned int moduleIndex,
					    unsigned int phiIndex,
					    unsigned int strawLayerIndex) const;

  void clear();

 private:

  TRT_DetElementCollection m_trtcoll;
  const TRT_Numerology  *m_trtnum;

  enum {NMODMAX=3};
  enum {NWHEELMAX=18};
  enum {NPHIMAX=32};
  enum {NSTRAWLAYMAXBR=30};
  enum {NSTRAWLAYMAXEC=16};

  TRT_BarrelElement *m_baArray[2][NMODMAX][NPHIMAX][NSTRAWLAYMAXBR]{};
  TRT_EndcapElement *m_ecArray[2][NWHEELMAX][NSTRAWLAYMAXEC][NPHIMAX]{};
  
};

} // namespace InDetDD

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( InDetDD::TRT_DetElementContainer , 1164489788, 1 )
#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( InDetDD::TRT_DetElementContainer, 1178261225 );

#endif // INDETREADOUTGEOMETRY_TRT_DETELEMENTCONTAINER_H
