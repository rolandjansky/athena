!
!> \author M.Virchaux
!
 SUBROUTINE PRFILI(IFLAG)
 USE M_MB_Control
 USE M_MB_FAX
 USE M_MB_PACONS
 USE M_MB_General_CONSTANTS, ONLY : CP0000, C1P000
 USE M_MB_MuGeom
 USE M_MB_CODRAT
 USE M_MB_COMUSC
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER :: IFLAG
 INTEGER :: NN, ISC, ID, L, J, I8P, I4P, NFX, NBFX
 REAL(8) :: CHICHI, DOFDOF
!
   IF( ItypFit /= 3 .AND. ItypFit /= 6 ) THEN
     DO NN=1,NBFIXE
       ISC = ISCFIXE(NN)
       ID  = IDFIXE(NN)
       IF( IWcross(ISC,1)+IWcross(ISC,2) > 0 ) ID = 0
       PRINT 1000,CARISC(ISC),ISC,ID,  (2*ISC+1-J,DZFIXE(NN,J,1),DSFIXE(NN,J,1),  &
                                                  NZFIXE(NN,J,1),NSFIXE(NN,J,1),J=1,2)
     ENDDO
     DO NN=1,NBLIBR
       ISC = ISCLIBR(NN)
       ID  = IDLIBR(NN)
       IF( IWcross(ISC,1)+IWcross(ISC,2) > 0 ) ID = 0
       PRINT 3000,CARISC(ISC),ISC,ID,  (2*ISCLIBR(NN)+1-J,DZLIBR(NN,J,1),DSLIBR(NN,J,1),  &
                                                          NZLIBR(NN,J,1),NSLIBR(NN,J,1),J=1,2)
     ENDDO
   ELSE
     DO NN=1,NBFAX
       PRINT 7000,CARISC(ISCFAX(NN)),ISCFAX(NN),(2*ISCFAX(NN)+1-J,IOFFAX(NN,J),IOZFAX(NN,J),  &
                              DZFIXE(NN,J,1),DSFIXE(NN,J,1),NZFIXE(NN,J,1),NSFIXE(NN,J,1),J=1,2)
     ENDDO
     IF( NCFAX > 0 ) THEN
       NN = NBFAX + 1
       WRITE(*,*) ' additionnal RPC/TGC ',ICTFAX(1:NCFAX)
       PRINT 7100,(DSFIXE(NN,J,1),NSFIXE(NN,J,1),J=1,2)
     ENDIF
   ENDIF
   IF( AtCosm == 0 ) PRINT 4000,ZEXTRA(1),REXTRA(1), (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
   PRINT 6000,NBMASC
   IF( NBMASC >= 1 ) PRINT 6100,LLMASC(1:NBMASC)
   IF( NBMASC >= 1 ) PRINT 6200,S1MASC(1:NBMASC)
   IF( NBMASC >= 1 ) PRINT 6300,S2MASC(1:NBMASC)
!
   CHICHI =  0.D0
   DOFDOF = -5.D0
   DO L=1,NBMASC
     IF( LLMASC(L) == 5 )  CHICHI = CHICHI + S1MASC(L)**2
   ENDDO
   NFX = MAX( NBFIXE , NBFAX )
   DO NN=1,NFX
     CHICHI = CHICHI + DZFIXE(NN,1,1)       + DZFIXE(NN,2,1)       &
                     + DSFIXE(NN,1,1)       + DSFIXE(NN,2,1)
     DOFDOF = DOFDOF + NZFIXE(NN,1,1)       + NZFIXE(NN,2,1)       &
                     + NSFIXE(NN,1,1)*FacSZ + NSFIXE(NN,2,1)*FacSZ
   ENDDO
   IF( NCFAX > 0 ) THEN
     NN = NFX + 1
     CHICHI = CHICHI + DSFIXE(NN,1,1)       + DSFIXE(NN,2,1)
     DOFDOF = DOFDOF + NSFIXE(NN,1,1)*FacSZ + NSFIXE(NN,2,1)*FacSZ
   ENDIF
   DO NN=1,NBLIBR
     CHICHI = CHICHI + DZLIBR(NN,1,1)       + DZLIBR(NN,2,1)       &
                     + DSLIBR(NN,1,1)       + DSLIBR(NN,2,1)
     DOFDOF = DOFDOF + NZLIBR(NN,1,1)       + NZLIBR(NN,2,1)       &
                     + NSLIBR(NN,1,1)*FacSZ + NSLIBR(NN,2,1)*FacSZ
   ENDDO
   IF( AtCosm == 0 ) THEN
     CHICHI = CHICHI + (REXTRA(1)/DREXTR(1))**2 + (ZEXTRA(1)/DZEXTR(1))**2
     DOFDOF = DOFDOF + 2.d0
   ENDIF
   DOFDOF = MAX( DOFDOF , C1P000 )
   CHICHI = CHICHI / DOFDOF
   PRINT 8000,CHICHI,DOFDOF
!
   IF(IFLAG <= 1) RETURN
!
   IF( ItypFit /= 3 ) THEN
     PRINT 5000, FMIN,100.D0/SMU,        VDEB(7,1),VFIN(7),PFIMIN,PFIMAX
   ELSE
     PRINT 5000, FMIN, 1.D0/VTHETAPHI(5),VDEB(7,1),VFIN(7),PFIMIN,PFIMAX
   ENDIF
   IF(IFLAG <= 2 .OR. ItypFit == 2) RETURN
!
   NBFX = MAX( NBFIXE , NBFAX )
   DO NN=1,NBFX
     DO J=1,2
       DO I8P=1,N8M
         IF( JSFIXE(I8P,NN,J ) == 0 )  EXIT
         PRINT 2000,JSFIXE(I8P,NN,J),MAX(ISFIXE(I8P,NN,J),0)
       ENDDO
     ENDDO
     DO J=1,2
       DO I4P=1,N4M
         IF( JZFIXE(I4P,1,NN,J) == 0 ) EXIT
         PRINT 2000,JZFIXE(I4P,1:4,NN,J),IZFIXE(I4P,1:4,NN,J)
       ENDDO
     ENDDO
   ENDDO
   IF( NCFAX > 0 ) THEN
     NN = NBFAX + 1
     DO J=1,2
       DO I8P=1,N8M
         IF( JSFIXE(I8P,NN,J ) == 0 )  EXIT
         PRINT 2000,JSFIXE(I8P,NN,J),MAX(ISFIXE(I8P,NN,J),0)
       ENDDO
     ENDDO
   ENDIF
!
1000 FORMAT(2X,'ISCfix,IDfix,IC,DZ-Sfix(.,1) = ',A3,3I4,2F15.3,2I5/   &
            2X,'             IC,DZ-Sfix(.,2) = ',11X,I4,2F15.3,2I5)
2000 FORMAT(2X,4I16,8X,4I8)
3000 FORMAT(2X,'ISClib,IDlib,IC,DZ-Slib(.,1) = ',A3,3I4,2F15.3,2I5/   &
            2X,'             IC,DZ-Slib(.,2) = ',11X,I4,2F15.3,2I5)
4000 FORMAT(2X,'Zextra,Rextra =',2F10.2,        16X,F15.3)
6000 FORMAT(2X,'NBmasc =',I5)
6100 FORMAT(2X,'LLmasc =',12I8)
6200 FORMAT(2X,'S1masc =',12F8.3)
6300 FORMAT(2X,'S2masc =',12F8.3)
5000 FORMAT(2X,'Fmin =',45X,F15.3/                                    &
            2X,'Pmu =',F9.2,'      Pdeb,Pfin,Pmin,Pmax=',4F9.2)
7000 FORMAT(2X,'ISCfax,IC,INOF_Z,DZ-Sfix(.,1) = ',A3,4I4,2F15.3,2I5/  &
            2X,'       IC,INOF_Z,DZ-Sfix(.,2) = ',7X,3I4,2F15.3,2I5)
7100 FORMAT(2X,'   Additionnal RPC/TGC  (.,1) = ',34X,F15.3,5X,I5/  &
            2X,'                        (.,2) = ',34X,F15.3,5X,I5)
8000 FORMAT(2X,'Chi2, Dof ',40('='),'>',F15.3,F10.2)
!
 END SUBROUTINE PRFILI
!
