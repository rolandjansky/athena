/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file GeoPrimitivesCompare.h
 *
 * @date Mar 25, 2014
 * @author Riccardo.Maria.Bianchi <riccardo.maria.bianchi@cern.ch>
 *
 * @section Description
 * Methods to compare Eigen-based vectors
 */

#ifndef GEOPRIMITIVESCOMPARE_H_
#define GEOPRIMITIVESCOMPARE_H_

#include <vector>
#include "GeoPrimitives/GeoPrimitives.h"

namespace Amg {


/**
 * comparison of two Vector3D,
 * needed for the definition of a std::set<Amg::Vector3D>
 *
 * originals for CLHEP::HepVector3D was in CLHEP/Vector/src/SpaceVector.cc:
 *   int Hep3Vector::compare (const Hep3Vector & v) const;
 *   bool Hep3Vector::operator > (const Hep3Vector & v) const;
 *   bool Hep3Vector::operator < (const Hep3Vector & v) const;
 *
 */
struct Vector3DComparer
{
	bool operator()(Amg::Vector3D const &v1, Amg::Vector3D const &v2) const
	{
		if ( v1.z() > v2.z() ) {
			return false;
		} else if ( v1.z() < v2.z() ) {
			return true;
		} else if ( v1.y() > v2.y() ) {
			return false;
		} else if ( v1.y() < v2.y() ) {
			return true;
		} else if ( v1.x() > v2.x() ) {
			return false;
		} else if ( v1.x() < v2.x() ) {
			return true;
		} else {
			return false;
		}
	}
};


struct VectorVector3DComparer
{
	bool operator()(std::vector<Amg::Vector3D> const &vv1, std::vector<Amg::Vector3D> const &vv2) const
	{
          std::vector<Amg::Vector3D>::const_iterator v1 = vv1.begin();
          std::vector<Amg::Vector3D>::const_iterator v2 = vv2.begin();

          while (v1 != vv1.end()) {
            if (v2 == vv2.end()) return false;
            if ( (*v1).z() > (*v2).z() ) {
              return false;
            } else if ( (*v1).z() < (*v2).z() ) {
              return true;
            } else if ( (*v1).y() > (*v2).y() ) {
              return false;
            } else if ( (*v1).y() < (*v2).y() ) {
              return true;
            } else if ( (*v1).x() > (*v2).x() ) {
              return false;
            } else if ( (*v1).x() < (*v2).x() ) {
              return true;
            }
            ++v1;
            ++v2;
          }
          if (v2 != vv2.end()) return true;
          return false;
}

};


} // end namespace

#endif /* GEOPRIMITIVESCOMPARE_H_ */
