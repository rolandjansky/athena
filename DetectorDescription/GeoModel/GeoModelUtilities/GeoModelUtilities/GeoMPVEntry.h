/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoMPVEntry_h
#define GeoMPVEntry_h 1

class GeoMPVEntry 
{
 public:
  bool operator <(const GeoMPVEntry &right) const;	
  bool operator ==(const GeoMPVEntry &right) const;
  GeoMPVEntry& operator =(const GeoMPVEntry &right);

  GeoMPVEntry(double aPhotonMomentum, double aPropertyValue); 
  GeoMPVEntry(const GeoMPVEntry &right);
  ~GeoMPVEntry();

  double GetPhotonMomentum();
  double GetProperty();
	
  void DumpEntry();

 private:
  double m_thePhotonMomentum;
  double m_theProperty;
};

inline double GeoMPVEntry::GetPhotonMomentum()
{
  return m_thePhotonMomentum;
}

inline double GeoMPVEntry::GetProperty()
{
  return m_theProperty;
}

#endif 
