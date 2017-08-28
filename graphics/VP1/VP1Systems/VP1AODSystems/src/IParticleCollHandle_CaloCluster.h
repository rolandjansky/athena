/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//                                                            //
//  Header file for class IParticleCollHandle_CaloCluster     //
//                                                            //
//  Description: Collection handle for calorimeter clusters   //
//                                                            //
//  June 2015 - Riccardo.Maria.Bianchi@cern.ch                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1AODSYSTEMS_IParticleCollHandle_CaloCluster_H
#define VP1AODSYSTEMS_IParticleCollHandle_CaloCluster_H

// local
#include "IParticleCollHandleBase.h"

// Qt
#include <QPair>

// xAOD
#include "xAODBase/ObjectType.h"




// FWD declarations
//class SoMaterial;
class CaloClusterCollectionSettingsButton;




class IParticleCollHandle_CaloCluster : public IParticleCollHandleBase {

	Q_OBJECT

public:

	static QStringList availableCollections(IVP1System*);//For the collection widget.

	IParticleCollHandle_CaloCluster( AODSysCommonData *,
			const QString& name, xAOD::Type::ObjectType type );
	virtual ~IParticleCollHandle_CaloCluster();

	virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
	virtual void setupSettingsFromControllerSpecific(AODSystemController*);
  virtual void dumpToJSON( std::ofstream& ) const ;

	const CaloClusterCollectionSettingsButton& collSettingsButton() const;
//	bool isRandomColors() const;
	bool isConsiderTransverseEnergy() const;
//	void setTransverseEnergyForCuts();
	double energyToLength(const double& energy) const;
	bool showOutlines() const;

	void setScale();

	// highest cluster energy
	double highestVisibleClusterEnergy() const;
	double calculateHighestVisibleClusterEnergy();
	void recheckHighestVisibleClusterEnergy();

	//  bool isMaxR() const;

	// This is created in this class, but passed to the JetCollectionButton for control etc. It is used in the Handle.
	//  SoMaterial* defaultParameterMaterial() const;
    
	/*
	 * Save and restore settings
	 * (implementation of the virtual functions in the base class)
	 */
	QByteArray persistifiableState() const; // save state
	void setState(const QByteArray&); // restore state

	static double calo_start_r;
	static double calo_start_z;
	static double calo_crack_eta;

protected:	
	virtual bool load();
	virtual bool cut(AODHandleBase*);
	virtual QColor defaultColor() const { return QColor::fromRgbF(1.0f, 1.0f, 0.5f); }

signals:
	  void highestVisibleClusterEnergyChanged();

private slots:
	void showParametersChanged(bool);
	void setScale(const QPair<bool,double>&);
	QPair<bool,double> scale() const;
	void rebuildAllObjects();
//	void setScale(const QPair<bool,double>& s);
//	void setRandomJetColours(const bool&);
//	void rerandomise();
//	void setMaxR(const double& max);
//	double maxR() const;
//	void setBTaggingEnabled(const bool&);
//	void setBTaggingSkin(const QString &filename);
//	void setBTaggingMaterial(SoMaterial* mat = NULL);
//	void setBTaggingTagger(const QString & );
//	void setBTaggingCut(const double &);
//	void setBTaggingMaterialChanged(const bool&);
//	void setBTaggingSkinChanged(const bool& ok);
	void setShowVolumeOutLines(bool);
	void resetCachedValuesCuts();


private:

	class Imp;
	Imp * d;

//	void updateBTaggingAllJets();
//	void updateBTaggingSwitchAllJets();

};

#endif
