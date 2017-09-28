/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1AODSystems/JetCollectionSettingsButton.h"
// UIs
//#include "ui_perjetcollectionsettings_form.h"
#include "ui_periparticlecollectionsettings_form.h"
#include "ui_jet_settings_display_form.h"

//VP1
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtInventorUtils.h"
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

// Misc
#include <iostream>

//____________________________________________________________________
class JetCollectionSettingsButton::Imp {
public:

//	Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0){}
	Imp():theclass(0),editwindow(0),matButton(0), last_scale(0.0), last_useDefaultCuts{},
	last_randomJetColours{}, last_useMaxR{}, last_maxR(0.0), last_bTaggingEnabled{},
	last_bTaggingCut(0.0),last_bTaggingRenderingSkin{}, last_bTaggingRenderingMaterial{},
	m_jetMaterialDefault(nullptr), materialFallback(nullptr),
	dim(0) {}

	// the main class
	JetCollectionSettingsButton * theclass;

	// setup the UI
	QWidget * editwindow; // the main window
	Ui::IParticleCollectionSettingsForm editwindow_ui; // the common main UI window
	Ui::JetSysSettingsDisplayForm ui_disp; // the custom jet cuts

	VP1MaterialButton* matButton;// main collection colour
//	VP1MaterialButton* defaultParametersMatButton;

//	QList<VP1MaterialButton*> parameterTypeMatButtons;// enum used to access the specific button

//	SoDrawStyle * trackDrawStyle;
//	SoLightModel * trackLightModel;

	//GUI - last values
	VP1Interval last_cutAllowedPt;
	VP1Interval last_cutAllowedP;
	VP1Interval last_cutAllowedEta;
	QList<VP1Interval> last_cutAllowedPhi;
	double last_scale;
	bool last_useDefaultCuts;
	bool last_randomJetColours;
	bool last_useMaxR;
	double last_maxR;
//	double last_trackTubeRadius;
//	QList<unsigned> last_cutRequiredNHits;
//	bool last_cutOnlyVertexAssocTracks;

	// GUI b-tagging
	bool last_bTaggingEnabled;
	QString last_bTaggingTagger;
	QString last_bTaggingSkin;
	double last_bTaggingCut;
//	SoMaterial* last_bTaggingMaterial; // not needed...
	bool last_bTaggingRenderingSkin;
	bool last_bTaggingRenderingMaterial;

	// jet material
	SoMaterial* m_jetMaterialDefault;

	// b-tagging material
	SoMaterial * materialFallback;

	// material functions
	SoMaterial * getMat(VP1MaterialButton*) const;
	SoMaterial * createMaterial(const int& r,const int& g,const int& b) const;
	SoMaterial * createMaterial(const int& r,const int& g,const int& b, const int& brightness, const int& transparency) const;


	int dim;
	QString name;
	QPoint dragStartPosition;

	void initEditWindow();
};


//____________________________________________________________________
SoMaterial * JetCollectionSettingsButton::Imp::createMaterial(const int& r,const int& g,const int& b) const
{
  return VP1MaterialButton::createMaterial(r/255.0,g/255.0,b/255.0,0.2/*brightness*/);
}
//____________________________________________________________________
SoMaterial * JetCollectionSettingsButton::Imp::createMaterial(const int& r,const int& g,const int& b, const int& brightness, const int& transparency) const
{
  return VP1MaterialButton::createMaterial(r/255.0,g/255.0,b/255.0, brightness, transparency);
}

