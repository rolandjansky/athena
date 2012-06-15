/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GeoMaterialPropertyVector_h   
#define GeoMaterialPropertyVector_h 1

#include "GeoModelUtilities/GeoMPVEntry.h"
#include <vector>

class GeoMaterialPropertyVector 
{
public:
  bool operator ++();
  GeoMaterialPropertyVector&
    operator =(const GeoMaterialPropertyVector &right);

  GeoMaterialPropertyVector(): MPV(0) 
  {
    CurrentEntry = -1;
    NumEntries   = 0;
  };

  GeoMaterialPropertyVector(double *PhotonMomenta, 
			    double *PropertyValues,
			    int     NumElements);

  GeoMaterialPropertyVector(const GeoMaterialPropertyVector &right);

  ~GeoMaterialPropertyVector();

  void ResetIterator();

  // Add a new element (pair of numbers) to the GeoMaterialPropertyVector.
  void AddElement(double aPhotonMomentum, 
		  double aPropertyValue);

  double GetProperty() const;
  double GetPhotonMomentum() const;

  // Dump contents
  void DumpVector();	

private:
  GeoMPVEntry GetEntry(int i) const;

  std::vector<GeoMPVEntry*> MPV;
  int NumEntries;
  int CurrentEntry;
};

#endif 
