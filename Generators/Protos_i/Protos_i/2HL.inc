1031  IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) THEN
        NUP=12
      ELSE
        NUP=13
      ENDIF

      READ (lun,*,END=1231) nn1,XWGTUP,SCALUP
      ISIGNWT=1
      IF (XWGTUP .LT. 0) ISIGNWT=-1

!     Initial codes and initial partons

      READ (lun,*) IDB1,IDB2
      IF (IDB1 .EQ. 25) NUP=NUP-2
      IF (IDB2 .EQ. 25) NUP=NUP-2

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

!     Charged lepton from L1 decay

      i=3
      ISTUP(i)=1
      MOTHUP(1,i)=i+4
      IF (IDB1 .EQ. 25) MOTHUP(1,i)=i+2
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

!     B1 decay products (if any) and B1

      IF (IDB1 .NE. 25) THEN     ! B1 = W,Z, decay products first

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

      ELSE                      !  B1 = H, no decay

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

      ENDIF

!     Check

      IF (IDUP(id_VB) .NE. IDB1) THEN
        PRINT *,'Error in B2'
        STOP
      ENDIF

!     L1

      i=id_VB+1
      id_L1=i
      ISTUP(i)=2
      CHL1 = CHVB+CHL
      IF (CHL1 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        IDUP(i)=-SIGN(17,CHL1)
      ENDIF
      IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) THEN
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ELSE
        MOTHUP(1,i)=NUP
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     Charged lepton from L2 decay

      i=i+1
      ISTUP(i)=1
      MOTHUP(1,i)=i+4
      IF (IDB2 .EQ. 25) MOTHUP(1,i)=i+2
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

!     B2 decay products (if any) and B2

      IF (IDB2 .NE. 25) THEN     ! B2 = W,Z, decay products first

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
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      ENDDO

!     B2

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

      ELSE                      !  B1 = H, no decay

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

      ENDIF

!     Check

      IF (IDUP(id_VB) .NE. IDB2) THEN
        PRINT *,'Error in B2'
        STOP
      ENDIF

!     L2

      i=id_VB+1
      id_L2=i
      ISTUP(i)=2
      CHL2 = CHVB+CHL
      IF (CHL2 .EQ. 0) THEN
        IDUP(i)=SIGN(18,IDUP(id_l))
      ELSE
        IDUP(i)=-SIGN(17,CHL2)
      ENDIF
      IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) THEN
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ELSE
        MOTHUP(1,i)=NUP
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_l)+PUP(j,id_VB)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

      IF ((IHRD .GE. 31) .AND. (IHRD .LE. 33)) RETURN

!     Resonance

      i=id_L2+1
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

1231  PRINT 10002,lun
      NUP = 0
      RETURN

