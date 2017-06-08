/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * jira_1
 * VP1EventDisplaySceneView.cxx
 *
 *  Created on: Sep 26, 2013
 *      Author: rbianchi <Riccardo.Maria.Bianchi@cern.ch>
 *
 */

#include "VP1Gui/VP1EventDisplaySceneView.h"
#include "VP1Base/VP1Msg.h"

#include <QtGui>
#include <QPrinter>
#include <QPrintDialog>

#ifndef QT_NO_OPENGL
	#include <QtOpenGL>
#endif

#include <qmath.h>

// with this we can use the mouse central wheel on the zoom sliders
void GraphicsView::wheelEvent(QWheelEvent *e)
{
	if (e->modifiers() & Qt::ControlModifier) {
		if (e->delta() > 0)
			view->zoomIn(6);
		else
			view->zoomOut(6);
		e->accept();
	} else {
		QGraphicsView::wheelEvent(e);
	}
}

VP1EventDisplaySceneView::VP1EventDisplaySceneView(const QString &name, bool preview, QWidget *parent)
: QFrame(parent),_frame(nullptr),_line(nullptr)
{
	_preview = preview;
	

	setFrameStyle(Sunken | StyledPanel);

	graphicsView = new GraphicsView(this);
	graphicsView->setRenderHint(QPainter::Antialiasing, false);
	graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
	QSize iconSize(size, size);


	// ZOOM BUTTONS

	zoomInIcon = new QToolButton;
	zoomInIcon->setAutoRepeat(true);
	zoomInIcon->setAutoRepeatInterval(33);
	zoomInIcon->setAutoRepeatDelay(0);
	zoomInIcon->setIcon(QPixmap(":/vp1/icons/icons/zoomin.png"));
	zoomInIcon->setIconSize(iconSize);

	zoomOutIcon = new QToolButton;
	zoomOutIcon->setAutoRepeat(true);
	zoomOutIcon->setAutoRepeatInterval(33);
	zoomOutIcon->setAutoRepeatDelay(0);
	zoomOutIcon->setIcon(QPixmap(":/vp1/icons/icons/zoomout.png"));
	zoomOutIcon->setIconSize(iconSize);

	zoomSlider = new QSlider();
	zoomSlider->setMinimum(0);
	zoomSlider->setMaximum(500);
	zoomSlider->setValue(250);
	zoomSlider->setTickPosition(QSlider::TicksRight);

	// Zoom slider layout
	zoomSliderLayout = new QVBoxLayout;
	zoomSliderLayout->addWidget(zoomInIcon);
	zoomSliderLayout->addWidget(zoomSlider);
	zoomSliderLayout->addWidget(zoomOutIcon);


	// ROTATION BUTTONS
	rotateLeftIcon = new QToolButton();
	rotateLeftIcon->setIcon(QPixmap(":/vp1/icons/icons/rotateleft.png"));
	rotateLeftIcon->setIconSize(iconSize);

	rotateRightIcon = new QToolButton();
	rotateRightIcon->setIcon(QPixmap(":/vp1/icons/icons/rotateright.png"));
	rotateRightIcon->setIconSize(iconSize);

	rotateSlider = new QSlider();
	rotateSlider->setOrientation(Qt::Horizontal);
	rotateSlider->setMinimum(-360);
	rotateSlider->setMaximum(360);
	rotateSlider->setValue(0);
	rotateSlider->setTickPosition(QSlider::TicksBelow);

	// Rotate slider layout
	rotateSliderLayout = new QHBoxLayout;
	rotateSliderLayout->addWidget(rotateLeftIcon);
	rotateSliderLayout->addWidget(rotateSlider);
	rotateSliderLayout->addWidget(rotateRightIcon);

	resetButton = new QToolButton;
	resetButton->setText(tr("Reset"));
	resetButton->setEnabled(false);

	// Label layout
	QHBoxLayout *labelLayout = new QHBoxLayout;
	label = new QLabel(name);
	label2 = new QLabel(tr("Pointer Mode"));

	selectModeButton = new QToolButton;
	selectModeButton->setText(tr("Select & Move items"));
	selectModeButton->setCheckable(true);
	selectModeButton->setChecked(true);

	dragModeButton = new QToolButton;
	dragModeButton->setText(tr("Drag View"));
	dragModeButton->setCheckable(true);
	dragModeButton->setChecked(false);

	antialiasButton = new QToolButton;
	antialiasButton->setText(tr("Antialiasing"));
	antialiasButton->setCheckable(true);
	antialiasButton->setChecked(false);

	openGlButton = new QToolButton;
	openGlButton->setText(tr("OpenGL"));
	openGlButton->setCheckable(true);
#ifndef QT_NO_OPENGL
	openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
	openGlButton->setEnabled(false);
#endif
	printButton = new QToolButton;
	printButton->setIcon(QIcon(QPixmap(":/vp1/icons/icons/fileprint.png")));

	QButtonGroup *pointerModeGroup = new QButtonGroup;
	pointerModeGroup->setExclusive(true);
	pointerModeGroup->addButton(selectModeButton);
	pointerModeGroup->addButton(dragModeButton);

	labelLayout->addWidget(label);
	labelLayout->addStretch();

	if (!preview) {
		labelLayout->addWidget(label2);
		labelLayout->addWidget(selectModeButton);
		labelLayout->addWidget(dragModeButton);
		labelLayout->addStretch();
//		labelLayout->addWidget(antialiasButton); // we don't use these buttons
//		labelLayout->addWidget(openGlButton);
//		labelLayout->addWidget(printButton);
	}

	QGridLayout *topLayout = new QGridLayout;
	topLayout->addLayout(labelLayout, 0, 0);
	topLayout->addWidget(graphicsView, 1, 0);
	/*
	 * PREVIEW WINDOW
	 * we don't need the rotation or zoom buttons for the "preview" view
	 * and we put the reset button just below the zoom buttons
	 */
	if (preview) {
		//		zoomSliderLayout->addWidget(resetButton);
	}
	/*
	 * FULL SIZE WINDOW
	 */
	else {
		topLayout->addLayout(zoomSliderLayout, 1, 1);
		topLayout->addLayout(rotateSliderLayout, 2, 0);
		topLayout->addWidget(resetButton, 2, 1);
	}

	setLayout(topLayout);




	connect(resetButton, SIGNAL(clicked()), this, SLOT(resetView()));

	connect(graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
	connect(graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));

	connect(selectModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));
	connect(dragModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));

	connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
	connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

	connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(rotateLeftIcon, SIGNAL(clicked()), this, SLOT(rotateLeft()));
	connect(rotateRightIcon, SIGNAL(clicked()), this, SLOT(rotateRight()));


	// we do not use these three buttons now...
