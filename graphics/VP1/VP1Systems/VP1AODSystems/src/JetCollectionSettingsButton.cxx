/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "VP1AODSystems/JetCollectionSettingsButton.h"
// UIs
#include "ui_periparticlecollectionsettings_form.h"
#include "ui_jet_settings_display_form.h"

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
	SoMaterial * createMaterial(int r, int g, int b) const;
	SoMaterial * createMaterial(int r, int g, int b, float brightness, float transparency) const;


	int dim;
	QString name;
	QPoint dragStartPosition;

	void initEditWindow();
};


//____________________________________________________________________
SoMaterial * JetCollectionSettingsButton::Imp::createMaterial(int r, int g, int b) const
{
  return VP1MaterialButton::createMaterial(r/255.0,g/255.0,b/255.0,0.2/*brightness*/);
}
//____________________________________________________________________
SoMaterial * JetCollectionSettingsButton::Imp::createMaterial(int r, int g, int b,
                                                              float brightness,
                                                              float transparency) const
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
	if (m_d->editwindow)
		m_d->editwindow->setWindowTitle(t);
	setToolTip(t);
}


//____________________________________________________________________
double JetCollectionSettingsButton::lengthOf100GeV()
{
	double val = m_d->ui_disp.doubleSpinBox_lengthOf100GeV->value();
	VP1Msg::messageVerbose("JetCollectionSettingsButton::lengthOf100GeV() - value: " + QString::number(val));
	return val;
}

//____________________________________________________________________
double JetCollectionSettingsButton::maxR()
{
	double val = m_d->ui_disp.checkBox_maxR->isChecked() ? (m_d->ui_disp.doubleSpinBox_maxR->value() ) : -1; //m_d->ui_disp.doubleSpinBox_maxR->value();
	VP1Msg::messageVerbose("JetCollectionSettingsButton::maxR() - value: " + QString::number(val));
	return val;
}


