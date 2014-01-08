!
!> Track fit using hits (used when ItypFit = 1 or 4)
!> \author M.Virchaux
!
 SUBROUTINE TRAKTU(NVDEB,IFLAG)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_Types
 USE M_MB_MuGeom
 USE M_MB_PACONS
 USE M_MB_CTRABAZ
 USE M_MB_CODRAT
 USE M_MB_Fit
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: NVDEB, IFLAG
!
 INTEGER :: IZTPLA, ISC, ID, J, IC, IWP, INOF, INOZ, IGEOM, ILIN
 INTEGER :: NN, IW, IWDRATfun, IL, ICT, IDmax, IVDEB, IW01
 INTEGER :: NTOT, ISTMDT1, IFILI, IOPT
 REAL(8) :: RDEB, ZDEB, ZZTT, VVTZ, VVZT, SSSS, VVSS, CTOT
 REAL(8) :: CHI2MI, XDENOM, CHI2
 INTEGER,SAVE :: NZLILI(NMATMA,2,NDRAMA), NSLILI(NMATMA,2,NDRAMA)
 INTEGER,SAVE :: JZLILI(N4M,4,NMATMA,2,NDRAMA)
 REAL(8),SAVE :: DZLILI(NMATMA,2,NDRAMA), DSLILI(NMATMA,2,NDRAMA)
 TYPE(DcaPoint), SAVE :: dcapoin
 TYPE(DcaStrip), SAVE :: dcastr 
 LOGICAL :: Ldrat
 INTEGER,SAVE :: ICTdeja(100)
 INTEGER :: Ndeja, NdejaFixe
 INTEGER,SAVE :: ICTtota(100)
 INTEGER :: Ntota, NNBB
 INTEGER, EXTERNAL :: intpos
 REAL(8), PARAMETER :: BIG = 9999999.D0
!
   NZFIXE(1:NBFIXE,1:2,1:NVDEB) = 1
   NSFIXE(1:NBFIXE,1:2,1:NVDEB) = 0
   DZFIXE(1:NBFIXE,1:2,1:NVDEB) = BIG
   DSFIXE(1:NBFIXE,1:2,1:NVDEB) = 0.D0
   NZLIBR(1:NBLIBR,1:2,1:NVDEB) = 1
   NSLIBR(1:NBLIBR,1:2,1:NVDEB) = 0
   DZLIBR(1:NBLIBR,1:2,1:NVDEB) = BIG
   DSLIBR(1:NBLIBR,1:2,1:NVDEB) = 0.D0
   IDLIBR(1:NBLIBR)             = 0
!
   IF( saveDcaTrk ) THEN
     JZFIXE(:,:,1:NBFIXE,1:2) = 0
     IZFIXE(:,:,1:NBFIXE,1:2) = 0
     JZLIBR(:,:,1:NBLIBR,1:2) = 0
     JSFIXE(:,1:NBFIXE,1:2)   = 0
     ISFIXE(:,1:NBFIXE,1:2)   = 0
     CSFIXE(:,1:NBFIXE,1:2)   = 0.D0
   ENDIF
!
!>> Do not track if outside range
   RDEB = SQRT( VDEB(1,1)**2 + VDEB(2,1)**2 )
   ZDEB = ABS( VDEB(3,1) )
   IF( RDEB > RCHAMA .OR. ZDEB > ZCHAMA .OR. (RDEB < RCHAMI.AND.ZDEB < ZCHAMI)    &
                     .OR. VDEB(7,1) < 0.5D0 )   RETURN
   IF( NBFIXE <= 0 )                            RETURN
   IF( NBFIXE+NBOKLI <= 1 )                     RETURN
!
!>> Set common CTRABAZ
   IZTPLA = 0
   Ndeja  = 0