//____________________________________________________________________
void JetCollectionSettingsButton::Imp::initEditWindow()
{
	if (editwindow)
		return;
	theclass->messageVerbose("Initialising material editor dialog");

	// create a parent widget
	editwindow = new QWidget(0,Qt::WindowStaysOnTopHint); // parent widget

	// init the different UIs, with the same parent widget
	editwindow_ui.setupUi(editwindow);
	ui_disp.setupUi(editwindow);



	//// CUSTOMIZE "Momentum cuts" widget
	// change labels to match jets' nomenclature
	editwindow_ui.groupBox_cuts_momentum->setTitle("Energy cuts");
	editwindow_ui.checkBox_cut_minpt->setText("Min Energy");
	editwindow_ui.checkBox_cut_maxpt->setText("Max Energy");
	editwindow_ui.comboBox_momtype->setItemText(0, "Et");
	editwindow_ui.comboBox_momtype->setItemText(1, "E");
	// change initial values to match jets' typical values
	editwindow_ui.doubleSpinBox_cut_minpt_gev->setValue(10.00);
	editwindow_ui.doubleSpinBox_cut_maxpt_gev->setValue(100.00);

	// get a handle on the material button
	matButton = editwindow_ui.pushButton_matButton;

	// compose UI: adding to the default GUI the "display" custom cuts for jets
	editwindow_ui.verticalLayout_additional_widgets->layout()->addWidget( ui_disp.groupBox_jet_display );

	// set b-tagging taggers
	ui_disp.bTaggingComboBox->clear(); // remove all taggers defined in the .ui file

	QStringList bTagList;
	//bTagList << "MV2c20" << "MV2c10" << "MV1" << "JetFitterCombNN_pb" << "JetFitterCombNN_pc" << "JetFitterCombNN_pu";
	//bTagList << "MV2c20" << "MV2c10" << "MV2c00" << "MV1" << "JetFitterCombNN_pb" << "JetFitterCombNN_pc" << "JetFitterCombNN_pu"; // the JetFitter helpers have been removed in xAODBTagging-00-00-35
//	bTagList << "MV2c20" << "MV2c10" << "MV2c00" << "MV1"; // (cfr. SVN changesets 797165 + 801102)
    bTagList << "MV2c20" << "MV2c10" << "MV2c00"; // 28 Sep 2017
	ui_disp.bTaggingComboBox->insertItems(0, bTagList);

	// set b-tagging "Material" checked by default ("Skin" will be optional)
	ui_disp.radioButton_material->setChecked(true);

	// x-y projection
	ui_disp.groupBox_IDprojection->setEnabled(false); // TODO: check what it did in previous versions and fix it!

	// when creating a new controller, check if b-tagging checkbox is enabled,
	// for example from a config file or from a drag&drop from another collection
	if (ui_disp.bTaggingCheckBox->isChecked())
		theclass->possibleChange_bTaggingEnabled(true); // init the b-tagging toolbox as active
	else
		theclass->possibleChange_bTaggingEnabled(false); // init the b-tagging toolbox as not-active
}

//____________________________________________________________________
void JetCollectionSettingsButton::setMaterialText(const QString& t)
{
	VP1Msg::messageVerbose("JetCollectionSettingsButton::setMaterialText()");
	if (d->editwindow)
		d->editwindow->setWindowTitle(t);
	setToolTip(t);
}


//____________________________________________________________________
double JetCollectionSettingsButton::lengthOf100GeV()
{
	double val = d->ui_disp.doubleSpinBox_lengthOf100GeV->value();
	VP1Msg::messageVerbose("JetCollectionSettingsButton::lengthOf100GeV() - value: " + QString::number(val));
	return val;
}

//____________________________________________________________________
double JetCollectionSettingsButton::maxR()
{
	double val = d->ui_disp.checkBox_maxR->isChecked() ? (d->ui_disp.doubleSpinBox_maxR->value() ) : -1; //d->ui_disp.doubleSpinBox_maxR->value();
	VP1Msg::messageVerbose("JetCollectionSettingsButton::maxR() - value: " + QString::number(val));
	return val;
}


