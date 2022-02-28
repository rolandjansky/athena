/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRDO/RpcSectorLogic.h"
#include "MuonRDO/RpcSectorLogicContainer.h"
#include "MuonRDO/RpcSLTriggerHit.h"
#include "RpcSectorLogicContainerCnv_p1.h"


// Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"

// Athena
#include "StoreGate/StoreGateSvc.h"

#include "AthAllocators/DataPool.h"

#include "boost/range/iterator_range.hpp"
#include <assert.h>

void RpcSectorLogicContainerCnv_p1::transToPers(const RpcSectorLogicContainer* transCont,  RpcSectorLogicContainer_p1* persCont, MsgStream & log) 
{
  unsigned int tCsize=transCont->size();
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "RpcSectorLogicContainerCnv_p1::transToPers " << endmsg;
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << " RpcSectorLogicContainerCnv_p1 sizes: trans: " << tCsize << " " << endmsg;
  //std::cout<<"m_sectorList[0]: "<<transCont->m_sectorList[0]<<", m_sectorList[1]"<<transCont->m_sectorList[1]<<std::endl;
  persCont->m_sectorList[0] = transCont->sectorList()[0];
  persCont->m_sectorList[1] = transCont->sectorList()[1];

  RpcSectorLogicContainer::const_iterator it_Coll     = transCont->begin();
  RpcSectorLogicContainer::const_iterator it_CollEnd  = transCont->end();
  //std::cout<<"RpcSectorLogicContainer has "<<transCont->size()<<" SLs"<<std::endl;
  unsigned int slVariableIndex=0;
  unsigned int triggerRateIndex=0;

  if (transCont->size()==0) return; //All we can do in this case
  
  //std::cout<<"persCont->m_slVariables.resize(transCont->size() * 8) = "<<transCont->size() * 8<<std::endl;
  
  persCont->m_slVariables.resize(tCsize * 8);// We know that AT MINIMUM it will be this big.
  //std::cout<<"A"<<std::endl;
  persCont->m_slVariables[slVariableIndex++]=tCsize; // 1st element is number of SLs. Format is defined in RpcSectorLogicContainer_p1.h
  //std::cout<<"B"<<std::endl;
  
  persCont->m_hasMoreThan2TriggerCand.reserve(tCsize);
  //unsigned int indexSL=0; 
  for (; it_Coll != it_CollEnd; ++it_Coll)  {
    //std::cout<<"Processing SL :"<<indexSL++<<", slVariableIndex="<<slVariableIndex<<std::endl;

    // Resize now, to allow stl::copy to work safely.
    persCont->m_slVariables.resize(persCont->m_slVariables.size() + 8 + (**it_Coll).counters().size()); // 8 because it's 5 SL variables, plus 3 size counters
    
    // SectorLogic variables    
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).sectorId();
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).fel1Id();
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).bcid();
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).errorCode();
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).crc();
    //std::cout<<"sl variables="<<persCont->m_slVariables[slVariableIndex-5]<<", "<<persCont->m_slVariables[slVariableIndex-4]<<", "<<persCont->m_slVariables[slVariableIndex-3]<<", "<<persCont->m_slVariables[slVariableIndex-2]<<", "<<persCont->m_slVariables[slVariableIndex-1]<<std::endl;
    
    // m_counters (pers version already resized at start of loop)
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).counters().size(); // Size of m_counters
    //std::cout<<"m_counters.size()="<<(**it_Coll).counters().size()<<", slVariableIndex="<<slVariableIndex<<std::endl;
    
    std::copy ((**it_Coll).counters().begin(), (**it_Coll).counters().end(), persCont->m_slVariables.begin()+slVariableIndex);   
    slVariableIndex+=(**it_Coll).counters().size();
    
    // m_triggerRates
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).triggerRates().size(); // Size of m_triggerRates
//std::cout<<"m_triggerRates :"<<persCont->m_slVariables[slVariableIndex]<<", slVariableIndex="<<slVariableIndex<<std::endl;
    persCont->m_triggerRates.resize(persCont->m_triggerRates.size()+(**it_Coll).triggerRates().size()); 
    std::copy ((**it_Coll).triggerRates().begin(), (**it_Coll).triggerRates().end(), persCont->m_triggerRates.begin()+triggerRateIndex);                
    triggerRateIndex+=(**it_Coll).triggerRates().size();

    persCont->m_hasMoreThan2TriggerCand.push_back((**it_Coll).hasMoreThan2TriggerCand());
    
    // Now handle RpcSLTriggerHit info
    persCont->m_slVariables.resize(persCont->m_slVariables.size() + 1 + 9*(**it_Coll).size());
    persCont->m_slVariables[slVariableIndex++]=(**it_Coll).size(); // Size of RpcSLTriggerHits
    DataVector<RpcSLTriggerHit>::const_iterator itSLTH=(**it_Coll).begin();
    DataVector<RpcSLTriggerHit>::const_iterator itSLTHEnd=(**it_Coll).end();
    for (; itSLTH!=itSLTHEnd ; ++itSLTH){
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).rowinBcid();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).padId();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).ptId();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).roi();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).outerPlane();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).overlapPhi();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).overlapEta();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).triggerBcid();
      persCont->m_slVariables[slVariableIndex++]=(**itSLTH).isInput();
    }
  }
  if (log.level() <= MSG::DEBUG) log << MSG::DEBUG
      << "RpcSectorLogicContainerCnv_p1 summary: vector size "
      << persCont->m_slVariables.size() << ", last index used "
      << slVariableIndex
      << endmsg;
}

