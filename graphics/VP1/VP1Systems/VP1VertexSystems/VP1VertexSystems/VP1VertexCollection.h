/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1VertexCollection                 //
//                                                            //
//  Description: Collections of reconstructed vertices        //
//                                                            //
//  Author: Andreas.Wildauer@cern.ch                          //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//          Ben Zastovnik                                     //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1VERTEXCOLLECTION_H
#define VP1VERTEXCOLLECTION_H

#include "VP1Base/VP1StdCollection.h"


class VertexSysController;
class SoPath;

namespace Trk{
  class Track;
}

class VP1VertexCollection : public VP1StdCollection {

  Q_OBJECT

public:

  static QList<VP1StdCollection*> createCollections(VertexSysController*controller);

  VP1VertexCollection(VertexSysController*controller,const QString& key);
  virtual ~VP1VertexCollection();
  
  void init(VP1MaterialButtonBase* mat=0);
  QList<QWidget*> provideExtraWidgetsForGuiRow() const;

  QStringList infoOnClicked(SoPath* pickedPath);//Provides info + pops path appropriately.

  enum COLOURBY { COLOUR_PERCOLLECTION, COLOUR_RANDOM};// FIXME - to StdCollection
  COLOURBY colourBy() const { return m_colourby; }
  virtual COLOURBY defaultColourBy () const { return COLOUR_PERCOLLECTION; }
  void setColourBy(COLOURBY);
protected:
  qint32 provideCollTypeID() const { return 0; }//Vertex col. type: 0 for reconstructed vertices, 1 for truth vertices.
  QString provideSection() const { return "Reconstructed Vertices"; };
  QString provideSectionToolTip() const { return "Collections of reconstructed vertices"; }
  QString provideText() const;
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();

  //FIXME: We need something like this for the trackparticles too:
signals:
  void tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >&);  
private slots:
  void colourByComboBoxItemChanged();
  void collVisibilityChanged(bool vis);
  void rerandomiseRandomVertexColours();
public slots:

  void recheckAllCuts();
  void updateAllShapes();
private:

  class Imp;
  Imp * m_d;
  COLOURBY m_colourby;
};

#endif