//____________________________________________________________________
double JetCollectionSettingsButton::bTaggingCut() const {
  return d->ui_disp.bTaggingSpinBox->value();
}
//____________________________________________________________________
QString JetCollectionSettingsButton::bTaggingTagger() const {
  return d->ui_disp.bTaggingComboBox->currentText();
}
//____________________________________________________________________
QString JetCollectionSettingsButton::bTaggingSkin() const {
  return d->ui_disp.skinsComboBox->currentText();
}
//____________________________________________________________________
SoMaterial* JetCollectionSettingsButton::bTaggingMaterial() const {
  return getMaterial(d->ui_disp.matButton_btaggedJets);
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingEnabled() const {
  return d->ui_disp.bTaggingCheckBox->isChecked();
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingRenderingSkin() const {
  return d->ui_disp.radioButton_skins->isChecked();
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingRenderingMaterial() const {
  return d->ui_disp.radioButton_material->isChecked();
}

//____________________________________________________________________
bool JetCollectionSettingsButton::is_bTaggingSkinEnabled() const
{
	return d->ui_disp.radioButton_skins->isChecked();
}

//____________________________________________________________________
bool JetCollectionSettingsButton::is_bTaggingMaterialEnabled() const
{
	return d->ui_disp.radioButton_material->isChecked();
}





//____________________________________________________________________
JetCollectionSettingsButton::JetCollectionSettingsButton(QWidget * parent,int _dim, QString name)
: VP1CollectionSettingsButtonBase(parent,0), d(new Imp)
//: VP1CollectionSettingsButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
{
	d->dim = _dim;
	d->name = name;

	d->theclass = this;
	d->initEditWindow();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//	//Draw Styles / Complexity:
//	VP1QtInventorUtils::setLimitsLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
//	VP1QtInventorUtils::setValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth,1.0);

//	d->trackDrawStyle = new SoDrawStyle;
//	d->trackDrawStyle->setName("JetDrawStyle");
//	d->trackDrawStyle->pointSize=5.0;
//	d->trackDrawStyle->ref();
//	updateTrackDrawStyle();
//	connect(d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));

//	d->trackLightModel = new SoLightModel;
//	d->trackLightModel->setName("JetLightModel");
//	d->trackLightModel->ref();
//	updateTrackLightModel(false);
//	connect(d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
//
//	d->last_trackTubeRadius=trackTubeRadius();
//	connect(d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
//	connect(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
//
//	connect(d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

//	// -> parameters
//	connect(d->editwindow_ui.checkBox_showparameters,SIGNAL(toggled(bool)),this,SLOT(possibleChange_showParameters()));
//	connect(d->editwindow_ui.checkBox_parameters_colourByType,SIGNAL(toggled(bool)),this,SLOT(possibleChange_parameterColours()));


	// -> scale
	connect(d->ui_disp.doubleSpinBox_lengthOf100GeV,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_scale()));

	// -> cutAllowedP/Pt
	connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(d->editwindow_ui.comboBox_momtype,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_cutAllowedPt()));

	// -> cutAllowedEta
	connect(d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)),this,SLOT(possibleChange_cutAllowedEta()));

	// -> cutAllowedPhi
	connect(d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(possibleChange_cutAllowedPhi()));

	// random colors
	connect(d->ui_disp.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(enableRandomColours(bool)));
	connect(d->ui_disp.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(possibleChange_randomJetColours()));
	connect(d->ui_disp.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));
	this->enableRandomColours(false);

	// maxR
	connect(d->ui_disp.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(enableMaxR(bool)));
	connect(d->ui_disp.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(possibleChange_maxR()));
	connect(d->ui_disp.doubleSpinBox_maxR, SIGNAL(valueChanged(double)), this, SLOT(possibleChange_maxR()));
	this->enableMaxR(false);


	// BTagging
	connect(d->ui_disp.bTaggingCheckBox, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingEnabled(bool)) );

	connect(d->ui_disp.bTaggingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(possibleChange_bTaggingTagger()) );

	connect(d->ui_disp.bTaggingSpinBox, SIGNAL(valueChanged(double)), this, SLOT(possibleChange_bTaggingCut()) );

	connect(d->ui_disp.radioButton_material, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingRenderingMaterial(bool)) );

	// not needed...
	//connect(d->ui_disp.matButton_btaggedJets, SIGNAL(lastAppliedChanged()), this, SLOT(possibleChange_bTaggingMaterial()) );

	connect(d->ui_disp.radioButton_skins, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingRenderingSkin(bool)) );


	// Material button
	connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
	connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
	setAcceptDrops(true);

	QTimer::singleShot(0, this, SLOT(updateButton()));

	////////////////////////
	//  Custom Materials  //
	////////////////////////

	std::cout << "setting jet custom materials..." << std::endl;



	// create default material for b-jets
	d->materialFallback = d->createMaterial(116,255,228, 0.2, 0.3); // light greenish-blue (R,G,B, brightness, transparency)
	d->materialFallback->ref();



	// init material for b-tagged jet
	d->ui_disp.matButton_btaggedJets->setMaterial(d->materialFallback);

