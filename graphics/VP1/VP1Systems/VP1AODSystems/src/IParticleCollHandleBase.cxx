/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class IParticleCollHandleBase               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

//Local includes
#include "IParticleCollHandleBase.h"
#include "VP1AODSystems/VP1AODSystem.h"
#include "IParticleHandleBase.h"
#include "VP1AODSystems/AODSystemController.h"
#include "AODSysCommonData.h"

//VP1 base includes
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Utils/VP1LinAlgUtils.h"
#include "VP1Base/IVP13DSystem.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

//SoCoin
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSwitch.h>
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"

//Athnea
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

//Qt
#include <QtGui/QComboBox>
#include <QTreeWidgetItem>
#include <qdatetime.h>
#include <vector>
#include <QString>

//____________________________________________________________________
class IParticleCollHandleBase::Imp {
public:
	IParticleCollHandleBase * theclass;
	QString name;

	//Vector of iParticle handles:
	std::vector<IParticleHandleBase*> handles;
	// QList of IParticle handles
	QList<IParticleHandleBase*> handlesList;

	//For iteration:
	std::vector<IParticleHandleBase*>::iterator itHandles;
	std::vector<IParticleHandleBase*>::iterator itHandlesEnd;

	//Extra widgets:
	QTreeWidgetItem* objBrowseTree;

	// N.B. Material button defined in children.
};



//____________________________________________________________________
IParticleCollHandleBase::IParticleCollHandleBase( AODSysCommonData * cd,
		const QString& name,
		xAOD::Type::ObjectType type)
: VP1StdCollection(cd->system(),"IParticleCollHandle_FIXME_"+name), d(new Imp), // Need to add back ObjectType once simple way to create string is added to xAODBase
  m_nshownhandles(0),m_type(type),
  m_commonData(cd),
  m_sephelper(0),
  m_cut_allowedPtSq(VP1Interval()),
  m_cut_allowedEta(VP1Interval()),
  m_cut_allowedPhi(QList<VP1Interval>()),
  m_cut_pt_allowall(false),
  m_cut_eta_allowall(false),
  m_cut_phi_allowall(false),
  m_cut_etaptphi_allwillfail(true)
{
	d->theclass = this;
	d->name = name;
	d->objBrowseTree = 0;
}

// //____________________________________________________________________
// void IParticleCollHandleBase::init(VP1MaterialButtonBase*)
// {
//     // d->matButton = new TrackCollectionSettingsButton;
//     // d->matButton->setText(text());
//     // VP1StdCollection::init(d->matButton);//this call is required. Passing in TrackCollectionSettingsButton means we have the more complex button. 
//   VP1StdCollection::init();//FIXME
//   setupSettingsFromController(common()->controller());
//   connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(collVisibilityChanged(bool)));
// 	
//   // collSwitch()->addChild(d->matButton->trackDrawStyle());
// }

//____________________________________________________________________
IParticleCollHandleBase::~IParticleCollHandleBase()
{
	messageVerbose("destructor start");

	// clean the vector<handle>
	cleanupPtrContainer(d->handles);

	// clean the QList<handle>
	foreach(IParticleHandleBase* handle, d->handlesList) {
		delete handle;
	}

	// delete the Imp instance
	delete d;

	if (m_sephelper) {
		SoSeparator * sep = m_sephelper->topSeparator();
		sep->ref();
		delete m_sephelper;
		sep->unref();
	}

	messageVerbose("destructor end");
}

//____________________________________________________________________
void IParticleCollHandleBase::setupSettingsFromController(AODSystemController* controller)
{
	messageVerbose("setupSettingsFromController start");
	if (!controller) {
		message("Not properly initialised: controller pointer is zero.");
		return;
	}

	largeChangesBegin();
	// connect(common()->system(),SIGNAL(newHandleSelected( const IParticleHandleBase&)),this,SLOT(handleSelectionChanged()));
	setupSettingsFromControllerSpecific(controller);

	largeChangesEnd();
	messageVerbose("setupSettingsFromController end");
}



