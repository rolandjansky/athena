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

#ifndef VP1CUSTOMTOUREDITOR_H
#define VP1CUSTOMTOUREDITOR_H

#include "VP1Gui/VP1EventDisplaySceneView.h"

#include "VP1Base/IVP1ChannelWidget.h"
#include "VP1Base/VP1Msg.h"

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QSplitter>

class VP1MainWindow;


class VP1EventDisplayEditor : public QWidget {

	Q_OBJECT

public:

	VP1EventDisplayEditor(VP1MainWindow* mainWindow, QList<unsigned long long> listRunEventNumberTimestamp);
	virtual ~VP1EventDisplayEditor();

//	QGraphicsView* getView();

	void addPixmapList(QList<QPixmap>& list, QStringList& listNames);
	void setTabsList(QStringList listNames);

	//  void setState(QByteArray);
	//  QByteArray state() const;


protected:
	void closeEvent(QCloseEvent*);

private slots:
	//  void buttonClicked();
	void setTextLabel();
	void removeObject();
	void addLogo();
	void setBackgroundColor();
	void savePicture();
	void printPicture();
	void getTabSnapshot();
	void enableCustomTextField(bool checked);

private:

	QString getEventDetailsLabel();
	void addTextLabel(QString text, QFont font);

	class Imp;
	Imp * d;
	QGraphicsScene* scene;
//	QGraphicsScene* scene_preview;
	QList< QGraphicsItem* > sceneItems;
//	QGraphicsItem *itemPreview;

	VP1EventDisplaySceneView* preview;

	QStringList _listTabNames;
	QList<IVP1ChannelWidget*> _allTabs;

	VP1MainWindow* _mainWindow;

	int _runNumber;
	unsigned long long _eventNumber;
	unsigned _eventTimestamp;

//	QSplitter *h1Splitter;
//	QSplitter *h2Splitter;
};

#endif
