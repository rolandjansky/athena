!
!> Reduce the number of radii of multiple scattering and
!> energy loss to NMAXS at maximum
!
 SUBROUTINE REDUCS(NMAXS)
 USE M_MB_COMUSC
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NMAXS
 INTEGER :: NNN, I, NTRY, IMASC, IMAMI, NORDRE(KMASC)
 REAL(8) :: DDD, EEE,            DMAMI, EMAMI, EFMASC(KMASC)
 REAL(8) :: R1, R2, X1, X2, E1, E2, P1, P2, PM, RM, XM, EM
 REAL(8), PARAMETER :: EPS=0.1D0, DISTMI=30.D0
!
   IF( NBMESU <= 2 ) NBMASC = 0
   IF( NBMASC <= 1 ) RETURN
!
!-- Re-order in increasing order radii of multiple scattering and energy losses
   CALL CROISS(RRMASC,NBMASC,NORDRE)
   CALL REORDO(RRMASC,NORDRE,NBMASC)
   CALL REORDO(X0MASC,NORDRE,NBMASC)
   CALL REORDO(ALMASC,NORDRE,NBMASC)
   CALL REORDO(BLMASC,NORDRE,NBMASC)
!-- Re-order in increasing order radii of measurements
   CALL CROISS(RRMESU,NBMESU,NORDRE)
   CALL REORDO(RRMESU,NORDRE,NBMESU)
   CALL REORDO(DDMESU,NORDRE,NBMESU)
   CALL REORDO(FFMESU,NORDRE,NBMESU)
   CALL IEORDO(ICMESU,NORDRE,NBMESU)
!-- And keep only the radii of multiple scattering that are
!-- between the first and the last radius of measurement
   NNN = 0
   DO I=1,NBMASC
     IF( RRMASC(I) < RRMESU(1)+EPS ) THEN
!-- For e.loss, merge all the matter before the first radius of measurement
!-- on the first radius of multiple scattering
       BLMASC(I+1) = ALMASC(I+1)*BLMASC(I) + BLMASC(I+1)
       ALMASC(I+1) = ALMASC(I+1)*ALMASC(I)
       CYCLE
     ELSEIF( RRMASC(I) > RRMESU(NBMESU)-EPS ) THEN
!-- For e.loss, DO NOT merge all the matter after the last radius of measurement
!-- on the last radius of multiple scattering
!******** IF( NNN >= 1 ) THEN
!********   BLMASC(NNN) = ALMASC(I)*BLMASC(NNN) + BLMASC(I)
!********   ALMASC(NNN) = ALMASC(I)*ALMASC(NNN)
!******** ENDIF
       CYCLE
     ENDIF
     NNN = NNN + 1
     RRMASC(NNN) = RRMASC(I)
     X0MASC(NNN) = X0MASC(I)
     ALMASC(NNN) = ALMASC(I)
     BLMASC(NNN) = BLMASC(I)
   ENDDO
   NBMASC = NNN
!
!-- When there are more than 2 radii of multiple scattering (and energy losses)
!-- between two consecutive radii of measurements, reduce these radii of multiple 
!-- scattering to  2  !
!****      DO J=1,NBMESU-1
!****       Rmesu0 = RRMESU(J  ) 
!****       Rmesu1 = RRMESU(J+1)
!****       Ibeg = 0
!****       Iend = 0
!****       DO I=1,NBMASC
!****        IF(     RRMASC(I) < Rmesu0-EPS ) THEN
!****           CYCLE
!****        ELSEIF( RRMASC(I) > Rmesu1+EPS ) THEN
!****           EXIT
!****        ELSE
!****           IF( Ibeg < 1 ) Ibeg = I
!****           Iend = I
!****        ENDIF
!****       ENDDO
!****       IF( Iend-Ibeg > 1 ) CALL REDUCS2(Ibeg,Iend,Rmesu0,Rmesu1)
!****      ENDDO
!****      IF(MBPri >= 3) PRINT 7771,NBMASC0,NBMASC
!****7771  FORMAT(' In first part of REDUCS, NBMASC :',I5,'  --->',I5)
!
!----------------------------------------------------------------------
!****10   IF( NBMASC <= NMAXS ) RETURN
10 IF( NBMASC <= 1 ) RETURN
!
!-- Compute the effect on the chi2 of each radius of multiple scattering
   DO IMASC=1,NBMASC
     CALL EFKIMS( NBMESU,RRMESU, RRMASC(IMASC),X0MASC(IMASC),EFMASC(IMASC) )
   ENDDO
!
!-- Find the minimum distance between two radii of multiple scattering
   IMAMI = 1
   DMAMI = (RRMASC(2)-RRMASC(1))
   DO I=2,NBMASC-1
     DDD = (RRMASC(I+1)-RRMASC(I))
     IF( DDD >= DMAMI ) CYCLE
     IMAMI = I
     DMAMI = DDD
   ENDDO
!-- If this distance is too small, merge the two corresponding radii
!-- of multiple scattering  and  go back to redo the procedure
   IF( DMAMI < DISTMI ) GO TO 15
!
!-- At this point, return if the number of radii of multiple
!-- scattering is small enough
   IF( NBMASC <= NMAXS ) RETURN
!
!-- Otherwise, find the first couple of radii
!-- of multiple scattering to be merged
   IMAMI = 1
   EMAMI = (RRMASC(2)-RRMASC(1))**2 * MIN(EFMASC(1),EFMASC(2))
   DO I=2,NBMASC-1
     EEE = (RRMASC(I+1)-RRMASC(I))**2 * MIN(EFMASC(I),EFMASC(I+1))
     IF( EEE >= EMAMI ) CYCLE
     IMAMI = I
     EMAMI = EEE
   ENDDO
!-- And merge them (this reduces the number of radii of multiple
!-- scattering by 1)  and  then go back to see whether it is enough
15 R1 = RRMASC(IMAMI)
   R2 = RRMASC(IMAMI+1)
   X1 = X0MASC(IMAMI)
   X2 = X0MASC(IMAMI+1)
   E1 = EFMASC(IMAMI)
   E2 = EFMASC(IMAMI+1)
   P1 = E1 / X1
   P2 = E2 / X2
   PM = ( E1*P1 + E2*P2 ) / ( E1 + E2 )
   RM = ( E1*R1 + E2*R2 ) / ( E1 + E2 )
   XM = X1*P1/PM + X2*P2/PM
   DO NTRY=1,4
     CALL EFKIMS( NBMESU,RRMESU, RM,XM, EM )
     XM = XM * (E1+E2)/EM
     IF( ABS(EM/(E1+E2)-1.D0) <= 0.001D0 ) EXIT
   ENDDO
   RRMASC(IMAMI) = RM
   X0MASC(IMAMI) = XM
   BLMASC(IMAMI) = ALMASC(IMAMI+1)*BLMASC(IMAMI) + BLMASC(IMAMI+1)
   ALMASC(IMAMI) = ALMASC(IMAMI+1)*ALMASC(IMAMI)
   DO I=IMAMI+1,NBMASC-1
     RRMASC(I) = RRMASC(I+1)
     X0MASC(I) = X0MASC(I+1)
     ALMASC(I) = ALMASC(I+1)
     BLMASC(I) = BLMASC(I+1)
   ENDDO
   NBMASC = NBMASC - 1
   GO TO 10
!----------------------------------------------------------------------
 END SUBROUTINE REDUCS
!
