/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1AODSystems/CaloClusterCollectionSettingsButton.h"
#include "IParticleCollHandle_CaloCluster.h"
// UIs
#include "ui_periparticlecollectionsettings_form.h"
#include "ui_percaloclustercollectionsettings_form.h"

//VP1
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1Msg.h"

//Coin3D
#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"
#include "Inventor/nodes/SoMaterial.h"

// Qt
#include <QTimer>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QShortcut>
#include <QMimeData>
#include <QDrag>

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

// Misc
#include <iostream>

//____________________________________________________________________
class CaloClusterCollectionSettingsButton::Imp {
public:

//	Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0){}
	Imp():dim(0),matButton(nullptr),theclass(nullptr),coll(nullptr),editwindow(nullptr),
	m_transverseEnergy{},m_forceRebuild{},last_useDefaultCuts{},
	gui_mostEnergetic{0.0} {
	  //nop
	}

	int dim;
	QPoint dragStartPosition;

	void initEditWindow();

	// the material button
	VP1MaterialButton* matButton;// main collection colour

	// the main 'button' class
	CaloClusterCollectionSettingsButton * theclass;

	// the collHandle to the Collection
	IParticleCollHandle_CaloCluster* coll;

	// setup the UI
	QWidget * editwindow; // the main window
	Ui::IParticleCollectionSettingsForm ui_commonsettings; // the common main UI window
	Ui::CaloClusterSettingsDisplayForm ui_customsettings; // the custom jet cuts


//	QList<VP1MaterialButton*> parameterTypeMatButtons;// enum used to access the specific button

//	SoDrawStyle * trackDrawStyle;
//	SoLightModel * trackLightModel;

	//GUI - last values

	// common cuts
	VP1Interval last_cutAllowedPt;
	VP1Interval last_cutAllowedP;
	VP1Interval last_cutAllowedEta;
	QList<VP1Interval> last_cutAllowedPhi;
	bool m_transverseEnergy;
	bool m_forceRebuild;

	// specific cuts
	QPair<bool,double> last_scale;
	bool last_useDefaultCuts;

	double gui_mostEnergetic;


};


////____________________________________________________________________
//SoMaterial * CaloClusterCollectionSettingsButton::Imp::createMaterial(const int& r,const int& g,const int& b) const
//{
//  return VP1MaterialButton::createMaterial(r/255.0,g/255.0,b/255.0,0.2/*brightness*/);
//}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::Imp::initEditWindow()
{
	if (editwindow)
		return;
	theclass->messageVerbose("Initialising material editor dialog");

	// create a parent widget
	editwindow = new QWidget(0,Qt::WindowStaysOnTopHint); // parent widget

	// init UI, with the same parent widget
	ui_commonsettings.setupUi(editwindow);
	ui_customsettings.setupUi(editwindow);

	//// CUSTOMIZE "Momentum cuts" widget
	// change labels to match object's nomenclature
	ui_commonsettings.groupBox_cuts_momentum->setTitle("Energy cuts");
	ui_commonsettings.checkBox_cut_minpt->setText("Min Energy");
	ui_commonsettings.checkBox_cut_maxpt->setText("Max Energy");

	ui_commonsettings.comboBox_momtype->setItemText(0, "Et");
	ui_commonsettings.comboBox_momtype->setItemText(1, "E");
//	ui_commonsettings.comboBox_momtype->setEnabled(false); // TODO: enable it back and make it working, now it is not connected

	// change initial values to match jets' typical values
	ui_commonsettings.doubleSpinBox_cut_minpt_gev->setValue(2.00);
	ui_commonsettings.doubleSpinBox_cut_maxpt_gev->setValue(100.00);

	// get a handle on the material button
	matButton = ui_commonsettings.pushButton_matButton;

	// compose UI: adding to the default GUI the "display" custom cuts for jets
	ui_commonsettings.verticalLayout_additional_widgets->layout()->addWidget( ui_customsettings.groupBox_calocluster_scale );
	ui_commonsettings.verticalLayout_additional_widgets->layout()->addWidget( ui_customsettings.groupBox_calocluster_drawsettings );

//	// set "Material" checked by default ("Skin" will be optional)
//	ui_customsettings.radioButton_material->setChecked(true);

//	defaultParametersMatButton = ui_commonsettings.matButton_parametersDefaultColour;
//	SoMaterial* defParamMat = new SoMaterial; // Default (for the moment)
//	defaultParametersMatButton->setMaterial(defParamMat);
//	ui_commonsettings.frame_parameterTypeColours->hide();

	//  // Set the default colours for the "colour by parameter type" section
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_2);
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_3);
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_4);
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_5);
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_6);
	//  parameterTypeMatButtons.append(ui_commonsettings.pushButton_matButton_parameters_default_colour_7);
	//  QList<QColor> colours;
	//  colours.append(QColor(0.0, 170.0, 255.0));
	//  colours.append(QColor(170, 85, 255));
	//  colours.append(QColor(255, 85, 0));
	//  colours.append(QColor(170, 0, 127));
	//  colours.append(QColor(170, 255, 0));
	//  colours.append(QColor(85, 0, 255));
	//
	//  for (unsigned int i=0; i<6;++i){
	//    SoMaterial * mat = VP1MaterialButton::createMaterial(colours.at(i));
	//    (parameterTypeMatButtons.at(i))->setMaterial(mat);
	//  }


