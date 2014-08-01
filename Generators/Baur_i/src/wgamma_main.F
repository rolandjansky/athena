
C
C   MONTE CARLO FOR ZW AND W-GAMMA PRODUCTION FROM q qbar
C
C   MAIN PROGRAM ONLY!!!
C
C   NEW VERSION WITH HBOOK4 SWITCH
C
C   LEPTON MASSES ADDED 10/30/94
C
C   PDFLIB_V5 UPGRADE 03/04/95
C

      subroutine baurmain
      IMPLICIT REAL*8 (A-H,O-Y), COMPLEX*16 (Z)
      REAL*4 HMEMOR(50000)
      INTEGER FINDEX(4), BOS(2)
      CHARACTER YN*1
      CHARACTER*20 PARM(20)
      REAL*8 VAL(20)
      PARAMETER (NHMAX=16,NBIN=50,NYY=17) ! NYY=IT1+IT2+2
c      REAL*4 XUNP(NBIN)
      LOGICAL SWACRT, SWPAR, SWWGT
CNAHO
      CHARACTER outputfilename*30
      LOGICAL   formatted
C      integer test(2)
      INTEGER BOSON(2)
CNAHOEND
      COMPLEX*16 XK(2), XL(2), G1(2), G4(2), G5(2), G6(2), G7(2)
      COMMON /PAWC/ HMEMOR
      COMMON /BWW1/ G1, XK, XL, G4, G5, G6, G7
      COMMON /BINIT/ ALFAI,FINDEX,BOS
      COMMON /BPSCUT/ YCUT,TAUMIN,TAUMAX,QSQMAX,EPHMIN,ECLMIN,EJTMIN, 
     *                PTC,CUTMASS,PTLCUT,PTMCUT,XMTCUT,YLCUT,XMTECUT
      COMMON /BCOMP/ SCCOMP, XNPARC, XNPARV, XN, SCOS 
      COMMON /BCONST/ ECM,S,XMXWGT,IBEAMS,IPCH, BRFAC, XLUMI
      COMMON /KOPLIN/ ALFA,SIN2W,ALFAS,XMZ,XMW,XMT
      COMMON /BHIST/ NEWHIS0
      COMMON /SWITCH/ SWPAR, SWACRT, SWWGT
      COMMON /BVEGAUX/ ALPH1,ALPH2,NCALL1,NCALL2,IT1,IT2
      COMMON /BRSEP/ RJLMIN
      COMMON /BHBOOK/ IDH
      COMMON /IGENE/ IGEN


CNAHO
C      COMMON /WGTMAX/ WMAX
      COMMON /OUTFILE/ outputfilename
      COMMON /OUTFILEFORMAT/ formatted
C      common /par_test/test
      common /bosons/ BOSON      
      INTEGER IJKLIN, NTOTIN, NTO2IN ! random number initialization 
c      INTEGER IJKLUT, NTOTUT, NTO2UT ! random
C See  http://wwwinfo.cern.ch/asdoc/shortwrupsdir/v113/top.html
CNAHOEND
      open (unit = 31, file = "wgamma_card.in")
            
C 
C Enter all input parameters
      Print *,'Read all input parameters for this event generation'
Cab      PRINT *,' Enter HBOOK3 (1) or HBOOK4 (2)'
      READ (31,*) IDH
      PRINT *,'      HBOOK3 (1) or HBOOK4 (2) =', IDH
      IF (IDH.EQ.2) CALL HLIMIT(50000)
Cab      PRINT *,' Enter VMS (1) or UNIX (2) machine'
      READ (31,*) IDM
      PRINT *,'      VMS (1) or UNIX (2) machine =', IDM
      YN = 'Y'
 801  IF (YN.EQ.'Y'.OR.YN.EQ.'y') THEN
C     
C     Initialize parameters
         ECM = 40000.
c$$$         ALFAI = 128.0
         ALFAI = 127.51
c$$$         ALFAS = 0.12
         ALFAS = 0.127
c$$$         XMZ = 91.1 
         XMZ = 91.695
C         XMW = -80.    
         XMW = 80.41
