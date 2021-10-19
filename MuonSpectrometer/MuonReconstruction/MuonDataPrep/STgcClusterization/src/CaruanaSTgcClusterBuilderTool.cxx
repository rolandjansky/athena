/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaruanaSTgcClusterBuilderTool.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include <set>
#include <vector>
#include <cmath>

using namespace Muon;
using namespace MuonGM;

Muon::CaruanaSTgcClusterBuilderTool::CaruanaSTgcClusterBuilderTool(const std::string& t, const std::string& n, const IInterface* p) :
  AthAlgTool(t,n,p)
{
  declareProperty("ChargeCut", m_chargeCut=0.0);
  declareProperty("maxHoleSize",m_maxHoleSize=1);
  declareProperty("addError",m_addError=0);
  declareProperty("angularStripResolution", m_angularStripResolution=0.07);
}

StatusCode Muon::CaruanaSTgcClusterBuilderTool::initialize()
{
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK(m_DetectorManagerKey.initialize());
  return StatusCode::SUCCESS;
}

//
// Build the clusters given a vector of single-hit PRD
//
StatusCode Muon::CaruanaSTgcClusterBuilderTool::getClusters(std::vector<Muon::sTgcPrepData>& stripsVect, std::vector<Muon::sTgcPrepData*>& clustersVect) const
{

  ATH_MSG_DEBUG("Size of the input vector: " << stripsVect.size());

  //
  // define the identifier hash
  IdentifierHash hash;

  double resolution=0.;
  bool isStrip = false;
  if ( !stripsVect.empty() ) {
    resolution = stripsVect.at(0).localCovariance()(0,0);
    Identifier chanId = stripsVect.at(0).identify();
    if ( m_idHelperSvc->stgcIdHelper().channelType(chanId)==1 ) isStrip = true;
    ATH_MSG_DEBUG(" channelType " << m_idHelperSvc->stgcIdHelper().channelType(chanId));
    ATH_MSG_DEBUG("isStrip: " << isStrip << "Single channel resolution: " << resolution);
  }
  else {
    ATH_MSG_DEBUG("Size of the channel vectors is zero");
    return StatusCode::SUCCESS;
  }

  std::vector<std::set<unsigned int>> clustersStripNum[2][4];
  std::vector<std::vector<Muon::sTgcPrepData>> clusters[2][4];

  for ( auto& it : stripsVect ) {
    if ( !addStrip(it,clustersStripNum,clusters) ) {
      ATH_MSG_ERROR("Could not add a strip to the sTGC clusters");
      return StatusCode::FAILURE;
    }
  }

  // now add the clusters to the PRD container
  //
  clustersVect.clear();

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* detManager = DetectorManagerHandle.cptr();
  if(!detManager){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return StatusCode::FAILURE;
  }

  for ( unsigned int multilayer=0 ; multilayer<2 ; ++multilayer ) {
    for ( unsigned int gasGap=0 ; gasGap<4 ; ++gasGap ) {
      //
      // loop on the clusters of that gap
      //
      for ( unsigned int i=0 ; i<clusters[multilayer][gasGap].size() ; ++i ) {
        // get the cluster
        std::vector<Muon::sTgcPrepData> const &cluster = clusters[multilayer][gasGap].at(i);
        //
        // loop on the strips and set the cluster weighted position and charge
        //
        std::vector<Identifier> rdoList;
        //vectors to hold the properties of the elements of a cluster
        std::vector<int> elementsCharge;
        std::vector<short int> elementsTime;
        std::vector<uint16_t> elementsChannel;
        std::vector<double> elementsLocalPositions;
        std::vector<Identifier> elementsIdentifier;
        Identifier clusterId;
        double reconstructedPosX = 0.0;
        double posY = (cluster.at(0)).localPosition().y();

        for ( const auto& it : cluster ){
          rdoList.push_back(it.identify());
          elementsCharge.push_back(it.charge());
          elementsChannel.push_back(m_idHelperSvc->stgcIdHelper().channel(it.identify()));
          elementsTime.push_back(it.time());
          elementsLocalPositions.push_back(it.localPosition().x());
          elementsIdentifier.push_back(it.identify());
        }

        // If the cluster is a cluster of pads or wires, use the default weighted mean method to compute the reconstructed postion. If it is a cluster of strips (with multiplicity >= 3), we use the Caruana method of fitting a Gaussian to the distribution
        bool caruanaFail = false;   // If the Caruana method fails at some point this is changed to true and the reconstruction reverts to the weighted mean method

        // If it is a strip cluster, make sure there are 3 or more strips with non-zero charge, otherwise revert to the weighted mean method
        if (isStrip){
          int multiplicity = 0;
          for (auto stripCharge : elementsCharge){
            if (stripCharge > 0) multiplicity += 1;
          }
          if (multiplicity < 3) caruanaFail = true;
        }

        double sigmaSq = 0.0;
        if (isStrip && !caruanaFail){
          // Here we implement the Caruana method to reconstruct the position of the cluster
          AmgSymMatrix(3) elementPosMatrix;
          for (int i=0; i<3; i++){
            for (int j=0; j<=i; j++) elementPosMatrix.fillSymmetric(i, j, 0);
          }
          Amg::Vector3D chargeVector(0., 0., 0.);

          // Some strips are recorded with a charge of zero, these strips will break the reconstruction (as the log of the charges are taken) and hence must be removed from the cluster temporarily.
          std::vector<int> charges = elementsCharge;
          std::vector<double> stripPositions = elementsLocalPositions;
          bool zeroChargeFlag = false;
          do {
            zeroChargeFlag = false;
            for (size_t i_charge=0; i_charge<charges.size(); i_charge++){
              if (charges.at(i_charge) <= 0){
                charges.erase(charges.begin()+i_charge);
                stripPositions.erase(stripPositions.begin()+i_charge);
                zeroChargeFlag = true;
                break;
              }
            }
          } while (zeroChargeFlag);

          // The reconstruction method becomes much simpiler when the strip positions are shifted such that the center of the cluster is at zero.
          float clusterCenter = 0;
          for (size_t i_strip=0; i_strip<stripPositions.size(); i_strip++) clusterCenter += stripPositions.at(i_strip);
          clusterCenter = clusterCenter/stripPositions.size();

          std::vector<double> stripPositions_shifted = {};
          for (size_t i_strip=0; i_strip<stripPositions.size(); i_strip++) stripPositions_shifted.push_back(stripPositions.at(i_strip)-clusterCenter);

          // Now build the matrix equation
          for (size_t i_element=0; i_element<stripPositions_shifted.size(); i_element++){
            if (charges.at(i_element) == 0) continue;
            elementPosMatrix.fillSymmetric(0, 0, elementPosMatrix(0,0) + 1);
            elementPosMatrix.fillSymmetric(0, 1, elementPosMatrix(0,1) + stripPositions_shifted.at(i_element));
            elementPosMatrix.fillSymmetric(0, 2, elementPosMatrix(0,2) + std::pow(stripPositions_shifted.at(i_element), 2));
            elementPosMatrix.fillSymmetric(1, 2, elementPosMatrix(1,2) + std::pow(stripPositions_shifted.at(i_element), 3));
            elementPosMatrix.fillSymmetric(2, 2, elementPosMatrix(2,2) + std::pow(stripPositions_shifted.at(i_element), 4));
            const double log_charge = std::log(charges.at(i_element));
            chargeVector(0) += log_charge;
            chargeVector(1) += stripPositions_shifted.at(i_element)*log_charge;
            chargeVector(2) += std::pow(stripPositions_shifted.at(i_element), 2)*log_charge;
          }
          elementPosMatrix(1,1) = elementPosMatrix(0,2);

          if (elementPosMatrix.determinant() == 0) caruanaFail = true;   // If the matrix is singular then the reconstruction method will fail. Revert back to the weighted mean.

          if (!caruanaFail){
            Amg::Vector3D caruanaPars = (elementPosMatrix.inverse())*chargeVector;
            reconstructedPosX = clusterCenter - caruanaPars(1)/(2*caruanaPars(2));
            if (std::isnan(reconstructedPosX)) caruanaFail = true;       // In the event that the caruanaPars(2) parameter is 0 we reconstruct and infinite position. In this case we must revert back to the weighted mean method.

            if (!caruanaFail){
              // Find the channel that the cluster position reconstructs on top of and set the cluster id to it's id
              double minCenterDistance = 9999.99;
              int channelIndex = 0;
              for (size_t i_elem=0; i_elem<elementsLocalPositions.size(); i_elem++){
                if (minCenterDistance > abs(elementsLocalPositions.at(i_elem)-reconstructedPosX)){
                  minCenterDistance = abs(elementsLocalPositions.at(i_elem)-reconstructedPosX);
                  channelIndex = i_elem;
                }
              }
              clusterId = elementsIdentifier.at(channelIndex);

              // We denote caruanaPars = (a, b, c) and find the error on the b and c componant
              double gamma0 = 0;
              double gamma2 = 0;
              double gamma4 = 0;
              for (size_t i_strip=0; i_strip<stripPositions_shifted.size(); i_strip++){
                gamma0 += 1;
                gamma2 += std::pow(stripPositions_shifted.at(i_strip), 2);
                gamma4 += std::pow(stripPositions_shifted.at(i_strip), 4);
              }

              // We also need the tan(theta) of the cluster
              const MuonGM::sTgcReadoutElement* detEl = detManager->getsTgcReadoutElement(clusterId);
              Amg::Vector3D globPos(0., 0.,0.);
              detEl->stripGlobalPosition(clusterId, globPos);
              double tan_theta = std::sqrt(globPos.x()*globPos.x() + globPos.y()*globPos.y())/globPos.z();
              double spreadFactor = m_angularStripResolution*std::sqrt(1+12*tan_theta*tan_theta);
              // m_angularStripResolution = 0.07 *** This value is also used in the Digitization and should therefore be modified to reference the same variable used there ***

              double sigma_b = spreadFactor/std::sqrt(gamma2);
              double sigma_c = spreadFactor*std::sqrt(gamma0/(gamma0*gamma4-gamma2*gamma2));

              // Now propagate the Uncertainty to find the uncertainty on the mean
              sigmaSq = std::pow((1/(2*caruanaPars(2)))*sigma_b, 2) + std::pow((caruanaPars(1)/(2*caruanaPars(2)*caruanaPars(2)))*sigma_c, 2);
            }
          }
        }

        // If the cluster is not a cluster of strip, the Caruana method constructed a matrix that is not invertible, or we choose to skip the Caruana reconstruction completely then the position is calculated using a weighted mean.
        if (!isStrip || caruanaFail){
          double weightedPosX = 0.0;
          double totalCharge  = 0.0;
          sigmaSq = 0;
          for ( const auto& it : cluster ){
            double weight = 0.0;
            isStrip ? weight = it.charge() : weight = 1.0;
            ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);
            weightedPosX += it.localPosition().x()*weight;
            totalCharge += weight;
            ATH_MSG_DEBUG("Channel local position and charge: " << it.localPosition().x() << " " << it.charge() );
          }
          reconstructedPosX = weightedPosX/totalCharge;

          //
          // get the error on the cluster position
          //
          ATH_MSG_DEBUG("Cluster size: " << cluster.size());
          if ( cluster.size() > 1 ) {
            double weight = 0.0;
            for ( const auto& it : cluster ) {
              isStrip ? weight = it.charge() : weight = 1.0;
              ATH_MSG_DEBUG("isStrip: " << isStrip << " weight: " << weight);
              //sigmaSq += weight*(it.localPosition().x()-weightedPosX)*(it.localPosition().x()-weightedPosX);
              sigmaSq += weight*weight*resolution;
              ATH_MSG_DEBUG(">>>> posX: " << it.localPosition().x() << " weightedPosX: " << weightedPosX);
            }
          }
          else {
            sigmaSq = resolution;
          }
          sigmaSq = sigmaSq/(totalCharge*totalCharge);
          ATH_MSG_DEBUG("Uncertainty on cluster position is: " << sqrt(sigmaSq));

          // Find the channel that the cluster position reconstructs on top of and set the cluster id to it's id
          double minCenterDistance = 9999.99;
          int channelIndex = 0;
          for (size_t i_elem=0; i_elem<elementsLocalPositions.size(); i_elem++){
            if (minCenterDistance > abs(elementsLocalPositions.at(i_elem)-reconstructedPosX)){
              minCenterDistance = abs(elementsLocalPositions.at(i_elem)-reconstructedPosX);
              channelIndex = i_elem;
            }
          }
          clusterId = elementsIdentifier.at(channelIndex);
        }

        Amg::Vector2D localPosition(reconstructedPosX,posY);

        auto covN = Amg::MatrixX(1,1);
        covN(0,0) = sigmaSq + m_addError*m_addError;

        //
        // memory allocated dynamically for the PrepRawData is managed by Event Store in the converters
        //
        ATH_MSG_DEBUG("error on cluster " << sqrt((covN)(0,0)) << " added error " <<  m_addError);

        sTgcPrepData* prdN = new sTgcPrepData(
          clusterId,
          hash,
          localPosition,
          rdoList,
          covN,
          cluster.at(0).detectorElement(),
          std::accumulate(elementsCharge.begin(), elementsCharge.end(), 0),
          (short int)0,
          (uint16_t)0,
          elementsChannel,
          elementsTime,
          elementsCharge);
        clustersVect.push_back(prdN);
      }
    }
  }

  ATH_MSG_DEBUG("Size of the output cluster vector: " << clustersVect.size());

  return StatusCode::SUCCESS;
}


