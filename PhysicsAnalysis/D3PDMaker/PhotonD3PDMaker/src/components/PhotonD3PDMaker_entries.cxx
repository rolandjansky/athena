// $Id$
/**
 * @file PhotonD3PDMaker/src/components/PhotonD3PDMaker_entries.cxx
 * @author Mike Hance <hance@hep.upenn.edu>
 * @date July 2010
 * @brief List Gaudi components.
 */


#include "../PAUDiPhotonAssociationTool.h"
#include "../PAUDiPhotonFillerTool.h"
#include "../PhotonTruthParticleFilterTool.h"
#include "../TruthParticleFilterCutTool.h"
#include "../EgammaFilter.h"
#include "../PhotonFilter.h"
#include "../ElectronFilter.h"
#include "../TrackFilter.h"
#include "../CellFilter.h"
#include "../PAUTruthPhotonAssociationTool.h"
#include "../PAUTruthPhotonFillerTool.h"
#include "../PAUPhotonFillerTool.h"
#include "../PAUElectronFillerTool.h"
#include "../PAUConversionFillerTool.h"
#include "../PAUIsolationFillerTool.h"
#include "../ElectronSorterAlg.h"
#include "../PhotonSorterAlg.h"
#include "../DREtAssociationTool.h"
#include "../PhotonPVFillerTool.h"
#include "../ClusterFillerTool.h"
#include "../PAUGammaAssociationTool.h"
#include "../PAUFillerTool.h"
#include "../TruthParticleAssociationTool.h"
#include "../TruthExplorerAlg.h"
#include "../EgammaTruthParticleFilter.h"
#include "../GSFOriginalTrackParticleAssociationTool.h"
#include "../TrackParticleParametersAtDiPhotonVertex.h"
#include "../VxTrackAtVertexAssociationTool.h"
#include "../ConversionPerigeeAssociationTool.h"
#include "../ConversionNeutralPerigeeAssociationTool.h"
#include "../NeutralPerigeeFillerTool.h"
#include "../NeutralPerigeeCovarianceAssociationTool.h"
#include "../ShowerRadiusFillerTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUDiPhotonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUDiPhotonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PhotonTruthParticleFilterTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TruthParticleFilterCutTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUTruthPhotonAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUTruthPhotonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUPhotonFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUElectronFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUConversionFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUIsolationFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, DREtAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PhotonPVFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, ClusterFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUGammaAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, PAUFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TruthParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, GSFOriginalTrackParticleAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, TrackParticleParametersAtDiPhotonVertex)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, VxTrackAtVertexAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, ConversionPerigeeAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, ConversionNeutralPerigeeAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, NeutralPerigeeFillerTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, NeutralPerigeeCovarianceAssociationTool)
DECLARE_NAMESPACE_TOOL_FACTORY      (D3PD, ShowerRadiusFillerTool)

DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, EgammaFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, PhotonFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, ElectronFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, TrackFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, CellFilter)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, ElectronSorterAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, PhotonSorterAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, TruthExplorerAlg)
DECLARE_NAMESPACE_ALGORITHM_FACTORY (D3PD, EgammaTruthParticleFilter)

DECLARE_FACTORY_ENTRIES(PhotonD3PDMaker) {
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUDiPhotonAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUDiPhotonFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PhotonTruthParticleFilterTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TruthParticleFilterCutTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUTruthPhotonAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUTruthPhotonFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUPhotonFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUElectronFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUConversionFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUIsolationFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, DREtAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PhotonPVFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, ClusterFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUGammaAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, PAUFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, TruthParticleAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, GSFOriginalTrackParticleAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, VxTrackAtVertexAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, ConversionPerigeeAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, ConversionNeutralPerigeeAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, NeutralPerigeeFillerTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, NeutralPerigeeCovarianceAssociationTool)
  DECLARE_NAMESPACE_TOOL      (D3PD, ShowerRadiusFillerTool)

  DECLARE_NAMESPACE_ALGORITHM (D3PD, EgammaFilter)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, PhotonFilter)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, ElectronFilter)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, TrackFilter)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, CellFilter)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, ElectronSorterAlg)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, PhotonSorterAlg)
  DECLARE_NAMESPACE_ALGORITHM (D3PD, TruthExplorerAlg)

  DECLARE_NAMESPACE_ALGORITHM (D3PD, EgammaTruthParticleFilter)
}
