/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "SimpleMMClusterBuilderTool.h"

#include "MuonPrepRawData/MMPrepData.h"

using namespace Muon;

Muon::SimpleMMClusterBuilderTool::SimpleMMClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p) {
  declareInterface<IMMClusterBuilderTool>(this);
  declareProperty("writeStripProperties", m_writeStripProperties = true ); // true  for debugging; needs to become false for large productions
  declareProperty("useErrorParametrization", m_useErrorParametrization = true);
  declareProperty("maxHoleSize", m_maxHoleSize = 1);
}

StatusCode Muon::SimpleMMClusterBuilderTool::initialize()
{
  ATH_CHECK(m_idHelperSvc.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode Muon::SimpleMMClusterBuilderTool::getClusters(std::vector<Muon::MMPrepData>& MMprds, 
							 std::vector<std::unique_ptr<Muon::MMPrepData>>& clustersVect) const 

{
  ATH_MSG_DEBUG("Size of the input vector: " << MMprds.size()); 
  ATH_MSG_DEBUG("Size of the output vector: " << clustersVect.size()); 
  std::vector<int> MMflag;
  IdentifierHash hash;

  if ( MMprds.size() > 0 ) {
    hash = MMprds.at(0).collectionHash();
  }
  else {
    ATH_MSG_DEBUG("Empty PRD collection: no clusterization" );
    return StatusCode::SUCCESS;
  }

  for (unsigned int i=0; i<MMprds.size(); ++i){
    MMflag.push_back(0);
  }

  for (unsigned int i=0; i<MMprds.size(); ++i){
    // skip the merged prds
    if(MMflag[i]==1) continue;
    
    unsigned int jmerge = -1;
    Identifier id_prd = MMprds[i].identify();
    int strip = m_idHelperSvc->mmIdHelper().channel(id_prd);
    int gasGap  = m_idHelperSvc->mmIdHelper().gasGap(id_prd);
    int layer   = m_idHelperSvc->mmIdHelper().multilayer(id_prd);
    ATH_MSG_VERBOSE("  MMprds " <<  MMprds.size() <<" index "<< i << " strip " << strip
		    << " gasGap " << gasGap << " layer " << layer << " z " << MMprds[i].globalPosition().z());
    for (unsigned int j = i+1; j < MMprds.size(); ++j) {
      Identifier id_prdN = MMprds[j].identify();
      int stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
      int gasGapN  = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
      int layerN   = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
      if (gasGapN == gasGap && layerN == layer) {
          ATH_MSG_VERBOSE(" next MMprds strip same gasGap and layer index " << j
                          << " strip " << stripN
                          << " gasGap " << gasGapN
                          << " layer " << layerN);
          if (static_cast<unsigned int>(std::abs(strip-stripN)) <= m_maxHoleSize + 1) {
          jmerge = j;
          break;
        }
      }
    }

    unsigned int nmerge = 0;
    std::vector<Identifier> rdoList;
    std::vector<unsigned int> mergeIndices;
    std::vector<uint16_t> mergeStrips;
    std::vector<short int> mergeStripsTime;
    std::vector<int> mergeStripsCharge;
    std::vector<float> mergeStripsDriftDists;
    std::vector<Amg::MatrixX> mergeStripsDriftDistErrors;


    rdoList.push_back(id_prd);
    MMflag[i] = 1;
    mergeIndices.push_back(i);
    mergeStrips.push_back(strip);
    if (m_writeStripProperties) {
      mergeStripsTime.push_back(MMprds[i].time());
      mergeStripsCharge.push_back(MMprds[i].charge());
    }
    mergeStripsDriftDists.push_back(MMprds[i].driftDist());
    mergeStripsDriftDistErrors.push_back(MMprds[i].localCovariance());

    unsigned int nmergeStrips = 1;
    unsigned int nmergeStripsMax = 50;
    for (unsigned int k=0; k < nmergeStripsMax; ++k) {
      for (unsigned int j=jmerge; j<MMprds.size(); ++j){
	if(MMflag[j] == 1) continue;
	Identifier id_prdN = MMprds[j].identify();
	int stripN = m_idHelperSvc->mmIdHelper().channel(id_prdN);
	if( static_cast<unsigned int>(std::abs(mergeStrips[k]-stripN)) <= m_maxHoleSize + 1 ) {
	  int gasGapN  = m_idHelperSvc->mmIdHelper().gasGap(id_prdN);
	  int layerN   = m_idHelperSvc->mmIdHelper().multilayer(id_prdN);
	  if( gasGapN==gasGap && layerN==layer ) {
	    if(mergeStrips[k]==stripN) {
	      MMflag[j] = 1;
	      continue;
	    }
	    nmerge++;
	    rdoList.push_back(id_prdN);
	    MMflag[j] = 1;
	    mergeIndices.push_back(j);
	    mergeStrips.push_back(stripN);
      if(m_writeStripProperties) {
        mergeStripsTime.push_back(MMprds[j].time());
        mergeStripsCharge.push_back(MMprds[j].charge());
      }
      mergeStripsDriftDists.push_back(MMprds[j].driftDist());
      mergeStripsDriftDistErrors.push_back(MMprds[j].localCovariance());
	    nmergeStrips++;
	  }
	}
      }
      if(k>=nmergeStrips) break;
    }
    ATH_MSG_VERBOSE(" add merged MMprds nmerge " << nmerge << " strip " << strip << " gasGap " << gasGap << " layer " << layer );

    // start off from strip in the middle
    int stripSum = 0;
    for (unsigned int k =0; k<mergeStrips.size(); ++k) {
      stripSum += mergeStrips[k];
    }
    stripSum = stripSum/mergeStrips.size();
    
    unsigned int j = jmerge;
    for (unsigned int k =0; k<mergeStrips.size(); ++k) {
      if(mergeStrips[k]==stripSum) j = mergeIndices[k];
      ATH_MSG_VERBOSE(" merged strip nr " << k <<  " strip " << mergeStrips[k] << " index " << mergeIndices[k]);
    }
    ATH_MSG_VERBOSE(" Look for strip nr " << stripSum << " found at index " << j);

    ///
    /// get the local position from the cluster centroid
    ///
    double weightedPosX = 0.0;
    double posY = 0.0;
    double totalCharge = 0.0;
    double theta = 0.0;
    if ( mergeStrips.size() > 0 ) { 
      /// get the Y local position from the first strip ( it's the same for all strips in the cluster)
      posY = MMprds[mergeIndices[0]].localPosition().y();
      for ( unsigned int k=0 ; k<mergeStrips.size() ; ++k ) {
	double posX = MMprds[mergeIndices[k]].localPosition().x();
	double charge = MMprds[mergeIndices[k]].charge();
	weightedPosX += posX*charge;
	totalCharge += charge;
  theta += std::atan(MMprds[mergeIndices[k]].globalPosition().perp()/std::abs(MMprds[mergeIndices[k]].globalPosition().z()))*charge;
	ATH_MSG_VERBOSE("Adding a strip to the centroid calculation: charge=" << charge);
      } 
      weightedPosX = weightedPosX/totalCharge;
      theta /= totalCharge;
    }

    
    Amg::Vector2D clusterLocalPosition(weightedPosX,posY);
    
    double covX = MMprds[j].localCovariance()(Trk::locX, Trk::locX);

    ///
    /// memory allocated dynamically for the PrepRawData is managed by Event Store
    ///
    Amg::MatrixX* covN = new Amg::MatrixX(1,1);
    covN->setIdentity();
    if(!m_useErrorParametrization) {
      (*covN)(0,0) = 6.*(nmerge + 1.)*covX;
      if(nmerge<=1) (*covN)(0,0) = covX;
    } else {
      double localUncertainty = 0.074+0.66*theta-0.15*theta*theta;
      if ( m_idHelperSvc->mmIdHelper().isStereo(MMprds[i].identify()) ) {
      	localUncertainty = 10.;
      } 
      (*covN)(0,0) = localUncertainty * localUncertainty;
    }
    ATH_MSG_VERBOSE(" make merged prepData at strip " << m_idHelperSvc->mmIdHelper().channel(MMprds[j].identify()) << " nmerge " << nmerge << " sqrt covX " << sqrt((*covN)(0,0)));
    
    ///
    /// memory allocated dynamically for the PrepRawData is managed by Event Store
    ///
    std::unique_ptr<Muon::MMPrepData> prdN = std::make_unique<MMPrepData>(MMprds[j].identify(), hash, clusterLocalPosition,
              rdoList, covN, MMprds[j].detectorElement(),
              static_cast<short int> (0), static_cast<int>(totalCharge), static_cast<float>(0.0),
              (m_writeStripProperties ? mergeStrips : std::vector<uint16_t>(0) ),
              mergeStripsTime, mergeStripsCharge);
    prdN->setDriftDist(mergeStripsDriftDists, mergeStripsDriftDistErrors);
    prdN->setAuthor(Muon::MMPrepData::Author::SimpleClusterBuilder);


    clustersVect.push_back(std::move(prdN));
  } // end loop MMprds[i]
  //clear vector and delete elements
  MMflag.clear();
  MMprds.clear();
 

  return StatusCode::SUCCESS;
}
