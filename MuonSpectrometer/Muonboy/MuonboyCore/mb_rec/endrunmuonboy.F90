!
!***********************************************************************
 SUBROUTINE EndRunMuonboy
 USE M_MB_General_Volumes
 USE M_MB_MuGeom
 USE M_MB_Comati
 USE M_MB_Digis
 USE M_MB_Reco_Segments
 USE M_MB_Reco_Tracks
 USE M_MB_AlowCell
 USE M_MB_SSBAN
 USE M_MB_Xml
 USE M_MB_Xml_TREE
 IMPLICIT NONE

   call Free_MB_General_Volumes

   call Free_MB_Comati

   call Free_MB_OffStations
   call Free_MB_CSCWireLayerOffStations
   call Free_MB_MDTOffStations
   call Free_MB_RPCOffStations
   call Free_MB_CSCOffStations
   call Free_MB_TGCOffStations

   call Free_MB_Digis_ZTUBHI
   call Free_MB_Digis_SBANHI 
 
   call Free_MB_Reco_Segments
   
   call Free_MB_Reco_Tracks

   call Free_MB_AlowCell
 
   call Free_MB_SSBAN

   call Free_ALLOC_info
 
   call Free_Read_MB_Xml_Gen_Iele_xob

   call Free_Read_MB_Xml_Gen_Ipos_xml_var
 
   call Free_ALLOC_Tree

 END SUBROUTINE EndRunMuonboy
!
