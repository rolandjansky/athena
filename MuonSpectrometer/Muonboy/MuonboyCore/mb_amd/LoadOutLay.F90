!
!> \author SamuSog
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids jtyp/jff/jzz/jobj of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLay( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    OffNber = 0
    CALL Free_MB_OffStations
    ALLOCATE( OffTyp (OffNberTot) )
    ALLOCATE( OffJff (OffNberTot) )
    ALLOCATE( OffJzz (OffNberTot) )
    ALLOCATE( OffJob (OffNberTot) )
!
 END SUBROUTINE ResetLoadOutLay
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids jtyp/jff/jzz/jobj of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLay( Jtyp, Jff, Jzz, Jobj )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj !< ids of a dead unit
!
    OffNber = OffNber + 1
    OffTyp (OffNber) = Jtyp
    OffJff (OffNber) = Jff
    OffJzz (OffNber) = Jzz
    OffJob (OffNber) = Jobj
!      
 END SUBROUTINE LoadOutLay
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids jtyp/jff/jzz/jobj/Jsl of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLayCSCWireLayer( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    CSCWireLayerOffNber = 0
    CALL Free_MB_CSCWireLayerOffStations
    ALLOCATE( CSCWireLayerOffTyp (OffNberTot) )
    ALLOCATE( CSCWireLayerOffJff (OffNberTot) )
    ALLOCATE( CSCWireLayerOffJzz (OffNberTot) )
    ALLOCATE( CSCWireLayerOffJob (OffNberTot) )
    ALLOCATE( CSCWireLayerOffJsl (OffNberTot) )
!
 END SUBROUTINE ResetLoadOutLayCSCWireLayer
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids jtyp/jff/jzz/jobj/Jsl of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLayCSCWireLayer( Jtyp, Jff, Jzz, Jobj, Jsl )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj, Jsl !< ids of a dead unit
!
    CSCWireLayerOffNber = CSCWireLayerOffNber + 1
    CSCWireLayerOffTyp (CSCWireLayerOffNber) = Jtyp
    CSCWireLayerOffJff (CSCWireLayerOffNber) = Jff
    CSCWireLayerOffJzz (CSCWireLayerOffNber) = Jzz
    CSCWireLayerOffJob (CSCWireLayerOffNber) = Jobj
    CSCWireLayerOffJsl (CSCWireLayerOffNber) = Jsl
!      
 END SUBROUTINE LoadOutLayCSCWireLayer
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids Jtyp/Jff/Jzz/Jobj/Jsl/Jtube of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLayMDTOff( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    MDTOffNber = 0
    CALL Free_MB_MDTOffStations
    ALLOCATE( MDTOffTyp   (OffNberTot) )
    ALLOCATE( MDTOffJff   (OffNberTot) )
    ALLOCATE( MDTOffJzz   (OffNberTot) )
    ALLOCATE( MDTOffJob   (OffNberTot) )
    ALLOCATE( MDTOffJsl   (OffNberTot) )
    ALLOCATE( MDTOffJtube (OffNberTot) )
!
 END SUBROUTINE ResetLoadOutLayMDTOff
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids Jtyp/Jff/Jzz/Jobj/Jsl/Jtube of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLayMDTOff( Jtyp, Jff, Jzz, Jobj, Jsl , Jtube )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj, Jsl, Jtube !< ids of a dead unit
!
    MDTOffNber = MDTOffNber + 1
    MDTOffTyp   (MDTOffNber) = Jtyp
    MDTOffJff   (MDTOffNber) = Jff
    MDTOffJzz   (MDTOffNber) = Jzz
    MDTOffJob   (MDTOffNber) = Jobj
    MDTOffJsl   (MDTOffNber) = Jsl
    MDTOffJtube (MDTOffNber) = Jtube
!      
 END SUBROUTINE LoadOutLayMDTOff
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids Jtyp/Jff/Jzz/Jobj/Jspli/Jsl/Jsz/Jstri of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLayRPCOff( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    RPCOffNber = 0
    CALL Free_MB_RPCOffStations
    ALLOCATE( RPCOffTyp   (OffNberTot) )
    ALLOCATE( RPCOffJff   (OffNberTot) )
    ALLOCATE( RPCOffJzz   (OffNberTot) )
    ALLOCATE( RPCOffJob   (OffNberTot) )
    ALLOCATE( RPCOffJspli (OffNberTot) )
    ALLOCATE( RPCOffJsl   (OffNberTot) )
    ALLOCATE( RPCOffJsz   (OffNberTot) )
    ALLOCATE( RPCOffJstri (OffNberTot) )
!
 END SUBROUTINE ResetLoadOutLayRPCOff
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids Jtyp/Jff/Jzz/Jobj/Jspli/Jsl/Jsz/Jstri of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLayRPCOff( Jtyp, Jff, Jzz, Jobj, Jspli , Jsl , Jsz , Jstri )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj, Jspli , Jsl , Jsz , Jstri !< ids of a dead unit
!
    RPCOffNber = RPCOffNber + 1
    RPCOffTyp   (RPCOffNber) = Jtyp
    RPCOffJff   (RPCOffNber) = Jff
    RPCOffJzz   (RPCOffNber) = Jzz
    RPCOffJob   (RPCOffNber) = Jobj
    RPCOffJspli (RPCOffNber) = Jspli
    RPCOffJsl   (RPCOffNber) = Jsl
    RPCOffJsz   (RPCOffNber) = Jsz
    RPCOffJstri (RPCOffNber) = Jstri
!      
 END SUBROUTINE LoadOutLayRPCOff
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids Jtyp/Jff/Jzz/Jobj/Jsl/Jtube/SZflag of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLayCSCOff( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    CSCOffNber = 0
    CALL Free_MB_CSCOffStations
    ALLOCATE( CSCOffTyp    (OffNberTot) )
    ALLOCATE( CSCOffJff    (OffNberTot) )
    ALLOCATE( CSCOffJzz    (OffNberTot) )
    ALLOCATE( CSCOffJob    (OffNberTot) )
    ALLOCATE( CSCOffJsl    (OffNberTot) )
    ALLOCATE( CSCOffJtube  (OffNberTot) )
    ALLOCATE( CSCOffSZflag (OffNberTot) )

 END SUBROUTINE ResetLoadOutLayCSCOff
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids Jtyp/Jff/Jzz/Jobj/Jsl/Jtube/SZflag of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLayCSCOff( Jtyp, Jff, Jzz, Jobj, Jsl , Jtube , SZflag )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj, Jsl, Jtube !< ids of a dead unit
 real(8), INTENT(IN) :: SZflag
!
    CSCOffNber = CSCOffNber + 1
    CSCOffTyp    (CSCOffNber) = Jtyp
    CSCOffJff    (CSCOffNber) = Jff
    CSCOffJzz    (CSCOffNber) = Jzz
    CSCOffJob    (CSCOffNber) = Jobj
    CSCOffJsl    (CSCOffNber) = Jsl
    CSCOffJtube  (CSCOffNber) = Jtube
    CSCOffSZflag (CSCOffNber) = SZflag
!      
 END SUBROUTINE LoadOutLayCSCOff
!
!--------------------------------------------------------------------
!> This routine desalllocates and re-alllocates memory for the arrays of modules M_MB_MuGeom containing
!> the ids Jtyp/Jff/Jzz/Jobj/Jspli/Jsl/Jsz/Jstri of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE ResetLoadOutLayTGCOff( OffNberTot )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: OffNberTot !< Nber of dead units to be stored
!
    TGCOffNber = 0
    CALL Free_MB_TGCOffStations
    ALLOCATE( TGCOffTyp   (OffNberTot) )
    ALLOCATE( TGCOffJff   (OffNberTot) )
    ALLOCATE( TGCOffJzz   (OffNberTot) )
    ALLOCATE( TGCOffJob   (OffNberTot) )
    ALLOCATE( TGCOffJspli (OffNberTot) )
    ALLOCATE( TGCOffJsl   (OffNberTot) )
    ALLOCATE( TGCOffJsz   (OffNberTot) )
    ALLOCATE( TGCOffJstri (OffNberTot) )
!
 END SUBROUTINE ResetLoadOutLayTGCOff
!
!--------------------------------------------------------------------
!> This routine stores 
!> the ids Jtyp/Jff/Jzz/Jobj/Jspli/Jsl/Jsz/Jstri of a dead unit
!--------------------------------------------------------------------
 SUBROUTINE LoadOutLayTGCOff( Jtyp, Jff, Jzz, Jobj, Jspli , Jsl , Jsz , Jstri )
!
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Jtyp, Jff, Jzz, Jobj, Jspli , Jsl , Jsz , Jstri !< ids of a dead unit
!
    TGCOffNber = TGCOffNber + 1
    TGCOffTyp   (TGCOffNber) = Jtyp
    TGCOffJff   (TGCOffNber) = Jff
    TGCOffJzz   (TGCOffNber) = Jzz
    TGCOffJob   (TGCOffNber) = Jobj
    TGCOffJspli (TGCOffNber) = Jspli
    TGCOffJsl   (TGCOffNber) = Jsl
    TGCOffJsz   (TGCOffNber) = Jsz
    TGCOffJstri (TGCOffNber) = Jstri
!      
 END SUBROUTINE LoadOutLayTGCOff
!