//	updateButton();


}

// QWidget& JetCollectionSettingsButton::editWindow() {
//   if (!d->editwindow)
//     initEditWindow();
//   return *(d->editwindow);
// } 
JetCollectionSettingsButton::~JetCollectionSettingsButton()
{
	delete d->editwindow;
//	d->trackDrawStyle->unref();
//	d->trackLightModel->unref();
	delete d;
}

void JetCollectionSettingsButton::updateButton()
{
	if (objectName().isEmpty())
		setObjectName("JetCollectionSettingsButton");
	messageVerbose("jet - setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
	VP1ColorSelectButton::setColButtonProperties(this,d->matButton->lastAppliedDiffuseColour(),d->dim);
}

void JetCollectionSettingsButton::setDimension(int _dim)
{
	if (d->dim == _dim)
		return;
	d->dim = _dim;
	updateButton();
}

void JetCollectionSettingsButton::showEditMaterialDialog()
{
	if (!d->editwindow)
		d->initEditWindow();

	if (d->editwindow->isHidden())
		d->editwindow->show();
	else
		d->editwindow->hide();
}

bool JetCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"JetCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
	if (!d->matButton) d->initEditWindow();
	d->matButton->setMaterial(mat);
	return true;
}

VP1MaterialButton* JetCollectionSettingsButton::getMaterialButton() const
{
	return d->matButton;
}

void JetCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
	if (!d->matButton) d->initEditWindow();
	d->matButton->setMaterial(mat);
}
double JetCollectionSettingsButton::lastAppliedTransparency() const
{
	if (!d->matButton) d->initEditWindow();
	return d->matButton->lastAppliedTransparency();
}
double JetCollectionSettingsButton::lastAppliedShininess() const
{
	if (!d->matButton) d->initEditWindow();
	return d->matButton->lastAppliedShininess();
}
double JetCollectionSettingsButton::lastAppliedBrightness() const
{
	if (!d->matButton) d->initEditWindow();
	return d->matButton->lastAppliedBrightness();
}

//double JetCollectionSettingsButton::trackTubeRadius() const
//{
//	return d->editwindow_ui.checkBox_trackTubes->isChecked() ?
//			d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
//}
//
//void JetCollectionSettingsButton::updateTrackTubeRadius()
//{
//	if (d->last_trackTubeRadius==trackTubeRadius()) return;
//	d->last_trackTubeRadius=trackTubeRadius();
//	messageVerbose("TrackTubeRadius changed to "+str(d->last_trackTubeRadius));
//	emit trackTubeRadiusChanged(d->last_trackTubeRadius);
//	return;
//}
//
//void JetCollectionSettingsButton::updateTrackDrawStyle()
//{
//	double val = VP1QtInventorUtils::getValueLineWidthSlider(d->editwindow_ui.horizontalSlider_trackWidth);
//	if (d->trackDrawStyle->lineWidth.getValue()!=val)
//		d->trackDrawStyle->lineWidth = val;
//}
//
//void JetCollectionSettingsButton::updateTrackLightModel(bool base)
//{
//	if (d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
//		messageVerbose("TrackLightModel changed (base = "+str(base));
//		if (base)
//			d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
//		else
//			d->trackLightModel->model.setValue(SoLightModel::PHONG);
//	}
//}
//
//void JetCollectionSettingsButton::updateHideActualTrackPath(bool hide)
//{
//	emit hideActualTrackPathChanged(hide);
//}
//
//
//SoDrawStyle * JetCollectionSettingsButton::trackDrawStyle() const
//{
//	return d->trackDrawStyle;
//}
//
//SoLightModel * JetCollectionSettingsButton::trackLightModel() const
//{
//	return d->trackLightModel;
//}
//
//SoMaterial* JetCollectionSettingsButton::defaultParameterMaterial() const {
//	return d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
//}
//SoMaterial* JetCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition position) const{
//	return (d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
//}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // d->defaultParametersMatButton->setObjectName("matButton_default");
//     // d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     d->defaultParametersMatButton->setMaterial(mat);
//     // connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void JetCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