//	// when creating a new controller, check if b-tagging checkbox is enabled,
//	// for example from a config file or from a drag&drop from another collection
//	if (ui_customsettings.bTaggingCheckBox->isChecked())
//		theclass->possibleChange_bTaggingEnabled(true); // init the b-tagging toolbox as active
//	else
//		theclass->possibleChange_bTaggingEnabled(false); // init the b-tagging toolbox as not-active
//
//
//	// default material for b-jets
//	materialFallback = createMaterial(116,255,228);
//	materialFallback->ref();

}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::setMaterialText(const QString& t)
{
	VP1Msg::messageVerbose("CaloClusterCollectionSettingsButton::setMaterialText()");
	if (m_d->editwindow)
		m_d->editwindow->setWindowTitle(t);
	setToolTip(t);
}


//____________________________________________________________________
double CaloClusterCollectionSettingsButton::lengthOf10GeV()
{
	double val = m_d->ui_customsettings.doubleSpinBox_lengthOf10GeV->value();
	VP1Msg::messageVerbose("CaloClusterCollectionSettingsButton::lengthOf10GeV() - value: " + QString::number(val));
	return val;
}

////____________________________________________________________________
//double CaloClusterCollectionSettingsButton::maxR()
//{
//	double val = m_d->ui_customsettings.checkBox_maxR->isChecked() ? (m_d->ui_customsettings.doubleSpinBox_maxR->value() ) : -1; //m_d->ui_customsettings.doubleSpinBox_maxR->value();
//	VP1Msg::messageVerbose("CaloClusterCollectionSettingsButton::maxR() - value: " + QString::number(val));
//	return val;
//}







//____________________________________________________________________
CaloClusterCollectionSettingsButton::CaloClusterCollectionSettingsButton(QWidget * parent,int dim)
: VP1CollectionSettingsButtonBase(parent,0), m_d(new Imp)
//: VP1CollectionSettingsButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
	m_d->dim = dim;

	m_d->theclass = this;
	m_d->initEditWindow();

	m_d->m_transverseEnergy = true;
	m_d->m_forceRebuild = false;

//	// default material for b-jets
//	m_d->materialFallback = m_d->createMaterial(116,255,228);
//	m_d->materialFallback->ref();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// COMMON SETTINGS

	// -> cutAllowedP/Pt
	connect(m_d->ui_commonsettings.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->ui_commonsettings.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->ui_commonsettings.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->ui_commonsettings.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->ui_commonsettings.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->ui_commonsettings.comboBox_momtype,SIGNAL(currentIndexChanged(int)),this,SLOT(setTransverseEnergy()));

	// -> cutAllowedEta
	connect(m_d->ui_commonsettings.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)),this,SLOT(possibleChange_cutAllowedEta()));

	// -> cutAllowedPhi
	connect(m_d->ui_commonsettings.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(possibleChange_cutAllowedPhi()));


	// CUSTOM SETTINGS

//	//Draw Styles / Complexity:
//	VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->ui_commonsettings.horizontalSlider_trackWidth);
//	VP1QtInventorUtils::setValueLineWidthSlider(m_d->ui_commonsettings.horizontalSlider_trackWidth,1.0);

	// -> scale
	connect(m_d->ui_customsettings.doubleSpinBox_lengthOf10GeV,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_scale()));
	connect(m_d->ui_customsettings.checkBox_logscale,SIGNAL(toggled(bool)),this,SLOT(possibleChange_scale()));
	connect(m_d->ui_customsettings.radioButton_relativeScale,SIGNAL(toggled(bool)),this,SLOT(possibleChange_scale()));
	connect(m_d->ui_customsettings.doubleSpinBox_lengthOfMostEnergetic,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_scale()));





