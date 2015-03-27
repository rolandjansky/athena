1017  NUP=11

      READ (lun,*,END=1217) nn1,XWGTUP,SCALUP
      ISIGNWT=1
      IF (XWGTUP .LT. 0) ISIGNWT=-1

!     Initial partons

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

!     top with SM decay

      i0=5
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

!     W+ / W-

      i=i0
      id_f1=i0-2
      id_f2=i0-1
      ISTUP(i)=2
      CHVB = IDUP(id_f1)+IDUP(id_f2)
      IF (CHVB .EQ. 0) THEN
        PRINT *,'Should be W'
        STOP
      ELSE
        IDUP(i)=SIGN(24,CHVB)
      ENDIF
      MOTHUP(1,i)=i+2
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)

!     b

      i=i0+1
      ISTUP(i)=1
      MOTHUP(1,i)=i+1
      mf=0d0
      READ (lun,*) IDUP(i),ICOLUP(1,i),ICOLUP(2,i),
     &    PUP(1,i),PUP(2,i),PUP(3,i)
      IF (ICOLUP(1,i) .NE. 0) ICOLUP(1,i)=500+ICOLUP(1,i)
      IF (ICOLUP(2,i) .NE. 0) ICOLUP(2,i)=500+ICOLUP(2,i)
      IF (ABS(IDUP(i)) .EQ. 5) mf=mb
      PUP(5,i)=mf
      PUP(4,i)=SQRT(PUP(5,i)**2+PUP(1,i)**2+PUP(2,i)**2+PUP(3,i)**2)

!     t

      i=i+1
      id_VB=i-2
      id_q=i-1
      ISTUP(i)=2
      IDUP(i)=SIGN(6,IDUP(id_q))
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_VB)+PUP(j,id_q)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_q)
      ICOLUP(2,i)=ICOLUP(2,id_q)

!     top with 3-body decay

      i0=i+4
      DO i=i0-3,i0-1
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

!     tbar

      i=i0
      id_f1=i0-3
      id_f2=i0-2
      id_q=i0-1
      ISTUP(i)=2
      IDUP(i)=SIGN(6,IDUP(id_q))
      MOTHUP(1,i)=1
      MOTHUP(2,i)=2
      DO j=1,4
        PUP(j,i)=PUP(j,id_f1)+PUP(j,id_f2)+PUP(j,id_q)
      ENDDO
      PUP(5,i)=SQRT(PUP(4,i)**2-PUP(1,i)**2-PUP(2,i)**2-PUP(3,i)**2)
      ICOLUP(1,i)=ICOLUP(1,id_q)
      ICOLUP(2,i)=ICOLUP(2,id_q)
      
      RETURN

1217  PRINT 10002,lun
      NUP = 0
      RETURN

