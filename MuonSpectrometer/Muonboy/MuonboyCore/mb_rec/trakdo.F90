!
!> Track fit using segments (used when ItypFit = 2 or 5)
!> \author M.Virchaux   19/05/95
!
 SUBROUTINE TRAKDO(NVDEB,IFLAG)
 USE M_MB_Digis
 USE M_MB_MuGeom
 USE M_MB_CTRABAZ
 USE M_MB_CODRAT
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NVDEB, IFLAG
 INTEGER, SAVE :: IZTPLA, NN, ISC, ID, J, IW, IC, IWP, INOF, INOZ, IVDEB
 INTEGER, SAVE :: IGEOM, IMDT, ILIN, IFILI, IW01, ICT, IOPT, Itrans
 REAL(8), SAVE :: RDEB, ZDEB, ZTDROI, CA0, SA0, SSDROI, RES_ZT, RES_SS, CHI2MI, CHI2
 REAL(8), SAVE :: SS, ZZ, TT, WZ, WT, UZT, UST
 REAL(8), SAVE :: SWIZZN, SWIZZP, CLIMSS
 REAL(8), SAVE :: DZLILI(NMATMA,2,NDRAMA), DSLILI(NMATMA,2,NDRAMA)
 REAL(8), PARAMETER :: BIG = 9999999.D0, TOLZ = 3.0D0, TOLS = 3.0D0
 LOGICAL, EXTERNAL  :: NOKCHA, isMisaligned
!
   NZFIXE(1:NBFIXE,1:2,1:NVDEB) = 1
   NSFIXE(1:NBFIXE,1:2,1:NVDEB) = 1
   DZFIXE(1:NBFIXE,1:2,1:NVDEB) = BIG
   DSFIXE(1:NBFIXE,1:2,1:NVDEB) = BIG
   NZLIBR(1:NBLIBR,1:2,1:NVDEB) = 1
   NSLIBR(1:NBLIBR,1:2,1:NVDEB) = 1
   DZLIBR(1:NBLIBR,1:2,1:NVDEB) = BIG
   DSLIBR(1:NBLIBR,1:2,1:NVDEB) = BIG
!
   IDLIBR(1:NBLIBR) = 0
!
!- Do not track if outside range
   RDEB = SQRT( VDEB(1,1)**2 + VDEB(2,1)**2 )
   ZDEB = ABS( VDEB(3,1) )
   IF( RDEB > RCHAMA .OR. ZDEB > ZCHAMA .OR. (RDEB < RCHAMI.AND.ZDEB < ZCHAMI)    &
                     .OR. VDEB(7,1) < 0.5D0 )   RETURN
   IF( NBFIXE <= 0 )                            RETURN
   IF( NBFIXE+NBOKLI <= 1 )                     RETURN
!
!- Set common CTRABAZ
   IZTPLA = 0
!- Fixed stations
   DO 10 NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     ID  = IDFIXE(NN)
     DO J=1,2
       IW = ABS( IWDRAT(ID,J,ISC) )
       IF( IW <= 0 ) THEN
         IC   = 2*ISC - 2 + J
         IWP  = ABS( IWDRAT(ID,3-J,ISC) )
         INOF = IFCHA(IC,IWP)
         INOZ = IZCHA(IC,IWP)
       ELSE
         IC   = 2*ISC + 1 - J
         INOF = IFCHA(IC,IW)
         INOZ = IZCHA(IC,IW)
       ENDIF
       IZTPLA = MIN( IZTPLA+1 , MAXPLA )
       CF0PLA(IZTPLA)   = COS(FFDRAT(ID,ISC))
       SF0PLA(IZTPLA)   = SIN(FFDRAT(ID,ISC))
       CA0PLA(IZTPLA)   = 1.D0
       SA0PLA(IZTPLA)   = 0.D0
       IF( IC <= NCOUBA ) THEN
         TT0PLA(IZTPLA) = TTDRAT(ID,J,ISC)
       ELSEIF( ISC <= NMATTR ) THEN
         TT0PLA(IZTPLA) = ZZDRAT(ID,J,ISC)
       ELSE
         CA0 = COS(GGDRAT(ID,ISC))
         SA0 = SIN(GGDRAT(ID,ISC))
         CA0PLA(IZTPLA) = CA0
         SA0PLA(IZTPLA) = SA0
         TT0PLA(IZTPLA) = CA0*ZZDRAT(ID,J,ISC) + SA0*TTDRAT(ID,J,ISC)
       ENDIF
       CALL CODEIN(1,0,IW,NN,ID,J,IC,INOF,INOZ,0, IZTPLA)
     ENDDO
