/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_STGC_H
#define PRDCOLLHANDLE_STGC_H

#include "VP1PRDSystems/PRDCollHandleBase.h"

class PRDCollHandle_sTGC : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_sTGC(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_sTGC();

  bool project() const { return m_project; }

public slots:
  void setProjectionMode( bool );

protected:
  virtual PRDHandleBase * addPRD(const Trk::PrepRawData*) override;
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
