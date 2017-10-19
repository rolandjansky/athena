/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRDCOLLHANDLE_CSC_H
#define PRDCOLLHANDLE_CSC_H

#include "VP1PRDSystems/PRDCollHandleBase.h"

class PRDCollHandle_CSC : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_CSC(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_CSC();

  bool project() const { return m_project; }

public slots:
  void setProjectionMode( bool );

protected:
  PRDHandleBase * addPRD(Trk::PrepRawData*);
  bool cut(PRDHandleBase*) { return true; }//accept all
  void setupSettingsFromControllerSpecific(PRDSystemController*);
  float lodArea() const { return 800.0f*800.0f; }

  QColor defaultColor() const;

private:
  class Imp;
  Imp * m_d;

  //Here for inlines:
  bool m_project;

};

#endif
