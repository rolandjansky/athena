/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// conditions
#include "PixelCalibAlgs/PixelChargeToTConversion.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#define private public
#include "InDetPrepRawData/PixelCluster.h"

PixelChargeToTConversion::PixelChargeToTConversion(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_calibsvc("PixelCalibSvc", name)
{
  declareProperty("PixelCalibSvc", m_calibsvc);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

PixelChargeToTConversion::~PixelChargeToTConversion(){}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixelChargeToTConversion::initialize(){

  ATH_MSG_INFO( "Initializing PixelChargeToTConversion" );

  if (StatusCode::SUCCESS!=m_calibsvc.retrieve() ) {
    msg(MSG::FATAL) << "PixelCalibSvc not found" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << " PixelCalibSvc found " << endreq;

  return StatusCode::SUCCESS;
}

StatusCode PixelChargeToTConversion::execute(){

  m_Pixel_clcontainer=0;
  StatusCode sc = evtStore()->retrieve(m_Pixel_clcontainer, m_PixelsClustersName);
  if (sc.isFailure()  || ! m_Pixel_clcontainer)
    {
      ATH_MSG_ERROR( "Pixel Cluster container not found" );
      return StatusCode::RECOVERABLE;
    }
  ATH_MSG_DEBUG( "Pixel Cluster container found:  " << m_Pixel_clcontainer->size() << " collections" );

  typedef InDet::PixelClusterContainer::const_iterator ClusterIter;
  ClusterIter itrCluster;
  ClusterIter itrClubeg=m_Pixel_clcontainer->begin();
  ClusterIter itrCluend=m_Pixel_clcontainer->end();
  for( itrCluster=itrClubeg;itrCluster!=itrCluend;++itrCluster){
    const InDet::PixelClusterCollection* ClusterCollection(*itrCluster);
    if (!ClusterCollection) continue;
    for(DataVector<InDet::PixelCluster>::const_iterator p_clus=ClusterCollection->begin(); p_clus!=ClusterCollection->end(); ++p_clus){
      const InDet::PixelCluster* theCluster = *p_clus;
      const std::vector<Identifier>& RDOs = theCluster->rdoList();
      const std::vector<int>& ToTs = theCluster->totList();
      const std::vector<float>& Charges = theCluster->chargeList();
      auto theNonConstCluster = const_cast<InDet::PixelCluster*> (theCluster);
      ATH_MSG_DEBUG( "cluster RDOs , size, ToTs, size, Charges, size    "<< RDOs <<"  "<<RDOs.size()<<"  "<< ToTs<<"  " <<ToTs.size()<<"  "<<Charges<<"  "<<Charges.size());

      int nRDO=RDOs.size(); 
      // convert from Charge -> ToT
      if(ToTs.size()==0 && Charges.size()!=0){
	// safety check in case proper conddb is not used and vector of charges is filled with zeros
	auto biggest_charge = std::max_element(std::begin(Charges), std::end(Charges));
	ATH_MSG_DEBUG( "Max element of Charges is " << *biggest_charge);
	if(*biggest_charge==0.) return StatusCode::SUCCESS;
	//
	int sumToT = 0;
	std::vector<int> totList;
	for (int i=0; i<nRDO; i++) {
	  Identifier pixid=RDOs[i];
	  int Charge=Charges[i];
          float A = m_calibsvc->getQ2TotA(pixid);
          float E = m_calibsvc->getQ2TotE(pixid);
          float C = m_calibsvc->getQ2TotC(pixid);
	  float tot;
          if ( A>0. && (Charge+C)!=0 ) {
            tot = A*(Charge+E)/(Charge+C);
          } else tot=0.;
	  ATH_MSG_DEBUG( "A   E   C  tot " << A <<"  "<<E <<"  "<<C<<"  "<<tot);
	  totList.push_back( (int) (tot + 0.1)) ; // Fudge to make sure we round to the correct number
	  ATH_MSG_DEBUG( "from Charge --> ToT   " << Charge <<"  "<<(int) (tot + 0.1) );
	  sumToT += (int) (tot + 0.1);
	}
	ATH_MSG_DEBUG( "sumToT   " << sumToT);
	theNonConstCluster->m_totList = totList; 
	theNonConstCluster->m_totalToT =  sumToT;
      }
      
    }//loop over clusters

  }//loop over collections



 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixelChargeToTConversion::finalize(){
  return StatusCode::SUCCESS;
}
