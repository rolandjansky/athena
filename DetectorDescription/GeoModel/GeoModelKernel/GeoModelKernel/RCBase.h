/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD1266D02D3.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD1266D02D3.cm

//## begin module%3CD1266D02D3.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD1266D02D3.cp

//## Module: RCBase%3CD1266D02D3; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/RCBase.h

#ifndef RCBase_h
#define RCBase_h 1

//## begin module%3CD1266D02D3.additionalIncludes preserve=no
//## end module%3CD1266D02D3.additionalIncludes

//## begin module%3CD1266D02D3.includes preserve=yes
//## end module%3CD1266D02D3.includes

//## begin module%3CD1266D02D3.additionalDeclarations preserve=yes
//## end module%3CD1266D02D3.additionalDeclarations


//## begin RCBase%3CD1266D02D3.preface preserve=yes
//## end RCBase%3CD1266D02D3.preface

//## Class: RCBase%3CD1266D02D3; Abstract
//	This is a base class for objects whose memory is managed
//	through reference counting.  Reference-counted objects
//	can only be created using
//	operator new, the cannot be created on the stack.
//
//	The methods ref() and unref() can be called to increase
//	and decrease the reference count of an object.  When
//	the reference count decreases to zero, the object deletes
//	itself.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class RCBase 
{
  //## begin RCBase%3CD1266D02D3.initialDeclarations preserve=yes
  //## end RCBase%3CD1266D02D3.initialDeclarations

  public:
    //## Constructors (generated)
      RCBase();


    //## Other Operations (specified)
      //## Operation: ref%3CD12A80000C
      //	Increase the reference count
      void ref () const;

      //## Operation: unref%3CD12A840120
      //	Decreases the reference count.  When the reference count
      //	falls to zero, the object deletes itself.
      void unref () const;

      //## Operation: refCount%3CD523F40158
      //	Return the reference count.
      unsigned int refCount () const;

    // Additional Public Declarations
      //## begin RCBase%3CD1266D02D3.public preserve=yes
      //## end RCBase%3CD1266D02D3.public

  protected:
    //## Destructor (generated)
      virtual ~RCBase();

    // Additional Protected Declarations
      //## begin RCBase%3CD1266D02D3.protected preserve=yes
      //## end RCBase%3CD1266D02D3.protected

  private:
    //## Constructors (generated)
      RCBase(const RCBase &right);

    //## Assignment Operation (generated)
      RCBase & operator=(const RCBase &right);

    // Data Members for Class Attributes

      //## Attribute: m_count%3CD12A9F013D
      //	The reference count
      //## begin RCBase::m_count%3CD12A9F013D.attr preserve=no  private: mutable unsigned short int {V} 0
      mutable unsigned short int m_count;
      //## end RCBase::m_count%3CD12A9F013D.attr

    // Additional Private Declarations
      //## begin RCBase%3CD1266D02D3.private preserve=yes
      //## end RCBase%3CD1266D02D3.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin RCBase%3CD1266D02D3.implementation preserve=yes
      //## end RCBase%3CD1266D02D3.implementation

};

//## begin RCBase%3CD1266D02D3.postscript preserve=yes
//## end RCBase%3CD1266D02D3.postscript

// Class RCBase 

//## begin module%3CD1266D02D3.epilog preserve=yes
//## end module%3CD1266D02D3.epilog


#endif
