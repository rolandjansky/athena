!
!> Among candidate tracks that share many common hits,
!> eliminate the ones of worst quality.
!> \author M.Virchaux
!
 SUBROUTINE CHECAN
 USE M_MB_Control
 USE M_MB_Reco_Tracks
 USE M_MB_PACONS
 IMPLICIT NONE
 INTEGER :: Nused, Nreje, K, KU1, KU2, K1, NZ1, K2, NZ2, NZC
 INTEGER :: J1, IZ1, J2, IDITOL, NSC1, NSC2, KU0
 INTEGER :: K0, Kelim
 INTEGER :: intpos
 REAL(8) :: PROP, X2DIF, CHID, V2DIF
 INTEGER, ALLOCATABLE :: NORDRE(:)
 REAL(8), ALLOCATABLE :: CORDRE(:)
!
   IF( NBCAN < 1 ) RETURN
!
   IF( MBSto >= 3 ) THEN
!>> Eliminate candidates  that use less than 6 tube hits
!>> or  that reject (or miss) more than 20 tube hits
!>> after the "grand final fit with matter"
     DO K=1,NBCAN
       IF( CHICAN(K) >= 1000.D0 )  CYCLE
       Nused = NZXCAN(K)
       Nreje = NBXCAN(K)
       IF( Nused < 6 .OR. (Nused < 12 .AND. Nreje > 20) .OR. Nreje > 30 ) THEN
         CHICAN(K) = CHICAN(K) + 1000.D0
         IF( MBPri >= 3 ) PRINT 7030,K
7030     FORMAT(2X,'Eliminate candidate nb.',I3,'  because it uses less than 6 tube hits',  &
                   ' or rejects too many tube hits')
       ENDIF
     ENDDO
   ENDIF
!
   ALLOCATE( CORDRE(NBCAN) )
   ALLOCATE( NORDRE(NBCAN) )
   CORDRE(1:NBCAN) = CXXCAN(2,1:NBCAN)
   CALL CROISS(CORDRE,NBCAN,NORDRE)
!
   DO KU1=1,NBCAN
     K1 = NORDRE(KU1)
     IF( CHICAN(K1) >= 1000.D0 )  CYCLE
     NZ1 = NZXCAN(K1)
     DO 210 KU2=KU1+1,NBCAN
       K2 = NORDRE(KU2)
       IF( CHICAN(K2) >= 1000.D0 ) CYCLE
       NZ2 = NZXCAN(K2)
!
       NZC = 0
       DO J1=1,NZ1
         IZ1 = IZXCAN(J1,K1)
         DO J2=1,NZ2
           IF( IZ1 /= IZXCAN(J2,K2) ) CYCLE
           NZC = NZC + 1
           EXIT
         ENDDO
       ENDDO
!
       PROP = REAL(NZC,8) / MIN(NZ1,NZ2)
       IF( PROP < 0.40D0 )  CYCLE
!>> Compare the reconstructed parameters for 0.40 < prop < 0.80
       IF( PROP < 0.80D0 ) THEN
         X2DIF = SUM( (VMUCAN(1:3,K1)-VMUCAN(1:3,K2))**2 ) / 25.D0    ! sigma = 5cm
         V2DIF = SUM( (VMUCAN(4:6,K1)-VMUCAN(4:6,K2))**2 ) / 0.0004D0 ! sigma = 2%
         CHID  = X2DIF + V2DIF
         IF( CHID > 20.D0 ) THEN
