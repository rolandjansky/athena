/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoSerialDenominator.h"
#include "GeoModelKernel/GeoSerialIdentifier.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include <sstream>

GeoVolumeCursor::GeoVolumeCursor (PVConstLink parent)

  :
  _parent(parent),
  _majorIndex(0),
  _minorIndex(0),
  _minorLimit(0),
  _serialTransformer(NULL),
  _nameTag (NULL),
  _serialDenominator (NULL),
  _idTag(NULL),
  _serialDenomPosition (0),
  _serialIdentifier(NULL),
  _serialIdentPosition(0),
  _volCount(0),
  _hasAlignTrans(false)
{
  setDepthLimit(0);
  next();
}

void GeoVolumeCursor::next() {


  resuscitate();
  
  
  if (_minorLimit) { // We are doing a serial transformer
    _minorIndex++;
    if (_minorIndex==_minorLimit) {  
      _minorIndex=0;
      _minorLimit=0;
      _majorIndex++;
      next();
    }
    else {
      _volCount++;
    }              
  }
  else {           // We are not doing a serial transformer
    
    _idTag             = NULL;
    _nameTag           = NULL;
    _serialTransformer = NULL;
    _pendingTransformList.erase (_pendingTransformList.begin (),
				 _pendingTransformList.end ());
    _hasAlignTrans = false;
    _volume=NULL;
    
    int N = _parent->getNChildNodes();
    if(N==0) return;
    const GeoGraphNode * const *node  = _parent->getChildNode(_majorIndex);
    const GeoGraphNode * const *back  = _parent->getChildNode(N-1);
    const GeoGraphNode * const *end   = back+1;
    while (node!=end) {
      (*node)->exec(this);
      if (_minorLimit) { // We have stepped into ST
      }
      else {            // We have not stepped into ST.
	node++;
	_majorIndex++;
      }
      if (_terminate) {
	_volCount++;
	break;
      }
    }
  }
}

void GeoVolumeCursor::resuscitate() {
  _terminate = false;
}



bool GeoVolumeCursor::atEnd() const {
  return !_volume;
}

GeoVolumeCursor::~GeoVolumeCursor()
{
}



void GeoVolumeCursor::handleTransform (const GeoTransform *xform)
{
  _pendingTransformList.push_back (xform);
  if(dynamic_cast<const GeoAlignableTransform*>(xform))
    _hasAlignTrans = true;
}

void GeoVolumeCursor::handlePhysVol (const GeoPhysVol *vol)
{

  _volume = vol;
  unsigned int listSize = _pendingTransformList.size ();
  if (listSize == 0) {
      _transform    = HepGeom::Transform3D();
      _defTransform = HepGeom::Transform3D();
  }
  else {
    _transform = _pendingTransformList[0]->getTransform ();
    _defTransform = _pendingTransformList[0]->getDefTransform ();
    for (unsigned int t = 1; t < _pendingTransformList.size (); t++) {
      _transform    = _transform    * _pendingTransformList[t]->getTransform ();
      _defTransform = _defTransform * _pendingTransformList[t]->getDefTransform ();
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleFullPhysVol (const GeoFullPhysVol *vol)
{
  _volume = vol;
  unsigned int listSize = _pendingTransformList.size ();
  if (listSize == 0) {
      _transform    = HepGeom::Transform3D();
      _defTransform = HepGeom::Transform3D();
  }
  else {
    _transform = _pendingTransformList[0]->getTransform ();
    _defTransform = _pendingTransformList[0]->getDefTransform ();
    for (unsigned int t = 1; t < _pendingTransformList.size (); t++) {
      _transform    = _transform    * _pendingTransformList[t]->getTransform ();
      _defTransform = _defTransform * _pendingTransformList[t]->getDefTransform ();
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleSerialTransformer (const GeoSerialTransformer  *sT)
{
  
  _minorLimit = sT->getNCopies();
  _minorIndex = 0;
  _serialTransformer = sT;
  _volume = sT->getVolume();

  unsigned int listSize = _pendingTransformList.size ();
  if (listSize == 0) {
      _transform    = HepGeom::Transform3D();
      _defTransform = HepGeom::Transform3D();
  }
  else {
    _transform = _pendingTransformList[0]->getTransform ();
    _defTransform = _pendingTransformList[0]->getDefTransform ();
    for (unsigned int t = 1; t < _pendingTransformList.size (); t++) {
      _transform    = _transform    * _pendingTransformList[t]->getTransform ();
      _defTransform = _defTransform * _pendingTransformList[t]->getDefTransform ();
    }
  }
  terminate ();
}

void GeoVolumeCursor::handleNameTag (const GeoNameTag *nameTag)
{
  _nameTag = nameTag;
  _serialDenominator = NULL;
  _serialDenomPosition = 0;
}

void GeoVolumeCursor::handleSerialDenominator (const GeoSerialDenominator *sD)
{
  _serialDenominator = sD;
  _serialDenomPosition = _volCount;
}


void GeoVolumeCursor::handleIdentifierTag (const GeoIdentifierTag *idTag)
{
    _idTag = idTag;
    _serialIdentifier = NULL;
    _serialIdentPosition = _volCount;
}

void GeoVolumeCursor::handleSerialIdentifier(const GeoSerialIdentifier *sI)
{
  _serialIdentifier = sI;
  _serialIdentPosition =_volCount;
}


PVConstLink GeoVolumeCursor::getVolume () const
{
  return _volume;
}

HepGeom::Transform3D GeoVolumeCursor::getTransform () const
{
  if (_minorLimit) {
    return _transform*_serialTransformer->getTransform(_minorIndex);
  }
  else {
    return _transform;
  }
}

HepGeom::Transform3D GeoVolumeCursor::getDefTransform () const
{
  if (_minorLimit) {
    return _transform*_serialTransformer->getTransform(_minorIndex);
  }
  else {
    return _defTransform;
  }
}



std::string  GeoVolumeCursor::getName () const
{
  std::string name;
  if (_nameTag)
    {
      name = _nameTag->getName ();
    }
  else if (_serialDenominator)
    {
      std::ostringstream o;
      o << _serialDenominator->getBaseName() << (int) (_volCount - _serialDenomPosition -1) ;   
      name = o.str();  
    }
  else
    {
      name = "ANON";
    }
  return name;
}


Query<int> GeoVolumeCursor::getId () const
{
  Query<int> id;
  if (_idTag)
    {
      id = Query<int> (_idTag->getIdentifier ());
    }
  else if (_serialIdentifier)
    {
      id = Query<int> (_volCount - _serialIdentPosition - 1 + _serialIdentifier->getBaseId());
    }
  return id;
}

bool GeoVolumeCursor::hasAlignableTransform() const
{
  return _hasAlignTrans;
}
