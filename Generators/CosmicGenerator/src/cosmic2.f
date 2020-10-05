      SUBROUTINE COSMIC2

C-- This version for Athena use wrapped into C++
C-- Modfied by Rob McPherson 9 Aug 2005
      
C-------------------------------------------------
C   Original version modified by B. Bloch in March 1988
C   Changes are :
C introduce common
C      IMPLICIT NONE

      COMMON /COSEVT/ ENER, COSTH, PHI, CHRG
      COMMON /COSIO/  IOU,IDB
      COMMON /COSPAR/ EMIN,EMAX,ECUT, RVRT , ZVRT
      COMMON /FLXOUT/ FLUX, FLUX2
      
      COMMON /COSCUT/ CTCUT

      REAL    LBINWID, LEMIN, LEMAX, PROBE
      INTEGER NBIN
      COMMON /GENPAR/ LBINWID, LEMIN, LEMAX, NBIN, PROBE(100)
      
      LOGICAL QFIRST/.TRUE./

      INTEGER IPART
      REAL    SINTH, PLAB(3)
      REAL    ZERO(4), DXDY, DZDY, XYZERO, ZYZERO

C  Arguments for GFPART
C      INTEGER NU, ITRTYP
C      REAL AMASS, CHARGE, TLIFE, UB(100)
C      CHARACTER CNAME*20
c..      Call GFPART (IPART,CNAME,ITRTYP,AMASS,CHARGE,TLIFE,UB,NU)

C  Arguments for GSKINE
C       INTEGER ITR, IVERT
C c..        CALL GSKINE(PLAB,IPART,IVERT,UB,0,ITR)

      
      
C  Make calls to HBOOK conform to HBOOK4 specifications
C  Change range of original histo ids to be >10000
C  Correct typing(?) error :CHRG was always negative
C  Introduce test on ECUT rather than a number (10 gev)
C  Skip part computing direction of muon entering surface element
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/28
C! - Program to generate the cosmic flux in I4
C

      IF(QFIRST) THEN
        QFIRST = .FALSE.
      
C Print flux tables
C
C         CTCUT   = 0.35
         XBOX    = 60000.
         ZBOX    = 60000.
C         YHEIGHT = 9400.
C         EMIN    = 10.
C         EMAX    = 5000.
C         LEMIN   = LOG10(EMIN)
C         LEMAX   = LOG10(EMAX)
C         NBIN    = 100
C         LBINWID = (LEMAX-LEMIN) / FLOAT(NBIN)

        EMIN = 10**LEMIN
        EMAX = 10**LEMAX
        IOU     = 6
          
        NACC    = 0
        
C         call hbook1(10001,' Gen E mu (surface) [GeV]',50,0.,1000.,0.)
C         call hbook1(10002,' Generated cos(theta)',50,0.,1.,0.)
C         call hbook2(10003,' Gen x vs z at surface',60,-30000.,30000.,60,
C      +                                                -30000.,30000.,0.)
C         call hbook1(10004,' Gen Plab(1)', 100, -1000., 1000., 0.)
C         call hbook1(10005,' Gen Plab(2)', 100, -1000., 1000., 0.)
C         call hbook1(10006,' Gen Plab(3)', 100, -1000., 1000., 0.)

C         call hbook1(11001,' Prob E mu surface [GeV]',50,0.,1000.,0.)
C         call hbook1(11002,' Prob cos(theta) surface',50,0.,1.,0.)
        
C         call hbook1(12001,' Gen Cuts E mu (surface) [GeV]',
C      +                      50,0.,1000.,0.)
C         call hbook1(12002,' Gen Cuts cos(theta)', 50, 0., 1., 0.)
C         call hbook2(12003,' Gen Cuts x vs z at surface',
C      +                    60,-30000.,30000.,60,-30000.,30000.,0.)
C         call hbook1(12004,' Gen Cuts Plab(1)', 100, -1000., 1000., 0.)
C         call hbook1(12005,' Gen Cuts Plab(2)', 100, -1000., 1000., 0.)
C         call hbook1(12006,' Gen Cuts Plab(3)', 100, -1000., 1000., 0.)
        
C         CALL COSIPR
C C
C C Initialize the cosmic muon generation
C C
C         CALL COSGIN
        NEV  = 1000000
C
C Event loop for cross-section calculation
C
        DO 100 I = 1,NEV
          CALL COSGEN(EMIN,EMAX,IACC)
          IF(IACC.EQ.1)THEN
C            CALL HF1(11001,ENER,1.)
C            CALL HF2(11002,COSTH,1.)
            NACC = NACC+1
          ENDIF
 100    CONTINUE
        ef = FLOAT(NACC)/FLOAT(NEV)

C     Total flux over 20m square (ATLAS):
        FLUX = COSINA(EMIN) - COSINA(EMAX)
        TOTINT = XBOX * ZBOX * FLUX
        TIME   = NEV/TOTINT
        FLUX2  = FLUX*ef
        TOTIN2 = TOTINT*ef
*
        WRITE(IOU,1001) NEV, NACC, FLUX, TOTINT, TIME, FLUX2, TOTIN2
1001  FORMAT(//' ------------------------FINAL STATISTICS-------------'/
     > 8x,'EVTS GENERATED -',I10,'  ACCEPTED -',I10//
     > 8x,'FLUX',10x,'TOTINT',11x,'TIME',11x,'FLUX2',10x,'TOTIN2 '/
     > 1X,5E15.5)

        RETURN
        
      ENDIF
      
