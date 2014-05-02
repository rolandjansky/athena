/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CE11DD8028A.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CE11DD8028A.cm

//## begin module%3CE11DD8028A.cp preserve=no
//## end module%3CE11DD8028A.cp

//## Module: GeoAccessVolumeAction%3CE11DD8028A; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoAccessVolumeAction.cxx

//## begin module%3CE11DD8028A.additionalIncludes preserve=no
//## end module%3CE11DD8028A.additionalIncludes

//## begin module%3CE11DD8028A.includes preserve=yes
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include <sstream>
#include <stdio.h>
//## end module%3CE11DD8028A.includes

// GeoAccessVolumeAction
#include "GeoModelKernel/GeoAccessVolumeAction.h"
//## begin module%3CE11DD8028A.additionalDeclarations preserve=yes
//## end module%3CE11DD8028A.additionalDeclarations


// Class GeoAccessVolumeAction 

GeoAccessVolumeAction::GeoAccessVolumeAction (unsigned int Index)
  //## begin GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.hasinit preserve=no
  //## end GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.hasinit
  //## begin GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.initialization preserve=yes
  :
_index (Index),
_counter (0),
_nameTag (NULL),
_serialDenominator (NULL),
_idTag(NULL),
_serialDenomPosition (0),
_serialIdentifier(NULL),
_serialIdentPosition(0)
  //## end GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.initialization
{
  //## begin GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.body preserve=yes
  setDepthLimit (1);
  _pendingTransformList.reserve(1);
  //## end GeoAccessVolumeAction::GeoAccessVolumeAction%3CE1249902FE.body
}


GeoAccessVolumeAction::~GeoAccessVolumeAction()
{
  //## begin GeoAccessVolumeAction::~GeoAccessVolumeAction%3CE11DD8028A_dest.body preserve=yes
  //## end GeoAccessVolumeAction::~GeoAccessVolumeAction%3CE11DD8028A_dest.body
}



//## Other Operations (implementation)
void GeoAccessVolumeAction::handleTransform (const GeoTransform *xform)
{
  //## begin GeoAccessVolumeAction::handleTransform%3CE121B602D4.body preserve=yes
  _pendingTransformList.push_back (xform);
  //## end GeoAccessVolumeAction::handleTransform%3CE121B602D4.body
}

void GeoAccessVolumeAction::handlePhysVol (const GeoPhysVol *vol)
{
  //## begin GeoAccessVolumeAction::handlePhysVol%3CE12437022B.body preserve=yes
  //      
  // Return if we have not got to any children:     
  //     
  if (getPath ()->getLength () - 1 == 0)
    return;
  //     
  // Otherwise check if the index equals the counter:     
  //     
  if (_index == _counter)
    {
      volume = vol;
      unsigned int listSize = _pendingTransformList.size ();
      if (listSize == 0)
	{
	  // do nothing.... 
	}
      else if (listSize == 1)
	{
	  transform = _pendingTransformList[0]->getTransform ();
	  defTransform = _pendingTransformList[0]->getDefTransform ();
	}
      else
	{
	  for (unsigned int t = 0; t < _pendingTransformList.size (); t++)
	    {
	      transform =
		transform * _pendingTransformList[t]->getTransform ();
	      defTransform =
		defTransform * _pendingTransformList[t]->getDefTransform ();
	    }
	}
      terminate ();
    }
  else
    {
	  _idTag   = NULL;
      _nameTag = NULL;
      _pendingTransformList.erase (_pendingTransformList.begin (),
				  _pendingTransformList.end ());
    }
  //     
  // Increment the counter:     
  //     
  _counter++;
  //## end GeoAccessVolumeAction::handlePhysVol%3CE12437022B.body
}

void GeoAccessVolumeAction::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  //## begin GeoAccessVolumeAction::handleFullPhysVol%3CE12437023F.body preserve=yes

  //      
  // Return if we have not got to any children:     
  //     
  if (getPath ()->getLength () - 1 == 0)
    return;
  //     
  // Otherwise check if the index equals the counter:     
  //     
  if (_index == _counter)
    {
      volume = vol;
      unsigned int listSize = _pendingTransformList.size ();
      if (listSize == 0)
	{
	  // do nothing.... 
	}
      else if (listSize == 1)
	{
	  transform = _pendingTransformList[0]->getTransform ();
	  defTransform = _pendingTransformList[0]->getDefTransform ();
	}
      else
	{
	  for (unsigned int t = 0; t < _pendingTransformList.size (); t++)
	    {
	      transform =
		transform * _pendingTransformList[t]->getTransform ();
	      defTransform =
		defTransform * _pendingTransformList[t]->getDefTransform ();
	    }
	}
      terminate ();
    }
  else
    {
      _pendingTransformList.erase (_pendingTransformList.begin (),
				  _pendingTransformList.end ()); 
	  _nameTag = NULL;
      _idTag   = NULL;
    }
  //     
  // Increment the counter:     
  //     
  _counter++;
  //## end GeoAccessVolumeAction::handleFullPhysVol%3CE12437023F.body
}

