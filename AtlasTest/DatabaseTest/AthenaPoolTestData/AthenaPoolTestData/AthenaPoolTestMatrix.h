/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthenaPoolTestMatrix.h
 * @brief Class definition for AthenaPoolTestMatrix
 */
/**
 * @class AthenaPoolTestMatrix
 * @brief Simple class with various CLHEP classes 
 * for testing pool i/o from Athena
 * @author RD Schaffer
 * Created May 2004 for DC2 validation
 */
/***************************************************************************
 Athena Pool Test data package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: AthenaPoolTestMatrix.h,v 1.10 2006-08-30 07:49:42 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef ATHENAPOOLTESTDATA_ATHENAPOOLTESTMATRIX_H
# define ATHENAPOOLTESTDATA_ATHENAPOOLTESTMATRIX_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <map>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class AthenaPoolTestMatrix
{
public:

    typedef std::vector< std::map<unsigned int, CLHEP::HepMatrix> > matrixMaps_t;

    AthenaPoolTestMatrix();

    ~AthenaPoolTestMatrix();
    
    const CLHEP::HepMatrix&          smallMatrix     () const;
    const CLHEP::HepMatrix&          bigMatrix       () const;
    const HepGeom::Point3D<double>&  point           () const;
    const HepGeom::Point3D<double>&  point1          () const;
    const HepGeom::Transform3D&      trans           () const;

    int                       numberOfMatrices() const;
    const CLHEP::HepMatrix&   matrix          ( int i ) const;
    const matrixMaps_t&       matrixMaps      () const;

    void                      addMatrix       (const CLHEP::HepMatrix* matrixPointer);
    void                      setTransform    (const HepGeom::Transform3D trans);
    
private:
    CLHEP::HepMatrix      m_smallMatrix;
    CLHEP::HepMatrix*     m_bigMatrix;
    HepGeom::Point3D<double>     m_point;
    HepGeom::Point3D<double>*    m_point1;
    std::vector< const CLHEP::HepMatrix* > m_matrices;
    matrixMaps_t   m_matrixMaps;
    HepGeom::Transform3D m_trans;
    std::vector<int> m_vint;
    std::vector<float> m_vfloat;

};


CLASS_DEF(AthenaPoolTestMatrix, 153882668, 0)

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const CLHEP::HepMatrix& 
AthenaPoolTestMatrix::smallMatrix() const { return m_smallMatrix; }

inline const CLHEP::HepMatrix& 
AthenaPoolTestMatrix::bigMatrix() const { return *m_bigMatrix; }

inline const HepGeom::Point3D<double>&   
AthenaPoolTestMatrix::point() const { return m_point; }

inline const HepGeom::Point3D<double>&   
AthenaPoolTestMatrix::point1() const { return *m_point1; }

inline const HepGeom::Transform3D& 
AthenaPoolTestMatrix::trans() const { return m_trans; }

inline void 
AthenaPoolTestMatrix::addMatrix( const CLHEP::HepMatrix* matrixPointer ) { m_matrices.push_back( matrixPointer ); }

inline void 
AthenaPoolTestMatrix::setTransform    (const HepGeom::Transform3D trans)
{
    m_trans = trans;
}

inline int 
AthenaPoolTestMatrix::numberOfMatrices() const {return m_matrices.size(); }

inline const CLHEP::HepMatrix& 
AthenaPoolTestMatrix::matrix( int i ) const { return *(m_matrices[i]); }

inline const AthenaPoolTestMatrix::matrixMaps_t& 
AthenaPoolTestMatrix::matrixMaps      () const
{
    return (m_matrixMaps);
}


#endif // ATHENAPOOLTESTDATA_ATHENAPOOLTESTMATRIX_H
