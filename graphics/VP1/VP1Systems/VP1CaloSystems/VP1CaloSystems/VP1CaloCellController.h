/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOSYSTEMS_VP1CALOCELLCONTROLLER_H
#define VP1CALOSYSTEMS_VP1CALOCELLCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VP1CaloSystems/VP1CaloCells.h"
#include <QByteArray>
#include <QString>
#include <map>

class TileRawChannel;
class TileRawChannelContainer;
class TileHWID;
class TileInfo;
class TileCablingService;

//class QwtPlot; // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
class QLabel;
class SoGroup;

//------------------------------------------
// Class VP1CaloCellController
//------------------------------------------
//
// Implementation of the VP1Controller mechanism for VP1CaloCellSystem
//

class VP1CaloCellController : public VP1Controller
{
  Q_OBJECT

 public:
  VP1CaloCellController(IVP1System* sys);
  virtual ~VP1CaloCellController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  //To put at start of the system scenegraph:
  SoGroup* drawOptions() const;

  // -------------- Digits display ----------------
  // We need these pointers for displaying Tile Pulse
  void initTilePulse(const TileHWID* _tile_hw_id,
		     const TileInfo* _tile_info,
		     const TileCablingService* _tile_cabling);

  // If digits cannot be displayed by whatever reason, then hide both of the digit forms and say they have no data
  void ClearHideDigitForms();

  // Delete digit forms and set pointers to 0
  void  DeleteDigitForms();

  // Enable/disable Digits checkbox
  void EnableDigitsCheckbox(bool enable);

  void displayLArDigits(int n_samples,
			std::vector<short> samples,
			std::vector<std::string>& msg);

  void displayTileDigits(int n_samples,
			 std::vector<float> samples1,
			 std::vector<float> samples2,
			 std::vector<std::string>& msg);

  void displayTileDigits(int n_samples,
			 std::vector<float> samples,
			 std::vector<std::string>& msg);

  void displayTilePulse(const TileRawChannel* rawchannel1,
			const TileRawChannel* rawchannel2,
			const TileRawChannelContainer* rawchannel_cont,
			size_t digitsize);

  void displayTilePulse(const TileRawChannel* rawchannel,
			const TileRawChannelContainer* rawchannel_cont,
			size_t digitsize);

  // -------------- Digits display ----------------

  // Return material by separator type
  SoMaterial* GetMaterial(VP1CC_SeparatorTypes type);

  VP1CCIntervalMap selectionIntervals() const;
  QPair<bool,double> scale() const;   //The bool indicates if it is logscale(true) or linear scale(false).
  VP1Interval selectionMbts() const;
  bool showDigits() const;
  bool showVolumeOutLines() const;
  bool energyModeEt() const;
  VP1CC_GlobalCuts globalCuts() const;

 signals:
  void selectionIntervalsChanged(const VP1CCIntervalMap&);
  void scaleChanged(const QPair<bool,double>&);
  void selectionMbtsChanged(const VP1Interval&);
  void showDigitsChanged(const bool&);
  void showVolumeOutLinesChanged(bool);
  void energyModeEtChanged(bool);
  void globalCutsChanged(const VP1CC_GlobalCuts&);

 private:
  class Imp;
  Imp * d;

  const TileHWID*            tile_hw_id;
  const TileInfo*            tile_info;
  const TileCablingService*  tile_cabling;

  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods
  static QString toString(const VP1CCIntervalMap& m) { return "VP1CCIntervalMap of size "+QString::number(m.count()); }
  static QString toString(const QPair<bool,double>& par) { return "<"+QString(par.first?"log":"linear")+", "+QString::number(par.second/(CLHEP::cm/CLHEP::GeV))+" cm/GeV>"; }
  static QString toString(const VP1CC_GlobalCuts& cuts) { return "VP1CC global cuts: sideA=" + QString(cuts.sideA?"True":"False") + ", sideC=" + QString(cuts.sideC?"True":"False") + ", allowedEta=" + VP1Controller::toString(cuts.allowedEta) + ", allowedPhi="  + VP1Controller::toString(cuts.allowedEta); }

//  // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//  void DrawTileDigits(QwtPlot* plot,
//		      int n_samples,
//		      std::vector<float> samples);

//  // FIXME:You have to compile Qwt with Qt5. LCG's Qwt is compiled with Qt4 only...
//  void DrawTilePulse(QwtPlot* plot,
//		     QLabel* gainlabel,
//		     const TileRawChannel* rawchannel,
//		     const TileRawChannelContainer* rawchannel_cont,
//		     size_t digitsize);

 private slots:
  void possibleChange_selectionIntervals();
  void possibleChange_scale();
  void possibleChange_selectionMbts();
  void possibleChange_showDigits();
  void possibleChange_showVolumeOutLines();
  void possibleChange_energyModeEt();
  void possibleChange_globalCuts();

  void enableUpperThreshold();
  void changeMode();
};

#endif
