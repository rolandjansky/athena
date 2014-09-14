// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVSTRUCTURE_TRIGGERELEMENTFACTORY_H
#define TRIGNAVSTRUCTURE_TRIGGERELEMENTFACTORY_H

#include <vector>
#include "TrigNavStructure/Types.h"
#include "TrigNavStructure/TriggerElement.h"

namespace HLT {
  /**
   * \brief The factory of TriggerElements 
   * Creation in controlled way.
   *
   * Main reason for this class is to allow controlled creation of the TEs 
   * (may be optimized to reuse the TEs from event to event)
   *
   * This can be much smarter to:
   *
   * - Beeing able to produce any object (like TiggerStore)
   * - Beeing able to hold forigen objects (no control over them, like)
   * .
   */
  class TriggerElementFactory {
  public:
    TriggerElementFactory();
    ~TriggerElementFactory();
    TriggerElement* produce(unsigned int id, bool ghost=false, bool nofwd=false);
    void reset();
    bool empty() const;

    const std::vector< TriggerElement* >& listOfProduced() const { return m_list; }
    std::vector< TriggerElement* >& listOfProduced() { return m_list; }
    const std::vector< TriggerElement* >& listOfProduced(unsigned int id) const { 
      return m_groupedById.find(id)->second; 
    }
    std::vector< TriggerElement* >& listOfProduced(unsigned int id) { 
      return m_groupedById.find(id)->second; 
    }

  private:
    std::vector< TriggerElement* > m_list;
    std::map<te_id_type, std::vector<TriggerElement*> > m_groupedById; //!< this is to speed up queries
  };
} // eof namespace HLT
#endif //
