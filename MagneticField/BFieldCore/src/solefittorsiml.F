*
*
*
*

* Author : Paul S Miyagawa
***************************************************************************
      SUBROUTINE solefittorsiml(AVER,RFZSC,II312, BXYZ,
     +NVERSION,BINNER,RMAINN,ZMAINN,RTORI,ZTORI,RTORO,ZTORO,EPS0,
     +SECMAG,NRMAG,NFMAG,NZMAG,RMAG ,FMAG ,ZMAG ,BRMAG,BFMAG,BZMAG,
     +SECFEL,NRR  ,NFF  ,NZZ  ,RRPOS,FFPOS,ZZPOS,TABBR,TABBF,TABBZ,
     +DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +RSOL,ZSOL,INFOBMAG)
*
*>> Interpolates tabulated nonlinear 3D ATLAS mag. field BXYZ !
*>> Author : Paul S Miyagawa
*>> Date   : 06/12/07
*
      IMPLICIT NONE
*
*
      CHARACTER*4 AVER
      INTEGER II312, NVERSION, NRMAG(*),NFMAG,NZMAG, NRR,NFF,NZZ
      INTEGER JRPOS(*),JFPOS(*),JZPOS(*), JRMAG(*),JFMAG(*),JZMAG(*) 
      REAL    RFZSC(6), BXYZ(12), BINNER, RMAINN, ZMAINN
      REAL    RTORI, ZTORI, RTORO, ZTORO, EPS0, SECMAG, SECFEL
      REAL    RMAG(*),FMAG(*),ZMAG(*), BRMAG(*),BFMAG(*),BZMAG(*)
      REAL    RRPOS(*),FFPOS(*),ZZPOS(*), TABBR(*),TABBF(*),TABBZ(*)
      REAL    DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI
      REAL    DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI
      REAL    RSOL,ZSOL
      REAL    SINF,COSF
*
      INTEGER I,INFOBMAG
      REAL    RR,AZ
      REAL    BXYZBS(12)
      real    bgrad(9),bcart(9)
*
      real(8), parameter :: zero=0.
      real(8), parameter :: pi=3.141592654
      real(8), parameter :: deg=180./pi
*
      real zi,ri,fi,bzo,bro,bfo,azi,r1,z1,r2,z2,
     +     bzo1,bzo2,bro1,bro2,bfo1,bfo2,dr1,dz1,s1,s2
      data r1,z1,r2,z2 /1.075,2.820,1.085,2.830/
      integer, parameter :: ig=2
*
      RR = RFZSC(1)
      AZ = RFZSC(6)      
*
* Convert cm to m for solenoid calculations
      ri  = rfzsc(1) *0.01
      azi = rfzsc(6) *0.01

* Use solenoid alone
c      IF( RR.LT.RSOL .AND. AZ.LT.ZSOL ) THEN
      if( ri.lt.r1 .and. azi.lt.z1 ) then
         zi  = rfzsc(3) *0.01
         fi  = rfzsc(2)
c        write(6,*) " call solenoid interpolation "
* Check whether field or fieldGradient
        if( ii312 .eq. 3 ) then
          call bsolinterp(ig,zi,ri,fi,bzo,bro,bfo)
        elseif( ii312 .eq. 12 ) then
          call bsolinterpg(ig,zi,ri,fi,bzo,bro,bfo,bgrad)
          call bsolgtocart(ig,zi,ri,fi,bzo,bro,bfo,bgrad,bcart)
* Convert G/m to T/cm (divide by 10000*100)
          do 14 i = 1, 9
            bxyz(i+3) = bcart(i) *0.000001
14        continue
        endif
        SINF    = SIN(RFZSC(2))
        COSF    = COS(RFZSC(2))
        BXYZ(1) = (BRO*COSF - BFO*SINF) * 0.0001
        BXYZ(2) = (BRO*SINF + BFO*COSF) * 0.0001
        BXYZ(3) = BZO * 0.0001

* Use mixture of solenoid interpolation and Tilecal
      elseif( ri.lt.r2 .and. azi.lt.z2 )then
         zi  = rfzsc(3) *0.01
         fi  = rfzsc(2) 
