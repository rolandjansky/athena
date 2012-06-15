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
  double thePhotonMomentum;
  double theProperty;
};

inline double GeoMPVEntry::GetPhotonMomentum()
{
  return thePhotonMomentum;
}

inline double GeoMPVEntry::GetProperty()
{
  return theProperty;
}

#endif 