//bool  JetCollectionSettingsButton::hideActualTrackPath() const
//{
//	if (!d->editwindow)
//		d->initEditWindow();
//	return d->editwindow_ui.checkBox_hideactualpaths->isChecked();
//}
//
//bool JetCollectionSettingsButton::showParameters() const
//{
//	if (!d->editwindow)
//		d->initEditWindow();
//	messageVerbose("JetCollectionSettingsButton::showParameters returning  "+str(d->editwindow_ui.checkBox_showparameters->isChecked()));
//	return d->editwindow_ui.checkBox_showparameters->isChecked();
//}
//
//bool JetCollectionSettingsButton::colourParametersByType() const
//{
//	if (!d->editwindow)
//		d->initEditWindow();
//	messageVerbose("JetCollectionSettingsButton::colourParametersByType returning  "+str(d->editwindow_ui.checkBox_parameters_colourByType->isChecked()));
//	return d->editwindow_ui.checkBox_parameters_colourByType->isChecked();
//}
//
//
//bool  JetCollectionSettingsButton::cutOnlyVertexAssocTracks() const
//{
//	if (!d->editwindow)
//		d->initEditWindow();
//	return d->editwindow_ui.checkBox_vertexAssociated->isChecked();
//}

//____________________________________________________________________
void JetCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		d->dragStartPosition = event->pos();
	QPushButton::mousePressEvent(event);
}

//____________________________________________________________________
void JetCollectionSettingsButton::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->source()!=this && event->mimeData()->hasFormat("vp1/material"))
		event->acceptProposedAction();
}

