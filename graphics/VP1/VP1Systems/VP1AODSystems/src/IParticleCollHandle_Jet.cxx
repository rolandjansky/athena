/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandle_Jet           //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: May 2014                                 //
//                                                            //
////////////////////////////////////////////////////////////////

//Local
#include "IParticleCollHandle_Jet.h"
#include "IParticleHandle_Jet.h"
#include "JetCollectionSettingsButton.h"
#include "AODSysCommonData.h"

//xAOD
#include "xAODJet/JetContainer.h"

//VP1
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Utils/VP1SGAccessHelper.h"
#include "VP1Utils/VP1SGContentsHelper.h"

//Qt
#include <QtCore/QStringList>

// SoCoin
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoMaterial.h>
//#include "Inventor/nodes/SoDrawStyle.h"
//#include "Inventor/nodes/SoLightModel.h"


//____________________________________________________________________
QStringList IParticleCollHandle_Jet::availableCollections( IVP1System*sys )
{
	return VP1SGContentsHelper(sys).getKeys<xAOD::JetContainer>();
}

//____________________________________________________________________
class IParticleCollHandle_Jet::Imp {
public:
	IParticleCollHandle_Jet * theclass;
	int updateGUICounter;
	JetCollectionSettingsButton* collSettingsButton;

	// a list of jet handles
	QList<IParticleHandleBase*> jets;

	// settings
	double scale = 1.; // the default scale of all jets
	bool randomColours = false;

	//	SoMaterial*        defaultParametersMaterial; /// This will be given to the VP1MaterialButton in

	void possiblyUpdateGUI() {//Fixme: to IParticleCollHandleBase
		if (!((updateGUICounter++)%50)) {
			theclass->systemBase()->updateGUI();
		}
	}
};

//____________________________________________________________________
IParticleCollHandle_Jet::IParticleCollHandle_Jet(AODSysCommonData * cd,
		const QString& name, xAOD::Type::ObjectType type)
: IParticleCollHandleBase(cd,name,type), d(new Imp)
{
	d->theclass = this;
	d->updateGUICounter = 0;
	d->collSettingsButton=0;


	//The object names should not contain all sorts of funky chars (mat button style sheets wont work for instance):
	QString safetext(text());
	safetext.replace(' ','_');
	safetext.replace('[','_');
	safetext.replace(']','_');
	safetext.replace('/','_');
	safetext.replace('.','_');
	safetext.replace(',','_');
	safetext.replace('<','_');
	safetext.replace('>','_');
	safetext.replace('&','_');

	//  d->defaultParametersMaterial = new SoMaterial;
	//  d->defaultParametersMaterial->setName(("IParticleCollHandle_Jet"+safetext).toStdString().c_str());
	//  d->defaultParametersMaterial->ref();
	// d->matButton->setDefaultParameterMaterial(d->defaultParametersMaterial); FIXME
}

//____________________________________________________________________
IParticleCollHandle_Jet::~IParticleCollHandle_Jet()
{
	//	d->defaultParametersMaterial->unref();

	// clean the QList<handle>
	foreach(IParticleHandleBase* handle, d->jets) {
		delete handle;
	}

	// TODO: implemet this del
	//	if (d->bTaggingSwitch) d->bTaggingSwitch->unref();

	delete d;
}

//____________________________________________________________________
void IParticleCollHandle_Jet::init(VP1MaterialButtonBase*)
{
	// std::cout<<"IParticleCollHandle_Jet::init 1"<<std::endl;

	//	d->matButton = new JetCollectionSettingsButton; // TODO: update for jet
	//	d->matButton->setMaterialText(name());
	d->collSettingsButton = new JetCollectionSettingsButton;
	d->collSettingsButton->setMaterialText(name());

	// std::cout<<"Calling VP1StdCollection::init with d->matButton (JetCollectionSettingsButton)="<<d->matButton<<std::endl;
	VP1StdCollection::init(d->collSettingsButton);//this call is required. Passing in d->collSettingsButton means we have the more complex button.
	setupSettingsFromController(common()->controller());
	connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));

	// std::cout<<"IParticleCollHandle_Jet::init 2"<<std::endl;
	// std::cout<<"swi: "<<collSwitch()<<std::endl;
	// std::cout<<"sep: "<<collSep()<<std::endl;
	// std::cout<<"mat: "<<material()<<std::endl;

	//	collSwitch()->addChild(d->collSettingsButton->trackLightModel()); // TODO: update for jets
	//	collSwitch()->addChild(d->collSettingsButton->trackDrawStyle()); // TODO: update for jets
}

