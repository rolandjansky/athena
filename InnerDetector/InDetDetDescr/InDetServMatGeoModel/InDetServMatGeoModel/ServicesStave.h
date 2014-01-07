/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ServicesStave_H
#define ServicesStave_H

#include "InDetServMatGeoModel/DetType.h"

#include <string>

/** There are several possible types of staves from services point of view:
    - half-staves (making up half of the barrel length), which are symmetric by definition
      (the services are identical on both sides of the barrel)
    - Full-barrel-length staves with services from one side only
      - all services coming out on one barrel side
      - half of the staves with services coming on one side and half on the other
    - Full-barrel-length staves with cooling inlet on one side and outlet on other side
    - ...
    To model all these stave types we have to forsee asymmetric stave services
 **/

class ServicesStave {
public:

  // side for stave mounted on right half-barrel (for half-staves) 
  // or full barrel (for full-length staves)
  enum ConnectionSide {Pos, Neg};

  ServicesStave( DetType::Type,
		 DetType::Part,
		 int nInlets, const std::string& inletPipe,
		 int nOutlets, const std::string& outletPipe,
		 int nCables, const std::string& cable,
		 int nFibres, const std::string& fibre);

  int nInletPipes() const {return m_nInlets;}
  int nOutletPipes() const {return m_nOutlets;}
  int nCables() const {return m_nCables;}
  int nFibres() const {return m_nFibres;}

  const std::string& inletPipe() const {return m_inletPipe;}
  const std::string& outletPipe() const {return m_outletPipe;}
  const std::string& cable() const {return m_cable;}
  const std::string& fibre() const {return m_fibre;}

  ConnectionSide inletSide() const;
  ConnectionSide outletSide() const;

private:

  DetType::Type     m_type;
  DetType::Part     m_part;
  int               m_nInlets;
  int               m_nOutlets;
  int               m_nCables;
  int               m_nFibres;
  std::string       m_inletPipe;
  std::string       m_outletPipe;
  std::string       m_cable;
  std::string       m_fibre;

};

#endif