C     XMW < 0 implies W-mass calculation from GFermi
         XMT = 175.
         XLUMI = 1D4   
C     Luminosity factor in # events / pb
C     
C     Initialize VEGAS call
         ALPH1 = 0.5
         ALPH2 = 0.2
         IT1 = 10
         IT2 = 5
C     
C     Choose weighted or unweighted events by setting maximum weight.
C     If XMXWGT < 0 weighted events are calculated
         XMXWGT = -1.0
C     
C     Choose pp or ppbar scattering:  1 : pp scattering
C     2 : ppbar scattering  
         IBEAMS = 1
C     
C     Choose WW-photon and WWZ anomalous couplings
C     Photon couplings
         G1(1)=(1.0,0.0)
         XK(1)=(1.0,0.0)
         XL(1)=(0.0,0.0)
         G4(1)=(0.0,0.0)
         G5(1)=(0.0,0.0)
         G6(1)=(0.0,0.0)
         G7(1)=(0.0,0.0)
C     Z boson couplings
         G1(2)=(1.0,0.0)
         XK(2)=(1.0,0.0)
         XL(2)=(0.0,0.0)
         G4(2)=(0.0,0.0)
         G5(2)=(0.0,0.0)
         G6(2)=(0.0,0.0)
         G7(2)=(0.0,0.0)
C     
C     Which bosons V1 and V2 are to be produced?
C     1: photon
C     2: Z-boson
C     3: W+ boson
C     4: W- boson
         BOS(1) = 2
         BOS(2) = 3
         BOSON(1) = 2
         BOSON(2) = 3
C     
C     Choose initial and final state flavors
C     The fermion indices are two digit numbers
C     the first digit is  1: neutrino of electron (muon,tau)
C     2: electron (muon,tau)
C     3: up quark (charm,top)
C     4: down quark (strange,bottom)
C     the second digit is the generation index
C     1: electron family
C     2: muon family
C     3: tau family
C     
C     V1 decay products  ( e.g. W-  -->  F1 + F2-bar )
         FINDEX(1) = 21
         FINDEX(2) = 21
C     
C     V2 decay products  ( e.g. W+  -->  F3 + F4-bar )
Cab         PRINT *,' Enter generation index: el: 1, mu: 2'
         READ (31,*) IGEN
         PRINT *,'      Generation index: el: 1, mu: 2 =',IGEN
*---  lepton generation : test(2)
         IF (IGEN.EQ.1) THEN    ! Electron
            FINDEX(3) = 11
            FINDEX(4) = 21
C            test(2) = 1
         ELSEIF (IGEN.EQ.2) THEN ! Muon
            FINDEX(3) = 12
            FINDEX(4) = 22
C            test(2) = 2
         ELSEIF (IGEN.EQ.3) THEN ! Tau
            FINDEX(3) = 13
            FINDEX(4) = 23
C            test(2) = 3
         else 
            goto 1000
         ENDIF
C     Choose cuts 
         YCUT = 2.5             ! Rapidity cut on produced bosons
         TAUMIN = (200./ECM)**2 ! Define minimal invariant mass to be produced
         QSQMAX = 300.**2       ! Max. Q**2 of vector bosons
         EPHMIN = 20.           ! Minimal photon energy     
         ECLMIN = 10.           ! Minimal charged lepton energy
         EJTMIN = 30.           ! Minimal jet (=parton) energy
         SCCOMP = ECM           ! Scale of form factors
         XNPARC = 2.            ! Supression for parity conserving form factors ( >=1.0 )
         XNPARV = 2.            ! Supression for parity violating form factors ( >=0.5 )
C     
C     Fix parameters for Feynman-x Distributions
         XN = -3.               ! dynamically calculated if < -1.
         SCOS = -10.            ! dynamically calculated if < 0.
         NEWHIS0 = 1            ! 0: All histograms printed
