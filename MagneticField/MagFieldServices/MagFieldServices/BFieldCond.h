/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// BFieldCond.h
//
// Current conductors used in BFieldZone.
// It may be finite : p1 to p2.
// It may be infinite : passing p1 and direction parallel to p2.
//
// Masahiro Morii, Harvard University
//
#ifndef BFIELDCOND_H
#define BFIELDCOND_H

#include <cmath>

class BFieldCond {
public:
    // constructor
    BFieldCond( bool finite, const double *p1, const double *p2, double curr )
        : m_finite(finite), m_curr(curr), m_nomCurr(curr)
    {
        for ( int i = 0; i < 3; i++ ) { m_p1[i] = p1[i]; m_p2[i] = p2[i]; }
        // fill m_u[3] = unit vector parallel to the conductor
        if ( finite ) {
            for ( int i = 0; i < 3; i++ ) m_u[i] = p2[i] - p1[i];
            double magu = sqrt(m_u[0]*m_u[0]+m_u[1]*m_u[1]+m_u[2]*m_u[2]);
            for ( int i = 0; i < 3; i++ ) m_u[i] /= magu;
        } else {
            for ( int i = 0; i < 3; i++ ) m_u[i] = p2[i];
        }
    }
    // compute magnetic field, plus derivatives if requested, and add
    inline void addBiotSavart( const double *xyz, double *B, double *deriv=0 ) const;
    // scale the current wrt the nominal current
    void scaleCurrent( double factor ) { m_curr = factor*m_nomCurr; }
    // accessors
    bool finite() const { return m_finite; }
    double p1( int i ) const { return m_p1[i]; }
    double p2( int i ) const { return m_p2[i]; }
    double curr() const { return m_curr; }
private:
    bool m_finite;  // true if the conductor is finite in length
    double m_p1[3]; // one end of a finite conductor, or one point on an infinite conductor
    double m_p2[3]; // the other end of a finite conductor, or the direction vector of an infinite conductor
    double m_u[3];  // direction vector (= m_p2 if infinite)
    double m_curr;  // current (in A) flowing through the conductor
    double m_nomCurr;  // nominal current (in A) read from the map file
};

//
// Compute the Biot-Savart field due to this conductor
// If deriv[9] is passed, also computes the field derivatives
// The results are _added_ to B[] and deriv[].
//
void
BFieldCond::addBiotSavart( const double *xyz, double *B, double *deriv ) const
{
    static const double mu04pi( 1.0e-7 );  // mu_0/4pi
    static const double minvsq( 10.*10. ); // (1 cm)^2
    if ( m_finite ) { // finite conductor segment
        double r1[3], r2[3];
        for ( int i = 0; i < 3; i++ ) {
            r1[i] = xyz[i] - m_p1[i];
            r2[i] = xyz[i] - m_p2[i];
        }
        double r1mag2 = r1[0]*r1[0]+r1[1]*r1[1]+r1[2]*r1[2]; 
        double r2mag2 = r2[0]*r2[0]+r2[1]*r2[1]+r2[2]*r2[2]; 
        double r1mag = sqrt(r1mag2);
        double r2mag = sqrt(r2mag2);
        double r1dotu = r1[0]*m_u[0]+r1[1]*m_u[1]+r1[2]*m_u[2];
        double r2dotu = r2[0]*m_u[0]+r2[1]*m_u[1]+r2[2]*m_u[2];
        double sinfac = r1dotu/r1mag - r2dotu/r2mag;
        double v[3];
        v[0] = m_u[1]*r1[2] - m_u[2]*r1[1];
        v[1] = m_u[2]*r1[0] - m_u[0]*r1[2];
        v[2] = m_u[0]*r1[1] - m_u[1]*r1[0];
        double vsq = std::max( v[0]*v[0]+v[1]*v[1]+v[2]*v[2], minvsq );
        double f1 = mu04pi*m_curr*sinfac/vsq;
        for ( int i = 0; i < 3; i++ ) {
            B[i] += f1*v[i];
        }
        if ( deriv ) { // compute the derivatives
            deriv[1] -= f1*m_u[2];
            deriv[2] += f1*m_u[1];
            deriv[3] += f1*m_u[2];
            deriv[5] -= f1*m_u[0];
            deriv[6] -= f1*m_u[1];
            deriv[7] += f1*m_u[0];
            if ( vsq > minvsq ) {
                double f2 = 2.0*f1/vsq;
                double f3 = mu04pi*m_curr/vsq;
                double w[3];
                for ( int i = 0; i < 3; i++ ) {
                    w[i] = f2*( m_u[i]*r1dotu - r1[i] )
                         + f3*( (m_u[i]-r1[i]*r1dotu/r1mag2)/r1mag - (m_u[i]-r2[i]*r2dotu/r2mag2)/r2mag );
                }
                for ( int i = 0; i < 3; i++ ) for ( int j = 0; j < 3; j++ ) {
                    deriv[3*i+j] += v[i]*w[j];
                }
            }
        }
    } else { // infinite line conductor
        double r1[3];
        for ( int i = 0; i < 3; i++ ) r1[i] = xyz[i] - m_p1[i];
        double v[3];
        v[0] = m_u[1]*r1[2] - m_u[2]*r1[1];
        v[1] = m_u[2]*r1[0] - m_u[0]*r1[2];
        v[2] = m_u[0]*r1[1] - m_u[1]*r1[0];
        double vsq = std::max( v[0]*v[0]+v[1]*v[1]+v[2]*v[2], minvsq );
        double f1 = 2.0*mu04pi*m_curr/vsq;
        for ( int i = 0; i < 3; i++ ) {
            B[i] += f1*v[i];
        }
        if ( deriv ) { // compute the derivatives
            deriv[1] -= f1*m_u[2];
            deriv[2] += f1*m_u[1];
            deriv[3] += f1*m_u[2];
            deriv[5] -= f1*m_u[0];
            deriv[6] -= f1*m_u[1];
            deriv[7] += f1*m_u[0];
            double f2 = 2.0*f1/vsq;
            double r1dotu = r1[0]*m_u[0]+r1[1]*m_u[1]+r1[2]*m_u[2];
            double w[3];
            if ( vsq > minvsq ) {
                for ( int i = 0; i < 3; i++ ) {
                    w[i] = f2*(m_u[i]*r1dotu - r1[i]);
                }
                for ( int i = 0; i < 3; i++ ) for ( int j = 0; j < 3; j++ ) {
                    deriv[3*i+j] += v[i]*w[j];
                }
            }
        }
    }
}

#endif
