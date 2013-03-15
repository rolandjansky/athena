*
*
*
*

* Author : Saclay Muon Software Group (SaMuSog)
************************************************************************
      SUBROUTINE RTESTH8(LUN,VERTEMP,NXXH,NYYH,NZZH,DDX,DDY,DDZ,
     + XMI,XMA,YMI,YMA,ZMI,ZMA,XCPOS,YCPOS,ZCPOS,TACBX,TACBY,TACBZ)
*
      IMPLICIT NONE
*
      INTEGER LUN, NXXH(3),NYYH(3),NZZH(3)
      REAL    DDX(3),DDY(3),DDZ(3)
      REAL    XMI(3),XMA(3),YMI(3),YMA(3),ZMI(3),ZMA(3)
      REAL    XCPOS(*),YCPOS(*),ZCPOS(*), TACBX(*),TACBY(*),TACBZ(*)
      CHARACTER*4 VERTEMP
*  
      CHARACTER*118 CAR118
      CHARACTER*6   A6
      REAL*8  XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX
      REAL*8  PASX,PASY,PASZ,DX,DY,DZ,COR
      REAL*8  X,Y,Z,BX,BY,BZ
      REAl    PRECI
      INTEGER NX,NY,NZ, NXPRE,NYPRE,NZPRE, NSTEP, I, LX,LY,LZ
      INTEGER IX,IY,IZ, LXYZ, IXMAX,IYMAX,IZMAX, LMAX, L2X,L2Y,L2Z
      INTEGER IPRINT,ILC,ICOMP,LXYZM,NTAB,IMAGN
      DATA PRECI / 0.0001 /
      DATA ILC   / 1      /
      DATA IMAGN / 0      /
      DATA ICOMP / 0      /
      DATA LXYZM / 0      /
      DATA IPRINT/ 0      /
*
      READ (LUN,4040,END=100) CAR118
      WRITE(6,*)'===> Infos B-field map construction'
      WRITE(6,4040)CAR118
      READ(VERTEMP(4:4),'(I1)')NTAB
      WRITE(6,*)'===> Number of Bfield map: ',ntab
*
 15   READ (LUN,4040,END=100) CAR118
      READ(CAR118,4051)A6,NX,NY,NZ,XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX,
     +                 DX,DY,DZ,COR
      LXYZM= LXYZM + NX*NY*NZ + 1
      ICOMP= ICOMP + 1
      XMAX = XMAX*100.
      YMAX = YMAX*100.
      ZMAX = ZMAX*100.
      XMIN = XMIN*100.
      YMIN = YMIN*100.
      ZMIN = ZMIN*100.
      PASX = (XMAX-XMIN)/FLOAT(NX-1) 
      PASY = (YMAX-YMIN)/FLOAT(NY-1) 
      PASZ = (ZMAX-ZMIN)/FLOAT(NZ-1) 
      IMAGN= IMAGN + 1
      NXXH(IMAGN)= NX
      NYYH(IMAGN)= NY
      NZZH(IMAGN)= NZ
      IXMAX=0
      IYMAX=0
      IZMAX=0
      LMAX =0
      IF(IMAGN.EQ.1) THEN
        NXPRE = 0
        NYPRE = 0
        NZPRE = 0
        NSTEP = 0
      ELSE
        NXPRE = NXPRE + NXXH(IMAGN-1)
        NYPRE = NYPRE + NYYH(IMAGN-1)
        NZPRE = NZPRE + NZZH(IMAGN-1)
        NSTEP = NSTEP + NXXH(IMAGN-1)*NYYH(IMAGN-1)*NZZH(IMAGN-1)
      ENDIF
      XMI(IMAGN) = XMIN+DX*100.
      YMI(IMAGN) = YMIN+DY*100.
      ZMI(IMAGN) = ZMIN+DZ*100.
      XMA(IMAGN) = XMAX+DX*100.
      YMA(IMAGN) = YMAX+DY*100.
      ZMA(IMAGN) = ZMAX+DZ*100.
      DDX(IMAGN) = DX*100. 
      DDY(IMAGN) = DY*100.
      DDZ(IMAGN) = DZ*100.
      write(6,*)'===> H8 data (H8): ',A6,'===> MPB#:',IMAGN
      write(6,*)'===> Max size of TAB ',LXYZM,NX,NY,NZ
      write(6,'(9f8.2)') XMIN,XMAX,YMIN,YMAX,ZMIN,ZMAX,
     +                   DDX(IMAGN),DDY(IMAGN),DDZ(IMAGN)
      write(6,'(6f8.2,3i4)') XMI(IMAGN),XMA(IMAGN),YMI(IMAGN),YMA(IMAGN)
     +        ,ZMI(IMAGN),ZMA(IMAGN),NXXH(IMAGN),NYYH(IMAGN),NZZH(IMAGN)
 10   CONTINUE
      ICOMP = ICOMP + 1
      READ(LUN,*,END=100)X,Y,Z,BX,BY,BZ
      IX   = INT((X-XMIN)/PASX+ 0.5) + 1
      IY   = INT((Y-YMIN)/PASY+ 0.5) + 1
      IZ   = INT((Z-ZMIN)/PASZ+ 0.5) + 1
      LXYZ = IX + NX*(IY-1) + NX*NY*(IZ-1) + NSTEP
      IF(IXMAX.LE.IX)   IXMAX=IX
      IF(IYMAX.LE.IY)   IYMAX=IY
      IF(IZMAX.LE.IZ)   IZMAX=IZ
      IF(LMAX .LE.LXYZ) LMAX =LXYZ
