/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD87550219.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD87550219.cm

//## begin module%3CDD87550219.cp preserve=no
//## end module%3CDD87550219.cp

//## Module: GeoFullPhysVol%3CDD87550219; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoFullPhysVol.cxx

//## begin module%3CDD87550219.additionalIncludes preserve=no
//## end module%3CDD87550219.additionalIncludes

//## begin module%3CDD87550219.includes preserve=yes
//## end module%3CDD87550219.includes

// GeoFullPhysVol
#include "GeoModelKernel/GeoFullPhysVol.h"
//## begin module%3CDD87550219.additionalDeclarations preserve=yes
#include <algorithm>
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoCountVolAction.h"
#include "GeoModelKernel/GeoAccessVolumeAction.h"
#include "GeoModelKernel/GeoCountVolAndSTAction.h"
//## end module%3CDD87550219.additionalDeclarations


// Class GeoFullPhysVol 

GeoFullPhysVol::GeoFullPhysVol (const GeoLogVol* LogVol)
  //## begin GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.hasinit preserve=no
  //## end GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.hasinit
  //## begin GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.initialization preserve=yes
  :
GeoVFullPhysVol (LogVol),
m_cloneOrigin(0)
  //## end GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.initialization
{
  //## begin GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.body preserve=yes
  //## end GeoFullPhysVol::GeoFullPhysVol%3CDD899100DB.body
}


GeoFullPhysVol::~GeoFullPhysVol()
{
  //## begin GeoFullPhysVol::~GeoFullPhysVol%3CDD87550219_dest.body preserve=yes
  for (size_t i = 0; i < m_daughters.size (); i++)
    m_daughters[i]->unref ();
  if(m_cloneOrigin && m_cloneOrigin!=this) 
    m_cloneOrigin->unref();
  //## end GeoFullPhysVol::~GeoFullPhysVol%3CDD87550219_dest.body
}



//## Other Operations (implementation)
void GeoFullPhysVol::add (GeoGraphNode* graphNode)
{
  //## begin GeoFullPhysVol::add%3CDD8ADA0138.body preserve=yes
  if(m_cloneOrigin)
    throw std::runtime_error("Attempt to modify contents of a cloned FPV");

  m_daughters.push_back (graphNode);
  graphNode->ref ();
  graphNode->dockTo (this);
  //## end GeoFullPhysVol::add%3CDD8ADA0138.body
}

unsigned int GeoFullPhysVol::getNChildVols () const
{
  //## begin GeoFullPhysVol::getNChildVols%3CDEA0B70318.body preserve=yes
  GeoCountVolAction cv;
  exec (&cv);
  return cv.getCount ();
  //## end GeoFullPhysVol::getNChildVols%3CDEA0B70318.body
}

PVConstLink  GeoFullPhysVol::getChildVol (unsigned int index) const
{
  //## begin GeoFullPhysVol::getChildVol%3CDEA0B70336.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getVolume ();
  //## end GeoFullPhysVol::getChildVol%3CDEA0B70336.body
}

HepGeom::Transform3D GeoFullPhysVol::getXToChildVol (unsigned int index) const
{
  //## begin GeoFullPhysVol::getXToChildVol%3CDEA0B70354.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getTransform ();
  //## end GeoFullPhysVol::getXToChildVol%3CDEA0B70354.body
}

HepGeom::Transform3D GeoFullPhysVol::getDefXToChildVol (unsigned int index) const
{
  //## begin GeoFullPhysVol::getDefXToChildVol%3CDEA0B70372.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getDefTransform ();
  //## end GeoFullPhysVol::getDefXToChildVol%3CDEA0B70372.body
}

void GeoFullPhysVol::exec (GeoNodeAction *action) const
{
  //## begin GeoFullPhysVol::exec%3CE0D088000B.body preserve=yes

  //        
  // Put this node on the head of the path:   bb 
  //    
  action->getPath ()->push (this);
  if (action->getDepthLimit ().isValid ()
      && action->getPath ()->getLength () - 1 > action->getDepthLimit ())
    {
      action->getPath ()->pop ();
      return;
    }
  //    
  // Perform the action on this node:    
  //    
  action->handleFullPhysVol (this);
  if (action->shouldTerminate ())
    {
      action->getPath ()->pop ();
      return;
    }

  //    
  // Pass this on to the children.    
  //    
  if (action->getDepthLimit ().isValid ()
      && action->getPath ()->getLength () > action->getDepthLimit ())
    {
    }
  else
    {
      for (size_t c = 0; c < m_daughters.size (); c++)
	{
	  m_daughters[c]->exec (action);
	  if (action->shouldTerminate ())
	    {
	      action->getPath ()->pop ();
	      return;
	    }
	}
    }

  //    
  // Take this node back off the head of the path:    
  //    
  action->getPath ()->pop ();
  //## end GeoFullPhysVol::exec%3CE0D088000B.body
}

std::string GeoFullPhysVol::getNameOfChildVol (unsigned int i) const
{
  //## begin GeoFullPhysVol::getNameOfChildVol%3CEB1777018C.body preserve=yes
  GeoAccessVolumeAction
  av (i);
  exec (&av);
  return av.getName ();
  //## end GeoFullPhysVol::getNameOfChildVol%3CEB1777018C.body
}

