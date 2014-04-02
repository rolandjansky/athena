/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETALIGNGENTOOLS_INDETALIGNFILLSICLUSTER_H
#define INDETALIGNGENTOOLS_INDETALIGNFILLSICLUSTER_H
// ================================================
// InDetAlignFillSiCluster
// ================================================
//
// InDetAlignFillSiCluster.h
// Headerfile for InDetAlignFillSiCluster
//
// Carlos Escobar, started 08/03/2008
//
// AthAlgTool to create store Silicon Cluster information in a ntuple

#include <string>

#include "AthenaBaseComps/AthAlgTool.h"

#include "InDetPrepRawData/PixelClusterContainer.h"
#include "InDetPrepRawData/SCT_ClusterContainer.h"

#include "InDetAlignGenTools/IInDetAlignFillSiCluster.h"

// Forward declaration
class PixelID;
class SCT_ID;

class InDetAlignFillSiCluster: virtual public IInDetAlignFillSiCluster, public AthAlgTool {

  typedef InDet::PixelClusterContainer PixelClusterContainer;
  typedef InDet::SCT_ClusterContainer SCT_ClusterContainer;

 public:
  InDetAlignFillSiCluster(const std::string& type, 
			  const std::string& name,
			  const IInterface* parent);
  virtual ~InDetAlignFillSiCluster();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  virtual StatusCode FillSiCluster();

 private:
  INTupleSvc*                           m_ntupleSvc;

  const PixelID*                        m_pixelid;              //!< Pixel ID helper  
  const SCT_ID*                         m_sctID;                //!< SCT ID helper

  std::string                           m_Pixel_SiClustersName;
  std::string                           m_Sct_SiClustersName;

  const InDet::PixelClusterContainer*          m_Pixel_clcontainer;
  const InDet::SCT_ClusterContainer*           m_Sct_clcontainer;
  
  // methods
  void bookNtuple();
  StatusCode RetrieveSCTSiClusters();
  StatusCode RetrievePixelSiClusters();
  void FillSCTSiNtuple();
  void FillPixelSiNtuple();

  // variables
  std::string                           m_ntupleName;    //!< ntuple name

  NTuple::Item<long> m_pixel_nclusters;   //!< number of Pixel Clusters
  NTuple::Item<long> m_sct_nclusters;   //!< number of SCT Clusters
  
  // ----------------------------------------------------------------------
  // Pixel Clusters
  NTuple::Array<float> m_pixel_clx;       //!< Pixel Cluster X
  NTuple::Array<float> m_pixel_cly;       //!< Pixel Cluster Y
  NTuple::Array<float> m_pixel_clz;       //!< Pixel Cluster Z
  NTuple::Array<long>  m_pixel_groupsize; //!< Pixel Cluster Group Size
  NTuple::Array<long>  m_pixel_layer;     //!< Pixel Cluster layer
  NTuple::Array<long>  m_pixel_phi;       //!< Pixel Cluster phi
  NTuple::Array<float> m_pixel_LocX;      //!< Pixel Cluster Local X
  NTuple::Array<float> m_pixel_LocY;      //!< Pixel Cluster Local Y
  NTuple::Matrix<long> m_pixel_clrow;     // list of pixel row
  NTuple::Matrix<long> m_pixel_clcol;     // list of pixel col
  // ----------------------------------------------------------------------

  // ----------------------------------------------------------------------
  // SCT Clusters
  NTuple::Array<float> m_sct_clx;       //!< SCT Cluster X
  NTuple::Array<float> m_sct_cly;       //!< SCT Cluster Y
  NTuple::Array<float> m_sct_clz;       //!< SCT Cluster Z
  NTuple::Array<long>  m_sct_groupsize; //!< SCT Cluster Group Size
  NTuple::Array<long>  m_sct_layer;     //!< SCT Cluster layer
  NTuple::Array<long>  m_sct_eta;       //!< SCT Cluster eta
  NTuple::Array<long>  m_sct_phi;       //!< SCT Cluster phi
  NTuple::Array<long>  m_sct_side;      //!< SCT Cluster side
  // ----------------------------------------------------------------------

};

#endif // INDETALIGNGENTOOLS_INDETALIGNFILLSICLUSTER_H
