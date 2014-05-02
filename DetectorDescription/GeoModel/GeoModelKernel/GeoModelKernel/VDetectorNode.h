/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.cm

//## begin module.cp preserve=no
//	Detector Description classes
//	Joe Boudreau
//	University of Pittsburgh
//
//	Summer 2002
//## end module.cp

//## Module: VDetectorNode; Pseudo Package specification
//## Subsystem: GeoModelKernel
//## Source file: L:\GEO\GeoModelKernel\VDetectorNode.h


#ifndef VDetectorNode_h
#define VDetectorNode_h 1

//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
//## end module.includes

// GeoVDetectorManager
#include "GeoModelKernel/GeoVDetectorManager.h"
// GeoVDetectorFactory
#include "GeoModelKernel/GeoVDetectorFactory.h"

//## begin module.additionalDeclarations preserve=yes
class GeoPhysVol;
//## end module.additionalDeclarations


//## Class: VDetectorNode; Abstract
//	Base class for detector node.  Detector nodes combine
//	factories and managers.  They are going out of style.  We
//	now prefer to separate the two functions.
//
//	This class is provided for backwards compatibility,
//	only.  Please use the classes "GeoVDetectorFactory" and
//	"GeoVDetectorManager".
//
//	This class will disappear in future releases.
//## Category: GeoModelKernel
//## Subsystem: GeoModelKernel
//## Persistence: Transient
//## Cardinality/Multiplicity: n


class VDetectorNode : public GeoVDetectorFactory, //## Inherits: <unnamed>
	public GeoVDetectorManager  //## Inherits: <unnamed>

{
  //## begin VDetectorNode.initialDeclarations preserve=yes
  //## end VDetectorNode.initialDeclarations

  public:
    //## Constructors (generated)
      VDetectorNode();

    //## Destructor (generated)
      virtual ~VDetectorNode();


    //## Other Operations (specified)
      //## Operation: getDetectorManager%1043616483
      //	Access to the Detector Manager.  VDetectorNode returns
      //	itself.
      virtual const GeoVDetectorManager * getDetectorManager() const;

    // Additional Public Declarations
      //## begin VDetectorNode.public preserve=yes
      //## end VDetectorNode.public


  protected:
    // Additional Protected Declarations
      //## begin VDetectorNode.protected preserve=yes
      //## end VDetectorNode.protected


  private:
    //## Constructors (generated)
      VDetectorNode(const VDetectorNode &right);

    //## Assignment Operation (generated)
      const VDetectorNode & operator=(const VDetectorNode &right);

    // Additional Private Declarations
      //## begin VDetectorNode.private preserve=yes
      //## end VDetectorNode.private


  private:  //## implementation
    // Additional Implementation Declarations
      //## begin VDetectorNode.implementation preserve=yes
      //## end VDetectorNode.implementation

};

// Class VDetectorNode 





#endif