10 ENDDO
!- Free stations
   DO 30 NN=1,NBLIBR
     ISC = ISCLIBR(NN)
     DO 20 ID=1,NBDRAT(ISC)
       IF( IDOKLI(ID,ISC) <= 0 ) CYCLE
       DO J=1,2
         IW = ABS( IWDRAT(ID,J,ISC) )
         IF( IW <= 0 ) THEN
           IC   = 2*ISC - 2 + J
           IWP  = ABS( IWDRAT(ID,3-J,ISC) )
           INOF = IFCHA(IC,IWP)
           INOZ = IZCHA(IC,IWP)
         ELSE
           IC   = 2*ISC + 1 - J
           INOF = IFCHA(IC,IW)
           INOZ = IZCHA(IC,IW)
         ENDIF
         IZTPLA = MIN( IZTPLA+1 , MAXPLA )
         CF0PLA(IZTPLA)   = COS(FFDRAT(ID,ISC))
         SF0PLA(IZTPLA)   = SIN(FFDRAT(ID,ISC))
         CA0PLA(IZTPLA)   = 1.D0
         SA0PLA(IZTPLA)   = 0.D0
         IF( IC <= NCOUBA ) THEN
           TT0PLA(IZTPLA) = TTDRAT(ID,J,ISC)
         ELSEIF( ISC <= NMATTR ) THEN
           TT0PLA(IZTPLA) = ZZDRAT(ID,J,ISC)
         ELSE
           CA0 = COS(GGDRAT(ID,ISC))
           SA0 = SIN(GGDRAT(ID,ISC))
           CA0PLA(IZTPLA) = CA0
           SA0PLA(IZTPLA) = SA0
           TT0PLA(IZTPLA) = CA0*ZZDRAT(ID,J,ISC)+SA0*TTDRAT(ID,J,ISC)
         ENDIF
         CALL CODEIN(0,0,IW,NN,ID,J,IC,INOF,INOZ,0, IZTPLA)
       ENDDO
20   ENDDO
30 ENDDO
!- Total number of planes
   NZTPLA = IZTPLA
!
   CALL ReorderPlanes(IZTPLA)
!
!- Track and catch the crossings
   IOPT = 1
   IF( IFLAG == 1 ) IOPT = 2
   CALL TRABAZ(NVDEB,2,IOPT)
!
!- Loop on crossings
   DO 60 IVDEB=1,NVDEB
!
     DO NN=1,NBLIBR
       ISC = ISCLIBR(NN)
       DO ID=1,NBDRAT(ISC)
         DZLILI(NN,1:2,ID) = BIG
         DSLILI(NN,1:2,ID) = BIG
       ENDDO
     ENDDO
!
     DO IZTPLA=1,NZTPLA
       IF( ICRPLA(IZTPLA) <= 0 )  CYCLE
       CALL DECOIN(IZTPLA,  IFILI,IW01,NN,ID,J,IC,INOF,INOZ,ICT)
       ISC  = (IC+1)/2
       IGEOM = IGEOMA(IC,INOF,INOZ)
       IMDT = ISTMDTV(IGEOM)
       ILIN = ISTLINV(IGEOM)
       IF(IC <= NCOUBA) THEN
         ZTDROI             = ZZDRAT(ID,J,ISC)
         ZZTRAKDO(ID,J,ISC) = ZTTPLA(IZTPLA,IVDEB)
         TTTRAKDO(ID,J,ISC) = TT0PLA(IZTPLA)
       ELSEIF(ISC <= NMATTR) THEN
         ZTDROI             = TTDRAT(ID,J,ISC)
         ZZTRAKDO(ID,J,ISC) = TT0PLA(IZTPLA)
         TTTRAKDO(ID,J,ISC) = ZTTPLA(IZTPLA,IVDEB)
       ELSE
         CA0 = COS(GGDRAT(ID,ISC))
         SA0 = SIN(GGDRAT(ID,ISC))
         ZTDROI             =-SA0*ZZDRAT(ID,J,ISC)     + CA0*TTDRAT(ID,J,ISC)
         ZZTRAKDO(ID,J,ISC) =-SA0*ZTTPLA(IZTPLA,IVDEB) + CA0*TT0PLA(IZTPLA)
         TTTRAKDO(ID,J,ISC) = CA0*ZTTPLA(IZTPLA,IVDEB) + SA0*TT0PLA(IZTPLA)
       ENDIF
       SSDROI             = SSDRAT(ID,J,ISC)
       SSTRAKDO(ID,J,ISC) = SSSPLA(IZTPLA,IVDEB)
       IF( IFILI == 0 .AND. IFLAG == 9 ) THEN
         Itrans = ItransCHA(IGEOM)
         CALL SZTWU_Ideal_from_True( Itrans, SSSPLA(IZTPLA,IVDEB),ZTTPLA(IZTPLA,IVDEB),TT0PLA(IZTPLA),         &
                                             VSSPLA(IZTPLA,IVDEB),VZTPLA(IZTPLA,IVDEB),VTZPLA(IZTPLA,IVDEB),   &
                                             SS,ZZ,TT,WZ,WT,UZT,UST)
         ZZ = ZZ + UZT*(TCECHAV(IGEOM)-TT)
         SS = SS + UST*(TCECHAV(IGEOM)-TT)
         IF( NOKCHA(IC,INOZ,IGEOM,ZZ,SS,TOLZ,TOLS) )  CYCLE
       ENDIF
       CLIMSS = 0.d0
       IF( IC <= NCOUCH ) THEN
