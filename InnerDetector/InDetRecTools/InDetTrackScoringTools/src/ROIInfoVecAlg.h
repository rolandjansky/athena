/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/* Dear emacs, this is -*-c++-*- */
#ifndef _ROIInfoVecAlg_H_
#define _ROIInfoVecAlg_H_

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

#include "InDetTrackScoringTools/ROIInfoVec.h"
#include "TrkCaloClusterROI/CaloClusterROI_Collection.h"


class ROIInfoVecAlg : public AthReentrantAlgorithm
{
public:
  /// Forward base class ctor.
  using AthReentrantAlgorithm::AthReentrantAlgorithm;

   /// Gaudi initialize method.
  virtual StatusCode initialize() override;

  /// Algorithm execute method.
  virtual StatusCode execute (const EventContext& ctx) const override;

private:
  SG::ReadHandleKey<CaloClusterROI_Collection> m_inputEmClusterContainerKey
     {this,"InputEmClusterContainerName","InDetCaloClusterROIs","Calo cluster ROIs"};

  SG::WriteHandleKey<InDet::ROIInfoVec>        m_outputROIInfoKey
     {this,"WriteKey","ROIInfoVec","Calo cluster ROIs"};

  Gaudi::Property<float>                       m_minPtEm
     {this,"minPtEM", 5000. , "minimum cluster Pt in MeV"};

};
#endif
