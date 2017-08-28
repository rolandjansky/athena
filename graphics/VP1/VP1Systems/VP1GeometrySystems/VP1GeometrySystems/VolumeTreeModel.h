/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VOLUMETREEMODEL_H
#define VOLUMETREEMODEL_H

#include <QAbstractItemModel>

#include "VP1GeometrySystems/VolumeHandle.h"
#include "VP1GeometrySystems/VP1GeoFlags.h"
#include "VP1Base/VP1Msg.h"

class VolumeTreeModel : public QAbstractItemModel {

  Q_OBJECT

public:

  VolumeTreeModel( QObject * parent = 0 );
  virtual ~VolumeTreeModel();

  //These are the methods used by the geometry system:
  void addSubSystem( VP1GeoFlags::SubSystemFlag flag,
		     const VolumeHandle::VolumeHandleList& roothandles );
  void enableSubSystem(VP1GeoFlags::SubSystemFlag flag);
  void disableSubSystem(VP1GeoFlags::SubSystemFlag flag);

  void getRootHandles(std::vector<std::pair<VolumeHandle::VolumeHandleListItr,VolumeHandle::VolumeHandleListItr> >&) const;

  //The following are for use by the view:
  QModelIndex index(int, int, const QModelIndex&) const;
  QModelIndex parent(const QModelIndex&) const;
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex& idx) const { return rowCount(idx) > 0 ? 1 : 0; }
  QVariant data(const QModelIndex&, int) const;
  Qt::ItemFlags flags(const QModelIndex &index) const;
  QVariant headerData(int section, Qt::Orientation orientation,int role) const;
  bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
  bool canFetchMore ( const QModelIndex & parent ) const;
  void fetchMore ( const QModelIndex & parent );

  //To be called from system uncreate:
  void cleanup();
private:
  class Imp;
  Imp * d;

};

#endif
