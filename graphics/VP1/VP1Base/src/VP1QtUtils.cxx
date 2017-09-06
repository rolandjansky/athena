/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1QtUtils                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: April 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1Msg.h"
#include <cstdlib>//For setenv/unsetenv
#include <QImage>
#include <QColor>

//____________________________________________________________________
bool VP1QtUtils::environmentVariableIsSet(const QString& name)
{
  const bool set = !qgetenv(name.toStdString().c_str()).isNull();
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1QtUtils::environmentVariableIsSet called for variable '"+name+"'. Returning "+VP1Msg::str(set));
  return set;
}

//____________________________________________________________________
QString VP1QtUtils::environmentVariableValue(const QString& name)
{
  QByteArray env = qgetenv(name.toStdString().c_str());
  QString val(env.isNull() ? "" : QString ( env ));
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1QtUtils::environmentVariableValue called for variable '"+name+"'. Returning '"+ val+"'.");
  return val;
}

//____________________________________________________________________
bool VP1QtUtils::environmentVariableIsOn(const QString& name)
{
  QByteArray env = qgetenv(name.toStdString().c_str());
  QString val(env.isNull() ? "" : QString ( env ));
  bool ison = true;
  if (val==""||val=="0")
    ison = false;
  else if (0==QString::compare(val,"false",Qt::CaseInsensitive))
    ison = false;
  else if (0==QString::compare(val,"off",Qt::CaseInsensitive))
    ison = false;
  else if (0==QString::compare(val,"no",Qt::CaseInsensitive))
    ison = false;
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1QtUtils::environmentVariableIsOn called for variable '"+name+"'. Returning "+ VP1Msg::str(ison)+".");
  return ison;
}

//____________________________________________________________________
void VP1QtUtils::setEnvironmentVariable( const QString& name, const QString& content )
{
  if (name.isEmpty()) {
    VP1Msg::messageDebug("VP1QtUtils::setEnvironmentVariable WARNING: called with empty variable name!");
    return;
  }

  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1QtUtils::setEnvironmentVariable: Setting variable '"+name+"' to value '"+content+"'.");

  //Don't see any Qt function for this, so we go directly to the stdlib call:
  ::setenv(name.toStdString().c_str(),content.toStdString().c_str(),1/*override present value*/);
}

//____________________________________________________________________
void VP1QtUtils::unsetEnvironmentVariable( const QString& name )
{
  if (name.isEmpty()) {
    VP1Msg::messageDebug("VP1QtUtils::unsetEnvironmentVariable WARNING: called with empty variable name!");
    return;
  }
  //Don't see any Qt function for this, so we go directly to the stdlib call:

  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VP1QtUtils::unsetEnvironmentVariable: Unsetting variable '"+name+"'.");

  ::unsetenv(name.toStdString().c_str());
}

//____________________________________________________________________
QImage VP1QtUtils::fadeImage(QImage img0, QImage img1, double fadefact ) {
  if (fadefact<0.0||fadefact>1.0||img0.isNull()||img1.isNull()||img0.width()!=img1.width()||img0.height()!=img1.height())
    return QImage();
  if (fadefact==0)
    return img0;
  if (fadefact==1)
    return img1;
  QImage img = img0;
  double oneminusfadefact(1.0-fadefact);
  const int w(img.width()), h(img.height());
  for (int iw = 0; iw<w; ++iw)
    for (int ih = 0; ih<h; ++ih) {
      QColor col0(img0.pixel(iw,ih));
      QColor col1(img1.pixel(iw,ih));
      img.setPixel ( iw,ih,QColor::fromRgbF(col0.redF()*oneminusfadefact + col1.redF()*fadefact,
					    col0.greenF()*oneminusfadefact + col1.greenF()*fadefact,
					    col0.blueF()*oneminusfadefact + col1.blueF()*fadefact).rgb() );
    }
  return img;
}