c        write(6,*) " call mixture of solenoid interpolation and Tilecal"
* Check whether field or fieldGradient
        if( ii312 .eq. 3 ) then
          call bsolinterp(ig,zi,ri,fi,bzo1,bro1,bfo1)
        elseif( ii312 .eq. 12 ) then
          call bsolinterpg(ig,zi,ri,fi,bzo1,bro1,bfo1,bgrad)
          call bsolgtocart(ig,zi,ri,fi,bzo1,bro1,bfo1,bgrad,bcart)
* Convert G/m to T/cm (divide by 10000*100)
          do 15 i = 1, 9
            bcart(i) = bcart(i) * 0.000001
15        continue
        endif
        bzo1 = bzo1 * 0.0001
        bro1 = bro1 * 0.0001
        bfo1 = bfo1 * 0.0001
        CALL BFELIXXX( SECFEL, NRR,NFF,NZZ, RRPOS,FFPOS,ZZPOS,
     +  TABBR,TABBF,TABBZ,
     +  DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +  DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +  RFZSC,II312, BXYZ )
        SINF    = SIN(RFZSC(2))
        COSF    = COS(RFZSC(2))
        bzo2 = bxyz(3)
        bro2 = bxyz(1)*cosf + bxyz(2)*sinf
        bfo2 = bxyz(2)*cosf - bxyz(1)*sinf
        dr1 = max(0.0,ri-r1)
        dz1 = max(0.0,azi-z1)
        s1  = sqrt( dr1*dr1 + dz1*dz1 )
        s2  = min(r2-ri,z2-azi)
        bzo = (bzo1*s2+bzo2*s1)/(s1+s2)
        bro = (bro1*s2+bro2*s1)/(s1+s2)
        bfo = (bfo1*s2+bfo2*s1)/(s1+s2)
        BXYZ(1) = BRO*COSF - BFO*SINF
        BXYZ(2) = BRO*SINF + BFO*COSF
        BXYZ(3) = BZO
        if( ii312 .eq. 12 ) then
          do 16 i = 1, 9
            bxyz(i+3) = (bcart(i)*s2 + bxyz(i+3)*s1) / (s1+s2)
16        continue
        endif

* Use Tilecal interpolation
      ELSEIF( RR.LT.RTORI .AND. AZ.LT.ZTORI ) THEN
c        write(6,*) " Tile region interpolation "
        CALL BFELIXXX( SECFEL, NRR,NFF,NZZ, RRPOS,FFPOS,ZZPOS,
     +  TABBR,TABBF,TABBZ,
     +  DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +  DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                  RFZSC,II312, BXYZ )
*
*	WRITE(96,'(3f9.1,6F9.4)')RFZSC(1),RFZSC(2)*deg,RFZSC(3),
*     + zero,zero,zero,BXYZ(1),BXYZ(2),BXYZ(3)
*

* Use spectrometer interpolation
      ELSE
c        write(6,*) " Spectrometer region interpolation "
        IF(INFOBMAG.LT.1) THEN
           CALL BINMAGFULL( SECMAG, NRMAG,NFMAG,NZMAG, RMAG,FMAG,ZMAG,
     +  BRMAG,BFMAG,BZMAG, 
     +  DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +  DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                  RFZSC,II312, BXYZBS)
C           WRITE(6,*)  " *******  BINMAGFULL  **********  ",INFOBMAG
        ELSE
           CALL BINMAGXX( SECMAG, NRMAG,NFMAG,NZMAG, RMAG,FMAG,ZMAG,
     +  BRMAG,BFMAG,BZMAG, 
     +  DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +  DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                  RFZSC,II312, BXYZBS)
C           WRITE(6,*)  " *******  BINMAGXX    **********  ",INFOBMAG
        ENDIF
c        
        CALL BFELIXXX( SECFEL, NRR,NFF,NZZ, RRPOS,FFPOS,ZZPOS,
     +  TABBR,TABBF,TABBZ,
     +  DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +  DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                  RFZSC,II312, BXYZ )
*
*      WRITE(96,'(3f9.1,6F9.4)')RFZSC(1),RFZSC(2)*deg,RFZSC(3),
*     + BXYZBS(1),BXYZBS(2),BXYZBS(3),BXYZ(1),BXYZ(2),BXYZ(3)
*
        DO I=1,II312
          BXYZ(I) = BXYZ(I) + BXYZBS(I)
        ENDDO
      ENDIF

*
      RETURN
      END

