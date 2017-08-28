      SUBROUTINE CAVBKG(INFILE)

      IMPLICIT NONE
      INTEGER IEVENT/0/,LOUT/6/
      REAL TWOPI/6.2831853/
      CHARACTER INFILE*40
C ATLRCOM included:
C      INTEGER MA,MB,MC,ME,FI,FE,FM,FO
      INTEGER CS
C      PARAMETER (MA=1)
C      PARAMETER (MB=2)
C      PARAMETER (MC=3)
C      PARAMETER (ME=4)
C      PARAMETER (FI=5)
C      PARAMETER (FE=6)
C      PARAMETER (FM=7)
C      PARAMETER (FO=8)
      PARAMETER (CS=9)
      REAL AT_ZONES
      COMMON /ATLASRAD/ AT_ZONES(6,9)
      CHARACTER FILE*40/'fvpn21n.dat'/
      INTEGER IER
* static variables:
      INTEGER LUN/21/,ISTAT/0/,IPRIN/2/,NFILE/0/,IEVNT/0/,NPART/0/
      INTEGER LENOCC,NREAD/170/,NSAFETY/1/,IS/0/,IS0/0/
      CHARACTER INPUT*40/'fvpn21n.dat'/
* input record:
      CHARACTER VOLU*4
      REAL GEKIN,VECT(7),TOFG
      REAL TOF,Z,R,RR,PHI,VECTO(3)
      REAL PVERT(3),VERT(3),GEKO,TOO,FIO
      REAL VDIST,CAV_RNDM,D,R1,R2,Z1,Z2,DPHI
      INTEGER ITYPE,I,ID,ITO,IR,IEV,IC,IC0,IC9,ICS
      REAL ZCSC/712./,S/0/,T0/25.E-9/
      LOGICAL FIRST/.TRUE./
* output record
      INTEGER NPVMAX
      PARAMETER (NPVMAX=2000)
      INTEGER NPV, ITYP, IRP
      REAL VX, VY, VZ, PX, PY, PZ, PTOF
      COMMON /BKGOUT/ IEVENT, NPV, VX(NPVMAX),VY(NPVMAX),VZ(NPVMAX),
     >     PX(NPVMAX),PY(NPVMAX),PZ(NPVMAX), PTOF(NPVMAX),
     >     IRP(NPVMAX), ITYP(NPVMAX)
* G3 to PDG conversion
      INTEGER G32PDG(25)
      DATA G32PDG/ 22, -11, 11, 12, 13, -13, 111, 211, -211, 130,
     >     321, -321, 2112, 2212, -2212, 310, 221, 3122, 3222, 3212,
     >     3112, 3322, 3312, 3334, -2112/
      logical retry

C Check FIRST
      IF (FIRST) THEN
         FIRST=.FALSE.
         IS = 0
         NPART = 0
         ISTAT = 1
         NFILE = 1
         IEVNT = 0
         FILE = INFILE
         INPUT = FILE
         OPEN (LUN,FILE=INPUT,FORM='UNFORMATTED',STATUS='OLD')
         CALL ATLRGEOM
         S=(780.-687.)/(780.-712.)
      ENDIF
* standalone event counting:
      IEVENT=IEVENT+1
      NPV = 0
C *
C     Loop here
      DO 5011 IEV=1,NREAD
        IS0=IS
        GEKO=GEKIN
        TOO=TOF
        FIO=PHI
        ITO=ITYPE
        CALL UCOPY(VECT,VECTO,3)
C     Check ISTAT==0
        retry = .false.
 5020   IF (ISTAT.EQ.0 .or. retry) THEN
           retry = .false.
           CLOSE(LUN)
           IC0=ICHAR('0')
           IC9=ICHAR('9')
           ICS=ICHAR('/')
C *
C     Loop here
           DO 5031 I=LENOCC(INPUT),1,-1
             IC=ICHAR(INPUT(I:I))
C     Check IC==ICS
             IF (IC.EQ.ICS)GO TO 5032
C     Skip Unless IC0<=IC&IC<=IC9
             IF (IC0.GT.IC.OR.IC.GT.IC9)GO TO 5031
             IC=IC+1
C     Check IC>IC9
             IF (IC.GT.IC9) IC=IC0
             INPUT(I:I)=CHAR(IC)
C     Check IC>IC0
             IF (IC.GT.IC0)GO TO 5032
 5031      CONTINUE
 5032      CONTINUE
           retry = .true.
           OPEN (LUN,FILE=INPUT,FORM='UNFORMATTED',STATUS='OLD',
     >          ERR=5020)
           retry = .false.
           NFILE=NFILE+1
           ISTAT=1
C     Check IEVNT>100
           IF (IEVNT.GT.100)
     >          NREAD=NREAD*(FLOAT(NSAFETY*IEVENT)/IEVNT)
C     Check IPRIN>=1
           IF (IPRIN.GE.1) WRITE(LOUT,5040)NFILE,INPUT,NREAD
 5040      FORMAT(' agusread: openning ',I5,' file ',A,' Nr=',I5)
        ENDIF