*     CALL HPRINT(0)

C.. Entry hook for ATLSIM
      
      ENTRY AGUKINE

 200  CONTINUE
        IACC = 0       
        CALL COSGEN(EMIN,EMAX,IACC)
      IF(IACC.EQ.0 ) GOTO 200
      
      IF(CHRG.GT.0.) THEN
        IPART = 5
      ELSE
        IPART = 6
      ENDIF

c..      Call GFPART (IPART,CNAME,ITRTYP,AMASS,CHARGE,TLIFE,UB,NU)
      SINTH   = SQRT(ABS(1.-COSTH**2))
      PLAB(1) = ENER * SINTH * SIN(PHI)
      PLAB(2) = (-1.) * ENER * COSTH
      PLAB(3) = ENER * SINTH * COS(PHI)
      
*.. Pick the location of vertex in x,z for the cosmic.
C      ZERO(1) = (-1. + 2.*RNDM(1)) * XBOX/2.
      ZERO(1) = (-1. + 2.*COSMICRNDM(1)) * XBOX/2.
      ZERO(2) = YHEIGHT
C      ZERO(3) = (-1. + 2.*RNDM(1)) * ZBOX/2.
      ZERO(3) = (-1. + 2.*COSMICRNDM(1)) * ZBOX/2.
*        Zero(4) = Rndm(1) * 25.e-9  ! Random 25 nsec time offsets in case that matters.
      ZERO(4) = 0.
      
      DXDY   = PLAB(1) / PLAB(2)
      DZDY   = PLAB(3) / PLAB(2)
      XYZERO = ZERO(1) - DXDY*ZERO(2)
      ZYZERO = ZERO(3) - DZDY*ZERO(2)

C      WRITE(*,*)' PLAB  = ', PLAB(1), PLAB(2), PLAB(3)
C      WRITE(*,*)' ZERO  = ', ZERO(1), ZERO(2), ZERO(3), ZERO(4)
C      WRITE(*,*)' X,Z(0)= ', XYZERO, ZYZERO
      
C       CALL HF1(10001,ENER,1.)
C       CALL HF1(10002,COSTH,1.)
C       CALL HF2(10003,ZERO(1),ZERO(3),1.)
C       CALL HF1(10004,PLAB(1),1.)
C       CALL HF1(10005,PLAB(2),1.)
C       CALL HF1(10006,PLAB(3),1.)
      
C       IF(ABS(XYZERO).LT.3000. .AND. ABS(ZYZERO).LT.3000. )THEN
C c..        CALL GSVERT(ZERO,0,0,UB,0,IVERT)
C c..        CALL GSKINE(PLAB,IPART,IVERT,UB,0,ITR)
C         CALL HF1(12001,ENER,1.)
C         CALL HF1(12002,COSTH,1.)
C         CALL HF2(12003,ZERO(1),ZERO(3),1.)
C         CALL HF1(12004,PLAB(1),1.)
C         CALL HF1(12005,PLAB(2),1.)
C         CALL HF1(12006,PLAB(3),1.)
C       ENDIF

      RETURN
      END
*
      SUBROUTINE COSGIN
C ----------------------------------------------------------------------      
      IMPLICIT NONE

      REAL PI, TWOPI, RADEG, DEGRAD
      COMMON/COSCON/PI,TWOPI,RADEG,DEGRAD
      REAL EMIN,EMAX,ECUT,RVRT,ZVRT
      COMMON/COSPAR/ EMIN,EMAX,ECUT, RVRT , ZVRT
      
      REAL    LBINWID, LEMIN, LEMAX, PROBE
      INTEGER NBIN
      COMMON /GENPAR/ LBINWID, LEMIN, LEMAX, NBIN, PROBE(100)

      REAL MAXBIN,EBIN,DEBIN
      REAL COSDIA
      INTEGER I
      
      PI     = ATAN(1.)*4.
      TWOPI  = 2.*PI
      RADEG  = 180./PI
      DEGRAD = PI/180.

      MAXBIN = 0.0
      DO 10 I=1,NBIN
        EBIN  = 10.0**(LEMIN+(FLOAT(I)-0.5)*LBINWID)
        DEBIN = 10.0**(LEMIN+FLOAT(I)*LBINWID) -
     +          10.0**(LEMIN+(FLOAT(I)-1.0)*LBINWID)
        PROBE(I) = COSDIA(EBIN) * DEBIN
        IF(MAXBIN.LT.PROBE(I)) MAXBIN = PROBE(I)
 10   CONTINUE

      DO 20 I=1,NBIN
        PROBE(I) = PROBE(I) / MAXBIN
 20   CONTINUE


      print*,'initialisation done..',lemin,lemax

      RETURN
      END
*
      SUBROUTINE COSGEN(EMIN,EMAX,IACC)
C ----------------------------------------------------------------------
      IMPLICIT NONE
      REAL EMIN,EMAX
      INTEGER IACC

      REAL ENER, COSTH, PHI, CHRG
      COMMON /COSEVT/ ENER ,COSTH , PHI , CHRG
      REAL EMI,EMA,ECUT, RVRT , ZVRT
      COMMON /COSPAR/ EMI,EMA,ECUT, RVRT , ZVRT
      INTEGER IOU, IDB
      COMMON /COSIO/  IOU ,IDB
      REAL PI,TWOPI,RADEG,DEGRAD
      COMMON /COSCON/ PI,TWOPI,RADEG,DEGRAD
      REAL CTCUT
      COMMON /COSCUT/ CTCUT

      REAL    LBINWID, LEMIN, LEMAX, PROBE
      INTEGER NBIN
      COMMON /GENPAR/ LBINWID, LEMIN, LEMAX, NBIN, PROBE(100)