//____________________________________________________________________
QList<IParticleHandleBase*> IParticleCollHandleBase::getHandlesList() const
{
	messageVerbose("IParticleCollHandleBase::getHandlesList()");
	return d->handlesList;
}

//____________________________________________________________________
QString IParticleCollHandleBase::name() const
{
	return d->name;
}

//____________________________________________________________________
bool IParticleCollHandleBase::cut(IParticleHandleBase* handle)
{
	messageVerbose("IParticleCollHandleBase::cut()");

	if (m_cut_etaptphi_allwillfail)
		return false;

	messageVerbose("m_cut_pt_allowall: " + QString::number(m_cut_pt_allowall) + " - m_cut_eta_allowall: " + QString::number(m_cut_eta_allowall) + " - m_cut_phi_allowall: " + QString::number(m_cut_phi_allowall) );

	if (!m_cut_pt_allowall||!m_cut_eta_allowall||!m_cut_phi_allowall)
	{
		messageVerbose("evaluating cut...");

		//    Trk::GlobalMomentum mom(handle->momentum());
		Amg::Vector3D mom(handle->momentum());
		// std::cout<<"mom mag2: "<<mom.mag2()<<std::endl;

		// convention is that if interval is real and negative, then P cut rather than pT
		bool isPCut = (!m_cut_allowedPtSq.noLowerBound() && m_cut_allowedPtSq.lower()<=0)
    				  || (!m_cut_allowedPtSq.noUpperBound() && m_cut_allowedPtSq.upper()<=0);

		if (mom.mag2()!=0.0) {

			if (!m_cut_pt_allowall&&!isPCut && !m_cut_allowedPtSq.contains(mom.perp2()))
				return false;
			if (!m_cut_pt_allowall&& isPCut && !m_cut_allowedPtSq.contains(-mom.mag2()))
				return false;

			//      if (!m_cut_eta_allowall&&!m_cut_allowedEta.contains(mom.pseudoRapidity())){
			if (!m_cut_eta_allowall&&!m_cut_allowedEta.contains(mom.eta())){ // migration to Amg (Eigen)
				return false;
			}
			if (!m_cut_phi_allowall) {
				double phi = VP1LinAlgUtils::phiFromXY(mom.x(), mom.y() );
				messageVerbose("value oh handle's phi: " + QString::number(phi));
				bool ok(false);
				foreach (VP1Interval phirange, m_cut_allowedPhi) {
					if (phirange.contains(phi)||phirange.contains(phi+2*M_PI)||phirange.contains(phi-2*M_PI)) {
						ok = true;
						break;
					}
				}
				if (!ok)
					return false;
			}
		}
		messageVerbose("mom.mag2() == 0.0!!!");

	}

	messageVerbose("cut() - Returning true!");
	return true;
}

//____________________________________________________________________
void IParticleCollHandleBase::recheckCutStatus(IParticleHandleBase* handle)
{
	messageVerbose("IParticleCollHandleBase::recheckCutStatus() & visible="+str(visible()));

	handle->setVisible( visible() && cut(handle));
}

//____________________________________________________________________
void IParticleCollHandleBase::recheckCutStatusOfAllVisibleHandles()
{
	messageVerbose("IParticleCollHandleBase::recheckCutStatusOfAllVisibleHandles");

	if (!isLoaded())
		return;

	//This method is called when a cut is tightened - thus we better start by deselectAll to avoid weird highlighting issues.
	common()->system()->deselectAll();

	largeChangesBegin();
	std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
	for (;it!=itE;++it) {
		if ((*it)->visible())
			recheckCutStatus(*it);
	}
	updateObjectBrowserVisibilityCounts();
	largeChangesEnd();

	message("recheckCutStatusOfAllVisibleHandles:  "+str(nShownHandles())+"/"+str(d->handles.size())+" pass cuts");
}

