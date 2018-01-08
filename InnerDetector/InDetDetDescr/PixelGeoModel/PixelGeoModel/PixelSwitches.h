/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PixelSwitches_H
#define PixelSwitches_H

#include <string>

// Class to hold the various switches and building options.

class PixelSwitches 
{

public:
  PixelSwitches();

  // Do I want the services?
  void setServices(bool flag);
  bool services() const;

  // JBdV Do I want the services?
  void setServicesOnLadder(bool flag);
  bool servicesOnLadder() const;

  // Make elements compatible with G3 digits
  void setG3CompatibleDigits(bool flag);
  bool g3CompatibleDigits() const;
  
  // Initial layout (2nd layer missing)
  void setInitialLayout(bool flag);
  bool initialLayout() const;
  
  // DC1 Geometry. 300 um long pixels and 200 um thick sensor in B layer. 
  void setDC1Geometry(bool flag);
  bool dc1Geometry() const;
  
  // Control whether callbacks get registered
  void setAlignable(bool flag);
  bool alignable() const;

  // SLHC
  void setSLHC(bool flag = true);
  bool slhc() const;

  // IBL
  void setIBL(bool flag = true);
  bool ibl() const;

  // DBM
  void setDBM(bool flag = false);
  bool dbm() const;

private:

  // flag to build also the services
  bool m_services;

  // JBdV flag to build also the servicesOnLadder
  bool m_servicesOnLadder;

  // flag to make elements compatible with G3 digits
  bool m_g3CompatibleDigits;

  // flag to indicate initial layout (2nd layer and disk missing)
  bool m_initialLayout;
  
  // flag to force B-layer to have 300um pixels and be 200um thick
  bool m_dc1Geometry;

  // control whether callbacks get registered 
  bool m_alignable;

  // SLHC
  bool m_slhc;

  // IBL
  bool m_ibl;

  // DBM
  bool m_dbm;

};

#endif // PixelSwitches_H




