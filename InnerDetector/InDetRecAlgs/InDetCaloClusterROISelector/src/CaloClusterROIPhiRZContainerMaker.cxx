/*
   Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
 */

#include "InDetCaloClusterROISelector/CaloClusterROIPhiRZContainerMaker.h"
#include "TrkCaloClusterROI/ROIPhiRZContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

//#include "TrkEventPrimitives/GlobalPosition.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"


#include <stdexcept>
#include <algorithm>
#include <cstdint>

namespace InDet {
CaloClusterROIPhiRZContainerMaker::CaloClusterROIPhiRZContainerMaker(const std::string& name,
                                                                       ISvcLocator* pSvcLocator):
    AthReentrantAlgorithm(name, pSvcLocator)
{
}

// ================================================================
CaloClusterROIPhiRZContainerMaker::~CaloClusterROIPhiRZContainerMaker()
{  
}

// =================================================================
StatusCode CaloClusterROIPhiRZContainerMaker::initialize()
{
    ATH_CHECK( m_egammaCaloClusterSelector.retrieve() );
    ATH_CHECK( m_calosurf.retrieve() );
    ATH_CHECK( m_caloMgrKey.initialize() );

    if (m_outputClusterContainerName.empty()) {
       ATH_MSG_FATAL( "No OutputROIContainerName given.");
       return StatusCode::FAILURE;
    }
    if (m_outputClusterContainerName.size() > std::numeric_limits<uint8_t>::max()) {
       ATH_MSG_FATAL( "Too many OutputROIContainerNames given.");
       return StatusCode::FAILURE;
    }
    if (m_minPtEm.size() != m_phiWidth.size() || m_minPtEm.size() != m_outputClusterContainerName.size() ) {
       ATH_MSG_FATAL( "Number of entries in minPt, phiWidth must match number of entries in OutputROIContainerName.");
       return StatusCode::FAILURE;
    }
    m_outputIndex.reserve(m_minPtEm.size());
    m_maxPhiWidth=m_phiWidth[0];
    for (unsigned int idx=0; idx<m_minPtEm.size(); ++idx) {
       m_outputIndex.push_back(idx);
       m_maxPhiWidth=std::max(m_maxPhiWidth,m_phiWidth[idx]);
    }
    std::sort(m_outputIndex.begin(),m_outputIndex.end(), [this](unsigned int idx_a, unsigned int idx_b) { return this->m_minPtEm[idx_a] < this->m_minPtEm[idx_b];});
    m_sortedMinPtEm.reserve( m_outputIndex.size());
    for (unsigned int idx : m_outputIndex) {
       m_sortedMinPtEm.push_back( m_minPtEm[idx] );
    }

    m_allClusters=0;
    m_selectedClusters=0;

    ATH_CHECK(m_outputClusterContainerName.initialize());
    ATH_CHECK(m_inputClusterContainerName.initialize(!m_inputClusterContainerName.key().empty()));
    m_outputSorted.reserve( m_outputIndex.size() );
    m_outputUnsorted.reserve( m_outputIndex.size() );
    for (unsigned int output_i=0; output_i<m_outputIndex.size(); ++output_i) {
       if (m_phiWidth[m_outputIndex[output_i]]>0.) {
          m_outputSorted.push_back(output_i);
       }
       else {
          m_outputUnsorted.push_back(output_i);
       }
       ATH_MSG_INFO( "ROIPhiRZ container " << m_outputClusterContainerName[  m_outputIndex[output_i] ]
                     << " : " << m_minPtEm[m_outputIndex[output_i] ] << " MeV " <<  ( m_phiWidth[m_outputIndex[output_i]]>0. ? " order by phi " : " unordered" ) );
    }
    return StatusCode::SUCCESS;
}



// ====================================================================
StatusCode CaloClusterROIPhiRZContainerMaker::finalize()
{
    //
    // finalize method
    //
   ATH_MSG_INFO ("AllClusters " << m_allClusters << " selected " << m_selectedClusters << " max ROIs per event " << m_maxNROIs);

    return StatusCode::SUCCESS;
}

// ======================================================================
StatusCode CaloClusterROIPhiRZContainerMaker::execute(const EventContext& ctx) const
{

    if (m_inputClusterContainerName.key().empty()) {
        return StatusCode::SUCCESS;
    }

    // retrieve cluster containers, return `failure' if not existing
    SG::ReadHandle<xAOD::CaloClusterContainer> inputClusterContainer(m_inputClusterContainerName,ctx);
    ATH_CHECK(inputClusterContainer.isValid());

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey,ctx};
    ATH_CHECK(caloMgrHandle.isValid());
    const CaloDetDescrManager *caloMgr =  caloMgrHandle.cptr();