//____________________________________________________________________
void IParticleCollHandleBase::recheckCutStatusOfAllNotVisibleHandles()
{
	messageVerbose("IParticleCollHandleBase::recheckCutStatusOfAllNotVisibleHandles");

	if (!isLoaded())
		return;

	largeChangesBegin();
	std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
	for (;it!=itE;++it) {
		if (!(*it)->visible())
			recheckCutStatus(*it);
	}
	updateObjectBrowserVisibilityCounts();
	largeChangesEnd();

	message("recheckCutStatusOfAllNotVisibleHandles:  "+str(nShownHandles())+"/"+str(d->handles.size())+" pass cuts");
}

//____________________________________________________________________
void IParticleCollHandleBase::recheckCutStatusOfAllHandles()
{
	messageVerbose("IParticleCollHandleBase::recheckCutStatusOfAllHandles()");
	if (!isLoaded())
		return;
	largeChangesBegin();
	std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
	for (;it!=itE;++it)
		recheckCutStatus(*it);
	updateObjectBrowserVisibilityCounts();
	largeChangesEnd();

	message("recheckCutStatusOfAllHandles:  "+str(nShownHandles())+"/"+str(d->handles.size())+" pass cuts");
}

//____________________________________________________________________
void IParticleCollHandleBase::update3DObjectsOfAllHandles()
{
	if (!isLoaded())
		return;
	if (verbose())
		messageVerbose("update3DObjectsOfAllHandles start");
	largeChangesBegin();
	std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());

	for (;it!=itE;++it)
		(*it)->update3DObjects();

	largeChangesEnd();
	messageVerbose("update3DObjectsOfAllHandles end");
}

//____________________________________________________________________
void IParticleCollHandleBase::updateMaterialOfAllHandles()
{
	if (!isLoaded())
		return;
	messageVerbose("updateMaterialOfAllHandles start");
	largeChangesBegin();
	std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
	for (;it!=itE;++it)
		(*it)->updateMaterial();
	largeChangesEnd();
	messageVerbose("updateMaterialOfAllHandles end");
}


//____________________________________________________________________
void IParticleCollHandleBase::hintNumberOfHandlesInEvent(unsigned n)
{
	d->handles.reserve(n);
}

//____________________________________________________________________
void IParticleCollHandleBase::addHandle(IParticleHandleBase* handle)
{
	d->handles.push_back(handle); // for the vector<handle>
	d->handlesList << handle; // for the QList<handle>
}

// //____________________________________________________________________
// void IParticleCollHandleBase::setLabels( AODSystemController::TrackLabelModes labels )
// {
//   // messageVerbose("setLabels called");
//   // if (m_labels==labels)
//   //   return;
//   // messageVerbose("setLabels ==> Changed");
//   // m_labels = labels;
//   // update3DObjectsOfAllHandles();
// }
// 
// //____________________________________________________________________
// void IParticleCollHandleBase::setLabelTrkOffset( float offset)
// {
//   // messageVerbose("setLabelTrkOffset called");
//   // if (m_labelsTrkOffset==offset)
//   //   return;
//   // messageVerbose("setLabelTrkOffset ==> Changed to "+QString::number(offset));
//   // m_labelsTrkOffset = offset;
//   // update3DObjectsOfAllHandles();
// }
// 
// //____________________________________________________________________
// void IParticleCollHandleBase::setLabelPosOffsets( QList<int> offsets)
// {
//   // messageVerbose("setLabelPosOffsets called");
//   // if (m_labelsPosOffsets==offsets)
//   //   return;
//   // messageVerbose("setLabelPosOffsets ==> Changed");
//   // m_labelsPosOffsets = offsets;
//   // update3DObjectsOfAllHandles();
// }

