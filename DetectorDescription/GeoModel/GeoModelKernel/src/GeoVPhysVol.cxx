/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD3E93F00F9.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD3E93F00F9.cm

//## begin module%3CD3E93F00F9.cp preserve=no
//## end module%3CD3E93F00F9.cp

//## Module: GeoVPhysVol%3CD3E93F00F9; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVPhysVol.cxx

//## begin module%3CD3E93F00F9.additionalIncludes preserve=no
//## end module%3CD3E93F00F9.additionalIncludes

//## begin module%3CD3E93F00F9.includes preserve=yes
#include "GeoModelKernel/GeoAccessVolumeAction.h"
#include "GeoModelKernel/GeoVolumeAction.h"
//## end module%3CD3E93F00F9.includes

// GeoVPhysVol
#include "GeoModelKernel/GeoVPhysVol.h"
//## begin module%3CD3E93F00F9.additionalDeclarations preserve=yes
//## end module%3CD3E93F00F9.additionalDeclarations


// Class GeoVPhysVol 

GeoVPhysVol::GeoVPhysVol (const GeoLogVol* LogVol)
  //## begin GeoVPhysVol::GeoVPhysVol%3CD403D2012C.hasinit preserve=no
  //## end GeoVPhysVol::GeoVPhysVol%3CD403D2012C.hasinit
  //## begin GeoVPhysVol::GeoVPhysVol%3CD403D2012C.initialization preserve=yes
:m_parentPtr (NULL), m_logVol (LogVol)
  //## end GeoVPhysVol::GeoVPhysVol%3CD403D2012C.initialization
{
  //## begin GeoVPhysVol::GeoVPhysVol%3CD403D2012C.body preserve=yes
  if(m_logVol) m_logVol->ref();
  //## end GeoVPhysVol::GeoVPhysVol%3CD403D2012C.body
}


GeoVPhysVol::~GeoVPhysVol()
{
  //## begin GeoVPhysVol::~GeoVPhysVol%3CD3E93F00F9_dest.body preserve=yes
  if(m_logVol) m_logVol->unref();
  //## end GeoVPhysVol::~GeoVPhysVol%3CD3E93F00F9_dest.body
}



//## Other Operations (implementation)
Query<unsigned int> GeoVPhysVol::indexOf (PVConstLink daughter) const
{
  //## begin GeoVPhysVol::indexOf%3CDE97BC00B9.body preserve=yes
  for (unsigned int i = 0; i < getNChildVols (); i++)
    {
      if (getChildVol (i) == daughter)
	return i;
    }
  return Query <unsigned int >();

  //## end GeoVPhysVol::indexOf%3CDE97BC00B9.body
}

PVConstLink GeoVPhysVol::getParent () const
{
  //## begin GeoVPhysVol::getParent%3CDE6F5903B1.body preserve=yes
  if (m_parentPtr && m_parentPtr != this)
    return m_parentPtr;
  return NULL;
  //## end GeoVPhysVol::getParent%3CDE6F5903B1.body
}

const GeoLogVol* GeoVPhysVol::getLogVol () const
{
  //## begin GeoVPhysVol::getLogVol%3CDE67950290.body preserve=yes
  return m_logVol;
  //## end GeoVPhysVol::getLogVol%3CDE67950290.body
}

void GeoVPhysVol::apply (GeoVolumeAction *action) const
{
  //## begin GeoVPhysVol::apply%3CE238BB0078.body preserve=yes


  if (action->getType () == GeoVolumeAction::TOP_DOWN)
    {

      action->handleVPhysVol (this);
      if (action->shouldTerminate ())
	return;

      action->getState ()->nextLevel (this);
      int nVols = getNChildVols ();
      for (int d = 0; d < nVols; d++)
	{

	  GeoAccessVolumeAction av (d);
	  exec (&av);

	  action->getState ()->setTransform (av.getTransform ());
	  action->getState ()->setDefTransform (av.getDefTransform ());
	  action->getState ()->setId(av.getId());
	  action->getState ()->setName(av.getName());

	  av.getVolume ()->apply (action);
	  if(action->shouldTerminate ())
	    break;
	}
      action->getState ()->previousLevel ();
    }
  else if (action->getType () == GeoVolumeAction::BOTTOM_UP)
    {


      action->getState ()->nextLevel (this);
      int nVols = getNChildVols ();
      for (int d = 0; d < nVols; d++)
	{

	  GeoAccessVolumeAction av (d);
	  exec (&av);

	  action->getState ()->setTransform (av.getTransform ());
	  action->getState ()->setDefTransform (av.getDefTransform ());
	  action->getState ()->setId(av.getId());
	  action->getState ()->setName(av.getName());
	  av.getVolume ()->apply (action);
	  if(action->shouldTerminate ())
	    break;
	}
      action->getState ()->previousLevel ();

      action->handleVPhysVol (this);
      if (action->shouldTerminate ())
	return;
    }

  //## end GeoVPhysVol::apply%3CE238BB0078.body
}

void GeoVPhysVol::dockTo (GeoVPhysVol* parent)
{
  //## begin GeoVPhysVol::dockTo%3DB85BC00025.body preserve=yes
  if(m_parentPtr)
    m_parentPtr = this;
  else
    m_parentPtr = parent;
  //## end GeoVPhysVol::dockTo%3DB85BC00025.body
}

// Additional Declarations
  //## begin GeoVPhysVol%3CD3E93F00F9.declarations preserve=yes
  //## end GeoVPhysVol%3CD3E93F00F9.declarations

//## begin module%3CD3E93F00F9.epilog preserve=yes
//## end module%3CD3E93F00F9.epilog
