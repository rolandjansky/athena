#ifndef StaveServices_H
#define StaveServices_H

#include "PixelServicesTool/DetTypeDyn.h"

#include <vector>

/** Class storing all the services of a stave (or a petal, etc...) 
    classifying them by function and by number of items.
    For example, it specifies the number of HV cables for each stave type.
 */
class StaveDynServices {
public:

  StaveDynServices( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer,
		 int nhv, int ndcs, int ndata, std::vector<int> lvGauge) :
    m_type( type), m_part(part), m_layer(layer), 
    m_numHV(nhv), m_numDCS( ndcs), m_numData( ndata), m_numLvTypes(0), m_lvGauge( lvGauge) {}
  
  StaveDynServices( DetTypeDyn::Type type, DetTypeDyn::Part part, int layer,
		 int nhv, int ndcs, int ndata, int lvGauge) :
    m_type( type), m_part(part), m_layer(layer), 
    m_numHV(nhv), m_numDCS( ndcs), m_numData( ndata), m_numLvTypes(0), m_lvGauge( 1, lvGauge) {}
  
  DetTypeDyn::Type type() const { return m_type;} 
  DetTypeDyn::Part part() const { return m_part;}
  int           layer() const { return m_layer;}

  int numHV() const { return m_numHV;}
  int numDCS() const { return m_numDCS;}
  int numData() const { return m_numData;}
  int numLvTypes() const { return m_lvGauge.size();}
  int lvGauge( int lvType) const { return m_lvGauge.at( lvType);}

private:

  DetTypeDyn::Type m_type;
  DetTypeDyn::Part m_part;
  int m_layer;
  int m_numHV;
  int m_numDCS;
  int m_numData;
  int m_numLvTypes;
  std::vector<int> m_lvGauge;

};

#endif
