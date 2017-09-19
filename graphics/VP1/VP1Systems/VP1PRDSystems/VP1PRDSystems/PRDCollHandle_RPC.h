/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_RPC_H
#define PRDCOLLHANDLE_RPC_H

#include "VP1PRDSystems/PRDCollHandleBase.h"

class PRDCollHandle_RPC : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_RPC(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_RPC();

  bool project() const { return m_project; }

public slots:
  void setProjectionMode( bool );

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);
  bool cut(PRDHandleBase*) { return true; }//accept all
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 650.0f*650.0f; }
  QColor defaultColor() const;

private:
  class Imp;
  Imp * m_d;

  //Here for inlines:
  bool m_project;

};

#endif