//____________________________________________________________________
double JetCollectionSettingsButton::bTaggingCut() const {
  return m_d->ui_disp.bTaggingSpinBox->value();
}
//____________________________________________________________________
QString JetCollectionSettingsButton::bTaggingTagger() const {
  return m_d->ui_disp.bTaggingComboBox->currentText();
}
//____________________________________________________________________
QString JetCollectionSettingsButton::bTaggingSkin() const {
  return m_d->ui_disp.skinsComboBox->currentText();
}
//____________________________________________________________________
SoMaterial* JetCollectionSettingsButton::bTaggingMaterial() const {
  return getMaterial(m_d->ui_disp.matButton_btaggedJets);
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingEnabled() const {
  return m_d->ui_disp.bTaggingCheckBox->isChecked();
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingRenderingSkin() const {
  return m_d->ui_disp.radioButton_skins->isChecked();
}
//____________________________________________________________________
bool JetCollectionSettingsButton::bTaggingRenderingMaterial() const {
  return m_d->ui_disp.radioButton_material->isChecked();
}

//____________________________________________________________________
bool JetCollectionSettingsButton::is_bTaggingSkinEnabled() const
{
	return m_d->ui_disp.radioButton_skins->isChecked();
}

//____________________________________________________________________
bool JetCollectionSettingsButton::is_bTaggingMaterialEnabled() const
{
	return m_d->ui_disp.radioButton_material->isChecked();
}





//____________________________________________________________________
JetCollectionSettingsButton::JetCollectionSettingsButton(QWidget * parent,int dim, QString name)
: VP1CollectionSettingsButtonBase(parent,0), m_d(new Imp)
//: VP1CollectionSettingsButtonBase(parent,0,"VP1MaterialButton"), m_d(new Imp)
{
	m_d->dim = dim;
	m_d->name = name;

	m_d->theclass = this;
	m_d->initEditWindow();


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////



//	//Draw Styles / Complexity:
//	VP1QtInventorUtils::setLimitsLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
//	VP1QtInventorUtils::setValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth,1.0);

//	m_d->trackDrawStyle = new SoDrawStyle;
//	m_d->trackDrawStyle->setName("JetDrawStyle");
//	m_d->trackDrawStyle->pointSize=5.0;
//	m_d->trackDrawStyle->ref();
//	updateTrackDrawStyle();
//	connect(m_d->editwindow_ui.horizontalSlider_trackWidth,SIGNAL(valueChanged(int)),this,SLOT(updateTrackDrawStyle()));

//	m_d->trackLightModel = new SoLightModel;
//	m_d->trackLightModel->setName("JetLightModel");
//	m_d->trackLightModel->ref();
//	updateTrackLightModel(false);
//	connect(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel,SIGNAL(toggled(bool)),this,SLOT(updateTrackLightModel(bool)));
//
//	m_d->last_trackTubeRadius=trackTubeRadius();
//	connect(m_d->editwindow_ui.checkBox_trackTubes,SIGNAL(toggled(bool)),this,SLOT(updateTrackTubeRadius()));
//	connect(m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM,SIGNAL(valueChanged(double)),this,SLOT(updateTrackTubeRadius()));
//
//	connect(m_d->editwindow_ui.checkBox_hideactualpaths,SIGNAL(toggled(bool)),this,SLOT(updateHideActualTrackPath(bool)));

//	// -> parameters
//	connect(m_d->editwindow_ui.checkBox_showparameters,SIGNAL(toggled(bool)),this,SLOT(possibleChange_showParameters()));
//	connect(m_d->editwindow_ui.checkBox_parameters_colourByType,SIGNAL(toggled(bool)),this,SLOT(possibleChange_parameterColours()));


	// -> scale
	connect(m_d->ui_disp.doubleSpinBox_lengthOf100GeV,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_scale()));

	// -> cutAllowedP/Pt
	connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->editwindow_ui.checkBox_cut_maxpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev,SIGNAL(valueChanged(double)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->editwindow_ui.checkBox_cut_minpt,SIGNAL(toggled(bool)),this,SLOT(possibleChange_cutAllowedPt()));
	connect(m_d->editwindow_ui.comboBox_momtype,SIGNAL(currentIndexChanged(int)),this,SLOT(possibleChange_cutAllowedPt()));

	// -> cutAllowedEta
	connect(m_d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)),this,SLOT(possibleChange_cutAllowedEta()));

	// -> cutAllowedPhi
	connect(m_d->editwindow_ui.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)),this,SLOT(possibleChange_cutAllowedPhi()));

	// random colors
	connect(m_d->ui_disp.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(enableRandomColours(bool)));
	connect(m_d->ui_disp.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(possibleChange_randomJetColours()));
	connect(m_d->ui_disp.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));
	this->enableRandomColours(false);

	// maxR
	connect(m_d->ui_disp.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(enableMaxR(bool)));
	connect(m_d->ui_disp.checkBox_maxR, SIGNAL(toggled(bool)), this, SLOT(possibleChange_maxR()));
	connect(m_d->ui_disp.doubleSpinBox_maxR, SIGNAL(valueChanged(double)), this, SLOT(possibleChange_maxR()));
	this->enableMaxR(false); // init

	// BTagging
	connect(m_d->ui_disp.bTaggingCheckBox, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingEnabled(bool)) );

	connect(m_d->ui_disp.bTaggingComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(possibleChange_bTaggingTagger()) );

	connect(m_d->ui_disp.bTaggingSpinBox, SIGNAL(valueChanged(double)), this, SLOT(possibleChange_bTaggingCut()) );

	connect(m_d->ui_disp.radioButton_material, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingRenderingMaterial(bool)) );

	// not needed...
	//connect(m_d->ui_disp.matButton_btaggedJets, SIGNAL(lastAppliedChanged()), this, SLOT(possibleChange_bTaggingMaterial()) );

	connect(m_d->ui_disp.radioButton_skins, SIGNAL(toggled(bool)), this, SLOT(possibleChange_bTaggingRenderingSkin(bool)) );


	// Material button
	connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(m_d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
	connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
	setAcceptDrops(true);

	QTimer::singleShot(0, this, SLOT(updateButton()));

	////////////////////////
	//  Custom Materials  //
	////////////////////////

	std::cout << "setting jet custom materials..." << std::endl;



	// create default material for b-jets
	m_d->materialFallback = m_d->createMaterial(116,255,228, 0.2, 0.3); // light greenish-blue (R,G,B, brightness, transparency)
	m_d->materialFallback->ref();



	// init material for b-tagged jet
	m_d->ui_disp.matButton_btaggedJets->setMaterial(m_d->materialFallback);

//	updateButton();


}

// QWidget& JetCollectionSettingsButton::editWindow() {
//   if (!m_d->editwindow)
//     initEditWindow();
//   return *(m_d->editwindow);
// } 
JetCollectionSettingsButton::~JetCollectionSettingsButton()
{
	delete m_d->editwindow;
//	m_d->trackDrawStyle->unref();
//	m_d->trackLightModel->unref();
	delete m_d;
}

