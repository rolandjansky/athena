/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// -*-c++-*-
// TopOverlapTrigger - TopOverlap with trigger implementation

#ifndef TOP_OVERLAP_TRIGGER_H
#define TOP_OVERLAP_TRIGGER_H

#include "TopOverlap.h"

class TopOverlapTriggerCnv_p1;

template<class Container>
class TopInputMapTriggerCnv_p1;
template<class Container>
class TopInputMapTriggerCnv_p2;

namespace TopRec {

  class TopOverlapTrigger : public TopOverlap {

    public:

      TopOverlapTrigger(){};
      ~TopOverlapTrigger(){};

      bool inTrigger() const	{ return m_inTrigger; }
      void setInTrigger(const bool& inTrig) { m_inTrigger = inTrig; }

    protected:

    private:

      bool m_inTrigger;

      friend class ::TopOverlapTriggerCnv_p1;
      template<class Container> friend class ::TopInputMapTriggerCnv_p1;
      template<class Container> friend class ::TopInputMapTriggerCnv_p2;

  }; // TopOverlapTrigger class definition

} // TopRec namespace

//CLASS_DEF(TopRec::TopOverlapTrigger,95687088,1)

#endif // TOP_OVERLAP_TRIGGER_H
