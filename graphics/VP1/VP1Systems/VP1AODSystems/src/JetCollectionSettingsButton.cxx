/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Local
#include "VP1Base/VP1ColorSelectButton.h"
#include "JetCollectionSettingsButton.h"
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

//SoCoin

#include "Inventor/nodes/SoDrawStyle.h"
#include "Inventor/nodes/SoLightModel.h"
#include "Inventor/nodes/SoMaterial.h"

// Qt
#include <QtCore/QTimer>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtGui/QMouseEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QShortcut>

// Misc
#include <iostream>

//____________________________________________________________________
class JetCollectionSettingsButton::Imp {
public:

//	Imp():theclass(0),editwindow(0),matButton(0), trackDrawStyle(0), trackLightModel(0){}
	Imp():theclass(0),editwindow(0),matButton(0) {}

	// the main class
	JetCollectionSettingsButton * theclass;

	// setup the UI
	QWidget * editwindow; // the main window
	Ui::IParticleCollectionSettingsForm editwindow_ui; // the main UI window
	Ui::JetSysSettingsDisplayForm ui_disp; // the custom "display" jet cuts


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
//	double last_trackTubeRadius;
//	QList<unsigned> last_cutRequiredNHits;
//	bool last_cutOnlyVertexAssocTracks;

	int dim;
	QPoint dragStartPosition;

	void initEditWindow();
};

//____________________________________________________________________
void JetCollectionSettingsButton::Imp::initEditWindow()
{
	if (editwindow)
		return;
	theclass->messageVerbose("Initialising material editor dialog");

	// create a parent widget
	editwindow = new QWidget(0,Qt::WindowStaysOnTopHint); // parent widget

	// init UI
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

	// compose UI: adding to the default GUI the "display" custom cuts for jets
	editwindow_ui.verticalLayout_additional_widgets->layout()->addWidget( ui_disp.groupBox_jet_display );

	// get a handle on the material button
	matButton = editwindow_ui.pushButton_matButton;


//	defaultParametersMatButton = editwindow_ui.matButton_parametersDefaultColour;
//	SoMaterial* defParamMat = new SoMaterial; // Default (for the moment)
//	defaultParametersMatButton->setMaterial(defParamMat);
//	editwindow_ui.frame_parameterTypeColours->hide();

	//  // Set the default colours for the "colour by parameter type" section
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_2);
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_3);
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_4);
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_5);
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_6);
	//  parameterTypeMatButtons.append(editwindow_ui.pushButton_matButton_parameters_default_colour_7);
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
JetCollectionSettingsButton::JetCollectionSettingsButton(QWidget * parent,int _dim)
: VP1MaterialButtonBase(parent,0,"VP1MaterialButton"), d(new Imp)
{
	d->dim = _dim;

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
	connect(d->ui_disp.checkBox_randomColours, SIGNAL(toggled(bool)), this, SLOT(possibleChange_randomJetColours()));
	connect(d->ui_disp.pushButton_colourbyrandom_rerandomise,SIGNAL(clicked()),this,SLOT(emitRerandomise()));


	connect(this,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(d->editwindow_ui.pushButton_close,SIGNAL(clicked()),this,SLOT(showEditMaterialDialog()));
	connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SLOT(updateButton()));
	connect(d->matButton,SIGNAL(lastAppliedChanged()),this,SIGNAL(lastAppliedChanged()));
	setAcceptDrops(true);

	QTimer::singleShot(0, this, SLOT(updateButton()));

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
	messageVerbose("setColButtonProperties: color=" + str(d->matButton->lastAppliedDiffuseColour()));
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
	// messageVerbose("getState");
	// if (d->editwindow_ui.checkBox_tracksUseBaseLightModel->isChecked()) messageVerbose("checked!");
	VP1Serialise serialise(1/*version*/);

	serialise.save(d->matButton);
	// serialise.disableUnsavedChecks();

	//	serialise.save(d->editwindow_ui.horizontalSlider_trackWidth);
	//serialise.save(d->editwindow_ui.checkBox_trackTubes);
	//serialise.save(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
	//serialise.save(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
	//serialise.save(d->editwindow_ui.checkBox_hideactualpaths);

	// Parameters
//	serialise.save(d->editwindow_ui.checkBox_showparameters);
//	serialise.save(d->editwindow_ui.checkBox_parameters_colourByType);

	serialise.widgetHandled(this);
	serialise.warnUnsaved(this);
	return serialise.result();
}

void JetCollectionSettingsButton::restoreFromState( const QByteArray& ba){

	VP1Deserialise state(ba,systemBase());
	if (state.version()<0||state.version()>1)
		return;//Ignore silently
	state.restore(d->matButton);

//	state.restore(d->editwindow_ui.horizontalSlider_trackWidth);
//	state.restore(d->editwindow_ui.checkBox_trackTubes);
//	state.restore(d->editwindow_ui.doubleSpinBox_trackTubesRadiusMM);
//	state.restore(d->editwindow_ui.checkBox_tracksUseBaseLightModel);
//	state.restore(d->editwindow_ui.checkBox_hideactualpaths);

//	// Parameters
//	state.restore(d->editwindow_ui.checkBox_showparameters);
//	state.restore(d->editwindow_ui.checkBox_parameters_colourByType);

	state.widgetHandled(this);
	state.warnUnrestored(this);

//	updateTrackTubeRadius();
//	updateTrackLightModel(d->editwindow_ui.checkBox_tracksUseBaseLightModel);

	updateButton();

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
	messageVerbose("lengthOf100GeV() changed");
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
