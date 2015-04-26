/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////// -*- C++ -*- /////////////////////////////
/**  RawInfoSummaryForTagCnv_p1.cxx
 *  @author: Marcin.Nowak@cern.ch
 *  @date March 2010
 *  @brief TP converter for RawInfoSummaryForTag_p1
****************************************************************/


#include "EventTagTPCnv/RawInfoSummaryForTagCnv_p1.h"

void
RawInfoSummaryForTagCnv_p1::persToTrans(const Pers_t* pers, Trans_t* trans, MsgStream& )
{
  /* floats */
  trans->setTrackLead1_d0    (pers->TrackLead1_d0    );        
  trans->setTrackLead1_z0    (pers->TrackLead1_z0    );      
  trans->setTrackLead1_phi0  (pers->TrackLead1_phi0  );      
  trans->setTrackLead1_theta (pers->TrackLead1_theta );
  trans->setTrackLead1_qoverp(pers->TrackLead1_qoverp);
  trans->setTrackLead2_d0    (pers->TrackLead2_d0    );      
  trans->setTrackLead2_z0    (pers->TrackLead2_z0    );      
  trans->setTrackLead2_phi0  (pers->TrackLead2_phi0  );      
  trans->setTrackLead2_theta (pers->TrackLead2_theta );      
  trans->setTrackLead2_qoverp(pers->TrackLead2_qoverp);
  trans->setCellEnergySum    (pers->CellEnergySum    );      
  trans->setCellEnergySumEMB (pers->CellEnergySumEMB );      
  trans->setCellEnergySumEMEC(pers->CellEnergySumEMEC);
  trans->setCellEnergySumHEC (pers->CellEnergySumHEC );      
  trans->setCellEnergySumFCAL(pers->CellEnergySumFCAL);
  trans->setCellEnergySumTile(pers->CellEnergySumTile);
  trans->setClusterEnergySum (pers->ClusterEnergySum );
  trans->setTopoClEt1        (pers->TopoClEt1        );         	 	 
  trans->setTopoClEta1       (pers->TopoClEta1       );        	 	 
  trans->setTopoClPhi1       (pers->TopoClPhi1       );     
  trans->setCellMET          (pers->CellMET          );      
  trans->setCellMETPhi       (pers->CellMETPhi       );      
  trans->setMBTStimeDiff     (pers->MBTStimeDiff     );      
  trans->setLArECtimeDiff    (pers->LArECtimeDiff    );
  trans->setMBTStimeAvg     (pers->MBTStimeAvg     );
  trans->setLArECtimeAvg    (pers->LArECtimeAvg    );
  trans->setTrtEventPhase    (pers->TrtEventPhase    );

  
  /* ints */
  trans->setPixelTracks                     (pers->PixelTracks                           );        
  trans->setSCTTracks                       (pers->SCTTracks                              ); 
  trans->setTRTTracks                       (pers->TRTTracks                              ); 
  trans->setMooreSegments                       (pers->MooreSegments                          ); 
  trans->setConvertedMBoySegments               (pers->ConvertedMBoySegments                  ); 
  trans->setMooreTracks                       (pers->MooreTracks                          ); 
  trans->setConvertedMBoyTracks               (pers->ConvertedMBoyTracks                  ); 
  trans->setNumberOfInnerConvertedMBoySegments  (pers->NumberOfInnerConvertedMBoySegments     ); 
  trans->setHitsOfBestInnerConvertedMBoySegments(pers->HitsOfBestInnerConvertedMBoySegments   ); 
  trans->setNumberOfInnerMooreSegments          (pers->NumberOfInnerMooreSegments             ); 
  trans->setHitsOfBestInnerMooreSegments        (pers->HitsOfBestInnerMooreSegments           ); 
  trans->setNsctSPs                         (pers->NsctSPs                                ); 
  trans->setNpixSPs                         (pers->NpixSPs                                ); 
  trans->setNtrtDCs                         (pers->NtrtDCs                                ); 
  trans->setNtrtHtDCs                       (pers->NtrtHtDCs                              ); 
  trans->setnMDTHits                        (pers->nMDTHits                               ); 
  trans->setnRPCHits                        (pers->nRPCHits                               ); 
  trans->setnTGCHits                        (pers->nTGCHits                               ); 
  trans->setnCSCHits                        (pers->nCSCHits                               ); 
  trans->setBCMHit                          (pers->BCMHit                                 ); 
  trans->setMBTSword                        (pers->MBTSword                              );        
   
}


