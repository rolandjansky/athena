/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// conditions
#include "PixelCalibAlgs/PixelChargeToTConversion.h"
#include "PixelConditionsServices/IPixelCalibSvc.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#define private public

#include "PixelGeoModel/IBLParameterSvc.h" 
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

#include "PixelConditionsServices/IPixelOfflineCalibSvc.h"

PixelChargeToTConversion::PixelChargeToTConversion(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_calibsvc("PixelCalibSvc", name),
  m_IBLParameterSvc("IBLParameterSvc",name),
  m_overflowIBLToT(0),
  m_offlineCalibSvc("PixelOfflineCalibSvc", name)
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

  if ( !m_offlineCalibSvc.empty() ) {
    StatusCode sc = m_offlineCalibSvc.retrieve();
    if (sc.isFailure() || !m_offlineCalibSvc ) {
      ATH_MSG_ERROR( m_offlineCalibSvc.type() << " not found! ");
      return StatusCode::RECOVERABLE;
    }
    else{
      ATH_MSG_INFO ( "Retrieved tool " <<  m_offlineCalibSvc.type() );
    }
  }

  if (m_IBLParameterSvc.retrieve().isFailure()) { 
      ATH_MSG_FATAL("Could not retrieve IBLParameterSvc"); 
      return StatusCode::FAILURE; 
  } else  
      ATH_MSG_INFO("Retrieved service " << m_IBLParameterSvc); 
 
  m_overflowIBLToT = m_offlineCalibSvc->getIBLToToverflow();

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

      const InDetDD::SiDetectorElement* element=theCluster->detectorElement();
      if (element==0) {
        ATH_MSG_ERROR("Could not get detector element");
       }
      const AtlasDetectorID* aid = element->getIdHelper();
      if (aid==0){
        ATH_MSG_ERROR("Could not get ATLASDetectorID");
      }
      const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
      if (!pixelIDp){
        ATH_MSG_ERROR("Could not get PixelID pointer");
      } 
      const PixelID& pixelID = *pixelIDp;

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
       
          int totInt = (int) (tot + 0.1);

          if( m_IBLParameterSvc->containsIBL() && pixelID.barrel_ec(pixid) == 0 && pixelID.layer_disk(pixid) == 0 ) {
            int tot0 = totInt;
	    if ( totInt >= m_overflowIBLToT ) totInt = m_overflowIBLToT;
            msg(MSG::DEBUG) << "barrel_ec = " << pixelID.barrel_ec(pixid) << " layer_disque = " <<  pixelID.layer_disk(pixid) << " ToT = " << tot0 << " Real ToT = " << totInt << endreq;
          }
	  
	  totList.push_back( totInt ) ; // Fudge to make sure we round to the correct number
	  ATH_MSG_DEBUG( "from Charge --> ToT   " << Charge <<"  "<< totInt);
	  sumToT += totInt;
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