C      REAL LETMP, RNDM, GAMMA, COSANG
      REAL COSMICRNDM, GAMMA
      REAL THETA,ANG_SCALE,WQPOS,WQNEG,ANG_TEST,PROB_to_E,prob,ANG_FLUX

      INTEGER I
C
C  Arguments of COSMICRNDM
C      REAL   POSMU/0.42/
C      INTEGER J
C        IF (COSMICRNDM(J).GT.POSMU) CHRG = 1.
C
C  Lost to the mists of time
C      REAL   DEPTH/68./,DENSI/30000./
C      REAL   COSINI
C      INTEGER IBIN
C
      IACC = 0



C     
C - Generate the muon energy
C
 10   CONTINUE
CC        LETMP = LEMIN + RNDM(1)*(LEMAX-LEMIN)
C        LETMP = LEMIN + COSMICRNDM(1)*(LEMAX-LEMIN)
C        IBIN = INT((LETMP-LEMIN)/LBINWID) + 1
C        IF(IBIN.LT.1) IBIN=1
C        IF(IBIN.GT.NBIN) IBIN=NBIN
CC      IF(RNDM(1).GT.PROBE(IBIN) )GOTO 10
C      IF(COSMICRNDM(1).GT.PROBE(IBIN) )GOTO 10
C      ENER = 10**LETMP
C - Get random number and use Prob_to_E to convert to Energy
C
      prob = COSMICRNDM(I)
      ENER = PROB_to_E(prob,EMIN)
      if (ENER .GT. EMAX) GOTO 10

C
C - Get exponent for the zenith angle (THETA) dependence for this energy
C
      
CCC corrected as per Jim Mueller and Hong
CCC        GAMMA=COSANG(ENER) + 1.
C        GAMMA=COSANG(ENER) + 2.
C        IF (GAMMA.LT.0.1) GAMMA=0.1
CC
CC - Generate the zenith angle (THET)
CC
CC        COSTH = RNDM(I)**(1./GAMMA)
C        COSTH = COSMICRNDM(I)**(1./GAMMA)
C
C - This uses full calculation from Dar.
C - Just acceptance rejection method
C - Assumes flux at theta=0.0 always gt costh*flux for theta>0
C - All plots I have made look like this, but ....
C
      CALL CDARFX(ENER,0.0,WQPOS,WQNEG)
      ANG_SCALE = (WQPOS+WQNEG)*1.000001
 11   CONTINUE
      COSTH = COSMICRNDM(I)
      THETA = ACOS(COSTH)
      CALL CDARFX(ENER,THETA,WQPOS,WQNEG)
      ANG_FLUX = COSTH*(WQPOS+WQNEG)
      IF(ANG_SCALE .LT. ANG_FLUX) THEN
         WRITE(IOU,1003)ENER,COSTH,ANG_FLUX,ANG_SCALE
 1003    FORMAT(1X,'Flux from CDARFX GT assumed maximum/',
     1        1X,'E,costh,flux,max',4E15.8)
      ENDIF
      ANG_TEST = COSMICRNDM(I)*ANG_SCALE
      IF(ANG_TEST .GT. ANG_FLUX) GOTO 11
      IF(ANG_TEST .GT. COSTH*WQPOS) THEN
         CHRG = -1.
      ELSE
         CHRG = 1.
      ENDIF
        
        
        IF( COSTH .LT. CTCUT ) GOTO 999
        IF( ENER.LT.EMIN ) GOTO 999
        IF( ENER.GT.EMAX ) GOTO 999
        
        IACC = 1
C
C - Generate the azimuthal angle (PHI)
C
C        PHI = TWOPI*RNDM(I)
        PHI = TWOPI*COSMICRNDM(I)
C
C - Generate the muon charge  (CHRG)
C
C        CHRG = -1.
CC        IF (RNDM(J).GT.POSMU) CHRG = 1.
C        IF (COSMICRNDM(J).GT.POSMU) CHRG = 1.
        IF(IDB.GT.0) WRITE(IOU,1002) IACC,ENER,COSTH,PHI,CHRG,GAMMA
1002    FORMAT(1X,'ENERGY,COST,PHI,CHARGE,GAMMA',I10,5X,5E10.3)
 999  CONTINUE
      RETURN
      END
*
      SUBROUTINE COSIPR
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the differential flux of athmospheric muons at sea level
C.   Arguments : EMU   (input)  final muon energy at sea level in GeV
C.               THEMU (input)  zenith angle in radian
C.               WQPOS (output) pos. muon flux in (GeV*cm**2*sec*sr)**-1
C.               WQNEG (output) neg. muon flux in (GeV*cm**2*sec*sr)**-1
C.
C? - The muon flux is analytically calculated following a paper of
C?      A. Dar ,  Phys. Rev. Letters 51, 227 (1983)
C?   and further references therein.
C.
      PARAMETER (NEVAL = 22)
      COMMON /COSIO/ IOU,IDB
      DIMENSION EVAL(NEVAL),FDIF(NEVAL),FINT(NEVAL),DANG(NEVAL)
      DIMENSION DINT(NEVAL),DAIN(NEVAL)
