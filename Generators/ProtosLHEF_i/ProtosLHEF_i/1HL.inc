1037  NUP=9

      READ (lun,*,END=1237) nn1,XWGTUP,SCALUP
      ISIGNWT=1
      IF (XWGTUP .LT. 0) ISIGNWT=-1

!     Initial codes and initial partons

      READ (lun,*) IDB1
      IF (IDB1 .EQ. 25) NUP=NUP-2
      IF (ABS(IDB1) .EQ. 34) NUP=NUP-1
      IF (ABS(IDB1) .EQ. 134) NUP=NUP+3

      i=1
      ISTUP(i)=-1
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),Pz
      PUP(3,i)=Pz
      PUP(4,i)=ABS(Pz)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)

      i=2
      ISTUP(i)=-1
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),Pz
      PUP(3,i)=Pz
      PUP(4,i)=ABS(Pz)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)

!     l2: Charged lepton from W/W' decay

      i=3
      id_L2=i
      ISTUP(i)=1
      IF (IHRD .EQ. 38) THEN
        MOTHUP(1,i)=NUP
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 15) mf=mtau
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      CHL1=-SIGN(1,IDUP(i))

!     Charged lepton from L1 decay

      i=i+1
      ISTUP(i)=1
      MOTHUP(1,i)=i+4
      IF (IDB1 .EQ. 25) MOTHUP(1,i)=i+2
      IF (ABS(IDB1) .EQ. 34) MOTHUP(1,i)=i+3
      IF (ABS(IDB1) .EQ. 134) MOTHUP(1,i)=i+7
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 15) mf=mtau
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

      id_l=i
      CHL=-SIGN(1,IDUP(i))
      IF ( (ABS(IDUP(i)) .EQ. 12) .OR. (ABS(IDUP(i)) .EQ. 14)
     &  .OR. (ABS(IDUP(i)) .EQ. 16) ) CHL=0 

!     N decay products and N

      IF ((ABS(IDB1) .EQ. 24) .OR. (IDB1 .EQ. 23)) THEN

!     --------
!     B1 = W,Z
!     --------

      i0=i+3
      DO i=i0-2,i0-1
      ISTUP(i)=1
      MOTHUP(1,i)=i0
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 15) mf=mtau
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      ENDDO

!     B1

      i=i0
      id_VB=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        IDUP(i)=23
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=i+1
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     L1

      i=id_VB+1
      id_L1=i
      ISTUP(i)=2
      CHL2 = CHVB+CHL
      IF (CHL2 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        PRINT *,'Wrong neutrino charge'
        STOP
      ENDIF
      IF (IHRD .EQ. 38) THEN
        MOTHUP(1,i)=NUP
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     ------
!     B1 = H
!     ------

      ELSE IF (IDB1 .EQ. 25) THEN

      i=i+1
      id_VB=i
      CHVB=0
      ISTUP(i)=1
      MOTHUP(1,i)=i+1
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 25) mf=MH
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

!     L1

      i=id_VB+1
      id_L1=i
      ISTUP(i)=2
      CHL2 = CHVB+CHL
      IF (CHL2 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        PRINT *,'Wrong neutrino charge'
        STOP
      ENDIF
      IF (IHRD .EQ. 38) THEN
        MOTHUP(1,i)=NUP
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     -------------
!     B1 = W' -> jj
!     -------------

      ELSE IF (ABS(IDB1) .EQ. 34) THEN

      i0=i+3
      id_f1=i0-2
      id_f2=i0-1
      DO i=i0-2,i0-1
      ISTUP(i)=1
      MOTHUP(1,i)=i0
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      ENDDO
      CHVB = IDUP(id_f1)+IDUP(id_f2)

!     L1

      i=i0
      id_L1=i
      ISTUP(i)=2
      CHL2 = CHVB+CHL
      IF (CHL2 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        PRINT *,'Wrong neutrino charge'
        STOP
      ENDIF
      IF (IHRD .EQ. 38) THEN
        MOTHUP(1,i)=NUP
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     -------------
!     B1 = W' -> tb
!     -------------

      ELSE

      i0=i+7

!     b from N

      i=i+1
      id_b1=i
      ISTUP(i)=1
      MOTHUP(1,i)=i0
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

!     b from t

      i=i+1
      id_b2=i
      ISTUP(i)=1
      MOTHUP(1,i)=i0-1
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

!     W decay products

      DO i=i0-4,i0-3
      ISTUP(i)=1
      MOTHUP(1,i)=i0-2
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 15) mf=mtau
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      ENDDO

!     W

      i=i0-2
      id_VB=i
      id_f1=i0-4
      id_f2=i0-3
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        PRINT *,'Wrong W charge'
        STOP
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=i+1
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     t

      i=i+1
      ISTUP(i)=2
      MOTHUP(1,i)=i+1
      ICOLUP(1,i)=ICOLUP(1,id_b2)
      ICOLUP(2,i)=ICOLUP(2,id_b2)
      IDUP(i)=SIGN(6,IDUP(id_b2))
      DO j=1,4
        PUP(j,i)=PUP(j,id_b2)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     L1

      i=i0
      id_L1=i
      ISTUP(i)=2
      CHL2 = CHL+IDUP(i-4)+IDUP(i-3)
      IF (CHL2 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        PRINT *,'Wrong neutrino charge'
        STOP
      ENDIF
      IF (IHRD .EQ. 38) THEN
        MOTHUP(1,i)=NUP
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_b1)+PUP(j,id_b2)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

      ENDIF

!     Check

      IF ((IDUP(id_VB) .NE. IDB1) .AND. (ABS(IDB1) .LT. 34)) THEN
        PRINT *,'Error in B1'
        STOP
      ENDIF

      IF (IHRD .NE. 38) RETURN

!     Finally, resonance

      i=id_L1+1
      IF (i .NE. NUP) THEN
        PRINT *,'Reconstruction error'
        STOP
      ENDIF
      ISTUP(i)=2
      CHVB = CHL1+CHL2
      IF (CHVB .EQ. 0) THEN
        IDUP(i)=32
      ELSE
        IDUP(i)=SIGN(34,CHVB)
      ENDIF
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_L1)+PUP(j,id_L2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

      RETURN

1237  PRINT 10002,lun
      NUP = 0
      RETURN

