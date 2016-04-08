/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "BCM_BackgroundAlgs/BcmCollisionTimeAlg.h"
#include "BCM_CollisionTime/BcmCollisionTime.h"

#include "Identifier/Identifier.h"

#include "InDetBCM_RawData/BCM_RDO_Container.h"

#include "AthenaKernel/errorcheck.h"

//Constructor
BcmCollisionTimeAlg:: BcmCollisionTimeAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator)
  {
    //declareProperty("cellContainerName",m_cellsContName);
    //m_nevt=0;
    m_timeCut = 6.25;
  }
  
//__________________________________________________________________________
//Destructor
  BcmCollisionTimeAlg::~BcmCollisionTimeAlg()
  {
    ATH_MSG_DEBUG("BcmCollisionTimeAlg destructor called" );
  }

//__________________________________________________________________________
StatusCode BcmCollisionTimeAlg::initialize()
  {
    
    ATH_MSG_INFO( "BcmCollisionTimeAlg initialize()" );

    CHECK( evtStore().retrieve() );

    return StatusCode::SUCCESS; 

  }

//__________________________________________________________________________
StatusCode BcmCollisionTimeAlg::finalize()
  {
    MsgStream log( messageService(), name() );
    log << MSG::DEBUG <<"BcmCollisionTimeAlg finalize()" << endreq;
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

   const BCM_RDO_Container *m_bcmRDO=0;
   if (StatusCode::SUCCESS!=evtStore()->retrieve(m_bcmRDO,"BCM_RDOs")) {
     ATH_MSG_WARNING("Cannot find BCM RDO! " );
   }   else {
     int num_collect = m_bcmRDO->size();
     if ( num_collect != 16 ){
       ATH_MSG_WARNING( " Number of collections: " << num_collect );
     }
   
     BCM_RDO_Container::const_iterator chan_itr = m_bcmRDO->begin();
     BCM_RDO_Container::const_iterator chan_itr_end = m_bcmRDO->end();
     
     int channelID=0;
     
     for (; chan_itr != chan_itr_end; chan_itr++) {
       channelID = (*chan_itr)->getChannel();
       //std::cout << " ChannelID: " << channelID << std::endl;

       
       // Loop over all BCM hits in this collection
       BCM_RDO_Collection::const_iterator bcm_itr = (*chan_itr)->begin();
       BCM_RDO_Collection::const_iterator bcm_itr_end = (*chan_itr)->end();
       
       for (; bcm_itr != bcm_itr_end; bcm_itr++) {
         if ((*bcm_itr)->getPulse1Width() != 0 && (*bcm_itr)->getLVL1A()==18) {
           //std::cout << "Found hit at LVL1A18! " << std::endl;
           ///                       PLEASE TAG THE EVENT!!!
           if (channelID <8) multiLG++;
           else multiHG++;
	   deltat_data hit(channelID,(*bcm_itr)->getLVL1A(),(*bcm_itr)->getPulse1Position());
	   //std::cout << "channel,LVL1A,pos" << channelID << " " << (*bcm_itr)->getLVL1A() << " " << (*bcm_itr)->getPulse1Position() << std::endl;
	   if (channelID >7 && channelID < 12) deltaTdataA_HG.push_back(hit);
	   if (channelID >11 ) deltaTdataC_HG.push_back(hit);


	   if ((*bcm_itr)->getPulse2Width() != 0) {
             //std::cout << "Found hit2! " << std::endl;
             if (channelID <8) multiLG++;
             else multiHG++;
	     deltat_data hit2(channelID,(*bcm_itr)->getLVL1A(),(*bcm_itr)->getPulse2Position());
	     //std::cout << "channel,LVL1A,pos" << channelID << " " << (*bcm_itr)->getLVL1A() << " " << (*bcm_itr)->getPulse2Position() << std::endl;
	     if (channelID >7 && channelID < 12) deltaTdataA_HG.push_back(hit2);
	     if (channelID >11 && (*bcm_itr)->getPulse2Width() != 0 ) deltaTdataC_HG.push_back(hit2);
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
   std::unique_ptr<BcmCollisionTime> bbw( new BcmCollisionTime(multiLG,multiHG,deltaT) );
  if (evtStore()->record(bbw.get(),"BcmCollisionTime").isFailure()) {
    ATH_MSG_WARNING( " Cannot record BcmCollisionTime " );
  }
  else {
    bbw.release();
  }

  return StatusCode::SUCCESS;
}
