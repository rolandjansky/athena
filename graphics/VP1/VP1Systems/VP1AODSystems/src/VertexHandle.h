/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VertexHandle                 //
//                                                            //
//  Description: Base class for IParticle xAOD.               //
//                                                            //
//  Author: E. Moyse                                          //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VERTEXHANDLE_H
#define VERTEXHANDLE_H

#include "GeoPrimitives/GeoPrimitives.h"

#include <QStringList>
#include <QTreeWidgetItem>
#include <vector>
#include <set>
#include <QList>
#include <QFlags>
#include "xAODTracking/Vertex.h"
#include "AODHandleBase.h"


class AssocObjAttachmentHandle;
class AssociatedObjectHandleBase;
class VertexCollHandle;
class SoMaterial;
class SoNode;

class AODSysCommonData;

class VertexHandle : public AODHandleBase {
public:

  static int numberOfInstances();

  VertexHandle(VertexCollHandle*, const xAOD::Vertex *vertex);
  virtual ~VertexHandle();
  
  /// Returns true if the 3D objects have been created
  virtual bool has3DObjects();

  /// Returns the 3Dobjects
  virtual SoNode* nodes();
  /// Should be implemented by children, in order to change the material depending on the interface etc.
  /// This will (by default) return the collection material.
  SoMaterial * determineMaterial();
	
  /// Called after some configuration related to material changes.
  void updateMaterial();
  
  /// Delete objects
  virtual void clear3DObjects(); 

  QStringList baseInfo() const;
  /** Called when user selects the node (stringlist is displayed in messagebox). 
  Reimplementations should add info momInfo() to get momentum, eta, phi, pid,charge, mass printed.*/
  virtual QStringList clicked() const;

  virtual QString shortInfo() const ; //!< returns position & type information about vertex
  virtual QString type() const { return QString("Vertex"); } //!< return very short word with type (maybe link with collection type?)
  QString vertexType() const;
 
  double getPositionX() const;
  double getPositionY() const;
  double getPositionZ() const;
  
private:

  // It is illegal to copy/assign a VertexHandle:
  VertexHandle(const VertexHandle & );
  VertexHandle & operator= (const VertexHandle & );

  friend class AssocObjAttachmentHandle;
  class Imp;
  Imp * d;
};


#endif
