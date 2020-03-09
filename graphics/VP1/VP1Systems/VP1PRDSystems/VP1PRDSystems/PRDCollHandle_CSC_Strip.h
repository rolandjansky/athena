/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1PRDSYSTEMS_PRDCOLLHANDLE_CSC_STRIP_H
#define VP1PRDSYSTEMS_PRDCOLLHANDLE_CSC_STRIP_H

#include "VP1PRDSystems/PRDCollHandleBase.h"

class PRDCollHandle_CSC_Strip : public PRDCollHandleBase {

  Q_OBJECT

public:

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  PRDCollHandle_CSC_Strip(PRDSysCommonData *,const QString& key);
  virtual ~PRDCollHandle_CSC_Strip();

  bool project() const { return m_project; }

public slots:
  void setProjectionMode( bool );

protected:
  virtual PRDHandleBase * addPRD(const Trk::PrepRawData*) override;
  virtual bool cut(PRDHandleBase*) override { return true; }//accept all
  virtual void setupSettingsFromControllerSpecific(PRDSystemController*) override;
  virtual float lodArea() const override { return 800.0f*800.0f; }

  virtual QColor defaultColor() const override;

private:
  class Imp;
  Imp * m_d;

  //Here for inlines:
  bool m_project;

};

#endif
