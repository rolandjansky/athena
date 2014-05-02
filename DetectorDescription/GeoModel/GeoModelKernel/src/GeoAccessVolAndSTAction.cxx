/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoAccessVolAndSTAction.h"

#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include <sstream>
#include <stdio.h>

GeoAccessVolAndSTAction::GeoAccessVolAndSTAction(unsigned int Index)
  :volume(0),
   serialTransformer(0),
   _index (Index),
   _counter (0),
   _nameTag (0),
   _serialDenominator (0),
   _idTag(0),
   _serialDenomPosition (0)
{
  setDepthLimit(1);
}

GeoAccessVolAndSTAction::~GeoAccessVolAndSTAction()
{
}

void GeoAccessVolAndSTAction::handleTransform(const GeoTransform *xform)
{
  _pendingTransformList.push_back (xform);
}

void GeoAccessVolAndSTAction::handlePhysVol(const GeoPhysVol *vol)
{
  // Return if we have not got any children     
  if(getPath()->getLength() - 1 == 0)
    return;

  // Otherwise check if the index equals the counter:     
  if(_index == _counter)
  {
    volume = vol;
    serialTransformer = 0;
    unsigned int listSize = _pendingTransformList.size();

    if(listSize == 0)
    {
      // do nothing.... 
    }
    else if(listSize == 1)
    {
      transform = _pendingTransformList[0]->getTransform();
      defTransform = _pendingTransformList[0]->getDefTransform();
    }
    else
      for (unsigned int t=0; t<_pendingTransformList.size(); t++)
      {
	transform = transform * _pendingTransformList[t]->getTransform();
	defTransform = defTransform * _pendingTransformList[t]->getDefTransform();
      }
    terminate ();
  }
  else
  {
    _idTag   = 0;
    _nameTag = 0;
    _pendingTransformList.erase(_pendingTransformList.begin(),
				 _pendingTransformList.end());
  }

  _counter++;
}

void GeoAccessVolAndSTAction::handleFullPhysVol(const GeoFullPhysVol *vol)
{
  // Return if we have not got any children     
  if(getPath()->getLength()-1 == 0)
    return;

  // Otherwise check if the index equals the counter:     
  if(_index == _counter)
  {
    volume = vol;
    serialTransformer = 0;

    unsigned int listSize = _pendingTransformList.size();
    if(listSize == 0)
    {
      // do nothing.... 
    }
    else if(listSize == 1)
    {
      transform = _pendingTransformList[0]->getTransform();
      defTransform = _pendingTransformList[0]->getDefTransform();
    }
    else
      for (unsigned int t = 0; t < _pendingTransformList.size (); t++)
      {
	transform =
	  transform * _pendingTransformList[t]->getTransform ();
	defTransform =
	  defTransform * _pendingTransformList[t]->getDefTransform ();
      }
    terminate ();
  }
  else
  {
    _pendingTransformList.erase (_pendingTransformList.begin (),
				 _pendingTransformList.end ()); 
    _nameTag = 0;
    _idTag   = 0;
  }

  _counter++;
}

PVConstLink GeoAccessVolAndSTAction::getVolume() const
{
  return volume;
}

const HepGeom::Transform3D & GeoAccessVolAndSTAction::getTransform() const
{
  return transform;
}

const HepGeom::Transform3D & GeoAccessVolAndSTAction::getDefTransform() const
{
  return defTransform;
}

const std::string & GeoAccessVolAndSTAction::getName() const
{
  if(_name.empty()) {
    if (_nameTag)
      _name = _nameTag->getName ();
    else if(_serialDenominator)
    {
      // Yes this is totally stupid but the ostringstream classes in the gnu 
      // compiler are broken....it seems: 
      // gcc version 2.96 20000731 (Red Hat Linux 7.3 2.96-110) 

      char Buff[500];
      snprintf (Buff, 500, "%d", _index - _serialDenomPosition);
      _name = _serialDenominator->getBaseName () + std::string (Buff);
    }
  }
  else
    _name = "ANON";

  return _name;
}

void GeoAccessVolAndSTAction::handleNameTag(const GeoNameTag *nameTag)
{
  _nameTag = nameTag;
  _serialDenominator = 0;
  _serialDenomPosition = 0;
}

void GeoAccessVolAndSTAction::handleSerialDenominator(const GeoSerialDenominator *sD)
{
  _serialDenominator = sD;
  _serialDenomPosition = _counter;
}

void GeoAccessVolAndSTAction::handleSerialTransformer(const GeoSerialTransformer  *sT)
{
  _pendingTransformList.erase(_pendingTransformList.begin(),
			      _pendingTransformList.end());

  if(_index == _counter)
  {
    volume = 0;
    serialTransformer = sT;
    terminate ();
  }
  else
  {
    _idTag   = 0;
    _nameTag = 0;
    _counter++;
  }
}

const GeoSerialTransformer* GeoAccessVolAndSTAction::getSerialTransformer() const
{
  return serialTransformer;
}

void GeoAccessVolAndSTAction::handleIdentifierTag(const GeoIdentifierTag *idTag)
{
  _idTag = idTag;
}

Query<unsigned int> GeoAccessVolAndSTAction::getId() const
{
  if(_idTag) 
    return Query<unsigned int>(_idTag->getIdentifier());
  else
    return Query<unsigned int>();
}