! parameters are far away. We check that none are them are really badly reconstructed
! before keeping them
           IF( NZXCAN(K1) <= NZXCAN(K2) .AND. NSSCAN(K1) <= NSSCAN(K2) ) THEN
             PROP = REAL(NBXCAN(K1),8) / REAL(NZXCAN(K1),8)
             Kelim = K1
             IF( NZXCAN(K2) == NZXCAN(K1) .AND. NSSCAN(K2) == NSSCAN(K1) .AND.  &
                 REAL(NBXCAN(K2),8) / REAL(NZXCAN(K2),8) > PROP ) THEN
               PROP = REAL(NBXCAN(K2),8) / REAL(NZXCAN(K2),8)
               Kelim = K2
             ENDIF
           ELSE IF( NZXCAN(K2) <= NZXCAN(K1) .AND. NSSCAN(K2) <= NSSCAN(K1) ) THEN
             PROP = REAL(NBXCAN(K2),8) / REAL(NZXCAN(K2),8)
             Kelim = K2
           ENDIF
           IF( PROP >= 1.5D0 ) THEN
              CHICAN(Kelim) = CHICAN(Kelim) + 1000.D0
             IF( MBPri >= 3 ) PRINT 7040,Kelim,K1+K2-Kelim
             IF( Kelim == K1 ) EXIT
           ENDIF
           CYCLE
         ENDIF
       ENDIF
!>> If   prop > 0.80   or   prop > 0.40 with similar recons. parameters,
!>> keep only the candidate that has more hits  or  when the number
!>> of hits is similar, keep the one with more S strips or one with the best chi2
!>> or with the least number of already used strips.
       IF( NZ1 > NZ2 .AND. CXXCAN(2,K1) < CXXCAN(2,K2) ) THEN
         Kelim = K2
       ELSE IF( NZ2 > NZ1 .AND. CXXCAN(2,K2) < CXXCAN(2,K1) ) THEN
         Kelim = K1
       ELSE
         IDITOL = 2.D0 + ABS(CHICAN(K1)-CHICAN(K2)) / 15.D0
         IDITOL = MIN( IDITOL , 7 )
         IF( IABS(NZ1-NZ2) <= IDITOL ) THEN
           NSC1 = 0
           NSC2 = 0
           DO 120 KU0=1,KU1-1
             K0 = NORDRE(KU0)
             IF( CHICAN(K0) >= 1000.D0 )  CYCLE
             DO J1=1,NSXCAN(K1)
               IF( intpos(ISXCAN(J1,K1),ISXCAN(1:NSXCAN(K0),K0),NSXCAN(K0)) > 0 ) NSC1 = NSC1 + 1
             ENDDO
             DO J2=1,NSXCAN(K2)
               IF( intpos(ISXCAN(J2,K2),ISXCAN(1:NSXCAN(K0),K0),NSXCAN(K0)) > 0 ) NSC2 = NSC2 + 1
             ENDDO
120        CONTINUE
           IF( NSC2 < NSC1 .AND. CXXCAN(2,K2) < CXXCAN(2,K1)+FLOOK2/15.D0 ) THEN
             Kelim = K1
           ELSEIF( NSC1 < NSC2 .AND. CXXCAN(2,K1) < CXXCAN(2,K2)+FLOOK2/15.D0 ) THEN
             Kelim = K2
           ELSEIF( NSSCAN(K1) >= 2*NSSCAN(K2)+1 ) THEN
             Kelim = K2
           ELSEIF( NSSCAN(K2) >= 2*NSSCAN(K1)+1 ) THEN
             Kelim = K1
           ELSEIF( CXXCAN(2,K2) < CXXCAN(2,K1) ) THEN
             Kelim = K1
           ELSE
             Kelim = K2
           ENDIF
         ELSE
           IF( NZ1 > NZ2 ) THEN
             Kelim = K2
           ELSE
             Kelim = K1
           ENDIF
         ENDIF
       ENDIF
       CHICAN(Kelim) = CHICAN(Kelim) + 1000.D0
       IF( MBPri >= 3 ) PRINT 7040,Kelim,K1+K2-Kelim
7040   FORMAT(2X,'Eliminate candidate nb.',I3,'  because it shares too many hits with',  &
                 ' (better) candidate',I3)
       IF( Kelim == K1 ) EXIT 
!
210  ENDDO
   ENDDO
!
   DEALLOCATE( CORDRE )
   DEALLOCATE( NORDRE )
!
 END SUBROUTINE CHECAN
!
