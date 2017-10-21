/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VertexSysController                 //
//                                                            //
//  Description: Vertex system controller.                    //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: July 2008                                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VERTEXSYSCONTROLLER_H
#define VERTEXSYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1VertexSystems/VertexCommonFlags.h"
class SoGroup;

class VertexSysController : public VP1Controller {

  Q_OBJECT

public:

  VertexSysController(IVP1System * sys);
  virtual ~VertexSysController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  VP1CollectionWidget * collWidget() const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  SoGroup * drawOptions() const;

  //Interactions (no change signals needed):
  bool printInfoOnClick() const;
  bool printVerboseInfoOnClick() const;
  bool zoomOnClick() const;

  //Truth cuts:
  bool truthCutPrimaryVertexOnly() const;
  QPair<VertexCommonFlags::QUANTITY,VP1Interval> truthCutQuantity() const;
  VP1Interval truthCutAllowedEta() const;
  QList<VP1Interval> truthCutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[

  //Truth display options:
  QPair<VertexCommonFlags::QUANTITY,double> truthCrossLength() const;//.second is either the fixed length (when <0),
                                                                     //or the scale factor (to .first)

  //Recon cuts:
  VertexCommonFlags::ReconVertexTypeFlags reconCutAllowedTypes() const;

  //Recon display options:
  double reconVertexRepresentation() const;//<0: sphere of that radius, 0 a point, >0 ellipsoid with that many stddevs.

  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void rerandomise();//This one doesn't have an access method, but is just emitted when needed.
  void truthCutAllowedEtaChanged(const VP1Interval&);
  void truthCutAllowedPhiChanged(const QList<VP1Interval>&);
  void truthCutPrimaryVertexOnlyChanged(bool);
  void truthCutQuantityChanged(const QPair<VertexCommonFlags::QUANTITY,VP1Interval>&);
  void truthCrossLengthChanged(QPair<VertexCommonFlags::QUANTITY,double>);
  void reconCutAllowedTypesChanged(VertexCommonFlags::ReconVertexTypeFlags);
  void reconVertexRepresentationChanged(const double&);

private:
  class Imp;
  Imp * m_d;
  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods
  static QString toString( const VertexCommonFlags::QUANTITY& par ) { return VertexCommonFlags::toString(par); }
  static QString toString( const QPair<VertexCommonFlags::QUANTITY,VP1Interval>& );
  static QString toString( const QPair<VertexCommonFlags::QUANTITY,double>& );

private slots:
  void emitRerandomise();
  void possibleChange_truthCutAllowedEta();
  void possibleChange_truthCutAllowedPhi();
  void possibleChange_truthCutPrimaryVertexOnly();
  void possibleChange_truthCutQuantity();
  void possibleChange_truthCrossLength();
  void possibleChange_reconCutAllowedTypes();
  void possibleChange_reconVertexRepresentation();
};

#endif
