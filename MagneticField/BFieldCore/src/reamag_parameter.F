*
*
*
*
 
* Author : Saclay Muon Software Group (SaMuSoG)
************************************************************************
      SUBROUTINE REAMAG_PARAMETER( LunB, Imag,Ifel,VERTEMP,
     +                             NFMAG,NZMAG,NRMAGmax,NRR,NFF,NZZ )
*
      IMPLICIT NONE
*
*
      INTEGER      LunB,Imag,Ifel, NFMAG,NZMAG,NRMAGmax, NRR,NFF,NZZ
      CHARACTER*4  VERTEMP
*
      CHARACTER*1  ATYPE
      CHARACTER*20 VERSION
      CHARACTER*80 Car80
      INTEGER      NRMAGloc(10), NF,NX,NXE, Ipass, Idummy1
      INTEGER      nmap, I, L, Imax
      REAL         Xdummy1,Xdummy2,Xdummy3,Xdummy4,Xdummy5,Xdummy6
      REAL         Xdummy7,Xdummy8
      DATA VERSION/'XXXXXXXX'/
*
      IF( Imag.GE.1 ) THEN
        READ (LunB,4000) VERSION
        REWIND LUNB
        READ (LunB,"(a80)") CAR80
	VERSION = CAR80(1:20)
        ATYPE   = VERSION(6:6)
        VERTEMP = VERSION(1:4)
        IF(ATYPE.EQ.'A' .OR. ATYPE.EQ.'B' .OR. ATYPE.EQ.'C' .OR.
     +     ATYPE.EQ.'D' .OR. ATYPE.EQ.'S') THEN
          PRINT 1000,ATYPE
1000      FORMAT(/'  Mag. field file of type ',A1,' cannot be read'
     +           ,' by routine REAMAG_PARAMETER  ======>  STOP !')
          STOP
        ELSEIF(VERSION(10:13).EQ.'Full') THEN
          write(6,*)'===> Layout version :',VERSION(5:8)
          write(6,*)'===> Map version    :',VERSION(10:13)
          VERTEMP = VERSION(10:13)
          REWIND LUNB
          RETURN
        ELSEIF(VERSION(1:2).EQ.'H8') THEN
          READ(VERSION(4:4),'(i1)')nmap
          write(6,*)'===> version in REAMAG_PARAMETER: ',VERSION(1:2)
          write(6,*)'===> Number of Bfield map:        ',nmap
          REWIND LUNB
          RETURN
        ELSEIF(VERSION(1:2).EQ.'XX') THEN
          write(6,*)' version in REAMAG_PARAMETER ',VERSION(1:2)
          REWIND LUNB
          RETURN
        ELSEIF(VERSION(6:6).EQ.'R') THEN
 	  VERTEMP = VERSION(5:8)
          write(6,*)' version in REAMAG_PARAMETER ',VERSION(1:8)
          write(6,*)' VERTEMP                     ',VERTEMP
          REWIND LUNB
          RETURN
        ELSEIF(VERSION(1:2).EQ.'IC') THEN
          write(6,*)' version in REAMAG_PARAMETER ',VERSION(1:2)
          REWIND LUNB
          RETURN
        ELSEIF( CAR80(41:41).EQ.'.' ) THEN
	  VERTEMP(1:1) = CAR80(41:41)
          write(6,*)' version in REAMAG_PARAMETER ',VERTEMP(1:1)
          REWIND LUNB
          RETURN
        ENDIF
        READ (LunB,4010) NF
        READ (LunB,4020) NX
        READ (LunB,4020) NXE
*
        Ipass = 1 + (NXE-1)/8
        DO I=1,Ipass
          READ(LunB,4030) Idummy1
        ENDDO
        DO I=1,Ipass
          READ (LunB,4040) Xdummy1
          READ (LunB,4040) Xdummy2
          READ (LunB,4040) Xdummy3
          READ (LunB,4040) Xdummy4
          READ (LunB,4040) Xdummy5
          READ (LunB,4040) Xdummy6
        ENDDO
*
        READ (LunB,4050) NFMAG,NZMAG
*
        Ipass = 1 + (NFMAG-1)/6
        DO I=1,Ipass
          READ(LunB,4060) Xdummy7
        ENDDO
*
        Ipass = 1 + (NZMAG-1)/6
        DO I=1,Ipass
          READ(LunB,4070) Xdummy8
        ENDDO
*
        Ipass = 1 + (NFMAG*NZMAG-1)/10
        DO I=1,Ipass
          READ(LunB,4090) (NRMAGloc(L),L=1,10)
        ENDDO
        Imax = NFMAG*NZMAG - 10*(Ipass-1)
        NRMAGmax = NRMAGloc(Imax)
*
4000    FORMAT(A8)
4010    FORMAT(I5,4F10.2)
4020    FORMAT(2I5,7F10.2)
4030    FORMAT(8I5)
4040    FORMAT(8F10.2)
4050    FORMAT(2I6,F15.9)
4060    FORMAT(6F12.9)
4070    FORMAT(6F12.4)
4090    FORMAT(10I8)
      ENDIF
*
      IF(ATYPE.EQ.'M') THEN
        REWIND LUNB
        RETURN
      ENDIF
*
      IF( Ifel.GE.1 ) THEN
100     READ(LunB,5000) Car80
        IF( Car80(1:1).NE.' ' ) GO TO 100
        READ(Car80,5050) NRR,NFF,NZZ
*
5000    FORMAT(A80)
5050    FORMAT(3I6,F15.9,4F11.3)
      ENDIF
*
*
       REWIND LUNB
*
      RETURN
      END

