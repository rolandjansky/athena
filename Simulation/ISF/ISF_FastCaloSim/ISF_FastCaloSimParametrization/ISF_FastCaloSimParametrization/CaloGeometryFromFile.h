/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeometryFromFile_h
#define CaloGeometryFromFile_h

#include "ISF_FastCaloSimParametrization/CaloGeoGeometry.h"


class CaloGeometryFromFile:public CaloGeoGeometry {
public :
   CaloGeometryFromFile();
   virtual ~CaloGeometryFromFile();
   
   virtual bool LoadGeometryFromFile(TString filename,TString treename);
   virtual bool LoadFCalGeometryFromFiles(TString filename1,TString filename2,TString filename3);
   void DrawFCalGraph(int isam,int color);
};

#endif

