!
!> Suivi d'une particule chargee dans un champs magnetique
!> par succession d'ELEMents d'Helices Circulaires.
!> Champs mag. en  Tesla  et distances en  cm  !!!
!>                *******                 ****
!>          (MV   Revu et corrige   /06/99)
!> Charge de la particule = VIN(8)    (en general +1. ou -1.)
!> \author M.Virchaux
!
 SUBROUTINE ELEMHC(VIN,NVIN,PAPAS,  VOUT)
 USE M_MB_General_CONSTANTS, ONLY : InvLight
 USE M_MB_CTRABAZ, ONLY : KVDEB
 USE M_MB_PRECIS
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: NVIN
 REAL(8), INTENT(IN)  :: VIN(8,NVIN), PAPAS
 REAL(8), INTENT(OUT) :: VOUT(8,NVIN)
 INTEGER, SAVE :: N312, INCREA, IBNUL
 REAL(8), SAVE  :: PASTOT, FAC, EB2MAX, EB2MIN, PAS, PAS2, BBA
 REAL(8), SAVE  :: VCUR4,VCUR5,VCUR6, TX,TY,TZ, VX,VY,VZ,VV, WX,WY,WZ
 REAL(8), SAVE  :: CT, ST, RR, AA,SA,CA, V,W,T, BBAMB2, A2
 REAL(8), SAVE  :: VCUR(8,KVDEB), VTEMP(3)
 REAL(8), SAVE  :: BXYZA(12), BXYZB(12)
 REAL(8), DIMENSION(3), SAVE :: Xprevious = 999999.D0
 REAL(8), DIMENSION(3), SAVE :: Bprevious = 0.D0
 REAL(8),               SAVE :: Pprevious = 6.D0, BBprevious = 0.d0
 REAL(8), PARAMETER :: EpsPos = 0.0100D0, EpsPas = 0.0001D0
 REAL(8) :: InvBBA
 REAL(8) :: InvVV
!
    IF( NVIN <= 1 ) THEN
      N312 =  3
    ELSE
      N312 = 12
    ENDIF
    VCUR(:,1:NVIN) = VIN(:,1:NVIN)
!
    PASTOT = 0.D0
    INCREA = 0
    FAC = VIN(7,1) / FACREF
    FAC = MAX( FACMIN , MIN( FACMAX , FAC ) )
    EB2MAX = ( DBMAX * FAC )**2
    EB2MIN = ( DBMIN * FAC )**2
!
!>>---------------------------------------------------------------------
!>> Compute the B field at the beginning of the step
    IF( ABS(VCUR(1,1)-Xprevious(1)) < EpsPos .AND. ABS(VCUR(2,1)-Xprevious(2)) < EpsPos   &
                                             .AND. ABS(VCUR(3,1)-Xprevious(3)) < EpsPos ) THEN
      PAS = MIN( 2.D0*Pprevious , PAPAS )
      BXYZA(1:3) = Bprevious(1:3)
      BBA = BBprevious
    ELSE
      PAS = PAPAS
      CALL DFIELD(VCUR(1,1), 3, BXYZA)
      BBA = SQRT( BXYZA(1)**2 + BXYZA(2)**2 + BXYZA(3)**2 )
    ENDIF
    PAS2 = PAS * 0.5d0
    Pprevious = 0.D0
!
!>> Perform preliminary step (pas/2) with a constant B field as computed above
    VCUR4 = VCUR(4,1)
    VCUR5 = VCUR(5,1)
    VCUR6 = VCUR(6,1)
    IF( BBA < BBNUL ) THEN
      IF( ABS(VCUR(3,1)) > AZBMIN .AND. VCUR(3,1)*VCUR6 > 0.d0 ) THEN
        VOUT(1:3,1:NVIN) = VIN(1:3,1:NVIN) + PAPAS*VIN(4:6,1:NVIN)
        VOUT(4:8,1:NVIN) = VIN(4:8,1:NVIN)
        Xprevious(1:3) = VOUT(1:3,1)
        Pprevious      = PAPAS
        RETURN
      ENDIF
      VTEMP(1) = VCUR(1,1) + PAS2*VCUR4
      VTEMP(2) = VCUR(2,1) + PAS2*VCUR5
      VTEMP(3) = VCUR(3,1) + PAS2*VCUR6
      GO TO 30
    ENDIF
    InvBBA = 1.d0 /BBA
    TX = BXYZA(1) * InvBBA
    TY = BXYZA(2) * InvBBA
    TZ = BXYZA(3) * InvBBA
    VX = VCUR5*BXYZA(3) - VCUR6*BXYZA(2)
    VY = VCUR6*BXYZA(1) - VCUR4*BXYZA(3)
    VZ = VCUR4*BXYZA(2) - VCUR5*BXYZA(1)
    VV = SQRT( VX**2 + VY**2 + VZ**2 )
    InvVV = 1.d0 /VV
    VX = VX * InvVV
    VY = VY * InvVV
    VZ = VZ * InvVV
    WX = TY*VZ - TZ*VY
    WY = TZ*VX - TX*VZ
    WZ = TX*VY - TY*VX
    CT = VV * InvBBA
    ST = VCUR4*TX + VCUR5*TY + VCUR6*TZ
    RR = VCUR(7,1) * CT * InvLight * InvBBA