C     ! 1: Prompt of hist id in OUTPUT 
Cab         PRINT *,' Enter pp (1)  or ppbar (2)  scattering'  
         READ (31,*) ibeams
         PRINT *,'      pp (1)  or ppbar (2)  scattering =',ibeams
         IBOS = BOS(2)
         READ (31,*) BOS
         PRINT* ,
     *       '      Bosons  = 1(photon) or 2(Z) or 3(W+) or 4(W-)='
     *            ,BOS(1),BOS(2)
         BOSON(1) = BOS(1)
         BOSON(2) = BOS(2)

         IF (BOS(2).NE.IBOS) THEN
            NINDEX = FINDEX(3)
            FINDEX(3) = FINDEX(4)
            FINDEX(4) = NINDEX
         ENDIF
c$$$CNAHO 
c$$$*--- charge of W and lepton : test(1)
c$$$         if (BOS(2).eq.3) then  ! V2 is W+
c$$$            test(1) = 1
c$$$         else if (BOS(2).eq.4) then ! V2 is W-
c$$$            test(1) = 2
c$$$         else
c$$$            print *,' ERROR:wg_main.f check BOS(2) in input file'
c$$$            goto 1000
c$$$         endif
c$$$CNAHOEND
Cab         PRINT* ,' Enter ECM = '
         READ (31,*) ECM
         PRINT* ,'      ECM = ',ECM
Cab         PRINT *,' Enter MAX INVARIANT MASS'
         READ (31,*) XMSMAX
         PRINT *,'      MAX INVARIANT MASS = ',XMSMAX
         TAUMAX = (XMSMAX/ECM)**2 ! Define minimal invariant mass to be produced
Cab                PRINT *,' Enter MIN PT-PHOTON'
         READ (31,*)  PTC
         PRINT *,'      MIN PT-PHOTON = ', PTC
Cab                PRINT *,' Enter MIN PT-LEPTON'
         READ (31,*)  PTLCUT
         PRINT *,'      MIN PT-LEPTON = ',PTLCUT
Cab                PRINT *,' Enter MISS. PT-CUT'
         READ (31,*)  PTMCUT
         PRINT *,'      MISS. PT-CUT = ', PTMCUT
Cab                PRINT *,' Enter CL.TRANS. MASS CUT'
         READ (31,*)  XMTCUT
         PRINT *,'      CL.TRANS. MASS CUT = ',XMTCUT
Cab                PRINT *,' Enter MIN L-PHOTON SEP.'
         READ (31,*)  RJLMIN
         PRINT *,'      MIN L-PHOTON SEP. = ',RJLMIN
Cab                PRINT* ,' Enter YCUT FOR PHOTON' 
         READ (31,*) YCUT
         PRINT* ,'      YCUT FOR PHOTON = ',YCUT 
Cab                PRINT *,' Enter YLCUT FOR CHARGED LEPTON'
         READ (31,*)  YLCUT
         PRINT *,'      YLCUT FOR CHARGED LEPTON = ',YLCUT
Cab                PRINT *,' Enter E-NU TRANS. MASS CUT' 
         READ (31,*)  XMTECUT
         PRINT *,'      E-NU TRANS. MASS CUT = ',XMTECUT 
Cab                PRINT* ,' Enter NCALL1'
         READ (31,*) NCALL1
         PRINT* ,'      NCALL1 = ',NCALL1
Cab                PRINT* ,' Enter NCALL2 = '
         READ (31,*) NCALL2
         PRINT* ,'      NCALL2 = ',NCALL2
Cab                PRINT *,' Enter MIN INVARIANT MASS'
         READ (31,*)  XMSMIN
         PRINT *,'      MIN INVARIANT MASS = ',XMSMIN
         TAUMIN = (XMSMIN/ECM)**2 ! Define minimal invariant mass to be produced
C-- Activate for Pdflib
Cab        PARM(1) = 'Nptype'
Cab        VAL(1) = 1
Cab        PARM(2) = 'Ngroup'
Cab        PARM(3) = 'Nset'
         PARM(1) = 'DEFAULT'
 1718    PRINT *,' Enter group for PDFLIB: '
         READ (31,*) VAL(1)