C
C- The measurements of the differential vertical muon cross section
C- have been interpolated at 22 energy values
C- Allkofer et al. Phys. Lett. B36, 425 (1971)
C
C  energy values in GeV
C
      DATA EVAL/ 0.2, 0.4, 0.8, 1.0, 1.5, 2.0, 3.0, 5.0, 7.0,10.0, 15.0,
     +           20., 30., 50., 70.,100.,150.,200.,300.,500.,700.,1000./
C
C  Differential vertical flux (cm**2 sr sec GeV)**-1  from Allkofer
C
      DATA FDIF/0.373E-02,0.372E-02,0.310E-02,0.279E-02,0.214E-02,
     +          0.167E-02,0.106E-02,0.497E-03,0.273E-03,0.133E-03,
     +          0.540E-04,0.270E-04,0.959E-05,0.236E-05,0.892E-06,
     +          0.304E-06,0.851E-07,0.335E-07,0.870E-08,0.152E-08,
     +          0.471E-09,0.134E-09/
C
C  Integral vertical flux (cm**2 sr sec)**-1          from Allkofer
C
      DATA FINT/0.994E-02,0.918E-02,0.781E-02,0.722E-02,0.600E-02,
     +          0.505E-02,0.372E-02,0.226E-02,0.152E-02,0.942E-03,
     +          0.513E-03,0.321E-03,0.157E-03,0.593E-04,0.298E-04,
     +          0.138E-04,0.555E-05,0.284E-05,0.107E-05,0.303E-06,
     +          0.130E-06,0.523E-07/
C
C  Integral vertical flux (cm**2 sr sec)**-1          from CDARFX
C
      DATA DINT/0.109E-01,0.100E-01,0.840E-02,0.757E-02,0.641E-02,
     +          0.541E-02,0.397E-02,0.239E-02,0.159E-02,0.976E-03,
     +          0.524E-03,0.325E-03,0.158E-03,0.586E-04,0.292E-04,
     +          0.135E-04,0.543E-05,0.277E-05,0.105E-05,0.300E-06,
     +          0.129E-06,0.523E-07/
C
C   Differential flux integrated over all angles using the
C   calculations of Dar                               from CDARFX
C
C      DATA DANG/.4340E-02,.4551E-02,.4516E-02,.4363E-02,.3840E-02,
C     +          .3292E-02,.2388E-02,.1328E-02,.8104E-03,.4415E-03,
C     +          .2018E-03,.1099E-03,.4380E-04,.1252E-04,.5220E-05,
C     +          .1986E-05,.6332E-06,.2741E-06,.8152E-07,.1688E-07,
C     +          .5835E-08,.1859E-08/
C
C This includes extra cos(theta) for flux through Horizontal surface
C
      DATA DANG/.3553E-02,.3747E-02,.3695E-02,.3551E-02,.3084E-02,
     +          .2610E-02,.1850E-02,.9902E-03,.5869E-03,.3089E-03,
     +          .1353E-03,.7138E-04,.2713E-04,.7278E-05,.2903E-05,
     +          .1051E-05,.3152E-06,.1303E-06,.3623E-07,.6855E-08,
     +          .2229E-08,.6648E-09/
C
C  Integral total flux (cm**2 sr sec)**-1             from CDARFX
C
C      DATA DAIN/0.213E-01,0.205E-01,0.186E-01,0.178E-01,0.157E-01,
C     +          0.139E-01,0.112E-01,0.763E-02,0.556E-02,0.377E-02,
C     +          0.228E-02,0.154E-02,0.839E-03,0.365E-03,0.202E-03,
C     +          0.105E-03,0.478E-04,0.268E-04,0.116E-04,0.387E-05,
C    +          0.185E-05,0.842E-06/
C
C  This includes extra cos(theta) for flux through Horizontal surface
C
      DATA DAIN/0.162E-01,0.155E-01,0.140E-01,0.133E-01,0.116E-01,
     +          0.102E-01,0.800E-02,0.527E-02,0.374E-02,0.246E-02,
     +          0.143E-02,0.931E-03,0.486E-03,0.198E-03,0.105E-03,
     +          0.515E-04,0.221E-04,0.118E-04,0.475E-05,0.144E-05,
     +          0.645E-06,0.270E-06/
C
      WRITE(IOU,6001)
6001  FORMAT(//' Comparison of measured and parametrized cosmic',
     +         ' muon fluxes at sea level',/,
     +         ' ==============================================',
     +         '=========================',///,
     +  10x,'A. Vertical fluxes ',/,
     +  10x,'------------------',//,
     +  ' 1-3. Differential flux as :'/
     +  '  1. measured by Allkofer et al, Phys. Lett. 36B (1971), 425'/
     +  '  2. predicted from the primary hadron flux by'/
     +  '     A. Dar, Phys. Rev. Lett. 51 (1983), 227 (CDARFX)',/,
     +  '  3. calculated from a simple parametrization',
     +        ' of the measured flux (COSDIF)',//,
     +  ' 4-6. Integral flux as calculated :'/
     +  '  4. by Allkofer from the differential flux',
     +        ' assuming a E**-Alpha dependence',/,
     +  '  5. using the DAR parametrization'/
     +  '  6. from a simple parametrization of the measured flux',
     +        ' (COSINT)',///,
     + 4X,'Energy',5x,'Differential vertical flux',
     +            10x,'Integral vertical flux'/,
     + 5X,'(GeV)', 7x,'(1./GeV*cm**2*sec*sr)',15x,'(1./cm**2*sec*sr)'/)

