/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1AODSYSTEMS_CALOCLUSTERCOLLSETTINGSBUTTON_H
#define VP1AODSYSTEMS_CALOCLUSTERCOLLSETTINGSBUTTON_H

#include "VP1Base/VP1CollectionSettingsButtonBase.h"
#include "VP1AODSystems/AODSystemController.h"

// FWD declarations
class IParticleCollHandle_CaloCluster;
//class SoDrawStyle;
//class SoLightModel;

class CaloClusterCollectionSettingsButton : public VP1CollectionSettingsButtonBase {

	Q_OBJECT

public:
  
	CaloClusterCollectionSettingsButton(QWidget * parent = 0, int dim = 25);//dim<=0 => Won't change sizepolicy
	virtual ~CaloClusterCollectionSettingsButton();
  
	void setCollHandle(IParticleCollHandle_CaloCluster* coll);
	void setDimension(int dim);

  
	// The below are necessary to fulfill the interface, but will just be passed onto the VP1MaterialButton owned by the CaloClusterCollectionSettingsButton form
	virtual bool setMaterial(SoMaterial*);
	virtual void copyValuesFromMaterial(SoMaterial*);
	virtual double lastAppliedTransparency() const ;
	virtual double lastAppliedShininess() const  ;
	virtual double lastAppliedBrightness() const;
  
//	// GUI for track lines...
//	double trackTubeRadius() const;//0 => don't show tubes for tracks
//	bool   hideActualTrackPath() const;
	
//	// GUI for parameters
//	bool showParameters() const;
//	bool colourParametersByType() const;
	double lengthOf10GeV();


	//Scale:
	//The bool indicates if it is logscale(true) or linear scale(false).
	//The double is a parameter, p, used to get the length as a function
	//of energy: Either p*|energy| or p*log(1+|energy|)
	QPair<bool,double> scale() const;


//	bool randomJetColours() const;
//	double maxR(); //!< Returns -1 if disabled in GUI, or value in m if enabled.

	// GUI for cuts
	VP1Interval cutAllowedPt() const;
	VP1Interval cutAllowedEta() const;
	QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
	bool isTransverseEnergy() const;

public slots:
	void setTransverseEnergy();


//	bool        cutOnlyVertexAssocTracks() const; //!< Return true if this collection should only should tracks associated to a vertex
//	//Cuts for number of hits per subsystem:
//	QList<unsigned> cutRequiredNHits() const;//Empty list: no requirements. Otherwise a list
//	// with a length of four (pixel,sct,trt,muon)
   
//	//Overall drawstyles, complexities and light models:
//	SoDrawStyle *  trackDrawStyle() const;
//	SoLightModel * trackLightModel() const;
  
//	// Parameters
//	SoMaterial* defaultParameterMaterial() const;
//	SoMaterial* parameterMaterial( xAOD::ParameterPosition) const;
    
	QByteArray saveState() const; //!< fill out with the state of the object (used for drag and drop etc)
	void restoreFromState( const QByteArray& );
  
signals:
//	void lastAppliedChanged(); // emitted when something related to material changes
	void cutAllowedPtChanged(const VP1Interval&);
	void cutAllowedEtaChanged(const VP1Interval&);
	void cutAllowedPhiChanged(const QList<VP1Interval>&);

	void scaleChanged(const QPair<bool,double>& scale);

	void energyTypeChanged();

//	void randomJetColoursChanged(const bool& ra);
//	void rerandomise();
//	void maxRChanged(const double& maxR);

//	void bTaggingEnabledChanged(const bool&);
//	void bTaggingTaggerChanged(const QString &);
//	void bTaggingCutChanged (const double&);
//	void bTaggingRenderingMaterialChanged(const bool&);
//	void bTaggingMaterialChanged();
//	void bTaggingRenderingSkinChanged(bool);

	 
public slots:
	void showEditMaterialDialog();
	void setMaterialText(const QString&);

protected slots:
	void updateButton();

protected:
	// void initEditWindow();//!< Make sure that edit window is constructed
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void dropEvent(QDropEvent *event);
	void dragEnterEvent(QDragEnterEvent *event);
private:

	class Imp;
	Imp * d;
    
private slots:
//	void updateTrackDrawStyle();
//	void updateTrackLightModel(bool);
//	void updateTrackTubeRadius();
//	void updateHideActualTrackPath(bool);
//	void possibleChange_cutRequiredNHits();
//	void possibleChange_cutOnlyVertexAssocTracks();
//	void possibleChange_showParameters();
//	void possibleChange_parameterColours();

	void possibleChange_cutAllowedPt();
	void possibleChange_cutAllowedEta();
	void possibleChange_cutAllowedPhi();

	void possibleChange_scale();
	void possibleChange_showVolumeOutLines();
	void possibleChange_useTransverseEnergies();

//	void possibleChange_randomJetColours();
//	void possibleChange_maxR();

//	void possibleChange_bTaggingEnabled(bool);
//	void possibleChange_bTaggingTagger();
//	void possibleChange_bTaggingCut();
//	void possibleChange_bTaggingRenderingMaterial(bool);
//	////void possibleChange_bTaggingMaterial(); // not needed...
//	void possibleChange_bTaggingRenderingSkin(bool ok);


//	void enableMaxR(bool);
//	void enableRandomColours(bool);
//
//	void emitRerandomise();
//	void emitMaxR();
    
};

#endif


