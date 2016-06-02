/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////
//                                                            //
//                                                            //
//  Header file for class MissingEtCollHandle                 //
//                                                            //
//  Description: Collection handle for MissingEt              //
//                                                            //
//  June 2014 - Riccardo.Maria.Bianchi@cern.ch                //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef MissingEtCollHandle_H
#define MissingEtCollHandle_H

#include "AODCollHandleBase.h"

#include "xAODBase/ObjectType.h"

//class SoMaterial;
class MissingEtCollectionSettingsButton;


class MissingEtCollHandle : public AODCollHandleBase {

  Q_OBJECT

public:

  // Loops
  // Must be called from derived classes when filling in new object handles.
  virtual void hintNumberOfHandlesInEvent(unsigned);
  void addHandle(AODHandleBase*);

  //For iterating over object handles:
  virtual void handleIterationBegin();
  virtual AODHandleBase* getNextHandle(); //Returns 0 when done.
  QList<AODHandleBase*> getHandlesList() const;

  static QStringList availableCollections(IVP1System*);//For the collection widget.

  MissingEtCollHandle( AODSysCommonData *, const QString& name, xAOD::Type::ObjectType type );
  virtual ~MissingEtCollHandle();

  virtual void init(VP1MaterialButtonBase* matBut=0);//reimplementations must start with a call to this.
  virtual void setupSettingsFromControllerSpecific(AODSystemController*);

  const MissingEtCollectionSettingsButton& collSettingsButton() const;
  //  bool isRandomColors() const;

  // This is created in this class, but passed to the JetCollectionButton for control etc. It is used in the Handle.
  //  SoMaterial* defaultParameterMaterial() const;

  /*
   * Save and restore settings
   * (implementation of the virtual functions in the base class)
   */
  QByteArray persistifiableState() const; // save state
  void setState(const QByteArray&); // restore state


public slots:
  void setCutAllowedPhi(const QList<VP1Interval>&);

protected:	
  virtual bool load();
  virtual bool cut(AODHandleBase*);
  virtual QColor defaultColor() const { return QColor::fromRgbF(1.f, 1.f, 1.f); } // default color: White


  // getters
  QList<VP1Interval> getCutAllowedPhi() {return m_cut_allowedPhi; };
  bool getPhiAllowall() {return m_cut_phi_allowall; };


private slots:

	void resetCachedValuesCuts();
//  void showParametersChanged(bool);
//  void setScale(const double& s);

  // setters
  void setMetSize(int size);

//  double scale() const;
  //    void setRandomJetColours(const bool&);
  //    void rerandomise();

private:

  class Imp;
  Imp * d;

  QList<VP1Interval> m_cut_allowedPhi;
  bool m_cut_phi_allowall;
  bool m_cut_phi_allwillfail;
};

#endif