20  AA = PAS2 * CT / RR
!>>--- aproximated sin and cos -------------
!** SA =  AA  - AA**3/6.D0 + AA**5/120.D0
!** CA = 1.D0 - AA**2/2.D0 + AA**4/24.D0
    A2 = AA**2
    SA = ( 1.d0 - A2/6.D0 ) * AA
    CA =   1.d0 - A2*0.5d0
!>>-----------------------------------------
    V = RR * (1.D0 - CA) * VCUR(8,1)
    W = RR * SA
    T = PAS2 * ST
    VTEMP(1) = VCUR(1,1) + V*VX + W*WX + T*TX
    VTEMP(2) = VCUR(2,1) + V*VY + W*WY + T*TY
    VTEMP(3) = VCUR(3,1) + V*VZ + W*WZ + T*TZ
!
!>> Compute the B field at the end of the preliminary step
30  CALL DFIELD(VTEMP,N312, BXYZB)
!
!>> Check wether the step (PAS) must be modified and, if yes,
!>> go back to perform another preliminary step
    BBAMB2 = (BXYZA(1)-BXYZB(1))**2 + (BXYZA(2)-BXYZB(2))**2 + (BXYZA(3)-BXYZB(3))**2
    IF( BBAMB2 > EB2MAX ) THEN
      IF( PAS > PASMIN ) THEN
        PAS  = PAS *0.5d0
        PAS2 = PAS *0.5d0 
        INCREA = 0
        IF( BBA < BBNUL ) THEN
          VTEMP(1) = VCUR(1,1) + PAS2*VCUR4
          VTEMP(2) = VCUR(2,1) + PAS2*VCUR5
          VTEMP(3) = VCUR(3,1) + PAS2*VCUR6
          GO TO 30
        ELSE
          GO TO 20
        ENDIF
      ENDIF
    ELSE IF( BBAMB2 < EB2MIN ) THEN
      IF( INCREA >= 1 ) THEN
        PAS2 = PAS
        PAS  = 2.D0 * PAS
        IF( PASTOT+PAS >= PAPAS ) THEN
          PAS  = PAPAS-PASTOT
          PAS2 = PAS *0.5d0
          INCREA = 0
        ENDIF
        IF( BBA < BBNUL ) THEN
          VTEMP(1) = VCUR(1,1) + PAS2*VCUR4
          VTEMP(2) = VCUR(2,1) + PAS2*VCUR5
          VTEMP(3) = VCUR(3,1) + PAS2*VCUR6
          GO TO 30
        ELSE
          GO TO 20
        ENDIF
      ENDIF
    ENDIF
!
!>> Perform actual step with B field taken at (roughly) half step
    PASTOT = PASTOT + PAS
    IF( PASTOT < 0.9999D0*PAPAS ) THEN
      CALL DOSTEPHC(VCUR,NVIN,BXYZB,PAS, VCUR,IBNUL, BBA, VX,VY,VZ, WX,WY,WZ, TX,TY,TZ, CT,ST,RR )
      BXYZA(1:3) = BXYZB(1:3)
      IF( IBNUL >= 1 ) THEN
        PAS2 = PAS
        PAS  = 2.D0*PAS
      ENDIF
      Pprevious = PAS
      IF( PASTOT+PAS >= PAPAS ) THEN
        PAS  = PAPAS-PASTOT
        PAS2 = PAS *0.5d0
        INCREA = 0
      ELSE
        INCREA = 1
      ENDIF
      IF( BBA < BBNUL ) THEN
        VTEMP(1) = VCUR(1,1) + PAS2*VCUR4
        VTEMP(2) = VCUR(2,1) + PAS2*VCUR5
        VTEMP(3) = VCUR(3,1) + PAS2*VCUR6
        GO TO 30
      ELSE
        GO TO 20
      ENDIF
    ELSE
      IF( Pprevious < EpsPas ) Pprevious = PAS
      Bprevious(1:3) = BXYZB(1:3)
      CALL DOSTEPHC(VCUR,NVIN,BXYZB,PAS, VOUT,IBNUL, BBA, VX,VY,VZ, WX,WY,WZ, TX,TY,TZ, CT,ST,RR )
      BBprevious = BBA
    ENDIF
!>>---------------------------------------------------------------------
!
    Xprevious(1:3) = VOUT(1:3,1)
!
 END SUBROUTINE ELEMHC
!
