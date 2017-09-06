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
	GraphicsView(VP1EventDisplaySceneView *v) : QGraphicsView(), view(v) { }

protected:
	void wheelEvent(QWheelEvent *);

private:
	VP1EventDisplaySceneView *view;
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
	GraphicsView *graphicsView;

	bool _preview;

	QGraphicsRectItem* _frame;
	QGraphicsLineItem * _line;

	QLabel *label;
	QLabel *label2;

	QToolButton *selectModeButton;
	QToolButton *dragModeButton;
	QToolButton *openGlButton;
	QToolButton *antialiasButton;
	QToolButton *printButton;
	QToolButton *resetButton;

	QVBoxLayout *zoomSliderLayout;
	QSlider *zoomSlider;
	QToolButton *zoomInIcon;
	QToolButton *zoomOutIcon;

	QHBoxLayout *rotateSliderLayout;
	QSlider *rotateSlider;
	QToolButton *rotateLeftIcon;
	QToolButton *rotateRightIcon;

};


#endif /* VP1EVENTDISPLAYSCENEVIEW_H_ */