// //____________________________________________________________________
// void IParticleCollHandleBase::setColourBy( IParticleCollHandleBase::COLOURBY cb )
// {
//   // messageVerbose("setColourBy called");
//   // if (m_colourby==cb)
//   //   return;
//   // messageVerbose("setColourBy ==> Changed");
//   // m_colourby=cb;
//   // 
//   // //Update gui combobox:
//   // QString targetText;
//   // switch(cb) {
//   // case COLOUR_BYPID:
//   //   targetText = Imp::comboBoxEntry_ColourByPID();
//   //   break;
//   // case COLOUR_RANDOM:
//   //   targetText = Imp::comboBoxEntry_ColourByRandom();
//   //   break;
//   // case COLOUR_MOMENTUM:
//   //   targetText = Imp::comboBoxEntry_ColourByMomentum();
//   //   break;
//   // case COLOUR_CHARGE:
//   //   targetText = Imp::comboBoxEntry_ColourByCharge();
//   //   break;
//   // case COLOUR_DISTANCE:
//   //   targetText = Imp::comboBoxEntry_ColourByDistanceFromSelectedTrack();
//   //   break;
//   // case COLOUR_VERTEX:
//   //   targetText = Imp::comboBoxEntry_ColourByVertex();
//   //   break;
//   // default:
//   // case COLOUR_PERCOLLECTION:
//   //   targetText = Imp::comboBoxEntry_ColourByCollection();
//   //   break;
//   // }
//   // if (targetText!=d->comboBox_colourby->currentText()) {
//   //   int i = d->comboBox_colourby->findText(targetText);
//   //   if (i>=0&&i<d->comboBox_colourby->count()) {
//   //     bool save = d->comboBox_colourby->blockSignals(true);
//   //     d->comboBox_colourby->setCurrentIndex(i);
//   //     d->comboBox_colourby->blockSignals(save);
//   //   } else {
//   //     message("ERROR: Problems finding correct text in combo box");
//   //   }
//   // }
//   // 
//   // //Actual material updates:
//   // largeChangesBegin();
//   // m_commonData->system()->deselectAll();//Todo: Reselect the selected track handles
//   //                                       //afterwards (collhandles should know selected handles)
//   // updateMaterialOfAllHandles();
//   // largeChangesEnd();
// 
// }
// 
// //____________________________________________________________________
// void IParticleCollHandleBase::rerandomiseRandomTrackColours()
// {
//   // if (!isLoaded())
//   //   return;
//   // messageVerbose("rerandomiseRandomTrackColours start");
//   // largeChangesBegin();
//   // std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
//   // for (;it!=itE;++it)
//   //   (*it)->rerandomiseRandomMaterial();
//   // largeChangesEnd();
//   // messageVerbose("rerandomiseRandomTrackColours end");
// }
// 
// //____________________________________________________________________
// void IParticleCollHandleBase::handleSelectionChanged()
// {
//   // if (!isLoaded() || colourBy()!= COLOUR_DISTANCE)
//   //   return;
//   // messageVerbose("handleSelectionChanged start");
//   // largeChangesBegin();
//   // std::vector<IParticleHandleBase*>::iterator it(d->handles.begin()),itE(d->handles.end());
//   // for (;it!=itE;++it)
//   //   (*it)->updateMaterial();
//   // largeChangesEnd();
//   // messageVerbose("handleSelectionChanged end");
// }

//____________________________________________________________________
void IParticleCollHandleBase::handleIterationBegin()
{
	d->itHandles = d->handles.begin();
	d->itHandlesEnd = d->handles.end();
}


