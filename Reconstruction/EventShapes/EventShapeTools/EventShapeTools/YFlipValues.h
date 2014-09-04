/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPETOOLS_YFLIPVALUES_H
#define EVENTSHAPETOOLS_YFLIPVALUES_H

#include "EventShapeTools/EventShapeToolBase.h"

namespace fastjet
{
  class JetDefinition;
  class ClusterSequence;
}

class YFlipValues : public EventShapeToolBase
{
 public:

  YFlipValues(const std::string& type,
	      const std::string& name,
	      const IInterface* pParent);
  
  virtual StatusCode initialize();
  
  virtual StatusCode calculate(const EventShapeTypes::es_4vect_vect_t* theParticles,
			       EventShapeStore *eventShapeStore );
  
 private:
  
  fastjet::JetDefinition*   m_jetdefinition;
  
  fastjet::ClusterSequence* m_clusterSeq;
  
  // options for jet definition
  std::string m_algorithm;
  
  double m_radius;
  
  std::string m_recombscheme;
  
  std::string m_strategy;
  
  // some finer options
  bool m_inclusive;
  
  unsigned int m_no_flipValues;

  // added 2D option
  bool m_calc3D;
  
};

#endif
