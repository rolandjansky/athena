/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTSHAPES_EVENTSHAPES_H
#define EVENTSHAPES_EVENTSHAPES_H

#include <string>
#include <vector>

class EventShapes
{
 public:
  
  template<class T>
    friend class EventShapeStoreConverterBase;
  
  enum EventShapeType {
    THRUST           = 0x0001,   /* Thrust */
    THRUST_MINOR     = 0x0002,   /* Thrust Minor */
    HEAVY_JETMASS    = 0x0007,   /* heavy jet mass */
    LIGHT_JETMASS    = 0x0008,   /* light jet mass */
    TOTAL_BROADENING = 0x0009,   /* total jet broadening */
    WIDE_BROADENING  = 0x000A,   /* wide-jet broadening */
    THRUST_PHI       = 0x000B,   /* phi value of Thrust axis */
    THRUST_ETA       = 0x000C,   /* eta value of Thrust axis, if calculated in 3 dimensions */
    
    TRANSVERSESPHERICITY = 0x000D, /* transverse sphericity defined in JHEP08(2000) 017 */
    
    CPARAMETER       = 0x0010,   /* C parameter */
    
    FOXWOLFRAM       = 0x0080,   /* Fox Wolfram H0 */
    FLIPVALUES       = 0x0081,   /* flipvalue y23 */
    EEC_PHI          = 0x0082,   /* EEC along phi */
    EEC_ETA          = 0x0083,   /* EEC along eta */
    EEC_RAD          = 0x0084,   /* EEC along rad */
    
    FORWARD_TERM     = 0x00F0,   /* exponentially suppressed forward term */
    CENTRAL_CUT      = 0x00F1,   /* cut for central region and forward region */
    JET_ETA_CUT      = 0x00F2,   /* cut for eta of first two jets */
    JET1_ET          = 0x00F3,   /* E_T of jet 1 */
    JET2_ET          = 0x00F4,   /* E_T of jet 2 */
    
    UNKNOWN          = 0xFFFF    /* unknown */
  };
  
  EventShapes() : m_type(UNKNOWN) {};
  
  EventShapes(EventShapeType type, double value)
   : m_type(type)
  { m_values.push_back(value); };

  EventShapes(EventShapeType type, std::vector<double>& values, bool canSwap = false)
   : m_type(type)
  {
    if ( canSwap )
      m_values.swap(values);
    else
      m_values = values;
  };
  
  ~EventShapes() {};
  
  EventShapeType getType() const { return m_type; };
  
  unsigned int getSize() const { return m_values.size(); };
  
  double getValue( unsigned int index = 0 ) const
  {
    return index < m_values.size() ? m_values[index] : 0;
  };
  
  void getValues( std::vector<double>& vec ) const
  {
    vec.resize( m_values.size() );
    for ( unsigned int i=0; i<m_values.size(); ++i )
      vec[i] = m_values[i];
    return;
  };
  
  void swapValues( EventShapes& es )
  {
    m_values.swap(es.m_values);
  };
  
  void setValue( double value, unsigned int index = 0 )
  {
    if ( index > m_values.size() )
      m_values.resize(index, 0);
    m_values[index]=value;
    return;
  };
  
  void setValues( std::vector<double>& values, bool canSwap = false )
  {
    if ( canSwap )
      {
	m_values.swap(values);
      }
    else
      {
	m_values.resize( values.size() );
	for ( unsigned int i=0; i<values.size(); ++i )
	  m_values[i] = values[i];
      }
    return;
  };
  
  std::string getName();
  
 private:
  
  EventShapeType m_type;
  
  std::vector<double> m_values;
};  

#include "CLIDSvc/CLASS_DEF.h"
CLASS_DEF(EventShapes, 255442581, 1)

#endif