!>> Fixed stations
   DO 20 NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     ID = 1
     IF( IWcross(ISC,1)+IWcross(ISC,2) < 1 ) ID = IDFIXE(NN)
     DO 10 J=1,2
       IW = ABS( IWDRATfun(ID,J,ISC) )
       IF(IW <= 0) THEN
         IC   = 2*ISC - 2 + J
         IWP  = ABS( IWDRATfun(ID,3-J,ISC) )
         INOF = IFCHA(IC,IWP)
         INOZ = IZCHA(IC,IWP)
         IGEOM= IGEOMA(IC,INOF,INOZ)
         ILIN = 0
       ELSE
         IC   = 2*ISC + 1 - J
         INOF = IFCHA(IC,IW)
         INOZ = IZCHA(IC,IW)
         IGEOM= IGEOMA(IC,INOF,INOZ)
         ILIN = ISTLINV(IGEOM)
       ENDIF
       DO IL=0,NBLINK(ILIN)
         IF( IL > 0 ) THEN
           ICT = ICTRIG(IL,ILIN)
           IF( isOffTrig(ICT)                  )  CYCLE
           IF( intpos(ICT, ICTdeja, Ndeja) > 0 )  CYCLE ! avoid duplication of RPC/TGC linked to several MDT stations
         ENDIF
         IZTPLA = MIN( IZTPLA+1 , MAXPLA )
         CA0PLA(IZTPLA) = 1.D0
         SA0PLA(IZTPLA) = 0.D0
         IF( IL <= 0 ) THEN
           TT0PLA(IZTPLA) = TCECHAV(IGEOM)
           CF0PLA(IZTPLA) = COSCHAV(IGEOM)
           SF0PLA(IZTPLA) = SINCHAV(IGEOM)
           IF( IC.GT.NCOUCH ) THEN
             CA0PLA(IZTPLA) = COS(GCECHAV(IGEOM))
             SA0PLA(IZTPLA) = SIN(GCECHAV(IGEOM))
           ENDIF
         ELSE
           ICT  = ICTRIG(IL,ILIN)
           Ndeja = MIN( Ndeja+1, 100 )
           ICTdeja(Ndeja) = ICT
           TT0PLA(IZTPLA) = TCETRI(ICT)
           CF0PLA(IZTPLA) = COSTRI(ICT)
           SF0PLA(IZTPLA) = SINTRI(ICT)
         ENDIF
!.............................................
         CALL CODEIN(1,IL,IW,NN,ID,J,IC,INOF,INOZ,ICT, IZTPLA)
       ENDDO
10   ENDDO
20 ENDDO
   NdejaFixe = Ndeja
   Ntota     = Ndeja
   ICTtota(1:Ntota) = ICTdeja(1:Ntota)
!>> Free stations
   DO 40 NN=1,NBLIBR
     ISC = ISCLIBR(NN)
     Ldrat = IWcross(ISC,1)+IWcross(ISC,2) < 1
     IDmax = 1
     IF( Ldrat ) IDmax = NBDRAT(ISC)
     Ndeja = NdejaFixe
     DO 30 ID=1,IDmax
       IF( Ldrat .AND. IDOKLI(ID,ISC) < 1 ) CYCLE
       DO J=1,2
         IW = ABS( IWDRATfun(ID,J,ISC) )
         IF(IW <= 0) THEN
           IC   = 2*ISC - 2 + J
           IWP  = ABS( IWDRATfun(ID,3-J,ISC) )
           INOF = IFCHA(IC,IWP)
           INOZ = IZCHA(IC,IWP)
           IGEOM= IGEOMA(IC,INOF,INOZ)
           ILIN = 0
         ELSE
           IC   = 2*ISC + 1 - J
           INOF = IFCHA(IC,IW)
           INOZ = IZCHA(IC,IW)
           IGEOM= IGEOMA(IC,INOF,INOZ)
           ILIN = ISTLINV(IGEOM)
         ENDIF
         DO IL=0,NBLINK(ILIN)
           IF( IL <= 0 ) THEN
             IF( isOffChav(IGEOM) )  CYCLE
           ELSE     ! avoid duplication of RPC/TGC linked to several MDT stations
             ICT = ICTRIG(IL,ILIN)
             IF( isOffTrig(ICT)                  )  CYCLE
             IF( intpos(ICT, ICTdeja, Ndeja) > 0 )  CYCLE
           ENDIF
           IZTPLA = MIN( IZTPLA+1 , MAXPLA )
           CA0PLA(IZTPLA) = 1.D0
           SA0PLA(IZTPLA) = 0.D0
           IF(IL <= 0) THEN
             TT0PLA(IZTPLA) = TCECHAV(IGEOM)
             CF0PLA(IZTPLA) = COSCHAV(IGEOM)
             SF0PLA(IZTPLA) = SINCHAV(IGEOM)
             IF( IC > NCOUCH ) THEN
               CA0PLA(IZTPLA) = COS(GCECHAV(IGEOM))
               SA0PLA(IZTPLA) = SIN(GCECHAV(IGEOM))
             ENDIF
           ELSE
             ICT  = ICTRIG(IL,ILIN)
             Ndeja = MIN( Ndeja+1, 100 )
             ICTdeja(Ndeja) = ICT
             IF( intpos(ICT, ICTtota, Ntota) <= 0 ) THEN
               Ntota = MIN( Ntota + 1, 100 )
               ICTtota(Ntota) = ICT
             ENDIF
             TT0PLA(IZTPLA) = TCETRI(ICT)
             CF0PLA(IZTPLA) = COSTRI(ICT)
             SF0PLA(IZTPLA) = SINTRI(ICT)
           ENDIF
           CALL CODEIN(0,IL,IW,NN,ID,J,IC,INOF,INOZ,ICT, IZTPLA)
         ENDDO
       ENDDO
