/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1EventDisplayEditor               //
//                                                            //
//  Description: Event display editor.                        //
//                                                            //
//  Author: Riccardo-Maria BIANCHI (rbianchi@cern.ch)         //
//  Initial version: September 2013                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Gui/VP1EventDisplayEditor.h"
#include "ui_vp1eventdisplayeditor.h"

#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Gui/VP1MainWindow.h"

#include <QRectF>
#include <QShortcut>
#include <QCloseEvent>
#include <QFileDialog>

#include <iostream>


//____________________________________________________________________
class VP1EventDisplayEditor::Imp {

public:

	VP1EventDisplayEditor * theclass;
	Ui::VP1EventDisplayEditorForm ui;

	//	VP1ExaminerViewer* viewer;

	//	QGraphicsScene* scene;

	//	QWidget * frameHolderWidget;

};


////____________________________________________________________________
//QByteArray VP1EventDisplayEditor::state() const
//{
//	VP1Serialise s(0/*version*/);
//	QList<QByteArray> frameStates;
//	foreach(VP1CustomTourFrameWidget*frame,d->frames) {
//		frameStates << frame->serialise();
//		s.ignoreWidget(frame);
//	}
//	s.save(frameStates);
//	s.save(d->ui.doubleSpinBox_theta);
//	s.save(d->ui.doubleSpinBox_radius);
//	s.save(d->ui.groupBox_utilityZoom);
//	s.warnUnsaved(this);
//	return s.result();
//}

////____________________________________________________________________
//void VP1EventDisplayEditor::setState(QByteArray ba)
//{
//	foreach(VP1CustomTourFrameWidget*frame,d->frames)
//    		frame->deleteLater();
//	d->frames.clear();
//
//	VP1Deserialise s(ba);
//	if (s.version()!=0)
//		return;
//	QList<QByteArray> frameStates = s.restore<QList<QByteArray> >();
//	s.restore(d->ui.doubleSpinBox_theta);
//	s.restore(d->ui.doubleSpinBox_radius);
//	s.restore(d->ui.groupBox_utilityZoom);
//	s.warnUnrestored(this);
//
//	d->ui.widget_utilityZoomContents->setVisible(d->ui.groupBox_utilityZoom->isChecked());
//
//	foreach(QByteArray ba2, frameStates)
//	d->addFrame(new VP1CustomTourFrameWidget(ba2));
//	d->updateFrameListVisuals();
//	enabledFrameListChanged();
//}


/*
 * My first, old version
 */
////____________________________________________________________________
////VP1EventDisplayEditor::VP1EventDisplayEditor(VP1ExaminerViewer* viewer)
//VP1EventDisplayEditor::VP1EventDisplayEditor()
//: QWidget(0,Qt::WindowStaysOnTopHint), d(new Imp)
//{
//	d->theclass = this;
//	d->ui.setupUi(this);
//
//	scene = new QGraphicsScene(); // the size of the scene is by default infinite. But we will limit the view, here below
//	scene->setSceneRect(QRectF(0.0, 0.0, 4000., 2641.));
//
//	scene_preview = new QGraphicsScene();
//
//
//	d->ui.graphicsView->setScene(scene);
//	d->ui.graphicsView->setSceneRect(0, 0, 4000, 2641); // the size of a final ATLAS event display
//	d->ui.graphicsView->setMaximumSize(1000, 660); // the size of the view window on the screen
//
//	d->ui.graphicsView_preview->setScene(scene_preview);
//	d->ui.graphicsView_preview->setSceneRect(0, 0, 333, 220); // the size of the preview scene
//	d->ui.graphicsView_preview->setMaximumSize(333, 220); // the size of the preview window
//
//	// set the default bkg color: black
//	setBackgroundColor();
//
//	setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));
//
//	connect(d->ui.pushButton_addText,SIGNAL(clicked()), this,SLOT(addTextLabel()));
//	connect(d->ui.pushButton_removeObj,SIGNAL(clicked()), this,SLOT(removeObject()));
//	connect(d->ui.pushButton_addLogo,SIGNAL(clicked()), this,SLOT(addLogo()));
//
//	connect(d->ui.radioButton_black,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
//	connect(d->ui.radioButton_white,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
//	connect(d->ui.radioButton_transparent,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
//
//	connect(scene,SIGNAL(changed(const QList<QRectF>&)), this,SLOT(updatePreview(const QList<QRectF>&)));
//
//}


