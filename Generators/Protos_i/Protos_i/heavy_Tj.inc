1061  IF (IHRD .EQ. 61) THEN
        NUP=8
        NJET=1
      ELSE IF (IHRD .EQ. 62) THEN
        NUP=9
        NJET=2
      ELSE IF (IHRD .EQ. 67) THEN
        NUP=13
        NJET=1
      ENDIF

      READ (lun,*,END=1261) nn1,XWGTUP,SCALUP
      ISIGNWT=1
      IF (XWGTUP .LT. 0) ISIGNWT=-1

!     Initial codes and initial partons

      READ (lun,*) IDB1
      IF (IDB1 .EQ. 23) NUP=NUP+4
      IF (IDB1 .EQ. 25) NUP=NUP+2

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
        id_Qt=id_HQ
        num_Qt=8
      ELSE
        id_Qt=id_b+4
        num_Qt=6
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

!     W+ decay products

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

!     W+

      i=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        PRINT *,'No Z here!!'
        STOP
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=id_Qt
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     T or t

      i=id_Qt
      ISTUP(i)=2
      IDUP(i)=SIGN(num_Qt,IDUP(id_b))
      
      IF (ABS(IDB1) .EQ. 24) THEN
        MOTHUP(1,i)=1
        MOTHUP(2,i)=2
      ELSE
        MOTHUP(1,i)=id_HQ
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

      IF (IDB1 .EQ. 23) THEN 

!     Read Z

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
        PRINT *,'No W here!!'
        STOP
      ENDIF
      MOTHUP(1,i)=id_HQ
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     T

      id_VB=i0
      i=id_HQ
      ISTUP(i)=2
      IDUP(i)=SIGN(8,IDUP(id_Qt))
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_VB)+PUP(j,id_Qt)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_Qt)
      ICOLUP(2,i)=ICOLUP(2,id_Qt)

      ELSE IF (IDB1 .EQ. 25) THEN

!     Read H

      i0=i+1
      i=i0
      ISTUP(i)=1
      MOTHUP(1,i)=id_HQ
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ABS(IDUP(i)) .EQ. 25) mf=MH
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

!     T

      id_VB=i0
      i=id_HQ
      ISTUP(i)=2
      IDUP(i)=SIGN(8,IDUP(id_Qt))
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

!     Additional particles: jet, b, top

      IF (IHRD .EQ. 67) THEN

      id_b=i0+1
      id_Qt=i0+5
      num_Qt=6

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

!     W decay products

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

!     W

      i=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        PRINT *,'No Z here!!'
        STOP
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=id_Qt
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     t or tbar

      i=id_Qt
      i0=id_Qt
      ISTUP(i)=2
      IDUP(i)=SIGN(num_Qt,IDUP(id_b))
      
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_VB)+PUP(j,id_b)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_b)
      ICOLUP(2,i)=ICOLUP(2,id_b)

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


1261  PRINT 10002,lun
      NUP = 0
      RETURN