PVConstLink GeoAccessVolumeAction::getVolume () const
{
  //## begin GeoAccessVolumeAction::getVolume%3CE124F50044.body preserve=yes
  return volume;
  //## end GeoAccessVolumeAction::getVolume%3CE124F50044.body
}

const HepGeom::Transform3D & GeoAccessVolumeAction::getTransform () const
{
  //## begin GeoAccessVolumeAction::getTransform%3CE1257A02B2.body preserve=yes
  return transform;
  //## end GeoAccessVolumeAction::getTransform%3CE1257A02B2.body
}

const HepGeom::Transform3D & GeoAccessVolumeAction::getDefTransform () const
{
  //## begin GeoAccessVolumeAction::getDefTransform%3CE1317A0331.body preserve=yes
  return defTransform;
  //## end GeoAccessVolumeAction::getDefTransform%3CE1317A0331.body
}

const std::string & GeoAccessVolumeAction::getName () const
{
  //## begin GeoAccessVolumeAction::getName%3CEB0CE5029B.body preserve=yes
  if (_name.empty ())
    {
      if (_nameTag)
	{
	  _name = _nameTag->getName ();
	}
      else if (_serialDenominator)
	{

	  // Yes this is totally stupid but the ostringstream classes in the gnu 
	  // compiler are broken....it seems: 
	  // gcc version 2.96 20000731 (Red Hat Linux 7.3 2.96-110) 

	  char Buff[500];
	  snprintf (Buff, 500, "%d", _index - _serialDenomPosition);
	  _name = _serialDenominator->getBaseName () + std::string (Buff);
	  //      o << _serialDenominator->getBaseName() << (int) _index;   
	  //      _name = o.str();  
	}
      else
	{
	  _name = "ANON";
	}
    }
  return _name;
  //## end GeoAccessVolumeAction::getName%3CEB0CE5029B.body
}

void GeoAccessVolumeAction::handleNameTag (const GeoNameTag *nameTag)
{
  //## begin GeoAccessVolumeAction::handleNameTag%3CEB0D370018.body preserve=yes
  _nameTag = nameTag;
  _serialDenominator = NULL;
  _serialDenomPosition = 0;
  //## end GeoAccessVolumeAction::handleNameTag%3CEB0D370018.body
}

void GeoAccessVolumeAction::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  //## begin GeoAccessVolumeAction::handleSerialDenominator%3CEB0D370040.body preserve=yes
  _serialDenominator = sD;
  _serialDenomPosition = _counter;
  //## end GeoAccessVolumeAction::handleSerialDenominator%3CEB0D370040.body
}

void GeoAccessVolumeAction::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  //## begin GeoAccessVolumeAction::handleSerialTransformer%3D208CCE030A.body preserve=yes

  //     
  // Otherwise check if the index equals the counter:     
  //     
  if (_index < _counter + sT->getNCopies ())
    {
      unsigned int copy = _index - _counter;
      volume = sT->getVolume ();
      for (unsigned int t = 0; t < _pendingTransformList.size (); t++)
	{
	  transform = transform * _pendingTransformList[t]->getTransform ();
	  defTransform =
	    defTransform * _pendingTransformList[t]->getDefTransform ();
	}
      transform    = transform    * sT->getTransform (copy);
      defTransform = defTransform * sT->getTransform (copy);
      _counter += (copy + 1);
      terminate ();
    }
  else
    {
      _idTag   = NULL;
      _nameTag = NULL;
      _pendingTransformList.erase (_pendingTransformList.begin (),
				  _pendingTransformList.end ());
      _counter += sT->getNCopies ();
    }
  //## end GeoAccessVolumeAction::handleSerialTransformer%3D208CCE030A.body
}

void GeoAccessVolumeAction::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
  //## begin GeoAccessVolumeAction::handleIdentifierTag%3E217F0E0298.body preserve=yes
    _idTag = idTag;
    _serialIdentifier = NULL;
    _serialIdentPosition = 0;
  //## end GeoAccessVolumeAction::handleIdentifierTag%3E217F0E0298.body
}

Query<int> GeoAccessVolumeAction::getId () const
{
  //## begin GeoAccessVolumeAction::getId%3E217F5C0268.body preserve=yes
  Query<int> id;
  if (_idTag)
    {
      id = Query<int> (_idTag->getIdentifier ());
    }
  else if (_serialIdentifier)
    {
      id = Query<int> (_index - _serialIdentPosition + _serialIdentifier->getBaseId());
    }
  return id;
  //## end GeoAccessVolumeAction::getId%3E217F5C0268.body
}

// Additional Declarations
  //## begin GeoAccessVolumeAction%3CE11DD8028A.declarations preserve=yes
void GeoAccessVolumeAction::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  _serialIdentifier = sI;
  _serialIdentPosition = _counter;
}
  //## end GeoAccessVolumeAction%3CE11DD8028A.declarations
//## begin module%3CE11DD8028A.epilog preserve=yes
//## end module%3CE11DD8028A.epilog
