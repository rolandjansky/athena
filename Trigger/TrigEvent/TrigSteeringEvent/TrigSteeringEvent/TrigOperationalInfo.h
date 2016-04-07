/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
#ifndef TRIGSTEERINGEVENT_TRIGOPERATIONALINFO_H
#define TRIGSTEERINGEVENT_TRIGOPERATIONALINFO_H


#include <map>
#include <string>
#include <vector>
#include "GaudiKernel/MsgStream.h"

#include "CLIDSvc/CLASS_DEF.h"
// TrigOperationalInfoCollection is included at the end (required by Trigger EDM schema)

//#include "TrigNavigation/Navigation.h"
/**
 * @brief This class defined generic object to store operational info 
 *   available during triggering online i.e. timing infomration.
 * To avoid various problems reated to schema evolution this are
 * just key: value pairs while value is always of type float (OK we can think about templating this for int and float maybe)
 * 
 */

class TrigOperationalInfo {

public:
  TrigOperationalInfo();
  TrigOperationalInfo(const std::vector<std::string>& keys,
                      const std::vector<float>& values);
  unsigned int    defined(const std::string& name) const;
  float  get(const std::string& name) const;
  void   set(const std::string& name, float value);
  std::pair<std::vector<std::string>, std::vector<float> > infos() const;
  const std::vector<std::string>& getKeys() const;
  const std::vector<float>& getValues() const;
  void updateAtLocation(unsigned int location, float value);

private:
  std::vector<std::string> m_infoName;
  std::vector<float> m_infoValue;  
};



std::string str( const TrigOperationalInfo& d );                      //<! printing helper
MsgStream& operator<< ( MsgStream& m, const TrigOperationalInfo& d ); //<! printing helper (wraps above)


bool operator== ( const TrigOperationalInfo& a, const TrigOperationalInfo& b ); 
inline bool operator!= ( const TrigOperationalInfo& a, const TrigOperationalInfo& b ) { return !(a==b); }
void diff( const TrigOperationalInfo& a, const TrigOperationalInfo& b, std::map<std::string, double>& variableChange ); //<! comparison with feedback (see: TrigRoiDescriptor)

CLASS_DEF( TrigOperationalInfo , 242645981 , 1 )

#include "TrigSteeringEvent/TrigOperationalInfoCollection.h"

#endif