!- Z and S "effective" resolutions of MDT layers
         RES_ZT = RESTUB(IMDT)
         IF( isMisaligned(IC,INOF,INOZ) ) RES_ZT = 2.d0 * RES_ZT
         IF( MIN(IWDRAT(ID,J,ISC),IWDRAT(ID,3-J,ISC)) < 0 .AND. DSLINK(ILIN).LT.100.D0 ) THEN
!- When straight track segment was obtained without any S measurement
!- (although there should be some), set S resolution to 30 cm
           RES_SS = 30.D0
         ELSE
           RES_SS = DSLINK(ILIN)
         ENDIF
         IF( RES_SS > 15.D0 ) THEN
!- and add a constraint on possible S range when no S measurement
           Itrans = ItransCHA(IGEOM)
           CALL SZTWU_Ideal_from_True( Itrans, SSSPLA(IZTPLA,IVDEB),ZTTPLA(IZTPLA,IVDEB),TT0PLA(IZTPLA),         &
                                               VSSPLA(IZTPLA,IVDEB),VZTPLA(IZTPLA,IVDEB),VTZPLA(IZTPLA,IVDEB),   &
                                               SS,ZZ,TT,WZ,WT,UZT,UST)
           ZZ = ZZ + UZT*(TCECHAV(IGEOM)-TT)
           SS = SS + UST*(TCECHAV(IGEOM)-TT)
           CALL Get_Tube_Lateral_Dim(IGEOM,ZZ, SWIZZN,SWIZZP)
           IF( SS < SWIZZN+TOLS ) CLIMSS = ( ( SS - (SWIZZN+TOLS) ) / TOLS )**2
           IF( SS > SWIZZP-TOLS ) CLIMSS = ( ( SS - (SWIZZP-TOLS) ) / TOLS )**2
         ENDIF
       ELSE
!- Z and S "effective" resolutions of CSC layers
         RES_ZT = EZDRAT(ID,ISC)
         RES_SS = RAYTUB(IMDT) * 0.25d0
       ENDIF
       IF( IFILI == 1 ) THEN
         DZFIXE(NN,J,IVDEB) = ( (ZTTPLA(IZTPLA,IVDEB)-ZTDROI) / RES_ZT )**2
         DSFIXE(NN,J,IVDEB) = ( (SSSPLA(IZTPLA,IVDEB)-SSDROI) / RES_SS )**2 + CLIMSS
         IF( IW01 <= 0 .AND. ISingle(IGEOM) /= 0 )  DZFIXE(NN,J,IVDEB) = DZFIXE(NN,J,IVDEB) + 50.D0
!------------------------------------------------> @@@@@@@@@@@@@@@@@@@@@
         IF( ISC > NMATTR )  DZFIXE(NN,J,IVDEB) = DZFIXE(NN,J,IVDEB) + CCDRAT(ID,ISC)/2.D0
!------------------------------------------------> @@@@@@@@@@@@@@@@@@@@@
       ELSE
         DZLILI(NN,J,ID) = ( (ZTTPLA(IZTPLA,IVDEB)-ZTDROI) / RES_ZT )**2
         DSLILI(NN,J,ID) = ( (SSSPLA(IZTPLA,IVDEB)-SSDROI) / RES_SS )**2 + CLIMSS
         IF( IW01 <= 0 .AND. ISingle(IGEOM) /= 0 )  DZLILI(NN,J,ID) = DZLILI(NN,J,ID) + 50.D0
!-------------------------------------------> @@@@@@@@@@@@@@@@@@@@@
         IF( ISC > NMATTR )  DZLILI(NN,J,ID) = DZLILI(NN,J,ID) + CCDRAT(ID,ISC)/2.D0
!-------------------------------------------> @@@@@@@@@@@@@@@@@@@@@
       ENDIF
     ENDDO
!
     IF( IVDEB == 1 ) THEN
       DO NN=1,NBLIBR
         ISC = ISCLIBR(NN)
         CHI2MI = BIG**2
         DO ID=1,NBDRAT(ISC)
           CHI2 = DZLILI(NN,1,ID) + DZLILI(NN,2,ID) + DSLILI(NN,1,ID) + DSLILI(NN,2,ID)
           IF(CHI2 < CHI2MI) THEN
             CHI2MI = CHI2
             IDLIBR(NN) = ID
             DZLIBR(NN,1:2,IVDEB) = DZLILI(NN,1:2,ID)
             DSLIBR(NN,1:2,IVDEB) = DSLILI(NN,1:2,ID)
           ENDIF
         ENDDO
       ENDDO
     ELSE
       DO NN=1,NBLIBR
         ID = IDLIBR(NN)
         DZLIBR(NN,1:2,IVDEB) = DZLILI(NN,1:2,ID)
         DSLIBR(NN,1:2,IVDEB) = DSLILI(NN,1:2,ID)
       ENDDO
     ENDIF
!
60 ENDDO
!
 END SUBROUTINE TRAKDO
!
