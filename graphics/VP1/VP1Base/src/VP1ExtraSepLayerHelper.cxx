/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <map>
#include <iostream>

//____________________________________________________________________
class VP1ExtraSepLayerHelper::Imp {
public:

	Imp(SoSeparator* topsep, int numberOfNodesPerExtraSep)
: topsep(topsep), limitpersep(numberOfNodesPerExtraSep), largechangessave(0) {}

	SoSeparator * topsep;
	const int limitpersep;
	std::map<SoNode*,SoSeparator*> nodes2extrasep;//We dont do any special refcounting here.

	//For notifications:
	int largechangessave;

	//For addNodeUnderMaterial:
	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> > mat2sepandhelper;
};


//____________________________________________________________________
VP1ExtraSepLayerHelper::VP1ExtraSepLayerHelper(SoSeparator* topsep, int numberOfNodesPerExtraSep)
: d(new Imp(topsep,numberOfNodesPerExtraSep))
{
	d->topsep->ref();
}

//____________________________________________________________________
VP1ExtraSepLayerHelper::~VP1ExtraSepLayerHelper()
{
	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> >::iterator it, itE = d->mat2sepandhelper.end();
	for (it = d->mat2sepandhelper.begin();it!=itE;++it) {
		delete it->second.second;
		it->second.first->unref();
	}
	d->topsep->removeAllChildren();
	d->topsep->unref();
	delete d; d=0;
}


//____________________________________________________________________
void VP1ExtraSepLayerHelper::addNode(SoNode* node )
{
	assert(d->nodes2extrasep.find(node)==d->nodes2extrasep.end());
	if (VP1Msg::verbose()) {
		if (d->nodes2extrasep.find(node)!=d->nodes2extrasep.end())
			VP1Msg::message("VP1ExtraSepLayerHelper::addNode ERROR: Node already added!");
	}

	if (d->topsep->getNumChildren()==0) {
		SoSeparator * sep = new SoSeparator;
		if (VP1Msg::verbose())
			sep->setName("SepHelperInternalSep");
		sep->renderCaching.setValue(SoSeparator::ON);
		sep->boundingBoxCaching.setValue(SoSeparator::ON);
		sep->addChild(node);
		d->topsep->addChild(sep);
		d->nodes2extrasep[node]=sep;
		return;
	}

	//Try to retrieve the last of the extra separators, and see if it still has room:
	assert(d->topsep->getChild(d->topsep->getNumChildren()-1)->getTypeId().isDerivedFrom(SoSeparator::getClassTypeId()));
	SoSeparator * lastsep = static_cast<SoSeparator*>(d->topsep->getChild(d->topsep->getNumChildren()-1));
	if (lastsep->getNumChildren()<d->limitpersep) {
		lastsep->addChild(node);
		d->nodes2extrasep[node]=lastsep;
		return;
	}

	//Add a new sep and return.
	SoSeparator * sep = new SoSeparator;
	if (VP1Msg::verbose())
		sep->setName("SepHelperInternalSep");
	sep->renderCaching.setValue(SoSeparator::ON);
	sep->boundingBoxCaching.setValue(SoSeparator::ON);
	sep->addChild(node);
	d->topsep->addChild(sep);
	d->nodes2extrasep[node]=sep;
}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::removeNode(SoNode* node )
{
	assert(d->nodes2extrasep.find(node)!=d->nodes2extrasep.end());
	if (VP1Msg::verbose()) {
		if (d->nodes2extrasep.find(node)==d->nodes2extrasep.end())
			VP1Msg::message("VP1ExtraSepLayerHelper::removeNode ERROR: Node not added previously!");
	}

	//Find extrasep and lastsep:
	SoSeparator * extrasep = d->nodes2extrasep[node];
	assert(extrasep->findChild(node)>-1);
	assert(d->topsep->getChild(d->topsep->getNumChildren()-1)->getTypeId()==SoSeparator::getClassTypeId());
	SoSeparator * lastsep = static_cast<SoSeparator*>(d->topsep->getChild(d->topsep->getNumChildren()-1));
	assert(lastsep->getNumChildren()>0);

	//First, remove the node:
	node->ref();
	extrasep->removeChild(node);
	node->unref();
	d->nodes2extrasep.erase(d->nodes2extrasep.find(node));

	//Do we need to move a node from the last separator into the separator?
	if (lastsep!=extrasep&&lastsep->getNumChildren()>0) {
		SoNode * othernode = lastsep->getChild(lastsep->getNumChildren()-1);
		othernode->ref();
		lastsep->removeChild(othernode);
		extrasep->addChild(othernode);
		othernode->unref();
		d->nodes2extrasep[othernode] = extrasep;
	}

	//Do we need to remove the last separator?
	if (lastsep->getNumChildren()==0) {
		lastsep->ref();
		d->topsep->removeChild(lastsep);
		lastsep->unref();
	}
}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::largeChangesBegin()
{
	++(d->largechangessave);
	d->topsep->enableNotify(false);
	//We "sterilise" the extra separators and the extra sep helpers
	//also.

	//Thus, we disable notifications on all children of topsep, and we
	//call largeChangesBegin on all internal sephelpers.

	int n = d->topsep->getNumChildren();
	for (int i = 0; i < n; ++i)
		d->topsep->getChild(i)->enableNotify(false);

	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> >::iterator it, itE = d->mat2sepandhelper.end();
	for (it = d->mat2sepandhelper.begin(); it!=itE;++it)
		it->second.second->largeChangesBegin();

}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::largeChangesEnd()
{
	if (d->largechangessave<=0)
		return;
	if (--(d->largechangessave)!=0)
		return;

	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> >::iterator it, itE = d->mat2sepandhelper.end();
	for (it = d->mat2sepandhelper.begin(); it!=itE;++it)
		it->second.second->largeChangesEnd();

	int n = d->topsep->getNumChildren();
	for (int i = 0; i < n; ++i) {
		d->topsep->getChild(i)->enableNotify(true);
		d->topsep->getChild(i)->touch();
	}

	d->topsep->enableNotify(true);
	d->topsep->touch();
}