/*
 * new version
 */
//____________________________________________________________________
VP1EventDisplayEditor::VP1EventDisplayEditor(VP1MainWindow* mainWindow, QList<unsigned long long> listRunEventNumberTimestamp)
//: QWidget(0,Qt::WindowStaysOnTopHint), d(new Imp)
: QWidget(0), m_d(new Imp)
{
	m_mainWindow = mainWindow;

	m_runNumber = listRunEventNumberTimestamp[0];
	m_eventNumber = listRunEventNumberTimestamp[1];
	m_eventTimestamp = listRunEventNumberTimestamp[2];


	m_d->theclass = this;
	m_d->ui.setupUi(this);

	/* initialize the scene
	 *
	 * this is the scene that will be rendered and saved to the file.
	 * By default the scene dimension is 4000x2641 pixels.
	 *
	 */
	m_scene = new QGraphicsScene();
	m_scene->setSceneRect(QRectF(0.0, 0.0, 4000., 2641.));


	VP1EventDisplaySceneView *view = new VP1EventDisplaySceneView("Full-size view");
	view->view()->setScene(m_scene);

	m_preview = new VP1EventDisplaySceneView("Preview", true);
	m_preview->view()->setScene(m_scene);

	// the right splitter, containing the list of objects in the scene and the preview
	QSplitter *previewSplitter = new QSplitter;
	previewSplitter->setOrientation(Qt::Vertical);

	// the center splitter, containing the main view and the preview splitter
	QSplitter *viewSplitter = new QSplitter;
	viewSplitter->setOrientation(Qt::Horizontal);

	// the center splitter, containing the main view and the preview splitter
	QSplitter *toolsSplitter = new QSplitter;
	toolsSplitter->setOrientation(Qt::Vertical);

	// the main splitter, containing all views
	QSplitter *mainSplitter = new QSplitter;
	mainSplitter->setOrientation(Qt::Horizontal);


	previewSplitter->addWidget(m_d->ui.groupBox_objectList);
	previewSplitter->addWidget(m_preview);
	previewSplitter->addWidget(m_d->ui.groupBox_actions);


	QWidget *containerPreview = new QWidget;
	QVBoxLayout *containerPreview_layout = new QVBoxLayout;

	containerPreview_layout->addWidget(previewSplitter);
	containerPreview->setLayout(containerPreview_layout);

	viewSplitter->addWidget(view);
	viewSplitter->addWidget(containerPreview);

	QWidget *containerAllViews = new QWidget;
	QHBoxLayout *containerAllViews_layout = new QHBoxLayout;
	containerAllViews_layout->addWidget(viewSplitter);
	containerAllViews->setLayout(containerAllViews_layout);


	toolsSplitter->addWidget(m_d->ui.groupBox_getTab);
	toolsSplitter->addWidget(m_d->ui.groupBox_bkg);
	toolsSplitter->addWidget(m_d->ui.groupBox_logo);
	toolsSplitter->addWidget(m_d->ui.groupBox_labels);

	QWidget *containerTools = new QWidget;
	QVBoxLayout *containerTools_layout = new QVBoxLayout;
	containerTools_layout->addWidget(toolsSplitter);
	containerTools_layout->addStretch(1);
	containerTools->setLayout(containerTools_layout);


	//	mainSplitter->addWidget(containerTools);
	mainSplitter->addWidget(containerAllViews);

	QGridLayout* grid = static_cast<QGridLayout*>(this->layout());
	grid->addWidget(containerTools);
	grid->addWidget(mainSplitter);



	/*
	 * setting the size of the preview window
	 */
	//m_preview->view()->setFixedSize(450,302);
	/*
	 * this triggers a better fitInView() but not enough for our purpose.
	 * See: http://stackoverflow.com/a/17085612
	 */
	//m_preview->show();

	/*
	 * setting the size of the viewport
	 *
	 * Details:
	 * The QGraphicsView is not the widget that actually contains the scene,
	 * because the QGraphicsView also manages the scrollbars.
	 * The scene is drawn in the viewport widget,
	 * which is a little smaller than the QGraphicsView.
	 * (http://qt-project.org/forums/viewthread/17504)
	 *
	 * Update: useful, but it's useless here, because we want to use fitInView()
	 * from the view, and that does not match with the viewport size.
	 */
	//	m_preview->view()->viewport()->setFixedSize(400,265);


	QSize size = m_preview->view()->maximumViewportSize();
	VP1Msg::message("scene rect: " + QString::number(m_scene->sceneRect().width()) + " - " +  QString::number(m_scene->sceneRect().height()) );
	VP1Msg::message("max preview size: " + QString::number(size.width()) + " - " +  QString::number(size.height()) );

	m_preview->view()->ensureVisible ( m_scene->sceneRect(), 0, 0  );
	m_preview->view()->fitInView( m_scene->sceneRect(), Qt::KeepAspectRatio);

	// add a frame to preview to define the the rendering area
	m_preview->addRenderingFrame();

	// set the default m_scene bkg color: black
	this->setBackgroundColor(); // the "black" checkbox is checked by default in the GUI

	setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));

	connect(m_d->ui.pushButton_get,SIGNAL(clicked()), this,SLOT(getTabSnapshot()));

