/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// conditions
#include "PixelCalibAlgs/PixelChargeToTConversion.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h" 
#include "CxxUtils/checker_macros.h"
#include "PixelGeoModel/IBLParameterSvc.h" 
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"

PixelChargeToTConversion::PixelChargeToTConversion(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_IBLParameterSvc("IBLParameterSvc",name)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

PixelChargeToTConversion::~PixelChargeToTConversion(){}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixelChargeToTConversion::initialize(){

  ATH_MSG_INFO( "Initializing PixelChargeToTConversion" );

  if (m_IBLParameterSvc.retrieve().isFailure()) { 
      ATH_MSG_FATAL("Could not retrieve IBLParameterSvc"); 
      return StatusCode::FAILURE; 
  } else  
      ATH_MSG_INFO("Retrieved service " << m_IBLParameterSvc); 

  ATH_CHECK(m_pixelCabling.retrieve());
  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_chargeDataKey.initialize());
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_pixelsClustersKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelChargeToTConversion::execute ATLAS_NOT_THREAD_SAFE (){ // const_cast is used.
  const EventContext &ctx = Gaudi::Hive::currentContext();
  SG::ReadHandle<InDet::PixelClusterContainer> pixel_container( m_pixelsClustersKey, ctx);
  if (!pixel_container.isValid())
    {
     ATH_MSG_ERROR( "Pixel Cluster container " << m_pixelsClustersKey.key() << " not found" );
      return StatusCode::RECOVERABLE;
    }
  ATH_MSG_DEBUG( "Pixel Cluster container found:  " << pixel_container->size() << " collections" );

  int overflowIBLToT=0;
  if( m_IBLParameterSvc->containsIBL()) {
    overflowIBLToT = SG::ReadCondHandle<PixelModuleData>(m_moduleDataKey)->getFEI4OverflowToT(0,0);
  }

  SG::ReadCondHandle<PixelChargeCalibCondData> calibData(m_chargeDataKey,ctx);

  typedef InDet::PixelClusterContainer::const_iterator ClusterIter;
  ClusterIter itrCluster;
  ClusterIter itrClubeg=pixel_container->begin();
  ClusterIter itrCluend=pixel_container->end();
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
        return StatusCode::FAILURE; 
       }
      const AtlasDetectorID* aid = element->getIdHelper();
      if (aid==0){
        ATH_MSG_ERROR("Could not get ATLASDetectorID");
      }
      const PixelID* pixelIDp=dynamic_cast<const PixelID*>(aid);
      if (!pixelIDp){
        ATH_MSG_ERROR("Could not get PixelID pointer");
        return StatusCode::FAILURE;
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

    Identifier moduleID = pixelID.wafer_id(pixid);
    IdentifierHash moduleHash = pixelID.wafer_hash(moduleID);
    int circ = m_pixelCabling->getFE(&pixid,moduleID);
    int type = m_pixelCabling->getPixelType(pixid);
    int totInt = (int)calibData->getToT((int)moduleHash, circ, type, Charges[i]);

    if( m_IBLParameterSvc->containsIBL() && pixelID.barrel_ec(pixid) == 0 && pixelID.layer_disk(pixid) == 0 ) {
      int tot0 = totInt;
      if ( totInt >= overflowIBLToT ) totInt = overflowIBLToT;
      msg(MSG::DEBUG) << "barrel_ec = " << pixelID.barrel_ec(pixid) << " layer_disque = " <<  pixelID.layer_disk(pixid) << " ToT = " << tot0 << " Real ToT = " << totInt << endmsg;
    }

    totList.push_back( totInt ) ; // Fudge to make sure we round to the correct number
    ATH_MSG_DEBUG( "from Charge --> ToT   " << Charge <<"  "<< totInt);
    sumToT += totInt;
  }
  ATH_MSG_DEBUG( "sumToT   " << sumToT);
  theNonConstCluster->setToTList (std::move (totList));
      }

    }//loop over clusters

  }//loop over collections

 
 return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode PixelChargeToTConversion::finalize(){
  return StatusCode::SUCCESS;
}