//____________________________________________________________________
SoSeparator * VP1ExtraSepLayerHelper::topSeparator() const
{
	return d->topsep;
}

//____________________________________________________________________
int VP1ExtraSepLayerHelper::numberOfNodesPerExtraSep() const
{
	return d->limitpersep;
}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::addNodeUnderMaterial( SoNode * node, SoMaterial * mat )
{
	VP1Msg::messageVerbose("VP1ExtraSepLayerHelper::addNodeUnderMaterial()");

	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> >::iterator it = d->mat2sepandhelper.find(mat);
	if (it==d->mat2sepandhelper.end()) {

		SoSeparator * matsep = new SoSeparator;
		if (VP1Msg::verbose())
			matsep->setName("SepHelperInternalSep_MatSep");
		matsep->ref();
		matsep->addChild(mat);
		SoSeparator * sep = new SoSeparator;
		if (VP1Msg::verbose())
			sep->setName("SepHelperInternalSep_InternalSepHelperTop");
		matsep->addChild(sep);
		VP1ExtraSepLayerHelper * mathelper = new VP1ExtraSepLayerHelper(sep,d->limitpersep);
		d->mat2sepandhelper[mat] = std::pair<SoSeparator*,VP1ExtraSepLayerHelper*>(matsep,mathelper);
		mathelper->addNode(node);
		addNode(matsep);
	} else {
		it->second.second->addNode(node);
	}
}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::removeNodeUnderMaterial(SoNode*node,SoMaterial*mat)
{
	std::map<SoMaterial*,std::pair<SoSeparator*,VP1ExtraSepLayerHelper*> >::iterator it = d->mat2sepandhelper.find(mat);
	if (it==d->mat2sepandhelper.end())
		return;
	it->second.second->removeNode(node);
	if (it->second.second->topSeparator()->getNumChildren()==0) {
		//No more use for this material apparently:
		removeNode(it->second.first);
		it->second.first->unref();
		d->mat2sepandhelper.erase(it);
	}
}

//____________________________________________________________________
void VP1ExtraSepLayerHelper::moveNodeToNewMaterial(SoNode*node,SoMaterial* oldmat,SoMaterial* newmat)
{
	addNodeUnderMaterial(node,newmat);
	removeNodeUnderMaterial(node,oldmat);
}
