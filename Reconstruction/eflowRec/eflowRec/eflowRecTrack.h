/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * eflowRecTrack.h
 *
 *  Created on: 30.09.2013
 *      Author: tlodd
 */

#ifndef EFLOWRECTRACK_H_
#define EFLOWRECTRACK_H_

#include <iostream>
#include <cassert>
#include <map>
#include <string>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "CxxUtils/fpcompare.h"

#include "AthLinks/ElementLink.h"

#include "Particle/TrackParticleContainer.h"

#include "eflowRec/eflowTrackCaloPoints.h"
#include "eflowRec/eflowRingSubtractionManager.h"
#include "eflowRec/PFMatchInterfaces.h"

#include "xAODTracking/TrackParticle.h"
#include "xAODTracking/TrackParticleContainer.h"

class eflowTrackClusterLink;
class eflowTrackExtrapolatorBaseAlgTool;

class IMessageSvc;
class ISvcLocator;

class eflowRecTrack {
public:
  eflowRecTrack(const ElementLink<xAOD::TrackParticleContainer>& trackElemLink,
                const ToolHandle<eflowTrackExtrapolatorBaseAlgTool>& theTrackExtrapolatorTool);
  eflowRecTrack(const eflowRecTrack& originalEflowRecTrack);
  eflowRecTrack& operator = (const eflowRecTrack& originalEflowRecTrack);
  virtual ~eflowRecTrack();

  const xAOD::TrackParticle* getTrack() const { return m_track; }

  const eflowTrackCaloPoints& getTrackCaloPoints() const { return *m_trackCaloPoints; }

  ElementLink<xAOD::TrackParticleContainer> getTrackElemLink() const { return m_trackElemLink; }
  void addClusterMatch(eflowTrackClusterLink* clusterMatch) { m_clusterMatches.push_back(clusterMatch); }
  void addAlternativeClusterMatch(eflowTrackClusterLink* clusterMatch, std::string key) { m_alternativeClusterMatches[key].push_back(clusterMatch); }

  eflowRingSubtractionManager& getCellSubtractionManager() { return m_ringSubtractionManager; }

  int getType() const { return m_type; }

  const std::vector<eflowTrackClusterLink*>& getClusterMatches() const { return m_clusterMatches; }
  void clearClusterMatches() { m_clusterMatches.clear(); }

  const std::vector<eflowTrackClusterLink*>* getAlternativeClusterMatches(std::string key) const;// { return m_alternativeClusterMatches.at(key); }

  bool hasBin() const { return m_hasBin; }
  void setHasBin(bool hasBin) { m_hasBin = hasBin; }

  void setCaloDepthArray(const double* depthArray);
  const std::vector<double>& getCaloDepthArray() const { return m_caloDepthArray; }

  bool isSubtracted() const { return m_isSubtracted; }

  void setSubtracted() {
    if (isSubtracted()){
      MsgStream* mlog = m_mlog.get();
      std::string errorString = "Invoke setSubtracted() on track that is subtracted already!";
      if (mlog) (*mlog) << MSG::WARNING << errorString << endmsg;
      else {
	std::string errorPrefix = "eflowRecTrack: WARNING";
	std::cerr << errorPrefix << " - have invalid pointer to MsgStream service " << std::endl;
	std::cerr << errorPrefix << errorString << std::endl;
      }//if don't have valid pointer to mlog service, warn and use cerr
      return;
    }//if track was already subtracted then print a warning to the user about that and return
    m_isSubtracted = true;
  }

  void setEExpect(double eExpect, double varEExpect){ m_eExpect = eExpect; m_varEExpect = varEExpect; }
  double getEExpect() const { return m_eExpect; }
  double getVarEExpect() const { return m_varEExpect; }
  int getTrackId() const { return m_trackId; }
  void setTrackId(int trackId) { m_trackId = trackId; }
  
  
  int getLayerHED() const { return m_layerHED; }
  void setLayerHED(int layerHED) { m_layerHED = layerHED; }
  
  std::vector<int> getLayerCellOrderVector() const { return m_layerCellOrderVector; }
  void setLayerCellOrderVector(std::vector<int> layerToStoreVector) { m_layerCellOrderVector = layerToStoreVector; }
  
  std::vector<float> getRadiusCellOrderVector() const { return m_radiusCellOrderVector; }
  void setRadiusCellOrderVector(std::vector<float> radiusToStoreVector) { m_radiusCellOrderVector = radiusToStoreVector; }
  
  std::vector<float> getAvgEDensityCellOrderVector() const { return m_avgEdensityCellOrderVector; }
  void setAvgEDensityCellOrderVector(std::vector<float> avgEdensityToStoreVector) { m_avgEdensityCellOrderVector = avgEdensityToStoreVector; }
  
  
  bool isInDenseEnvironment() const { return m_isInDenseEnvironment;}
  void setIsInDenseEnvironment() { m_isInDenseEnvironment = true; }
  
  void setpull15(double pull15){ m_pull15 = pull15; }
  double getpull15() const { return m_pull15; }

private:

  int m_trackId;
  ElementLink<xAOD::TrackParticleContainer> m_trackElemLink;
  const xAOD::TrackParticle* m_track;
  int m_type;
  double m_pull15;
  
  int m_layerHED;
  std::vector<int> m_layerCellOrderVector;
  std::vector<float> m_radiusCellOrderVector;
  std::vector<float> m_avgEdensityCellOrderVector;

  double m_eExpect;
  double m_varEExpect;

  bool m_isInDenseEnvironment;
  bool m_isSubtracted;
  bool m_hasBin;

  std::vector<double> m_caloDepthArray;

  std::unique_ptr<eflowTrackCaloPoints> m_trackCaloPoints;
  eflowRingSubtractionManager m_ringSubtractionManager;
  std::vector<eflowTrackClusterLink*> m_clusterMatches;
  std::map<std::string,std::vector<eflowTrackClusterLink*> > m_alternativeClusterMatches;

  ISvcLocator* m_svcLoc;
  IMessageSvc* m_msgSvc;
  std::unique_ptr<MsgStream> m_mlog;

public:
  class SortDescendingPt {
  public:
    bool operator()(const eflowRecTrack* a, const eflowRecTrack* b) {
      return CxxUtils::fpcompare::greater(a->getTrack()->pt(),
                                          b->getTrack()->pt());
    }
  };
};
class eflowRecMatchTrack: public PFMatch::ITrack {
public:
  eflowRecMatchTrack(const eflowRecTrack* efRecTrack): m_efRecTrack(efRecTrack) { assert(m_efRecTrack); }
  virtual ~eflowRecMatchTrack() { }

  virtual eflowEtaPhiPosition etaPhiInLayer(PFMatch::LayerType layer) const {
    if (m_efRecTrack) return m_efRecTrack->getTrackCaloPoints().getEtaPhiPos(layer);
    else {
      std::cerr << "eflowRecMatchTrack ERROR: Invalid pointer to eflowRecTrack " << std::endl;
      return eflowEtaPhiPosition(-999.,-999.);
    }
  }

private:
  const eflowRecTrack* m_efRecTrack;
};

#include "AthContainers/DataVector.h"
#include "CLIDSvc/CLASS_DEF.h"

class eflowRecTrackContainer : public DataVector< eflowRecTrack >

{

 public:

  void print() { };

};

CLASS_DEF(eflowRecTrackContainer, 9803, 1)
#endif /* EFLOWRECTRACK_H_ */
