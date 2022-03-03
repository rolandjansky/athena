/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "VP1CaloSystems/VP1CaloCellManager.h"

#include "CaloEvent/CaloCellContainer.h"

#include <QCheckBox>
#include <QDoubleSpinBox>

VP1CaloCellManager::VP1CaloCellManager(VP1CC_SelectionTypes type,
				       VP1CC_SoNode2CCMap* node2cc,
				       bool useEt,
				       const QPair<bool,double>& scale,
				       bool outline,
				       const VP1CC_GlobalCuts& globalCuts):
  m_type(type),
  m_node2cc(node2cc),
  m_useEt(useEt),
  m_scale(scale),
  m_outline(outline),
  m_globalCuts(globalCuts)
{
  // Initialize private iterators
  m_firstDisplayedPos = m_positiveCells.end();
  m_lastDisplayedPos  = m_positiveCells.end();
  m_firstDisplayedNeg = m_negativeCells.end();
  m_lastDisplayedNeg  = m_negativeCells.end();

  m_intervals = VP1CCIntervalPair(VP1Interval(),VP1Interval());
}

VP1CaloCellManager::~VP1CaloCellManager()
{
  // Clear multimaps
  VP1CCMultimapIterator it = m_positiveCells.begin();
  while(it!=m_positiveCells.end()) {
    if(it->second) {
      delete it->second;
      it->second = 0;
    }
    ++it;
  }

  it = m_negativeCells.begin();
  while(it!=m_negativeCells.end()) {
    if(it->second) {
      delete it->second;
      it->second = 0;
    }
    ++it;
  }

  m_positiveCells.clear();
  m_negativeCells.clear();
}

void VP1CaloCellManager::add(VP1CaloCell* cell)
{
  double energy = cell->getCaloCell()->energy();

  if(m_useEt)
    energy = cell->energyToTransverse(energy);

  // We need to choose which map to add the new cell to according to its energy sign
  VP1CCMultimap& useMap = energy < 0 ? m_negativeCells : m_positiveCells;

  // This is going to be the new key in the map
  double ccKey = fabs(energy);

  // Add new element to the map
  useMap.emplace(ccKey,cell);
}

// ----------------- Slots --------------------------
void VP1CaloCellManager::selectionUpdated(const VP1CCIntervalMap& intervals)
{
  VP1CCIntervalMap::const_iterator it = intervals.find(m_type);
  if(it!=intervals.end()) {
    // Update scene with pos/neg cells
    updateScene(it->first,true);
    updateScene(it->second,false);
  }
}

void VP1CaloCellManager::scaleUpdated(const QPair<bool,double>& new_scale)
{
  m_scale = new_scale;

  // Redraw objects with new scale
  // Pos
  for(VP1CCMultimapIterator it=m_firstDisplayedPos; it!=m_lastDisplayedPos; ++it) {
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  }

  // Neg
  for(VP1CCMultimapIterator it=m_firstDisplayedNeg; it!=m_lastDisplayedNeg; ++it) {
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  }
}

void VP1CaloCellManager::outlineUpdated(const bool& new_outline)
{
  m_outline = new_outline;

  // Redraw objects with new outline settings
  // Pos
  for(VP1CCMultimapIterator it=m_firstDisplayedPos; it!=m_lastDisplayedPos; ++it) {
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  }

  // Neg
  for(VP1CCMultimapIterator it=m_firstDisplayedNeg; it!=m_lastDisplayedNeg; ++it) {
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  }
}

void VP1CaloCellManager::globalCutsUpdated(const VP1CC_GlobalCuts& new_cuts)
{
  m_globalCuts = new_cuts;
  for(VP1CCMultimapIterator it=m_firstDisplayedPos; it!=m_lastDisplayedPos; ++it)
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  for(VP1CCMultimapIterator it=m_firstDisplayedNeg; it!=m_lastDisplayedNeg; ++it)
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
}

void VP1CaloCellManager::clipVolumeRadiusChanged(double radius)
{
  m_globalCuts.clipRadius=radius; // Adjust radius to match new value
  for(VP1CCMultimapIterator it=m_firstDisplayedPos; it!=m_lastDisplayedPos; ++it)
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
  for(VP1CCMultimapIterator it=m_firstDisplayedNeg; it!=m_lastDisplayedNeg; ++it)
    it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
}

