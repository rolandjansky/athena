/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigParticle/TrigElectron.h"
#include "TrigParticleTPCnv/TrigElectron_p1.h"
#undef private
#undef protected


#include "TrigParticleTPCnv/TrigElectronCnv_p1.h"


void TrigElectronCnv_p1::persToTrans(const TrigElectron_p1* persObj, 
				     TrigElectron* transObj, 
				     MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p1::persToTrans called " << endreq;

//    transObj->m_trackAlgo = persObj->m_trackAlgo; 
//    transObj->m_roiID     = persObj->m_roiID    ; 
//    transObj->m_trackIndx = persObj->m_trackIndx;
//    transObj->m_charge    = persObj->m_charge   ;
//    transObj->m_Zvtx      = persObj->m_Zvtx     ; 
//    transObj->m_err_Zvtx  = persObj->m_err_Zvtx ;
//    transObj->m_err_eta   = persObj->m_err_eta  ;
//    transObj->m_err_phi   = persObj->m_err_phi  ;
//    transObj->m_err_Pt    = persObj->m_err_Pt   ; 
//    transObj->m_dEta      = persObj->m_dEta     ;  
//    transObj->m_dPhi      = persObj->m_dPhi     ;
//    transObj->m_etoverpt  = persObj->m_etoverpt ;
//    transObj->m_valid     = persObj->m_valid    ; 

//   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );


   transObj->m_roiWord        = persObj->m_roiID     ; // meaning changed! 
   transObj->m_valid          = persObj->m_valid     ; // meaning changed slightly (now also used for validating object construction)! 
   transObj->m_tr_Algo        = persObj->m_trackAlgo   ; 
   transObj->m_tr_Zvtx        = persObj->m_Zvtx   ; 
   transObj->m_tr_eta_at_calo = -999.99;               // un-recoverable
   transObj->m_tr_phi_at_calo = -999.99;               // un-recoverable
   transObj->m_etoverpt       = persObj->m_etoverpt  ;
   transObj->m_cl_eta         = -999.99;               // un-recoverable
   transObj->m_cl_phi         = -999.99;               // un-recoverable
   transObj->m_cl_Rcore       = -999.99;               // un-recoverable
   transObj->m_cl_Eratio      = -999.99;               // un-recoverable
   transObj->m_cl_EThad       = -999.99;               // un-recoverable
   
//    No way to re-create ElementLinks from TrigElectron_p1 
//    without using Navigation (which may not work for ARA)
//    transObj->m_cluster = ElementLink(...);
//    transObj->m_track   = ElementLink(...);

   fillTransFromPStore( &m_p4PtEtaPhiMCnv, persObj->m_p4PtEtaPhiM, transObj, log );

}


void TrigElectronCnv_p1::transToPers(const TrigElectron* /*transObj*/, 
                                     TrigElectron_p1* /*persObj*/, 
				       MsgStream &log)
{
   log << MSG::DEBUG << "TrigElectronCnv_p1::transToPers called " << endreq;
   log << MSG::WARNING << "Trying to write TrigElectron to obsolete persistent class TrigElectron_p1!" << endreq;

//    persObj->m_trackAlgo = transObj->m_trackAlgo; 
//    persObj->m_roiID     = transObj->m_roiID    ; 
//    persObj->m_trackIndx = transObj->m_trackIndx;
//    persObj->m_charge    = transObj->m_charge   ;
//    persObj->m_Zvtx      = transObj->m_Zvtx     ; 
//    persObj->m_err_Zvtx  = transObj->m_err_Zvtx ;
//    persObj->m_err_eta   = transObj->m_err_eta  ;
//    persObj->m_err_phi   = transObj->m_err_phi  ;
//    persObj->m_err_Pt    = transObj->m_err_Pt   ; 
//    persObj->m_dEta      = transObj->m_dEta     ;  
//    persObj->m_dPhi      = transObj->m_dPhi     ;
//    persObj->m_etoverpt  = transObj->m_etoverpt ;
//    persObj->m_valid     = transObj->m_valid    ; 

//    persObj->m_p4PtEtaPhiM = baseToPersistent( &m_p4PtEtaPhiMCnv, transObj, log );

}