C     TODO: gfortran doesn't like END or ERR to be outside the current block
        READ (LUN,END=5020,ERR=5020) VOLU,ITYPE,GEKIN,VECT,TOF
C*
C     Check (VOLU == 'XXXX')
        IF ((VOLU .EQ. 'XXXX')) THEN
           IEVNT=IEVNT+1
C           WRITE(LOUT,5050)IEVNT,NPART,ITYPE,GEKIN,VECT(1)
C 5050      FORMAT(' END EVENT ievnt,npart,ievts,SPILLS,TIME: ',
C     *          3I6,2X,3G9.4)
           NPART = 0
           GO TO 5011
        END IF
*
        READ(VOLU,'(1x,i2)') ID
        NPART=NPART+1
        IF (.NOT. ( ITYPE.EQ.ITO .AND.GEKO*.1.LE.GEKIN.AND.
     *       GEKIN.LE.GEKO .AND. TOF.GE.TOO .AND.
     *       VDIST(VECT,VECTO,3).LT.MAX(3.E10*(TOF-TOO),100.) ))
     *       IS=IS+1
*     selecte correct types
C     Skip Unless ITYPE<=3 | ITYPE==13 | ITYPE==14
        IF ((ITYPE.GT.3).AND.((ITYPE.NE.13).AND.(ITYPE.NE.14)))GO TO
     *       5011
        IR = MAX(MOD(ID,10),1) + (ID/10-1)*4
        Z = ABS(VECT(3))
C     Check ID==30
        IF (ID.EQ.30) Z=(Z-ZCSC)*S+AT_ZONES(5,CS)
        PHI = ATAN2(VECT(2),VECT(1))
        R = SQRT(VECT(1)**2+VECT(2)**2)
        RR = SQRT(R**2 +VECT(3)**2)
        R1 = AT_ZONES(3,IR)
        R2 = AT_ZONES(4,IR)
        Z1 = AT_ZONES(5,IR)
        Z2 = AT_ZONES(6,IR)
*     check envelop boundaries
        D = MIN(ABS(R-R1),ABS(R-R2),ABS(Z-Z1),ABS(Z-Z2))
C     Skip Unless D<.1
        IF (D.GE..1)GO TO 5011
C     Check ITYPE==2 | ITYPE==3 | ITYPE==14
        IF (ITYPE.EQ.2 .OR. ITYPE.EQ.3 .OR. ITYPE.EQ.14) THEN
*     charge particles are taken only once and only after 1 msec
           WRITE(LOUT,5060)VOLU,IS-IS0,ITYPE,GEKIN,TOF,VECT
 5060      FORMAT(' VOL=',A,' Is,Id=',2I4,' GEKIN,TOF',2E10.3,
     *          ' V=',3F9.3,1X,3F9.4,F9.3)
C     Check IS==IS0 | TOF<1.E-6
           IF (IS.EQ.IS0 .OR. TOF.LT.1.E-6)GO TO 5011
        ENDIF
        DPHI = TWOPI*CAV_RNDM(-1)
        VERT(1) = R*COS(PHI+DPHI)
        VERT(2) = R*SIN(PHI+DPHI)
        VERT(3) = SIGN(Z,VECT(3))
        PVERT(1) = (VECT(4)*COS(DPHI)-VECT(5)*SIN(DPHI))*VECT(7)
        PVERT(2) = (VECT(4)*SIN(DPHI)+VECT(5)*COS(DPHI))*VECT(7)
        PVERT(3) = VECT(6)*VECT(7)
        TOFG = T0*CAV_RNDM(-1)

        NPV = NPV + 1
        IF(NPV .GT. NPVMAX) THEN
           print *, 'WARNING: NPV > NPVMAX'
        ELSE
           VX(NPV) = VERT(1)
           VY(NPV) = VERT(2)
           VZ(NPV) = VERT(3)
           PX(NPV) = PVERT(1)
           PY(NPV) = PVERT(2)
           PZ(NPV) = PVERT(3)
           PTOF(NPV) = TOFG
           IRP(NPV) = IR
           IF (ITYPE.GT.25) THEN
              print *, 'WARNING: ITYPE > 25'
              ITYP(NPV) = ITYPE
           ELSE
              ITYP(NPV) = G32PDG(ITYPE)
           END IF
        END IF
C        if (ITYPE.ne.1 .and. ITYPE.ne.13 .and. ITYPE.ne.2 .and.
C     >       ITYPE.ne.3 ) print *, 'NPART, IEVENT, ITYPE ',
C     >       NPART, IEVENT, ITYPE
C     * call Gsvert(Vert,  0,-ir,  0,0,iv)
C     * call Gskine(Pvert,Itype,iv,0,0,it)
 5011 CONTINUE
C 5012 CONTINUE
      IER=0
      RETURN
C 1    CONTINUE
      END
	  
