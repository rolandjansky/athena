!
!> \author SaMuSoG
!
 SUBROUTINE Bool_Sub_Seg(NP,XP,YP,XA,YA,XB,YB,Ndim,Ioverf, NE,EE)
 IMPLICIT NONE
 INTEGER :: NP,Ndim,Ioverf,NE
 REAL(8) :: XP(*),YP(*)
 REAL(8) :: XA,YA,XB,YB
 REAL(8) :: EE(*)
 INTEGER :: I,IC,ID,NENEW
 REAL(8) :: DD,DDB,EAB,ECD
 REAL(8), PARAMETER :: ZEROM=-0.000001D0, ZEROP=0.000001D0
 REAL(8), PARAMETER ::  ONEM=0.999999D0,   ONEP=1.000001D0
!
   NE = 0
   DO I=1,NP
     IC = I
     ID = MOD(I,NP) + 1
     DD = (YB-YA)*(XP(ID)-XP(IC)) - (XB-XA)*(YP(ID)-YP(IC))
     IF( ABS(DD) < ZEROP ) THEN
       DDB = (YB-YA)*(XP(ID)-XA) - (XB-XA)*(YP(ID)-YA)
       IF( ABS(DDB) < ZEROP ) THEN
         IF( ABS(YB-YA) > ABS(XB-XA) ) THEN
           EAB = (YP(IC)-YA) / (YB-YA)
           IF( EAB > ZEROP .AND. EAB < ONEM ) THEN
             CALL Bool_Incr(NE,Ndim,'Ndim_seg',Ioverf)
             IF( Ioverf /= 0 ) GO TO 96
             EE(NE) = EAB
           ENDIF
           EAB = (YP(ID)-YA) / (YB-YA)
           IF( EAB > ZEROP .AND. EAB < ONEM ) THEN
             CALL Bool_Incr(NE,Ndim,'Ndim_seg',Ioverf)
             IF( Ioverf /= 0 ) GO TO 96
             EE(NE) = EAB
           ENDIF
         ELSE
           EAB = (XP(IC)-XA) / (XB-XA)
           IF( EAB > ZEROP .AND. EAB < ONEM ) THEN
             CALL Bool_Incr(NE,Ndim,'Ndim_seg',Ioverf)
             IF( Ioverf /= 0 ) GO TO 96
             EE(NE) = EAB
           ENDIF
           EAB = (XP(ID)-XA) / (XB-XA)
           IF( EAB > ZEROP .AND. EAB < ONEM ) THEN
             CALL Bool_Incr(NE,Ndim,'Ndim_seg',Ioverf)
             IF( Ioverf /= 0 ) GO TO 96
             EE(NE) = EAB
           ENDIF
         ENDIF
       ENDIF
     ELSE
       EAB = ( (YP(IC)-YA)*(XP(ID)-XP(IC)) - (XP(IC)-XA)*(YP(ID)-YP(IC)) ) / DD
       ECD = ( (XB-XA)*(YP(IC)-YA) - (YB-YA)*(XP(IC)-XA) ) / DD
       IF(   EAB > ZEROP .AND. EAB < ONEM .AND. ECD > ZEROM .AND. ECD < ONEP ) THEN
         CALL Bool_Incr(NE,Ndim,'Ndim_seg',Ioverf)
         IF( Ioverf /= 0 ) GO TO 96
         EE(NE) = EAB
       ENDIF
     ENDIF
   ENDDO
!
   CALL CROIST(EE,NE)
!
   IF( NE >= 2 ) THEN
     NENEW = 1
     DO I=2,NE
       IF( ABS(EE(I)-EE(I-1)) > ZEROP ) THEN
         NENEW = NENEW + 1
         EE(NENEW) = EE(I)
       ENDIF
     ENDDO
     NE = NENEW
   ENDIF
!
   RETURN
!
96 Ioverf = 6
   RETURN
!
 END SUBROUTINE Bool_Sub_Seg
!