bool Muon::CaruanaSTgcClusterBuilderTool::addStrip(const Muon::sTgcPrepData& strip, std::vector<std::set<unsigned int>> (&clustersStripNum)[2][4],
                  std::vector<std::vector<Muon::sTgcPrepData>> (&clusters)[2][4])const
{

  Identifier prd_id = strip.identify();
  int channelType = m_idHelperSvc->stgcIdHelper().channelType(prd_id);
  int multilayer = m_idHelperSvc->stgcIdHelper().multilayer(prd_id)-1;
  int gasGap = m_idHelperSvc->stgcIdHelper().gasGap(prd_id)-1;
  unsigned int stripNum = m_idHelperSvc->stgcIdHelper().channel(prd_id);

  ATH_MSG_DEBUG(">>>>>>>>>>>>>> In addStrip: channelType, multilayer, gasGap, stripNum: " << channelType
      << " " << multilayer << " "
      << gasGap << " " << stripNum);

  // if no cluster is present start creating a new one
  if (clustersStripNum[multilayer][gasGap].empty() ) {

    ATH_MSG_DEBUG( ">>> No strip present in this gap: adding it as first cluster " );
    std::set<unsigned int> clusterStripNum;
    std::vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    std::vector<std::set<unsigned int>> &clustersOfLayerStripNum = clustersStripNum[multilayer][gasGap];
    std::vector<std::vector<Muon::sTgcPrepData>> &clustersOfLayer = clusters[multilayer][gasGap];

    clustersOfLayerStripNum.emplace_back();
    clustersOfLayerStripNum.back().insert(stripNum);
    clustersOfLayer.emplace_back();
    clustersOfLayer.back().push_back(strip);

    return true;
  }
  else {

    //
    // check if the strip can be added to a cluster
    //
    for ( unsigned int i=0 ; i<clustersStripNum[multilayer][gasGap].size() ; ++i  ) {

      std::set<unsigned int> &clusterStripNum = clustersStripNum[multilayer][gasGap].at(i);

      unsigned int firstStrip = *(clusterStripNum.begin());
      unsigned int lastStrip  = *(--clusterStripNum.end());
      ATH_MSG_DEBUG("First strip and last strip are: " << firstStrip << " " << lastStrip);
      unsigned int diffFirst = (stripNum-firstStrip) > 0 ? stripNum - firstStrip : firstStrip-stripNum ;
      unsigned int diffLast  = (stripNum-lastStrip)  > 0 ? stripNum - lastStrip  : lastStrip-stripNum ;
      if ( diffFirst<=m_maxHoleSize + 1 || diffLast<=m_maxHoleSize + 1) {

        ATH_MSG_DEBUG(">> inserting a new strip");
	      clustersStripNum[multilayer][gasGap].at(i).insert(stripNum);
	      clusters[multilayer][gasGap].at(i).push_back(strip);

        ATH_MSG_DEBUG("size after inserting is: " << clustersStripNum[multilayer][gasGap].at(i).size());
        ATH_MSG_DEBUG("and the first and last strip are: "
          << *(clustersStripNum[multilayer][gasGap].at(i).begin()) << " "
          << *(--clustersStripNum[multilayer][gasGap].at(i).end()));
	return true;
      }
    }
    // if not, build a new cluster starting from it
    //
    std::set<unsigned int> clusterStripNum;
    std::vector<Muon::sTgcPrepData> cluster;

    clusterStripNum.insert(stripNum);
    cluster.push_back(strip);

    clustersStripNum[multilayer][gasGap].push_back(std::move(clusterStripNum));
    clusters[multilayer][gasGap].push_back(std::move(cluster));

    return true;
  }

  return false;
}

///
/// sort the strips if needed
void CaruanaSTgcClusterBuilderTool::dumpStrips( std::vector<Muon::sTgcPrepData>& stripsVect,
					       std::vector<Muon::sTgcPrepData*>& clustersVect ) const
{

  ATH_MSG_INFO("====> Dumping all strips:  ");
  for ( const auto& it : stripsVect ) {
    Identifier stripId = it.identify();
    ATH_MSG_INFO("Strip identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(stripId) );
  }

  ATH_MSG_INFO("Dumping all clusters:  ");
  for ( auto *it : clustersVect ) {
    Identifier clusterId = it->identify();
    ATH_MSG_INFO("***> New cluster identifier: " << m_idHelperSvc->stgcIdHelper().show_to_string(clusterId) );
    ATH_MSG_INFO("Cluster size: " << it->rdoList().size() );
    ATH_MSG_INFO("List of associated RDO's: ");

  }

  return;
}
