/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CaloGeometryFromFile_h
#define CaloGeometryFromFile_h

#include "ISF_FastCaloSimParametrization/CaloGeometry.h"

class CaloGeometryFromFile:public CaloGeometry {
public :
   CaloGeometryFromFile();
   virtual ~CaloGeometryFromFile();
   
   virtual bool LoadGeometryFromFile(TString filename,TString treename);
   virtual void LoadFCalGeometryFromFiles(TString filename1,TString filename2,TString filename3);
   void DrawFCalGraph(int isam,int color);
};

#endif

