/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1TruthVertexCollection            //
//                                                            //
//  Description: Collections of truth vertices.               //
//                                                            //
//  Author: Andreas.Wildauer@cern.ch                          //
//          Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//          Ben Zastovnik                                     //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1TRUTHVERTEXCOLLECTION_H
#define VP1TRUTHVERTEXCOLLECTION_H

#include "VP1Base/VP1StdCollection.h"

namespace Trk {
  class Track;
}

class SoPath;
class VertexSysController;

class VP1TruthVertexCollection : public VP1StdCollection {

  Q_OBJECT

public:

  static QList<VP1StdCollection*> createCollections(VertexSysController*);

  VP1TruthVertexCollection(VertexSysController*,const QString& key);
  virtual ~VP1TruthVertexCollection();

  QStringList infoOnClicked(SoPath* pickedPath);//Provides info + pops path appropriately.

protected:
  qint32 provideCollTypeID() const { return 1; }//Vertex col. type: 0 for reconstructed vertices, 1 for truth vertices.
  QString provideSection() const { return "Truth Vertices"; };
  QString provideSectionToolTip() const { return "Collections of truth vertices"; }
  QString provideText() const;
  void assignDefaultMaterial(SoMaterial*) const;
  bool load();
signals:
  void tracksFromVertexChanged(QList< std::pair<const SoMaterial*, QList< const Trk::Track*> > >&);  

private slots:
  void recheckAllCuts();
  void updateAllShapes();

private:

  class Imp;
  Imp * m_d;

};

#endif
