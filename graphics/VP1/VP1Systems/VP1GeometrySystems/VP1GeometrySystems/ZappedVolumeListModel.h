/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZAPPEDVOLUMELISTMODEL_H
#define ZAPPEDVOLUMELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "VP1GeometrySystems/VolumeHandle.h"

class QWidget;
class QToolBox;

class ZappedVolumeListModel : public QAbstractListModel {

  Q_OBJECT

public:

  ZappedVolumeListModel(QObject * parent = 0);

  virtual ~ZappedVolumeListModel();

  inline int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  inline Qt::ItemFlags flags ( const QModelIndex & index ) const;
  inline QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

  //For use by VolumeHandleSharedData's:
  inline void addToZappedVolumes( VolumeHandle * );//Assumes that the handle is known not to be zapped before.
  inline void removeFromZappedVolumes( VolumeHandle * );//Assumes that the handle is known to be zapped before.

public slots:
  void activated(const QModelIndex&);

private:

  QList<VolumeHandle*> m_zappedhandles;
};

/////////////////
//   INLINES   //
/////////////////

inline int ZappedVolumeListModel::rowCount ( const QModelIndex & parent ) const
{
  return parent.isValid() ? 0 : m_zappedhandles.count();
}

inline Qt::ItemFlags ZappedVolumeListModel::flags ( const QModelIndex & index ) const
{
  return index.isValid() ? (Qt::ItemIsEnabled|Qt::ItemIsSelectable) : static_cast<Qt::ItemFlags>(0);
}


inline QVariant ZappedVolumeListModel::data ( const QModelIndex & index, int role ) const
{
  return (role!=Qt::DisplayRole||!index.isValid()) ? QVariant() : m_zappedhandles.at(index.row())->getName();
}

inline void ZappedVolumeListModel::addToZappedVolumes( VolumeHandle * handle )
{
  beginResetModel(); // see: http://doc.qt.io/qt-5/qabstractitemmodel-obsolete.html
  m_zappedhandles << handle;
  endResetModel();
}

inline void ZappedVolumeListModel::removeFromZappedVolumes( VolumeHandle * handle )
{
  beginResetModel();
  m_zappedhandles.removeAll(handle);
  endResetModel();
}

#endif