//	// random colors
//	connect(m_d->ui_customsettings.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(enableRandomColours(bool)));
//	connect(m_d->ui_customsettings.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(possibleChange_randomJetColours()));
//	connect(m_d->ui_customsettings.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));
//	this->enableRandomColours(false);

//	// maxR
//	connect(m_d->ui_customsettings.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(enableMaxR(bool)));
//	connect(m_d->ui_customsettings.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(possibleChange_maxR()));
//	connect(m_d->ui_customsettings.doubleSpinBox_maxR, SIGNAL(valueChanged(double)), this, SLOT(possibleChange_maxR()));
//	this->enableMaxR(false);




	// Material button
	connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(m_d->ui_commonsettings.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
	connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
	setAcceptDrops(true);


//	// init material for b-tagged jet
//	m_d->ui_customsettings.matButton_btaggedJets->setMaterial(m_d->materialFallback);


	QTimer::singleShot(0, this, SLOT(updateButton()));

}

// QWidget& CaloClusterCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
CaloClusterCollectionSettingsButton::~CaloClusterCollectionSettingsButton()
{
	delete m_d->editwindow;
//	m_d->trackDrawStyle->unref();
//	m_d->trackLightModel->unref();
	delete m_d;
}

void CaloClusterCollectionSettingsButton::updateButton()
{
	if (objectName().isEmpty())
		setObjectName("CaloClusterCollectionSettingsButton");
	messageVerbose("setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
	VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void CaloClusterCollectionSettingsButton::setDimension(int dim)
{
	if (m_d->dim == dim)
		return;
	m_d->dim = dim;
	updateButton();
}

void CaloClusterCollectionSettingsButton::showEditMaterialDialog()
{
	if (!m_d->editwindow)
		m_d->initEditWindow();

	if (m_d->editwindow->isHidden())
		m_d->editwindow->show();
	else
		m_d->editwindow->hide();
}

bool CaloClusterCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"CaloClusterCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
	if (!m_d->matButton) m_d->initEditWindow();
	m_d->matButton->setMaterial(mat);
	return true;
}

void CaloClusterCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
	if (!m_d->matButton) m_d->initEditWindow();
	m_d->matButton->setMaterial(mat);
}
double CaloClusterCollectionSettingsButton::lastAppliedTransparency() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedTransparency();
}
double CaloClusterCollectionSettingsButton::lastAppliedShininess() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedShininess();
}
double CaloClusterCollectionSettingsButton::lastAppliedBrightness() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedBrightness();
}



//____________________________________________________________________
void CaloClusterCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_d->dragStartPosition = event->pos();
	QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
		event->acceptProposedAction();
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - m_d->dragStartPosition).manhattanLength()
			< QApplication::startDragDistance())
		return;

	QDrag *drag = new QDrag(this);
	QMimeData *mimeData = new QMimeData;

	/////////////////////////////////////////////////////
	// For dragging state onto other material buttons: //
	/////////////////////////////////////////////////////

	QByteArray byteArray = saveState();

	// buffer.close();
	mimeData->setData("vp1/material", byteArray);

	// ////////////////////////////////////////////////////////
	// //  For dragging c++ code for defining this material  //
	// //  into e.g. an editor:                              //
	// ////////////////////////////////////////////////////////
	//
	// QString s = "SoMaterial * mat = new SoMaterial;\n";
	// QString str_ambient = m_d->toSbColTxt(m_d->lastapplied_ambient);
	// if (str_ambient!="SbColor(0.2,0.2,0.2)")
	//   s += "mat->ambientColor.setValue("+str_ambient+");\n";
	// QString str_diffuse = m_d->toSbColTxt(m_d->lastapplied_diffuse);
	// if (str_diffuse!="SbColor(0.8,0.8,0.8)")
	//   s += "mat->diffuseColor.setValue("+str_diffuse+");\n";
	// QString str_specular = m_d->toSbColTxt(m_d->lastapplied_specular);
	// if (str_specular!="SbColor(0,0,0)")
	//   s += "mat->specularColor.setValue("+str_specular+");\n";
	// QString str_emissive = m_d->toSbColTxt(m_d->lastapplied_emissive);
	// if (str_emissive!="SbColor(0,0,0)")
	//   s += "mat->emissiveColor.setValue("+str_emissive+");\n";
	// QString str_shininess = m_d->printFloat(m_d->lastapplied_shininess/100.0);
	// if (str_shininess!="0.2")
	//   s +=     "mat->shininess.setValue("+str_shininess+");\n";
	// QString str_transparency = m_d->printFloat(m_d->lastapplied_transparency/100.0);
	// if (str_transparency!="0")
	//   s +=     "mat->transparency.setValue("+str_transparency+");\n";
	// mimeData->setText(s);

	//Execute drag:
	drag->setMimeData(mimeData);//drag assumes ownership of mimeData
	drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::dropEvent(QDropEvent *event)
{
	QByteArray data = event->mimeData()->data("vp1/material");
	event->acceptProposedAction();
	restoreFromState(data);
}


