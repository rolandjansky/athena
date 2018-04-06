/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * jira_1
 * VP1EventDisplaySceneView.h
 *
 *  Created on: Sep 26, 2013
 *      Author: rbianchi <Riccardo.Maria.Bianchi@cern.ch>
 *
 */

#ifndef VP1EVENTDISPLAYSCENEVIEW_H_
#define VP1EVENTDISPLAYSCENEVIEW_H_


#include <QtGui>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsView>

QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QToolButton)

class VP1EventDisplaySceneView;


/*
 * we create a new class inheriting from QGraphicsView,
 * to reimplement the wheel event in order to use the wheel
 * to zoom the image, when used over the zoom sliders
 */
class GraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	GraphicsView(VP1EventDisplaySceneView *v) : QGraphicsView(), m_view(v) { }

protected:
	void wheelEvent(QWheelEvent *);

private:
	VP1EventDisplaySceneView *m_view;
};



/*
 * The QFrame which contains the graphicsView and all other widgets.
 *
 * If the 'preview' argument is set to true, then a 'preview' view is built
 * without rotation sliders and the zoom set to see the whole scene.
 */
class VP1EventDisplaySceneView : public QFrame
{
	Q_OBJECT
public:
	VP1EventDisplaySceneView(const QString &name, bool preview = false, QWidget *parent = 0);
	~VP1EventDisplaySceneView();

	QGraphicsView *view() const;
	void setPreviewZoom(qreal xx);
	void addRenderingFrame();
	void showRenderingFrame();
	void hideRenderingFrame();

	public slots:
	void zoomIn(int level = 1);
	void zoomOut(int level = 1);
	void print();

	private slots:
	void resetView();
	void setResetButtonEnabled();
	void setupMatrix();
	void togglePointerMode();
	void toggleOpenGL();
	void toggleAntialiasing();
	void rotateLeft();
	void rotateRight();

	private:
	GraphicsView *m_graphicsView;

	bool m_preview;

	QGraphicsRectItem* m_frame;
	QGraphicsLineItem * m_line;

	QLabel *m_label;
	QLabel *m_label2;

	QToolButton *m_selectModeButton;
	QToolButton *m_dragModeButton;
	QToolButton *m_openGlButton;
	QToolButton *m_antialiasButton;
	QToolButton *m_printButton;
	QToolButton *m_resetButton;

	QVBoxLayout *m_zoomSliderLayout;
	QSlider *m_zoomSlider;
	QToolButton *m_zoomInIcon;
	QToolButton *m_zoomOutIcon;

	QHBoxLayout *m_rotateSliderLayout;
	QSlider *m_rotateSlider;
	QToolButton *m_rotateLeftIcon;
	QToolButton *m_rotateRightIcon;

};


#endif /* VP1EVENTDISPLAYSCENEVIEW_H_ */