void  RpcSectorLogicContainerCnv_p1::persToTrans(const RpcSectorLogicContainer_p1* persCont, RpcSectorLogicContainer* transCont, MsgStream & /**log*/) 
{
   transCont->setSectorList (persCont->m_sectorList);
   //std::cout<<"**** RpcSectorLogicContainerCnv_p1::persToTrans"<<std::endl;
   //std::cout<<"m_sectorList[0]: "<<transCont->m_sectorList[0]<<", m_sectorList[1]"<<transCont->m_sectorList[1]<<std::endl;

   if (persCont->m_slVariables.size()==0) return; //empty container

   unsigned int index = 0; // for position within persCont->m_slVariables
   unsigned int numOfSL= persCont->m_slVariables[index++];// 1st element is number of SLs
//std::cout<<"persCont->m_slVariables.size(): "<<persCont->m_slVariables.size()<<", with numSL="<<numOfSL<<std::endl;
   transCont->resize(numOfSL);
   unsigned int indexSL = 0; // for number of SL & position within persCont->m_hasMoreThan2TriggerCand
   unsigned int indexTR = 0;// for m_triggerRates
   while (indexSL<numOfSL){
//std::cout<<"Processing SL :"<<indexSL<<", index"<<index<<std::endl;
//std::cout<<"sl variables="<<persCont->m_slVariables[index]<<", "<<persCont->m_slVariables[index+1]<<", "<<persCont->m_slVariables[index+2]<<", "<<persCont->m_slVariables[index+3]<<", "<<persCont->m_slVariables[index+4]<<std::endl;

     RpcSectorLogic* rsl = new RpcSectorLogic(persCont->m_slVariables[index], //m_sectorId
                        persCont->m_slVariables[index+1], //m_fel1Id
                        persCont->m_slVariables[index+2], //m_bcid
                        persCont->m_slVariables[index+3], //m_errorCode
                        persCont->m_slVariables[index+4]);//m_crc
      index+=5; // Fix for coverity defect 17799 - no multiple index++ in the same line.
      rsl->setHasMoreThan2TriggerCand (persCont->m_hasMoreThan2TriggerCand[indexSL]);
      uint16_t numCounters=persCont->m_slVariables[index++];
      for (uint16_t i : boost::make_iterator_range (persCont->m_slVariables.begin()+index,
                                                    persCont->m_slVariables.begin()+index + numCounters))
        rsl->addCounter(i);
      index+=numCounters;
      
      uint16_t numTriggerRates=persCont->m_slVariables[index++];
      //std::cout<<"numTriggerRates: "<<numTriggerRates<<", index"<<index<<", indexTR: "<<indexTR<<std::endl;
      
//std::cout<<"persCont->m_triggerRates.size(): "<<persCont->m_triggerRates.size()<<std::endl;
          
      for (double r : boost::make_iterator_range(persCont->m_triggerRates.begin()+indexTR,
                                                 persCont->m_triggerRates.begin()+indexTR+numTriggerRates))
        rsl->addTriggerRate (r);
      indexTR+=numTriggerRates;
      rsl->setHasMoreThan2TriggerCand (persCont->m_hasMoreThan2TriggerCand[indexSL]);
      uint16_t numRpcSLTriggerHits = persCont->m_slVariables[index++];   
      //std::cout<<"numRpcSLTriggerHits: "<<numRpcSLTriggerHits<<std::endl; 
      rsl->resize(numRpcSLTriggerHits);  
      for (unsigned int th=0; th<numRpcSLTriggerHits; ++th){
        //std::cout<<"begin th "<<th<<", index="<<index<<std::endl;
        (*rsl)[th]=new RpcSLTriggerHit(persCont->m_slVariables[index], //rowinBcid
                            persCont->m_slVariables[index+1], //padid
                            persCont->m_slVariables[index+2], //ptid
                            persCont->m_slVariables[index+3], //roi
                            persCont->m_slVariables[index+4],//outerPlane
                            persCont->m_slVariables[index+5],//overlapPhi
                            persCont->m_slVariables[index+6],//overlapEta
                            persCont->m_slVariables[index+7]);//triggerBcid                            
        index+=8;
        (*rsl)[th]->setIsInput (static_cast<bool>(persCont->m_slVariables[index++]));
        //std::cout<<"end th "<<th<<", index="<<index<<std::endl;
      }         
      (*transCont)[indexSL++]=rsl;
      //std::cout<<"End of loop on SL, index"<<index<<std::endl;
   }
}