Cab 1718   PRINT *,' Enter group for PDFLIB: '
Cab        READ (31,*) VAL(2)
Cab        PRINT *,' Enter set for PDFLIB: '
Cab        READ (31,*) VAL(3)
         IF (VAL(2).EQ.3) THEN
           IF (VAL(3).GE.8.AND.VAL(3).LE.9) THEN
             PRINT *,
     +            ' This set has been retracted, try a different one'
             GOTO 1718
           ELSEIF (VAL(3).GE.26.AND.VAL(3).LE.28) THEN
            PRINT *, ' This set has been retracted, try a different one'
            GOTO 1718
          ENDIF
        ELSEIF (VAL(2).EQ.5.AND.VAL(3).LE.2) THEN
          PRINT *, ' This set has been retracted, try a different one'
          GOTO 1718
        ENDIF
Cab        DO 1717 I = 4,20
Cab          PARM(I) = '                   '
Cab          VAL(I) = 0
Cab  1717   CONTINUE
        DO 1717 I = 2,20
          PARM(I) = '                   '
          VAL(I) = 0
 1717   CONTINUE
        CALL PDFSET(PARM,VAL)
        PRINT *,' '
C    
Cab         PRINT *, ' Enter CHANGE XN AND SCOS? Y/N'
        READ (31,*) YN
         YN = 'N'
         PRINT *, '      CHANGE XN AND SCOS? Y/N =', YN
         IF (YN.EQ.'Y') THEN
                 PRINT *,' Enter XN = ', XN
            READ (31,*) XN
                 PRINT *,'      XN = ', XN
                 PRINT *,' Enter RT(SCOS)'
            READ (31,*) SCOS
                 PRINT *,'      RT(SCOS) =  ',SCOS
            IF (SCOS.GT.0D0) SCOS = SCOS**2
         ENDIF
C
Cab                 PRINT* ,' Enter Anomalous couplings?   Enter Y/N'
         READ (31,*) YN
         PRINT* ,'      Anomalous couplings Y/N =',YN
c     YN = 'N'
C     Photon couplings
         G1(1)=(1.0,0.0)
         XK(1)=(1.0,0.0)
         XL(1)=(0.0,0.0)
         G4(1)=(0.0,0.0)
         G5(1)=(0.0,0.0)
         G6(1)=(0.0,0.0)
         G7(1)=(0.0,0.0)
C     Z boson couplings
         G1(2)=(1.0,0.0)
         XK(2)=(1.0,0.0)
         XL(2)=(0.0,0.0)
         G4(2)=(0.0,0.0)
         G5(2)=(0.0,0.0)
         G6(2)=(0.0,0.0)
         G7(2)=(0.0,0.0)
         IF (YN.EQ.'Y'.OR.YN.EQ.'y') THEN
            I2=1
            READ (31,*) SCCOMP
            PRINT* ,' FORM FACTOR SCALE = ',SCCOMP
            PRINT*, ' ********************************* '
            PRINT*, ' Coupling has changed              '
 802        IF (I2.GT.0) THEN
Cab               PRINT*,'  enter coupling:  I2 coupl.# '
               I1 = BOS(1)
               READ (31,*)  I2
               IF (I2.LE.0) GOTO 57
Cab               PRINT*,'  enter coupling :'
               READ (31,*) Z
               IF (I2.EQ.1) THEN
                  G1(I1) = 1.+Z
               ELSEIF (I2.EQ.2) THEN
                  XK(I1) = 1.+Z 
                  WRITE (*,*) " delta kappa = ", Z
               ELSEIF (I2.EQ.3) THEN
                  WRITE (*,*)" lambda       = ", Z
                  XL(I1) = Z 
               ELSEIF (I2.EQ.4) THEN
                  G4(I1) = Z 
               ELSEIF (I2.EQ.5) THEN
                  G5(I1) = Z 
               ELSEIF (I2.EQ.6) THEN
                  G6(I1) = Z 
               ELSEIF (I2.EQ.7) THEN
                  G7(I1) = Z 
               ELSEIF (I2.EQ.8) THEN
                  G1(I1)=Z
                  XK(I1)=0.
                  XL(I1)=0.
                  G4(I1)=(0.0,-1.0)*Z
                  G5(I1)=-Z
                  G6(I1)=(0.0,-1.0)*Z
               ENDIF
 57            CONTINUE
               GOTO 802
            ENDIF
            SWACRT = .TRUE.



            PRINT*, ' ********************************* '
         ENDIF