//	connect(m_d->ui.pushButton_eventDetailsLabel,SIGNAL(clicked()), this,SLOT(getEventDetailsLabel()));


	connect(m_d->ui.radioButton_eventDetails,SIGNAL(toggled(bool)), this,SLOT(enableCustomTextField(bool)));
	//	connect(m_d->ui.radioButton_customText,SIGNAL(toggled()), this,SLOT(showCustomTextField()));

	connect(m_d->ui.pushButton_addText,SIGNAL(clicked()), this,SLOT(setTextLabel()));
	connect(m_d->ui.pushButton_removeObj,SIGNAL(clicked()), this,SLOT(removeObject()));
	connect(m_d->ui.pushButton_addLogo,SIGNAL(clicked()), this,SLOT(addLogo()));

	connect(m_d->ui.pushButton_save,SIGNAL(clicked()), this,SLOT(savePicture()));
	connect(m_d->ui.pushButton_print,SIGNAL(clicked()), this,SLOT(printPicture()));


	connect(m_d->ui.radioButton_black,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
	connect(m_d->ui.radioButton_white,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
	connect(m_d->ui.radioButton_transparent,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));

	this->setWindowTitle(tr("ATLAS VP1 Event Display Editor"));

	this->getEventDetailsLabel();


}

/*
 * Qt example
 */
//VP1EventDisplayEditor::VP1EventDisplayEditor()
////: QWidget(0,Qt::WindowStaysOnTopHint), m_d(new Imp)
//: QWidget(0), m_d(new Imp)
//{
//	m_d->theclass = this;
//	m_d->ui.setupUi(this);
//
//	m_scene = new QGraphicsScene();
//
//	h1Splitter = new QSplitter;
//	h2Splitter = new QSplitter;
//
//	QSplitter *vSplitter = new QSplitter;
//	vSplitter->setOrientation(Qt::Vertical);
//	vSplitter->addWidget(h1Splitter);
//	vSplitter->addWidget(h2Splitter);
//
//	VP1EventDisplaySceneView *view = new VP1EventDisplaySceneView("Top left view");
//	view->view()->setScene(m_scene);
//	h1Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Top right view");
//	view->view()->setScene(m_scene);
//	h1Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Bottom left view");
//	view->view()->setScene(m_scene);
//	h2Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Bottom right view");
//	view->view()->setScene(m_scene);
//	h2Splitter->addWidget(view);
//
//	QLayout *layout = this->layout();
//	layout->addWidget(vSplitter);
////	setLayout(layout);
//
//	setWindowTitle(tr("Chip Demo"));
//
//
//}



