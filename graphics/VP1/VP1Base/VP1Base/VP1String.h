/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1String                           //
//                                                            //
//  Description: Utility class for converting various types   //
//               to QStrings. Main purpose is to serve as     //
//               base for IVP1System, IVP1Channel, and        //
//               VP1HelperClassBase, thus providing str(..).  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1STRING_H
#define VP1STRING_H

// FIXME: need to include this before any QT header, because QT defines
// a macro `foreach' that collides with an identifier in AthenaKernel.
#include "xAODCore/BaseInfo.h"

#include <QString>

#include "GeoPrimitives/GeoPrimitives.h" // for the Amg (Eigen) vectors/matrices/...



namespace HepGeom { template<class T> class BasicVector3D; }
class VP1Interval;
class QColor;
class SbVec2d;
class SbColor;
class SbVec2f;
class SbVec2s;
class SbVec3d;
class SbVec3f;
class SbVec3s;
class SbVec4d;
class SbVec4f;

class VP1String {
public:
  VP1String(){}
  virtual ~VP1String(){}

  //Strings:
  static QString str( const QString& s ) { return s; }
  static QString str( const char * c) { return c; }

  //Boolean:
  static QString str( const bool b ) { return b?"True":"False"; }

  //Colours:
  static QString str( const QColor& );
  static QString str( const SbColor& );

  //VP1 interval:
  static QString str( const VP1Interval& );

  //COIN vectors:
  static QString str( const SbVec2d&  );
  static QString str( const SbVec2f&  );
  static QString str( const SbVec2s&  );
  static QString str( const SbVec3d&  );
  static QString str( const SbVec3f&  );
  static QString str( const SbVec3s&  );
  static QString str( const SbVec4d&  );
  static QString str( const SbVec4f&  );

  //Numbers:
  static QString str( short int n ) { return QString::number(n); }
  static QString str( unsigned short int n ) { return QString::number(n); }
  static QString str( long n ) { return QString::number(n); }
  static QString str( ulong n ) { return QString::number(n); }
  static QString str( int n ) { return QString::number(n); }
  static QString str( uint n ) { return QString::number(n); }
  static QString str( qlonglong n ) { return QString::number(n); }
  static QString str( qulonglong n ) { return QString::number(n); }
  static QString str( const double& d ) { return QString::number(d); }
  static QString str( const float& f) { return QString::number(f); }

  //Pointers:
  static QString str(const void *);
  template <class T>
  static QString str( const T* t ) { return str(static_cast<const void* >(t)); }

  //Flags:
  template <class T>
  static QString str( const QFlags<T>& f ) { return "0x"+QString::number(f, 16).toUpper().rightJustified(8,'0'); }

  //CLHEP vectors/points:
  template <class T>
  static QString str( const HepGeom::BasicVector3D<T>& t ) { return "("+str(t.x())+", "+str(t.y())+", "+str(t.z())+")"; }

  //Amg (Eigen) vectors:
  static QString str( const Amg::Vector3D& t ) { return "("+str(t.x())+", "+str(t.y())+", "+str(t.z())+")"; }

  //Lists
  template <class T>
  static QString str( const QList<T>& t ) {  return "QList of size"+QString::number(t.size()); }
 
  
};

#endif