//____________________________________________________________________
void JetCollectionSettingsButton::mouseMoveEvent(QMouseEvent *event)
{
	if (!(event->buttons() & Qt::LeftButton))
		return;
	if ((event->pos() - d->dragStartPosition).manhattanLength()
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
	// QString str_ambient = d->toSbColTxt(d->lastapplied_ambient);
	// if (str_ambient!="SbColor(0.2,0.2,0.2)")
	//   s += "mat->ambientColor.setValue("+str_ambient+");\n";
	// QString str_diffuse = d->toSbColTxt(d->lastapplied_diffuse);
	// if (str_diffuse!="SbColor(0.8,0.8,0.8)")
	//   s += "mat->diffuseColor.setValue("+str_diffuse+");\n";
	// QString str_specular = d->toSbColTxt(d->lastapplied_specular);
	// if (str_specular!="SbColor(0,0,0)")
	//   s += "mat->specularColor.setValue("+str_specular+");\n";
	// QString str_emissive = d->toSbColTxt(d->lastapplied_emissive);
	// if (str_emissive!="SbColor(0,0,0)")
	//   s += "mat->emissiveColor.setValue("+str_emissive+");\n";
	// QString str_shininess = d->printFloat(d->lastapplied_shininess/100.0);
	// if (str_shininess!="0.2")
	//   s +=     "mat->shininess.setValue("+str_shininess+");\n";
	// QString str_transparency = d->printFloat(d->lastapplied_transparency/100.0);
	// if (str_transparency!="0")
	//   s +=     "mat->transparency.setValue("+str_transparency+");\n";
	// mimeData->setText(s);

	//Execute drag:
	drag->setMimeData(mimeData);//drag assumes ownership of mimeData
	drag->exec(Qt::CopyAction | Qt::MoveAction);
}

//____________________________________________________________________
void JetCollectionSettingsButton::dropEvent(QDropEvent *event)
{
	QByteArray data = event->mimeData()->data("vp1/material");
	event->acceptProposedAction();
	restoreFromState(data);
}


QByteArray JetCollectionSettingsButton::saveState() const{

	messageDebug("JetCollectionSettingsButton::saveState()");

	// messageVerbose("getState");
	// if (d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
	VP1Serialise serialise(1/*version*/);


	serialise.save(d->matButton);
	//serialise.disableUnsavedChecks(); // TODO: what this does??

	// COMMON SETTINGS
	serialise.save(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
	serialise.save(d->editwindow_ui.checkBox_cut_minpt);
	serialise.save(d->editwindow_ui.doubleSpinBox_cut_minpt_gev);
	serialise.save(d->editwindow_ui.checkBox_cut_maxpt);
	serialise.save(d->editwindow_ui.doubleSpinBox_cut_maxpt_gev);
	serialise.save(d->editwindow_ui.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	serialise.save(d->editwindow_ui.etaPhiCutWidget);

	// JET SETTINGS
	serialise.save(d->ui_disp.checkBox_randomColours);
	serialise.save(d->ui_disp.doubleSpinBox_lengthOf100GeV);
	serialise.save(d->ui_disp.checkBox_maxR);
	serialise.save(d->ui_disp.doubleSpinBox_maxR);

	serialise.save(d->ui_disp.groupBox_IDprojection);
	serialise.save(d->ui_disp.checkBox_radialScale);
	serialise.save(d->ui_disp.doubleSpinBox_radialScale);

	serialise.save(d->ui_disp.bTaggingCheckBox);
	serialise.save(d->ui_disp.bTaggingComboBox);
	serialise.save(d->ui_disp.bTaggingSpinBox);
	serialise.save(d->ui_disp.radioButton_material, d->ui_disp.radioButton_skins);
	serialise.save(d->ui_disp.matButton_btaggedJets);
	serialise.save(d->ui_disp.skinsComboBox);

	// Parameters
//	serialise.save(d->editwindow_ui.checkBox_showparameters);
//	serialise.save(d->editwindow_ui.checkBox_parameters_colourByType);

	serialise.widgetHandled(this);
	serialise.warnUnsaved(this);
	return serialise.result();
}

void JetCollectionSettingsButton::restoreFromState( const QByteArray& ba){

	messageDebug("JetCollectionSettingsButton::restoreFromState()");

	VP1Deserialise state(ba,systemBase());
	if (state.version()<0||state.version()>1) {
		messageDebug("restoreFromState() - ignoring...");
		return;//Ignore silently
	}
	state.restore(d->matButton);


	// COMMON SETTINGS
	state.restore(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
	state.restore(d->editwindow_ui.checkBox_cut_minpt);
	state.restore(d->editwindow_ui.doubleSpinBox_cut_minpt_gev);
	state.restore(d->editwindow_ui.checkBox_cut_maxpt);
	state.restore(d->editwindow_ui.doubleSpinBox_cut_maxpt_gev);
	state.restore(d->editwindow_ui.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	state.restore(d->editwindow_ui.etaPhiCutWidget);

	// JET SETTINGS
	state.restore(d->ui_disp.checkBox_randomColours);
	state.restore(d->ui_disp.doubleSpinBox_lengthOf100GeV);
	state.restore(d->ui_disp.checkBox_maxR);
	state.restore(d->ui_disp.doubleSpinBox_maxR);

	state.restore(d->ui_disp.groupBox_IDprojection);
	state.restore(d->ui_disp.checkBox_radialScale);
	state.restore(d->ui_disp.doubleSpinBox_radialScale);

	state.restore(d->ui_disp.bTaggingCheckBox);
	state.restore(d->ui_disp.bTaggingComboBox);
	state.restore(d->ui_disp.bTaggingSpinBox);
	state.restore(d->ui_disp.radioButton_material, d->ui_disp.radioButton_skins);
	state.restore(d->ui_disp.matButton_btaggedJets);
	state.restore(d->ui_disp.skinsComboBox);


	//	// Parameters
//	state.restore(d->editwindow_ui.checkBox_showparameters);
//	state.restore(d->editwindow_ui.checkBox_parameters_colourByType);

	state.widgetHandled(this);
	state.warnUnrestored(this);

//	updateTrackTubeRadius();
//	updateTrackLightModel(d->editwindow_ui.checkBox_tracksUseBaseLightModel);

	updateButton();

	// after restoring the state, check if b-tagging checkbox is enabled,
	messageDebug("updating b-tagging status for collection " + d->name + "...");
	if (d->ui_disp.bTaggingCheckBox->isChecked()) {
		possibleChange_bTaggingEnabled(true); // init the b-tagging toolbox as active

	}
	else
		possibleChange_bTaggingEnabled(false); // init the b-tagging toolbox as not-active


	//FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval JetCollectionSettingsButton::cutAllowedPt() const
{
	if (!d->editwindow)
		d->initEditWindow();
	if (!d->editwindow_ui.checkBox_cut_minpt)
		return VP1Interval();

	// will set range to negative if we have momcut=P
	// if minCut unset then min=-inf
	// if minCut set, and Pt selected, then min=-minCut
	// if minCut set, and P selected, then min=-maxCut
	// etc
	bool isPCut = d->editwindow_ui.comboBox_momtype->currentText()=="P";

	const double minFromInterface=d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*1000;
	const double maxFromInterface=d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*1000;

	double min=0.0,max=0.0;
	if (!isPCut) {
		//Pt cut
		min = (d->editwindow_ui.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
		max = (d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
	} else {
		min = (d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
		max = (d->editwindow_ui.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
	}

	//message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));

	if (max<min)
		return VP1Interval();

	return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval JetCollectionSettingsButton::cutAllowedEta() const
{
	if (!d->editwindow)
		d->initEditWindow();
	return d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> JetCollectionSettingsButton::cutAllowedPhi() const
{
	if (!d->editwindow)
		d->initEditWindow();
	return d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}



void JetCollectionSettingsButton::possibleChange_cutAllowedPt()
{
	messageVerbose("possibleChange_cutAllowedPt() ");

	if (d->last_cutAllowedPt==cutAllowedPt()) return;
	messageVerbose("cutAllowedPt() changed");
	d->last_cutAllowedPt= cutAllowedPt();
	emit cutAllowedPtChanged(d->last_cutAllowedPt);
}

void JetCollectionSettingsButton::possibleChange_cutAllowedEta()
{
	if (d->last_cutAllowedEta==cutAllowedEta()) return;
	messageVerbose("cutAllowedEta() changed");
	d->last_cutAllowedEta=cutAllowedEta();
	emit cutAllowedEtaChanged(d->last_cutAllowedEta);
}

void JetCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
	if (d->last_cutAllowedPhi==cutAllowedPhi()) return;
	messageVerbose("cutAllowedPhi() changed");
	d->last_cutAllowedPhi=cutAllowedPhi();
	emit cutAllowedPhiChanged(d->last_cutAllowedPhi);
}

void JetCollectionSettingsButton::possibleChange_scale()
{
	messageVerbose("possibleChange_scale() ");

	if (d->last_scale == lengthOf100GeV()) return;

	messageVerbose("lengthOf100GeV changed");
	d->last_scale = lengthOf100GeV();
	emit scaleChanged(d->last_scale);
}


void JetCollectionSettingsButton::possibleChange_randomJetColours()
{
	messageVerbose("possibleChange_randomJetColours() ");

	if (d->last_randomJetColours == randomJetColours() ) return;
	messageVerbose("checkBox_randomColours changed");
	d->last_randomJetColours = randomJetColours();
	emit randomJetColoursChanged(d->last_randomJetColours);
}


//____________________________________________________________________
void JetCollectionSettingsButton::enableRandomColours(bool bb)
{
	d->ui_disp.pushButton_colourbyrandom_rerandomise->setEnabled(bb);
}

//____________________________________________________________________
bool JetCollectionSettingsButton::randomJetColours() const
{
  return d->ui_disp.checkBox_randomColours->isChecked();
}


//____________________________________________________________________
void JetCollectionSettingsButton::emitRerandomise()
{
  messageVerbose("JetCollectionSettingsButton - Emitting rerandomise");
  emit rerandomise();
}

////____________________________________________________________________
//void JetCollectionSettingsButton::emitMaxR()
//{
//  messageVerbose("JetCollectionSettingsButton - Emitting signalMaxR");
//  emit signalMaxR();
//}

//____________________________________________________________________
void JetCollectionSettingsButton::enableMaxR(bool bb)
{
	d->ui_disp.doubleSpinBox_maxR->setEnabled(bb);
}

//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_maxR()
{
	messageVerbose("possibleChange_maxR() ");

	if ( !  d->ui_disp.checkBox_maxR->isChecked() ) {
		messageVerbose("maxR unchecked --> setting maxR=0.0 and returning");
		emit maxRChanged(0.0); // setting marR=0.0 disables the maxR option in handleJet
		return;
	}

	messageVerbose("setting maxR");
	d->last_maxR = maxR();
	emit maxRChanged(d->last_maxR);
}

//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingEnabled(bool bb)
{
	messageVerbose("possibleChange_bTaggingEnabled() - "+str(bb));

	d->ui_disp.bTaggingAlgLabel->setEnabled(bb);
	d->ui_disp.bTagginWeightCutLabel->setEnabled(bb);
	d->ui_disp.bTaggingComboBox->setEnabled(bb);
	d->ui_disp.bTaggingSpinBox->setEnabled(bb);
	d->ui_disp.groupBox_btagging_render->setEnabled(bb);
	d->ui_disp.matButton_btaggedJets->setEnabled(bb);
	emit bTaggingEnabledChanged(bb);
}

//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingTagger()
{
	messageVerbose("possibleChange_bTaggingTagger() - " + bTaggingTagger() );
	emit bTaggingTaggerChanged( bTaggingTagger() );
}
//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingCut()
{
	messageVerbose("possibleChange_bTaggingCut() - " + QString::number(bTaggingCut()) );

	if (d->last_bTaggingCut == bTaggingCut() ) return;

	messageVerbose("bTaggingSpinBox changed");
	d->last_bTaggingCut = bTaggingCut();

	emit bTaggingCutChanged( bTaggingCut() );
}


// called when user clicks on "Material" checkbox
//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingRenderingMaterial(bool ok)
{
	messageVerbose("possibleChange_bTaggingRenderingMaterial()");
	messageVerbose("radioButton_material changed - " + QString::number(ok) );

	emit bTaggingRenderingMaterialChanged(ok);
}

// not needed...
//// called when b-tagging material changes
////____________________________________________________________________
//void JetCollectionSettingsButton::possibleChange_bTaggingMaterial()
//{
//	messageVerbose("possibleChange_bTaggingMaterial()");
//
//	if (d->last_bTaggingMaterial == bTaggingMaterial() ) {
//		messageVerbose("material not changed. Returning...");
//		return;
//	}
//
//	messageVerbose("matButton_btaggedJets changed");
//	d->last_bTaggingMaterial = bTaggingMaterial();
//
//	emit bTaggingMaterialChanged();
//}




// called when user clicks on "Skin" checkbox
//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingRenderingSkin(bool ok)
{
	messageVerbose("possibleChange_bTaggingRenderingSkin()");
	messageVerbose("radioButton_skins changed - " + QString::number(ok) );

	emit bTaggingRenderingSkinChanged(ok);
}
