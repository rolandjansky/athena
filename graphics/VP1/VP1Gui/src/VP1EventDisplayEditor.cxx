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
: QWidget(0), d(new Imp)
{
	_mainWindow = mainWindow;

	_runNumber = listRunEventNumberTimestamp[0];
	_eventNumber = listRunEventNumberTimestamp[1];
	_eventTimestamp = listRunEventNumberTimestamp[2];


	d->theclass = this;
	d->ui.setupUi(this);

	/* initialize the scene
	 *
	 * this is the scene that will be rendered and saved to the file.
	 * By default the scene dimension is 4000x2641 pixels.
	 *
	 */
	scene = new QGraphicsScene();
	scene->setSceneRect(QRectF(0.0, 0.0, 4000., 2641.));


	VP1EventDisplaySceneView *view = new VP1EventDisplaySceneView("Full-size view");
	view->view()->setScene(scene);

	preview = new VP1EventDisplaySceneView("Preview", true);
	preview->view()->setScene(scene);

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


	previewSplitter->addWidget(d->ui.groupBox_objectList);
	previewSplitter->addWidget(preview);
	previewSplitter->addWidget(d->ui.groupBox_actions);


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


	toolsSplitter->addWidget(d->ui.groupBox_getTab);
	toolsSplitter->addWidget(d->ui.groupBox_bkg);
	toolsSplitter->addWidget(d->ui.groupBox_logo);
	toolsSplitter->addWidget(d->ui.groupBox_labels);

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
	//preview->view()->setFixedSize(450,302);
	/*
	 * this triggers a better fitInView() but not enough for our purpose.
	 * See: http://stackoverflow.com/a/17085612
	 */
	//preview->show();

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
	//	preview->view()->viewport()->setFixedSize(400,265);


	QSize size = preview->view()->maximumViewportSize();
	VP1Msg::message("scene rect: " + QString::number(scene->sceneRect().width()) + " - " +  QString::number(scene->sceneRect().height()) );
	VP1Msg::message("max preview size: " + QString::number(size.width()) + " - " +  QString::number(size.height()) );

	preview->view()->ensureVisible ( scene->sceneRect(), 0, 0  );
	preview->view()->fitInView( scene->sceneRect(), Qt::KeepAspectRatio);

	// add a frame to preview to define the the rendering area
	preview->addRenderingFrame();

	// set the default scene bkg color: black
	this->setBackgroundColor(); // the "black" checkbox is checked by default in the GUI

	setWindowIcon(QIcon(QString(":/vp1/icons/icons/3d_32x32.png")));

	connect(d->ui.pushButton_get,SIGNAL(clicked()), this,SLOT(getTabSnapshot()));

//	connect(d->ui.pushButton_eventDetailsLabel,SIGNAL(clicked()), this,SLOT(getEventDetailsLabel()));


	connect(d->ui.radioButton_eventDetails,SIGNAL(toggled(bool)), this,SLOT(enableCustomTextField(bool)));
	//	connect(d->ui.radioButton_customText,SIGNAL(toggled()), this,SLOT(showCustomTextField()));

	connect(d->ui.pushButton_addText,SIGNAL(clicked()), this,SLOT(setTextLabel()));
	connect(d->ui.pushButton_removeObj,SIGNAL(clicked()), this,SLOT(removeObject()));
	connect(d->ui.pushButton_addLogo,SIGNAL(clicked()), this,SLOT(addLogo()));

	connect(d->ui.pushButton_save,SIGNAL(clicked()), this,SLOT(savePicture()));
	connect(d->ui.pushButton_print,SIGNAL(clicked()), this,SLOT(printPicture()));


	connect(d->ui.radioButton_black,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
	connect(d->ui.radioButton_white,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));
	connect(d->ui.radioButton_transparent,SIGNAL(clicked()), this,SLOT(setBackgroundColor()));

	this->setWindowTitle(tr("ATLAS VP1 Event Display Editor"));

	this->getEventDetailsLabel();


}

/*
 * Qt example
 */
//VP1EventDisplayEditor::VP1EventDisplayEditor()
////: QWidget(0,Qt::WindowStaysOnTopHint), d(new Imp)
//: QWidget(0), d(new Imp)
//{
//	d->theclass = this;
//	d->ui.setupUi(this);
//
//	scene = new QGraphicsScene();
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
//	view->view()->setScene(scene);
//	h1Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Top right view");
//	view->view()->setScene(scene);
//	h1Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Bottom left view");
//	view->view()->setScene(scene);
//	h2Splitter->addWidget(view);
//
//	view = new VP1EventDisplaySceneView("Bottom right view");
//	view->view()->setScene(scene);
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
	//	d->frameHolderWidget->deleteLater();
	//	foreach(VP1CustomTourFrameWidget*frame,d->frames)
	//	frame->deleteLater();
	delete d;
	delete scene;
	//	delete scene_preview;
}