void JetCollectionSettingsButton::updateButton()
{
	if (objectName().isEmpty())
		setObjectName("JetCollectionSettingsButton");
	messageVerbose("jet - setColButtonProperties: color=" + str(m_d->matButton->lastAppliedDiffuseColour()));
	VP1ColorSelectButton::setColButtonProperties(this,m_d->matButton->lastAppliedDiffuseColour(),m_d->dim);
}

void JetCollectionSettingsButton::setDimension(int dim)
{
	if (m_d->dim == dim)
		return;
	m_d->dim = dim;
	updateButton();
}

void JetCollectionSettingsButton::showEditMaterialDialog()
{
	if (!m_d->editwindow)
		m_d->initEditWindow();

	if (m_d->editwindow->isHidden())
		m_d->editwindow->show();
	else
		m_d->editwindow->hide();
}

bool JetCollectionSettingsButton::setMaterial(SoMaterial*mat)
{  
	// std::cout<<"JetCollectionSettingsButton::setMaterial with mat="<<mat<<std::endl;
	if (!m_d->matButton) m_d->initEditWindow();
	m_d->matButton->setMaterial(mat);
	return true;
}

VP1MaterialButton* JetCollectionSettingsButton::getMaterialButton() const
{
	return m_d->matButton;
}

void JetCollectionSettingsButton::copyValuesFromMaterial(SoMaterial*mat)
{
	if (!m_d->matButton) m_d->initEditWindow();
	m_d->matButton->setMaterial(mat);
}
double JetCollectionSettingsButton::lastAppliedTransparency() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedTransparency();
}
double JetCollectionSettingsButton::lastAppliedShininess() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedShininess();
}
double JetCollectionSettingsButton::lastAppliedBrightness() const
{
	if (!m_d->matButton) m_d->initEditWindow();
	return m_d->matButton->lastAppliedBrightness();
}

//double JetCollectionSettingsButton::trackTubeRadius() const
//{
//	return m_d->editwindow_ui.checkBox_trackTubes->isChecked() ?
//			m_d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM->value() : 0.0;
//}
//
//void JetCollectionSettingsButton::updateTrackTubeRadius()
//{
//	if (m_d->last_trackTubeRadius==trackTubeRadius()) return;
//	m_d->last_trackTubeRadius=trackTubeRadius();
//	messageVerbose("TrackTubeRadius changed to "+str(m_d->last_trackTubeRadius));
//	emit trackTubeRadiusChanged(m_d->last_trackTubeRadius);
//	return;
//}
//
//void JetCollectionSettingsButton::updateTrackDrawStyle()
//{
//	double val = VP1QtInventorUtils::getValueLineWidthSlider(m_d->editwindow_ui.horizontalSlider_trackWidth);
//	if (m_d->trackDrawStyle->lineWidth.getValue()!=val)
//		m_d->trackDrawStyle->lineWidth = val;
//}
//
//void JetCollectionSettingsButton::updateTrackLightModel(bool base)
//{
//	if (m_d->trackLightModel->model.getValue()!=(base?SoLightModel::BASE_COLOR:SoLightModel::PHONG)) {
//		messageVerbose("TrackLightModel changed (base = "+str(base));
//		if (base)
//			m_d->trackLightModel->model.setValue(SoLightModel::BASE_COLOR);
//		else
//			m_d->trackLightModel->model.setValue(SoLightModel::PHONG);
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
//	return m_d->trackDrawStyle;
//}
//
//SoLightModel * JetCollectionSettingsButton::trackLightModel() const
//{
//	return m_d->trackLightModel;
//}
//
//SoMaterial* JetCollectionSettingsButton::defaultParameterMaterial() const {
//	return m_d->defaultParametersMatButton->handledMaterials().at(0);// Only have one material per button.
//}
//SoMaterial* JetCollectionSettingsButton::parameterMaterial( xAOD::ParameterPosition position) const{
//	return (m_d->parameterTypeMatButtons.at(static_cast<unsigned int>(position)))->handledMaterials().at(0);// Only have one material per button.
//}

