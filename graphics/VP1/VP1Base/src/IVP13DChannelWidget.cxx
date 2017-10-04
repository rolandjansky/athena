/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class IVP13DChannelWidget            //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: April 2007                            //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VP1Base/IVP13DChannelWidget.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1CameraHelper.h"
#include "VP1Base/VP1ExaminerViewer.h"
#include "VP1Base/VP1Msg.h"

#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QByteArray>

#include <Inventor/C/errors/debugerror.h>
#include "Inventor/Qt/SoQtRenderArea.h"
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include "Inventor/nodes/SoNode.h"
#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <iostream>

class IVP13DChannelWidget::Imp {
public:

	//	typedef std::vector<SoQtRenderArea*> RenderAreaList;
	//	std::vector<SoQtRenderArea*>::iterator it, itE;
	typedef std::vector<VP1ExaminerViewer*> RenderAreaList;
	std::vector<VP1ExaminerViewer*>::iterator it, itE;

	RenderAreaList renderareas;
};

//___________________________________________________
IVP13DChannelWidget::IVP13DChannelWidget(const QString & name, const QString & information, const QString & contact_info)
: IVP1ChannelWidget(name,information,contact_info), m_d(new Imp)
{
	m_d->itE = m_d->renderareas.end();

}

//___________________________________________________
IVP13DChannelWidget::~IVP13DChannelWidget()
{
	setUpdatesEnabled(false);
	m_d->it = m_d->renderareas.begin();
	for (;m_d->it!=m_d->itE;++(m_d->it)) {
		//    SoQtRenderArea* ra = *(m_d->it);
		VP1ExaminerViewer* ra = *(m_d->it);
		ra->setAutoRedraw(false);//extra
		SoNode * root = ra->getSceneGraph();
		root->ref();
		ra->setSceneGraph(0);
		//     SoGLRenderAction * renderaction1 = ra->getGLRenderAction();
		//     SoGLRenderAction * renderaction2 = ra->getOverlayGLRenderAction();
		SoLineHighlightRenderAction * trick = new SoLineHighlightRenderAction();
		SoLineHighlightRenderAction * trick2 = new SoLineHighlightRenderAction();
		ra->setGLRenderAction(trick);
		ra->setOverlayGLRenderAction(trick2);
		//     ra->setGLRenderAction(0);
		//     ra->setOverlayGLRenderAction(0);
		delete ra;
		//     delete renderaction1;
		//     delete renderaction2;
		delete trick;
		delete trick2;
		root->unref();
	}
	delete m_d;
}

//___________________________________________________
void IVP13DChannelWidget::setUpdatesEnabled ( bool enable )
{
	m_d->it = m_d->renderareas.begin();
	for (;m_d->it!=m_d->itE;++(m_d->it)) {
		(*(m_d->it))->setAutoRedraw(enable);
	}
	IVP1ChannelWidget::setUpdatesEnabled(enable);
}

//___________________________________________________
//void IVP13DChannelWidget::registerRenderArea(SoQtRenderArea* ra)
void IVP13DChannelWidget::registerRenderArea(VP1ExaminerViewer* ra)
{
	if (!ra) {
		message("Error - tried to register NULL render area!");
		return;
	}

	VP1QtInventorUtils::ensureInitLineWidthAndPointSize(ra);

	m_d->renderareas.push_back(ra);
	m_d->itE = m_d->renderareas.end();

	//To make sure that it is the actual 3D widget that gets focus when
	//we e.g. click anywhere else in the widget where the render area is
	//placed within:
	if (ra->getParentWidget()&&ra->getWidget())
		ra->getParentWidget()->setFocusProxy(ra->getWidget());
}

//___________________________________________________
void IVP13DChannelWidget::goingToNextEvent()
{
	//Stop spinning and abort any ongoing animations:
	m_d->it = m_d->renderareas.begin();
	for (;m_d->it!=m_d->itE;++(m_d->it)) {
		VP1ExaminerViewer* ra = *(m_d->it);
		if (ra->getTypeId().isDerivedFrom(SoQtViewer::getClassTypeId())) {
			VP1CameraHelper::abortAnyCurrentZoom(static_cast<SoQtViewer*>(ra)->getCamera());
			if (ra->getTypeId().isDerivedFrom(SoQtExaminerViewer::getClassTypeId())) {
				if (static_cast<SoQtExaminerViewer*>(ra)->isAnimating())
					static_cast<SoQtExaminerViewer*>(ra)->stopAnimating();
			}
		}
	}
}

