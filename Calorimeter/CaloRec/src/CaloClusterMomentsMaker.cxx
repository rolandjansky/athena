/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------
// File and Version Information:
//
// Description: see CaloClusterMomentsMaker.h
// 
// Environment:
//      Software developed for the ATLAS Detector at CERN LHC
//
// Author List:
//      Sven Menke
//      Peter Loch
//
//-----------------------------------------------------------------------

#include "CaloClusterMomentsMaker.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloEvent/CaloCluster.h"
#include "CaloGeoHelpers/proxim.h"
#include "CaloEvent/CaloPrefetch.h"
#include "CaloInterface/ILArHVFraction.h"
#include "CaloGeoHelpers/CaloPhiRange.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "AthAllocators/ArenaPoolSTLAllocator.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CxxUtils/prefetch.h"
#include <Eigen/Dense>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <sstream>

#include <map>
#include <vector>
#include <tuple>
#include <string>
#include <cstdio>
#include <cmath>

using CLHEP::deg;
using CLHEP::cm;


// Known moments
namespace {
  // name -> enum translator
  std::map<std::string,xAOD::CaloCluster::MomentType> momentNameToEnumMap = { 
    { "AVG_LAR_Q",         xAOD::CaloCluster::AVG_LAR_Q },
    { "AVG_TILE_Q",        xAOD::CaloCluster::AVG_TILE_Q },
    { "BADLARQ_FRAC",      xAOD::CaloCluster::BADLARQ_FRAC },
    { "BAD_CELLS_CORR_E",  xAOD::CaloCluster::BAD_CELLS_CORR_E },
    { "CELL_SIGNIFICANCE", xAOD::CaloCluster::CELL_SIGNIFICANCE },
    { "CELL_SIG_SAMPLING", xAOD::CaloCluster::CELL_SIG_SAMPLING },
    { "CENTER_LAMBDA",     xAOD::CaloCluster::CENTER_LAMBDA },
    { "CENTER_MAG",        xAOD::CaloCluster::CENTER_MAG },
    { "CENTER_X",          xAOD::CaloCluster::CENTER_X },
    { "CENTER_Y",          xAOD::CaloCluster::CENTER_Y },
    { "CENTER_Z",          xAOD::CaloCluster::CENTER_Z },
    { "DELTA_ALPHA",       xAOD::CaloCluster::DELTA_ALPHA },
    { "DELTA_PHI",         xAOD::CaloCluster::DELTA_PHI },
    { "DELTA_THETA",       xAOD::CaloCluster::DELTA_THETA },
    { "ENG_BAD_CELLS",     xAOD::CaloCluster::ENG_BAD_CELLS },
    { "ENG_BAD_HV_CELLS",  xAOD::CaloCluster::ENG_BAD_HV_CELLS },
    { "ENG_FRAC_CORE",     xAOD::CaloCluster::ENG_FRAC_CORE },
    { "ENG_FRAC_EM",       xAOD::CaloCluster::ENG_FRAC_EM },
    { "ENG_FRAC_MAX",      xAOD::CaloCluster::ENG_FRAC_MAX },
    { "ENG_POS",           xAOD::CaloCluster::ENG_POS },
    { "FIRST_ENG_DENS",    xAOD::CaloCluster::FIRST_ENG_DENS },
    { "FIRST_ETA",         xAOD::CaloCluster::FIRST_ETA },
    { "FIRST_PHI",         xAOD::CaloCluster::FIRST_PHI },
    { "ISOLATION",         xAOD::CaloCluster::ISOLATION },
    { "LATERAL",           xAOD::CaloCluster::LATERAL },
    { "LONGITUDINAL",      xAOD::CaloCluster::LONGITUDINAL },
    { "MASS",              xAOD::CaloCluster::MASS },
    { "N_BAD_CELLS",       xAOD::CaloCluster::N_BAD_CELLS },
    { "N_BAD_HV_CELLS",    xAOD::CaloCluster::N_BAD_HV_CELLS },
    { "N_BAD_CELLS_CORR",  xAOD::CaloCluster::N_BAD_CELLS_CORR },
    { "PTD",               xAOD::CaloCluster::PTD },
    { "SECOND_ENG_DENS",   xAOD::CaloCluster::SECOND_ENG_DENS },
    { "SECOND_LAMBDA",     xAOD::CaloCluster::SECOND_LAMBDA },
    { "SECOND_R",          xAOD::CaloCluster::SECOND_R },
    { "SECOND_TIME",       xAOD::CaloCluster::SECOND_TIME },
    { "SIGNIFICANCE",      xAOD::CaloCluster::SIGNIFICANCE },
    { "EM_PROBABILITY",    xAOD::CaloCluster::EM_PROBABILITY },
    { "NCELL_SAMPLING",    xAOD::CaloCluster::NCELL_SAMPLING }
  };
  // enum -> name translator
  std::map<xAOD::CaloCluster::MomentType,std::string> momentEnumToNameMap = { 
    { xAOD::CaloCluster::AVG_LAR_Q,          "AVG_LAR_Q"        },
    { xAOD::CaloCluster::AVG_TILE_Q,         "AVG_TILE_Q"       },
    { xAOD::CaloCluster::BADLARQ_FRAC,       "BADLARQ_FRAC"     },
    { xAOD::CaloCluster::BAD_CELLS_CORR_E,   "BAD_CELLS_CORR_E" },
    { xAOD::CaloCluster::CELL_SIGNIFICANCE,  "CELL_SIGNIFICANCE"},
    { xAOD::CaloCluster::CELL_SIG_SAMPLING,  "CELL_SIG_SAMPLING"},
    { xAOD::CaloCluster::CENTER_LAMBDA,      "CENTER_LAMBDA"    },
    { xAOD::CaloCluster::CENTER_MAG,         "CENTER_MAG"       },
    { xAOD::CaloCluster::CENTER_X,           "CENTER_X"         },
    { xAOD::CaloCluster::CENTER_Y,           "CENTER_Y"         },
    { xAOD::CaloCluster::CENTER_Z,           "CENTER_Z"         },
    { xAOD::CaloCluster::DELTA_ALPHA,        "DELTA_ALPHA"      },
    { xAOD::CaloCluster::DELTA_PHI,          "DELTA_PHI"        },
    { xAOD::CaloCluster::DELTA_THETA,        "DELTA_THETA"      },
    { xAOD::CaloCluster::ENG_BAD_CELLS,      "ENG_BAD_CELLS"    },
    { xAOD::CaloCluster::ENG_BAD_HV_CELLS,   "ENG_BAD_HV_CELLS" },
    { xAOD::CaloCluster::ENG_FRAC_CORE,      "ENG_FRAC_CORE"    },
    { xAOD::CaloCluster::ENG_FRAC_EM,        "ENG_FRAC_EM"      },
    { xAOD::CaloCluster::ENG_FRAC_MAX,       "ENG_FRAC_MAX"     },
    { xAOD::CaloCluster::ENG_POS,            "ENG_POS"          },
    { xAOD::CaloCluster::FIRST_ENG_DENS,     "FIRST_ENG_DENS"   },
    { xAOD::CaloCluster::FIRST_ETA,          "FIRST_ETA"        },
    { xAOD::CaloCluster::FIRST_PHI,          "FIRST_PHI"        },
    { xAOD::CaloCluster::ISOLATION,          "ISOLATION"        },
    { xAOD::CaloCluster::LATERAL,            "LATERAL"          },
    { xAOD::CaloCluster::LONGITUDINAL,       "LONGITUDINAL"     },
    { xAOD::CaloCluster::MASS,               "MASS"             },
    { xAOD::CaloCluster::N_BAD_CELLS,        "N_BAD_CELLS"      },
    { xAOD::CaloCluster::N_BAD_HV_CELLS,     "N_BAD_HV_CELLS"   },
    { xAOD::CaloCluster::N_BAD_CELLS_CORR,   "N_BAD_CELLS_CORR" },
    { xAOD::CaloCluster::PTD,                "PTD"              },
    { xAOD::CaloCluster::SECOND_ENG_DENS,    "SECOND_ENG_DENS"  },
    { xAOD::CaloCluster::SECOND_LAMBDA,      "SECOND_LAMBDA"    },
    { xAOD::CaloCluster::SECOND_R,           "SECOND_R"         },
    { xAOD::CaloCluster::SECOND_TIME,        "SECOND_TIME"      },
    { xAOD::CaloCluster::SIGNIFICANCE,       "SIGNIFICANCE"     },
    { xAOD::CaloCluster::EM_PROBABILITY,     "EM_PROBABILITY"   },
    { xAOD::CaloCluster::NCELL_SAMPLING,     "NCELL_SAMPLING"   }
  };
}

