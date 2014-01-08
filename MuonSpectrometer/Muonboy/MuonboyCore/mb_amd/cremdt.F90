!
!> Create a new precision (MDT or CSC) station
!> \author M.Virchaux
!
 SUBROUTINE CREMDT(NTUTU,PTUTU,RTUTU,DTUTU,ETUTU,XTUTU,ATUTU,  &
                         UTUTU,OTUTU1,OTUTU2,TTUTU1,TTUTU2,JrtTUTU, NMDTTO, JMDT)
 USE M_MB_MuGeom
 USE M_MB_Reco_PARAMETERS
 IMPLICIT NONE
 INTEGER, INTENT(IN)    :: NTUTU, JrtTUTU
 REAL(8), INTENT(IN)    :: PTUTU, RTUTU, DTUTU, ETUTU, XTUTU, ATUTU, UTUTU
 REAL(8), INTENT(IN)    :: OTUTU1(N4M), OTUTU2(N4M), TTUTU1(N4M), TTUTU2(N4M)
 INTEGER, INTENT(INOUT) :: NMDTTO
 INTEGER, INTENT(OUT)   :: JMDT
 INTEGER :: L, K
!
   IF( NTUTU <= 0 ) THEN
     WRITE(*,'(/" In CREMDT : Atempt to create an MDT type with 0 layers ======> STOP !!!")')
     STOP
   ELSE
     DO 50 L=1,NMDTTO
       IF( NTUTU /= NLTUB(L) )                        CYCLE
       IF( ABS(PTUTU-PASTUB(L)) > 0.0010D0 )          CYCLE
       IF( ABS(RTUTU-RAYTUB(L)) > 0.0010D0 )          CYCLE
       IF( ABS(DTUTU-DEDTUB(L)) > 0.1000D0 )          CYCLE
       IF( ABS(ETUTU-RESTUB(L)) > 0.0010D0 )          CYCLE
       IF( ABS(XTUTU-RADTUB(L)) > 0.0010D0 )          CYCLE
       IF( ABS(ATUTU-DRITUB(L)) > 0.0010D0 )          CYCLE
       IF( ABS(UTUTU-ACUTUB(L)) > 0.0002D0 )          CYCLE
       DO K=1,NTUTU
         IF( ABS(OTUTU1(K)-OLTUB(L,K,1)) > 0.0010D0 ) GO TO 50
         IF( ABS(OTUTU2(K)-OLTUB(L,K,2)) > 0.0010D0 ) GO TO 50
         IF( ABS(TTUTU1(K)-TLTUB(L,K,1)) > 0.0010D0 ) GO TO 50
         IF( ABS(TTUTU2(K)-TLTUB(L,K,2)) > 0.0010D0 ) GO TO 50
       ENDDO
       IF( JrtTUTU /= JrtTUB(L) )                     CYCLE
       JMDT = L
       RETURN
50   CONTINUE
     NMDTTO = NMDTTO + 1
     IF( NMDTTO > NMDTMA ) THEN
       WRITE(*,'(/" In CREMDT : Total number of MDT types is too large ======> STOP !!!")')
       STOP
     ENDIF
     IF( NTUTU > N4M ) THEN
       WRITE(*,'(/" In CREMDT : Attempt to create an NLTUB(i) > N4M ======> STOP !!!")')
       STOP
     ENDIF
     NLTUB(NMDTTO)      = NTUTU
     PASTUB(NMDTTO)     = PTUTU
     RAYTUB(NMDTTO)     = RTUTU
     DEDTUB(NMDTTO)     = DTUTU
     REDTUB(NMDTTO)     = RTUTU * DTUTU/ATUTU
     RESTUB(NMDTTO)     = ETUTU
     RADTUB(NMDTTO)     = XTUTU
     DRITUB(NMDTTO)     = ATUTU
     ACUTUB(NMDTTO)     = UTUTU
     OLTUB(NMDTTO,1:NTUTU,1) = OTUTU1(1:NTUTU)
     OLTUB(NMDTTO,1:NTUTU,2) = OTUTU2(1:NTUTU)
     TLTUB(NMDTTO,1:NTUTU,1) = TTUTU1(1:NTUTU)
     TLTUB(NMDTTO,1:NTUTU,2) = TTUTU2(1:NTUTU)
     JrtTUB(NMDTTO)     = JrtTUTU
     JMDT = NMDTTO
   ENDIF
!
 END SUBROUTINE CREMDT
!
