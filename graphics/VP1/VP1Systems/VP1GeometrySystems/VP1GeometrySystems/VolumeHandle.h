/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VOLUMEHANDLE_H
#define VOLUMEHANDLE_H

#include <vector>

#include "VP1GeometrySystems/VP1GeoFlags.h"
#include "GeoModelKernel/GeoPVConstLink.h"
#include <QString>

//Only initialises child classes on demand. And even then their SoNodes are not initialised until they must be shown.
class VolumeHandleSharedData;
class SoMaterial;
class SoSeparator;
class GeoMaterial;
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbMatrix.h>

class VolumeHandle {
public:

  enum MuonChamberState { NONMUONCHAMBER,//Default
			  MUONCHAMBERCHILD,//Top-level parent is muon chamber
			  MUONCHAMBER,//Muon chamber, adjusted to event data
			  MUONCHAMBER_DIRTY };//Muon chamber, not adjusted to event data
  inline bool isInMuonChamber() const;//Muon chamber volume or muon chamber offspring
  inline bool isMuonChamber() const;//Muon chamber volume (top-level volume)
  inline void setMuonChamberDirty(bool);//Don't call unless isMuonChamber()
  inline bool muonChamberDirty() const;//if is a muon chamber volume which is dirty
  inline MuonChamberState muonChamberState() const;

  VolumeHandle(VolumeHandleSharedData * ,VolumeHandle * parent, const GeoPVConstLink&,int childNumber,
	       const MuonChamberState& mcs = NONMUONCHAMBER, const SbMatrix& accumTrans = SbMatrix() );

  virtual ~VolumeHandle();//lots of stuff to do here!
  //Used (recursively) upon deletion (never delete before children are deleted).
  void initialiseChildren();
  inline bool childrenAreInitialised() const; //Always check this before getting the child list iterators
  unsigned nChildren() const;//Works even before children are initialised

  SoMaterial * material();

  //Information for printout:
  QString getName() const;
  int copyNumber() const;//Returns -1 if doesnt have a valid copy number (because volume is not replicated), -2 in case of error.
  quint32 hashID() const;//For vp1 persistification

  GeoPVConstLink geoPVConstLink() const;
  const GeoMaterial * geoMaterial() const;
  std::string getNameStdString() const;

  bool hasName(const std::string&) const;

  const SbMatrix& getGlobalTransformToVolume() const;//Transform from world to this volume.
  SbMatrix getLocalTransformToVolume() const;//Transform from mother (=world for treetops) to this volume.

  bool isPositiveZ() const;//Uses getGlobalTransformToVolume to figure out whether Z of the transformation.

  /////////////////////
  //   Navigation:   //
  /////////////////////
  typedef std::vector<VolumeHandle *> VolumeHandleList;
  typedef VolumeHandleList::iterator VolumeHandleListItr;
  typedef VolumeHandleList::const_iterator VolumeHandleListConstItr;

  inline VolumeHandle * parent();//returns 0 if toplevel.
  inline VolumeHandle * topLevelParent();//goes up through parents and returns the toplevel one (returns itself if toplevel).
  inline VolumeHandleListItr childrenBegin();//Dont use if not childrenAreInitialised().
  inline VolumeHandleListItr childrenEnd();//Dont use if not childrenAreInitialised()
  inline VolumeHandle * child(int index) const;//Will crash if out of range!
  inline int childNumber() const;

  //  void zoomToVolume(SoCamera*, SoSeparator * root);
  //  void ensureVisibleInTreeWidget();

  inline VP1GeoFlags::VOLSTATE state() const;
  void setState( const VP1GeoFlags::VOLSTATE& state );
  void reset();//Set state of volume and all daughters to contracted
  bool isAttached() const;
  void contractDaughtersRecursively();

  bool isEther() const;
  void expandMothersRecursivelyToNonEther();//expands the tree down to volumes that are either not ether or without children

  bool isInitialisedAndHasNonStandardShape() const;//True if the 3D shape of this volume can not be represented by standard VRML nodes.

  SoSeparator * nodeSoSeparator() const;//Returns pointer to 3D representation of volume (null if not built yet).
  void ensureBuildNodeSep();

  //Convenience:
  //   void zap();
  //   void ensureVisible();
  //   void expandAllChildren(unsigned limit = 50);

  //  void ensureVisibleInTreeWidget();
  //  double mass() const;
  //  int copyNumber() const;
  // ...
  // Get SoNodes, pvconstlink, ...
  
  void updateLabels();

  QByteArray getPersistifiableState() const;
  void applyPersistifiableState(QByteArray);

  // Which subsystem I'm in?
  VP1GeoFlags::SubSystemFlag subsystem() const;

  // For labels
  virtual QString getDescriptiveName() const;
  virtual QString muonChamberT0(unsigned int=0) const;

public:
  class Imp;//For once this is declared public. This is to avoid
	    //problems with datastream operators.
private:

  // Illegal to copy/assign a VolumeHandle:
  VolumeHandle(const VolumeHandle & );
  VolumeHandle & operator= (const VolumeHandle & );

  Imp * d;

  //Here for inline methods:
  const int m_childNumber;//0 if the first child of parent, 1 if the second, etc.
  const unsigned m_nchildren;//cached for efficiency.
  MuonChamberState m_muonChamberState;

  VolumeHandle * m_parent;
  VolumeHandleList m_children;
  VP1GeoFlags::VOLSTATE m_state;
  //Some of these could be moved to private implementation with some care:
  bool haveParentsNotExpanded() const;
  void attachAllContractedChildren();//also checks children of expanded children (recursively).
  void detachAllContractedChildren();//also checks children of expanded children (recursively).
};

/////////////////
//   INLINES   //
/////////////////

inline VolumeHandle * VolumeHandle::parent() { return m_parent; }
inline VolumeHandle * VolumeHandle::topLevelParent() { return m_parent ? m_parent->topLevelParent() : this; }
inline bool VolumeHandle::childrenAreInitialised() const { return m_children.size()==m_nchildren; }
inline VolumeHandle::VolumeHandleListItr VolumeHandle::childrenBegin() { return m_children.begin(); }
inline VolumeHandle::VolumeHandleListItr VolumeHandle::childrenEnd() { return m_children.end(); }
inline unsigned VolumeHandle::nChildren() const { return m_nchildren; }
inline VolumeHandle * VolumeHandle::child(int index) const { return m_children.at(index); }
inline int VolumeHandle::childNumber() const { return m_childNumber; }
inline VP1GeoFlags::VOLSTATE VolumeHandle::state() const { return m_state; }
inline bool VolumeHandle::isInMuonChamber() const { return m_muonChamberState!=NONMUONCHAMBER; }
inline bool VolumeHandle::isMuonChamber() const { return m_muonChamberState==MUONCHAMBER || m_muonChamberState==MUONCHAMBER_DIRTY; }
inline VolumeHandle::MuonChamberState VolumeHandle::muonChamberState() const { return m_muonChamberState; }
inline void VolumeHandle::setMuonChamberDirty(bool b) {  if (isMuonChamber()) m_muonChamberState = b?MUONCHAMBER_DIRTY:MUONCHAMBER; }
inline bool VolumeHandle::muonChamberDirty() const { return m_muonChamberState==MUONCHAMBER_DIRTY; }
inline void VolumeHandle::reset() {
  setState( VP1GeoFlags::CONTRACTED );
  contractDaughtersRecursively();
}
inline QString VolumeHandle::getDescriptiveName() const { 
  QString name = getName();
  name.remove("_Station");
  return name;
}

inline QString VolumeHandle::muonChamberT0(unsigned int /**i*/) const { 
  return QString();
}


#endif