//###############################################################################

CaloClusterMomentsMaker::CaloClusterMomentsMaker(const std::string& type, 
						 const std::string& name,
						 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_calo_id(nullptr),
    m_maxAxisAngle(20*deg), 
    m_minRLateral(4*cm), 
    m_minLLongitudinal(10*cm),
    m_minBadLArQuality(4000),
    m_calculateSignificance(false),
    m_calculateIsolation(false),
    m_calculateLArHVFraction(false),
    m_twoGaussianNoise(false),
    m_caloDepthTool("CaloDepthTool",this),
    m_larHVFraction("LArHVFraction",this),
    m_absOpt(false) 
{
  declareInterface<CaloClusterCollectionProcessor> (this);
  // Name(s) of Moments to calculate
  declareProperty("MomentsNames",m_momentsNames);

  // Maximum allowed angle between shower axis and the vector pointing
  // to the shower center from the IP in degrees. This property is needed
  // to protect against cases where all significant cells are in one sampling
  // and the shower axis can thus not be defined.
  declareProperty("MaxAxisAngle",m_maxAxisAngle);
  declareProperty("MinRLateral",m_minRLateral);
  declareProperty("MinLLongitudinal",m_minLLongitudinal);
  declareProperty("MinBadLArQuality",m_minBadLArQuality);
  // Use 2-gaussian noise for Tile
  declareProperty("TwoGaussianNoise",m_twoGaussianNoise);
  declareProperty("LArHVFraction",m_larHVFraction,"Tool Handle for LArHVFraction");
  // Not used anymore (with xAOD), but required when configured from  COOL.
  declareProperty("AODMomentsNames",m_momentsNamesAOD);
  // Use weighting of neg. clusters option?
  declareProperty("WeightingOfNegClusters", m_absOpt);
  // Set eta boundary for transition from outer to inner wheel in EME2
  declareProperty("EMECAbsEtaWheelTransition",m_etaInnerWheel);
}

//###############################################################################

StatusCode CaloClusterMomentsMaker::initialize()
{
  // loop list of requested moments
  std::string::size_type nstr(0); int nmom(0); 
  for ( const auto& mom : m_momentsNames ) {
    ATH_MSG_INFO( "Moment " << mom << " requested" );
    // check if moment is known (enumerator available)
    auto fmap(momentNameToEnumMap.find(mom)); 
    if ( fmap != momentNameToEnumMap.end() ) {
      // valid moment found 
      nstr = std::max(nstr,mom.length()); ++nmom;
      if ( fmap->second == xAOD::CaloCluster::SECOND_TIME ) {
	// special flag for second moment of cell times - this moment is not 
	// calculated in this tool! Do not add to internal (!) valid moments list. 
	// Its value is available from xAOD::CaloCluster::secondTime()!
	m_secondTime = true;
      } else if ( fmap->second == xAOD::CaloCluster::NCELL_SAMPLING ) {
	// flag indicates if number of cells in a sampling should be counted. 
	// This is a vector of integers counts that is filled in this tool but does 
	// not need any post-processing (e.g. normalization). It is not 
	// added to the valid moments list for this reason.
	ATH_MSG_INFO( "moment " << fmap->first << " found");  
	m_nCellsPerSampling = true; 
      } else if ( fmap->second == xAOD::CaloCluster::EM_PROBABILITY ) {
	ATH_MSG_WARNING( mom << " not calculated in this tool - misconfiguration?" );
      } else {  
	// all other valid moments
	m_validMoments.push_back(fmap->second);
	// flag some special requests 
	switch (fmap->second) { 
	case xAOD::CaloCluster::SIGNIFICANCE:
	case xAOD::CaloCluster::CELL_SIGNIFICANCE:
	  m_calculateSignificance = true; 
	  break;
	case xAOD::CaloCluster::ISOLATION:
	  m_calculateIsolation = true;
	  break;
	case xAOD::CaloCluster::ENG_BAD_HV_CELLS:
	  m_calculateLArHVFraction = true;
	  break;
	default:
	  break;
	} // set special processing flags
      } // moment calculated with this tool
    } else { 
      ATH_MSG_ERROR( "Moment name " << mom << " not known; known moments are:" );
      char buffer[128]; std::string::size_type lstr(nstr); 
      // determine field size
      for ( const auto& fmom : momentNameToEnumMap ) { lstr = std::max(lstr,fmom.first.length()); } 
      // print available moments
      for ( const auto& fmom : momentNameToEnumMap ) { 
	sprintf(buffer,"moment name: %-*.*s - enumerator: %i",(int)lstr,(int)lstr,fmom.first.c_str(),(int)fmom.second); 
	ATH_MSG_INFO(buffer);
      }
      auto fmom(momentNameToEnumMap.find("SECOND_TIME"));
      sprintf(buffer,"moment name: %-*.*s - enumerator: %i",(int)nstr,(int)nstr,fmom->first.c_str(),(int)fmom->second);
      ATH_MSG_INFO( buffer );
      fmom = momentNameToEnumMap.find("NCELL_SAMPLING");
      sprintf(buffer,"moment name: %-*.*s - enumerator: %i",(int)nstr,(int)nstr,fmom->first.c_str(),(int)fmom->second);
      ATH_MSG_INFO( buffer );
      return StatusCode::FAILURE;
    } // found unknown moment name
  } // loop configured moment names
  
  // sort and remove duplicates
  std::sort(m_validMoments.begin(), m_validMoments.end());
  m_validMoments.erase(std::unique(m_validMoments.begin(),m_validMoments.end()),m_validMoments.end());
   
  // print configured moments
  ATH_MSG_INFO( "Construct and save " << nmom << " cluster moments: " );
  char buffer[128];
  for ( auto menum : m_validMoments ) { 
    sprintf(buffer,"moment name: %-*.*s - enumerator: %i",(int)nstr,(int)nstr,momentEnumToNameMap.at(menum).c_str(),(int)menum); 
    ATH_MSG_INFO( buffer );
  }
  if ( m_secondTime ) { 
    auto fmom(momentNameToEnumMap.find("SECOND_TIME"));
    sprintf(buffer,"moment name: %-*.*s - enumerator: %i (save only)",(int)nstr,(int)nstr,fmom->first.c_str(),(int)fmom->second);
    ATH_MSG_INFO( buffer );
  }
  if ( m_nCellsPerSampling ) { 
    auto fmom(momentNameToEnumMap.find("NCELL_SAMPLING"));
    sprintf(buffer,"moment name: %-*.*s - enumerator: %i",(int)nstr,(int)nstr,fmom->first.c_str(),(int)fmom->second);
    ATH_MSG_INFO( buffer );
  }

  // retrieve CaloCell ID server  
  CHECK(detStore()->retrieve(m_calo_id,"CaloCell_ID"));
  
  // retrieve the calo depth tool
  CHECK(m_caloDepthTool.retrieve());
  ATH_CHECK(m_caloMgrKey.initialize());

  // retrieve specific servers and tools for selected processes
  ATH_CHECK(m_noiseCDOKey.initialize(m_calculateSignificance));
  if (m_calculateLArHVFraction) { ATH_CHECK(m_larHVFraction.retrieve()); } else { m_larHVFraction.disable(); }

  return StatusCode::SUCCESS;
}

StatusCode CaloClusterMomentsMaker::finalize()
{
  return StatusCode::SUCCESS;
}

//#############################################################################

namespace CaloClusterMomentsMaker_detail {


struct cellinfo {
  double x;
  double y;
  double z;
  double energy;
  double eta;
  double phi;
  double r;
  double lambda;
  double volume;
  CaloCell_ID::CaloSample sample;
};

} // namespace CaloClusterMomentsMaker_detail

StatusCode
CaloClusterMomentsMaker::execute(const EventContext& ctx,
                                 xAOD::CaloClusterContainer *theClusColl)
  const
{ 
  ATH_MSG_DEBUG("Executing " << name());

  // Maps cell IdentifierHash to cluster index in cluster collection.
  // Only used when cluster isolation moment is calculated.
  using clusterIdx_t = std::uint16_t;
  typedef std::pair<clusterIdx_t, clusterIdx_t> clusterPair_t;
  std::vector<clusterPair_t> clusterIdx;
  const clusterIdx_t noCluster = std::numeric_limits<clusterIdx_t>::max();

  const CaloNoise* noise=nullptr;
  if (m_calculateSignificance) {
    SG::ReadCondHandle<CaloNoise> noiseHdl{m_noiseCDOKey,ctx};
    noise=*noiseHdl;
  }

  SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{ m_caloMgrKey, ctx };
  const CaloDetDescrManager* caloDDMgr = *caloMgrHandle;
  // Counters for number of empty and non-empty neighbor cells per sampling
  // layer Only used when cluster isolation moment is calculated.
  int nbEmpty[CaloCell_ID::Unknown];
  int nbNonEmpty[CaloCell_ID::Unknown];

  // prepare stuff from entire collection in case isolation moment
  // should be calculated

  if ( m_calculateIsolation ) {

    if (theClusColl->size() >= noCluster) {
      msg(MSG::ERROR) << "Too many clusters" << endmsg;
      return StatusCode::FAILURE;
    }

    // initialize with "empty" values
    clusterIdx.resize(m_calo_id->calo_cell_hash_max(),
                      clusterPair_t(noCluster, noCluster));

    int iClus = 0;
    for (xAOD::CaloCluster* theCluster : *theClusColl) {
      // loop over all cell members and fill cell vector for used cells
      xAOD::CaloCluster::cell_iterator cellIter    = theCluster->cell_begin();
      xAOD::CaloCluster::cell_iterator cellIterEnd = theCluster->cell_end();
      for(; cellIter != cellIterEnd; cellIter++ ){
        CxxUtils::prefetchNext(cellIter, cellIterEnd);
        const CaloCell* pCell = *cellIter;

	Identifier myId = pCell->ID();
	IdentifierHash myHashId = m_calo_id->calo_cell_hash(myId); 
	if ( clusterIdx[(unsigned int)myHashId].first != noCluster) {
	  // check weight and assign to current cluster if weight is > 0.5
	  double weight = cellIter.weight();
	  if ( weight > 0.5 )
	    clusterIdx[(unsigned int)myHashId].first = iClus;
	}
	else {
	  clusterIdx[(unsigned int)myHashId].first = iClus;
	}
      }
      ++iClus;
    }
  }

  // Move allocation of temporary arrays outside the cluster loop.
  // That way, we don't need to delete and reallocate them
  // each time through the loop.

  std::vector<CaloClusterMomentsMaker_detail::cellinfo> cellinfo;
  std::vector<double> maxSampE (CaloCell_ID::Unknown);
  std::vector<double> myMoments(m_validMoments.size(),0);
  std::vector<double> myNorms(m_validMoments.size(),0);
  std::vector<std::tuple<int,int> > nCellsSamp; nCellsSamp.reserve(CaloCell_ID::Unknown);
  std::vector<IdentifierHash> theNeighbors;
  // loop over individual clusters
  xAOD::CaloClusterContainer::iterator clusIter = theClusColl->begin();
  xAOD::CaloClusterContainer::iterator clusIterEnd = theClusColl->end();
  int iClus = 0;
  for( ;clusIter!=clusIterEnd;++clusIter,++iClus) {
    xAOD::CaloCluster * theCluster = *clusIter;

    double w(0),xc(0),yc(0),zc(0),mx(0),my(0),mz(0),mass(0);
    double eBad(0),ebad_dac(0),ePos(0),eBadLArQ(0),sumSig2(0),maxAbsSig(0);
    double eLAr2(0),eLAr2Q(0);
    double eTile2(0),eTile2Q(0);
    double eBadLArHV(0);
    int nbad(0),nbad_dac(0),nBadLArHV(0);
    unsigned int ncell(0),i,nSigSampl(0);
    unsigned int theNumOfCells = theCluster->size();
    
    // these two are needed for the LATERAL moment
    int iCellMax(-1);
    int iCellScndMax(-1);

    if (cellinfo.capacity() == 0)
      cellinfo.reserve (theNumOfCells*2);
    cellinfo.resize (theNumOfCells);
    
    for(i=0;i<(unsigned int)CaloCell_ID::Unknown;i++) 
      maxSampE[i] = 0;
    
    if ( !m_momentsNames.empty() ) {
      std::fill (myMoments.begin(), myMoments.end(), 0);
      std::fill (myNorms.begin(),   myNorms.end(),   0);
      if ( m_calculateIsolation ) {
        std::fill_n(nbNonEmpty, CaloCell_ID::Unknown, 0);
        std::fill_n(nbEmpty, CaloCell_ID::Unknown, 0);
      }
      
      // loop over all cell members and calculate the center of mass
      xAOD::CaloCluster::cell_iterator cellIter    = theCluster->cell_begin();
      xAOD::CaloCluster::cell_iterator cellIterEnd = theCluster->cell_end();
      for(; cellIter != cellIterEnd; cellIter++ ){
        CaloPrefetch::nextDDE(cellIter, cellIterEnd);

        const CaloCell* pCell = (*cellIter);
	Identifier myId = pCell->ID();
	const CaloDetDescrElement* myCDDE = pCell->caloDDE();
	double ene = pCell->e();
        if(m_absOpt) ene = fabs(ene);  
	double weight = cellIter.weight();//theCluster->getCellWeight(cellIter);
	if ( pCell->badcell() ) {
	  eBad += ene*weight;
	  nbad++;
	  if(ene!=0){
	    ebad_dac+=ene*weight;
	    nbad_dac++;
	  }
	}
	else {
	  if ( myCDDE && ! (myCDDE->is_tile()) 
	       && ((pCell->provenance() & 0x2000) == 0x2000) 
	       && !((pCell->provenance() & 0x0800) == 0x0800)) {
	    if ( pCell->quality() > m_minBadLArQuality ) {
	      eBadLArQ += ene*weight;
	    }
	    eLAr2  += ene*weight*ene*weight;
	    eLAr2Q += ene*weight*ene*weight*pCell->quality();
	  }
	  if ( myCDDE && myCDDE->is_tile() ) {
	    uint16_t tq = pCell->quality();
	    uint8_t tq1 = (0xFF00&tq)>>8; // quality in channel 1
	    uint8_t tq2 = (0xFF&tq); // quality in channel 2
	    // reject cells with either 0xFF00 or 0xFF
	    if ( ((tq1&0xFF) != 0xFF) && ((tq2&0xFF) != 0xFF) ) {    
	      eTile2  += ene*weight*ene*weight;
	      // take the worse of both qualities (one might be 0 in
	      // 1-channel cases)
	      eTile2Q += ene*weight*ene*weight*(tq1>tq2?tq1:tq2);
	    }
	  } 
	}
	if ( ene > 0 ) {
	  ePos += ene*weight;
	}
      
	if ( m_calculateSignificance ) {
	  const float sigma = m_twoGaussianNoise ?\
	    noise->getEffectiveSigma(pCell->ID(),pCell->gain(),pCell->energy()) : \
	    noise->getNoise(pCell->ID(),pCell->gain());

	  sumSig2 += sigma*sigma;
	  // use geomtery weighted energy of cell for leading cell significance
	  double Sig = (sigma>0?ene*weight/sigma:0);
	  if ( fabs(Sig) > fabs(maxAbsSig) ) {
	    maxAbsSig = Sig;
	    nSigSampl = myCDDE->getSampling();
	  }
	}
	if ( m_calculateIsolation ) {
	  // get all 2D Neighbours if the cell is not inside another cluster with
	  // larger weight

	  IdentifierHash myHashId = m_calo_id->calo_cell_hash(myId);
	  if ( clusterIdx[myHashId].first == iClus ) {
            theNeighbors.clear();
	    m_calo_id->get_neighbours(myHashId, LArNeighbours::all2D, theNeighbors);
	    for (const auto& nhash: theNeighbors) {
              clusterPair_t& idx = clusterIdx[nhash];

	      // only need to look at each cell once per cluster
	      if ( idx.second == iClus ) continue;
	      idx.second = iClus;

	      if ( idx.first == noCluster ) {
	        ++ nbEmpty[m_calo_id->calo_sample(nhash)];
	      } else if ( idx.first != iClus ) {
                ++ nbNonEmpty[m_calo_id->calo_sample(nhash)];
	      }

	    }
	  }
	}

	if ( myCDDE != nullptr ) { 
	  if ( m_nCellsPerSampling ) { 
	    CaloCell_ID::CaloSample sam = myCDDE->getSampling();
	    size_t idx((size_t)sam);
	    if ( idx >= nCellsSamp.size() ) { nCellsSamp.resize(idx+1, { 0, 0 } ); }
	    std::get<0>(nCellsSamp[idx])++;
	    // special count for inner wheel cells in EME2
	    if ( sam == CaloCell_ID::EME2 && std::abs(myCDDE->eta()) > m_etaInnerWheel ) { std::get<1>(nCellsSamp[idx])++; }
	  }
	  if ( ene > 0. && weight > 0) {
	    // get all geometric information needed ...
	    CaloClusterMomentsMaker_detail::cellinfo& ci = cellinfo[ncell];
	    ci.x      = myCDDE->x();
	    ci.y      = myCDDE->y();
	    ci.z      = myCDDE->z();
	    ci.eta    = myCDDE->eta();
	    ci.phi    = myCDDE->phi();
	    ci.energy = ene*weight;
	    ci.volume = myCDDE->volume();
	    ci.sample = myCDDE->getSampling();

	    if ( ci.energy > maxSampE[(unsigned int)ci.sample] )
	      maxSampE[(unsigned int)ci.sample] = ci.energy;
	    
	    if (iCellMax < 0 || ci.energy > cellinfo[iCellMax].energy ) {
	      iCellScndMax = iCellMax;
	      iCellMax = ncell;
	    }
	    else if (iCellScndMax < 0 ||
		     ci.energy > cellinfo[iCellScndMax].energy )
	      {
		iCellScndMax = ncell;
	      }
	  
	    xc += ci.energy*ci.x;
	    yc += ci.energy*ci.y;
	    zc += ci.energy*ci.z;

	    double dir = ci.x*ci.x+ci.y*ci.y+ci.z*ci.z;
	    
	    if ( dir > 0) {
	      dir = sqrt(dir);
	      dir = 1./dir;
	    }
	    mx += ci.energy*ci.x*dir;
	    my += ci.energy*ci.y*dir;
	    mz += ci.energy*ci.z*dir;
	    
	    w  += ci.energy;
	    
	    ncell++;
	  } // cell has E>0 and weight != 0
	} // cell has valid DDE
      } //end of loop over all cells
      if (m_calculateLArHVFraction) {
	const auto hvFrac=m_larHVFraction->getLArHVFrac(theCluster->getCellLinks(),ctx);
	eBadLArHV= hvFrac.first;
	nBadLArHV=hvFrac.second;
      }

      if ( w > 0 ) {
	mass = w*w - mx*mx - my*my - mz*mz;
	if ( mass > 0) {
	  mass = sqrt(mass);
	}
	else {
	  // make mass negative if m^2 was negative
	  mass = -sqrt(-mass);
	}

	xc/=w;
	yc/=w;
	zc/=w;
	Amg::Vector3D showerCenter(xc,yc,zc);
	w=0;
	

	//log << MSG::WARNING << "Found bad cells " <<  xbad_dac << " " << ybad_dac << " " << zbad_dac << " " << ebad_dac <<  endmsg;
	//log << MSG::WARNING << "Found Cluster   " <<  xbad_dac << " " << ybad_dac << " " << zbad_dac << " " <<  endmsg;
	// shower axis is just the vector pointing from the IP to the shower center
	// in case there are less than 3 cells in the cluster
	
	Amg::Vector3D showerAxis(xc,yc,zc);
	Amg::setMag(showerAxis,1.0);

	// otherwise the principal direction with the largest absolute 
	// eigenvalue will be used unless it's angle w.r.t. the vector pointing
	// from the IP to the shower center is larger than allowed by the 
	// property m_maxAxisAngle

	double angle(0),deltaPhi(0),deltaTheta(0);
	if ( ncell > 2 ) {
	  Eigen::Matrix3d C=Eigen::Matrix3d::Zero();
	  for(i=0;i<ncell;i++) {
            const CaloClusterMomentsMaker_detail::cellinfo& ci = cellinfo[i];
            const double e2 = ci.energy * ci.energy;
            
	    C(0,0) += e2*(ci.x-xc)*(ci.x-xc);
	    C(1,0) += e2*(ci.x-xc)*(ci.y-yc);
	    C(2,0) += e2*(ci.x-xc)*(ci.z-zc);
	    
	    C(1,1) += e2*(ci.y-yc)*(ci.y-yc);
	    C(2,1) += e2*(ci.y-yc)*(ci.z-zc);
	    
	    C(2,2) += e2*(ci.z-zc)*(ci.z-zc);
	    w += e2;
	  } 
	  C/=w;
	  
	  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigensolver(C);
	  if (eigensolver.info() != Eigen::Success) {
	    msg(MSG::WARNING) << "Failed to compute Eigenvalues -> Can't determine shower axis" << endmsg;
	  }
	  else {
	    // don't use the principal axes if at least one of the 3 
	    // diagonal elements is 0

	    const Eigen::Vector3d& S=eigensolver.eigenvalues(); 
	    const Eigen::Matrix3d& U=eigensolver.eigenvectors();

            const double epsilon = 1.E-6;

	    if ( !( std::abs(S[0]) < epsilon || std::abs(S[1]) < epsilon || std::abs(S[2]) < epsilon ) ) { 
	    
	      Amg::Vector3D prAxis(showerAxis);
	      int iEigen = -1;
	    
	      for (i=0;i<3;i++) {
		Amg::Vector3D tmpAxis=U.col(i);

		// calculate the angle		
		double tmpAngle=Amg::angle(tmpAxis,showerAxis);
		
		if ( tmpAngle > 90*deg ) { 
		  tmpAngle = 180*deg - tmpAngle;
		  tmpAxis = -tmpAxis;
		}
		
		if ( iEigen == -1 || tmpAngle < angle ) {
		  iEigen = i;
		  angle = tmpAngle;
		  prAxis = tmpAxis;
		}
	      }//end for loop 	  
	    
	      // calculate theta and phi angle differences
	      
	      deltaPhi = CaloPhiRange::diff(showerAxis.phi(),prAxis.phi());
	    
	      deltaTheta = showerAxis.theta() - prAxis.theta();
	    
	      // check the angle
	    
	      if ( angle < m_maxAxisAngle ) {
		showerAxis = prAxis;
	      }
	      else 
		ATH_MSG_DEBUG("principal Direction (" << prAxis[Amg::x] << ", " 
			      << prAxis[Amg::y] << ", " << prAxis[Amg::z] << ") deviates more than " 
			      << m_maxAxisAngle*(1./deg) 
			      << " deg from IP-to-ClusterCenter-axis (" << showerAxis[Amg::x] << ", "
			      << showerAxis[Amg::y] << ", " << showerAxis[Amg::z] << ")");
	    }//end if fabs(S)<epsilon
	    else {
	      ATH_MSG_DEBUG("Eigenvalues close to 0, do not use principal axis");
	    }
	  }//end got eigenvalues
	} //end if ncell>2
      
	ATH_MSG_DEBUG("Shower Axis = (" << showerAxis[Amg::x] << ", "
		      << showerAxis[Amg::y] << ", " << showerAxis[Amg::z] << ")");
	

	// calculate radial distance from and the longitudinal distance
	// along the shower axis for each cell. The cluster center is 
	// at r=0 and lambda=0
	
	for (auto& ci : cellinfo) {
	  const Amg::Vector3D currentCell(ci.x,ci.y,ci.z);
	  // calculate distance from shower axis r
	  ci.r = ((currentCell-showerCenter).cross(showerAxis)).mag();
	  // calculate distance from shower center along shower axis
	  ci.lambda = (currentCell-showerCenter).dot(showerAxis);
	}  
	
	// loop over all positive energy cells and calculate all desired moments
	
	// define common norm for all simple moments
	double commonNorm = 0;
        double phi0 = ncell > 0 ? cellinfo[0].phi : 0;

	for(unsigned i=0;i<ncell;i++) {
	  const CaloClusterMomentsMaker_detail::cellinfo& ci = cellinfo[i];
	  // loop over all valid moments
	  commonNorm += ci.energy;
 	  for(size_t iMoment = 0, size = m_validMoments.size();
              iMoment != size;
              ++ iMoment)
          {
	    // now calculate the actual moments
	    switch (m_validMoments[iMoment]) {
	    case xAOD::CaloCluster::FIRST_ETA:
	      myMoments[iMoment] += ci.energy*ci.eta;
	      break;
	    case xAOD::CaloCluster::FIRST_PHI:
	      // first cell decides the sign in order to avoid
	      // overlap problem at phi = -pi == +pi
	      // need to be normalized to the range [-pi,+pi] in the end
              myMoments[iMoment] += ci.energy * proxim (ci.phi, phi0);
	      break;
	    case xAOD::CaloCluster::SECOND_R:
	      myMoments[iMoment] += ci.energy*ci.r*ci.r;
	      break;
	    case xAOD::CaloCluster::SECOND_LAMBDA:
	      myMoments[iMoment] += ci.energy*ci.lambda*ci.lambda;
	      break;
	    case xAOD::CaloCluster::LATERAL:
	      if ( (int)i != iCellMax && (int)i != iCellScndMax ) {
		myMoments[iMoment] += ci.energy*ci.r*ci.r;
		myNorms[iMoment] += ci.energy*ci.r*ci.r;
	      }
	      else {
		double rm = ci.r;
		if ( rm < m_minRLateral ) 
		  rm = m_minRLateral;
		myNorms[iMoment] += rm*rm*ci.energy;
	      }
	      break;
	    case xAOD::CaloCluster::LONGITUDINAL:
	      if ( (int)i != iCellMax && (int)i != iCellScndMax ) {
		myMoments[iMoment] += ci.energy*ci.lambda*ci.lambda;
		myNorms[iMoment] += ci.energy*ci.lambda*ci.lambda;
	      }
	      else {
		double lm = ci.lambda;
		if ( lm < m_minLLongitudinal ) 
		  lm = m_minLLongitudinal;
		myNorms[iMoment] += lm*lm*ci.energy;
	      }
	      break;
	    case xAOD::CaloCluster::FIRST_ENG_DENS:
	      if ( ci.volume > 0 ) {
		myMoments[iMoment] += ci.energy*ci.energy/ci.volume;
		myNorms[iMoment] += ci.energy;
	      }
	      break;
	    case xAOD::CaloCluster::SECOND_ENG_DENS:
	      if ( ci.volume > 0 ) {
		myMoments[iMoment] += ci.energy*std::pow(ci.energy/ci.volume,2);
		myNorms[iMoment] += ci.energy;
	      }
	      break;
	    case xAOD::CaloCluster::ENG_FRAC_EM:
	      if ( ci.sample == CaloCell_ID::EMB1 
		   || ci.sample == CaloCell_ID::EMB2 
		   || ci.sample == CaloCell_ID::EMB3 
		   || ci.sample == CaloCell_ID::EME1
		   || ci.sample == CaloCell_ID::EME2
		   || ci.sample == CaloCell_ID::EME3
		   || ci.sample == CaloCell_ID::FCAL0 )
		myMoments[iMoment] += ci.energy;
	      break;
	    case xAOD::CaloCluster::ENG_FRAC_MAX:
	      if ( (int)i == iCellMax ) 
		myMoments[iMoment] = ci.energy;
	      break;
	    case xAOD::CaloCluster::PTD:
	      // do not convert to pT since clusters are small and
	      // there is virtually no difference and cosh just costs
	      // time ...
	      myMoments[iMoment] += ci.energy*ci.energy;
	      myNorms[iMoment] += ci.energy;
	      break;
	    default:
	      // nothing to be done for other moments
	      break;
	    }
	  }
	} //end of loop over cell
      
	
	// assign moments which don't need the loop over the cells
        for (size_t iMoment = 0, size = m_validMoments.size();
             iMoment != size;
             ++ iMoment)
        {
	  // now calculate the actual moments
          switch (m_validMoments[iMoment]) {
	  case xAOD::CaloCluster::FIRST_ETA:
	  case xAOD::CaloCluster::FIRST_PHI:
	  case xAOD::CaloCluster::SECOND_R:
	  case xAOD::CaloCluster::SECOND_LAMBDA:
	  case xAOD::CaloCluster::ENG_FRAC_EM:
	  case xAOD::CaloCluster::ENG_FRAC_MAX:
	    myNorms[iMoment] = commonNorm;
	    break;
	  case xAOD::CaloCluster::DELTA_PHI:
	    myMoments[iMoment] = deltaPhi;
	    break;
	  case xAOD::CaloCluster::DELTA_THETA:
	    myMoments[iMoment] = deltaTheta;
	    break;
	  case xAOD::CaloCluster::DELTA_ALPHA:
	    myMoments[iMoment] = angle;
	    break;
	  case xAOD::CaloCluster::CENTER_X:
	    myMoments[iMoment] = showerCenter.x();
	    break;
	  case xAOD::CaloCluster::CENTER_Y:
	    myMoments[iMoment] = showerCenter.y();
	    break;
	  case xAOD::CaloCluster::CENTER_Z:
	    myMoments[iMoment] = showerCenter.z();
	    break;
	  case xAOD::CaloCluster::CENTER_MAG:
	    myMoments[iMoment] = showerCenter.mag();
	    break;
	  case xAOD::CaloCluster::CENTER_LAMBDA:
	    // calculate the longitudinal distance along the shower axis
	    // of the shower center from the calorimeter start
	    
	    // first need calo boundary at given eta phi try LAREM barrel
	    // first, then LAREM endcap OW, then LAREM endcap IW, then
	    // FCal
	    {
	      double r_calo(0),z_calo(0),lambda_c(0); 
	      r_calo = m_caloDepthTool->get_entrance_radius(CaloCell_ID::EMB1,
							    showerCenter.eta(),
							    showerCenter.phi(),
                  caloDDMgr);
	      if ( r_calo == 0 ) {
		z_calo = m_caloDepthTool->get_entrance_z(CaloCell_ID::EME1,
							 showerCenter.eta(),
							 showerCenter.phi(),
               caloDDMgr);
		if ( z_calo == 0 ) 
		  z_calo = m_caloDepthTool->get_entrance_z(CaloCell_ID::EME2,
							   showerCenter.eta(),
							   showerCenter.phi(),
                 caloDDMgr);
		if ( z_calo == 0 ) 
		  z_calo = m_caloDepthTool->get_entrance_z(CaloCell_ID::FCAL0,
							   showerCenter.eta(),
							   showerCenter.phi(),
                 caloDDMgr);
		if ( z_calo == 0 ) // for H6 TB without EMEC outer wheel 
		  z_calo = m_caloDepthTool->get_entrance_z(CaloCell_ID::HEC0,
							   showerCenter.eta(),
							   showerCenter.phi(),
                 caloDDMgr);
		if ( z_calo != 0 && showerAxis.z() != 0 ) {
		  lambda_c = fabs((z_calo-showerCenter.z())/showerAxis.z());
		}
	      }
	      else {
		double r_s2 = showerAxis.x()*showerAxis.x()
		  +showerAxis.y()*showerAxis.y();
		double r_cs = showerAxis.x()*showerCenter.x()
		  +showerAxis.y()*showerCenter.y();
		double r_cr = showerCenter.x()*showerCenter.x()
		  +showerCenter.y()*showerCenter.y()-r_calo*r_calo;
		if ( r_s2 > 0 ) {
		  double det = r_cs*r_cs/(r_s2*r_s2) - r_cr/r_s2;
		  if ( det > 0 ) {
		    det = sqrt(det);
		    double l1(-r_cs/r_s2);
		    double l2(l1);
		    l1 += det;
		    l2 -= det;
		    if ( fabs(l1) < fabs(l2) ) 
		      lambda_c = fabs(l1);
		    else
		      lambda_c = fabs(l2);
		  }
		}
	      }
	      myMoments[iMoment] = lambda_c;
	    }
	    break;
	  case xAOD::CaloCluster::ENG_FRAC_CORE:
	    for(i=0;i<(int)CaloCell_ID::Unknown;i++) 
	      myMoments[iMoment] += maxSampE[i];
	    myNorms[iMoment] = commonNorm;
	    break;
	  case xAOD::CaloCluster::ISOLATION:
	    {
	      // loop over empty and filled perimeter cells and
	      // get a weighted ratio by means of energy fraction per layer
	      for(unsigned int i=0; i != CaloSampling::Unknown; ++ i) {
		const xAOD::CaloCluster::CaloSample s=( xAOD::CaloCluster::CaloSample)(i);
		if (theCluster->hasSampling(s)) {
		  const double eSample = theCluster->eSample(s);
		  if (eSample > 0) {
		    int nAll = nbEmpty[i]+nbNonEmpty[i];
		    if (nAll > 0) {
		      myMoments[iMoment] += (eSample*nbEmpty[i])/nAll;
		      myNorms[iMoment] += eSample;
		    }
		  }//end of eSample>0
		}//end has sampling
	      }//end loop over samplings
	    }
	    break;
	  case xAOD::CaloCluster::ENG_BAD_CELLS:
	    myMoments[iMoment] = eBad;
	    break;
	  case xAOD::CaloCluster::N_BAD_CELLS:
	    myMoments[iMoment] = nbad;
            break;
          case xAOD::CaloCluster::N_BAD_CELLS_CORR:
            myMoments[iMoment] = nbad_dac;
            break;
	  case xAOD::CaloCluster::BAD_CELLS_CORR_E:
	    myMoments[iMoment] = ebad_dac;
            break;
	  case xAOD::CaloCluster::BADLARQ_FRAC:
	    myMoments[iMoment] = eBadLArQ/(theCluster->e()!=0.?theCluster->e():1.);
            break;
	  case xAOD::CaloCluster::ENG_POS:
	    myMoments[iMoment] = ePos;
            break;
	  case xAOD::CaloCluster::SIGNIFICANCE:
	    myMoments[iMoment] = (sumSig2>0?theCluster->e()/sqrt(sumSig2):0.);
            break;
	  case xAOD::CaloCluster::CELL_SIGNIFICANCE:
	    myMoments[iMoment] = maxAbsSig;
            break;
	  case xAOD::CaloCluster::CELL_SIG_SAMPLING:
	    myMoments[iMoment] = nSigSampl;
            break;
	  case xAOD::CaloCluster::AVG_LAR_Q:
	    myMoments[iMoment] = eLAr2Q/(eLAr2>0?eLAr2:1);
            break;
	  case xAOD::CaloCluster::AVG_TILE_Q:
	    myMoments[iMoment] = eTile2Q/(eTile2>0?eTile2:1);
            break;
	  case xAOD::CaloCluster::ENG_BAD_HV_CELLS:
	    myMoments[iMoment] = eBadLArHV;
	    break;
	  case xAOD::CaloCluster::N_BAD_HV_CELLS:
	    myMoments[iMoment] = nBadLArHV;
            break;
	  case xAOD::CaloCluster::PTD:
	    myMoments[iMoment] = sqrt(myMoments[iMoment]);
            break;
	  case xAOD::CaloCluster::MASS:
	    myMoments[iMoment] = mass;
	    break;
	  default:
	    // nothing to be done for other moments
	    break;
	  }
	}
      }

      // normalize moments and copy to Cluster Moment Store
      size_t size= m_validMoments.size();
      for (size_t iMoment = 0; iMoment != size; ++iMoment) {
        xAOD::CaloCluster::MomentType moment = m_validMoments[iMoment];
	if ( myNorms[iMoment] != 0 ) 
	  myMoments[iMoment] /= myNorms[iMoment];
	if ( moment == xAOD::CaloCluster::FIRST_PHI ) 
	  myMoments[iMoment] = CaloPhiRange::fix(myMoments[iMoment]);
	theCluster->insertMoment(moment,myMoments[iMoment]);
      } // loop on moments for cluster
    } // check on requested moments
    // check on second moment of time if requested
    if ( m_secondTime ) { theCluster->insertMoment(xAOD::CaloCluster::SECOND_TIME,theCluster->secondTime()); }
    // check on number of cells per sampling moment if requested
    if ( m_nCellsPerSampling ) {
      for ( size_t isam(0); isam < nCellsSamp.size(); ++isam ) { 
	theCluster->setNumberCellsInSampling((CaloCell_ID::CaloSample)isam,std::get<0>(nCellsSamp.at(isam)),false);
	if ( isam == (size_t)CaloCell_ID::EME2 && std::get<1>(nCellsSamp.at(isam)) > 0 ) { 
	  theCluster->setNumberCellsInSampling((CaloCell_ID::CaloSample)isam,std::get<1>(nCellsSamp.at(isam)),true); 
	}
      } // loop on samplings
      nCellsSamp.clear();
    }
  } // loop on clusters

  return StatusCode::SUCCESS;
}
