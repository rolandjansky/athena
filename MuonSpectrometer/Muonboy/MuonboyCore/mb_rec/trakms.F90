!
!> Estimate starting value of MS angles, such that they compensate the deflexion
!> from eloss when we switch on matter in recotra
!> \author A.Ouraou
!
 SUBROUTINE TRAKMS(RRFMIN,RRFMAX)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_COMUSC
 USE M_MB_CTRABAZ
 USE M_MB_CODRAT
 USE M_MB_Fit
 IMPLICIT NONE
 REAL(8), INTENT(IN) :: RRFMIN, RRFMAX
 LOGICAL :: tobedone
 INTEGER :: IZTPLA, ISC, ID, J, IC, INOF, INOZ, IGEOM
 INTEGER :: NN, IW, IWDRATfun, IZT,NZT, IMSMIN,IMSMAX, IZB, NZB
 INTEGER :: IMS, NBMASCO, ICR0(MAXPLA), IR(MAXPLA), IORD(MAXPLA), ITMP(MAXPLA)
 REAL(8) :: RDEB, ZDEB, ZR(MAXPLA), RR(MAXPLA), Z0(MAXPLA), TEMP(MAXPLA)
 REAL(8) :: RRA, RMSMAX, RMSMIN, FAC, RCUR, ZZZ, TTT
 REAL(8), PARAMETER :: step=0.2d0
!
   IF( NoField )                               RETURN 
!
!-- Do not track if outside range
   RDEB = SQRT( VDEB(1,1)**2 + VDEB(2,1)**2 )
   ZDEB = ABS( VDEB(3,1) )
   IF( RDEB > RCHAMA .OR. ZDEB > ZCHAMA .OR. (RDEB < RCHAMI.AND.ZDEB < ZCHAMI)    &
                     .OR. VDEB(7,1) < 0.5D0 )  RETURN
   IF( NBFIXE <= 0 )                           RETURN
   IF( NBFIXE+NBOKLI <= 1 )                    RETURN
!
!-- Set common CTRABAZ
   IZTPLA = 0
!-- Fixed stations
   DO NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     ID = 1
     IF( IWcross(ISC,1)+IWcross(ISC,2) < 1 ) ID = IDFIXE(NN)
     DO J=1,2
       IW = ABS( IWDRATfun(ID,J,ISC) )
       IF( IW <= 0 ) CYCLE
       IC   = 2*ISC + 1 - J
       INOF = IFCHA(IC,IW)
       INOZ = IZCHA(IC,IW)
       IGEOM= IGEOMA(IC,INOF,INOZ)
       IZTPLA = MIN( IZTPLA+1 , MAXPLA )
       CA0PLA(IZTPLA) = 1.D0
       SA0PLA(IZTPLA) = 0.D0
       TT0PLA(IZTPLA) = TCECHAV(IGEOM)
       CF0PLA(IZTPLA) = COSCHAV(IGEOM)
       SF0PLA(IZTPLA) = SINCHAV(IGEOM)
       IF( IC > NCOUCH ) THEN
         CA0PLA(IZTPLA) = COS(GCECHAV(IGEOM))
         SA0PLA(IZTPLA) = SIN(GCECHAV(IGEOM))
       ENDIF
       CALL CODEIN(1,0,IW,NN,ID,J,IC,INOF,INOZ,0, IZTPLA)
     ENDDO
   ENDDO
!-- Total number of planes
   NZTPLA = IZTPLA
!
! trajectory without matter
   NBMASCO = NBMASC
   NBMASC  = 0
   CALL TRABAZ(1,2,2)
   NZT = 0
   DO IZTPLA=1,NZTPLA
     IF( ICRPLA(IZTPLA) == 0 )                        CYCLE
     IF( ICCPLA(IZTPLA) < 0 ) THEN
       RRA = getRMUSC( TT0PLA(IZTPLA), SSSPLA(IZTPLA,1), ZTTPLA(IZTPLA,1) )
     ELSE IF( ICCPLA(IZTPLA) > 0 ) THEN
       RRA = getRMUSC( ZTTPLA(IZTPLA,1), SSSPLA(IZTPLA,1), TT0PLA(IZTPLA) )
     ELSE
       ZZZ = - SA0PLA(IZTPLA)*ZTTPLA(IZTPLA,1) + CA0PLA(IZTPLA)*TT0PLA(IZTPLA)
       TTT =   CA0PLA(IZTPLA)*ZTTPLA(IZTPLA,1) + SA0PLA(IZTPLA)*TT0PLA(IZTPLA)
       RRA = getRMUSC( ZZZ, SSSPLA(IZTPLA,1), TTT )
     ENDIF
     IF( RRA < RRFMIN-50.d0 .OR. RRA > RRFMAX+50.d0 ) CYCLE
     NZT = NZT + 1
     ZR(NZT) = ZTTPLA(IZTPLA,1)
     RR(NZT) = RRA
     IR(NZT) = IZTPLA
   ENDDO
   NBMASC = NBMASCO
   S1MASC(1:NBMASC) = 0.d0
   IF( NZT <= 0 )  GO TO 90
