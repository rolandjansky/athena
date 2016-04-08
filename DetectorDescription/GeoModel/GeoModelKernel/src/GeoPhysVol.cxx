/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD3E9AB0054.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD3E9AB0054.cm

//## begin module%3CD3E9AB0054.cp preserve=no
//## end module%3CD3E9AB0054.cp

//## Module: GeoPhysVol%3CD3E9AB0054; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoPhysVol.cxx

//## begin module%3CD3E9AB0054.additionalIncludes preserve=no
//## end module%3CD3E9AB0054.additionalIncludes

//## begin module%3CD3E9AB0054.includes preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoCountVolAction.h"
#include "GeoModelKernel/GeoAccessVolumeAction.h"
#include "GeoModelKernel/GeoCountVolAndSTAction.h"
#include <algorithm>
//## end module%3CD3E9AB0054.includes

// GeoPhysVol
#include "GeoModelKernel/GeoPhysVol.h"
//## begin module%3CD3E9AB0054.additionalDeclarations preserve=yes
//## end module%3CD3E9AB0054.additionalDeclarations


// Class GeoPhysVol 

GeoPhysVol::GeoPhysVol (const GeoLogVol* LogVol)
  //## begin GeoPhysVol::GeoPhysVol%3CD4040202A8.hasinit preserve=no
  //## end GeoPhysVol::GeoPhysVol%3CD4040202A8.hasinit
  //## begin GeoPhysVol::GeoPhysVol%3CD4040202A8.initialization preserve=yes
:
GeoVPhysVol (LogVol)
  //## end GeoPhysVol::GeoPhysVol%3CD4040202A8.initialization
{
  //## begin GeoPhysVol::GeoPhysVol%3CD4040202A8.body preserve=yes
  //## end GeoPhysVol::GeoPhysVol%3CD4040202A8.body
}


GeoPhysVol::~GeoPhysVol()
{
  //## begin GeoPhysVol::~GeoPhysVol%3CD3E9AB0054_dest.body preserve=yes
  for (size_t i = 0; i < m_daughters.size (); i++)
    {
      m_daughters[i]->unref ();
    }
  //## end GeoPhysVol::~GeoPhysVol%3CD3E9AB0054_dest.body
}



//## Other Operations (implementation)
void GeoPhysVol::add (GeoGraphNode* graphNode)
{
  //## begin GeoPhysVol::add%3CDD8AD20263.body preserve=yes
  m_daughters.push_back (graphNode);
  graphNode->ref ();
  graphNode->dockTo (this);
  //## end GeoPhysVol::add%3CDD8AD20263.body
}

unsigned int GeoPhysVol::getNChildVols () const
{
  //## begin GeoPhysVol::getNChildVols%3CDEB5690059.body preserve=yes
  GeoCountVolAction cv;
  exec (&cv);
  return cv.getCount ();
  //## end GeoPhysVol::getNChildVols%3CDEB5690059.body
}

PVConstLink  GeoPhysVol::getChildVol (unsigned int index) const
{
  //## begin GeoPhysVol::getChildVol%3CDEB5690077.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getVolume ();
  //## end GeoPhysVol::getChildVol%3CDEB5690077.body
}

HepGeom::Transform3D GeoPhysVol::getXToChildVol (unsigned int index) const
{
  //## begin GeoPhysVol::getXToChildVol%3CDEB5690095.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getTransform ();
  //## end GeoPhysVol::getXToChildVol%3CDEB5690095.body
}

HepGeom::Transform3D GeoPhysVol::getDefXToChildVol (unsigned int index) const
{
  //## begin GeoPhysVol::getDefXToChildVol%3CDEB56900B3.body preserve=yes
  GeoAccessVolumeAction
  av (index);
  exec (&av);
  return av.getDefTransform ();
  //## end GeoPhysVol::getDefXToChildVol%3CDEB56900B3.body
}

void GeoPhysVol::exec (GeoNodeAction *action) const
{
  //## begin GeoPhysVol::exec%3CE0D091019F.body preserve=yes
  //    
  // Put this node on the head of the path:    
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
  action->handlePhysVol (this);
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
  //## end GeoPhysVol::exec%3CE0D091019F.body
}

std::string GeoPhysVol::getNameOfChildVol (unsigned int i) const
{
  //## begin GeoPhysVol::getNameOfChildVol%3CEB17A0024A.body preserve=yes
  GeoAccessVolumeAction
  av (i);
  exec (&av);
  return av.getName ();
  //## end GeoPhysVol::getNameOfChildVol%3CEB17A0024A.body
}

Query<int> GeoPhysVol::getIdOfChildVol (unsigned int i) const
{
  //## begin GeoPhysVol::getIdOfChildVol%3E2ADC070296.body preserve=yes
	GeoAccessVolumeAction
	av (i);
	exec (&av);
	return av.getId ();
  //## end GeoPhysVol::getIdOfChildVol%3E2ADC070296.body
}

unsigned int GeoPhysVol::getNChildVolAndST () const
{
  //## begin GeoPhysVol::getNChildVolAndST%3FD32F39000A.body preserve=yes  
  GeoCountVolAndSTAction cv;
  exec (&cv);
  return cv.getCount();
  //## end GeoPhysVol::getNChildVolAndST%3FD32F39000A.body
}

// Additional Declarations
  //## begin GeoPhysVol%3CD3E9AB0054.declarations preserve=yes




HepGeom::Transform3D GeoPhysVol::getX    () const {
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

HepGeom::Transform3D GeoPhysVol::getDefX    () const {
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

  

unsigned int GeoPhysVol::getNChildNodes() const {
  return m_daughters.size();
}

const GeoGraphNode * const * GeoPhysVol::getChildNode(unsigned int i) const {
  return &(m_daughters[i]);
}
const GeoGraphNode * const * GeoPhysVol::findChildNode(const GeoGraphNode * n) const {
  std::vector<const GeoGraphNode *>::const_iterator i = std::find(m_daughters.begin(),m_daughters.end(),n);
  if (i==m_daughters.end()) return NULL;
  else return &*i;
}



  //## end GeoPhysVol%3CD3E9AB0054.declarations

//## begin module%3CD3E9AB0054.epilog preserve=yes
//## end module%3CD3E9AB0054.epilog
