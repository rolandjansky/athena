/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 -----------------------------------------
 ***************************************************************************
  Test class for IOV DB
 ***************************************************************************/

//<doc><file>	$Id: IOVDbTestAmdbCorrection.h,v 1.3 2006-03-26 17:26:30 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IOVDBTEST_AMDB_CORRECTION_H
#define IOVDBTEST_AMDB_CORRECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "CLHEP/Geometry/Point3D.h"

class IOVDbTestAmdbCorrection{

 public:
  IOVDbTestAmdbCorrection();

  ~IOVDbTestAmdbCorrection();

  std::string name() const; // test method

  HepGeom::Point3D<double> getTranslation() const;
  HepGeom::Point3D<double> getRotation() const;

  void set(const HepGeom::Point3D<double>& trans, const HepGeom::Point3D<double>& rot, std::string name);

 private:
   std::string m_name;
   HepGeom::Point3D<double>  m_translation;
   HepGeom::Point3D<double>  m_rotation;
};

CLASS_DEF(IOVDbTestAmdbCorrection, 132798543, 1)
#endif //IOVDBTEST_AMDB_CORRECTION_H