//____________________________________________________________________
void VP1EventDisplayEditor::enableCustomTextField(bool checked)
{
//	if (d->ui.radioButton_eventDetails->isChecked())
	if (checked)
		d->ui.lineEdit_customLabelText->setEnabled(false);
	else
		d->ui.lineEdit_customLabelText->setEnabled(true);
}


//____________________________________________________________________
QString VP1EventDisplayEditor::getEventDetailsLabel()
{

	QString evtstr = "Run: "+QString::number(_runNumber)+"\n"
			+ "Event: "+QString::number(_eventNumber)+"\n"
			+ QString(_eventTimestamp>0 ? QDateTime::fromTime_t(_eventTimestamp).toString(Qt::ISODate).replace('T',' ')+ " CEST" : "");

	//VP1Msg::messageVerbose(evtstr);

	return evtstr;
}


//____________________________________________________________________
void VP1EventDisplayEditor::printPicture()
{
	preview->print();
}


//____________________________________________________________________
void VP1EventDisplayEditor::savePicture()
{
	VP1Msg::messageVerbose("VP1EventDisplayEditor::savePicture()");

	// hide the rendering frame, cause we do not want it in the final picture
	preview->hideRenderingFrame();

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
			"event_display.png",
			tr("Images (*.png *.jpg)"));

	QImage image(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::transparent);

	QPainter painter(&image);
	painter.setRenderHint(QPainter::Antialiasing);

	scene->render(&painter);

	image.save(fileName);

	// show the rendering frame again
	preview->showRenderingFrame();

}






//____________________________________________________________________
void VP1EventDisplayEditor::addPixmapList(QList<QPixmap>& list, QStringList& listNames)
{
	VP1Msg::messageVerbose("VP1EventDisplayEditor::addPixmapList()");

	if (!list.isEmpty()) {
		foreach (QPixmap img, list) {
			// add  and set it movableimage to the scene
			QGraphicsPixmapItem* item = scene->addPixmap(img);
			item->setFlags(QGraphicsItem::ItemIsMovable);
			sceneItems << item;
		}
	}

	if (!listNames.isEmpty()) {
		foreach (QString name, listNames) {
			// add image name to the list
			new QListWidgetItem(name, d->ui.listWidget);
		}
	}
}