    unsigned int all_clusters{};
    unsigned int selected_clusters{};
    ROIPhiRZContainer          rois;      // temporary ROI container
    std::vector<unsigned int >   n_rois;   // number of ROIs per output container
    n_rois.resize(m_outputIndex.size(),0);

    std::vector<uint8_t >       max_output;// the outputs are ordered by the pt-cut, this is the index of the last output which passed the pt-cut per ROI
    rois.reserve( inputClusterContainer->size());
    max_output.resize(inputClusterContainer->size());

    // create ROIs.
    // first they are only stored in the temporary container
    for(const xAOD::CaloCluster* cluster : *inputClusterContainer )
    {
        all_clusters++;
        if (m_egammaCaloClusterSelector->passSelection(cluster,*caloMgr))
        {
            selected_clusters++;
            addROI(*cluster, *caloMgr, rois, max_output, n_rois);
        }
    }
    // This may happen if a ROI close to +-pi gets duplicated...
    //   so don't warn.  See ATLASRECTS-7160.
    //if  (rois.size()  > inputClusterContainer->size() ) {
    //   ATH_MSG_INFO( "Did not reserve enough storage for " << m_outputClusterContainerName[m_outputIndex[0]].key() );
    //}


    // create ROI output container
    std::vector< SG::WriteHandle<ROIPhiRZContainer> > output_rois;
    output_rois.reserve(m_outputIndex.size());
    for (unsigned int output_idx : m_outputIndex) {
       unsigned int the_size = n_rois[output_rois.size()];
       output_rois.emplace_back( m_outputClusterContainerName[output_idx], ctx);
       ATH_CHECK( output_rois.back().record( std::make_unique<ROIPhiRZContainer>() ) );
       output_rois.back()->reserve( the_size);
    }

    if (!m_outputSorted.empty()) {
       // sort ROIs by phi
       std::vector<unsigned int> roi_order;
       roi_order.reserve( rois.size() );
       for (unsigned int idx=0; idx< rois.size(); ++idx) { roi_order.push_back( idx ); }
       std::sort(roi_order.begin(),roi_order.end(),[&rois](unsigned int a, unsigned int b) { return rois[a][0] < rois[b][0]; });
       // copy ROIs in sort order to output container which are to be sorted
       for (unsigned int roi_i : roi_order) {
          for (unsigned int output_i : m_outputSorted) {
             if  (output_i>=max_output[roi_i]) break;
             output_rois[output_i]->push_back( rois[ roi_i ] );
          }
       }
    }

    // copy ROIs in original order to output container which should have the original order
    // also remove duplicates
    if (!m_outputUnsorted.empty()) {
       for (unsigned int roi_unordered_i=0;  roi_unordered_i < rois.size(); ++roi_unordered_i) {
          for (unsigned int output_i : m_outputUnsorted) {
             if  (output_i>=max_output[roi_unordered_i]) break;
             if (std::abs(rois[ roi_unordered_i ][0])<M_PI || rois[ roi_unordered_i ][0]==M_PI ) {
                output_rois[output_i]->push_back( rois[ roi_unordered_i ] );
             }
          }
       }
    }

    // gather statistics
    {
       unsigned int max_size;
       do {
          max_size = m_maxNROIs;
       } while (rois.size()>max_size && ! m_maxNROIs.compare_exchange_weak(max_size, rois.size()));
    }
    m_allClusters += all_clusters;
    m_selectedClusters += selected_clusters;
    return StatusCode::SUCCESS;
}

