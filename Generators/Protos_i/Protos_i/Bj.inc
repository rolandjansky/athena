1063  IF (IHRD .EQ. 63) THEN
        NUP=12
        NJET=1
      ELSE
        NUP=13
        NJET=2
      ENDIF

      READ (lun,*,END=1263) nn1,XWGTUP,SCALUP
      ISIGNWT=1
      IF (XWGTUP .LT. 0) ISIGNWT=-1

!     Initial codes and initial partons

      READ (lun,*) IDB1
      IF (IDB1 .EQ. 23) NUP=NUP-4
      IF (IDB1 .EQ. 25) NUP=NUP-6

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

!     Set pointers for first heavy quark

      id_HQ=3
      id_b=4
      IF (ABS(IDB1) .EQ. 24) THEN
        id_Qt=id_b+4
        num_Qt=6
      ELSE
        id_Qt=id_HQ
        num_Qt=7
      ENDIF

!     b quark

      i=id_b
      ISTUP(i)=1
      MOTHUP(1,i)=id_Qt
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

      IF (IDB1 .NE. 25) THEN

!     W or Z decay products

      i0=i+3
      id_VB=i0
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

!     W or Z

      i=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        IDUP(i)=23
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=id_Qt
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

      ELSE

!     Read H

      i0=i+1
      i=i0
      id_VB=i0
      ISTUP(i)=1
      MOTHUP(1,i)=id_HQ
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ABS(IDUP(i)) .EQ. 25) mf=MH
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

      ENDIF

!     B or t

      i=id_Qt
      ISTUP(i)=2
      IDUP(i)=SIGN(num_Qt,IDUP(id_b))
      
      IF (ABS(IDB1) .EQ. 24) THEN
        MOTHUP(1,i)=id_HQ
      ELSE
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ENDIF
      DO j=1,4
        PUP(j,i)=PUP(j,id_VB)+PUP(j,id_b)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_b)
      ICOLUP(2,i)=ICOLUP(2,id_b)

!     --------------------
!     Extra bosons, if any
!     --------------------

      IF (ABS(IDB1) .EQ. 24) THEN

!     Read one more W

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

      i=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        IDUP(i)=23
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=id_HQ
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     B

      id_VB=i0
      i=id_HQ
      ISTUP(i)=2
      IDUP(i)=SIGN(7,IDUP(id_Qt))
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_VB)+PUP(j,id_Qt)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_Qt)
      ICOLUP(2,i)=ICOLUP(2,id_Qt)
      
      ENDIF

!     ===============================================

      DO i=i0+1,i0+NJET
      ISTUP(i)=1
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)
      ENDDO

      RETURN

1263  PRINT 10002,lun
      NUP = 0
      RETURN