// ----------------- Private methods ------------------------
void VP1CaloCellManager::updateScene(const VP1Interval& newInterval, bool positive)
{
  // We need to choose which map to work with, according to the value of 'positive' flag
  VP1CCMultimap* useMap = 0;
  VP1CCMultimapIterator* useItFirst = 0;
  VP1CCMultimapIterator* useItLast = 0;
  VP1Interval* currentInterval = 0;

  if(positive) {
    useMap = &m_positiveCells;
    useItFirst = &m_firstDisplayedPos;
    useItLast = &m_lastDisplayedPos;
    currentInterval = &m_intervals.first;
  } else {
    useMap = &m_negativeCells;
    useItFirst = &m_firstDisplayedNeg;
    useItLast = &m_lastDisplayedNeg;
    currentInterval = &m_intervals.second;
  }

  // If new iterval is equal to the existing one just check global cuts
  if(*currentInterval==newInterval) {
    for(VP1CCMultimapIterator it=*useItFirst; it!=*useItLast; ++it)
      it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
    return;
  }

  // If new interval is empty hide everything and update the current interval
  if(newInterval.isEmpty()) {
    for(VP1CCMultimapIterator it=*useItFirst; it!=*useItLast; ++it)
      it->second->remove3DObjects(m_node2cc);
    *currentInterval = newInterval;
    *useItFirst = useMap->end();
    *useItLast = useMap->end();
    return;
  }

  // If old interval was empty, find values for first & last iterators and
  // display everything between them, taking into account global cuts
  if(currentInterval->isEmpty()) {
    *useItFirst = useMap->lower_bound(newInterval.lower());
    if(newInterval.upper() == VP1Interval::inf())
      *useItLast = useMap->end();
    else
      *useItLast = useMap->upper_bound(newInterval.upper());

    for(VP1CCMultimapIterator it=*useItFirst; it!=*useItLast; ++it)
      it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);

    *currentInterval = newInterval;
    return;
  }

  // No one is empty.
  VP1CCMultimapIterator newIterator;

  if(currentInterval->lower() != newInterval.lower() &&
     currentInterval->upper() != newInterval.upper()) {
    // This is unexpected!
    // Hide everything what was there before and display new thresholds
    for(VP1CCMultimapIterator it=*useItFirst; it!=*useItLast; ++it)
      it->second->remove3DObjects(m_node2cc);

    *useItFirst = useMap->lower_bound(newInterval.lower());
    if(newInterval.upper() == VP1Interval::inf())
      *useItLast = useMap->end();
    else
      *useItLast = useMap->upper_bound(newInterval.upper());

    for(VP1CCMultimapIterator it=*useItFirst; it!=*useItLast; ++it)
      it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);

  } else if (currentInterval->lower() != newInterval.lower()) {
    // Adjust lower threshold
    newIterator = useMap->lower_bound(newInterval.lower());

    if(currentInterval->lower() < newInterval.lower())
      // May need to hide something
      for(VP1CCMultimapIterator it=*useItFirst; it!=newIterator; ++it)
	it->second->remove3DObjects(m_node2cc);
    else
      // May need to add new objects to the scene
      for(VP1CCMultimapIterator it=newIterator; it!=*useItFirst; ++it)
	it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);

    *useItFirst = newIterator;

  } else {

    // Adjust upper threshold
    if(newInterval.upper() == VP1Interval::inf()) {
      newIterator = useMap->end();

      // May need to add new objects to the scene
      for(VP1CCMultimapIterator it=*useItLast; it!=newIterator; ++it)
	it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
    } else {
      newIterator = useMap->upper_bound(newInterval.upper());

      if(*useItLast == useMap->end() ||
	 currentInterval->upper() > newInterval.upper()) {
	// May need to hide something
	for(VP1CCMultimapIterator it=newIterator; it!=*useItLast; ++it)
	  it->second->remove3DObjects(m_node2cc);
      } else {
	// May need to add new objects to the scene
	for(VP1CCMultimapIterator it=*useItLast; it!=newIterator; ++it)
	  it->second->updateScene(m_node2cc,m_useEt,m_scale,m_outline,m_globalCuts);
      }
    }
    *useItLast = newIterator;
  }

  *currentInterval = newInterval;
}