Query<int> GeoFullPhysVol::getIdOfChildVol (unsigned int i) const
{
  //## begin GeoFullPhysVol::getIdOfChildVol%3E2AE8120232.body preserve=yes
  GeoAccessVolumeAction
  av (i);
  exec (&av);
  return av.getId ();
  //## end GeoFullPhysVol::getIdOfChildVol%3E2AE8120232.body
}

unsigned int GeoFullPhysVol::getNChildVolAndST () const
{
  //## begin GeoFullPhysVol::getNChildVolAndST%3FD32F5201B9.body preserve=yes
  GeoCountVolAndSTAction cv;
  exec (&cv);
  return cv.getCount();
  //## end GeoFullPhysVol::getNChildVolAndST%3FD32F5201B9.body
}

// Additional Declarations
  //## begin GeoFullPhysVol%3CDD87550219.declarations preserve=yes
  // Meaning of the input parameter 'attached'
  // TRUE: all cloned volumes are meant to stay identical to their clone origin for the lifetime
  //       further changes are permitted neither in the origin nor in the clone results
  //
  // FALSE: use this value if you expect further changes in either clone origing or its clone results
  //        which don't need to be syncronized. The clone origin and its clone are identical ONLY by
  //        the time of cloning, further identity is not guaranteed
GeoFullPhysVol* GeoFullPhysVol::clone(bool attached)
{
  GeoFullPhysVol* clone = new GeoFullPhysVol(this->getLogVol());
  for(unsigned int ind = 0; ind < this->m_daughters.size(); ind++) {
    GeoGraphNode* daughter =(GeoGraphNode*) m_daughters[ind];
    clone->add(daughter);
  }

  if(attached) {
    if(this->m_cloneOrigin==0) {
      this->m_cloneOrigin = this;
    }
    clone->m_cloneOrigin = this->m_cloneOrigin;
    this->m_cloneOrigin->ref();
  }

  return clone;
}

const GeoFullPhysVol* GeoFullPhysVol::cloneOrigin() const
{
  return m_cloneOrigin;
}

// Breaks the consistency of cloned volumes!
// Use it only in Simulation jobs and
// Don't call it until geometry has been completely translated to G4
void GeoFullPhysVol::clear()
{
  for(size_t i=0; i<m_daughters.size(); i++)
    m_daughters[i]->unref();
  m_daughters.clear();
}

HepGeom::Transform3D GeoFullPhysVol::getX    () const {
  //
  // Check we are not shared:
  //
  if (isShared()) throw std::runtime_error("Transform requested from shared volume");
  HepGeom::Transform3D xform;
  //
  // Get the address of the first graph node, from the parent:
  //
  const GeoGraphNode * const * fence =  getParent()->getChildNode(0);
  const GeoGraphNode * const * node1 =  getParent()->findChildNode(this);
  
  for( const GeoGraphNode * const * current = node1 - 1; current>=fence; current--) {

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoVPhysVol *>(*current)) {                   //
      break;                                                             //
    }                                                                    //
    //-------------------------------------------------------------------//

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoSerialTransformer *>(*current)) {          //
      break;                                                             //
    }                                                                    //
    //-------------------------------------------------------------------//

    const GeoTransform *xf = dynamic_cast<const GeoTransform *> (*current);
    
    //-------------------------------------------------------------------//
    // If this happens, accumulate into transform                        //
    if (xf) {                                                            //
      xform  = xf->getTransform()*xform;                                 //
    }                                                                    //
    //-------------------------------------------------------------------//
  }
  return xform;
  
}

HepGeom::Transform3D GeoFullPhysVol::getDefX    () const {
  //
  // Check we are not shared:
  //
  if (isShared()) throw std::runtime_error("Transform requested from shared volume");
  HepGeom::Transform3D xform;
  //
  // Get the address of the first graph node, from the parent:
  //
  const GeoGraphNode * const * fence =  getParent()->getChildNode(0);
  const GeoGraphNode * const * node1 =  getParent()->findChildNode(this);
  
  for( const GeoGraphNode * const * current = node1 - 1; current>=fence; current--) {

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoVPhysVol *>(*current)) {                   //
      break;                                                             //
    }                                                                    //
    //-------------------------------------------------------------------//

    // If this happens, we are done, compute & return--------------------//
    //                                                                   //
    if (dynamic_cast<const GeoSerialTransformer *>(*current)) {          //
      break;                                                             //
    }                                                                    //
    //-------------------------------------------------------------------//

    const GeoTransform *xf = dynamic_cast<const GeoTransform *> (*current);
    
    //-------------------------------------------------------------------//
    // If this happens, accumulate into transform                        //
    if (xf) {                                                            //
      xform  = xf->getDefTransform()*xform;                              //
    }                                                                    //
    //-------------------------------------------------------------------//
  }
  return xform;
  
}

  

unsigned int GeoFullPhysVol::getNChildNodes() const {
  return m_daughters.size();
}

const GeoGraphNode * const * GeoFullPhysVol::getChildNode(unsigned int i) const {
  return &(m_daughters[i]);
}
const GeoGraphNode * const * GeoFullPhysVol::findChildNode(const GeoGraphNode * n) const {
  std::vector<const GeoGraphNode *>::const_iterator i = std::find(m_daughters.begin(),m_daughters.end(),n);
  if (i==m_daughters.end()) return NULL;
  else return &*i;
}

  //## end GeoFullPhysVol%3CDD87550219.declarations

//## begin module%3CDD87550219.epilog preserve=yes
//## end module%3CDD87550219.epilog
