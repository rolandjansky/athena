!
 SUBROUTINE arrangeTubes
 USE M_MB_Digis
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER :: IC, IW, IMDT, IP, IT, ITDEB,ITFIN, ITT, ITP1
 INTEGER :: ICUR, JCUR, LCUR
 REAL(8) :: ZCUR, DCUR, RCUR, SCUR
!
!>> Re-order digi. bank !
    CALL ATUEND
!
!>> Then rearrange MDT digi. to have all hits on same tube (if any) consecutives
!
    DO IC=1,NCOUCH
      DO IW=1,NCHAHI(IC)
        IMDT = ISTMDTV( IGEOMA(IC,IFCHA(IC,IW),IZCHA(IC,IW)) )
        DO IP=1,NLTUB(IMDT)
          IF( NTUBHI(IC,IW,IP) <= 0 ) CYCLE
!
          ITDEB = NTUBH0(IC,IW,IP) + 1
          ITFIN = NTUBH0(IC,IW,IP) + NTUBHI(IC,IW,IP)
          DO IT=ITDEB,ITFIN
            ZCUR = ZTUBHI(IT)
            DO ITT=IT+1,ITFIN
              IF( ABS(ZTUBHI(ITT)-ZCUR) > 0.1D0 )  CYCLE
              ITP1 = IT + 1
              IF( ITT == ITP1 )                    EXIT
              ZCUR = ZTUBHI(ITT)
              DCUR = DTUBHI(ITT)
              RCUR = RTUBHI(ITT)
              SCUR = STUBHI(ITT)
              ICUR = ITUBHI(ITT)
              JCUR = JTUBHI(ITT)
              LCUR = LTUBHI(ITT)
              ZTUBHI(ITT) = ZTUBHI(ITP1)
              DTUBHI(ITT) = DTUBHI(ITP1)
              RTUBHI(ITT) = RTUBHI(ITP1)
              STUBHI(ITT) = STUBHI(ITP1)
              ITUBHI(ITT) = ITUBHI(ITP1)
              JTUBHI(ITT) = JTUBHI(ITP1)
              LTUBHI(ITT) = LTUBHI(ITP1)
              ZTUBHI(ITP1) = ZCUR
              DTUBHI(ITP1) = DCUR
              RTUBHI(ITP1) = RCUR
              STUBHI(ITP1) = SCUR
              ITUBHI(ITP1) = ICUR
              JTUBHI(ITP1) = JCUR
              LTUBHI(ITP1) = LCUR
              EXIT
            ENDDO
          ENDDO
!
        ENDDO
      ENDDO
    ENDDO
!
! and allocate space for dca computation
   IF( ASSOCIATED(SaWiXPT) ) THEN
     DEALLOCATE(SaWiXPT)
     DEALLOCATE(SaWiYPT)
     DEALLOCATE(SaWiZPT)
     DEALLOCATE(SaWiXDI)
     DEALLOCATE(SaWiYDI)
     DEALLOCATE(SaWiZDI)
   ENDIF
   ALLOCATE( SaWiXPT(NTUBHM) )
   ALLOCATE( SaWiYPT(NTUBHM) )
   ALLOCATE( SaWiZPT(NTUBHM) )
   ALLOCATE( SaWiXDI(NTUBHM) )
   ALLOCATE( SaWiYDI(NTUBHM) )
   ALLOCATE( SaWiZDI(NTUBHM) )
!
 END SUBROUTINE arrangeTubes
!