// void setDefaultParameterMaterial(SoMaterial* mat) {    
//     // m_d->defaultParametersMatButton->setObjectName("matButton_default");
//     // m_d->defaultParametersMatButton->setToolTip(matButtonToolTip());
//     m_d->defaultParametersMatButton->setMaterial(mat);
//     // connect(m_d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(possibleChangeMatTranspOrBrightness()));
// }
// 
// void JetCollectionSettingsButton::setParameterMaterial(SoMaterial*, xAOD::ParameterPosition){
//   // FIXME!
// }

//bool  JetCollectionSettingsButton::hideActualTrackPath() const
//{
//	if (!m_d->editwindow)
//		m_d->initEditWindow();
//	return m_d->editwindow_ui.checkBox_hideactualpaths->isChecked();
//}
//
//bool JetCollectionSettingsButton::showParameters() const
//{
//	if (!m_d->editwindow)
//		m_d->initEditWindow();
//	messageVerbose("JetCollectionSettingsButton::showParameters returning  "+str(m_d->editwindow_ui.checkBox_showparameters->isChecked()));
//	return m_d->editwindow_ui.checkBox_showparameters->isChecked();
//}
//
//bool JetCollectionSettingsButton::colourParametersByType() const
//{
//	if (!m_d->editwindow)
//		m_d->initEditWindow();
//	messageVerbose("JetCollectionSettingsButton::colourParametersByType returning  "+str(m_d->editwindow_ui.checkBox_parameters_colourByType->isChecked()));
//	return m_d->editwindow_ui.checkBox_parameters_colourByType->isChecked();
//}
//
//
//bool  JetCollectionSettingsButton::cutOnlyVertexAssocTracks() const
//{
//	if (!m_d->editwindow)
//		m_d->initEditWindow();
//	return m_d->editwindow_ui.checkBox_vertexAssociated->isChecked();
//}

//____________________________________________________________________
void JetCollectionSettingsButton::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		m_d->dragStartPosition = event->pos();
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
void JetCollectionSettingsButton::dropEvent(QDropEvent *event)
{
	QByteArray data = event->mimeData()->data("vp1/material");
	event->acceptProposedAction();
	restoreFromState(data);
}


