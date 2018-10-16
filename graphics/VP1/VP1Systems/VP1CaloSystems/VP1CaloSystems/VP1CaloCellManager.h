/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1CALOSYSTEMS_VP1CALOCELLMANAGER_H
#define VP1CALOSYSTEMS_VP1CALOCELLMANAGER_H

#include "VP1CaloSystems/VP1CaloCells.h"
#include <QObject>

#include <map>

class VP1ExtraSepLayerHelper;
class QCheckBox;
class QDoubleSpinBox;
class ICaloBadChanTool;


//------------------------------------------
// Class VP1CaloCellManager
//------------------------------------------
//
// This class is designed to keep pointers to the VP1CC objects of the similar type.
//    LAr cells are grouped by samplings
//    Tile cells are grouped by regions (Barrel, End cap, Crack)
//
// The objects are stored into two containers - one keeps cells with positive energies, and one with negative energies.
// The containers are represented by multimaps, indexed either by cell energy or by Et


// Notes:
// 1. Consider subclasing from VP1HelperClassBase (?)
//

typedef std::multimap<double,VP1CaloCell*> VP1CCMultimap;
typedef VP1CCMultimap::iterator VP1CCMultimapIterator;

class VP1CaloCellManager : public QObject
{
  Q_OBJECT

 public:
  VP1CaloCellManager(VP1CC_SelectionTypes type,
		     VP1CC_SoNode2CCMap* node2cc,
		     bool useEt,
		     const QPair<bool,double>& scale,
		     bool outline,
		     VP1CC_GlobalCuts globalCuts);

  ~VP1CaloCellManager();

  // Add new VP1CaloCell to the appropriate container
  void add(VP1CaloCell* cell);

  public slots:
    void selectionUpdated(const VP1CCIntervalMap&);
    void scaleUpdated(const QPair<bool,double>&);
    void outlineUpdated(const bool&);
    void globalCutsUpdated(const VP1CC_GlobalCuts&);
    void clipVolumeRadiusChanged(double radius);

 private:
  // ------------ Private methods ------------
  // Update scene with newInterval for pos/neg cells
  void updateScene(VP1Interval newInterval, bool positive);

  // ------------ Private data --------------
  // Selection Type
  VP1CC_SelectionTypes m_type;

  // node2cc map pointer
  VP1CC_SoNode2CCMap* m_node2cc;

  // E/Et mode switch
  bool m_useEt;

  // scale for ed objects
  QPair<bool,double> m_scale;

  // outline flag
  bool m_outline;

  // 2 multimaps for cells with positive and negative energie
  VP1CCMultimap     m_positiveCells;
  VP1CCMultimap     m_negativeCells;

  // Iterators pointing to the first/last Calo Cells with pos/neg energies satisfying selected Energy Thresholds
  VP1CCMultimapIterator m_firstDisplayedPos;
  VP1CCMultimapIterator m_lastDisplayedPos;
  VP1CCMultimapIterator m_firstDisplayedNeg;
  VP1CCMultimapIterator m_lastDisplayedNeg;

  // Current intervals
  VP1CCIntervalPair m_intervals;

  // Global cuts
  VP1CC_GlobalCuts m_globalCuts;
};

typedef std::map<VP1CC_SelectionTypes, VP1CaloCellManager*, std::less<VP1CC_SelectionTypes> > VP1CCManagerContainer;

#endif
