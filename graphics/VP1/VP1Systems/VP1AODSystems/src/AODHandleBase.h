/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class AODHandleBase.cxx                     //
//                                                            //
//  Description: Base class for AOD handles.                //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef AODHANDLEBASE_H
#define AODHANDLEBASE_H

//#include "GeoPrimitives/GeoPrimitives.h"

#include <QStringList>
#include <QTreeWidgetItem>
#include <vector>
#include <set>
#include <QList>
#include <QFlags>
#include <iostream>

class AssocObjAttachmentHandle;
class AssociatedObjectHandleBase;
//class IParticleCollHandleBase;
//class TrackSysCommonData;

class SoMaterial;
class SoNode;

namespace xAOD {
	class IParticle;
}

class AODCollHandleBase;
class AODSysCommonData;

class AODHandleBase {
public:

  static int numberOfInstances();

  AODHandleBase(AODCollHandleBase*);
  virtual ~AODHandleBase();
  
  AODSysCommonData * common() const;

  void setVisible(bool);//!< use by the collection handle.
  bool visible() const { return m_visible; }
  AODCollHandleBase * collHandle() const { return m_collhandle; }
	/// Returns true if the 3D objects have been created
	virtual bool has3DObjects() {return false;}
	/// Called after some configuration changes, or when the object is first created. Must be overloaded by children.
  virtual void update3DObjects(); 
	virtual void rebuild3DObjects();
	/// Delete objects.
  virtual void clear3DObjects()=0; 
	/// Connect to the scene graph.
	virtual void attach3DObjects();
	/// Remove from the scene graph.
	virtual void detach3DObjects();	
	/// Returns the 3Dobjects
	virtual SoNode* nodes()=0;
	/// Should be implemented by children, in order to change the material depending on the interface etc.
	/// This will (by default) return the collection material.
	virtual SoMaterial * determineMaterial()=0;
	
	/// Called after some configuration related to material changes.
  void updateMaterial();
	/// Returns the current material of the handle.
  SoMaterial * currentMaterial() const { return m_currentmaterial; }

  virtual QStringList baseInfo() const;
  /** Called when user selects the node (stringlist is displayed in messagebox). 
  Reimplementations should add info momInfo() to get momentum, eta, phi, pid,charge, mass printed.*/
  virtual QStringList clicked() const = 0;

//  virtual Amg::Vector3D momentum() const { return Amg::Vector3D(0,0,0); }

//  virtual const xAOD::IParticle& iParticle() const =0;

//  virtual double charge() const =0 ;//!< Returns unknown() in case of trouble.
//  virtual double mass() const;//!< Returns unknown() in case of trouble.

//  bool hasCharge() const { return charge()!=unknown(); }
//  bool hasMass() const { return mass()!=unknown(); }

  static double unknown() { return -999.0; }

  virtual QString shortInfo() const { return QString("Not yet implemented"); } //!< returns mom and hit information about track
  virtual QString type() const { return QString("Not yet implemented"); } //!< return very short word with type (maybe link with collection type?)
  
  // Object browser code.
	/// Create and fill the object browser QTreeWidgetItem.
  virtual void fillObjectBrowser(QList<QTreeWidgetItem *>& list); 
	/// Update  object browser QTreeWidgetItem.
  void updateObjectBrowser(){};
	/// Return the QTreeWidgetItem;
  QTreeWidgetItem* browserTreeItem() const ;
  
  virtual void dumpToJSON( std::ofstream& ) const {}; //!< Dummy implementation does nothing.
  
protected:

  virtual void visibleStateChanged();//!< override if need to take action in this case.
  virtual void currentMaterialChanged() {};//!< override if need to take action in this case.

private:

  // It is illegal to copy/assign a AODHandleBase.cxx:
  AODHandleBase(const AODHandleBase & );
  AODHandleBase & operator= (const AODHandleBase & );

  friend class AssocObjAttachmentHandle;
  class Imp;
  Imp * d;


  //Here for inlining:
  bool m_visible;
  AODCollHandleBase * m_collhandle;
  SoMaterial * m_currentmaterial;
};


#endif
