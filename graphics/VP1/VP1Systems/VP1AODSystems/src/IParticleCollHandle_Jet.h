/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//                                                            //
//  Header file for class IParticleCollHandle_Jet             //
//                                                            //
//  Description: Collection handle for Jets                   //
//                                                            //
//  June 2014 - Riccardo.Maria.Bianchi@cern.ch                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef IPARTICLECOLLHANDLE_JET_H
#define IPARTICLECOLLHANDLE_JET_H

#include "IParticleCollHandleBase.h"
#include "xAODBase/ObjectType.h"

//class SoMaterial;
class JetCollectionSettingsButton;


class IParticleCollHandle_Jet : public IParticleCollHandleBase {

	Q_OBJECT

public:

	static QStringList availableCollections(IVP1System*);//For the collection widget.

	IParticleCollHandle_Jet( AODSysCommonData *,
			const QString& name, xAOD::Type::ObjectType type );
	virtual ~IParticleCollHandle_Jet();

	virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
	virtual void setupSettingsFromControllerSpecific(AODSystemController*);
  virtual void dumpToJSON( std::ofstream& ) const ;

	const JetCollectionSettingsButton& collSettingsButton() const;
	bool isRandomColors() const;
	//  bool isMaxR() const;

	// This is created in this class, but passed to the JetCollectionButton for control etc. It is used in the Handle.
	//  SoMaterial* defaultParameterMaterial() const;
    
	/*
	 * Save and restore settings
	 * (implementation of the virtual functions in the base class)
	 */
	QByteArray persistifiableState() const; // save state
	void setState(const QByteArray&); // restore state

protected:	
	virtual bool load();
	virtual bool cut(AODHandleBase*);
	virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 1.0f, 0.5f); } // light yellow
	//virtual QString matButtonToolTip() const { return "Edit the material, properties and cuts of "+text(); }

private slots:
	void showParametersChanged(bool);
	void setScale(const double& s);
	double scale() const;
	void setRandomJetColours(const bool&);
	void rerandomise();
	void setMaxR(const double& max);
	double maxR() const;
	void setBTaggingEnabled(const bool&);
	void setBTaggingSkin(const QString &filename);
	void setBTaggingMaterial(SoMaterial* mat = NULL);
	void setBTaggingTagger(const QString & );
	void setBTaggingCut(const double &);
	void setBTaggingMaterialChanged(const bool&);
	void setBTaggingSkinChanged(const bool& ok);
	void resetCachedValuesCuts();

private:

	class Imp;
	Imp * d;

	void updateBTaggingAllJets();
	void updateBTaggingSwitchAllJets();

};

#endif