C    F    '     PARAMETRIZED     PARAM_INTEG     PARAM_D_INTEG'//)
C    F ' THE DIFFERENTIAL BY THE PROGRAM ASSUMING E**-ALF FORM'/
C    F 2X,'PARAMETRIZED IS THE DIFFERENTIAL (AT 0 DEG) GIVEN BY A.DAR ',
C    F 'PHYS.REV.LETT. 51 (1983) 227',
C    F /2X,'PARAM_INTEG IS DAR-S PARAMETRIZATION INTEGRATED OVER ALL',
C    F ' ANGLES'/2X,'PARAM_D_INTEG IS PARAM_INTEG INTEGRATED OVER'
C    F ,' ENERGIES ASSUMING E**-ALF DEPENDENCE)'
C
      DO 100 I=1,NEVAL
        EMU = EVAL(I)
        CALL CDARFX(EMU,0.,WQPOS,WQNEG)
        DDIF = WQPOS + WQNEG
        WRITE(IOU,6002) EMU,FDIF(I),DDIF,COSDIF(EMU),
     X                      FINT(I),DINT(I),COSINT(EMU)
6002    FORMAT(F9.1, 2X,3E11.3, 2X,3E11.3)
 100  CONTINUE
*     - - - - - - - - - - - - - - - - - - - -
      WRITE(IOU,6003)