//____________________________________________________________________
IParticleHandleBase* IParticleCollHandleBase::getNextHandle() {
	if (d->itHandles==d->itHandlesEnd)
		return 0;
	else
		return *(d->itHandles++);
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedPt(const VP1Interval& allowedpt)
{
	messageVerbose("IParticleCollHandleBase::setCutAllowedPt()");

	if (!allowedpt.isSane())
		return;

	//why do we need to do this? Why does it matter if it's negative?
	//double minptsq = allowedpt.lower() <= 0.0 ? -std::numeric_limits<double>::infinity() : allowedpt.lower()*allowedpt.lower();
	//VP1Interval allowedptsq(minptsq,allowedpt.upper()*allowedpt.upper());

	//Modified code from EJWM
	double signMin = allowedpt.lower()<0.0 ? -1.0 : 1.0;
	double signMax = allowedpt.upper()<0.0 ? -1.0 : 1.0;
	double minptsq = allowedpt.lower() == -std::numeric_limits<double>::infinity() ? -std::numeric_limits<double>::infinity() : signMin*(allowedpt.lower()*allowedpt.lower());
	double maxptsq = allowedpt.upper() == std::numeric_limits<double>::infinity() ? std::numeric_limits<double>::infinity()   : signMax*(allowedpt.upper()*allowedpt.upper());

	VP1Interval allowedptsq(minptsq,maxptsq);
	// std::cout<<"allowedptsq:isAllR"<<allowedptsq.isAllR()<<std::endl;
	// std::cout<<"allowedptsq.isEmpty()"<<allowedptsq.isEmpty()<<std::endl;


	if (m_cut_allowedPtSq==allowedptsq)
		return;
	m_cut_pt_allowall = allowedptsq.isAllR();
	m_cut_etaptphi_allwillfail = allowedptsq.isEmpty() || m_cut_allowedEta.isEmpty() || m_cut_allowedPhi.isEmpty();

	if (!m_cut_allowedPtSq.contains(allowedptsq)&&!allowedptsq.contains(m_cut_allowedPtSq)) {
		m_cut_allowedPtSq = allowedptsq;
		recheckCutStatusOfAllHandles();
		return;
	}
	bool relaxcut = allowedptsq.contains(m_cut_allowedPtSq);
	m_cut_allowedPtSq = allowedptsq;
	if (relaxcut)
		recheckCutStatusOfAllNotVisibleHandles();
	else
		recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedEta(const VP1Interval& allowedEta)
{
	messageVerbose("signal received in setCutAllowedEta ("+allowedEta.toString()+")");
	if (!allowedEta.isSane())
		return;

	if (m_cut_allowedEta==allowedEta)
		return;
	m_cut_eta_allowall = allowedEta.isAllR();
	m_cut_etaptphi_allwillfail = allowedEta.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedPhi.isEmpty();

	if (!m_cut_allowedEta.contains(allowedEta)&&!allowedEta.contains(m_cut_allowedEta)) {
		m_cut_allowedEta = allowedEta;
		recheckCutStatusOfAllHandles();
		return;
	}
	bool relaxcut = allowedEta.contains(m_cut_allowedEta);
	m_cut_allowedEta = allowedEta;
	if (relaxcut)
		recheckCutStatusOfAllNotVisibleHandles();
	else
		recheckCutStatusOfAllVisibleHandles();
}

//____________________________________________________________________
void IParticleCollHandleBase::setCutAllowedPhi(const QList<VP1Interval>& allowedPhi)
{
	messageVerbose("IParticleCollHandleBase::setCutAllowedPhi() - signal received in setCutAllowedPhi.");

	if (m_cut_allowedPhi==allowedPhi)
		return;
	m_cut_allowedPhi = allowedPhi;

	m_cut_phi_allowall = m_cut_allowedPhi.count()==1 && m_cut_allowedPhi.at(0).isAllR();
	m_cut_etaptphi_allwillfail = m_cut_allowedPhi.isEmpty() || m_cut_allowedPtSq.isEmpty() || m_cut_allowedEta.isEmpty();

	if (m_cut_phi_allowall) {
		recheckCutStatusOfAllNotVisibleHandles();
		return;
	}
	if (m_cut_etaptphi_allwillfail) {
		recheckCutStatusOfAllVisibleHandles();
		return;
	}
	recheckCutStatusOfAllHandles();
	return;
}


//____________________________________________________________________
qint32 IParticleCollHandleBase::provideCollTypeID() const
{
	// return TrackType::typeToInt(m_type);
	return 0;
}

QString IParticleCollHandleBase::provideSection() const {
	QString section;
	switch (m_type) {
	case xAOD::Type::TrackParticle:
		section = "TrackParticle";
		break;
	case xAOD::Type::Jet:
		section = "Jet";
		break;
	default:
		section = "Not yet supported for type=";
		section+=QString::number(m_type);
	}
	return section;
}

QString IParticleCollHandleBase::provideSectionToolTip() const {
	return QString("TODO!");
}

//____________________________________________________________________
void IParticleCollHandleBase::collVisibilityChanged(bool vis)
{
	if (verbose())
		messageVerbose("IParticleCollHandleBase::collVisibilityChanged => "+str(vis));

	if (!m_sephelper)
		m_sephelper = new VP1ExtraSepLayerHelper(collSep());

	if (!d->objBrowseTree)
		fillObjectBrowser();

	if (vis){
		recheckCutStatusOfAllNotVisibleHandles();//Fixme -> ofallhandles? All must be not visible anyway...
		if (d->objBrowseTree) d->objBrowseTree->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
	}else{
		recheckCutStatusOfAllVisibleHandles();
		//    QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
		//    if (d->objBrowseTree && trkObjBrowser) {
		//      trkObjBrowser->takeTopLevelItem(trkObjBrowser->indexOfTopLevelItem(d->objBrowseTree));
		//      delete d->objBrowseTree; d->objBrowseTree=0;
		//    }
		// FIXME - need to loop through handles setting pointers to deleted QTreeWidgetItems
		if (d->objBrowseTree) d->objBrowseTree->setFlags(0); // not selectable, not enabled
	}
} 

void IParticleCollHandleBase::updateObjectBrowserVisibilityCounts() {
	messageVerbose("IParticleCollHandleBase::updateObjectBrowserVisibilityCounts called for "+name());
	QTreeWidget* trkObjBrowser = common()->controller()->objBrowser();
	if (!trkObjBrowser || !d->objBrowseTree) {
		messageVerbose("IParticleCollHandleBase::updateObjectBrowserVisibilityCounts: no common()->controller()->objBrowser() and/or d->objBrowseTree. Aborting");
		messageVerbose("trkObjBrowser: "+str(trkObjBrowser)+"\t d->objBrowseTree: "+str(d->objBrowseTree));
		return;
	}
	QString text(QString(": (")+QString::number(nShownHandles())+QString("/")+QString::number(d->handles.size())+QString(") visible"));
	d->objBrowseTree->setText(1, text);
}

void IParticleCollHandleBase::fillObjectBrowser()
{
	QTime t;
	t.start();
	messageVerbose("IParticleCollHandleBase::fillObjectBrowser called for "+name());

	QTreeWidget* trkObjBrowser = common()->controller()->objBrowser();
	if (!trkObjBrowser) {
		messageVerbose("IParticleCollHandleBase::fillObjectBrowser: no common()->controller()->objBrowser(). Aborting");
		return;
	}
	// if (!nShownHandles()) {
	//   messageVerbose("IParticleCollHandleBase::fillObjectBrowser: no shown handles, so leaving.");
	//   return; // don't bother with hidden collection
	// }

	trkObjBrowser->setUpdatesEnabled(false);

	bool firstTime=false;
	if (!d->objBrowseTree) {
		d->objBrowseTree = new QTreeWidgetItem(0);
		firstTime=true;
		messageVerbose("IParticleCollHandleBase::fillObjectBrowser: First time so creating QTreeWidgetItem.");
	} else {
		int index = trkObjBrowser->indexOfTopLevelItem(d->objBrowseTree);
		if (index==-1 ) {
			messageVerbose("Missing from WidgetTree! Will continue but something must be wrong");
		}
	}

	messageVerbose("IParticleCollHandleBase::fillObjectBrowser about to start looping over handles at "+QString::number(t.elapsed())+" ms");

	QList<QTreeWidgetItem *> list;
	handleIterationBegin();
	IParticleHandleBase* trkHandle=0;
	unsigned int i=0;
	unsigned int numVisible=0;
	while ((trkHandle=getNextHandle()))
	{
		if (firstTime){
			trkHandle->fillObjectBrowser(list);
		} else {
			trkHandle->updateObjectBrowser();
		}

		// messageVerbose("IParticleCollHandleBase::fillObjectBrowser for handle completed in "+QString::number(t.elapsed()));

		if (trkHandle->visible() ) numVisible++;
		i++;
	}

	QString text(QString(": (")+QString::number(numVisible)+QString("/")+QString::number(i)+QString(") visible"));

	d->objBrowseTree->setText(0, name());
	d->objBrowseTree->setText(1, text);
	d->objBrowseTree->addChildren(list);
	trkObjBrowser->addTopLevelItem(d->objBrowseTree);
	trkObjBrowser->setUpdatesEnabled(true);

	messageVerbose("IParticleCollHandleBase::fillObjectBrowser completed in "+QString::number(t.elapsed())+" ms");

}

//____________________________________________________________________
void IParticleCollHandleBase::assignDefaultMaterial(SoMaterial * m) const
{
	VP1QtInventorUtils::setMatColor( m, defaultColor(), 0.18/*brightness*/ );
}

//____________________________________________________________________
// QString IParticleCollHandleBase::toString(const COLOURBY&cb)
// {
//   switch (cb) {
//   case COLOUR_PERCOLLECTION: return "COLOUR_PERCOLLECTION";
//   case COLOUR_RANDOM: return "COLOUR_RANDOM";
//   case COLOUR_MOMENTUM: return "COLOUR_MOMENTUM";
//   case COLOUR_CHARGE: return "COLOUR_CHARGE";
//   case COLOUR_BYPID: return "COLOUR_BYPID";
//   case COLOUR_DISTANCE: return "COLOUR_DISTANCE";
//   default: return "Unknown (ERROR)";
//   }
// }
//____________________________________________________________________
QList<QWidget*> IParticleCollHandleBase::provideExtraWidgetsForGuiRow() const
{
	// return QList<QWidget*>() << d->comboBox_colourby;
	return QList<QWidget*>();
}

//____________________________________________________________________
QByteArray IParticleCollHandleBase::extraWidgetsState() const
{
	VP1Serialise serialise(0/*version*/,systemBase());
	// serialise.save(d->comboBox_colourby);
	// serialise.disableUnsavedChecks();
	return serialise.result();
}

//____________________________________________________________________
void IParticleCollHandleBase::setExtraWidgetsState(const QByteArray& ba)
{
	// VP1Deserialise state(ba, systemBase());
	// if (state.version()!=0)
	//   return;//just ignore silently... i guess we ought to warn?
	// state.restore(d->comboBox_colourby);
	// state.disableUnrestoredChecks();
	// colourByComboBoxItemChanged();
}

//____________________________________________________________________
void IParticleCollHandleBase::setState(const QByteArray&state)
{
	VP1Deserialise des(state);
	des.disableUnrestoredChecks();
	if (des.version()!=0&&des.version()!=1) {
		messageDebug("Warning: Ignoring state with wrong version");
		return;
	}
	bool vis = des.restoreBool();

	QByteArray matState = des.restoreByteArray();
	// d->matButton->restoreFromState(matState);
	QByteArray extraWidgetState = des.version()>=1 ? des.restoreByteArray() : QByteArray();
	setVisible(vis);

	if (extraWidgetState!=QByteArray())
		setExtraWidgetsState(extraWidgetState);
}

//____________________________________________________________________
QByteArray IParticleCollHandleBase::persistifiableState() const
{
	// if (!d->matButton) {
	//   message("ERROR: persistifiableState() called before init()");
	//   return QByteArray();
	// }
	VP1Serialise serialise(1/*version*/);
	serialise.disableUnsavedChecks();
	serialise.save(visible());
	// Q_ASSERT(d->matButton&&"Did you forget to call init() on this VP1StdCollection?");
	// serialise.save(d->matButton->saveState());
	serialise.save(extraWidgetsState());//version 1+
	return serialise.result();
}
