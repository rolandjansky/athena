/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3E33EC600353.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E33EC600353.cm

//## begin module%3E33EC600353.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module%3E33EC600353.cp

//## Module: GeoVDetectorManager%3E33EC600353; Pseudo Package specification
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVDetectorManager.h

#ifndef GeoVDetectorManager_h
#define GeoVDetectorManager_h 1

//## begin module%3E33EC600353.additionalIncludes preserve=no
//## end module%3E33EC600353.additionalIncludes

//## begin module%3E33EC600353.includes preserve=yes
//## end module%3E33EC600353.includes

// GeoVPhysVol
#include "GeoModelKernel/GeoVPhysVol.h"

class GeoVDetectorElement;

//## begin module%3E33EC600353.additionalDeclarations preserve=yes
//## end module%3E33EC600353.additionalDeclarations


//## begin GeoVDetectorManager%3E33EC600353.preface preserve=yes
//## end GeoVDetectorManager%3E33EC600353.preface

//## Class: GeoVDetectorManager%3E33EC600353
//	This class is the Manager for each detector. It has two
//	required query mechanism:  one for "tree tops" (These
//	are top level pieces of raw geometry; there may be one
//	or more of these), and another one for detector
//	elements, which represent readout units.
//
//	Subsystems engineers should override this.  They may in
//	addition add their own query mechanisms to their
//	interface.
//	They may in fact add anything else they require to their
//	interface.
//## Category: GeoModelKernel%3CD1146300F9
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E33FC3D0110;GeoVPhysVol { -> }
//## Uses: <unnamed>%3E341EEE03CC;GeoVDetectorElement { -> F}

class GeoVDetectorManager 
{
  //## begin GeoVDetectorManager%3E33EC600353.initialDeclarations preserve=yes
  //## end GeoVDetectorManager%3E33EC600353.initialDeclarations

  public:
    //## Constructors (generated)
      GeoVDetectorManager();

    //## Destructor (generated)
      virtual ~GeoVDetectorManager();


    //## Other Operations (specified)
      //## Operation: getNumTreeTops%3E33EDF20026
      //	Returns the number of Tree Tops (Top Level Physicsl
      //	Volumes)
      virtual unsigned int getNumTreeTops () const = 0;

      //## Operation: getTreeTop%3E33EDF20058
      //	Access to the Tree Top
      virtual PVConstLink getTreeTop (unsigned int i) const = 0;

    // Additional Public Declarations
      //## begin GeoVDetectorManager%3E33EC600353.public preserve=yes
      const std::string & getName() const;
      void                setName(const std::string & name);
      //## end GeoVDetectorManager%3E33EC600353.public
  protected:
    // Additional Protected Declarations
      //## begin GeoVDetectorManager%3E33EC600353.protected preserve=yes
      //## end GeoVDetectorManager%3E33EC600353.protected

  private:
    //## Constructors (generated)
      GeoVDetectorManager(const GeoVDetectorManager &right);

    //## Assignment Operation (generated)
      GeoVDetectorManager & operator=(const GeoVDetectorManager &right);

    // Additional Private Declarations
      //## begin GeoVDetectorManager%3E33EC600353.private preserve=yes
      std::string m_name;
      //## end GeoVDetectorManager%3E33EC600353.private
  private: //## implementation
    // Additional Implementation Declarations
      //## begin GeoVDetectorManager%3E33EC600353.implementation preserve=yes
      //## end GeoVDetectorManager%3E33EC600353.implementation

};

//## begin GeoVDetectorManager%3E33EC600353.postscript preserve=yes
//## end GeoVDetectorManager%3E33EC600353.postscript

// Class GeoVDetectorManager 

//## begin module%3E33EC600353.epilog preserve=yes
//## end module%3E33EC600353.epilog


#endif