//____________________________________________________________________
VP1EventDisplayEditor::~VP1EventDisplayEditor()
{
	//	m_d->frameHolderWidget->deleteLater();
	//	foreach(VP1CustomTourFrameWidget*frame,m_d->frames)
	//	frame->deleteLater();
	delete m_d;
	delete m_scene;
	//	delete scene_preview;
}


//____________________________________________________________________
void VP1EventDisplayEditor::enableCustomTextField(bool checked)
{
//	if (m_d->ui.radioButton_eventDetails->isChecked())
	if (checked)
		m_d->ui.lineEdit_customLabelText->setEnabled(false);
	else
		m_d->ui.lineEdit_customLabelText->setEnabled(true);
}


//____________________________________________________________________
QString VP1EventDisplayEditor::getEventDetailsLabel()
{

	QString evtstr = "Run: "+QString::number(m_runNumber)+"\n"
			+ "Event: "+QString::number(m_eventNumber)+"\n"
			+ QString(m_eventTimestamp>0 ? QDateTime::fromTime_t(m_eventTimestamp).toString(Qt::ISODate).replace('T',' ')+ " CEST" : "");

	//VP1Msg::messageVerbose(evtstr);

	return evtstr;
}


//____________________________________________________________________
void VP1EventDisplayEditor::printPicture()
{
	m_preview->print();
}


//____________________________________________________________________
void VP1EventDisplayEditor::savePicture()
{
	VP1Msg::messageVerbose("VP1EventDisplayEditor::savePicture()");

	// hide the rendering frame, cause we do not want it in the final picture
	m_preview->hideRenderingFrame();

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
			"event_display.png",
			tr("Images (*.png *.jpg)"));

	QImage image(m_scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);

	m_scene->render(&painter);

	image.save(fileName);

	// show the rendering frame again
	m_preview->showRenderingFrame();

}






//____________________________________________________________________
void VP1EventDisplayEditor::addPixmapList(QList<QPixmap>& list, QStringList& listNames)
{
	VP1Msg::messageVerbose("VP1EventDisplayEditor::addPixmapList()");

	if (!list.isEmpty()) {
		foreach (QPixmap img, list) {
			// add  and set it movableimage to the scene
			QGraphicsPixmapItem* item = m_scene->addPixmap(img);
			item->setFlags(QGraphicsItem::ItemIsMovable);
			m_sceneItems << item;
		}
	}

	if (!listNames.isEmpty()) {
		foreach (QString name, listNames) {
			// add image name to the list
			new QListWidgetItem(name, m_d->ui.listWidget);
		}
	}
}


//____________________________________________________________________
void VP1EventDisplayEditor::removeObject()
{
	QListWidgetItem * currentItem = m_d->ui.listWidget->currentItem();
	QString text = currentItem->text();
	int currentPos = m_d->ui.listWidget->row( currentItem );

	// remove the object from the scene
	m_scene->removeItem( m_sceneItems[currentPos] );
	// remove its associated pointer in our list
	m_sceneItems.removeAt( currentPos );
	// remove its associated label
	QListWidgetItem * item = m_d->ui.listWidget->takeItem(currentPos);
	delete item;

	VP1Msg::message("Successfully removed the object: " + text);
}

//// SLOT
////____________________________________________________________________
//void VP1EventDisplayEditor::addTextLabelSlot()
//{
//	this->addTextLabel();
//}


