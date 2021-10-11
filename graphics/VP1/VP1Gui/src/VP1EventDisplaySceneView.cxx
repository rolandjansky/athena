/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
		if (e->angleDelta().y() > 0)
			m_view->zoomIn(6);
		else
			m_view->zoomOut(6);
		e->accept();
	} else {
		QGraphicsView::wheelEvent(e);
	}
}

VP1EventDisplaySceneView::VP1EventDisplaySceneView(const QString &name, bool preview, QWidget *parent)
: QFrame(parent),m_frame(nullptr),m_line(nullptr)
{
	m_preview = preview;
	

	setFrameStyle(Sunken | StyledPanel);

	m_graphicsView = new GraphicsView(this);
	m_graphicsView->setRenderHint(QPainter::Antialiasing, false);
	m_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
	m_graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
	m_graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	m_graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
	QSize iconSize(size, size);


	// ZOOM BUTTONS

	m_zoomInIcon = new QToolButton;
	m_zoomInIcon->setAutoRepeat(true);
	m_zoomInIcon->setAutoRepeatInterval(33);
	m_zoomInIcon->setAutoRepeatDelay(0);
	m_zoomInIcon->setIcon(QPixmap(":/vp1/icons/icons/zoomin.png"));
	m_zoomInIcon->setIconSize(iconSize);

	m_zoomOutIcon = new QToolButton;
	m_zoomOutIcon->setAutoRepeat(true);
	m_zoomOutIcon->setAutoRepeatInterval(33);
	m_zoomOutIcon->setAutoRepeatDelay(0);
	m_zoomOutIcon->setIcon(QPixmap(":/vp1/icons/icons/zoomout.png"));
	m_zoomOutIcon->setIconSize(iconSize);

	m_zoomSlider = new QSlider();
	m_zoomSlider->setMinimum(0);
	m_zoomSlider->setMaximum(500);
	m_zoomSlider->setValue(250);
	m_zoomSlider->setTickPosition(QSlider::TicksRight);

	// Zoom slider layout
	m_zoomSliderLayout = new QVBoxLayout;
	m_zoomSliderLayout->addWidget(m_zoomInIcon);
	m_zoomSliderLayout->addWidget(m_zoomSlider);
	m_zoomSliderLayout->addWidget(m_zoomOutIcon);


	// ROTATION BUTTONS
	m_rotateLeftIcon = new QToolButton();
	m_rotateLeftIcon->setIcon(QPixmap(":/vp1/icons/icons/rotateleft.png"));
	m_rotateLeftIcon->setIconSize(iconSize);

	m_rotateRightIcon = new QToolButton();
	m_rotateRightIcon->setIcon(QPixmap(":/vp1/icons/icons/rotateright.png"));
	m_rotateRightIcon->setIconSize(iconSize);

	m_rotateSlider = new QSlider();
	m_rotateSlider->setOrientation(Qt::Horizontal);
	m_rotateSlider->setMinimum(-360);
	m_rotateSlider->setMaximum(360);
	m_rotateSlider->setValue(0);
	m_rotateSlider->setTickPosition(QSlider::TicksBelow);

	// Rotate slider layout
	m_rotateSliderLayout = new QHBoxLayout;
	m_rotateSliderLayout->addWidget(m_rotateLeftIcon);
	m_rotateSliderLayout->addWidget(m_rotateSlider);
	m_rotateSliderLayout->addWidget(m_rotateRightIcon);

	m_resetButton = new QToolButton;
	m_resetButton->setText(tr("Reset"));
	m_resetButton->setEnabled(false);

	// Label layout
	QHBoxLayout *labelLayout = new QHBoxLayout;
	m_label = new QLabel(name);
	m_label2 = new QLabel(tr("Pointer Mode"));

	m_selectModeButton = new QToolButton;
	m_selectModeButton->setText(tr("Select & Move items"));
	m_selectModeButton->setCheckable(true);
	m_selectModeButton->setChecked(true);

	m_dragModeButton = new QToolButton;
	m_dragModeButton->setText(tr("Drag View"));
	m_dragModeButton->setCheckable(true);
	m_dragModeButton->setChecked(false);

	m_antialiasButton = new QToolButton;
	m_antialiasButton->setText(tr("Antialiasing"));
	m_antialiasButton->setCheckable(true);
	m_antialiasButton->setChecked(false);

	m_openGlButton = new QToolButton;
	m_openGlButton->setText(tr("OpenGL"));
	m_openGlButton->setCheckable(true);
#ifndef QT_NO_OPENGL
	m_openGlButton->setEnabled(QGLFormat::hasOpenGL());
#else
	m_openGlButton->setEnabled(false);
#endif
	m_printButton = new QToolButton;
	m_printButton->setIcon(QIcon(QPixmap(":/vp1/icons/icons/fileprint.png")));

	QButtonGroup *pointerModeGroup = new QButtonGroup;
	pointerModeGroup->setExclusive(true);
	pointerModeGroup->addButton(m_selectModeButton);
	pointerModeGroup->addButton(m_dragModeButton);

	labelLayout->addWidget(m_label);
	labelLayout->addStretch();

	if (!preview) {
		labelLayout->addWidget(m_label2);
		labelLayout->addWidget(m_selectModeButton);
		labelLayout->addWidget(m_dragModeButton);
		labelLayout->addStretch();
//		labelLayout->addWidget(m_antialiasButton); // we don't use these buttons
//		labelLayout->addWidget(m_openGlButton);
//		labelLayout->addWidget(m_printButton);
	}

	QGridLayout *topLayout = new QGridLayout;
	topLayout->addLayout(labelLayout, 0, 0);
	topLayout->addWidget(m_graphicsView, 1, 0);
	/*
	 * PREVIEW WINDOW
	 * we don't need the rotation or zoom buttons for the "preview" view
	 * and we put the reset button just below the zoom buttons
	 */
	if (preview) {
		//		m_zoomSliderLayout->addWidget(m_resetButton);
	}
	/*
	 * FULL SIZE WINDOW
	 */
	else {
		topLayout->addLayout(m_zoomSliderLayout, 1, 1);
		topLayout->addLayout(m_rotateSliderLayout, 2, 0);
		topLayout->addWidget(m_resetButton, 2, 1);
	}

	setLayout(topLayout);




	connect(m_resetButton, SIGNAL(clicked()), this, SLOT(resetView()));

	connect(m_graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));
	connect(m_graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(setResetButtonEnabled()));

	connect(m_selectModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));
	connect(m_dragModeButton, SIGNAL(toggled(bool)), this, SLOT(togglePointerMode()));

	connect(m_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(m_zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
	connect(m_zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

	connect(m_rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
	connect(m_rotateLeftIcon, SIGNAL(clicked()), this, SLOT(rotateLeft()));
	connect(m_rotateRightIcon, SIGNAL(clicked()), this, SLOT(rotateRight()));


	// we do not use these three buttons now...
//	connect(m_printButton, SIGNAL(clicked()), this, SLOT(print()));
//	connect(m_antialiasButton, SIGNAL(toggled(bool)), this, SLOT(toggleAntialiasing()));
//	connect(m_openGlButton, SIGNAL(toggled(bool)), this, SLOT(toggleOpenGL()));

	setupMatrix(); // only useful for the full-view window
}

VP1EventDisplaySceneView::~VP1EventDisplaySceneView()
{
	/* if preview, many buttons are not added to any widget or layout,
	 * so we have to destroy them by hand
	 */
	if (m_preview) {
		delete m_rotateSlider;
		delete m_rotateLeftIcon;
		delete m_rotateRightIcon;
		delete m_rotateSliderLayout;
		delete m_zoomSlider;
		delete m_zoomInIcon;
		delete m_zoomOutIcon;
		delete m_zoomSliderLayout;
		delete m_resetButton;
		delete m_label2;
		delete m_selectModeButton;
		delete m_dragModeButton;
	}
	/*
	 * we have to destroy them by hand, because we do not use them in this version of the GUI
	 */
	delete m_antialiasButton;
	delete m_openGlButton;
	delete m_printButton;
}

QGraphicsView *VP1EventDisplaySceneView::view() const
{
	return static_cast<QGraphicsView *>(m_graphicsView);
}

void VP1EventDisplaySceneView::addRenderingFrame()
{
// adding a rect to the preview to define the rendering area
//	QPen pen2(Qt::green, 5, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
	QPen pen(Qt::red, 5, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
	QRectF sceneRect = m_graphicsView->scene()->sceneRect();
	m_frame = m_graphicsView->scene()->addRect( sceneRect, pen );

	/* we also have to add a line on the bottom of the scene,
	 * since the bottom side of the frame will go out of the view,
	 * due to the thickness of the frame lines.
	 */
	qreal x1 = sceneRect.left();
	qreal y1 = sceneRect.bottom();
	qreal y2 = y1;
	qreal x2 = sceneRect.right();

	m_line = m_graphicsView->scene()->addLine( x1, y1, x2, y2, pen);
}

void VP1EventDisplaySceneView::showRenderingFrame()
{
	m_frame->show();
	m_line->show();
}

void VP1EventDisplaySceneView::hideRenderingFrame()
{
	m_frame->hide();
	m_line->hide();
}



void VP1EventDisplaySceneView::resetView()
{
	m_zoomSlider->setValue(250);
	m_rotateSlider->setValue(0);
	setupMatrix();
	m_graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

	m_resetButton->setEnabled(false);
}

void VP1EventDisplaySceneView::setResetButtonEnabled()
{
	m_resetButton->setEnabled(true);
}

void VP1EventDisplaySceneView::setPreviewZoom(qreal xx)
{
	if (m_preview) {
		QTransform matrix;
		matrix.scale(xx, xx);
		m_graphicsView->setTransform(matrix);
	} else {
		VP1Msg::message("Warning!! using setPreviewZoom() on a full-size view has no effect.");
	}

}

void VP1EventDisplaySceneView::setupMatrix()
{
	if (!m_preview) {
		QTransform matrix;
		qreal scale = qPow(qreal(2), (m_zoomSlider->value() - 250) / qreal(50));
		matrix.scale(scale, scale);
		matrix.rotate(m_rotateSlider->value());

		m_graphicsView->setTransform(matrix);
		setResetButtonEnabled();
	}

}

void VP1EventDisplaySceneView::togglePointerMode()
{
	m_graphicsView->setDragMode(m_selectModeButton->isChecked()
			? QGraphicsView::RubberBandDrag
					: QGraphicsView::ScrollHandDrag);
	m_graphicsView->setInteractive(m_selectModeButton->isChecked());
}

void VP1EventDisplaySceneView::toggleOpenGL()
{
#ifndef QT_NO_OPENGL
	m_graphicsView->setViewport(m_openGlButton->isChecked() ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}

void VP1EventDisplaySceneView::toggleAntialiasing()
{
	m_graphicsView->setRenderHint(QPainter::Antialiasing, m_antialiasButton->isChecked());
}

void VP1EventDisplaySceneView::print()
{
#ifndef QT_NO_PRINTER
	QPrinter printer;
	QPrintDialog dialog(&printer, this);
	if (dialog.exec() == QDialog::Accepted) {
		QPainter painter(&printer);
		m_graphicsView->render(&painter);
	}
#endif
}

void VP1EventDisplaySceneView::zoomIn(int level)
{
	m_zoomSlider->setValue(m_zoomSlider->value() + level);
}

void VP1EventDisplaySceneView::zoomOut(int level)
{
	m_zoomSlider->setValue(m_zoomSlider->value() - level);
}

void VP1EventDisplaySceneView::rotateLeft()
{
	m_rotateSlider->setValue(m_rotateSlider->value() - 10);
}

void VP1EventDisplaySceneView::rotateRight()
{
	m_rotateSlider->setValue(m_rotateSlider->value() + 10);
}


