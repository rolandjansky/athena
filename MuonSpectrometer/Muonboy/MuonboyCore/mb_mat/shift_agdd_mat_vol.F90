!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE SHIFT_AGDD_MAT_VOL(Vdir,IVDEB,IVFIN,IGS,CENIN,CENOUT,EDGIN,EDGOUT)
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IVDEB,IVFIN,IGS
 REAL(8), INTENT(IN)  :: Vdir(*),CENIN, EDGIN
 REAL(8), INTENT(OUT) ::         CENOUT,EDGOUT
 INTEGER :: IV,IP,IE,IT0,ITshift,ITresult
 REAL(8) :: Vxyz(3),AXMIN,AXMAX,CXYZ
 REAL(8), DIMENSION(3,3), PARAMETER :: UNIT = RESHAPE( (/ 1.D0, 0.D0, 0.D0,    &
                                                          0.D0, 1.D0, 0.D0,    &
                                                          0.D0, 0.D0, 1.D0 /), (/3, 3/) )
!
   CENOUT = CENIN
   EDGOUT = EDGIN
   IF( IVFIN < IVDEB  ) RETURN
   IF( IVDEB < 1      ) RETURN
   IF( IVFIN > NVMATI ) RETURN
!
   AXMIN =  999999999.D0
   AXMAX = -999999999.D0
!
   DO IV=IVDEB,IVFIN
     DO IP=NPMAT0(IV),NPMAT1(IV)
       DO IE=NEMAT0(IP),NEMAT1(IP)
         CALL GETXYZMAT( IV, IE, Vxyz )
         CXYZ = DOT_PRODUCT( Vdir(1:3) , Vxyz(1:3) )
         AXMIN = MIN( AXMIN , CXYZ )
         AXMAX = MAX( AXMAX , CXYZ )
       ENDDO
     ENDDO
   ENDDO
!
   IF( IGS >= 1 ) THEN
     CENOUT = CENIN
   ELSE
     CENOUT = EDGIN  - AXMIN
   ENDIF
   EDGOUT    = CENOUT + AXMAX
!
   DO IV=IVDEB,IVFIN
     IT0 = MAX( ITRANSMATI(IV) , 1 )
     CALL INCR_NTMATI
     ITshift = NTMATI
     TRANSMATI(1:3,1:3,ITshift) = UNIT(1:3,1:3)
     TRANSMATI(1:3, 4 ,ITshift) = CENOUT*Vdir(1:3)
     CALL COMPOS_TRANSMATI( IT0, ITshift, ITresult )
     IF( ITresult >= ITshift ) THEN
       TRANSMATI(1:3,1:4,ITshift) = TRANSMATI(1:3,1:4,ITresult)
       ITresult = ITshift
       NTMATI   = ITshift
     ELSE
       NTMATI   = ITshift - 1
     ENDIF
     ITRANSMATI(IV) = ITresult
   ENDDO
!
   CALL CLEANTRANSMATI
!
 END
