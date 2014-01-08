!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE SHIFT_VOL(Vdir,IVDEB,IVFIN,IGS,CENIN,CENOUT,EDGIN,EDGOUT)
 USE M_MB_General_Volumes
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: IVDEB,IVFIN,IGS
 REAL(8), INTENT(IN)  :: Vdir(*),CENIN, EDGIN
 REAL(8), INTENT(OUT) ::         CENOUT,EDGOUT
 INTEGER              :: NADMI,NADMA,IV,IP,IE,IA
 INTEGER, ALLOCATABLE :: JAA(:)
 REAL(8)              :: AXMIN,AXMAX,CXYZ,VdirLoc(3)
!
   CENOUT = CENIN
   EDGOUT = EDGIN
   IF( IVFIN < IVDEB ) RETURN
   IF( IVDEB < 1     ) RETURN
   IF( IVFIN > Nvolu ) RETURN
!
   NADMI = Nadge
   NADMA =     1
   AXMIN =  999999999.D0
   AXMAX = -999999999.D0
   SELECT CASE( I_Atlas_Ref_Syst )
   CASE( 1 )
      VdirLoc( 1 ) =   Vdir( 1 )
      VdirLoc( 2 ) = - Vdir( 3 )
      VdirLoc( 3 ) =   Vdir( 2 )
   CASE DEFAULT
      VdirLoc(1:3) =   Vdir(1:3)
   END SELECT
!
   DO IV=IVDEB,IVFIN
    DO IP=NplanI(1,IV),NplanA(M4,IV)
     DO IE=NedgeI(IP),NedgeA(IP)
      IA = IadEdge(IE)
      NADMI = MIN( NADMI , IA )
      NADMA = MAX( NADMA , IA )
      CXYZ = VdirLoc(1)*XYZadge(1,IA) + VdirLoc(2)*XYZadge(2,IA) + VdirLoc(3)*XYZadge(3,IA)
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
   ALLOCATE( JAA(NADMI:NADMA) )
   JAA(NADMI:NADMA) = 0
!
   DO IV=IVDEB,IVFIN
    CenVolu(1:3,IV) = CenVolu(1:3,IV) + CENOUT*VdirLoc(1:3)
    DO IP=NplanI(1,IV),NplanA(M4,IV)
     DO IE=NedgeI(IP),NedgeA(IP)
      IA = IadEdge(IE)
      IF( JAA(IA) > 0 ) CYCLE
         JAA(IA) = 1
         XYZadge(1:3,IA) = XYZadge(1:3,IA) + CENOUT*VdirLoc(1:3)
     ENDDO
    ENDDO
   ENDDO
!
   DEALLOCATE( JAA )
!
 END