!
! Reorder by increasing RR
   CALL CROISS(RR,NZT,IORD)
   TEMP(1:NZT) = RR(1:NZT)
   RR(1:NZT) = TEMP(IORD(1:NZT))
   TEMP(1:NZT) = ZR(1:NZT)
   ZR(1:NZT) = TEMP(IORD(1:NZT))
   ITMP(1:NZT) = IR(1:NZT)
   IR(1:NZT) = ITMP(IORD(1:NZT))   
!
! trajectory with matter, but without MS (different from the previous one because of eloss)
   CALL TRABAZ(1,2,2)
   DO IZT=1,NZT
     ICR0(IZT) = ICRPLA(IR(IZT))
     Z0(IZT)   = ZTTPLA(IR(IZT),1)
   ENDDO
   tobedone = .false.
!
! now add progressively MS
   RCUR = getRMUSC( VDEB(1,1), VDEB(2,1), VDEB(3,1) )
   DO IZT=1,NZT
     IF( RR(IZT) < RCUR ) CYCLE
     IMSMIN = 99
     IMSMAX =  0
     DO IMS=Navant+1,NBMASC
       IF( RRMASC(IMS) >= RCUR .AND. RRMASC(IMS) < RR(IZT) ) THEN
         IF( IMS < IMSMIN ) IMSMIN = IMS
         IF( IMS > IMSMAX ) IMSMAX = IMS
       ENDIF
     ENDDO
     RCUR = RR(IZT)
     IF( IMSMIN > IMSMAX ) CYCLE
     IF( tobedone ) THEN
       CALL TRABAZ(1,2,2)
       DO IZB=1,NZT
         ICR0(IZB) = ICRPLA(IR(IZB))
         Z0(IZB)   = ZTTPLA(IR(IZB),1)
       ENDDO
       tobedone = .false.
     ENDIF
     S1MASC(IMSMIN:IMSMAX) = step
     CALL TRABAZ(1,2,2)
     RMSMAX = 999999.d0
     IF( IMSMAX < NBMASC ) RMSMAX = RRMASC(IMSMAX+1)
     NZB = 0
     FAC = 0.d0
     DO IZB=IZT,NZT
       IF( RR(IZB) >= RMSMAX ) EXIT
       IF( ICRPLA(IR(IZB)) == 0 ) CYCLE
       IF( ICR0(IZB)       == 0 ) CYCLE
       IF( ABS(ZTTPLA(IR(IZB),1)-Z0(IZB)) < 0.0005d0 ) CYCLE
       NZB = NZB + 1
       FAC = FAC + (ZR(IZB)-Z0(IZB))/(ZTTPLA(IR(IZB),1)-Z0(IZB))
     ENDDO
     IF( NZB == 0 ) THEN
       S1MASC(IMSMIN:IMSMAX) = 0.d0
     ELSE
       tobedone = .true.
       S1MASC(IMSMIN:IMSMAX) = step * FAC / NZB
     ENDIF
   ENDDO
!
   RCUR = getRMUSC( VDEB(1,1), VDEB(2,1), VDEB(3,1) )
   DO IZT=NZT,1,-1
     IF( RR(IZT) >= RCUR ) CYCLE
     IMSMIN = 99
     IMSMAX =  0
     DO IMS=Navant+1,NBMASC
       IF( RRMASC(IMS) < RCUR .AND. RRMASC(IMS) >= RR(IZT) ) THEN
         IF( IMS < IMSMIN ) IMSMIN = IMS
         IF( IMS > IMSMAX ) IMSMAX = IMS
       ENDIF
     ENDDO
     RCUR = RR(IZT)
     IF( IMSMIN > IMSMAX ) CYCLE
     IF( tobedone ) THEN
       CALL TRABAZ(1,2,2)
       DO IZB=1,NZT
         ICR0(IZB) = ICRPLA(IR(IZB))
         Z0(IZB)   = ZTTPLA(IR(IZB),1)
       ENDDO
       tobedone = .false.
     ENDIF
     S1MASC(IMSMIN:IMSMAX) = step
     CALL TRABAZ(1,2,2)
     RMSMIN = 0.d0
     IF( IMSMIN > 1 ) RMSMIN = RRMASC(IMSMIN-1)
     NZB = 0
     FAC = 0.d0
     DO IZB=IZT,1,-1
       IF( RR(IZB) <= RMSMIN ) EXIT
       IF( ICRPLA(IR(IZB)) == 0 ) CYCLE
       IF( ICR0(IZB)       == 0 ) CYCLE
       IF( ABS(ZTTPLA(IR(IZB),1)-Z0(IZB)) < 0.0005d0 ) CYCLE
       NZB = NZB + 1
       FAC = FAC + (ZR(IZB)-Z0(IZB))/(ZTTPLA(IR(IZB),1)-Z0(IZB))
     ENDDO
     IF( NZB == 0 ) THEN
       S1MASC(IMSMIN:IMSMAX) = 0.d0
     ELSE
       tobedone = .true.
       S1MASC(IMSMIN:IMSMAX) = step * FAC / NZB
     ENDIF
  ENDDO
!
! Then recomputes the trajectory with the starting values of scattering angles
90 NZTPLA = 0
   CALL TRABAZ(1,2,0)  
!
 END SUBROUTINE TRAKMS
!
