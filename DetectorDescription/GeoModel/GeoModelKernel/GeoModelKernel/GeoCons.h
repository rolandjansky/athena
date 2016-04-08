/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD26BFA00BE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD26BFA00BE.cm

//## begin module%3CD26BFA00BE.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3CD26BFA00BE.cp

//## Module: GeoCons%3CD26BFA00BE; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoCons.h

#ifndef GeoCons_h
#define GeoCons_h 1

//## begin module%3CD26BFA00BE.additionalIncludes preserve=no
//## end module%3CD26BFA00BE.additionalIncludes

//## begin module%3CD26BFA00BE.includes preserve=yes
//## end module%3CD26BFA00BE.includes

// GeoShape
#include "GeoModelKernel/GeoShape.h"
//## begin module%3CD26BFA00BE.additionalDeclarations preserve=yes
//## end module%3CD26BFA00BE.additionalDeclarations


//## begin GeoCons%3CD26BFA00BE.preface preserve=yes
//## end GeoCons%3CD26BFA00BE.preface

//## Class: GeoCons%3CD26BFA00BE
//	This shape represents a Cone or Cone Section
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class GeoCons : public GeoShape  //## Inherits: <unnamed>%3CD26BFA00C1
{
  //## begin GeoCons%3CD26BFA00BE.initialDeclarations preserve=yes
  //## end GeoCons%3CD26BFA00BE.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: GeoCons%3CD58C08006B
      //	Constructor for the CONS.
      GeoCons (double RMin1, double RMin2, double RMax1, double RMax2, double DZ, double SPhi, double DPhi);


    //## Other Operations (specified)
      //## Operation: volume%3CD2A6BC0134
      //	Returns the volume of the shape, for mass inventory
      virtual double volume () const;

      //## Operation: type%3CD2A83400A8
      //	Returns the CONS shape type, as a string.
      virtual const std::string & type () const;

      //## Operation: typeID%3CD2A83400BC
      //	Returns the CONS shape type, as a coded integer.
      virtual ShapeType typeID () const;

      //## Operation: exec%3DB96A2B0118
      //	Executes a GeoShapeAction
      virtual void exec (GeoShapeAction *action) const;

    //## Get and Set Operations for Class Attributes (generated)

      //## Attribute: classType%3CD26BFA00BF
      //	For type identification.
      static const std::string& getClassType ();

      //## Attribute: classTypeID%3CD26BFA00C0
      //	For type identification.
      static ShapeType getClassTypeID ();

      //## Attribute: rMin1%3CD589870359
      //	Inside radius at -dZ
      const double& getRMin1 () const;

      //## Attribute: rMin2%3CD589B803C7
      //	Inside radius at +dZ
      const double& getRMin2 () const;

      //## Attribute: rMax1%3CD589CD015B
      //	Outside radius at -dZ
      const double& getRMax1 () const;

      //## Attribute: rMax2%3CD589E503B8
      //	Outside radius at +dZ
      const double& getRMax2 () const;

      //## Attribute: dZ%3CD58A02020B
      //	Half length in Z direction.
      const double& getDZ () const;

      //## Attribute: sPhi%3CD58A280274
      //	Starting angle of the segment in radians
      const double& getSPhi () const;

      //## Attribute: dPhi%3CD58A2A01E1
      //	Delta angle of the segment in radians.
      const double& getDPhi () const;

    // Additional Public Declarations
      //## begin GeoCons%3CD26BFA00BE.public preserve=yes
      //## end GeoCons%3CD26BFA00BE.public

  protected:
    //## Destructor (generated)
      virtual ~GeoCons();

    // Additional Protected Declarations
      //## begin GeoCons%3CD26BFA00BE.protected preserve=yes
      //## end GeoCons%3CD26BFA00BE.protected

  private:
    //## Constructors (generated)
      GeoCons(const GeoCons &right);

    //## Assignment Operation (generated)
      GeoCons & operator=(const GeoCons &right);

    // Data Members for Class Attributes

      //## begin GeoCons::s_classType%3CD26BFA00BF.attr preserve=no  public: static const std::string {U} "Cons"
      static const std::string s_classType;
      //## end GeoCons::s_classType%3CD26BFA00BF.attr

      //## begin GeoCons::s_classTypeID%3CD26BFA00C0.attr preserve=no  public: static const ShapeType {U} 0x11
      static const ShapeType s_classTypeID;
      //## end GeoCons::s_classTypeID%3CD26BFA00C0.attr

      //## begin GeoCons::m_rMin1%3CD589870359.attr preserve=no  public: double {U} 
      double m_rMin1;
      //## end GeoCons::m_rMin1%3CD589870359.attr

      //## begin GeoCons::m_rMin2%3CD589B803C7.attr preserve=no  public: double {U} 
      double m_rMin2;
      //## end GeoCons::m_rMin2%3CD589B803C7.attr

      //## begin GeoCons::m_rMax1%3CD589CD015B.attr preserve=no  public: double {U} 
      double m_rMax1;
      //## end GeoCons::m_rMax1%3CD589CD015B.attr

      //## begin GeoCons::m_rMax2%3CD589E503B8.attr preserve=no  public: double {U} 
      double m_rMax2;
      //## end GeoCons::m_rMax2%3CD589E503B8.attr

      //## begin GeoCons::m_dZ%3CD58A02020B.attr preserve=no  public: double {U} 
      double m_dZ;
      //## end GeoCons::m_dZ%3CD58A02020B.attr

      //## begin GeoCons::m_sPhi%3CD58A280274.attr preserve=no  public: double {U} 
      double m_sPhi;
      //## end GeoCons::m_sPhi%3CD58A280274.attr

      //## begin GeoCons::m_dPhi%3CD58A2A01E1.attr preserve=no  public: double {U} 
      double m_dPhi;
      //## end GeoCons::m_dPhi%3CD58A2A01E1.attr

    // Additional Private Declarations
      //## begin GeoCons%3CD26BFA00BE.private preserve=yes
      //## end GeoCons%3CD26BFA00BE.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoCons%3CD26BFA00BE.implementation preserve=yes
      //## end GeoCons%3CD26BFA00BE.implementation

};