//____________________________________________________________________
void VP1EventDisplayEditor::setTextLabel()
{
	QString text;
	QFont font;

//	/* if we use this function programmatically,
//	 * passing text directly
//	 */
//	if (!textString.isEmpty()) {
//
//		text = textString;
//
//		// default settings
//		int size = 32;
//		int weight = QFont::Normal;
//		bool italics = false;
//		QString typeface = "Courier";
//
//		// default font
//		font = QFont(typeface, size, weight, italics);
//
//		// default font family, if Courier is not available
//		font.setStyleHint(QFont::TypeWriter);
//
//	}

//	/* if we use this function from the GUI,
//	 * then we get all user's settings from the GUI itself
//	 */
//	else {

		if (m_d->ui.radioButton_eventDetails->isChecked()) {
			// get the event details
			text = this->getEventDetailsLabel();
		}
		else {
			// get the user's text
			text = m_d->ui.lineEdit_customLabelText->text();
		}


		// get users' settings
		int size = 0;
		if (m_d->ui.radioButton_size12->isChecked()) size = 12;
		else if (m_d->ui.radioButton_size24->isChecked()) size = 24;
		else if (m_d->ui.radioButton_size32->isChecked()) size = 32;
		else if (m_d->ui.radioButton_size44->isChecked()) size = 44;
		else if (m_d->ui.radioButton_sizeCustom->isChecked()) {
			size = m_d->ui.spinBox_size->value();
		}

		QString typeface = "Courier"; // default typeface

		int weight = QFont::Normal;
		if (m_d->ui.radioButton_light->isChecked()) weight = QFont::Light;
		else if (m_d->ui.radioButton_normal->isChecked()) weight = QFont::Normal;
		else if (m_d->ui.radioButton_bold->isChecked()) weight = QFont::Bold;
		else if (m_d->ui.radioButton_black_2->isChecked()) weight = QFont::Black;

		bool italics = false;
		if (m_d->ui.radioButton_italics->isChecked()) italics = true;

		font = QFont(typeface, size, weight, italics);

		if (m_d->ui.radioButton_monaco->isChecked()) {
			//		typeface = "Monaco";
			typeface = "Courier";
			font.setStyleHint(QFont::TypeWriter); // this defines the family to choose from, if the preferred typeface is not available on the host system
		}
		else if (m_d->ui.radioButton_helvetica->isChecked()) {
			typeface = "Helvetica";
			font.setStyleHint(QFont::SansSerif);
		}
		else if (m_d->ui.radioButton_times->isChecked()) {
			typeface = "Times";
			font.setStyleHint(QFont::Serif);
		}
		font.setFamily(typeface);

//	}

	this->addTextLabel(text, font);
}


//____________________________________________________________________
void VP1EventDisplayEditor::addTextLabel(QString text, QFont font)
{
// adding the label to the m_scene
QGraphicsTextItem* item = m_scene->addText(text, font);
item->setFlags(QGraphicsItem::ItemIsMovable);

// set the user's preferred color
if (m_d->ui.radioButton_whiteText->isChecked()) {
	item->setDefaultTextColor(Qt::white);
} else if (m_d->ui.radioButton_blackText->isChecked()) {
	item->setDefaultTextColor(Qt::black);
}

// adding the label to the list of objects
QString strippedText = text.replace('\n',' ').replace('\t', ' ').simplified();
new QListWidgetItem(strippedText, m_d->ui.listWidget);

// adding the label to list of object pointers
m_sceneItems << item;
}


//____________________________________________________________________
void VP1EventDisplayEditor::getTabSnapshot()
{
	int tabN = m_d->ui.comboBox_tabNumber->currentIndex();
	QString tabName = m_d->ui.comboBox_tabNumber->currentText();
	VP1Msg::messageVerbose("Got tab: " + tabName + " at idx: " + QString::number(tabN) );

	int imageWidth = m_d->ui.spinBox_imageWidth->value();
	VP1Msg::messageVerbose("width: " + QString::number(imageWidth) );

	QPixmap snap = m_mainWindow->getSingleChannelCustomSnapshot( tabName, imageWidth );

	tabName = tabName + "_" + QString::number(imageWidth);


	QList<QPixmap> listPixmap;
	QStringList listNamesPixmap;

	listPixmap << snap;
	listNamesPixmap << tabName;

	this->addPixmapList(listPixmap, listNamesPixmap);

}

