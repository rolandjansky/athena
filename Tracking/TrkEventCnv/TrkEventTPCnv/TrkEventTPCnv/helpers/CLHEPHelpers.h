/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// author: Lukasz Janyst <ljanyst@cern.ch>
// file:   CLHEPHelpers.h
//
//-----------------------------------------------------------------------------

#ifndef CLHEP_HELPERS_H
#define CLHEP_HELPERS_H

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Matrix/Vector.h"

#include <vector>

namespace {

class SetTransform3D : public HepGeom::Transform3D
{
public:
  void setTransform (double XX, double XY, double XZ, double DX,
                            double YX, double YY, double YZ, double DY,
                            double ZX, double ZY, double ZZ, double DZ)
  {
    HepGeom::Transform3D::setTransform (XX, XY, XZ, DX,
                                        YX, YY, YZ, DY,
                                        ZX, ZY, ZZ, DZ);
  }
};

}

class CLHEPHelpers
{
    public:

        /** Convert HepGeom::Transform3D to std :: vector<double>*/
	template <class T> 
        static void hepTransform3DToVector( const HepGeom::Transform3D        &trans,
                                                  T                     &vec )
        {
	    vec.reserve(12);
            vec.push_back( trans.xx() );
            vec.push_back( trans.xy() );
            vec.push_back( trans.xz() );
            vec.push_back( trans.dx() );
            
            vec.push_back( trans.yx() );
            vec.push_back( trans.yy() );
            vec.push_back( trans.yz() );
            vec.push_back( trans.dy() );

            vec.push_back( trans.zx() );
            vec.push_back( trans.zy() );
            vec.push_back( trans.zz() );
            vec.push_back( trans.dz() );
        }

        /** Convert std :: vector<double> to HepGeom::Transform3D*/
	template <class T>
        static void vectorToHepTransform3D( const T                     &vec,
                                                  HepGeom::Transform3D        &trans )
        {
          static_cast<SetTransform3D&> (trans).setTransform (
                                        vec[0],    // xx
                                        vec[1],    // xy
                                        vec[2],    // xz
                                        vec[3],    // dx
                                        vec[4],    // yx
                                        vec[5],    // yy
                                        vec[6],    // yz
                                        vec[7],    // dy
                                        vec[8],    // zx
                                        vec[9],    // zy
                                        vec[10],   // zz
                                        vec[11] ); // dz
        }

        /** Convert HepGeom::BasicVector3D<double> to std :: vector<double> and vice versa*/
        template <class T>
        static void basicVector3DToVector( const T                     &bvec,
                                                 std :: vector<double> &vec )
        {
	    vec.reserve(3);
            vec.push_back( bvec.x() );
            vec.push_back( bvec.y() );
            vec.push_back( bvec.z() );
        }

        template <class T>
        static void vectorToBasicVector3D( const std :: vector<double> &vec,
                                                 T                     &bvec )
        {
            bvec.setX( vec[0] );
            bvec.setY( vec[1] );
            bvec.setZ( vec[2] );
        }

        //---------------------------------------------------------------------
        // Convert CLHEP::HepVector to std vector<double> and vice versa
        //---------------------------------------------------------------------
        static void hepVectorToVector( const CLHEP::HepVector             &hvec,
                                             std :: vector<double> &vec )
        {
	    vec.reserve(hvec.num_row());
            for( int i = 0; i < hvec.num_row(); i++ )
                vec.push_back( hvec[i] );
        }

        static void vectorToHepVector( const std :: vector<double> &vec,
                                             CLHEP::HepVector             &hvec )
        {
            for( unsigned i = 0; i < vec.size(); i++ )
                hvec[i] = vec[i];
        }
};

#endif // CLHEP_HELPERS_H