30   ENDDO
40 ENDDO
! Add now the additionnal trigger layers of all fixed stations
   DO 60 NN=1,NBFIXE
     ISC = ISCFIXE(NN)
     ID = 1
     IF( IWcross(ISC,1)+IWcross(ISC,2) < 1 ) ID = IDFIXE(NN)
     DO 50 J=1,2
       IW = ABS( IWDRATfun(ID,J,ISC) )
       IF(IW <= 0) THEN
         IC   = 2*ISC - 2 + J
         IWP  = ABS( IWDRATfun(ID,3-J,ISC) )
         INOF = IFCHA(IC,IWP)
         INOZ = IZCHA(IC,IWP)
         IGEOM= IGEOMA(IC,INOF,INOZ)
       ELSE
         IC   = 2*ISC + 1 - J
         INOF = IFCHA(IC,IW)
         INOZ = IZCHA(IC,IW)
         IGEOM= IGEOMA(IC,INOF,INOZ)
       ENDIF
       IF( ISTLINV(IGEOM) > 0 ) THEN
         ILIN = ISTLINV(IGEOM)
         NNBB = NBLINK(ILIN)
       ELSE IF( JSTLINV(IGEOM) > 0 ) THEN
         ILIN = JSTLINV(IGEOM)
         NNBB = MBLINK(ILIN)
       ELSE
         CYCLE
       ENDIF
       DO IL=1,NNBB
         IF( ISTLINV(IGEOM) > 0 ) THEN
           ICT = ICTRIG(IL,ILIN)
         ELSE
           ICT = JCTRIG(IL,ILIN)
         ENDIF
         IF( isOffTrig(ICT)                  )  CYCLE
         IF( intpos(ICT, ICTtota, Ntota) > 0 )  CYCLE ! avoid duplication of RPC/TGC linked to several MDT stations
         Ntota = MIN( Ntota + 1, 100 )
         ICTtota(Ntota) = ICT
         IZTPLA = MIN( IZTPLA+1 , MAXPLA )
         CA0PLA(IZTPLA) = 1.D0
         SA0PLA(IZTPLA) = 0.D0
         TT0PLA(IZTPLA) = TCETRI(ICT)
         CF0PLA(IZTPLA) = COSTRI(ICT)
         SF0PLA(IZTPLA) = SINTRI(ICT)
!.............................................
         CALL CODEIN(1,IL,IW,NN,ID,J,IC,INOF,INOZ,ICT, IZTPLA)
       ENDDO
50   ENDDO
60 ENDDO
!>> Total number of planes
   NZTPLA = IZTPLA
!
   CALL ReorderPlanes(IZTPLA)
!
!>> Track and catch the crossings
   IOPT = 1
   IF( IFLAG == 1 ) IOPT = 2
   CALL TRABAZ(NVDEB,2,IOPT)
!
!>> Loop on crossings
   DO 90 IVDEB=1,NVDEB