*
      if(icomp.gt.lxyzm-1) then
        write(6,'(4i10)')ilc,imagn,icomp,lxyzm
        write(6,'(3i10)')lxyz,lmax,NSTEP
        write(6,'(4i10)')NX*NY*NZ,NX,NY,NZ
        write(6,'(4i10)')NXPRE*NYPRE*NZPRE,NXPRE,NYPRE,NZPRE
        write(6,'(6i10)')ix,iy,iz,ixmax,iymax,izmax
        write(6,'(3f10.2,3f10.4)')x,y,z,bx,by,bz
      endif
*
      IF(ICOMP .EQ. LXYZM .AND. ILC.LT.4) THEN
*
        if(iprint.eq.1) then
          write(6,*)'***> avant ilc,icomp,lxyzm ',ilc,icomp,lxyzm
          write(6,'(3i12)')ixmax,iymax,izmax
          write(6,'(4f12.2)') xmax,xcpos(IXMAX+NXPRE),
     +                        xmin,xcpos(NXPRE+1)
          write(6,'(4f12.2)') ymax,ycpos(IYMAX+NYPRE),
     +                        ymin,ycpos(NYPRE+1)
          write(6,'(4f12.2)') zmax,zcpos(IZMAX+NZPRE),
     +                        zmin,zcpos(NZPRE+1)
        endif
*
        IF( XMAX-XCPOS(IXMAX+NXPRE) .GT.PRECI.OR.
     +      YMAX-YCPOS(IYMAX+NYPRE) .GT.PRECI.OR.
     +      ZMAX-ZCPOS(IZMAX+NZPRE) .GT.PRECI.OR.
     +      XMIN-XCPOS(NXPRE+1).GT.PRECI.OR.
     +      YMIN-YCPOS(NYPRE+1).GT.PRECI.OR.
     +      ZMIN-ZCPOS(NZPRE+1).GT.PRECI    ) THEN
          WRITE(6,*)' ******** ATTENTION WRONG DTB H8 ***********'
          WRITE(6,'(4F15.2)') XMAX,XCPOS(IXMAX+NXPRE),
     +                        XMIN,XCPOS(NXPRE+1)
          WRITE(6,'(4F15.2)') YMAX,YCPOS(IYMAX+NYPRE),
     +                        YMIN,YCPOS(NYPRE+1)
          WRITE(6,'(4F15.2)') ZMAX,ZCPOS(IZMAX+NZPRE),
     +                        ZMIN,ZCPOS(NZPRE+1)
          STOP
        ENDIF
*
        ILC        = ILC + 1