//____________________________________________________________________
void VP1EventDisplayEditor::setTabsList( QStringList listNames )
{
	VP1Msg::messageVerbose("VP1EventDisplayEditor::setTabsList()");

	m_d->ui.comboBox_tabNumber->addItems(listNames);

}


//____________________________________________________________________
void VP1EventDisplayEditor::addLogo()
{
	QPixmap pix;
	QString name;

	// load the right version of the ATLAS logo
	if ( m_d->ui.radioButton_300px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_300pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-300px.png");
		name = "logo_300px";
	}
	else if ( m_d->ui.radioButton_450px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_450pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-450px.png");
		name = "logo_450px";
	}
	else if ( m_d->ui.radioButton_600px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_600pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-600px.png");
		name = "logo_600px";
	}
	else if ( m_d->ui.radioButton_800px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_800pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-800px.png");
		name = "logo_800px";
	}
	else if ( m_d->ui.radioButton_1000px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_1000pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-1000px.png");
		name = "logo_1000px";
	}
	else if ( m_d->ui.radioButton_1200px->isChecked() ) {
		if ( m_d->ui.radioButton_logo_white->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_1200pixels.png");
		if ( m_d->ui.radioButton_logo_main->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-Ref-RGB-transpBkg-1200px.png");
		name = "logo_1200px";
	}

	// add the logo to the m_scene
	QGraphicsPixmapItem* item = m_scene->addPixmap(pix);
	item->setFlags(QGraphicsItem::ItemIsMovable);
	// add to the list of pointers
	m_sceneItems << item;
	// add to the list of object labels
	new QListWidgetItem(name, m_d->ui.listWidget);
}


//____________________________________________________________________
void VP1EventDisplayEditor::setBackgroundColor()
{

	// a white background
	if ( m_d->ui.radioButton_white->isChecked() )
		m_scene->setBackgroundBrush(Qt::white);
	// a black background
	else if ( m_d->ui.radioButton_black->isChecked() )
		m_scene->setBackgroundBrush(Qt::black);
	// a transparent background
	else if ( m_d->ui.radioButton_transparent->isChecked() ) {
		m_scene->setBackgroundBrush(Qt::NoBrush);
		//		// a gradient background
		//		QRadialGradient gradient(0, 0, 10);
		//		gradient.setSpread(QGradient::RepeatSpread);
		//		m_scene->setBackgroundBrush(gradient);
	}
}



//____________________________________________________________________
void VP1EventDisplayEditor::closeEvent(QCloseEvent*ev)
{
	// if you want to just hide the window when closed, uncomment the lines below
	ev->ignore();
	hide();

	// this, instead, will close the window
	//	ev->accept();
}

//QGraphicsView* VP1EventDisplayEditor::getView()
//{
//	return m_d->ui.graphicsView;
//}




////____________________________________________________________________
//void VP1EventDisplayEditor::buttonClicked()
//{
//	if (m_d->ui.pushButton_addCurrentView==sender()) {
//		VP1CustomTourFrameWidget * frame = new VP1CustomTourFrameWidget(m_d->viewer->currentCamIsPerspective(),
//				m_d->viewer->currentCameraState());
//		m_d->addFrame(frame);
//		m_d->updateFrameSnapshot(frame);
//		m_d->updateFrameListVisuals();
//	} else if (m_d->ui.pushButton_refreshPreviews==sender()) {
//		foreach(VP1CustomTourFrameWidget*frame,m_d->frames)
//    		  m_d->updateFrameSnapshot(frame);
//	} else if (m_d->ui.pushButton_execute==sender()) {
//		m_d->viewer->startCustomTour();
//	} else if (m_d->ui.pushButton_utilityZoomShow==sender()) {
//	}
//}

