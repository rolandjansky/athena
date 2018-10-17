/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * CollectionSettingsButtonBase.h
 *
 *  Created on: Sep 2, 2014
 *      Author: Riccardo-Maria BIANCHI - rbianchi@cern.ch
 */

#ifndef VP1COLLECTIONSETTINGSBUTTONBASE_H_
#define VP1COLLECTIONSETTINGSBUTTONBASE_H_

#include "VP1Base/VP1MaterialButton.h"

class VP1CollectionSettingsButtonBase : public VP1MaterialButtonBase {

	Q_OBJECT

public:

	VP1CollectionSettingsButtonBase(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
	virtual ~VP1CollectionSettingsButtonBase();

	void setDimension(int dim);

	SoMaterial * getMaterial(VP1MaterialButton*) const;
	SoMaterial * fallBackMaterial() const;

//	SoMaterial * fallBackMaterial;

  QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
  void restoreFromState( const QByteArray& );

private:

  class Imp;
  Imp * m_d;

};

#endif /* VP1COLLECTIONSETTINGSBUTTONBASE_H_ */