Trk::LocalParameters CaloClusterROIPhiRZContainerMaker::getClusterLocalParameters(const xAOD::CaloCluster &cluster,
                                                                                   const Trk::Surface &surf) const
{

  Amg::Vector3D surfRefPoint = surf.globalReferencePoint();

  double eta = cluster.eta();
  double theta = 2 * atan(exp(-eta)); //  -log(tan(theta/2));
  double tantheta = tan(theta);
  double phi = cluster.phi();

  if (xAOD::EgammaHelpers::isBarrel(&cluster)) {
    // Two corindate in a cyclinder are
    // Trk::locRPhi = 0 (ie phi)
    // Trk::locZ    = 1(ie z)
    double r = surfRefPoint.perp();
    double z = tantheta == 0 ? 0. : r / tantheta;
    Trk::DefinedParameter locRPhi(r * phi, Trk::locRPhi);
    Trk::DefinedParameter locZ(z, Trk::locZ);
    std::vector<Trk::DefinedParameter> defPar{ locRPhi, locZ};
    return Trk::LocalParameters(defPar);
  } else {
    // Local paramters of a disk are
    // Trk::locR   = 0
    // Trk::locPhi = 1
    double z = surfRefPoint.z();
    double r = z * tantheta;
    Trk::DefinedParameter locR(r, Trk::locR);
    Trk::DefinedParameter locPhi(phi, Trk::locPhi);
    std::vector<Trk::DefinedParameter> defPar {locR, locPhi} ;
    return Trk::LocalParameters(defPar);
  }
}


void CaloClusterROIPhiRZContainerMaker::addROI( const xAOD::CaloCluster &cluster,
                                                 const CaloDetDescrManager &caloDDMgr,
                                                 ROIPhiRZContainer &output_rois,
                                                 std::vector<uint_fast8_t> &max_output,
                                                 std::vector<unsigned int> &n_rois) const {

  double energy = cluster.e();

  // do we want to make energy be EM energy only?
  if (m_EMEnergyOnly) {
    static const SG::AuxElement::ConstAccessor<float> acc("EMFraction");
    double emFrac(0.);
    if (acc.isAvailable(cluster)) {
      emFrac = acc(cluster);
    } else if (!cluster.retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM,
                                        emFrac)) {
      ATH_MSG_ERROR("EM energy requested, but No EM fraction momement stored");
      return;
    }
    energy *= emFrac;
  }

  std::unique_ptr<const Trk::Surface> surface( getCaloSurface(cluster, caloDDMgr) );
  if (!surface) {
     ATH_MSG_ERROR( "Failed to create surface for cluster");
     return;
  }

  const Trk::LocalParameters localParams( getClusterLocalParameters(cluster, *surface) );

  Amg::Vector3D global_position( surface->localToGlobal( localParams) );
  double et = energy * std::sin(global_position.theta());
  if ( et >= m_sortedMinPtEm[0]){
     unsigned int roi_idx=output_rois.size();
     output_rois.addROI(global_position, m_maxPhiWidth);

     unsigned int n_duplicates = output_rois.size()-roi_idx-1;
     if (n_duplicates>0) {
        m_duplicateROI += n_duplicates;
     }

     unsigned int output_idx=0;
     for ( ; output_idx<m_outputIndex.size(); ++output_idx) {
        if (et < m_sortedMinPtEm[output_idx]) break;
        n_rois[output_idx] += n_duplicates+1;
     }
     // addROI may duplicate the ROI at phi - 2 pi
     // So, have to set the last output_idx for all newly added ROIs
     if (max_output.size() < output_rois.size()) {
       max_output.resize (output_rois.size());
     }
     for (; roi_idx < output_rois.size(); ++roi_idx) {
        max_output[roi_idx]=output_idx;
     }

  }
  else {
     ATH_MSG_INFO("Skip selected cluster " << energy << " * " << std::sin(global_position.theta()) << " = " << energy * std::sin(global_position.theta())<< " >= " << m_sortedMinPtEm[0] );
  }
}
}
