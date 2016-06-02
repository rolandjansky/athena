/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class MissingEtHandle                     //
//                                                            //
//  Description: Handles for MET                              //
//                                                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef MissingEtHandle_H
#define MissingEtHandle_H

#include "AODHandleBase.h"

//#include "xAODTracking/TrackParticle.h"
#include "xAODMissingET/MissingET.h"

//#include "GeoPrimitives/GeoPrimitives.h"

class MissingEtHandle : public AODHandleBase {
public:

	MissingEtHandle(AODCollHandleBase*, const xAOD::MissingET * );
	virtual ~MissingEtHandle();

	virtual bool has3DObjects();
	virtual void clear3DObjects();
	virtual SoNode* nodes();
	virtual SoMaterial * determineMaterial();

	// Setters
	void setScale( const double& sc);

	void updateHeight();

	// set/update random material
//	void rerandomiseMaterial();


	virtual QStringList clicked() const;
	//virtual Amg::Vector3D momentum() const;
//	const xAOD::IParticle& iParticle() const;

//	virtual double charge() const ;//!< Returns unknown() in case of trouble.
	double phi() const;
	double met() const;
//	double energy() const;
//	double transverseEnergy() const;
//	double energyForCuts() const;

	virtual QString type() const { return QString("MissingEt"); } //!< return very short word with type (maybe link with collection type?)

	/// This returns the information shown about the object in the object browser
	QString shortInfo() const;

	void fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) ;
	//void updateMaterial(bool);


protected:  

private:

	class Imp;
	Imp * d;
	void ensureInitSubSysHitInfo() const;

};

#endif
