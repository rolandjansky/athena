/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_EGAMMATOPOCLUSTERMAP_H
#define EGAMMATOOLS_EGAMMATOPOCLUSTERMAP_H

//xAOD includes:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "egammaInterfaces/IegammaTopoClusterMap.h"

class egammaTopoClusterMap : public AthAlgTool, virtual public IegammaTopoClusterMap {

 public:

  //Constructor/destructor.
  egammaTopoClusterMap(const std::string& type,
		       const std::string& name,
		       const IInterface* parent);

  egammaTopoClusterMap();
  ~egammaTopoClusterMap();

  /** @brief AlgTool interface methods */
  //static const InterfaceID& interfaceID();
  
  //Tool standard routines.
  StatusCode initialize();
  StatusCode finalize();

  //Fill and sort map.
  StatusCode execute(const xAOD::CaloClusterContainer*);

  //Function to insert a topocluster, then re-sort the vector it belongs to.
  void InsertTopoCluster(xAOD::CaloCluster*);

  inline egammaTopoClusterMap& DuplicateMap() {
    return (*this);
  }

  //Clear the map.
  void ClearMap();

  TopoCluster2DMap& RetrieveMap() {return _map;}

  //Routine to retrieve vector of TopoClusters for a given Pt region.
  std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
						       double Pt);

  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(double eta, double phi,
						       double dEta,
						       double dPhi);

  //Routine to retrieve vector of TopoClusters for a given (eta, phi) region.
  inline std::vector<const xAOD::CaloCluster*> RetrieveTopoClusters(int eta_key, int phi_key) 
  {
    return _map[eta_key][phi_key];
  }

  //Private routine to translate an input eta, phi into integer keys for map.
  inline std::pair<int,int> GetEtaPhiKeys(double eta, double phi) {

    //Some checks on eta, phi values.
    if (eta > _maxEta) eta = _maxEta;
    if (eta < _minEta) eta = _minEta;
    if (phi > _maxPhi) phi = _maxPhi;
    if (phi < _minPhi) phi = _minPhi;

    //Need some warnings against max/min eta, phi here, I think.
    int eta_key = (int)((eta-_minEta)/_dEta);

    //NOTE: May have to account for phi wraparound here.
    int phi_key = (int)((phi-_minPhi)/_dPhi);

    return std::pair<int,int>(eta_key,phi_key);

  }

  void DumpMapContents();

 private:

  TopoCluster2DMap _map;

  //Function to set TopoClusters.
  void SetTopoClusters(const xAOD::CaloClusterContainer*);

  //Map granularity and range.
  double _minEta, _minPhi, _maxEta, _maxPhi;
  double _dEta, _dPhi;

  //Routine to sort topocluster grid elements by Pt.
  void SortGridVectors();
  void SortGridVector(int eta_key, int phi_key);

  //Used for removing clusters from map - reducing size, access time.
  double GetLArThirdLayerRatio (const xAOD::CaloCluster*);

};

#endif