CNAHO
Cab              Print *,' Enter the output file name'
         READ (31,*) outputfilename
         Print *,'      The output file name =',outputfilename
Cab         Print *,' Enter output file is formatted Y/N'
         READ (31, *) YN
         Print *,'      output file is formatted Y/N =',YN
         if (YN.eq.'Y'.or. YN.eq.'y') then
            formatted = .TRUE.
         else
            formatted = .FALSE.
         endif
c... give random seed

C.....if you want to start generating with new random seed,
C.....set NTOTIN = 0 and NTO2IN = 0
         NTOTIN = 0
         NTO2IN = 0
Cab               print *,' Enter the random seed'
         READ (31,*) IJKLIN
         print *,'      The random seed =', IJKLIN
         CALL RMARIN(IJKLIN,NTOTIN,NTO2IN)
C
C     END of input parameters: now generate events
         Print *,'    '
         Print *,' END OF INPUT PARAMETERS'
         Print *,'    '
         Print *,'    '
CNAHOEND

         CALL INIT
         YN = 'Y'
Cab              PRINT* ,' Enter READY  FOR   OUTPUT ??? '
Cab         READ (5,'(A)') YN 
Cab              PRINT* ,'      READY  FOR   OUTPUT =',YN
         CALL OUTPT2
C         WRITE (*,*) "f3 IJKLUT = ",IJKLUT
C         write (*,*) "3 NTOTUT = ",NTOTUT
C         write (*,*) "3 NTO2UT = ",NTO2UT

C         CALL ENDOUTPT
CCab         IF (IDH.EQ.2) THEN
CCab            PRINT *,'Want PAW file? Enter Y/N '
CCab            READ (5,'(A)') YN
CCab           YN = 'N'
CCab           IF (YN.EQ.'Y'.OR.YN.EQ.'y') THEN
CCab             CALL HRPUT(0,'PAWPLOT.PAW','N')
CCab           ENDIF
CCab         ENDIF
CCab         PRINT *,'Want TOPDRAWER file(s)? Enter Y/N '
CCab         READ (5,'(A)') YN
CCab         YN = 'N'
CCab         IF (YN.EQ.'Y'.OR.YN.EQ.'y') THEN
CCab 601        PRINT *,'Give ID: (0=all) (<0: STOP)'
CCab            READ (31,*)  IDT
CCab            IF (IDT.LT.0) GOTO 602
CCab            IF (IDT.GT.NHMAX) THEN               
CCab               PRINT *,'Histogram ',IDT,' does not exist'
CCab               GOTO 601
CCab            ENDIF
CCab            CALL XCURVE(IDT)
CCab            PRINT *,'Filename for TOPDRAWER file (le. 12 characters): '
CCab            IF (IDM.EQ.1) 
CCab     *           PRINT *,'file extension .TOP provided automatically '
CCab            READ (5,'(A)') NFILE
CCab            CALL XCDUMP(IDT,NFILE,IDM)
CCab            GOTO 601
CCab         ENDIF
CCab 602     CONTINUE
CC...         
C         WRITE (*,*) "f2 IJKLUT = ",IJKLUT
C         write (*,*) "2 NTOTUT = ",NTOTUT
C         write (*,*) "2 NTO2UT = ",NTO2UT
C
C         IDT = -2
C         CALL RMARUT(IJKLUT,NTOTUT,NTO2UT)
CC...The arguments NTOTIN and NTO2IN are used to restart the generation 
CC...from a given point by skipping over already performed extractions. 
CC...They are returned by RMARUT and should not be touched by the user
C         WRITE (*,*) " IJKLUT = ",IJKLUT
C         write (*,*) " NTOTUT = ",NTOTUT
C         write (*,*) " NTO2UT = ",NTO2UT
CC         PRINT *,'Want another set of couplings? Enter Y/N '
C         READ (5,'(A)') YN       
C         GOTO 801
      ENDIF

*     --- Jump here if input file has error
 1000 continue
c      STOP
      END