//	connect(printButton, SIGNAL(clicked()), this, SLOT(print()));
//	connect(antialiasButton, SIGNAL(toggled(bool)), this, SLOT(toggleAntialiasing()));
//	connect(openGlButton, SIGNAL(toggled(bool)), this, SLOT(toggleOpenGL()));

	setupMatrix(); // only useful for the full-view window
}

VP1EventDisplaySceneView::~VP1EventDisplaySceneView()
{
	/* if preview, many buttons are not added to any widget or layout,
	 * so we have to destroy them by hand
	 */
	if (_preview) {
		delete rotateSlider;
		delete rotateLeftIcon;
		delete rotateRightIcon;
		delete rotateSliderLayout;
		delete zoomSlider;
		delete zoomInIcon;
		delete zoomOutIcon;
		delete zoomSliderLayout;
		delete resetButton;
		delete label2;
		delete selectModeButton;
		delete dragModeButton;
	}
	/*
	 * we have to destroy them by hand, because we do not use them in this version of the GUI
	 */
	delete antialiasButton;
	delete openGlButton;
	delete printButton;
}

QGraphicsView *VP1EventDisplaySceneView::view() const
{
	return static_cast<QGraphicsView *>(graphicsView);
}

void VP1EventDisplaySceneView::addRenderingFrame()
{
// adding a rect to the preview to define the rendering area
//	QPen pen2(Qt::green, 5, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
	QPen pen(Qt::red, 5, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	QRectF sceneRect = graphicsView->scene()->sceneRect();
	_frame = graphicsView->scene()->addRect( sceneRect, pen );

	/* we also have to add a line on the bottom of the scene,
	 * since the bottom side of the frame will go out of the view,
	 * due to the thickness of the frame lines.
	 */
	qreal x1 = sceneRect.left();
	qreal y1 = sceneRect.bottom();
	qreal y2 = y1;
	qreal x2 = sceneRect.right();

	_line = graphicsView->scene()->addLine( x1, y1, x2, y2, pen);
}

void VP1EventDisplaySceneView::showRenderingFrame()
{
	_frame->show();
	_line->show();
}

void VP1EventDisplaySceneView::hideRenderingFrame()
{
	_frame->hide();
	_line->hide();
}



void VP1EventDisplaySceneView::resetView()
{
	zoomSlider->setValue(250);
	rotateSlider->setValue(0);
	setupMatrix();
	graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

	resetButton->setEnabled(false);
}

void VP1EventDisplaySceneView::setResetButtonEnabled()
{
	resetButton->setEnabled(true);
}

void VP1EventDisplaySceneView::setPreviewZoom(qreal xx)
{
	if (_preview) {
		QMatrix matrix;
		matrix.scale(xx, xx);
		graphicsView->setMatrix(matrix);
	} else {
		VP1Msg::message("Warning!! using setPreviewZoom() on a full-size view has no effect.");
	}

}

void VP1EventDisplaySceneView::setupMatrix()
{
	if (!_preview) {
		QMatrix matrix;
		qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));
		matrix.scale(scale, scale);
		matrix.rotate(rotateSlider->value());

		graphicsView->setMatrix(matrix);
		setResetButtonEnabled();
	}

}

void VP1EventDisplaySceneView::togglePointerMode()
{
	graphicsView->setDragMode(selectModeButton->isChecked()
			? QGraphicsView::RubberBandDrag
					: QGraphicsView::ScrollHandDrag);
	graphicsView->setInteractive(selectModeButton->isChecked());
}

void VP1EventDisplaySceneView::toggleOpenGL()
{
#ifndef QT_NO_OPENGL
	graphicsView->setViewport(openGlButton->isChecked() ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void VP1EventDisplaySceneView::toggleAntialiasing()
{
	graphicsView->setRenderHint(QPainter::Antialiasing, antialiasButton->isChecked());
}

void VP1EventDisplaySceneView::print()
{
#ifndef QT_NO_PRINTER
	QPrinter printer;
	QPrintDialog dialog(&printer, this);
	if (dialog.exec() == QDialog::Accepted) {
		QPainter painter(&printer);
		graphicsView->render(&painter);
	}
#endif
}

void VP1EventDisplaySceneView::zoomIn(int level)
{
	zoomSlider->setValue(zoomSlider->value() + level);
}

void VP1EventDisplaySceneView::zoomOut(int level)
{
	zoomSlider->setValue(zoomSlider->value() - level);
}

void VP1EventDisplaySceneView::rotateLeft()
{
	rotateSlider->setValue(rotateSlider->value() - 10);
}

void VP1EventDisplaySceneView::rotateRight()
{
	rotateSlider->setValue(rotateSlider->value() + 10);
}