void
RawInfoSummaryForTagCnv_p1::transToPers(const Trans_t* trans, Pers_t* pers, MsgStream& )
{
 /* floats */
   pers->TrackLead1_d0 		=  trans->getTrackLead1_d0();
   pers->TrackLead1_z0	 	=  trans->getTrackLead1_z0();
   pers->TrackLead1_phi0 	=  trans->getTrackLead1_phi0();
   pers->TrackLead1_theta  	=  trans->getTrackLead1_theta (); 
   pers->TrackLead1_qoverp 	=  trans->getTrackLead1_qoverp(); 
   pers->TrackLead2_d0		=  trans->getTrackLead2_d0    (); 
   pers->TrackLead2_z0		=  trans->getTrackLead2_z0    (); 
   pers->TrackLead2_phi0	=  trans->getTrackLead2_phi0  (); 
   pers->TrackLead2_theta	=  trans->getTrackLead2_theta (); 
   pers->TrackLead2_qoverp	=  trans->getTrackLead2_qoverp(); 
   pers->CellEnergySum		=  trans->getCellEnergySum    (); 
   pers->CellEnergySumEMB	=  trans->getCellEnergySumEMB (); 
   pers->CellEnergySumEMEC	=  trans->getCellEnergySumEMEC(); 
   pers->CellEnergySumHEC	=  trans->getCellEnergySumHEC (); 
   pers->CellEnergySumFCAL	=  trans->getCellEnergySumFCAL(); 
   pers->CellEnergySumTile	=  trans->getCellEnergySumTile(); 
   pers->ClusterEnergySum	=  trans->getClusterEnergySum (); 
   pers->TopoClEt1              =  trans->getTopoClEt1        ();    	 	 
   pers->TopoClEta1             =  trans->getTopoClEta1       ();   	 	 
   pers->TopoClPhi1             =  trans->getTopoClPhi1       ();
   pers->CellMET		=  trans->getCellMET          (); 
   pers->CellMETPhi		=  trans->getCellMETPhi       (); 
   pers->MBTStimeDiff		=  trans->getMBTStimeDiff     (); 
   pers->LArECtimeDiff     	=  trans->getLArECtimeDiff    (); 
   pers->MBTStimeAvg		=  trans->getMBTStimeAvg     ();
   pers->LArECtimeAvg     	=  trans->getLArECtimeAvg    ();
   pers->TrtEventPhase    	=  trans->getTrtEventPhase    (); 
	
  /* ints */
   pers->PixelTracks                        = trans->getPixelTracks                          ();        
   pers->SCTTracks			    = trans->getSCTTracks                            (); 
   pers->TRTTracks			    = trans->getTRTTracks                            (); 
   pers->MooreSegments			    = trans->getMooreSegments                        (); 
   pers->ConvertedMBoySegments 		    = trans->getConvertedMBoySegments                (); 
   pers->MooreTracks			    = trans->getMooreTracks                        (); 
   pers->ConvertedMBoyTracks 		    = trans->getConvertedMBoyTracks                (); 
   pers->NumberOfInnerConvertedMBoySegments  	= trans->getNumberOfInnerConvertedMBoySegments   (); 
   pers->HitsOfBestInnerConvertedMBoySegments	= trans->getHitsOfBestInnerConvertedMBoySegments (); 
   pers->NumberOfInnerMooreSegments	    	= trans->getNumberOfInnerMooreSegments           (); 
   pers->HitsOfBestInnerMooreSegments 	    = trans->getHitsOfBestInnerMooreSegments         (); 
   pers->NsctSPs				    = trans->getNsctSPs                              (); 
   pers->NpixSPs				    = trans->getNpixSPs                              (); 
   pers->NtrtDCs				    = trans->getNtrtDCs                              (); 
   pers->NtrtHtDCs				    = trans->getNtrtHtDCs                            (); 
   pers->nMDTHits				    = trans->getnMDTHits                             (); 
   pers->nRPCHits				    = trans->getnRPCHits                             (); 
   pers->nTGCHits				    = trans->getnTGCHits                             (); 
   pers->nCSCHits				    = trans->getnCSCHits                             (); 
   pers->BCMHit				    = trans->getBCMHit                               (); 
   pers->MBTSword                              = trans->getMBTSword     ();            
   
}
