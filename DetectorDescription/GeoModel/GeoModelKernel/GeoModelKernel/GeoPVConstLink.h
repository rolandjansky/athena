/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D13D8DE0092.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D13D8DE0092.cm

//## begin module%3D13D8DE0092.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3D13D8DE0092.cp

//## Module: GeoPVConstLink%3D13D8DE0092; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPVConstLink.h

#ifndef GeoPVConstLink_h
#define GeoPVConstLink_h 1

//## begin module%3D13D8DE0092.additionalIncludes preserve=no
//## end module%3D13D8DE0092.additionalIncludes

//## begin module%3D13D8DE0092.includes preserve=yes
//## end module%3D13D8DE0092.includes

//## begin module%3D13D8DE0092.additionalDeclarations preserve=yes
class GeoVPhysVol;
//## end module%3D13D8DE0092.additionalDeclarations


//## begin GeoPVConstLink%3D13D8DE0092.preface preserve=yes
//## end GeoPVConstLink%3D13D8DE0092.preface

//## Class: GeoPVConstLink%3D13D8DE0092
//	Smart pointer to physical volume... constant
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoPVConstLink 
{
  //## begin GeoPVConstLink%3D13D8DE0092.initialDeclarations preserve=yes
  //## end GeoPVConstLink%3D13D8DE0092.initialDeclarations

  public:
    //## Constructors (generated)
      GeoPVConstLink();

      GeoPVConstLink(const GeoPVConstLink &right);

    //## Constructors (specified)
      //## Operation: GeoPVConstLink%3D14059C00B8
      GeoPVConstLink (const GeoVPhysVol  *addr);

    //## Destructor (generated)
      virtual ~GeoPVConstLink();

    //## Equality Operations (generated)
      int operator==(const GeoPVConstLink &right) const;

      int operator!=(const GeoPVConstLink &right) const;

    //## Relational Operations (generated)
      int operator<(const GeoPVConstLink &right) const;

      int operator>(const GeoPVConstLink &right) const;

      int operator<=(const GeoPVConstLink &right) const;

      int operator>=(const GeoPVConstLink &right) const;


    //## Other Operations (specified)
      //## Operation: operator *%3D13DCE90234
      //	Dereference this smart pointer, like a regular pointer.
      virtual const GeoVPhysVol & operator * () const;

      //## Operation: operator->%3D13DD9601ED
      //	Allow use of smartPointer->operation().....
      virtual const GeoVPhysVol * operator -> () const;

      //## Operation: operator bool%3D140A1E02A1
      operator bool () const;

      //## Operation: operator=%3D1DC05000AA
      //	Assignment operator.
      GeoPVConstLink& operator = (const GeoPVConstLink& right);

    // Additional Public Declarations
      //## begin GeoPVConstLink%3D13D8DE0092.public preserve=yes
      //## end GeoPVConstLink%3D13D8DE0092.public

  protected:
    // Data Members for Class Attributes

      //## Attribute: ptr%3D13DAF4008F
      //	A pointer to the underlying volume
      //## begin GeoPVConstLink::ptr%3D13DAF4008F.attr preserve=no  protected: mutable GeoVPhysVol *  {U} 
      GeoVPhysVol *  m_ptr;
      //## end GeoPVConstLink::ptr%3D13DAF4008F.attr

    // Additional Protected Declarations
      //## begin GeoPVConstLink%3D13D8DE0092.protected preserve=yes
      //## end GeoPVConstLink%3D13D8DE0092.protected

  private:
    // Additional Private Declarations
      //## begin GeoPVConstLink%3D13D8DE0092.private preserve=yes
      //## end GeoPVConstLink%3D13D8DE0092.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoPVConstLink%3D13D8DE0092.implementation preserve=yes
      //## end GeoPVConstLink%3D13D8DE0092.implementation

};

//## begin GeoPVConstLink%3D13D8DE0092.postscript preserve=yes
//## end GeoPVConstLink%3D13D8DE0092.postscript

// Class GeoPVConstLink 

//## begin module%3D13D8DE0092.epilog preserve=yes
//## end module%3D13D8DE0092.epilog


#endif
