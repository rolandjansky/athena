/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"

//Needed for abs option
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

#include "xAODCaloEvent/CaloClusterKineHelper.h"

/// Standard AlgTool constructor
using xAOD::CaloCluster;
CaloClusterLocalCalib::CaloClusterLocalCalib(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  :CaloClusterProcessor(type, name, parent),
   m_classificationTool(this),
   m_calibTools(this),
   m_absOpt(false)
{
  m_recoStatus.resize(0);
  declareProperty ("ClusterClassificationTool",m_classificationTool);
  declareProperty ("LocalCalibTools",m_calibTools) ;
  declareProperty ("ClusterRecoStatus",m_recoStatus) ;
  //Use weighting of negative clusters?
  declareProperty ("WeightingOfNegClusters",m_absOpt) ;  
}


StatusCode  CaloClusterLocalCalib::initialize() {

  if (m_classificationTool.size()>1) {
    msg(MSG::ERROR) << "More than one classification tool specified!" << endmsg;
    return StatusCode::FAILURE;
  }
  //Retrieve classification tool (if necessary)
  if (m_classificationTool.size()>0) {
    ATH_CHECK(m_classificationTool.retrieve());
    msg(MSG::INFO) << "Found classification tool " << m_classificationTool[0] << endmsg;
  }

  //Retrieve calibration tools
  ATH_CHECK(m_calibTools.retrieve());
  msg(MSG::INFO) <<"Loaded "<< m_calibTools.size() <<" hadronic calibration tools"<<endmsg;
  
  // check that at least one reco status is defined
  if ( m_recoStatus.size() == 0 ) {
    msg(MSG::ERROR) << "Please specify at least one valid reco status with ClusterRecoStatus = [...] for this calib tool " << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode  CaloClusterLocalCalib::execute(const EventContext& /*ctx*/,
                                           CaloCluster* theCluster) const
{
  CaloRecoStatus& recoStatus=theCluster->recoStatus();
  // call classification tool
  if (!m_classificationTool.empty()) {
    recoStatus=m_classificationTool[0]->classify(theCluster);
  }
  // check desired reco status
  bool isSelected (false);
  for (unsigned int i=0;!isSelected && i<m_recoStatus.size();i++ )
    isSelected = recoStatus.checkStatus(CaloRecoStatus::StatusIndicator(m_recoStatus[i]));
  if ( isSelected ) {    
    if( m_absOpt ){  
        

        //double dm_weight(0.);          
        //double had_weight(0.);
        double centLambd(0.);
        double engdens(0.);
        double emprobability(0.);
        double isolation(0.);
        
// Store moments and energy of the old cluster
        //theCluster->retrieveMoment(xAOD::CaloCluster::DM_WEIGHT,  dm_weight); 
        //theCluster->retrieveMoment(xAOD::CaloCluster::HAD_WEIGHT,had_weight); 
        if (!theCluster->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,centLambd))
          centLambd = 0;
        if (!theCluster->retrieveMoment(xAOD::CaloCluster::FIRST_ENG_DENS,engdens))
          engdens = 0;
        if (!theCluster->retrieveMoment(xAOD::CaloCluster::EM_PROBABILITY,emprobability))
          emprobability = 0;
        if (!theCluster->retrieveMoment(xAOD::CaloCluster::ISOLATION,isolation))
          isolation = 0;
        
        double oldEnergy = theCluster->e();           
        
        const CaloDetDescrManager*   calo_dd_man  = CaloDetDescrManager::instance(); 
        const CaloCell_ID*               calo_id  = calo_dd_man->getCaloCell_ID();   
              
//         Make new Cluster and CellColl
        CaloCellContainer* myCellColl = new CaloCellContainer(SG::OWN_ELEMENTS);  
        xAOD::CaloCluster* myCluster  = CaloClusterStoreHelper::makeCluster(myCellColl);
        int cellIndex = 0; 
        CaloCell* CellReplica = 0;     
//         Iterate over cells of old cluster and replicate them with energy weighted by -1 if negative and add it to the new cluster
        xAOD::CaloCluster::cell_iterator cellIter = theCluster->cell_begin();  
        for(;cellIter!=theCluster->cell_end();cellIter++) {
          const  CaloCell* pCell = *cellIter;  
          double CellEnergy = pCell->e();    
          //Identifier myId = pCell->ID();    
          //IdentifierHash myHashId = calo_id->calo_cell_hash(myId); 
          if( CellEnergy < 0. )   CellEnergy = -1 * pCell->e();    
          CellReplica = new CaloCell( pCell->caloDDE(), pCell->ID(), CellEnergy, pCell->time(), pCell->quality(),  pCell->provenance(), pCell->gain() );
          myCellColl->push_back(CellReplica);          
          double cellWeight = cellIter.weight();         
          myCluster->addCell(cellIndex,cellWeight);
          cellIndex++;
        }  
        
        CaloClusterKineHelper::calculateKine(myCluster,false,false);
        myCluster->setRawE(myCluster->e());
        
// Set sample energy of the new cluster to the one of the old cluster, other wise it is zero, important for dm correction
        for(unsigned int i=0; i != CaloSampling::Unknown; ++ i) {
          const xAOD::CaloCluster::CaloSample s=(xAOD::CaloCluster::CaloSample)(i);
          if (myCluster->hasSampling(s)) myCluster->setEnergy((CaloSampling::CaloSample)i, theCluster->eSample((CaloSampling::CaloSample)i)); 
        }//end loop over samplings  
        
        CaloClusterKineHelper::calculateKine(myCluster,true,false);
//           Give the new cluster the same moments as the old one
        myCluster->insertMoment(xAOD::CaloCluster::CENTER_LAMBDA,centLambd);
        myCluster->insertMoment(xAOD::CaloCluster::FIRST_ENG_DENS,engdens);
        myCluster->insertMoment(xAOD::CaloCluster::EM_PROBABILITY,emprobability);
        myCluster->insertMoment(xAOD::CaloCluster::ISOLATION,isolation);
        
//         Weight the new cluster
        for (const ToolHandle<IClusterCellWeightTool>& tool : m_calibTools) {
          if (tool->weight(myCluster).isFailure())
            msg(MSG::ERROR) << " failed to weight cluster " << endmsg;
        }

//       Iterate over new, calibrated cluster and write out the weights of every cell into a map
         std::map<IdentifierHash,double> weightMap;
         xAOD::CaloCluster::cell_iterator mycellIter    = myCluster->cell_begin();
         xAOD::CaloCluster::cell_iterator mycellIterEnd = myCluster->cell_end();      
         for(;mycellIter!=mycellIterEnd;mycellIter++) {
           const CaloCell* pCell = *mycellIter;
           double cellWeight = mycellIter.weight();  
           Identifier myId = pCell->ID();    
           IdentifierHash myHashId = calo_id->calo_cell_hash(myId);
           weightMap[myHashId] = cellWeight;           
         }

        
//          Apply cellweight on the corresponding cell of the  original cluster

         cellIter    = theCluster->cell_begin();
         for(;cellIter!=theCluster->cell_end();cellIter++) {
           const CaloCell* pCell = *cellIter;
           IdentifierHash myHashId = calo_id->calo_cell_hash(pCell->ID()); 
           double weight = weightMap[myHashId];
           theCluster->reweightCell(cellIter,weight);  
         }           

//          Update kinematics of new cluster
         CaloClusterKineHelper::calculateKine(theCluster,true,false); 
  

        //Manually insert weight moments to the original cluster, in the OOC case this is a multiplicative weight from LCOut and LCOutPi0
         double newWeightMoment = theCluster->e()/oldEnergy;  
         if(m_calibTools[0].typeAndName() == "CaloLCWeightTool/LCWeight")             theCluster->insertMoment(xAOD::CaloCluster::HAD_WEIGHT,newWeightMoment);         
         if(m_calibTools[0].typeAndName() == "CaloLCDeadMaterialTool/LCDeadMaterial") theCluster->insertMoment(xAOD::CaloCluster::DM_WEIGHT, newWeightMoment);          
         if(m_calibTools[0].typeAndName() == "CaloLCOutOfClusterTool/LCOut")          theCluster->insertMoment(xAOD::CaloCluster::OOC_WEIGHT,newWeightMoment);         
         if(m_calibTools[0].typeAndName() == "CaloLCOutOfClusterTool/LCOutPi0") 
         {
           double ooc_weight;
           theCluster->retrieveMoment(xAOD::CaloCluster::OOC_WEIGHT,ooc_weight);
           newWeightMoment *= ooc_weight;
           theCluster->insertMoment(xAOD::CaloCluster::OOC_WEIGHT,newWeightMoment);
         }         
                  
         delete myCellColl;          
         delete myCluster; 

      }
//    else, what as was always done     
      else{ 
        
        for (const ToolHandle<IClusterCellWeightTool>& tool : m_calibTools) {
          if (tool->weight(theCluster).isFailure())
            msg(MSG::ERROR) << " failed to weight cluster " << endmsg;
        }       
 
      }
    //report <<MSG::DEBUG<<"new cluster energy="<<theCluster->e()<<endmsg;
  }//end of "has correct reco status" if statement

  // PL add calibration method bits to reco status
  recoStatus.setStatus(CaloRecoStatus::CALIBRATEDLHC);

  return StatusCode::SUCCESS;
}



