/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HTTCLUSTERINGTOOL_H
#define HTTCLUSTERINGTOOL_H

/*
 * httClustering
 * ---------------
 *
 * Routines to perform clustering in the pixels, based on TrigHTTSim
 *
 */

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigHTTMaps/HTTClusteringToolI.h"
#include "TrigHTTObjects/HTTHit.h"
#include "TrigHTTObjects/HTTCluster.h"

namespace HTTCLUSTERING {
  void attachTruth(std::vector<HTTHit> &);
  bool updatePixelCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster);
  bool updateStripCluster(HTTCluster &currentCluster, HTTHit &incomingHit, bool newCluster);
  void updateClusterContents(HTTCluster &currentCluster, int &clusterRow, int &clusterRowWidth, int &clusterCol, int &clusterColWidth, HTTHit &incomingHit);
  bool sortITkInputEta(const HTTHit& hitA, const HTTHit& hitB);
  bool sortITkInputPhi(const HTTHit& hitA, const HTTHit& HitB);
}

class HTTClusteringTool : public extends <AthAlgTool,HTTClusteringToolI> {
public:

  HTTClusteringTool(const std::string&, const std::string&, const IInterface*);

  virtual ~HTTClusteringTool() = default;

  virtual StatusCode DoClustering(HTTLogicalEventInputHeader &, std::vector<HTTCluster> &) const override;

 private:

  //HTT pixel clustering using the HTTSim objects
  void SortedClustering(const std::vector<std::vector<HTTHit> >& sorted_hits, std::vector<HTTCluster> &) const;
  void Clustering(std::vector<HTTHit>, std::vector<HTTCluster> &) const ;

  // Other helper functions
  void splitAndSortHits(std::vector<HTTHit>& hits, std::vector<std::vector<HTTHit> >& hitsPerModule, int& eta_phi) const;
  void splitAndSortHits(std::vector<HTTHit>& hits, std::vector<std::vector<HTTHit> >& hitsPerModule) const;
  void splitHitsToModules(std::vector<HTTHit>& hits, std::vector<std::vector<HTTHit> >& hitsPerModule) const;
  void normaliseClusters(std::vector<HTTCluster> &clusters) const;
  void sortHitsOnModules(std::vector<std::vector<HTTHit> >& hitsPerModule, int& eta_phi) const;
  void sortHitsOnModules(std::vector<std::vector<HTTHit> >& hitsPerModule) const;
  bool etaOrPhi(const HTTHit& hit) const;
  bool sortIBLInput(const std::unique_ptr<HTTHit>& i, const std::unique_ptr<HTTHit>& j) const;
  bool sortPixelInput(const std::unique_ptr<HTTHit>& i, const  std::unique_ptr<HTTHit>& j) const;



};

#endif // HTTCLUSTERINGTOOL_H
