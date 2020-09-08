/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StaveServices_H
#define StaveServices_H

#include "InDetServMatGeoModel/DetType.h"

#include <vector>

/** Class storing all the services of a stave (or a petal, etc...) 
    classifying them by function and by number of items.
    For example, it specifies the number of HV cables for each stave type.
 */
class StaveServices {
public:

  StaveServices( DetType::Type type, DetType::Part part, int layer,
		 int nhv, int ndcs, int ndata, const std::vector<int>& lvGauge) :
    m_type( type), m_part(part), m_layer(layer), 
    m_numHV(nhv), m_numDCS( ndcs), m_numData( ndata), m_lvGauge( lvGauge) {}
  
  StaveServices( DetType::Type type, DetType::Part part, int layer,
		 int nhv, int ndcs, int ndata, int lvGauge) :
    m_type( type), m_part(part), m_layer(layer), 
    m_numHV(nhv), m_numDCS( ndcs), m_numData( ndata), m_lvGauge( 1, lvGauge) {}
  
  DetType::Type type() const { return m_type;} 
  DetType::Part part() const { return m_part;}
  int           layer() const { return m_layer;}

  int numHV() const { return m_numHV;}
  int numDCS() const { return m_numDCS;}
  int numData() const { return m_numData;}
  int numLvTypes() const { return m_lvGauge.size();}
  int lvGauge( int lvType) const { return m_lvGauge.at( lvType);}

private:

  DetType::Type m_type;
  DetType::Part m_part;
  int m_layer;
  int m_numHV;
  int m_numDCS;
  int m_numData;
  std::vector<int> m_lvGauge;

};

#endif