//## begin GeoCons%3CD26BFA00BE.postscript preserve=yes
//## end GeoCons%3CD26BFA00BE.postscript

// Class GeoCons 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string& GeoCons::getClassType ()
{
  //## begin GeoCons::getClassType%3CD26BFA00BF.get preserve=no
  return s_classType;
  //## end GeoCons::getClassType%3CD26BFA00BF.get
}

inline ShapeType GeoCons::getClassTypeID ()
{
  //## begin GeoCons::getClassTypeID%3CD26BFA00C0.get preserve=no
  return s_classTypeID;
  //## end GeoCons::getClassTypeID%3CD26BFA00C0.get
}

inline const double& GeoCons::getRMin1 () const
{
  //## begin GeoCons::getRMin1%3CD589870359.get preserve=no
  return m_rMin1;
  //## end GeoCons::getRMin1%3CD589870359.get
}

inline const double& GeoCons::getRMin2 () const
{
  //## begin GeoCons::getRMin2%3CD589B803C7.get preserve=no
  return m_rMin2;
  //## end GeoCons::getRMin2%3CD589B803C7.get
}

inline const double& GeoCons::getRMax1 () const
{
  //## begin GeoCons::getRMax1%3CD589CD015B.get preserve=no
  return m_rMax1;
  //## end GeoCons::getRMax1%3CD589CD015B.get
}

inline const double& GeoCons::getRMax2 () const
{
  //## begin GeoCons::getRMax2%3CD589E503B8.get preserve=no
  return m_rMax2;
  //## end GeoCons::getRMax2%3CD589E503B8.get
}

inline const double& GeoCons::getDZ () const
{
  //## begin GeoCons::getDZ%3CD58A02020B.get preserve=no
  return m_dZ;
  //## end GeoCons::getDZ%3CD58A02020B.get
}

inline const double& GeoCons::getSPhi () const
{
  //## begin GeoCons::getSPhi%3CD58A280274.get preserve=no
  return m_sPhi;
  //## end GeoCons::getSPhi%3CD58A280274.get
}

inline const double& GeoCons::getDPhi () const
{
  //## begin GeoCons::getDPhi%3CD58A2A01E1.get preserve=no
  return m_dPhi;
  //## end GeoCons::getDPhi%3CD58A2A01E1.get
}

//## begin module%3CD26BFA00BE.epilog preserve=yes
//## end module%3CD26BFA00BE.epilog


#endif
