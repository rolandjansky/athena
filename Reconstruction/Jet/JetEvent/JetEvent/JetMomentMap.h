// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETEVENT_JETMOMENTMAP_H
#define JETEVENT_JETMOMENTMAP_H

#include "CLIDSvc/CLASS_DEF.h"

#include "DataModel/ElementLinkVector.h"

#include "JetEvent/JetMapBase.h"
#include "JetEvent/JetMoment.h"

#include <vector>
#include <map>

class JetMomentMap : public JetMapBase<JetMoment::MomentData>
{
public:

  // make the JetCollection a friend
  friend class JetCollection;

  typedef JetMoment::float_t    float_t;
  typedef JetMoment::tag_t      tag_t;
  typedef JetMoment::MomentData data_t;
  typedef JetMapBase<data_t>    base_t;
  typedef base_t::map_t         map_t;
  typedef base_t::record_t      record_t;
  
  JetMomentMap();
  virtual ~JetMomentMap();
  
  virtual bool    setMoment(size_t jetIndex,size_t keyIndex,float_t  moment);
  virtual bool    setMoment(size_t jetIndex,size_t keyIndex,float_t  moment)
    const;
  virtual bool    getMoment(size_t jetIndex,size_t keyIndex,float_t& moment) 
    const;
  virtual float_t getMoment(size_t jetIndex,size_t keyIndex) const;

  // dummy implementation, needed for the DataLink to call the converter
  void access() const { };


protected:
 
};

CLASS_DEF(JetMomentMap, 117927529, 1)


class OrphanJetMomentMap : public JetMomentMap, public OrphanJetCounter {
public:
  friend class JetGlobalEventSetup;

  static OrphanJetMomentMap instance;
};

  
/*! \class JetMomentMap
 *
 *  \author Peter Loch <loch@physics.arizona.edu>
 *  \author Rolf Seuster <Rolf.Seuster@cern.ch>
 *  \author Michel Lefebvre <lefebvre@uvic.ca>
 *
 *  \date June 29, 2006 
 *  
 *  DEPRECIATED. Jets now store moment internally
 *
 *  Keyed store for jet moments. The key is the pointer to the \a Jet, and the
 *  data is a \a JetMomentStore. 
 */

#endif