QByteArray CaloClusterCollectionSettingsButton::saveState() const{

	messageDebug("CaloClusterCollectionSettingsButton::saveState()");

	// messageVerbose("getState");
	// if (m_d->ui_commonsettings.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
	VP1Serialise serialise(1/*version*/);


	serialise.save(m_d->matButton);
	//serialise.disableUnsavedChecks(); // TODO: what this does??

	// COMMON SETTINGS
	serialise.save(m_d->ui_commonsettings.checkBox_tracksUseBaseLightModel);
	serialise.save(m_d->ui_commonsettings.checkBox_cut_minpt);
	serialise.save(m_d->ui_commonsettings.doubleSpinBox_cut_minpt_gev);
	serialise.save(m_d->ui_commonsettings.checkBox_cut_maxpt);
	serialise.save(m_d->ui_commonsettings.doubleSpinBox_cut_maxpt_gev);
	serialise.save(m_d->ui_commonsettings.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	serialise.save(m_d->ui_commonsettings.etaPhiCutWidget);

	// CALO CLUSTER SETTINGS
	serialise.save(m_d->ui_customsettings.radioButton_absoluteScale, m_d->ui_customsettings.radioButton_relativeScale);
	serialise.save(m_d->ui_customsettings.doubleSpinBox_lengthOf10GeV);
	serialise.save(m_d->ui_customsettings.doubleSpinBox_lengthOfMostEnergetic);
	serialise.save(m_d->ui_customsettings.checkBox_logscale);
	serialise.save(m_d->ui_customsettings.checkBox_showVolumeOutLines);



//	serialise.save(m_d->ui_customsettings.doubleSpinBox_radialScale);

//	serialise.save(m_d->ui_customsettings.bTaggingCheckBox);
//	serialise.save(m_d->ui_customsettings.bTaggingComboBox);
//	serialise.save(m_d->ui_customsettings.bTaggingSpinBox);
//	serialise.save(m_d->ui_customsettings.radioButton_material, m_d->ui_customsettings.radioButton_skins);
//	serialise.save(m_d->ui_customsettings.matButton_btaggedJets);
//	serialise.save(m_d->ui_customsettings.skinsComboBox);

	// Parameters
//	serialise.save(m_d->ui_commonsettings.checkBox_showparameters);
//	serialise.save(m_d->ui_commonsettings.checkBox_parameters_colourByType);

	serialise.widgetHandled(this);
	serialise.warnUnsaved(this);
	return serialise.result();
}

void CaloClusterCollectionSettingsButton::restoreFromState( const QByteArray& ba){

	messageDebug("CaloClusterCollectionSettingsButton::restoreFromState()");

	VP1Deserialise state(ba,systemBase());
	if (state.version()<0||state.version()>1) {
		messageDebug("restoreFromState() - ignoring...");
		return;//Ignore silently
	}
	state.restore(m_d->matButton);


	// COMMON SETTINGS
	state.restore(m_d->ui_commonsettings.checkBox_tracksUseBaseLightModel);
	state.restore(m_d->ui_commonsettings.checkBox_cut_minpt);
	state.restore(m_d->ui_commonsettings.doubleSpinBox_cut_minpt_gev);
	state.restore(m_d->ui_commonsettings.checkBox_cut_maxpt);
	state.restore(m_d->ui_commonsettings.doubleSpinBox_cut_maxpt_gev);
	state.restore(m_d->ui_commonsettings.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	state.restore(m_d->ui_commonsettings.etaPhiCutWidget);

	// CALO CLUSTER SETTINGS
	state.restore(m_d->ui_customsettings.radioButton_absoluteScale, m_d->ui_customsettings.radioButton_relativeScale);
	state.restore(m_d->ui_customsettings.doubleSpinBox_lengthOf10GeV);
	state.restore(m_d->ui_customsettings.doubleSpinBox_lengthOfMostEnergetic);
	state.restore(m_d->ui_customsettings.checkBox_logscale);
	state.restore(m_d->ui_customsettings.checkBox_showVolumeOutLines);



//	state.restore(m_d->ui_customsettings.doubleSpinBox_radialScale);

//	state.restore(m_d->ui_customsettings.bTaggingCheckBox);
//	state.restore(m_d->ui_customsettings.bTaggingComboBox);
//	state.restore(m_d->ui_customsettings.bTaggingSpinBox);
//	state.restore(m_d->ui_customsettings.radioButton_material, m_d->ui_customsettings.radioButton_skins);
//	state.restore(m_d->ui_customsettings.matButton_btaggedJets);
//	state.restore(m_d->ui_customsettings.skinsComboBox);


	//	// Parameters
//	state.restore(m_d->ui_commonsettings.checkBox_showparameters);
//	state.restore(m_d->ui_commonsettings.checkBox_parameters_colourByType);

	state.widgetHandled(this);
	state.warnUnrestored(this);

//	updateTrackTubeRadius();
//	updateTrackLightModel(m_d->ui_commonsettings.checkBox_tracksUseBaseLightModel);

	updateButton();

//	// after restoring the state, check if b-tagging checkbox is enabled,
//	if (m_d->ui_customsettings.bTaggingCheckBox->isChecked())
//		possibleChange_bTaggingEnabled(true); // init the b-tagging toolbox as active
//	else
//		possibleChange_bTaggingEnabled(false); // init the b-tagging toolbox as not-active


	//FIXME - anything else need updating?
}


//____________________________________________________________________
bool CaloClusterCollectionSettingsButton::isTransverseEnergy() const
{
	return m_d->m_transverseEnergy;
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::setTransverseEnergy()
{
	if(m_d->ui_commonsettings.comboBox_momtype->currentText()=="Et")
		m_d->m_transverseEnergy = true;
	else
		m_d->m_transverseEnergy = false;


	messageDebug("setTransverseEnergy: "+QString::number(m_d->m_transverseEnergy));

//	// when we change Et<-->E we force the rebuild of all objects in the scene
//	m_d->m_forceRebuild = true;
	// then, we rebuild all objects
	emit energyTypeChanged();
}

//____________________________________________________________________
VP1Interval CaloClusterCollectionSettingsButton::cutAllowedPt() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();

	if (!m_d->ui_commonsettings.checkBox_cut_minpt)
		return VP1Interval();

	// will set range to negative if we have momcut=P
	// if minCut unset then min=-inf
	// if minCut set, and Pt selected, then min=-minCut
	// if minCut set, and P selected, then min=-maxCut // TODO: Check this logic!!!!!  ????
	// etc
	//bool EtCut = isTransverseEnergy();

	const double minFromInterface=m_d->ui_commonsettings.doubleSpinBox_cut_minpt_gev->value()*1000; // from GeV to MeV
	const double maxFromInterface=m_d->ui_commonsettings.doubleSpinBox_cut_maxpt_gev->value()*1000;

	double min=0.0,max=0.0;

	// TODO: I don't understand this logic...
	// it looks wrong to me, and the E cut does not currently work...
	// Moving to a simpler logic.
	/*
	if (EtCut) {
		//Et cut
		min = (m_d->ui_commonsettings.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
		max = (m_d->ui_commonsettings.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
	} else {
		//E cut
		min = (m_d->ui_commonsettings.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
		max = (m_d->ui_commonsettings.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
	}*/

	min = (m_d->ui_commonsettings.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
	max = (m_d->ui_commonsettings.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());

	message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));

	if (max<min)
		return VP1Interval();

	return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval CaloClusterCollectionSettingsButton::cutAllowedEta() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();
	return m_d->ui_commonsettings.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> CaloClusterCollectionSettingsButton::cutAllowedPhi() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();
	return m_d->ui_commonsettings.etaPhiCutWidget->allowedPhi();
}



//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_cutAllowedPt()
{
	messageVerbose("possibleChange_cutAllowedPt() ");

	if ( m_d->last_cutAllowedPt==cutAllowedPt()) return;
//	if ( !m_d->m_forceRebuild && m_d->last_cutAllowedPt==cutAllowedPt()) return;

	messageVerbose("cutAllowedPt() changed");
	m_d->last_cutAllowedPt= cutAllowedPt();
	emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_cutAllowedEta()
{
	if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
	messageVerbose("cutAllowedEta() changed");
	m_d->last_cutAllowedEta=cutAllowedEta();
	emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
	if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
	messageVerbose("cutAllowedPhi() changed");
	m_d->last_cutAllowedPhi=cutAllowedPhi();
	emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_scale()
{
	messageDebug("CaloClusterCollectionSettingsButton::possibleChange_scale() ");

//	if (m_d->last_scale == lengthOf10GeV()) return;
	if (m_d->last_scale == scale()) return;

//	messageVerbose("lengthOf10GeV changed");
//	m_d->last_scale = lengthOf10GeV();

	messageDebug("cluster size settings changed");
	m_d->last_scale = scale();

	emit scaleChanged(m_d->last_scale); // it will trigger an update in the collHandle
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::setCollHandle(IParticleCollHandle_CaloCluster* collHandle)
{
	m_d->coll = collHandle;
}

//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_showVolumeOutLines()
{
	messageVerbose("CaloClusterCollectionSettingsButton::possibleChange_showVolumeOutLines() ");

	message("CaloCluster - possibleChange_showVolumeOutLines(). to be implemented...");
}


//____________________________________________________________________
void CaloClusterCollectionSettingsButton::possibleChange_useTransverseEnergies()
{
	messageVerbose("CaloClusterCollectionSettingsButton::possibleChange_useTransverseEnergies() ");

	message("CaloCluster - possibleChange_useTransverseEnergies(). to be implemented...");
}


//____________________________________________________________________
QPair<bool,double> CaloClusterCollectionSettingsButton::scale() const
{
  const bool relative = m_d->ui_customsettings.radioButton_relativeScale->isChecked();
  const bool logscale = m_d->ui_customsettings.checkBox_logscale->isChecked();

  double highestvisibleenergy=0*SYSTEM_OF_UNITS::eV;

////  foreach(VP1StdCollection* stdcol, m_d->collWidget->visibleStdCollections()) {
////    VP1CaloClusterCollection* col = dynamic_cast<VP1CaloClusterCollection*>(stdcol);

  std::cout << "coll: ";
  std::cout << m_d->coll << std::endl;
  std::cout << "highestvisibleenergy: ";
  std::cout << highestvisibleenergy << std::endl;
  std::cout << "coll->name: ";
  QString colname = m_d->coll->name();
  VP1Msg::messageDebug(colname);
//  std::cout << "coll->highestVisibleClusterEnergy(): ";
//  std::cout << m_d->coll->highestVisibleClusterEnergy() << std::endl;

  //IParticleCollHandle_CaloCluster* col = dynamic_cast<IParticleCollHandle_CaloCluster*>(m_d->coll);
  if (dynamic_cast<IParticleCollHandle_CaloCluster*>(m_d->coll)) {
	  std::cout << "col OK! " << std::endl;
    if ( m_d->coll && highestvisibleenergy < m_d->coll->highestVisibleClusterEnergy() )
      highestvisibleenergy = m_d->coll->highestVisibleClusterEnergy();
  }

  ////  }

  if (m_d->gui_mostEnergetic!=highestvisibleenergy) {
    m_d->gui_mostEnergetic=highestvisibleenergy;
    m_d->ui_customsettings.label_current_most_energetic->setText("Current value: "+QString::number(m_d->gui_mostEnergetic/SYSTEM_OF_UNITS::GeV,'f',2)+" GeV");
  }

  const double length = (relative ? m_d->ui_customsettings.doubleSpinBox_lengthOfMostEnergetic->value()*SYSTEM_OF_UNITS::m
			 : m_d->ui_customsettings.doubleSpinBox_lengthOf10GeV->value()*SYSTEM_OF_UNITS::m );
  const double energy = relative ? highestvisibleenergy : 10*SYSTEM_OF_UNITS::GeV;
  const double minscale = 1*SYSTEM_OF_UNITS::mm/(1*SYSTEM_OF_UNITS::GeV);
  const double maxscale = 1*SYSTEM_OF_UNITS::m/(1*SYSTEM_OF_UNITS::MeV);
  double scl;
  if (energy<=0)
    scl = maxscale;
  else
    scl = logscale ? length/log(1+fabs(energy)) :length/energy;
  if (scl!=scl||scl>maxscale)
    scl = maxscale;
  if (scl<minscale)
    scl = minscale;
  return QPair<bool,double>(logscale,scl);
}


////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::enableRandomColours(bool bb)
//{
//	m_d->ui_customsettings.pushButton_colourbyrandom_rerandomise->setEnabled(bb);
//}
//
////____________________________________________________________________
//bool CaloClusterCollectionSettingsButton::randomJetColours() const
//{
//  return m_d->ui_customsettings.checkBox_randomColours->isChecked();
//}
//
//
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::emitRerandomise()
//{
//  messageVerbose("CaloClusterCollectionSettingsButton - Emitting rerandomise");
//  emit rerandomise();
//}

////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::emitMaxR()
//{
//  messageVerbose("CaloClusterCollectionSettingsButton - Emitting signalMaxR");
//  emit signalMaxR();
//}

////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::enableMaxR(bool bb)
//{
//	m_d->ui_customsettings.doubleSpinBox_maxR->setEnabled(bb);
//}

////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_maxR()
//{
//	messageVerbose("possibleChange_maxR() ");
//
//	if ( !  m_d->ui_customsettings.checkBox_maxR->isChecked() ) {
//		messageVerbose("maxR unchecked --> setting maxR=0.0 and returning");
//		emit maxRChanged(0.0); // setting marR=0.0 disables the maxR option in handleJet
//		return;
//	}
//
//	messageVerbose("setting maxR");
//	m_d->last_maxR = maxR();
//	emit maxRChanged(m_d->last_maxR);
//}

////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingEnabled(bool bb)
//{
//	messageVerbose("possibleChange_bTaggingEnabled()");
//
//	m_d->ui_customsettings.bTaggingAlgLabel->setEnabled(bb);
//	m_d->ui_customsettings.bTagginWeightCutLabel->setEnabled(bb);
//	m_d->ui_customsettings.bTaggingComboBox->setEnabled(bb);
//	m_d->ui_customsettings.bTaggingSpinBox->setEnabled(bb);
//	m_d->ui_customsettings.groupBox_btagging_render->setEnabled(bb);
//	m_d->ui_customsettings.matButton_btaggedJets->setEnabled(bb);
//
//	emit bTaggingEnabledChanged(bb);
//}
//
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingTagger()
//{
//	messageVerbose("possibleChange_bTaggingTagger() - " + bTaggingTagger() );
//	emit bTaggingTaggerChanged( bTaggingTagger() );
//}
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingCut()
//{
//	messageVerbose("possibleChange_bTaggingCut() - " + QString::number(bTaggingCut()) );
//
//	if (m_d->last_bTaggingCut == bTaggingCut() ) return;
//
//	messageVerbose("bTaggingSpinBox changed");
//	m_d->last_bTaggingCut = bTaggingCut();
//
//	emit bTaggingCutChanged( bTaggingCut() );
//}


//// called when user clicks on "Material" checkbox
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingRenderingMaterial(bool ok)
//{
//	messageVerbose("possibleChange_bTaggingRenderingMaterial()");
//	messageVerbose("radioButton_material changed - " + QString::number(ok) );
//
//	emit bTaggingRenderingMaterialChanged(ok);
//}

// not needed...
//// called when b-tagging material changes
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingMaterial()
//{
//	messageVerbose("possibleChange_bTaggingMaterial()");
//
//	if (m_d->last_bTaggingMaterial == bTaggingMaterial() ) {
//		messageVerbose("material not changed. Returning...");
//		return;
//	}
//
//	messageVerbose("matButton_btaggedJets changed");
//	m_d->last_bTaggingMaterial = bTaggingMaterial();
//
//	emit bTaggingMaterialChanged();
//}




//// called when user clicks on "Skin" checkbox
////____________________________________________________________________
//void CaloClusterCollectionSettingsButton::possibleChange_bTaggingRenderingSkin(bool ok)
//{
//	messageVerbose("possibleChange_bTaggingRenderingSkin()");
//	messageVerbose("radioButton_skins changed - " + QString::number(ok) );
//
//	emit bTaggingRenderingSkinChanged(ok);
//}
