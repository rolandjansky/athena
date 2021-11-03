/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BcmCollisionTimeAlg.h"
#include "BCM_CollisionTime/BcmCollisionTime.h"

#include "Identifier/Identifier.h"


#include "AthenaKernel/errorcheck.h"

//Constructor
BcmCollisionTimeAlg:: BcmCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator)
  {
  }

//__________________________________________________________________________
//Destructor
  BcmCollisionTimeAlg::~BcmCollisionTimeAlg()
  {
  }

//__________________________________________________________________________
StatusCode BcmCollisionTimeAlg::initialize()
  {
    ATH_CHECK( m_bcmContainerName.initialize() );
    ATH_CHECK( m_bcmCollisionTimeName.initialize() );
    return StatusCode::SUCCESS;
  }

//__________________________________________________________________________
StatusCode BcmCollisionTimeAlg::finalize()
  {
    return StatusCode::SUCCESS;
  }

//__________________________________________________________________________
StatusCode BcmCollisionTimeAlg::execute()
  {
    //.............................................

    ATH_MSG_DEBUG( "BcmCollisionTimeAlg execute()" );

   //declare variables here
   //unsigned int bcmLGHit=0;
   //unsigned int bcmHGHit=0;
   std::vector<float> deltaT;
   unsigned int multiLG = 0;
   unsigned int multiHG = 0;
   //bool IsCol = false;
   //bool IsBkg = false;
   std::vector<deltat_data> deltaTdataA_HG;
   std::vector<deltat_data> deltaTdataC_HG;

  SG::ReadHandle<BCM_RDO_Container> bcmRDO(m_bcmContainerName);
  if( !bcmRDO.isValid() ) {
    ATH_MSG_WARNING("Cannot find BCM RDO " << m_bcmContainerName.key() << " ! " );
    return StatusCode::SUCCESS;
  }
  else {
     int num_collect = bcmRDO->size();
     if ( num_collect != 16 ){
       ATH_MSG_WARNING( " Number of collections: " << num_collect );
     }
     int channelID=0;

     for (const BCM_RDO_Collection *chan: *bcmRDO) {
       channelID = chan->getChannel();
       //std::cout << " ChannelID: " << channelID << std::endl;

       // Loop over all BCM hits in this collection
       for (const BCM_RawData *bcm: *chan) {
         if (bcm->getPulse1Width() != 0 && bcm->getLVL1A()==18) {
           if (channelID <8) multiLG++;
           else multiHG++;
	   deltat_data hit(channelID,bcm->getLVL1A(),bcm->getPulse1Position());
	   //std::cout << "channel,LVL1A,pos" << channelID << " " << bcm_->getLVL1A() << " " << bcm->getPulse1Position() << std::endl;
	   if (channelID >7 && channelID < 12) deltaTdataA_HG.push_back(hit);
	   if (channelID >11 ) deltaTdataC_HG.push_back(hit);


	   if (bcm->getPulse2Width() != 0) {
             if (channelID <8) multiLG++;
             else multiHG++;
	     deltat_data hit2(channelID,bcm->getLVL1A(),bcm->getPulse2Position());
	     //std::cout << "channel,LVL1A,pos" << channelID << " " << bcm->getLVL1A() << " " << bcm->getPulse2Position() << std::endl;
	     if (channelID >7 && channelID < 12) deltaTdataA_HG.push_back(hit2);
	     if (channelID >11 && bcm->getPulse2Width() != 0 ) deltaTdataC_HG.push_back(hit2);
	   }


         }
       } // end of loop over raw data
     } // end of loop over collections

     // calculate deltaTs from deltatdata now
     for(unsigned int i=0; i < deltaTdataA_HG.size(); i++){
       for(unsigned int j=0; j < deltaTdataC_HG.size(); j++){
         if((deltaTdataA_HG.at(i)).m_bcid == (deltaTdataC_HG.at(j)).m_bcid){
	   //std::cout << "time diff in bins: " << ((int)(deltaTdataA_HG.at(i)).m_position - (int)(deltaTdataC_HG.at(j)).m_position) << std::endl;
	   float deltaTtime = ((float)(deltaTdataA_HG.at(i)).m_position - (float)(deltaTdataC_HG.at(j)).m_position)/64*25;
           //std::cout << "delta t: " << deltaTtime << std::endl;
	   deltaT.push_back(deltaTtime);
	 }
       }
     }

     // now set bits
     //std::cout << "set bits: " << std::endl;
     //for (unsigned int i=0;  i< deltaT.size(); i++){
     //  if( fabs(deltaT.at(i)) < m_timeCut ) IsCol = true;
     //  else IsBkg = true;
     //}

   }
 

  //std::cout << " multiLG, multiHG, IsCol, IsBkg  " << multiLG << " " << multiHG << " " << IsCol << " " << IsBkg << std::endl;
  //std::cout << " multiLG, multiHG " << multiLG << " " << multiHG << std::endl;
  //fill object here
  //BcmBkgWord * bbw = new BcmBkgWord(multiLG,multiHG,IsCol,IsBkg,deltaT);
  SG::WriteHandle<BcmCollisionTime> bbw(m_bcmCollisionTimeName);
  if (bbw.record( std::make_unique<BcmCollisionTime>(multiLG,multiHG,deltaT) ).isFailure()) {
    ATH_MSG_WARNING( " Cannot record BcmCollisionTime " );
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
