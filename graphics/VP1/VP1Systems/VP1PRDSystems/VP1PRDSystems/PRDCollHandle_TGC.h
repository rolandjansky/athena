/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_TGC_H
#define PRDCOLLHANDLE_TGC_H

#include "VP1PRDSystems/PRDCollHandleBase.h"

class PRDCollHandle_TGC : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_TGC(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_TGC();

  bool project() const { return m_project; }

public slots:
  void setProjectionMode( bool );

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);
  bool cut(PRDHandleBase*) { return true; }//accept all
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 1000.0f*1000.0f; }
  QColor defaultColor() const;

private:
  class Imp;
  Imp * m_d;

  //Here for inlines:
  bool m_project;
};

#endif