//_______________________________________________________
QPixmap IVP13DChannelWidget::getSnapshot(bool transp, int width, bool /*batch*/)
{
	VP1Msg::messageVerbose("IVP13DChannelWidget::getSnapshot()");

    // TODO: check if transp and width can be used in this implementation of the method
	VP1Msg::messageVerbose("(currently, we are not using the values - transp: "+QString::number(transp)+" - width: "+QString::number(width)+")");

	//Unfortunately, the renderer's areas does not render properly by a
	//simple grabWidget. We remedy this the hard way...
	//  m_d->it = m_d->renderareas.begin();
	//  message("Have "+QString::number(m_d->renderareas.size())+" ras");
	//  for (;m_d->it!=m_d->itE;++(m_d->it)) {
	//    SoQtRenderArea* ra = *(m_d->it);
	//    if (ra->isDoubleBuffer()) message("doublebuffer");
	//    if (ra->isDrawToFrontBufferEnable()) message("isDrawToFrontBufferEnable");
	//    if (ra->isQuadBufferStereo()) message("isQuadBufferStereo");
	//    if (ra->getAccumulationBuffer()) message("getAccumulationBuffer");
	//    if (ra->isOverlayRender()) message("isOverlayRender");
	//    if (ra->isClearBeforeRender()) message("isClearBeforeRender");
	//    ra->render();
	//  }
	//  return QPixmap::grabWidget ( this );

	QPixmap pm(geometry().size());

	QPainter painter;
	painter.begin(&pm);
	painter.drawPixmap(0,0,QPixmap::grabWidget ( this ));

	m_d->it = m_d->renderareas.begin();
	for (;m_d->it!=m_d->itE;++(m_d->it)) {
		VP1ExaminerViewer* ra = *(m_d->it);
		QWidget * ra_w = ra->getNormalWidget();
		QPixmap pmra = VP1QtInventorUtils::renderToPixmap(ra, ra_w->geometry().width(),ra_w->geometry().height());
		if (pmra.isNull()) {
			message("Error rendering scene to QPixmap!");
			return QPixmap();
		}
		QPoint pos = ra_w->mapTo( this, QPoint(0,0) );

		painter.drawPixmap(pos.x(),pos.y(),pmra);

	}
	painter.end();

	return pm;
}

//_______________________________________________________
QByteArray IVP13DChannelWidget::saveState()
{
	// ===> Setup stream writing to a byteArray:
	QByteArray byteArray;
	QBuffer buffer(&byteArray);
	buffer.open(QIODevice::WriteOnly);
	QDataStream out(&buffer);

	// ===> Write Data:

	//Version & base state:
	out << (qint32)1; //version
	out << IVP1ChannelWidget::saveState();//Always include state info from the base class.

	// Data from this class:

	//Transparency types (version 0 saved as strings instead of ints):
	QList<int> transptypes;
	m_d->it = m_d->renderareas.begin();
	for (;m_d->it!=m_d->itE;++(m_d->it))
		transptypes << VP1QtInventorUtils::transparencyTypeToInt((*(m_d->it))->getTransparencyType());
	out << transptypes;

	//For any render area that can be casted as an SoQtViewer, we store the camera type and parameters.
	//SoQtViewer::toggleCameraType()
	//todo...

	// ===> Finish up:
	buffer.close();
	return byteArray;

}

//_______________________________________________________
void IVP13DChannelWidget::restoreFromState(QByteArray ba)
{
	// ===> Setup stream for getting the contents of the byteArray:
	QBuffer buffer(&ba);
	buffer.open(QIODevice::ReadOnly);
	QDataStream state(&buffer);
	// ===> Check version and pass on state info to base class:
	qint32 version;
	state >> version;
	if (version!=0&&version!=1) {
		message("Warning: State data in .vp1 file is in wrong format - ignoring!");
		return;
	}

	QByteArray basestate;
	state >> basestate;
	IVP1ChannelWidget::restoreFromState(basestate);
	// ===> Decode the state info:

	//Transparency types:
	if (version==0) {
		QStringList dummy;
		state >> dummy;
	} else {
		QList<int> transptypes;
		state >> transptypes;
		if (transptypes.count()!=static_cast<int>(m_d->renderareas.size())) {
			message("Warning: State data in .vp1 file is in wrong format - ignoring!");
			buffer.close();
			return;
		}
		m_d->it = m_d->renderareas.begin();
		for (;m_d->it!=m_d->itE;++(m_d->it))
			(*(m_d->it))->setTransparencyType(VP1QtInventorUtils::intToTransparencyType(transptypes.takeFirst()));
	}

	//Camera angles:
	//todo..

	// ===> Finish up:
	buffer.close();
}