//____________________________________________________________________
void VP1EventDisplayEditor::removeObject()
{
	QListWidgetItem * currentItem = d->ui.listWidget->currentItem();
	QString text = currentItem->text();
	int currentPos = d->ui.listWidget->row( currentItem );

	// remove the object from the scene
	scene->removeItem( sceneItems[currentPos] );
	// remove its associated pointer in our list
	sceneItems.removeAt( currentPos );
	// remove its associated label
	QListWidgetItem * item = d->ui.listWidget->takeItem(currentPos);
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

		if (d->ui.radioButton_eventDetails->isChecked()) {
			// get the event details
			text = this->getEventDetailsLabel();
		}
		else {
			// get the user's text
			text = d->ui.lineEdit_customLabelText->text();
		}


		// get users' settings
		int size = 0;
		if (d->ui.radioButton_size12->isChecked()) size = 12;
		else if (d->ui.radioButton_size24->isChecked()) size = 24;
		else if (d->ui.radioButton_size32->isChecked()) size = 32;
		else if (d->ui.radioButton_size44->isChecked()) size = 44;
		else if (d->ui.radioButton_sizeCustom->isChecked()) {
			size = d->ui.spinBox_size->value();
		}

		QString typeface = "Courier"; // default typeface

		int weight = QFont::Normal;
		if (d->ui.radioButton_light->isChecked()) weight = QFont::Light;
		else if (d->ui.radioButton_normal->isChecked()) weight = QFont::Normal;
		else if (d->ui.radioButton_bold->isChecked()) weight = QFont::Bold;
		else if (d->ui.radioButton_black_2->isChecked()) weight = QFont::Black;

		bool italics = false;
		if (d->ui.radioButton_italics->isChecked()) italics = true;

		font = QFont(typeface, size, weight, italics);

		if (d->ui.radioButton_monaco->isChecked()) {
			//		typeface = "Monaco";
			typeface = "Courier";
			font.setStyleHint(QFont::TypeWriter); // this defines the family to choose from, if the preferred typeface is not available on the host system
		}
		else if (d->ui.radioButton_helvetica->isChecked()) {
			typeface = "Helvetica";
			font.setStyleHint(QFont::SansSerif);
		}
		else if (d->ui.radioButton_times->isChecked()) {
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
// adding the label to the scene
QGraphicsTextItem* item = scene->addText(text, font);
item->setFlags(QGraphicsItem::ItemIsMovable);

// set the user's preferred color
if (d->ui.radioButton_whiteText->isChecked()) {
	item->setDefaultTextColor(Qt::white);
} else if (d->ui.radioButton_blackText->isChecked()) {
	item->setDefaultTextColor(Qt::black);
}

// adding the label to the list of objects
QString strippedText = text.replace('\n',' ').replace('\t', ' ').simplified();
new QListWidgetItem(strippedText, d->ui.listWidget);

// adding the label to list of object pointers
sceneItems << item;
}


//____________________________________________________________________
void VP1EventDisplayEditor::getTabSnapshot()
{
	int tabN = d->ui.comboBox_tabNumber->currentIndex();
	QString tabName = d->ui.comboBox_tabNumber->currentText();
	VP1Msg::messageVerbose("Got tab: " + tabName + " at idx: " + QString::number(tabN) );

	int imageWidth = d->ui.spinBox_imageWidth->value();
	VP1Msg::messageVerbose("width: " + QString::number(imageWidth) );

	QPixmap snap = _mainWindow->getSingleChannelCustomSnapshot( tabName, imageWidth );

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

	d->ui.comboBox_tabNumber->addItems(listNames);

}


//____________________________________________________________________
void VP1EventDisplayEditor::addLogo()
{
	QPixmap pix;
	QString name;

	// load the right version of the ATLAS logo
	if ( d->ui.radioButton_300px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_300pixels.png");
		if (d->ui.radioButton_blue->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_300px.png");
		name = "logo_300px";
	}
	else if ( d->ui.radioButton_450px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_450pixels.png");
		if (d->ui.radioButton_blue->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_450px.png");
		name = "logo_450px";
	}
	else if ( d->ui.radioButton_600px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_600pixels.png");
		if (d->ui.radioButton_blue->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_600px.png");
		name = "logo_600px";
	}
	else if ( d->ui.radioButton_800px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_800pixels.png");
//		if (d->ui.radioButton_blue->isChecked() )
//			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_600px.png");
		name = "logo_800px";
	}
	else if ( d->ui.radioButton_1000px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_1000pixels.png");
//		if (d->ui.radioButton_blue->isChecked() )
//			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_600px.png");
		name = "logo_1000px";
	}
	else if ( d->ui.radioButton_1200px->isChecked() ) {
		if ( d->ui.radioButton_gray->isChecked() )
			pix.load(":/vp1/images/images/ATLAS-Logo-New_1200pixels.png");
//		if (d->ui.radioButton_blue->isChecked() )
//			pix.load(":/vp1/images/images/ATLAS-chrome-logo-URL-blue_600px.png");
		name = "logo_1200px";
	}

	// add the logo to the scene
	QGraphicsPixmapItem* item = scene->addPixmap(pix);
	item->setFlags(QGraphicsItem::ItemIsMovable);
	// add to the list of pointers
	sceneItems << item;
	// add to the list of object labels
	new QListWidgetItem(name, d->ui.listWidget);
}


//____________________________________________________________________
void VP1EventDisplayEditor::setBackgroundColor()
{

	// a white background
	if ( d->ui.radioButton_white->isChecked() )
		scene->setBackgroundBrush(Qt::white);
	// a black background
	else if ( d->ui.radioButton_black->isChecked() )
		scene->setBackgroundBrush(Qt::black);
	// a transparent background
	else if ( d->ui.radioButton_transparent->isChecked() ) {
		scene->setBackgroundBrush(Qt::NoBrush);
		//		// a gradient background
		//		QRadialGradient gradient(0, 0, 10);
		//		gradient.setSpread(QGradient::RepeatSpread);
		//		scene->setBackgroundBrush(gradient);
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
//	return d->ui.graphicsView;
//}




////____________________________________________________________________
//void VP1EventDisplayEditor::buttonClicked()
//{
//	if (d->ui.pushButton_addCurrentView==sender()) {
//		VP1CustomTourFrameWidget * frame = new VP1CustomTourFrameWidget(d->viewer->currentCamIsPerspective(),
//				d->viewer->currentCameraState());
//		d->addFrame(frame);
//		d->updateFrameSnapshot(frame);
//		d->updateFrameListVisuals();
//	} else if (d->ui.pushButton_refreshPreviews==sender()) {
//		foreach(VP1CustomTourFrameWidget*frame,d->frames)
//    		  d->updateFrameSnapshot(frame);
//	} else if (d->ui.pushButton_execute==sender()) {
//		d->viewer->startCustomTour();
//	} else if (d->ui.pushButton_utilityZoomShow==sender()) {
//	}
//}