6003  FORMAT(///10X,'B. Fluxes integrated over the angle',/,
     +          10X,'-----------------------------------',//
     +  ' 1. Differential flux calculated using Dar.s model'/,
     +  '    integrated over the whole angle range'/,
     +  ' 2. Differential flux as calculated from a simple'/,
     +  '    parametrization of the measured flux (COSDIA)',/,
     +  ' 3. Total integral flux as a function of the cut-off'/,
     +  '    energy using a the DAR parametrization (CDARFX)'/,
     +  ' 4. Total integral flux as a function of the cut-off'/,
     +  '    energy using a simple parametrization (COSINA)',/,
     + /4X,'Energy   Gamma',7x,'Differential flux',12x,'Integral flux',
     + /4X,' (GeV)',    15x,   '(1/GeV*cm**2*sec)',12x,'(1/cm**2*sec)',
     + //)
      DO 200 I=1,NEVAL-1
        DAIN(I) = DAIN(I+1)+FLUINT(EVAL(I),EVAL(I+1),DANG(I),DANG(I+1))
        EMU = EVAL(I)
        WRITE(IOU,6004) EMU,COSANG(EMU),DANG(I),COSDIA(EMU),DAIN(I),
     >                  COSINA(EMU)
6004    FORMAT(F9.1,F9.3, 2x,2E12.3, 2X,2E12.3)
 200  CONTINUE
*
      WRITE(IOU,6102)
6102  FORMAT(/,2X,'FOR COMPARISON : MEASURED TOTAL FLUX ABOVE 0.35 GEV',
     F/19X,'BY ALLKOFER ET AL. COSMIC RAYS ON EARTH,ISSN 0344-8401'/19X
     F      ,'J2 = (1.90 +- 0.12).E-2'//)
      END
*
      SUBROUTINE CDARFX(EMU,THEMU,WQPOS,WQNEG)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the differential flux of athmospheric muons at sea level
C.   Arguments : EMU   (input)  final muon energy at sea level in GeV
C.               THEMU (input)  zenith angle in radian
C.               WQPOS (output) pos. muon flux in (GeV*cm**2*sec*sr)**-1
C.               WQNEG (output) neg. muon flux in (GeV*cm**2*sec*sr)**-1
C.
C? - The muon flux is analytically calculated following a paper of
C?      A. Dar ,  Phys. Rev. Letters 51, 227 (1983)
C?   and further references therein.
C.
      DIMENSION BRM(4),AM(4),AMSQ(4),CTM(4),GHH(5),DGHH(5)
C        Branching Ratios for  M --> mu nu   for Pions and Kaons
      DATA BRM       /2*1.      , 2*0.632   /
C                     Mass values for M in GeV/c**2
      DATA AM        /2*0.139567, 2*0.493669/
C                    Mass squared
      DATA AMSQ      /2*0.019479, 2*0.243710/
C                    c*tau for M
      DATA CTM       /2*7.804   , 2*3.709   /
C       muon            mass             mass**2              c*tau
      DATA AMMU      /0.10566/ , AMUSQ /0.011164/ , CTMU   /658.68/
C       scale parameter of the upper athmosphere   in m
      DATA H0        /6300./
C       energy loss in athmosphere     in    GeV*m**2/kg
      DATA ALPHA     /2.06E-04/
C       constants to calculate athmospheric depths  in    kg/m**2
      DATA ALAM0     /1200./,  ALAM1 /10300./
C
C      Constants to calculate the production coefficients in the athm.
C      Values taken from A. Liland Intern. Cosmic Ray Conf. Kyoto 1979
C      Vol 13, p. 353
C
C                  p pi+   p pi-   p K+     p K-     p N
      DATA GHH / .04190, .03014, .007354, .002633, .3175/
      DATA DGHH/ .00655,-.01019,-.006456,-.003179, .1815/
C
C  Assume a primary flux of 1.6 E**-2.67 nucleons/cm**2 sec sr GeV
C
      AKM(E,GM) = 1.6*E**(-2.67)/(2.67+(2.67+1)*GM*E)
C  Final athmospheric depths
      ALAMF = ALAM1/COS(THEMU)
C  Muon momentum at sea level
      PMU   = SQRT(EMU**2 - AMUSQ)
C  Initial muon energy
      E0    = EMU + ALPHA*(ALAMF - ALAM0)
C  Average energy in athmosphere
      EBAR  = EMU + ALPHA*(ALAMF - ALAM0)/2.
C  Gamma mu
      GAMMU = COS(THEMU)*CTMU/(AMMU*H0)
C  Probability to reach sea level before decay
      SMU   =  (ALAM0/ALAMF)**(1./(GAMMU*EBAR))
C
C  Calculate contributions from the 4 decay processes
C
      GN = GHH(5) + DGHH(5)/SQRT(E0)
      WQPOS = 0.
      WQNEG = 0.
      DO 100 I = 1,4
        GM   = GHH(I) + DGHH(I)/SQRT(E0)
C  Multiply by 1.15 for nuclear enhancement of multiplicity in air coll.
        GMAT = GM/(1.-GN) * 1.15
C  gamma M
        GAMM = COS(THEMU)*CTM(I)/(AM(I)*H0)
C  beta  M
        BETM = AMSQ(I)/AMUSQ
C  alpha M
        ALPHM = AMSQ(I)/(AMSQ(I) - AMUSQ)
C
        DFMDE = GMAT*BRM(I)*ALPHM*SMU*(AKM(E0,GAMM)-AKM(BETM*E0,GAMM))
        IF (I.EQ.1.OR.I.EQ.3) THEN
          WQPOS = WQPOS + DFMDE
        ELSE
          WQNEG = WQNEG + DFMDE
        ENDIF
100   CONTINUE
        END
*
      FUNCTION COSANG(EMU)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the angular (theta) dependence of cosmic muon
C.   Arguments : EMU   (input)  muon energy at sea level in GeV
C? - The angular dependence as a function of energy is calculated
C?   The flux can be written in a factorized form
C?   FLUX(ENERGY,THETA) =  FLUX(ENERGY)*COS (THETA)) ** GAMMA(ENERGY)
C?    The function returns the energie dependent value of gamma
C.
      REAL * 8 COT,ALE,DE
      DIMENSION COT(4)
      DATA COT/3.4240,-0.7730,-0.051078,0.011682/
      DE     = EMU
      ALE    = DLOG(DE)
      COSANG = COT(1) + COT(2)*ALE + COT(3)*ALE*ALE + COT(4)*ALE*ALE*ALE
      RETURN
      END
*
      FUNCTION COSINA(EMU)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the integral flux integrated over all angles
C.   Arguments : EMU   (input)  muon energy at sea level in GeV
C? - The total flux integrated over all angles as a function of E
C?   is calculated. The function parametrizes the measured integral
C?   flux. The calculated values agree within few percent
C?   with the actual measurements.
C?
C?   The function value contains the differential flux in 1/(cm**2 * sec )
C
C      REAL*8 COE,DE,ALE,ALE2,ALE3,ADI
C      DIMENSION COE(4)
CC
CC commented values for int over angles
CC new values fit to flux through horizontal surface, int over angles
CC -J. Mueller
CC
CC      DATA COE/-3.9526  ,-0.30382 ,-.19817 ,0.0043692/
C      DATA COE/-4.2140  ,-0.30105 ,-.23861 ,0.0078817/
C      DE     = EMU
C      ALE    = DLOG(DE)
C      ALE2   = ALE*ALE
C      ALE3   = ALE2*ALE
C      ADI    = COE(1) + COE(2)*ALE + COE(3)*ALE2 + COE(4)*ALE3
C      COSINA = DEXP(ADI)
C      RETURN
C      END
C
C New Code just uses a lookup table in log(Flux) vs log(E_muon/Mass_muon)
C along with linear interpotaion between bins. An extrapolation using the power
C law from the primary flux is used beyond the top bin (E>346TeV).
C
C The table was calculated by numerically integrating results from CDARFX.
C The table should really be calculated and filled at initiallization time.
C Perhaps that could be added in the future.
C
      REAL EMU
      INTEGER ibin
      REAL*8 logE_scale,deltaE,logflux
      integer BINMAX
      PARAMETER (BINMAX=150)
      REAL*8 BINWIDTH,M_muon
      PARAMETER (BINWIDTH=0.1)
      PARAMETER (M_muon=0.10565837)
      REAL E2FLUX(0:BINMAX)

      DATA E2FLUX/-4.09495,
     1     -4.09722,-4.09976,-4.10260,-4.10576,-4.10930,
     +     -4.11326,-4.11769,-4.12266,-4.12822,-4.13445,
     2     -4.14144,-4.14929,-4.15809,-4.16796,-4.17903,
     +     -4.19145,-4.20535,-4.22093,-4.23834,-4.25779,
     3     -4.27947,-4.30360,-4.33039,-4.36007,-4.39286,
     +     -4.42899,-4.46867,-4.51211,-4.55951,-4.61105,
     4     -4.66690,-4.72720,-4.79207,-4.86162,-4.93591,
     +     -5.01501,-5.09894,-5.18770,-5.28129,-5.37967,
     5     -5.48280,-5.59061,-5.70303,-5.81999,-5.94139,
     +     -6.06713,-6.19713,-6.33128,-6.46949,-6.61167,
     6     -6.75771,-6.90754,-7.06106,-7.21820,-7.37887,
     +     -7.54301,-7.71055,-7.88142,-8.05556,-8.23291,
     7     -8.41342,-8.59702,-8.78367,-8.97331,-9.16587,
     +     -9.36131,-9.55957,-9.76059,-9.96429,-10.1706,
     8     -10.3795,-10.5909,-10.8047,-11.0209,-11.2393,
     +     -11.4599,-11.6826,-11.9074,-12.1342,-12.3629,
     9     -12.5933,-12.8256,-13.0595,-13.2950,-13.5321,
     +     -13.7707,-14.0107,-14.2520,-14.4947,-14.7386,
     +     -14.9837,-15.2300,-15.4773,-15.7258,-15.9752,
     +     -16.2256,-16.4769,-16.7291,-16.9822,-17.2360,
     1     -17.4907,-17.7460,-18.0021,-18.2588,-18.5161,
     +     -18.7740,-19.0325,-19.2916,-19.5511,-19.8112,
     2     -20.0716,-20.3325,-20.5938,-20.8555,-21.1176,
     +     -21.3799,-21.6426,-21.9056,-22.1689,-22.4324,
     3     -22.6962,-22.9602,-23.2244,-23.4889,-23.7535,
     +     -24.0183,-24.2832,-24.5483,-24.8136,-25.0790,
     4     -25.3445,-25.6101,-25.8758,-26.1417,-26.4076,
     +     -26.6737,-26.9398,-27.2060,-27.4723,-27.7387,
     5     -28.0052,-28.2718,-28.5385,-28.8053,-29.0722,
     +     -29.3393,-29.6067,-29.8742,-30.1421,-30.4104/

      logE_scale = DLOG(EMU/M_muon)/BINWIDTH
      ibin = int(logE_scale)
      deltaE = logE_scale - ibin
      IF (logE_scale .LE. 0.0) THEN
         logflux = E2FLUX(0)
      ELSE IF (ibin .LT. BINMAX) THEN
         logflux = E2FLUX(ibin) + deltaE * (E2FLUX(ibin+1)-E2FLUX(ibin))
      ELSE
         deltaE = deltaE + ibin-BINMAX
         logflux = E2FLUX(BINMAX) + deltaE*2.67/BINWIDTH
      ENDIF

      COSINA = DEXP(logflux)

      RETURN
      END
*
      REAL FUNCTION COSDIA(EMU)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the differential flux integrated over all angles
C.   Arguments : EMU   (input)  muon energy at sea level in GeV
C? - The differential flux integrated ver all angles as a function of E
C?   is calculated. The function parametrizes the measured differential
C?   flux. The calculated values agree within few percent
C?   with the actual measurements.
C?
C?   The function value contains the differential flux in 1/(GeV*cm**2*sec)
C
      IMPLICIT NONE
      REAL EMU
      REAL*8 COE,DE,ALE,ALE2,ALE3,ADI
      DIMENSION COE(4)
C
C commented values for int over angles
C new values fit to flux through horizontal surface, int over angles
C -J. Mueller
C
C      DATA COE/-5.3229  ,-0.35023 ,-.34803 ,0.012680/
      DATA COE/-5.5178  ,-0.36625 ,-.37610 ,0.014775/
      DE     = EMU
      ALE    = DLOG(DE)
      ALE2   = ALE*ALE
      ALE3   = ALE2*ALE
      ADI    = COE(1) + COE(2)*ALE + COE(3)*ALE2 + COE(4)*ALE3
      COSDIA = DEXP(ADI)
      RETURN
      END
*
      FUNCTION COSDIF(EMU)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the differential vertical flux
C.   Arguments : EMU   (input)  muon energy at sea level in GeV
C? - The differential vertical flux as a function of energy is
C?   calculated. The function parametrizes the measured differential
C?   vertical flux. The calculated values agree within few percent
C?   with the actual measurements.
C?
C?   The function value returns the differential vertical flux in 
C?    1/(GeV*cm**2*sec*sr)
C
      REAL*8 COE,DE,ALE,ALE2,ALE3,ADI
      DIMENSION COE(4)
      DATA COE/-5.8339  ,-0.51588 ,-.40199 ,0.017717/
      DE     = EMU
      ALE    = DLOG(DE)
      ALE2   = ALE*ALE
      ALE3   = ALE2*ALE
      ADI    = COE(1) + COE(2)*ALE + COE(3)*ALE2 + COE(4)*ALE3
      COSDIF = DEXP(ADI)
      RETURN
      END
*
      FUNCTION COSINT(EMU)
C ----------------------------------------------------------------------
C. - Author   : A. Putzer  - 87/12/14
C! - Calculates the integral vertical flux
C.   Arguments : EMU   (input)  muon energy at sea level in GeV
C? - The integral vertical flux as a function of energy is
C?   calculated. The function parametrizes the measured integral
C?   vertical flux. The calculated values agree within few percent
C?   with the actual measurements.
C?
C?   The function value returns the integral vertical flux in
C?          1./(cm**2*sec*sr)
C.
C
      REAL*8 COE,DE,ALE,ALE2,ALE3,ADI
      DIMENSION COE(4)
      DATA COE/-4.8597  ,-0.44915 ,-.22676 ,0.0059028/
      DE     = EMU
      ALE    = DLOG(DE)
      ALE2   = ALE*ALE
      ALE3   = ALE2*ALE
      ADI    = COE(1) + COE(2)*ALE + COE(3)*ALE2 + COE(4)*ALE3
      COSINT = DEXP(ADI)
      RETURN
      END
*
         REAL FUNCTION FLUINT(E1,E2,F1,F2)
C        ============================
C
C --- Given E1 and E2 lower and upper energy limits and F1 and F2 the
C        corresponding differentiald istribution dN/dE = a*E**-alf
C        at E1 and E2. Then FLUINT = Integral dN/dE from E1 to E2
C
         IMPLICIT NONE
         REAL E1,E2,F1,F2
         FLUINT = (E1*F1-E2*F2)/((-ALOG(F1/F2))/ALOG(E1/E2)-1.)
         RETURN
         END
******************************************************************************
      REAL FUNCTION PROB_to_E(prob,Emin)
C Given a number between 0<prob<1, return the energy for which the probability
C of the muon being > Energy is prob.
C
C Function uses a lookup table in log(E_muon/Mass_muon) vs log(1/Probability)
C along with linear interpotaion between bins. An extrapolation using the power
C law from the primary flux in CDARFX is used beyond the top bin (E>5TeV).
C The input probability is scaled so that an input prob=1.0 will return Emin. 
C
C The table was calculated by numerically integrating results from CDARFX.
C The table has an implicit zero bin (prob(E_mu >= M_mu)==1).  Interpolation
C between that and the first bin (E=539 MeV) might yeild small deviations from
C the true curve, if anyone cares.
C The table should really be calculated and filled at initiallization time.
C Perhaps that could be added in the future.
C

      implicit none

      REAL prob,emin
      REAL COSINA,M_muon
      INTEGER ibin
      REAL*8 LOGp_scale,deltap,LOGE,prob_emin
      integer BINMAX
      real*8 BINWIDTH
      PARAMETER ( BINMAX=150)
      PARAMETER ( BINWIDTH=0.1)
      PARAMETER ( M_muon=0.10565837)
      REAL PROB2E(BINMAX)
      DATA PROB2E/
     1     1.6259, 2.1652, 2.5056, 2.7614, 2.9695,
     +     3.1472, 3.3038, 3.4454, 3.5751, 3.6954,
     2     3.8075, 3.9137, 4.0147, 4.1110, 4.2035,
     +     4.2929, 4.3788, 4.4620, 4.5427, 4.6213,
     3     4.6983, 4.7731, 4.8461, 4.9177, 4.9883,
     +     5.0571, 5.1247, 5.1916, 5.2570, 5.3214,
     4     5.3853, 5.4478, 5.5094, 5.5708, 5.6309,
     +     5.6907, 5.7494, 5.8074, 5.8652, 5.9220,
     5     5.9786, 6.0342, 6.0898, 6.1443, 6.1989,
     +     6.2524, 6.3055, 6.3587, 6.4109, 6.4631,
     6     6.5145, 6.5660, 6.6166, 6.6674, 6.7173,
     +     6.7673, 6.8165, 6.8659, 6.9145, 6.9633,
     7     7.0113, 7.0594, 7.1069, 7.1545, 7.2014,
     +     7.2485, 7.2954, 7.3416, 7.3880, 7.4337,
     8     7.4797, 7.5249, 7.5705, 7.6154, 7.6605,
     +     7.7050, 7.7498, 7.7944, 7.8384, 7.8827,
     9     7.9263, 7.9702, 8.0135, 8.0572, 8.1002,
     +     8.1435, 8.1868, 8.2294, 8.2723, 8.3147,
     +     8.3574, 8.4000, 8.4419, 8.4843, 8.5260,
     +     8.5682, 8.6097, 8.6516, 8.6934, 8.7347,
     1     8.7763, 8.8173, 8.8588, 8.9000, 8.9409,
     +     8.9820, 9.0226, 9.0637, 9.1041, 9.1449,
     2     9.1857, 9.2259, 9.2666, 9.3066, 9.3471,
     +     9.3876, 9.4274, 9.4677, 9.5074, 9.5476,
     3     9.5877, 9.6273, 9.6672, 9.7067, 9.7466,
     +     9.7864, 9.8257, 9.8654, 9.9046, 9.9442,
     4     9.9837, 10.023, 10.062, 10.101, 10.141,
     +     10.180, 10.219, 10.258, 10.297, 10.336,
     5     10.375, 10.414, 10.453, 10.492, 10.530,
     +     10.569, 10.608, 10.647, 10.685, 10.724/


      PROB_emin = COSINA(EMIN)/COSINA(M_muon)
      LOGp_scale = (-Dlog(prob*PROB_emin))/BINWIDTH
      ibin = int(LOGp_scale)
      deltap = LOGp_scale - ibin
      IF (ibin .LT. 1) THEN
         LOGE = deltap * PROB2E(1)
      ELSE IF(ibin .LT. BINMAX) THEN
         LOGE = PROB2E(ibin) + deltap * (PROB2E(ibin+1)-PROB2e(ibin))
      ELSE
         deltap = deltap + ibin-BINMAX
         LOGE = PROB2E(BINMAX) + deltap/2.67/BINWIDTH
      ENDIF

      PROB_to_E = DEXP(LOGE)*M_muon
      
      RETURN
      END