!
     DO NN=1,NBLIBR
       ISC = ISCLIBR(NN)
       IDmax = 1
       IF( IWcross(ISC,1)+IWcross(ISC,2) < 1 ) IDmax = NBDRAT(ISC)
       NZLILI(NN,1:2,1:IDmax) = 1
       NSLILI(NN,1:2,1:IDmax) = 0
       DZLILI(NN,1:2,1:IDmax) = BIG
       DSLILI(NN,1:2,1:IDmax) = 0.D0
       JZLILI(:,:,NN,1:2,1:IDmax) = 0
     ENDDO
!
b70: DO 70 IZTPLA=1,NZTPLA
       IF( ICRPLA(IZTPLA) <= 0 ) CYCLE
       CALL DECOIN(IZTPLA,  IFILI,IW01,NN,ID,J,IC,INOF,INOZ,ICT)
       ZZTT  = ZTTPLA(IZTPLA,IVDEB)
       VVTZ  = VTZPLA(IZTPLA,IVDEB)
       VVZT  = VZTPLA(IZTPLA,IVDEB)
       SSSS  = SSSPLA(IZTPLA,IVDEB)
       VVSS  = VSSPLA(IZTPLA,IVDEB)
       IF( ICT <= 0 ) THEN
         IGEOM = IGEOMA(IC,INOF,INOZ)
         IF( IW01 <= 0 ) THEN
           IF( ISingle(IGEOM) == 0 ) THEN
             IF( IFILI == 1 ) THEN
               NZFIXE(NN,J,IVDEB) = 0
               DZFIXE(NN,J,IVDEB) = 0.D0
             ELSE
               NZLILI(NN,J,ID) = 0
               DZLILI(NN,J,ID) = 0.D0
             ENDIF
             CYCLE b70
           ENDIF
           NTOT = 0
           ISTMDT1 = ISTMDTV(IGEOM)
           IF(ISTMDT1 >= 1) NTOT = NLTUB(ISTMDT1)
           IF( IFILI == 1 ) THEN
             NZFIXE(NN,J,IVDEB) = NTOT
             DZFIXE(NN,J,IVDEB) = NTOT * 10.D0 * SIGMAK2
             IF( IVDEB == 1 ) THEN
               JZFIXE(1:NTOT,1,NN,J) = -10000000
               IZFIXE(1:NTOT,1,NN,J) = 0
             ENDIF
           ELSE
             NZLILI(NN,J,ID) = NTOT
             DZLILI(NN,J,ID) = NTOT * 10.D0 * SIGMAK2
             IF( IVDEB == 1 ) THEN
               JZLILI(1:NTOT, 1 ,NN,J,ID) = -10000000
               JZLILI(1:NTOT,2:4,NN,J,ID) = 0
             ENDIF
           ENDIF
         ELSE
           CALL CHIZZZ(IC,INOF,INOZ,ZZTT,SSSS,VVZT,VVTZ,VVSS, NTOT,CTOT, dcapoin)
           IF( IFILI == 1 ) THEN
             NZFIXE(NN,J,IVDEB) = NTOT
             DZFIXE(NN,J,IVDEB) = CTOT
             IF( IVDEB == 1 .AND. saveDcaTrk ) THEN
               WHERE( dcapoin%JJJ(:,:) /= 0 )   ! car sinon non initialises => inutile de les copier
                 JZFIXE(:,:,NN,J) = dcapoin%JJJ(:,:)
                 IZFIXE(:,:,NN,J) = dcapoin%III(:,:)
                 IDZFIX(:,:,NN,J) = dcapoin%IDE(:,:)
                 RZFIXE(:,:,NN,J) = dcapoin%RHIT(:,:)
                 RIFIXE(:,:,NN,J) = dcapoin%RTRA(:,:)
                 XDFIXE(:,:,NN,J) = dcapoin%XDCA(:,:)
                 YDFIXE(:,:,NN,J) = dcapoin%YDCA(:,:)
                 ZDFIXE(:,:,NN,J) = dcapoin%ZDCA(:,:)
                 XDdFIXE(:,:,NN,J)= dcapoin%XDIR(:,:)
                 YDdFIXE(:,:,NN,J)= dcapoin%YDIR(:,:)
                 ZDdFIXE(:,:,NN,J)= dcapoin%ZDIR(:,:)
                 ENFIXE(:,:,NN,J) = dcapoin%EDCA(:,:)
                 RDFIXE(:,:,NN,J) = dcapoin%RDCA(:,:)
                 SDFIXE(:,:,NN,J) = dcapoin%SDCA(:,:)
               END WHERE
             ENDIF
           ELSE
             NZLILI(NN,J,ID) = NTOT
             DZLILI(NN,J,ID) = CTOT
             IF( IVDEB == 1 .AND. saveDcaTrk ) JZLILI(:,:,NN,J,ID) = dcapoin%JJJ(:,:)
           ENDIF
         ENDIF
       ELSE
         CALL CHISSS(ICT,ZZTT,SSSS,VVZT,VVTZ,VVSS, NTOT,CTOT, dcastr)
         IF( IFILI == 1 ) THEN
           IF( IVDEB == 1 .AND. saveDcaTrk ) THEN
             IF( NTOT >= 1 .AND. NSFIXE(NN,J,IVDEB)+NTOT <= N8M ) THEN
               JSFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%JJJ(1:NTOT)
               ISFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%III(1:NTOT)
               CSFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%CCC(1:NTOT)
               XSDFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J)  = dcastr%XDCA(1:NTOT)
               YSDFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J)  = dcastr%YDCA(1:NTOT)
               ZSDFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J)  = dcastr%ZDCA(1:NTOT)
               XSDdFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%XDir(1:NTOT)
               YSDdFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%YDir(1:NTOT)
               ZSDdFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J) = dcastr%ZDir(1:NTOT)
               ESNFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J)  = dcastr%EneDCA(1:NTOT)
               RSDFIXE(NSFIXE(NN,J,IVDEB)+1:NSFIXE(NN,J,IVDEB)+NTOT,NN,J)  = dcastr%ResDCA(1:NTOT)
             ENDIF
           ENDIF
           NSFIXE(NN,J,IVDEB) = NSFIXE(NN,J,IVDEB) + NTOT
           DSFIXE(NN,J,IVDEB) = DSFIXE(NN,J,IVDEB) + CTOT
         ELSE
           NSLILI(NN,J,ID) = NSLILI(NN,J,ID) + NTOT
           DSLILI(NN,J,ID) = DSLILI(NN,J,ID) + CTOT
         ENDIF
       ENDIF