QByteArray JetCollectionSettingsButton::saveState() const{

	messageDebug("JetCollectionSettingsButton::saveState()");

	// messageVerbose("getState");
	// if (m_d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
	VP1Serialise serialise(1/*version*/);


	serialise.save(m_d->matButton);
	//serialise.disableUnsavedChecks(); // TODO: what this does??

	// COMMON SETTINGS
	serialise.save(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
	serialise.save(m_d->editwindow_ui.checkBox_cut_minpt);
	serialise.save(m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev);
	serialise.save(m_d->editwindow_ui.checkBox_cut_maxpt);
	serialise.save(m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev);
	serialise.save(m_d->editwindow_ui.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	serialise.save(m_d->editwindow_ui.etaPhiCutWidget);

	// JET SETTINGS
	serialise.save(m_d->ui_disp.checkBox_randomColours);
	serialise.save(m_d->ui_disp.doubleSpinBox_lengthOf100GeV);
	serialise.save(m_d->ui_disp.checkBox_maxR);
	serialise.save(m_d->ui_disp.doubleSpinBox_maxR);

	serialise.save(m_d->ui_disp.groupBox_IDprojection);
	serialise.save(m_d->ui_disp.checkBox_radialScale);
	serialise.save(m_d->ui_disp.doubleSpinBox_radialScale);

	serialise.save(m_d->ui_disp.bTaggingCheckBox);
	serialise.save(m_d->ui_disp.bTaggingComboBox);
	serialise.save(m_d->ui_disp.bTaggingSpinBox);
	serialise.save(m_d->ui_disp.radioButton_material, m_d->ui_disp.radioButton_skins);
	serialise.save(m_d->ui_disp.matButton_btaggedJets);
	serialise.save(m_d->ui_disp.skinsComboBox);

	// Parameters
//	serialise.save(m_d->editwindow_ui.checkBox_showparameters);
//	serialise.save(m_d->editwindow_ui.checkBox_parameters_colourByType);

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
	state.restore(m_d->matButton);


	// COMMON SETTINGS
	state.restore(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);
	state.restore(m_d->editwindow_ui.checkBox_cut_minpt);
	state.restore(m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev);
	state.restore(m_d->editwindow_ui.checkBox_cut_maxpt);
	state.restore(m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev);
	state.restore(m_d->editwindow_ui.comboBox_momtype);

	// ETA-PHI CUTS (from VP1Base/VP1EtaPhiCutWidget.cxx)
	state.restore(m_d->editwindow_ui.etaPhiCutWidget);

	// JET SETTINGS
	state.restore(m_d->ui_disp.checkBox_randomColours);
	state.restore(m_d->ui_disp.doubleSpinBox_lengthOf100GeV);
	state.restore(m_d->ui_disp.checkBox_maxR);
	state.restore(m_d->ui_disp.doubleSpinBox_maxR);

	state.restore(m_d->ui_disp.groupBox_IDprojection);
	state.restore(m_d->ui_disp.checkBox_radialScale);
	state.restore(m_d->ui_disp.doubleSpinBox_radialScale);

	state.restore(m_d->ui_disp.bTaggingCheckBox);
	state.restore(m_d->ui_disp.bTaggingComboBox);
	state.restore(m_d->ui_disp.bTaggingSpinBox);
	state.restore(m_d->ui_disp.radioButton_material, m_d->ui_disp.radioButton_skins);
	state.restore(m_d->ui_disp.matButton_btaggedJets);
	state.restore(m_d->ui_disp.skinsComboBox);


	//	// Parameters
//	state.restore(m_d->editwindow_ui.checkBox_showparameters);
//	state.restore(m_d->editwindow_ui.checkBox_parameters_colourByType);

	state.widgetHandled(this);
	state.warnUnrestored(this);

//	updateTrackTubeRadius();
//	updateTrackLightModel(m_d->editwindow_ui.checkBox_tracksUseBaseLightModel);

	updateButton();

	// after restoring the state, check status of selection cuts
	checkSelection();


}

//____________________________________________________________________
void JetCollectionSettingsButton::checkSelection()
{
	messageDebug("JetCollectionSettingsButton::checkSelection()");

    // -- b-tagging settings/cuts
	messageDebug("updating b-tagging status for collection " + m_d->name + "...");
	if (m_d->ui_disp.bTaggingCheckBox->isChecked()) {
		possibleChange_bTaggingEnabled(true); // init the b-tagging toolbox as active
		possibleChange_bTaggingCut();
		if (m_d->ui_disp.radioButton_material->isChecked())
			possibleChange_bTaggingRenderingMaterial(true);
		else if (m_d->ui_disp.radioButton_skins->isChecked())
			possibleChange_bTaggingRenderingSkin(true);
		possibleChange_bTaggingTagger();
	}
	else
		possibleChange_bTaggingEnabled(false); // init the b-tagging toolbox as not-active

	// -- other cuts
	messageDebug("updating other selection cuts for collection " + m_d->name + "...");
    possibleChange_maxR();
    possibleChange_cutAllowedEta();
    possibleChange_cutAllowedPhi();
    possibleChange_cutAllowedPt();
    possibleChange_scale();
    possibleChange_randomJetColours();
    //FIXME - anything else need updating?
}

//____________________________________________________________________
VP1Interval JetCollectionSettingsButton::cutAllowedPt() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();
	if (!m_d->editwindow_ui.checkBox_cut_minpt)
		return VP1Interval();

	// will set range to negative if we have momcut=P
	// if minCut unset then min=-inf
	// if minCut set, and Pt selected, then min=-minCut
	// if minCut set, and P selected, then min=-maxCut
	// etc
	bool isPCut = m_d->editwindow_ui.comboBox_momtype->currentText()=="P";

	const double minFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_minpt_gev->value()*1000;
	const double maxFromInterface=m_d->editwindow_ui.doubleSpinBox_cut_maxpt_gev->value()*1000;

	double min=0.0,max=0.0;
	if (!isPCut) {
		//Pt cut
		min = (m_d->editwindow_ui.checkBox_cut_minpt->isChecked() ? minFromInterface : -std::numeric_limits<double>::infinity());
		max = (m_d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? maxFromInterface : std::numeric_limits<double>::infinity());
	} else {
		min = (m_d->editwindow_ui.checkBox_cut_maxpt->isChecked() ? -maxFromInterface : -std::numeric_limits<double>::infinity());
		max = (m_d->editwindow_ui.checkBox_cut_minpt->isChecked() ? -minFromInterface : std::numeric_limits<double>::infinity());
	}

	//message("cutAllowedPt: min,max="+QString::number(min)+","+QString::number(max));

	if (max<min)
		return VP1Interval();

	return VP1Interval( min, max );//fixme: closed interval??
}

//____________________________________________________________________
VP1Interval JetCollectionSettingsButton::cutAllowedEta() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();
	return m_d->editwindow_ui.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> JetCollectionSettingsButton::cutAllowedPhi() const
{
	if (!m_d->editwindow)
		m_d->initEditWindow();
	return m_d->editwindow_ui.etaPhiCutWidget->allowedPhi();
}



void JetCollectionSettingsButton::possibleChange_cutAllowedPt()
{
	messageVerbose("possibleChange_cutAllowedPt() ");

	if (m_d->last_cutAllowedPt==cutAllowedPt()) return;
	messageVerbose("cutAllowedPt() changed");
	m_d->last_cutAllowedPt= cutAllowedPt();
	emit cutAllowedPtChanged(m_d->last_cutAllowedPt);
}

void JetCollectionSettingsButton::possibleChange_cutAllowedEta()
{
	if (m_d->last_cutAllowedEta==cutAllowedEta()) return;
	messageVerbose("cutAllowedEta() changed");
	m_d->last_cutAllowedEta=cutAllowedEta();
	emit cutAllowedEtaChanged(m_d->last_cutAllowedEta);
}

void JetCollectionSettingsButton::possibleChange_cutAllowedPhi()
{
	if (m_d->last_cutAllowedPhi==cutAllowedPhi()) return;
	messageVerbose("cutAllowedPhi() changed");
	m_d->last_cutAllowedPhi=cutAllowedPhi();
	emit cutAllowedPhiChanged(m_d->last_cutAllowedPhi);
}

void JetCollectionSettingsButton::possibleChange_scale()
{
	messageVerbose("possibleChange_scale() ");

	if (m_d->last_scale == lengthOf100GeV()) return;

	messageVerbose("lengthOf100GeV changed");
	m_d->last_scale = lengthOf100GeV();
	emit scaleChanged(m_d->last_scale);
}


void JetCollectionSettingsButton::possibleChange_randomJetColours()
{
	messageVerbose("possibleChange_randomJetColours() ");

	if (m_d->last_randomJetColours == randomJetColours() ) return;
	messageVerbose("checkBox_randomColours changed");
	m_d->last_randomJetColours = randomJetColours();
	emit randomJetColoursChanged(m_d->last_randomJetColours);
}


//____________________________________________________________________
void JetCollectionSettingsButton::enableRandomColours(bool bb)
{
	m_d->ui_disp.pushButton_colourbyrandom_rerandomise->setEnabled(bb);
}

//____________________________________________________________________
bool JetCollectionSettingsButton::randomJetColours() const
{
  return m_d->ui_disp.checkBox_randomColours->isChecked();
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
	m_d->ui_disp.doubleSpinBox_maxR->setEnabled(bb);
}

//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_maxR()
{
	messageVerbose("possibleChange_maxR() ");

	if ( !  m_d->ui_disp.checkBox_maxR->isChecked() ) {
		messageVerbose("maxR unchecked --> setting maxR=0.0 and returning");
		emit maxRChanged(0.0); // setting marR=0.0 disables the maxR option in handleJet
		return;
	}

	messageDebug("setting maxR: "+QString::number(maxR()));
	m_d->last_maxR = maxR();
	emit maxRChanged(m_d->last_maxR);
}

//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingEnabled(bool bb)
{
	messageVerbose("possibleChange_bTaggingEnabled() - "+str(bb));

	m_d->ui_disp.bTaggingAlgLabel->setEnabled(bb);
	m_d->ui_disp.bTagginWeightCutLabel->setEnabled(bb);
	m_d->ui_disp.bTaggingComboBox->setEnabled(bb);
	m_d->ui_disp.bTaggingSpinBox->setEnabled(bb);
	m_d->ui_disp.groupBox_btagging_render->setEnabled(bb);
	m_d->ui_disp.matButton_btaggedJets->setEnabled(bb);
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

	if (m_d->last_bTaggingCut == bTaggingCut() ) return;

	messageVerbose("bTaggingSpinBox changed");
	m_d->last_bTaggingCut = bTaggingCut();

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




// called when user clicks on "Skin" checkbox
//____________________________________________________________________
void JetCollectionSettingsButton::possibleChange_bTaggingRenderingSkin(bool ok)
{
	messageVerbose("possibleChange_bTaggingRenderingSkin()");
	messageVerbose("radioButton_skins changed - " + QString::number(ok) );

	emit bTaggingRenderingSkinChanged(ok);
}
