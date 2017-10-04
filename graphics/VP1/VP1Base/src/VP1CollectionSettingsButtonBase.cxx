/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * VP1CollectionSettingsButtonBase.cxx
 *
 *  Created on: Sep 2, 2014
 *      Author: Riccardo-Maria BIANCHI - rbianchi
 */


#include "VP1Base/VP1CollectionSettingsButtonBase.h"
#include "Inventor/C/errors/debugerror.h"
#include "Inventor/nodes/SoMaterial.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
//____________________________________________________________________
class VP1CollectionSettingsButtonBase::Imp {
public:

//	Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0){}
	Imp():theclass(0), dim(0), fallBackMaterial(0) {}

	// the main class
	VP1CollectionSettingsButtonBase * theclass;

	int dim;
	SoMaterial * fallBackMaterial;
};


//____________________________________________________________________
VP1CollectionSettingsButtonBase::VP1CollectionSettingsButtonBase(QWidget * parent,int dim)
: VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
	m_d->dim = dim;
	m_d->fallBackMaterial = 0;
	m_d->theclass = this;
}


//____________________________________________________________________
VP1CollectionSettingsButtonBase::~VP1CollectionSettingsButtonBase()
{
  if (m_d->fallBackMaterial)
    m_d->fallBackMaterial->unref();
  delete m_d;
}

//____________________________________________________________________
SoMaterial * VP1CollectionSettingsButtonBase::fallBackMaterial() const
{
  if (!m_d->fallBackMaterial) {
    m_d->fallBackMaterial = new SoMaterial;
    m_d->fallBackMaterial->ref();
  }
  return m_d->fallBackMaterial;
}

//____________________________________________________________________
SoMaterial * VP1CollectionSettingsButtonBase::getMaterial(VP1MaterialButton*mb) const
{
  if (!mb) {
    message("ERROR: Null material button pointer");
    return fallBackMaterial();
  }
  QList<SoMaterial*> mats = mb->handledMaterials();
  if (mats.count()!=1) {
    message("ERROR: No material in button "+mb->objectName());
    return fallBackMaterial();
  }
  return mats.at(0);


}

QByteArray VP1CollectionSettingsButtonBase::saveState() const{
  VP1Serialise serialise(1/*version*/);
  serialise.save(this);  
  return serialise.result();
}

void VP1CollectionSettingsButtonBase::restoreFromState( const QByteArray& ba){
  VP1Deserialise state(ba,systemBase());
  if (state.version()<0||state.version()>1)
    return;//Ignore silently
  state.restore(this);
}