70   ENDDO b70
!
     IF( IVDEB == 1 ) THEN
       DO NN=1,NBLIBR
         ISC = ISCLIBR(NN)
         CHI2MI = BIG**2
         IDmax = 1
         IF( IWcross(ISC,1)+IWcross(ISC,2) < 1 ) IDmax = NBDRAT(ISC)
         DO ID=1,IDmax
           XDENOM = NZLILI(NN,1,ID) + NZLILI(NN,2,ID) + NSLILI(NN,1,ID)*FacSZ + NSLILI(NN,2,ID)*FacSZ - 1.D0
           XDENOM = MAX( XDENOM , 1.D0 )
           CHI2 = ( DZLILI(NN,1,ID) + DZLILI(NN,2,ID) + DSLILI(NN,1,ID) + DSLILI(NN,2,ID) ) / XDENOM
           IF(CHI2 < CHI2MI) THEN
             CHI2MI = CHI2
             IDLIBR(NN) = ID
             JZLIBR(:,:,NN,1:2) = JZLILI(:,:,NN,1:2,ID)
             NZLIBR(NN,1:2,IVDEB) = NZLILI(NN,1:2,ID)
             NSLIBR(NN,1:2,IVDEB) = NSLILI(NN,1:2,ID)
             DZLIBR(NN,1:2,IVDEB) = DZLILI(NN,1:2,ID)
             DSLIBR(NN,1:2,IVDEB) = DSLILI(NN,1:2,ID)
           ENDIF
         ENDDO
       ENDDO
     ELSE
       DO NN=1,NBLIBR
         ID = IDLIBR(NN)
         NZLIBR(NN,1:2,IVDEB) = NZLILI(NN,1:2,ID)
         NSLIBR(NN,1:2,IVDEB) = NSLILI(NN,1:2,ID)
         DZLIBR(NN,1:2,IVDEB) = DZLILI(NN,1:2,ID)
         DSLIBR(NN,1:2,IVDEB) = DSLILI(NN,1:2,ID)
       ENDDO
     ENDIF
!
90 ENDDO
!
 END SUBROUTINE TRAKTU
!