void IParticleCollHandle_Jet::setupSettingsFromControllerSpecific(AODSystemController*) {

	//cuts  
	connect(d->collSettingsButton,SIGNAL(cutAllowedPtChanged(const VP1Interval&)),this,SLOT(setCutAllowedPt(const VP1Interval&)));
	setCutAllowedPt(d->collSettingsButton->cutAllowedPt());

	connect(d->collSettingsButton,SIGNAL(cutAllowedEtaChanged(const VP1Interval&)),this,SLOT(setCutAllowedEta(const VP1Interval&)));
	setCutAllowedEta(d->collSettingsButton->cutAllowedEta());

	connect(d->collSettingsButton,SIGNAL(cutAllowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(setCutAllowedPhi(const QList<VP1Interval>&)));
	setCutAllowedPhi(d->collSettingsButton->cutAllowedPhi());

	//	// Parameters
	//	connect(d->collSettingsButton,SIGNAL(showParametersChanged(bool)),        this,SLOT(showParametersChanged(bool)));
	//	connect(d->collSettingsButton,SIGNAL(colourParametersByTypeChanged(bool)),this,SLOT(showParametersChanged(bool)));
	// Just reusing the same slot, since it doesn't

	// scale
	connect(d->collSettingsButton,SIGNAL(scaleChanged(const double&)),this,SLOT(setScale(const double&)));
	setScale( this->scale() );

	// random jet colors
	connect(d->collSettingsButton,SIGNAL(rerandomise()),this,SLOT(rerandomise()));
	connect(d->collSettingsButton,SIGNAL(randomJetColoursChanged(const bool&)),this,SLOT(setRandomJetColours(const bool&)));
	setRandomJetColours(d->collSettingsButton->randomJetColours());
}

//SoMaterial* IParticleCollHandle_Jet::defaultParameterMaterial() const {
//	return d->defaultParametersMaterial;
//}


const JetCollectionSettingsButton& IParticleCollHandle_Jet::collSettingsButton() const {
	if (!d->collSettingsButton){
		messageVerbose("Jet - No collSettingsButton set! Can't call init(), so crash is imminent...");
	}
	return *d->collSettingsButton;
}



//____________________________________________________________________
void IParticleCollHandle_Jet::setScale(const double& sca)
{

	messageVerbose("IParticleCollHandle_Jet::setScale() - scale: " + QString::number(sca));

	if (d->jets.isEmpty()) {
		messageVerbose("no jet handles defined! returning.");
		return;
	}

	if (d->scale == sca)
		return;

	d->scale = std::max(1*Gaudi::Units::mm/(100*Gaudi::Units::GeV),
			std::min(99*Gaudi::Units::m/(1*Gaudi::Units::MeV),
					//						d->collSettingsButton->lengthOf100GeV() * Gaudi::Units::m/(100.0*Gaudi::Units::GeV)));
					sca * Gaudi::Units::m/(100.0*Gaudi::Units::GeV)));

	if (!isLoaded())
		return;

	messageVerbose("Scale change: to "+str(d->scale/(Gaudi::Units::m/(100.0 * Gaudi::Units::GeV)))+" m/100GeV. Updating "+str(d->jets.count())+" jets");
	std::cout << "Scale change: d->scale/(Gaudi::Units::m/(100.0*Gaudi::Units::GeV)))" <<  "m/100GeV. Updating " << d->jets.count() << " jets" << std::endl;
	largeChangesBegin();
	foreach(IParticleHandleBase* partHandle, d->jets) {
		IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(partHandle);
		if (jet->has3DObjects()) {
			jet->setScale(d->scale);
			jet->updateHeight();
		}
	}
	largeChangesEnd();
}

//____________________________________________________________________
double IParticleCollHandle_Jet::scale() const
{
	return d->scale;
}


//____________________________________________________________________
bool IParticleCollHandle_Jet::isRandomColors() const
{
	VP1Msg::messageDebug("IParticleCollHandle_Jet::isRandomColors()");
	return d->randomColours;
}


//____________________________________________________________________
void IParticleCollHandle_Jet::setRandomJetColours(const bool& b)
{
	messageVerbose("IParticleCollHandle_Jet::setRandomJetColours() - b: "+QString::number(b));

	if (d->randomColours == b)
		return;

	d->randomColours = b;
	std::cout << "isRandom: " << d->randomColours << std::endl;;

	if (!isLoaded())
		return;

	largeChangesBegin();
	int ijet = 0;
	foreach(IParticleHandleBase* partHandle, d->jets) {
		++ijet;
		messageVerbose("considering jet: "+QString::number(ijet));
		IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(partHandle);
		if (jet->has3DObjects()) {
			int randomColors = isRandomColors();
			messageVerbose("setting random material on jet: "+QString::number(ijet)+" - random colors? "+QString::number(randomColors));
//			std::cout << "isRandomColors() --> " << isRandomColors() << std::endl;
			jet->updateMaterial( randomColors );
		}
	}
	largeChangesEnd();
}



//____________________________________________________________________
void IParticleCollHandle_Jet::rerandomise()
{
	messageVerbose("IParticleCollHandle_Jet::rerandomise()");

	if (!isLoaded()) {
		messageVerbose("IParticleCollHandle_Jet::rerandomise() - Returning.");
		return;
	}
	if (!isRandomColors()) {
		VP1Msg::message("'Random jet colours' is not checked. Nothing to do. Returning.");
		return;
	}

	largeChangesBegin();
	foreach(IParticleHandleBase* partHandle, d->jets) {
		IParticleHandle_Jet* jet = dynamic_cast<IParticleHandle_Jet*>(partHandle);
		if (jet->has3DObjects())
			jet->rerandomiseMaterial();
	}
	largeChangesEnd();
}







//____________________________________________________________________
bool IParticleCollHandle_Jet::load()
{
	messageVerbose("loading Jet collection");

	//Get collection:
	const xAOD::JetContainer * coll(0);
	if (!VP1SGAccessHelper(systemBase()).retrieve(coll, name())) {
		message("Error: Could not retrieve Jet collection with key="+name());
		return false;
	}

	// // Retrieve the xAOD particles:
	//  const xAOD::JetContainer* xaod = evtStore()->retrieve<const xAOD::JetContainer>( m_JetCollection );

	// Check that the auxiliary store association was made successfully:
	if( ! coll->hasStore() ) {
		message("No auxiliary store got associated to the Jet container with key: " +name());
	}

	// This is needed for now, until the issues around the DV code are
	// sorted out...
	const_cast< xAOD::JetContainer* >( coll )->setStore(
			( SG::IAuxStore* ) coll->getConstStore() );

	//Make appropriate Jet handles:
	// hintNumberOfTracksInEvent(coll->size());
	xAOD::JetContainer::const_iterator it, itEnd = coll->end();
	for ( it = coll->begin() ; it != itEnd; ++it) {
		d->possiblyUpdateGUI();
		if (!*it) {
			messageDebug("WARNING: Ignoring null Jet pointer.");
			continue;
		}
		/*
        if ((*it)->charge()==0.0) {
			messageDebug("WARNING: Ignoring Jet which claims to be neutral (charge()==0.0).");
			continue;
		}
		 */
		addHandle(new IParticleHandle_Jet(this,*it));
	}

	// get handles list and update locally
	d->jets = this->getHandlesList();

	return true;
}



//Cuts:
//____________________________________________________________________
bool IParticleCollHandle_Jet::cut(IParticleHandleBase* c) {
	messageVerbose("IParticleCollHandle_Jet::cut()");

	if (IParticleHandle_Jet* handle = dynamic_cast<IParticleHandle_Jet*>(c))
	{

		//------ ENERGY CUTS
		messageVerbose("jet's energy, allowedPt^2, PtAll: - " + QString::number(handle->energyForCuts()) + " - " + getCutAllowedPt().toString() + " - " + QString::number(int(getPtAllowall()))  );
		double jEnergy = handle->energyForCuts();
		double jEnergySqr = jEnergy * jEnergy; // in IParticleCollHandleBase the allowedPt is stored as squared, to avoid square-root operations
		if (!getPtAllowall() && !getCutAllowedPt().contains(jEnergySqr) ) {
			messageVerbose("Pt cut not passed");
			return false;
		}

		//------ ETA CUTS
		messageVerbose("jet's eta, etaCut, EtaAll: " + QString::number(handle->eta()) + " - " + getCutAllowedEta().toString() + " - " + QString::number(int(getEtaAllowall())) );
		if (!getEtaAllowall() && !getCutAllowedEta().contains(handle->eta())) {
			messageVerbose("Eta cut not passed");
			return false;
		}

		//------ PHI CUTS
		if (!getPhiAllowall() ) {
			double phi = handle->phi();
			bool ok(false);
			foreach (VP1Interval phirange, getCutAllowedPhi() ) {
				messageVerbose("jet's phi, phiCut, PhiAll: " + QString::number(phi)  + " - " + phirange.toString() + " - " + QString::number(int(getPhiAllowall())) );
				if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
					ok = true;
					break;
				}
			}
			if (!ok) {
				messageVerbose("Phi cut not passed");
				return false;
			}
		}

		//------ OTHER CUTS
		messageVerbose("Other cuts??? Calling base method...");
		if (!IParticleCollHandleBase::cut(handle))
			return false;

	} // if(dynamic_cast)
	else{
		messageVerbose("Not a IParticleCollHandle_Jet. Calling base method...");
		if (!IParticleCollHandleBase::cut(handle))
			return false;
	}

	return true; //default

}



void IParticleCollHandle_Jet::showParametersChanged(bool val) {
	messageVerbose("IParticleCollHandle_Jet::showParametersChanged to "+str(val));
	update3DObjectsOfAllHandles();
}