*
        if(iprint.eq.1) then
          write(6,*)'apres ilc,imagn,icomp,lxyzm ',ilc,imagn,icomp,lxyzm
          write(6,'(3i12)')ixmax,iymax,izmax
          write(6,'(3i12)')NXXH(IMAGN),NYYH(IMAGN),NZZH(IMAGN)
        endif
*
*****   IF(ILC.LT.4) THEN
        IF(ILC.LT.NTAB+1) THEN
          GOTO 15
        ELSE
          GOTO 100
        ENDIF
      ENDIF
      L2X = IX + NXPRE
      L2Y = IY + NYPRE
      L2Z = IZ + NZPRE     
***** WRITE(79,'(10I8)')LXYZ,L2X,L2Y,L2Z,NXPRE,NYPRE,NZPRE,NX,NY,NZ
*  CORrection due to the bad results of calculation w.r.t measurment
      XCPOS(L2X)  = X
      YCPOS(L2Y)  = y
      ZCPOS(L2Z)  = Z
      TACBX(LXYZ) = BX*COR*(-1.)
      TACBY(LXYZ) = BY*COR*(-1.)
      TACBZ(LXYZ) = BZ*COR*(-1.)
      GOTO 10
 100  CONTINUE
*      
*  permet a findmaph8 de ne pas se tromper si 1 map
      if(imagn.lt.2) then
        xma(2) = 9999999
        write(6,*)' xma(2) ',xma(2)
      endif
*     iprint = 1
      IF(IPRINT.EQ.1) THEN
        write(6,*)'***> APRES continue 100 '
        write(6,*)' passage ilc    ',ilc
        write(6,*)' nval    icomp  ',icomp
        write(6,*)' nval    lmax   ',lmax
        write(6,*)' nval    lxyzm  ',lxyzm
        write(6,*)' nb        map  ',imagn
        write(6,*)' max size map   '
        do i=1,imagn
          write(6,'(3i12)')NXXH(I),NYYH(I),NZZH(I)
        enddo
        open( unit=77, file='ecriture.dat', status='new')
        DO I=1,IMAGN
          IF(I.EQ.1) THEN
            NXPRE = 0
            NYPRE = 0
            NZPRE = 0
            NSTEP = 0
          ELSE
            NXPRE = NXPRE + NXXH(I-1)
            NYPRE = NYPRE + NYYH(I-1)
            NZPRE = NZPRE + NZZH(I-1)
            NSTEP = NSTEP + NXXH(I-1)*NYYH(I-1)*NZZH(I-1)
          ENDIF
          write(6,*)I,NSTEP
          WRITE(77,'("MaxSize",5I6)')I,NXXH(I),NYYH(I),NZZH(I),NSTEP
          WRITE(77,4070) (XCPOS(LX + NXPRE),LX=1,NXXH(I))
          WRITE(77,4070) (YCPOS(LY + NYPRE),LY=1,NYYH(I))
          WRITE(77,4070) (ZCPOS(LZ + NZPRE),LZ=1,NZZH(I))
          DO LZ=1,NZZH(I)
            DO LY=1,NYYH(I)
              DO LX=1,NXXH(I)
                L2X  = LX   + NXPRE
                L2Y  = LY   + NYPRE
                L2Z  = LZ   + NZPRE
                LXYZ = LX + NXXH(I)*(LY-1) 
     +               + NXXH(I)*NYYH(I)*(LZ-1) + NSTEP
                if(lx.gt.1      .and.ly.gt.1      .and.lz.gt.1  .and.
     +             lx.lt.NXXH(I).and.ly.lt.NYYH(I).and.lz.lt.NZZH(I))
     +            WRITE(77,4071)XCPOS(L2X),YCPOS(L2Y),ZCPOS(L2Z),
     +                          TACBX(LXYZ),TACBY(LXYZ),TACBZ(LXYZ)
              ENDDO
            ENDDO
          ENDDO
        ENDDO
        CLOSE(77)
      ENDIF
*
 4040 FORMAT(A118)
 4051 FORMAT(A6,3I6,9F8.2,F8.4)
 4070 FORMAT(6F12.2)
 4071 FORMAT(6F12.4)
*
      return
      end
