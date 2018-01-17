C      SUBROUTINE TAUOLA(MODE,KEYPOL) 
C------------------------------------------------------------
      SUBROUTINE TAUOLA_HEPEVT(MODE) 
C     *************************************
C     
C general tauola interface, should work in every case until 
C hepevt is OK, does not check if hepevt is 'clean'
C in particular will decay decayed taus...
C only longitudinal spin effects are included.
C in W decay v-a vertex is assumed
C date: 12 DEC 1998. date: 21 June 1999. date: 24 Jan 2001 date: 24 Aug 2001
C this is the hepevt class in old style. No d_h_ class pre-name
c
c  June 17, 2008:  Vladimir Savinov (V.S.) and Reza Yoosoofmiya (R.Y.) fixed this 
c                  program to allow decays of taus from exotic decays in TAUOLA
c  June 23, 2008:  V.S. commented out a number of declarations of unused variables 
c                  and modified "SAVE" statements for common blocks to eliminate
c                  compiler warnings
c  Aug. 28, 2008: V.S.  
c    1) Allowed double-charged Higgses' decays to pairs of like-sign taus (most of the changes), 
c    2) Made sure that taus from HR++ and HL++ decays are not polarized, while those from Z', Z'' and ZR_0 decays could be (also see 3)
c    3) Prepared some skeleton code in TAUOLA_HEPEVT to include Bertrand's and Georges's Z' mods 
c       for spin correlations between two taus in Z' decays
c    4) A better treatment of WR decays to tau NR and NR -> tau q q' (or similar exotic decays with one tau)
c
c  Jan. 16, 2009:  V.S., a cosmetic change (search for "Jan. 16, 2009" line) to avoid future confusion
c
c  Jan. 17, 2009:  V.S. put in angular correlations in decays of tau pairs that come 
c                  from gamma/Z/Z' interference implemented by Georges and Bertrand 
c                  with corrections from Zofia and Mogens. 
c                  Notice that the new code works only for simulation with Pythia. 
c                  The code from Georges and Bertrand is between the "c >>--GA-->>" and "c <<--GA--<<"
c
c  Sept.23, 2009:  V.S. single-tau MC problem fixed
c
c  Nov.3, 2009: V.S. "designed" and "implemented" another dirty, dirty, dirty hack to help his collegues Michael W. and Haiping P. 
c               Take notice: this code is becoming unmanageable and, at some point, will have to be completely redesigned / rewritten.
c
c  Nov.13, 2009, V.S.: do not count documentary taus (exotics, MADGRAPH again...)
c
c  Mar.19, 2011, V.S.: a convoluted implementation of twisted logic that allows to decay multiple taus coming from some BSM decays 
c                without breaking down scenarios when tau polarization information can be inferred from parent particle. Also, this 
c                package can now handle some of the cases when the event record information is incomplete or even corrupted. 
c
c  Mar.21, 2011, V.S.: more special cases of the Mar.19,2011 included (take note this program is NOT immune against UNDECAYED taus even now!)
c     
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep       ! to be real*4/ *8  depending on host
      REAL*8 polz0,plzapx
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $     jdahep
      COMMON /hepevt/
     $     nevhep,              ! serial number
     $     nhep,                ! number of particles
     $     isthep(nmxhep),      ! status code
     $     idhep(nmxhep),       ! particle ident KF
     $     jmohep(2,nmxhep),    ! parent particles
     $     jdahep(2,nmxhep),    ! childreen particles
     $     phep(5,nmxhep),      ! four-momentum, mass [GeV]
     $     vhep(4,nmxhep)       ! vertex [mm]
*     ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)       ! Photos flag
*     ----------------------------------------------------------------------
c     SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/
      

      COMMON /TAUPOS/ NP1, NP2 
c     REAL*4 PHOI(4),PHOF(4)
      double precision Q1(4),Q2(4),P1(4),P2(4),P3(4),P4(4)
c     
c     Aug.28'08  V.S.: for H++ and H-- purposes
c     
      integer NP1_SAVED, NP2_SAVED
      double precision P1_SAVED(4), P2_SAVED(4)
c
      COMMON / MOMDEC / Q1,Q2,P1,P2,P3,P4
* tauola, photos and jetset overall switches
      COMMON /LIBRA/ JAK1,JAK2,ITDKRC,IFPHOT,IFHADM,IFHADP
      REAL*4 RRR(1)
      LOGICAL IFPSEUDO
      common /pseudocoup/ csc,ssc
      REAL*4 csc,ssc
c     save pseudocoup
      save /pseudocoup/
      COMMON / INOUT / INUT,IOUT
      
C     to switch tau polarization OFF in taus 
      DIMENSION POL1(4), POL2(4)
      double precision POL1x(4)
c     , POL2x(4)
      DATA  POL1 /0.0,0.0,0.0,0.0/
      DATA  POL2 /0.0,0.0,0.0,0.0/
      DATA PI /3.141592653589793238462643D0/
      
C     (ET) Record information for taus decayed
#include "Tauola_i/taudet.inc"
C     (ET)
      
C     store decay vertexes 
C     INTEGER NMXTAU
C     PARAMETER (NMXTAU=20)
      
      DIMENSION IMOTHER (NMXTAU)
      DIMENSION ICHILD (NMXTAU)
      logical SMOTH (NMXTAU)
      
      INTEGER KFHIGGS(3)
      INTEGER KFHIGCH
      INTEGER KFGAM, KFZ0

      INTEGER KFZ0P, KFZ0PP, KFZR0
      INTEGER KFHRPP, KFHLPP
      
C     store daughter pointers
      INTEGER ISON
      COMMON /ISONS_TAU/ISON(2)
      SAVE /ISONS_TAU/
      
C     PARAMETER COMMON
      INTEGER IFPOL,IFRADCOR,IFDMOD,IFJAK1,IFJAK2
      REAL FFPHX
      COMMON /FFTAU/IFPOL,IFRADCOR,IFDMOD,IFJAK1,IFJAK2,FFPHX
      
      REAL*8 FFPHOX
      INTEGER KEYPOL
      COMMON /POLVAL/KEYPOL
      SAVE /POLVAL/
      
      INTEGER ITAUBR            ! DECAY MODES
      COMMON /TAUBRAN/ITAUBR(2)
      
      LOGICAL ISHERWIG
      COMMON /LOGHW/ISHERWIG
c     
      common / LOGPH / IS_Z0P, IS_PYTHIA
      logical IS_Z0P
      logical IS_PYTHIA
c     
      
      DOUBLE PRECISION VETMP(4),VTMP,VEORG(4)
      LOGICAL ZPOL
      
C     Internal information about decay mode
      COMMON / JAKI   /  IJAK1,IJAK2,JAKP,JAKM,KTOM
      
c------------------------------------------------------------------------------
c     
c     V.S. and R.Y.: June 17, 2008: a flag for tau lepton of exotic origin.
c     "Exotic origin" definition: a tau, whose parent's daughters do not include
c     another tau neither tau neutrino
c     
      LOGICAL EXOTAU
c     
c     The number of exotic tau's siblings according to event record 
c     
      INTEGER NSIB
c     
c     The number of taus to decay
c     
      INTEGER NTAU
c
c------------------------------------------------------------------------------
C     JK - Single Particle Generator switch
C     to remove jump 777  into the do-loop
C     0 - standard mode (jump off) 
C     1 - SPG mode (jump on)
      INTEGER SPGSWITCH

C...S.T. To propagate polatization state from external generators
      INTEGER ATLASCHOICE
      COMMON/LESATLAS/ATLASCHOICE
      SAVE /LESATLAS/

      INTEGER ALPGEN,MCATNLO,LHAEXT
      PARAMETER (ALPGEN=400000,MCATNLO=900000,LHAEXT=800000) 

      INTEGER MAXNUP
      PARAMETER (MAXNUP=500)
      INTEGER NUP,IDPRUP,IDUP,ISTUP,MOTHUP,ICOLUP
      DOUBLE PRECISION XWGTUP,SCALUP,AQEDUP,AQCDUP,PUP,VTIMUP,SPINUP
      COMMON/HEPEUP/NUP,IDPRUP,XWGTUP,SCALUP,AQEDUP,AQCDUP,IDUP(MAXNUP),
     &ISTUP(MAXNUP),MOTHUP(2,MAXNUP),ICOLUP(2,MAXNUP),PUP(5,MAXNUP),
     &VTIMUP(MAXNUP),SPINUP(MAXNUP)
      SAVE /HEPEUP/

      INTEGER ALPZ,ALPTAU,INDXT1,INDXT2

      SPGSWITCH = 0
c------------------------------------------------------------------------------
      
      IF(MODE.EQ.-1) THEN
C     ***********************
         
         JAK1  =  0             ! decay mode first tau 
         JAK2  =  0             ! decay mode second tau
         ITDKRC=1.0             ! switch of radiative corrections in decay
         IFPHOT=0.0             ! PHOTOS switch - DONE BY SEPARATE CALL
         IFHADM=1.0
         IFHADP=1.0
         POL=1.0                ! tau polarization dipswitch must be 1 or 0
         FFPHOX=0.01d0
         
         KFHIGGS(1) = 25
         KFHIGGS(2) = 35
         KFHIGGS(3) = 36
         KFHIGCH = 37
         
         KFZ0    = 23
         KFGAM   = 22
         
         KFTAU   = 15
         KFNUE   = 16
         
C     
C     Aug.28'08  V.S.:  include Z'0, Z''0 (generic), and Z_R0 and double-charged Higgses (LRSM) into the list of possible parents 
C     
c     >>--GA-->>
         KFZ0P  = 32
c     <<--GA--<<
         KFZ0PP = 33
         KFZR0  = 9900023
c     
c     Notice that these two double-charged Higgses are NOT their own antiparticles!
c     
         KFHLPP = 9900041
         KFHRPP = 9900042
         
c     ouplings of the `pseudoscalar higgs' as in CERN-TH/2003-166
         psi=0.5*PI             ! 0.15*PI
         xmtau=1.777            ! tau mass
         xmh=120                ! higgs boson mass
         betah=sqrt(1d0-4*xmtau**2/xmh**2)
         csc=cos(psi)*betah
         ssc=sin(psi)
C     write(*,*) ' scalar component=',csc,' pseudo-scalar component=',ssc
         
C     TAU DECAY TIME IN CT UNITS (TAKEN FROM PYTHIA)
         ATAUDEC=0.0872
         
c     PRINT *,'PARS',IFPOL,IFRADCOR,IFDMOD,IFJAK1,IFJAK2,FFPHX
c     call flush(6)
         
C-----open output files 
         OPEN(IOUT  ,file='atlastauola.out')
         
         
         KEYPOL=IFPOL
         IF (KEYPOL.EQ.0) POL=0.
         
         IF (IFDMOD.NE.0) THEN
            JAK1=0
            JAK2=0
         ELSE
            JAK1=IFJAK1
            JAK2=IFJAK2
         ENDIF
         ITDKRC=IFRADCOR
         FFPHOX=DBLE(FFPHX)
         
C     (BPK) ORIGINAL
C     IF (IFPHOT.EQ.1) CALL  PHOINI  ! this if PHOTOS was not initialized earlier
         CALL INIETC(JAK1,JAK2,ITDKRC,IFPHOT)
         CALL TAU_INIMAS
         CALL INIPHX(FFPHOX)
         CALL INITDK
C     activation of pi0 and eta decays: (-1,1) means on,  (-1,0) off
         CALL  TAUPI0(-1,1)
         CALL DEKAY(-1,POL1x)
         
         WRITE(IOUT,7001) pol,psi
         
      ELSEIF(MODE.EQ.0) THEN
C     ***********************
C     
C.....find tau-s and fill common block /TAUPOS/
C     this is to avoid LUND history fillings. This call is optional
         CALL PHYFIX(NSTOP,NSTART)
         
C     DEFAULT Z POLARISATION TREATMENT
         ZPOL=.TRUE.
         
C     clear mothers of the previous event
         DO II=1,NMXTAU
            IMOTHER(II)=0
            ICHILD(II)=0
            SMOTH(II) = .FALSE.
         ENDDO
         
         DO II=1,2
            ISON(II)=0
         ENDDO
C     ... and to find mothers giving taus.
         NDEC    = 0
         
C     (BPK)-->
         IF (NHEP.EQ.1) THEN    ! SINGLE PARTICLE GENERATOR
            DO I=1,4
               Q1(I)= 0.D0
            ENDDO
            Q1(4)=2*PHEP(4,1)
            
            I=1
            
            IF(IDHEP(I).EQ.-KFTAU.OR.IDHEP(I).EQ.-KFNUE) THEN           
               NP1=1
               NP2=2
               DO I=1,4
                  P1(I)= PHEP(I,NP1) !momentum of tau+
                  P2(I)= Q1(I)-P1(I) 
               ENDDO
            ENDIF
c     
c-------------------------------------------------------------------
c     
c     V.S. and R.Y.:  June 17, 2008   "I" must be reset to 1! Otherwise 
c     this won't work for tau- from signle particle generator!!!
c     
            I=1
c     
c-------------------------------------------------------------------
c     
            IF(IDHEP(I).EQ. KFTAU.OR.IDHEP(I).EQ. KFNUE) THEN
               NP1=2        
               NP2=1        
               DO I=1,4
                  P2(I)= PHEP(I,NP2) !momentum of tau-
                  P1(I)= Q1(I)-P2(I)
               ENDDO
            ENDIF
            NCOUNT=1
            NDEC=0
c     
c     V.S.  June 23, 2008: notice that we are going to jump inside the loop! II is a loop variable, we set it to 0. 
c     We also set NCOUNT=1 to avoid executing the loop (there will be a jump out of the loop in the end). 
c     Yes, this program is written in FORTRAN...
c     
            II=0
c------------------------------------------------------------------------------
c     GOTO 777
c     JK - instead goto 777 -- perform "jump"
            SPGSWITCH=1
         ENDIF
c            
         IF(SPGSWITCH.EQ.0) THEN
c------------------------------------------------------------------------------
            
C     (BPK)--> LOOK FOR MOTHER, CHECK THAT IT IS NOT THE HISTORY ENTRY (E.G. MSTP(128)=0)
            DO I=NSTART,NHEP
               IF(ABS(IDHEP(I)).EQ.KFTAU.AND.((ISTHEP(I).EQ.195)
     $              .OR.ISTHEP(I).EQ.198)) THEN ! TAU ALREADY DECAYED
                  GOTO 9999
c     RETURN
               ENDIF
               IF(ABS(IDHEP(I)).EQ.KFTAU.AND.((ISTHEP(I).EQ.1).OR.
     $              (ISTHEP(I).GE.125.OR.ISTHEP(I).LT.120))) THEN
                  IMOTH=JMOHEP(1,I)
                  DO WHILE ( ABS(IDHEP(IMOTH)) .eq. KFTAU  .and. IMOTH 
     $                 .ne. 0 ) ! KEEP WALKING UP
                     IMOTH=JMOHEP(1,IMOTH)
                  ENDDO
                  JMOTH=IMOTH   ! THERE HAS TO BE STH!
                  IF (ISTHEP(IMOTH).EQ.3.OR.
     $                 (ISTHEP(IMOTH).GE.120.AND.ISTHEP(IMOTH).LE.125)) 
     $                 THEN 
                     DO J=NSTART,NHEP ! WE HAVE WALKED INTO HARD RECORD
                        IF (IDHEP(J).EQ.IDHEP(IMOTH).AND.
     $                       JMOHEP(1,J).EQ.IMOTH.AND.
     $                       ISTHEP(J).EQ.2) THEN
                           JMOTH=J
                           GOTO 66
                        ENDIF
                     ENDDO
                  ELSE
                     JMOTH=IMOTH
                  ENDIF
 66               CONTINUE
                  
c     
c     March 19, 2011 by V.S.: the meaning of this block of code is 
c     
c     "When the parent of a tau to decay is a BSM particle but not Z'', nor doubly-charged Higgs nor WR, 
c     force each tau to be decayed independently, without trying to pair it up with some other daughter 
c     particle hoping that the info on parent polarization could be used later to treat tau polarization
c     properly. Instead, make such tau leptons decay unpolarized." In such cases, if the parent decays into
c     more than one tau lepton, each tau would be decayed completely independently, and NDEC would be 
c     incremented by 1 for each tau (incontrast to incrementing it by 1 for each parent's decay as, e.g., 
c     in case of Z->tau tau). 
c     
                  DO II=1,NDEC
                     if ( JMOTH .gt. 0 .and. (
     +                    abs(IDHEP(JMOTH)) .lt. 1000000 .or.
     +                    abs(IDHEP(JMOTH)) .eq. 9900023 .or.
     +                    abs(IDHEP(JMOTH)) .eq. 9900041 .or. 
     +                    abs(IDHEP(JMOTH)) .eq. 9900042 
     +                    )
     +                    ) then
                        
                        IF(JMOTH.EQ.IMOTHER(II)) GOTO 9999
                     endif
                     
                  ENDDO
C     (BPK)--<
c     
                  NDEC=NDEC+1
                  IMOTHER(NDEC)= JMOTH
                  
                  if ( NDEC .gt. NMXTAU ) then
                     write(*,*)
     +                    'TAUOLA-HEPEVT-ERROR-Max number of taus to decay exceeded. NMXTAU = ',
     +                    NMXTAU,
     +                    ' Ignoring this error could lead',
     +                    ' to an exception\!',
     +                    ' Please report this error to ATLAS MC group\!'
                     NDEC = NDEC - 1
                  endif
                  
                  IMOTHER(NDEC) = JMOTH
                  ICHILD(NDEC) = I
                  
C     tau from a "special" parent (see the March 19, 2011 explanation above)!
                  
                  if ( 
     +                 abs(IDHEP(JMOTH)) .ge. 1000000 .and.
     +                 abs(IDHEP(JMOTH)) .ne. 9900023 .and.
     +                 abs(IDHEP(JMOTH)) .ne. 9900041 .and. 
     +                 abs(IDHEP(JMOTH)) .ne. 9900042
     +                 ) 
     +                 SMOTH(II) = .TRUE.
                  
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-NDEC=',NDEC
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-JMOTH=',JMOTH
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-IDHEP(JMOTH)=',IDHEP(JMOTH)
c     
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-IMOTHER=',IMOTHER(NDEC)
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-IDHEP(IMOTHER)=',IDHEP(IMOTHER(NDEC))
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-IMOTHER=',ICHILD(NDEC)
c     write(*,*)'TAUOLA_HEPEVT-DEBUG-IDHEP(ICHILD)=',IDHEP(ICHILD(NDEC))
                  
               ENDIF
 9999          CONTINUE
            ENDDO  
            
c     
c     Aug.28'08  V.S.: more bosons included in the list
c     
C     (BPK)--> IF MORE THAN ONE BOSON DON'T EVEN TRY TO RE-CALCULATE POLARISATION
            NBOS=0
            
            
            DO II=1,NDEC
               IM=IMOTHER(II)
               IF (
     $              IDHEP(IM).EQ.KFHIGGS(1)   .or.
     $              IDHEP(IM).EQ.KFHIGGS(2)   .or.
     $              IDHEP(IM).EQ.KFHIGGS(3)   .or.
     $              abs(IDHEP(IM)).EQ.KFHIGCH .or.
     $              IDHEP(IM).EQ.KFZ0         .or.
     $              IDHEP(IM).EQ.KFZ0P        .or.
     $           IDHEP(IM).EQ.KFZ0PP       .or.
     $              IDHEP(IM).EQ.KFZR0        .or.
     $              abs(IDHEP(IM)).EQ.KFHRPP  .or.
     $              abs(IDHEP(IM)).EQ.KFHLPP  .or.
     $              IDHEP(IM).EQ.KFGAM
     $              ) THEN
                  NBOS=NBOS+1
               ENDIF
            ENDDO
            IF (NBOS.GT.1) ZPOL=.FALSE.
            
C     (BPK)--<
c------------------------------------------------------------------------------
C     JK  - JUMP
         ENDIF                  !  IF(SPGSWITCH.NE.0)

         IF(SPGSWITCH.EQ.1) THEN
            III=0
            NDEC=0
         ELSE
            III=1
         ENDIF

C     ... taus of every mother are treated in this main loop

         DO II=III,NDEC
C     JK  - JUMP
            IF(SPGSWITCH.NE.0) GOTO 777
c------------------------------------------------------------------------------
            IM=IMOTHER(II)
            NCOUNT=0
            NP1=0
            NP2=0
            ISON(1)=0
            ISON(2)=0
            
C     (BPK)--> 
C     CORRECTING HEPEVT IS OUT OF QUESTION AT THIS POINT..
            IM0=IM
            IF (IDHEP(JMOHEP(1,IM0)).EQ.IDHEP(IM0)) IM0=JMOHEP(1,IM0)
            ISEL=-1
            DO I=NSTART,NHEP
               IF (ISTHEP(I).EQ.3.OR.
     $              (ISTHEP(I).GE.120.AND.ISTHEP(I).LE.125)) THEN ! HARD RECORD
                  GOTO 76
               ENDIF
               IMOTH=JMOHEP(1,I)
               DO WHILE ( 
     $              ( 
     $              IDHEP(IMOTH) .eq. IDHEP(I) .or. 
     $              ABS(IDHEP(IMOTH)).eq.KFTAU 
     $              ) 
     $              .and. 
     $              IMOTH .ne. 0 
     $              )           ! KEEP WALKING UP
                  IMOTH=JMOHEP(1,IMOTH)
               ENDDO
               IF ((IMOTH.EQ.IM0.OR.IMOTH.EQ.IM).AND.ISEL.EQ.-1) THEN
                  IF ((ABS(IDHEP(I)).EQ.KFTAU.OR.
     $                 ABS(IDHEP(I)).EQ.KFNUE)
     $                 .AND.ISTHEP(I).EQ.1) THEN
                     ISON(1)=I
                     ISEL=0
                  ENDIF
               ELSEIF ((IMOTH.EQ.IM0.OR.IMOTH.EQ.IM)
     $                 .AND.ISEL.EQ.0) THEN
                  IF ((ABS(IDHEP(I)).EQ.KFTAU.OR.
     $                 ABS(IDHEP(I)).EQ.KFNUE)
     $                 .AND.ISTHEP(I).EQ.1) THEN
                     ISON(2)=I
                  ENDIF
               ELSEIF ((IMOTH.NE.IM0.AND.IMOTH.NE.IM)
     $                 .AND.ISEL.EQ.0) THEN
                  ISEL=1
                  GOTO 77
               ENDIF
 76            CONTINUE
            ENDDO
 77         CONTINUE
c     
c-------------------------------------------------------------------------
c     
c     V.S. and R.Y.: June 17, 2008: now we are going to handle cases when
c     there is a single tau (no otehr tau, neither tau neutrino) coming from 
c     some decay - this is possible in various BSM processes and exotic decays.
c     Notice that this functionality used to be present in this code, but was 
c     (inadvertently) removed when the requirement was introduced on "the other
c     daughter" of tau's mother particle in above code. 
c     
c     Notice that we do NOT interfere with the new logic and apply our "fix" 
c     only when the problem is actually present!
c     
            EXOTAU = .FALSE.
c     
c     
c     CRUCIAL PIECE OF THE NEW CODE (circa March 19, 2011)!!!
c     
            if ( SMOTH(II) ) then
               ISON(1) = ICHILD(II)
               ISON(2) = 0
c     write(*,*)
c     +'TAUOLA_HEPEVT-DEBUG-Exotic tau is placed into the buffer!'
c     write(*,*)'II, IDHEP(II) = ',II,',',IDHEP(II)
            endif
c     
            if ( ISON(1) .ne. 0 .and. ISON(2) .eq. 0 ) then
               if ( abs(IDHEP(ISON(1))) .eq. KFTAU ) then
c     
c     c           write(*,*)
c     c     +'TAUOLA_HEPEVT-DEBUG-Exotic tau identified!!!'
c     
                  EXOTAU = .TRUE.
c     
c     No siblilngs have been found yet for the parent (if ever found!) of this tau
c     
                  NSIB = 0
c     
c     Notice that in such exotic scenario we should forget about polarization, 
c     because as of June 17, 2008 there is no implementation of how to handle 
c     such cases in the current code! Notice that if exotic event in question 
c     contains more than one tau, polarization has been already turned off anyway. 
c     
                  ZPOL=.FALSE.
                  POL = 0.0
c     
c     c           write(*,*)
c     c     +'DEBUG: IM, JDAHEP(1,IM), JDAHEP(2,IM) = ',
c     c     +IM, JDAHEP(1,IM),JDAHEP(2,IM),
c     c     +', IDHEP(JDAHEP(1,IM)) = ',
c     c     +IDHEP(JDAHEP(1,IM)),
c     c     +', IDHEP(JDAHEP(2,IM)) = ',
c     c     +IDHEP(JDAHEP(2,IM))
c     
c     March 19, 2011 by V.S.: don't bother to look for the siblings if this is a tau from a "special" parent
c     
                  if ( .not. SMOTH(II) ) then
c     
c     V.S. and R.Y.:  June 17, 2008: 
c     Look for other daughter particles (and count the daughters!)
c     Notice that it is NOT necessary to have any information about ALL 
c     siblings for the CURRENT (June 17, 2008) code, however, it might 
c     come in handy in the future (if and when we want to include 
c     exotic taus (i.e. tau leptons produced in exotic processes) 
c     polarization into account. 
c     
                     if ( IM .ne. 0 ) then
                        
c     if ( JDAHEP(2,IM) .ne. 0 ) then
c     ISON(2) = JDAHEP(2,IM) 
c     endif
                        IM0=IM
                        if (IDHEP(JMOHEP(1,IM0)).EQ.IDHEP(IM0))
     $                       IM0=JMOHEP(1,IM0)
c     
c     loop over all particles
c     
                        do I = NSTART, NHEP
                           if ( ISTHEP(I) .ne. 3 .and.
     $                             ( ISTHEP(I) .lt. 120 .or.
     $                          ISTHEP(I) .gt. 125
     $                             )
     $                          ) then ! only if NOT hard record!
                                 
                              IMOTH=JMOHEP(1,I)
c     
                              do while ( 
     $                             ( 
     $                             IDHEP(IMOTH) .eq. IDHEP(I) .or.
     $                             ABS(IDHEP(IMOTH)) .eq. KFTAU 
     $                             ) 
     $                             .and. 
     $                             IMOTH .ne. 0 
     $                             ) ! KEEP WALKING UP
                                 IMOTH=JMOHEP(1,IMOTH)
                              enddo
c     
c     notice that "I" should not end up being the particle iself,
c     because neither "I" or ISON(1) have been "walked up the tree" but are
c     the "final" particles in event history chains...
c     
                              if ( ( IMOTH .eq. IM0 .or. 
     $                             IMOTH .eq. IM 
     $                             ) 
     $                             .and. 
     $                             I .ne. ISON(1) 
     $                             ) then
c     
c     located one more daughter
c     
c     c      write(*,*)' IMOTH, IM, IM0, I = ',
c     c     +IMOTH, IM, IM0, I
                                 
                                 NSIB = NSIB + 1
c     
c     Notice that we keep the first sibling's index in ISON(2)
c     (unless another tau is found among the siblings!)
c     
                                 if ( 
     +                                IDHEP(I) .eq.  KFTAU .or.
     +                                IDHEP(I) .eq. -KFTAU .or.
     +                                IDHEP(I) .eq. -KFNUE .or.
     +                                IDHEP(I) .eq.  KFNUE
     +                                ) then
                                    ISON(2) = I
                                 endif
                                 
                                 if ( ISON(2) .eq. 0 ) then
                                    ISON(2) = I
                                 endif
c     
                              endif
                           endif
                        enddo
                     endif
                  endif
               endif
            endif
               
c     
c     Nov.3, 2009: It's another dirty, dirty, dirty hack by VS... Notice it's 7777 (NOT 777)
c     
            if ( EXOTAU .and. NSIB .eq. 0 ) goto 7777
c     
c-------------------------------------------------------------------------
c     
C     (BPK)--< 
c     c         PRINT *,'I',ISON(1),ISON(2),IDHEP(ISON(1)),
c     c     # IDHEP(ISON(2)),IDHEP(IM),IM,IM0
            
C     SKIP IF MISSING INFO
            if ( ISON(1) .eq.0 .or. ISON(2) .eq.0 ) then
c     
c-------------------------------------------------------------------------
c     
c     V.S. and R.Y.: June 17, 2008: reporting that tau won't be decayed
c     
               if ( 
     +              ( ISON(1) .ne. 0 .and. abs(IDHEP(ISON(1))) 
     +              .eq. KFTAU ) .or. 
     +              ( ISON(2) .ne. 0 .and. abs(IDHEP(ISON(2))) 
     +              .eq. KFTAU )
     +              ) then
c     
                  
                  WRITE(*,*) 
     +                 'TAUOLA_HEPEVT does not know what',
     +                 ' to do and quits: ',
     +                 'continuing could lead to an exception.',
     +                 ' This tau will not be decayed.',
     +                 ' Indices ISON(1), ISON(2), IM',
     +                 ' IM0, NSIB, IFPHOT = '
                  
                  WRITE(*,*)
     +                 ISON(1), ', ', 
     +                 ISON(2), ', ', 
     +                 IM, ', ', 
     +                 IM0, ', ',
     +                 NSIB, ', ',
     +                 IFPHOT
c     
                  if ( ISON(1) .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(ISON(1)) = ',
     +                 IDHEP(ISON(1))
                  if ( ISON(2) .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(ISON(2)) = ',
     +                 IDHEP(ISON(2))
                  if ( IM0 .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(IM0) = ',
     +                 IDHEP(IM0)
                  if ( IM .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(IM) = ',
     +                 IDHEP(IM)
c     
               endif
c     
c-------------------------------------------------------------------------
c     
               GOTO 999
            ENDIF
            
C     ... we correct HEPEVT (fix developped with Catherine BISCARAT)
c     IF (JDAHEP(2,IM).EQ.0) THEN                      ! ID of second daughter was missing
c     ISECU=1
c     DO I=JDAHEP(1,IM)+1,NHEP                        ! OK lets look for it
c     IF (JMOHEP(1,I).EQ.IM.AND.ISECU.EQ.1) THEN     ! we have found one
c     JDAHEP(2,IM)=I
c     ELSEIF (JMOHEP(1,I).EQ.IM.AND.ISECU.NE.1) THEN ! we have found one after there
c     JDAHEP(2,IM)=0                              ! was something else, lets kill game
c     ENDIF
c     IF (JMOHEP(1,I).NE.IM) ISECU=0                 ! other stuff starts
c     ENDDO
c     ENDIF
               
C     ... we check whether there are just two or more tau-likes 
            DO I=ISON(1),ISON(2)
c     >> V.S.  Nov.13, 2009
               if ( ISTHEP(I) .eq. 1 ) then               
c     << V.S.  Nov.13, 2009
                  IF(IDHEP(I).EQ.-KFTAU.OR.IDHEP(I).EQ.-KFNUE) 
     $                 NCOUNT=NCOUNT+1
                  IF(IDHEP(I).EQ. KFTAU.OR.IDHEP(I).EQ. KFNUE) 
     $                 NCOUNT=NCOUNT+1
               endif
            ENDDO
            
C     ... if there will be more we will come here again
c     
c-------------------------------------------------------------------------
c     V.S. and R.Y.: June 17, 2008:  notice that in case of "exotic taus" 
c     NCOUNT would necessarily be just 1 (so we won't come back here)
c-------------------------------------------------------------------------
c     
 666        CONTINUE
            
C     (BPK)--> 
            NP1LAST=NP1
            NP1=0
            DO I=MAX(NP1LAST+1,ISON(1)),ISON(2) 
C     (BPK)--< 
               IF(IDHEP(I).EQ.-KFTAU.OR.IDHEP(I).EQ.-KFNUE) THEN
                  NP1=I
                  
c     write(*,*)'tauface-jetset-D-NP1,NP1LAST+1,ISON(1),ISON(2)=',
c     +NP1,', ',NP1LAST+1,', ',ISON(1),', ',ISON(2)
c     write(*,*)'tauface-jetset-D-ISTHEP(NP1) = ',
c     +ISTHEP(NP1)
c     
c     write(*,*)'tauface-D-phep(5,NP1)=',
c     +phep(1,NP1),', ',
c     +phep(2,NP1),', ',
c     +phep(3,NP1),', ',
c     +phep(4,NP1),', ',
c     +phep(5,NP1),', '
c     
c     write(*,*)'tauface-D-mass=',
c     +sqrt(phep(4,NP1)**2-(phep(1,NP1)**2+phep(2,NP1)**2+phep(3,NP1)**2))
c     write(*,*)'tauface-D-squared mass=',
c     +(phep(4,NP1)**2-(phep(1,NP1)**2+phep(2,NP1)**2+phep(3,NP1)**2))
                  
                  if ( IDHEP(NP1) .eq. -KFNUE .and. 
     +                 ISTHEP(NP1) .eq. 1 ) then
                     GOTO 661
                  endif
                  
               ENDIF
            ENDDO
 661        CONTINUE
C     (BPK)--> 
            NP2LAST=NP2
            NP2=0
            DO I=MAX(NP2LAST+1,ISON(1)),ISON(2)
C     (BPK)--< 
               IF(IDHEP(I).EQ. KFTAU.OR.IDHEP(I).EQ. KFNUE) THEN
                  NP2=I
                  
c     write(*,*)'tauface-jetset-D-NP2,NP2LAST+1,ISON(1),ISON(2)=',
c     +NP2,', ',NP2LAST+1,', ',ISON(1),', ',ISON(2)
c     write(*,*)'tauface-jetset-D-ISTHEP(NP2) = ',
c     +ISTHEP(NP2)
c     
c     write(*,*)'tauface-D-phep(5,NP2)=',
c     +phep(1,NP2),', ',
c     +phep(2,NP2),', ',
c     +phep(3,NP2),', ',
c     +phep(4,NP2),', ',
c     +phep(5,NP2),', '
c     
c     write(*,*)'tauface-D-mass=',
c     +sqrt(phep(4,NP2)**2-(phep(1,NP2)**2+phep(2,NP2)**2+phep(3,NP2)**2))
c     write(*,*)'tauface-D-squared mass=',
c     +(phep(4,NP2)**2-(phep(1,NP2)**2+phep(2,NP2)**2+phep(3,NP2)**2))
                     
                  if ( IDHEP(NP2) .eq. KFNUE .and. 
     +                 ISTHEP(NP2) .eq. 1 ) then
                     GOTO 662
                  endif
                  
               ENDIF
            ENDDO
 662        CONTINUE
            
c     
c     Aug.28'08  V.S.: take into account double-charged Higgses (or other exotics particles)
c     
            if ( NP1 .eq. 0 .or. NP2 .eq. 0 ) then
               if ( 
     $              ( IDHEP(ISON(1)) .eq. -KFTAU .and. 
     $              IDHEP(ISON(2)) .eq. -KFTAU )     .or. 
     $              ( IDHEP(ISON(1)) .eq.  KFTAU .and. 
     $              IDHEP(ISON(2)) .eq.  KFTAU )      
     $              ) then
c     
                  NP1LAST = NP1
                  NP2LAST = NP2
c     
                  NP1 = ISON(1)
                  NP2 = ISON(2) 
c     
               endif
            endif
            
c     PRINT *,'NPS',NP1,NP2,IDHEP(NP1),IDHEP(NP2)
c     call flush(6)
            
c     
c-------------------------------------------------------------------------
c     
c     Nov.3,2009: another dirty hack by VS. Such abomination could only be developed by a Fortran programmer...
c     
 7777       continue
            
c     
c     V.S. and R.Y.: June 17, 2008  in case of exotic tau, NP1=0 or NP2=0 is
c     not really the end of the world - we need to decay ONE tau AND we currently
c     don't care about polarization, so we can treat this single exotic tau 
c     similarly to how "single particle generator" case is processed
c     
            if ( EXOTAU ) then 
               
c     write(*,*)"We are in the EXOTAU world!!!"
c     write(*,*)"ISON(1)=NP2? = ",ISON(1)
c     write(*,*)"ISON(2)=NP1? = ",ISON(2)
c     write(*,*)"NSIB = ",NSIB
               
c     
c     The logic below seems to be a bit strange, however, this is necessary to be 
c     consistent with how NP1 and NP2 are used below (and, once again, this is similar 
c     to single particle generator case
c     
               do I = 1, 4
                  Q1(I) = 0.D0
               enddo
c     
c     In case of tau from an exotic process ISON(1) always contains HEPEVT index of this tau
c     
               
c     
c     Oct.15, 2009,  VS:
c     In case of genuine EXOTICA *or* when hard event is not properly stored in event history 
c     we can have parents that decay into tau + X and NO tau- or/and neutrinos!
c     
               if ( NSIB .eq. 2 .and. 
     +              ( IDHEP(ISON(2)) .eq.  KFTAU .or.
     +              IDHEP(ISON(2)) .eq. -KFTAU .or.
     +              IDHEP(ISON(2)) .eq. -KFNUE .or.
     +              IDHEP(ISON(2)) .eq.  KFNUE
     +              ) ) then
                  do J = 1, 4
                     Q1(J)=PHEP(J,IM)
                  enddo
               else
                  Q1(4)=2*PHEP(4,ISON(1))
               endif
               
               if ( IDHEP(ISON(1)) .eq. -KFTAU ) then           
                  NP1 = ISON(1)
                  NP2 = ISON(2)
                  do I = 1, 4
                     P1(I)= PHEP(I,ISON(1)) !momentum of tau+
                     P2(I)= Q1(I)-P1(I) 
                  ENDDO
               endif
c     
               if ( IDHEP(ISON(1)).EQ. KFTAU ) then 
                  NP2 = ISON(1)        
                  NP1 = ISON(2)        
                  DO I=1,4
                     P2(I)= PHEP(I,ISON(1)) !momentum of tau-
                     P1(I)= Q1(I)-P2(I)
                  ENDDO
               endif
c     
               
               GOTO 777
c     
            endif
            
c     
c-------------------------------------------------------------------------
c     
            
C     SKIP IF MISSING INFO
            IF (NP1.EQ.0.OR.NP2.EQ.0) THEN
C     CALL HWUEPR
c     
c-------------------------------------------------------------------------
c     
c     V.S. and R.Y.: June 17, 2008: reporting that tau won't be decayed
c     
               if ( 
     +              ( ISON(1) .ne. 0 .and. abs(IDHEP(ISON(1))) 
     +              .eq. KFTAU ) .or. 
     +              ( ISON(2) .ne. 0 .and. abs(IDHEP(ISON(2))) 
     +              .eq. KFTAU )
     +              ) then
c     
                  WRITE(*,*) 
     +                 'TAUOLA_HEPEVT does not know what to do and quits: ',
     +                 'continuing could lead to an exception.' , 
     +                 ' This tau will not be decayed.' ,
     +                 ' Indices NP1, NP2, ISON(1), ISON(2),' , 
     +                 ' IM, IM0 = ',
     +                 NP1, ', ', 
     +                 NP2, ', ', 
     +                 ISON(1), ', ', 
     +                 ISON(2), ', ', 
     +                 IM, ', ', 
     +                 IM0, ', '
c     
                  if ( NP1 .ne. 0 ) then 
                     write(*,*)
     +                    ' PDG_ID IDHEP(NP1), JDAHEP(1,NP1),', 
     +                    ' JDAHEP(2,NP1) = ',
     +                    IDHEP(NP1),', ',
     +                    JDAHEP(1,NP1),', ',
     +                    JDAHEP(2,NP1)
                  endif
c     
                  if ( NP2 .ne. 0 ) then
                     write(*,*)
     +                    ' PDG_ID IDHEP(NP2), JDAHEP(1,NP2),', 
     +                    ' JDAHEP(2,NP2) = ',
     +                    IDHEP(NP2),', ',
     +                    JDAHEP(1,NP2),', ',
     +                    JDAHEP(2,NP2)
                  endif
c     
                  if ( ISON(1) .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(ISON(1)) = ',
     +                 IDHEP(ISON(1))
                  if ( ISON(2) .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(ISON(2)) = ',
     +                 IDHEP(ISON(2))
                  if ( IM0 .ne. 0) 
     +                 write(*,*)' PDG_ID IDHEP(IM0) = ',
     +                 IDHEP(IM0)
                  if ( IM .ne. 0) 
     +                    write(*,*)' PDG_ID IDHEP(IM) = ',
     +                 IDHEP(IM)
c     
                  WRITE(*,*) 
     +                 'IFDMOD, POL, KEYPOL, NSIB, IFPHOT = ',
     +                 IFDMOD, ', ', 
     +                 POL, ', ', 
     +                 KEYPOL, ', ', 
     +                 NSIB, ', ', 
     +                 IFPHOT
c     
                  WRITE(*,*) 
     +                 'NBOS, NDEC = ',
     +                 NBOS, ', ', 
     +                 NDEC
c     
                  do J = 1, NDEC
                     write(*,*)
     +                    ' IMOTHER(',J,') = ',IMOTHER(J)
                  enddo
c     
               endif
c     
c-------------------------------------------------------------------------
c     
               GOTO 999
            ENDIF
            
            DO I=1,4
               P1(I)= PHEP(I,NP1) !momentum of tau+
               P2(I)= PHEP(I,NP2) !momentum of tau-
               Q1(I)= P1(I)+P2(I)
            ENDDO
            
c-------------------------------------------------------------------------
c     
c     V.S. and R.Y.:  June 17, 2008: it turned out that this piece of code is 
c     not protected against, e.g. two tau-neutrinos scenario - however, there 
c     is no need to decay SM tau neutrinos! Even more dangerously, when tau 
c     leptons are decayed by PYTHIA, we end up with taus that decayed into  
c     X + tau neutrinos resulting in two tau neutrinos in the event! In the 
c     scenario when a user also asks TAUOLA to get involved, THIS piece of 
c     code would still be executed and two neutrinos would be treated as 
c     if they had to be "decayed". Yes, this is so! 
c     
            NTAU = 0
            if ( ABS(IDHEP(NP1)) .eq. KFTAU ) NTAU = NTAU + 1
            if ( ABS(IDHEP(NP2)) .eq. KFTAU ) NTAU = NTAU + 1
            if ( NTAU .eq. 0 ) goto 999
c     
c-------------------------------------------------------------------------
               
 777        CONTINUE            ! SINGLE PARTICLE GENERATOR JUMP
            
C     JK - JUMP 777
               
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c     c
c     WRITE(*,*) 
c     +'TAUOLA_HEPEVT reports: ',
c     +' Indices NP1, NP2, ISON(1), ISON(2), IM, IM0 = ',
c     +NP1, ', ', 
c     +NP2, ', ', 
c     +ISON(1), ', ', 
c     +ISON(2), ', ', 
c     +IM, ', ', 
c     +IM0, ', '
c     c
c           if ( NP1 .ne. 0 ) then 
c     write(*,*)
c     +' PDG_ID IDHEP(NP1), JDAHEP(1,NP1), JDAHEP(2,NP1) = ',
c     +IDHEP(NP1),', ',
c     +JDAHEP(1,NP1),', ',
c     +JDAHEP(2,NP1)
c     endif
cc
c     if ( NP2 .ne. 0 ) then
c     write(*,*)
c     +' PDG_ID IDHEP(NP2), JDAHEP(1,NP2), JDAHEP(2,NP2) = ',
c     +IDHEP(NP2),', ',
c     +JDAHEP(1,NP2),', ',
c     +JDAHEP(2,NP2)
c     endif
cc
c     if ( ISON(1) .ne. 0) 
c     +write(*,*)' PDG_ID IDHEP(ISON(1)) = ',
c     +IDHEP(ISON(1))
c     if ( ISON(2) .ne. 0) 
c     +write(*,*)' PDG_ID IDHEP(ISON(2)) = ',
c     +IDHEP(ISON(2))
c     if ( IM0 .ne. 0) 
c     +write(*,*)' PDG_ID IDHEP(IM0) = ',
c     +IDHEP(IM0)
c           if ( IM .ne. 0) 
c     +write(*,*)' PDG_ID IDHEP(IM) = ',
c     +IDHEP(IM)
cc
c           WRITE(*,*) 
c     +'IFDMOD, POL, KEYPOL, NSIB, IFPHOT = ',
c     +IFDMOD, ', ', 
c     +POL, ', ', 
c     +KEYPOL, ', ', 
c     +NSIB, ', ', 
c     +IFPHOT
cc
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
c
C     (BPK)-->  CHECK IF THERE ARE INDEED TWO TAUS AND PICK THE DECAY MODE
            IF (IFDMOD.GT.0) THEN
               IF (IFDMOD.EQ.1) THEN ! LEP-LEP
                  ITAUBR(1)=1
                  ITAUBR(2)=1
               ELSEIF (IFDMOD.EQ.2) THEN ! HAD-HAD
                  ITAUBR(1)=2
                  ITAUBR(2)=2
               ELSEIF (IFDMOD.EQ.3) THEN ! LEP-HAD
                  IF (ABS(IDHEP(NP1)).EQ.KFTAU.AND. ! TWO TAUS NEEDED
     $                 ABS(IDHEP(NP2)).EQ.KFTAU) THEN  
                     CALL ACRMAR(RRR,1)
                     IF (RRR(1).LT.0.5) THEN
                        ITAUBR(1)=1
                        ITAUBR(2)=2
                     ELSE
                        ITAUBR(1)=2
                        ITAUBR(2)=1
                     ENDIF
                  ELSE
                     ITAUBR(1)=0
                     ITAUBR(2)=0
                  ENDIF
               ELSEIF (IFDMOD.EQ.4) THEN ! TAU->PI NU
                  ITAUBR(1)=3
                  ITAUBR(2)=3
               ENDIF
            ELSE                ! SET TO ANYTHING
               ITAUBR(1)=0
               ITAUBR(2)=0
            ENDIF
C     (BPK)--< 
            
            POL1(3)=  0D0
            POL2(3)=  0D0
            
            IF(KEYPOL.EQ.1) THEN
c.....include polarisation effect
               CALL ACRMAR(RRR,1)
               
c     
c     Jan.17'09  V.S.: to facilitate taking into account Z' polarization
c     
               IS_Z0P = .false.
c     
c     Aug.28'08  V.S.: included more bosons in the list
c     
               IF(
     $              IDHEP(IM)      .eq. KFHIGGS(1) .or.
     $              IDHEP(IM)      .eq. KFHIGGS(2) .or.
     $              IDHEP(IM)      .eq. KFHIGGS(3) .or.
     $              abs(IDHEP(IM)) .eq. KFHRPP     .or.
     $              abs(IDHEP(IM)) .eq. KFHLPP 
     $              ) THEN      ! case of Higgs 
                  IF(RRR(1).LT.0.5) THEN
                     POL1(3)= POL
                     POL2(3)=-POL
                  ELSE     
                     POL1(3)=-POL
                     POL2(3)= POL
                  ENDIF
c     >>--GA-->>  Treat Z' as Z/gamma
               ELSEIF(
     $                 IDHEP(IM) .eq. KFZ0   .or.
     $                 IDHEP(IM) .eq. KFGAM  .or. 
     $                 IDHEP(IM) .eq. KFZ0P  
     $                 ) THEN   ! case of gamma/Z/Z' 
c     <<--GA--<<
C     there is no angular dependence in gamma/Z polarization 
C     there is no s-dependence in gamma/Z polarization at all
C     there is even no Z polarization in any form
C     main reason is that nobody asked ...
C     but it is prepared and longitudinal correlations 
C     can be included up to KORALZ standards
c     
c     The parent is Z', if it's PYTHIA production, know how to take Z' polarization into account
c     
                  if ( IDHEP(IM) .eq. KFZ0P ) then
                     IS_Z0P = .true.
                  endif
c     
                  POLZ0=PLZAPX(ZPOL,IM,NP1,NP2)
                  IF(RRR(1).LT.POLZ0) THEN
                     POL1(3)= POL
                     POL2(3)= POL
                  ELSE     
                     POL1(3)=-POL
                     POL2(3)=-POL
                  ENDIF
c     
c     Aug.28'08  V.S.: one new case:   Z'' and Z_R: assume 50/50 of two cases of tau pairs polarizations, 
c     however, do NOTHING concerning correlations between the two taus
c     
               ELSEIF(
     $                 IDHEP(IM) .eq. KFZ0PP  .or. 
     $                 IDHEP(IM) .eq. KFZR0  
     $                 ) THEN   ! case of Z'' and Z0_R (notice that we ignore actual Z'' or Z0_R polarization) 
                  IF(RRR(1).LT.0.5) THEN
                     POL1(3)= POL
                     POL2(3)= POL
                  ELSE     
                     POL1(3)=-POL
                     POL2(3)=-POL
                  ENDIF
c     
               ELSEIF(IDHEP(NP1).EQ.-IDHEP(NP2))THEN ! undef orig. only s-dep poss.
                  POLZ0=PLZAPX(ZPOL,IM,NP1,NP2)
                  IF(RRR(1).LT.POLZ0) THEN
                     POL1(3)= POL
                     POL2(3)= POL
                  ELSE     
                     POL1(3)=-POL
                     POL2(3)=-POL
                  ENDIF
               ELSEIF(ABS(IDHEP(IM)).EQ.KFHIGCH) THEN ! case of charged Higgs 
                  POL1(3)=  POL
                  POL2(3)=  POL
               ELSEIF( EXOTAU ) THEN ! case of a single Exotic tau
                  IF(RRR(1).LT.0.5) THEN
                     POL1(3)= POL
                     POL2(3)= POL
                  ELSE     
                     POL1(3)=-POL
                     POL2(3)=-POL
                  ENDIF
               ELSE             ! case of W+ or W-
                  POL1(3)= -POL
                  POL2(3)= -POL
               ENDIF

C...S.T. Fix Alpgen-Pythia polarization from SPINUP.
               IF (ATLASCHOICE.EQ.ALPGEN) THEN
C...Temporal fix... only apply this for Z->tautau process.
                 ALPZ   = 0
                 ALPTAU = 0
                 INDXT1 = -1
                 INDXT2 = -1
                 DO I=3,NUP
                   IF (IDUP(I).EQ.23) THEN
                     ALPZ = ALPZ+1
                   ENDIF
                   IF (IDUP(I).EQ.15) THEN
                     ALPTAU = ALPTAU+1
                     INDXT1 = I
                   ENDIF
                   IF (IDUP(I).EQ.-15) THEN
                     ALPTAU = ALPTAU+1
                     INDXT2 = I
                   ENDIF
                 ENDDO
C...suppose this is Z->tautau
                 IF (ALPZ.EQ.1.AND.ALPTAU.EQ.2) THEN
                   IF (IDHEP(NP1).EQ.IDUP(INDXT1)) THEN
                     POL1(3) = SPINUP(INDXT1)
                     POL2(3) = SPINUP(INDXT2)
                   ELSE IF (IDHEP(NP1).EQ.IDUP(INDXT2)) THEN
                     POL1(3) = SPINUP(INDXT2)
                     POL2(3) = SPINUP(INDXT1)
                   ENDIF
                 ENDIF
               ENDIF

c.....include polarisation effect
            ENDIF  
            
c     
c     Aug.28'08  V.S.: notice that for double-charged Higgses we need to swap NP1 and NP2 
c     (and, also, the contents of common MOMDEC) because TAUOLA expects the first particle 
c     in the list to be always tau+ and the second one tau- (so when we have, e.g., tau+tau+,
c     when decaying the second tau, we need to swap the two taus). 
c     
            if ( 
     +           ( 
     +           IDHEP(IM) .eq. KFHIGGS(1) .or. 
     +           IDHEP(IM) .eq. KFHIGGS(2) .or.
     +           IDHEP(IM) .eq. KFHIGGS(3) 
     +           ) .and. 
     +           abs(IDHEP(NP1)) .eq. KFTAU .and.  
     +           abs(IDHEP(NP2)) .eq. KFTAU
     +           ) then
c     
               IF(IDHEP(NP1).EQ.-KFTAU                        .AND.
     $              (JDAHEP(1,NP1).LE.NP1.OR.JDAHEP(1,NP1).GT.NHEP) 
     $              .AND.
     $              IDHEP(NP2).EQ. KFTAU              .AND.
     $              (JDAHEP(1,NP2).LE.NP2.OR.JDAHEP(1,NP2).GT.NHEP)
     $              ) THEN
                  IF     (IDHEP(IM).EQ.KFHIGGS(1)) THEN
                     IFPSEUDO= .FALSE.
                  ELSEIF (IDHEP(IM).EQ.KFHIGGS(2)) THEN
                     IFPSEUDO= .FALSE.
                  ELSEIF (IDHEP(IM).EQ.KFHIGGS(3)) THEN
                     IFPSEUDO= .TRUE.
                  ELSE
                     WRITE(*,*) 'Warning from TAUOLA:'
                     WRITE(*,*) 'I stop this run, wrong IDHEP(IM)=',
     $                    IDHEP(IM)
                     STOP
                  ENDIF
                  CALL SPINHIGGS(IM,NP1,NP2,IFPSEUDO,Pol1,Pol2)
c     
c     Aug.28'08  V.S. commented out an explicit call to PHOTOS
c     
c     IF (IFPHOT.EQ.1) CALL PHOTOS(IM)  ! Bremsstrahlung in Higgs decay
                                ! AFTER adding taus !!
                  
                  
               ENDIF
            ELSE
c     
c     Aug.28'08  V.S.: to take H++->tau+tau+ and H--->tau-tau- cases into account
c     
c     case 1/4:  NP1 is tau+ (which it is always supposed to be in the previous version of this interface) 
c     
               IF(IDHEP(NP1).EQ.-KFTAU.AND.
     $              (JDAHEP(1,NP1).LE.NP1.OR.JDAHEP(1,NP1).GT.NHEP))
     $              THEN
              
C     here check on if NP1 was not decayed should be verified
C     PRINT  *,'ACTUALLY '
C     (BPK)-> TAU DECAY LENGTH
                  CALL ACRMAR(RRR,1)
c     V.S.: June 23, 2008 made compiler happier  
c     VTMP=-ATAUDEC*LOG(RRR(1))
                  VTMP=-(ATAUDEC*LOG(RRR(1)))
                  DO J=1,4
                     VEORG(J)=VHEP(J,NP1)
                     VETMP(J)=VTMP*PHEP(J,NP1)/PHEP(5,NP1)
                     VHEP(J,NP1)=VHEP(J,NP1)+VETMP(J)
                  ENDDO
C     (BPK)-<
                  CALL TAUBRS(ITAUBR(1))
                  CALL DEXAY(1,POL1)
                  
C     (ET)-> Record what was done
                  CALL TDETSV(NP1, JAKP, POL1) ! DEXAY(1,...) -> Tau plus (JAKP)
C     (ET)-<
                  
c     
c     Aug.28'08  V.S. commented out an explicit call to PHOTOS
c     
c     IF (IFPHOT.EQ.1) CALL PHOTOS(NP1)
                  CALL TAUPI0(0,1)
C     (BPK)->  RESTORE TAU VERTEX
                  DO J=1,4
                     VHEP(J,NP1)=VEORG(J)
                  ENDDO
                  IF (ISHERWIG) THEN
                     ISTHEP(NP1)=195
                  ENDIF
C     (BPK)-<
                  
C     CHECK
c     DO K=JDAHEP(1,NP1),JDAHEP(2,NP1)
c     PRINT *,'K1',IDHEP(K),(VHEP(J,K),J=1,4)
c     ENDDO
               ENDIF
               
c     
c     case 2/4:  NP1 is tau- (which would be "wrong" slot for TAUOLA purposes!)
c     
               IF(IDHEP(NP1).EQ. KFTAU.AND.
     $              (JDAHEP(1,NP1).LE.NP1.OR.JDAHEP(1,NP1).GT.NHEP)) 
     $              THEN

C     here check on if NP2 was not decayed should be added
C     PRINT  *,'ACTUALLY '
c++   
c     save current NP2 and P2, replace them with NP1 and P1
                  NP2_SAVED = NP2
                  NP1_SAVED = NP1
c     
                  NP2 = NP1_SAVED
                  NP1 = NP2_SAVED
c     
                  do J=1,4
                     P1_SAVED(J) = P1(J)
                     P2_SAVED(J) = P2(J)
c     
                     P2(J) = P1_SAVED(J)
                     P1(J) = P2_SAVED(J)
                  enddo
c++   
C     (BPK)-> TAU DECAY LENGTH
                  CALL ACRMAR(RRR,1)
c     V.S.: June 23, 2008 made compiler happier
c     VTMP=-ATAUDEC*LOG(RRR(1))
                  VTMP=-(ATAUDEC*LOG(RRR(1)))
                  DO J=1,4
                     VEORG(J)=VHEP(J,NP2)
                     VETMP(J)=VTMP*PHEP(J,NP2)/PHEP(5,NP2)
                     VHEP(J,NP2)=VHEP(J,NP2)+VETMP(J)
                  ENDDO
C     (BPK)-<
                  CALL TAUBRS(ITAUBR(2))
c     notice that POL1 is used because this is the case of the FIRST tau being tau-!
                  CALL DEXAY(2,POL1)
                  
C     (ET)-> Record what was done
                  CALL TDETSV(NP2, JAKM, POL1) ! DEXAY(2,...) -> Tau minus (JAKM)
C     (ET)-<
                  
c     
c     Aug.28'08  V.S. commented out an explicit call to PHOTOS
c     
c     IF (IFPHOT.EQ.1) CALL PHOTOS(NP2)
                  CALL TAUPI0(0,2)
C     (BPK)->  RESTORE TAU VERTEX
                  DO J=1,4
                     VHEP(J,NP2)=VEORG(J)
                  ENDDO
                  IF (ISHERWIG) THEN
                     ISTHEP(NP2)=195
                  ENDIF
C     (BPK)-<
C     CHECK
c     DO K=JDAHEP(1,NP2),JDAHEP(2,NP2)
c     PRINT *,'K2',IDHEP(K),(VHEP(J,K),J=1,4)
c     ENDDO
c++   
c     Restore NP2 and P2
                  NP1 = NP1_SAVED
                  NP2 = NP2_SAVED
c     
                  do J=1,4
                     P1(J) = P1_SAVED(J)
                     P2(J) = P2_SAVED(J)
                  enddo
c++   
               ENDIF
c     
c     case 3/4:  NP2 is tau- (which it is always supposed to be in the previous version of this interface) 
c     
               IF(IDHEP(NP2).EQ. KFTAU.AND.
     $              (JDAHEP(1,NP2).LE.NP2.OR.JDAHEP(1,NP2).GT.NHEP))
     $              THEN

C     here check on if NP2 was not decayed should be added
C     PRINT  *,'ACTUALLY '
C     (BPK)-> TAU DECAY LENGTH
                  CALL ACRMAR(RRR,1)
c     V.S.: June 23, 2008 made compiler happier
c     VTMP=-ATAUDEC*LOG(RRR(1))
                  VTMP=-(ATAUDEC*LOG(RRR(1)))
                  DO J=1,4
                     VEORG(J)=VHEP(J,NP2)
                     VETMP(J)=VTMP*PHEP(J,NP2)/PHEP(5,NP2)
                     VHEP(J,NP2)=VHEP(J,NP2)+VETMP(J)
                  ENDDO
C     (BPK)-<
                  CALL TAUBRS(ITAUBR(2))
                  CALL DEXAY(2,POL2)
                  
C     (ET)-> Record what was done
                  CALL TDETSV(NP2, JAKM, POL2) ! DEXAY(2,...) -> Tau minus (JAKM)
C     (ET)-<
c     
c     Aug.28'08  V.S. commented out an explicit call to PHOTOS
c     
c     IF (IFPHOT.EQ.1) CALL PHOTOS(NP2)
                  CALL TAUPI0(0,2)
C     (BPK)->  RESTORE TAU VERTEX
                  DO J=1,4
                     VHEP(J,NP2)=VEORG(J)
                  ENDDO
                  IF (ISHERWIG) THEN
                     ISTHEP(NP2)=195
                  ENDIF
C     (BPK)-<
C     CHECK
c     DO K=JDAHEP(1,NP2),JDAHEP(2,NP2)
c     PRINT *,'K2',IDHEP(K),(VHEP(J,K),J=1,4)
c     ENDDO
               ENDIF
c     
c     case 4/4:  NP2 is tau+ (which would be "wrong" slot for TAUOLA purposes!)
c     
               IF(IDHEP(NP2).EQ.-KFTAU.AND.
     $              (JDAHEP(1,NP2).LE.NP2.OR.JDAHEP(1,NP2).GT.NHEP)) 
     $              THEN
                  
C     here check on if NP1 was not decayed should be verified
C     PRINT  *,'ACTUALLY '
c++   
c     save current NP2 and P2, replace them with NP1 and P1
                  NP2_SAVED = NP2
                  NP1_SAVED = NP1
c     
                  NP2 = NP1_SAVED
                  NP1 = NP2_SAVED
c     
                  do J=1,4
                     P1_SAVED(J) = P1(J)
                     P2_SAVED(J) = P2(J)
c     
                     P2(J) = P1_SAVED(J)
                     P1(J) = P2_SAVED(J)
                  enddo
c++   
C     (BPK)-> TAU DECAY LENGTH
                  CALL ACRMAR(RRR,1)
c     V.S.: June 23, 2008 made compiler happier  
c     VTMP=-ATAUDEC*LOG(RRR(1))
                  VTMP=-(ATAUDEC*LOG(RRR(1)))
                  DO J=1,4
                     VEORG(J)=VHEP(J,NP1)
                     VETMP(J)=VTMP*PHEP(J,NP1)/PHEP(5,NP1)
                     VHEP(J,NP1)=VHEP(J,NP1)+VETMP(J)
                  ENDDO
C     (BPK)-<
                  CALL TAUBRS(ITAUBR(1))
c     notice that POL1 is used because this is the case of the SECOND tau being tau+!
                  CALL DEXAY(1,POL2)
                  
C     (ET)-> Record what was done
                  CALL TDETSV(NP1, JAKP, POL2) ! DEXAY(1,...) -> Tau plus (JAKP)
C     (ET)-<
                  
c     
c     Aug.28'08  V.S. commented out an explicit call to PHOTOS
c     
c     IF (IFPHOT.EQ.1) CALL PHOTOS(NP1)
                  CALL TAUPI0(0,1)
C     (BPK)->  RESTORE TAU VERTEX
                  DO J=1,4
                     VHEP(J,NP1)=VEORG(J)
                  ENDDO
                  IF (ISHERWIG) THEN
                     ISTHEP(NP1)=195
                  ENDIF
C     (BPK)-<
                  
C     CHECK
c     DO K=JDAHEP(1,NP1),JDAHEP(2,NP1)
c     PRINT *,'K1',IDHEP(K),(VHEP(J,K),J=1,4)
c     ENDDO
c++   
c     Restore NP2 and P2
                  NP1 = NP1_SAVED
                  NP2 = NP2_SAVED
c     
                  do J=1,4
                     P1(J) = P1_SAVED(J)
                     P2(J) = P2_SAVED(J)
                  enddo
c++   
               ENDIF
               
               
            ENDIF
 999        CONTINUE
            NCOUNT=NCOUNT-2
            IF (NCOUNT.GT.0) GOTO 666
            IF (II.GE.NDEC) GOTO 888 
      ENDDO
      
      
 888  CONTINUE
      
      ELSEIF(MODE.EQ.1) THEN
C     ***********************
C     
         CALL DEXAY(100,POL1)
         CALL DEKAY(100,POL1x)
         WRITE(IOUT,7002)
         CALL TAUBR_PRINT(IFDMOD)
         
         CLOSE(IOUT)
         
      ENDIF
C     *****
 7001 FORMAT(///1X,15(5H*****)
     $ /,' *',     25X,'*****TAUOLA UNIVERSAL INTERFACE: ******',9X,1H*,
     $ /,' *',     25X,'*****VERSION 1.20, August   2005 ******',9X,1H*,
     $ /,' *',     25X,'**AUTHORS: P. Golonka, B. Kersevan, ***',9X,1H*,
     $ /,' *',     25X,'**T. Pierzchala, E. Richter-Was, ******',9X,1H*,
     $ /,' *',     25X,'****** Z. Was, M. Worek ***************',9X,1H*,
     $ /,' *',     25X,'**USEFUL DISCUSSIONS, IN PARTICULAR ***',9X,1H*,
     $ /,' *',     25X,'*WITH C. Biscarat and S. Slabospitzky**',9X,1H*,
     $ /,' *',     25X,'****** are warmly acknowledged ********',9X,1H*,
     $ /,' *',     25X,'                                       ',9X,1H*,
     $ /,' *',     25X,'********** INITIALIZATION  ************',9X,1H*,
     $ /,' *',F20.5,5X,'tau polarization switch must be 1 or 0 ',9X,1H*,
     $ /,' *',F20.5,5X,'Higs scalar/pseudo mix CERN-TH/2003-166',9X,1H*,
     $  /,1X,15(5H*****)/)

 7002 FORMAT(///1X,15(5H*****)
     $ /,' *',     25X,'*****TAUOLA UNIVERSAL INTERFACE: ******',9X,1H*,
     $ /,' *',     25X,'*****VERSION 1.20, August   2005 ******',9X,1H*,
     $ /,' *',     25X,'**AUTHORS: P. Golonka, B. Kersevan, ***',9X,1H*,
     $ /,' *',     25X,'**T. Pierzchala, E. Richter-Was, ******',9X,1H*,
     $ /,' *',     25X,'****** Z. Was, M. Worek ***************',9X,1H*,
     $ /,' *',     25X,'**USEFUL DISCUSSIONS, IN PARTICULAR ***',9X,1H*,
     $ /,' *',     25X,'*WITH C. Biscarat and S. Slabospitzky**',9X,1H*,
     $ /,' *',     25X,'****** are warmly acknowledged ********',9X,1H*,
     $ /,' *',     25X,'****** END OF MODULE OPERATION ********',9X,1H*,
     $  /,1X,15(5H*****)/)

      END

      SUBROUTINE SPINHIGGS(IM,NP1,NP2,IFPSEUDO,Pol1,Pol2)
      LOGICAL IFPSEUDO
      REAL*8 HH1,HH2,wthiggs,wt
      DIMENSION POL1(4), POL2(4),HH1(4),HH2(4), RRR(1)

      INTEGER ITAUBR ! DECAY MODES
      COMMON /TAUBRAN/ITAUBR(2)

      LOGICAL ISHERWIG
      COMMON /LOGHW/ISHERWIG

C this is the hepevt class in old style. No d_h_ class pre-name
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      DOUBLE PRECISION VETMP(4),VTMP,VEORG(4)

C TAU DECAY TIME IN CT UNITS (TAKEN FROM PYTHIA)
         ATAUDEC=0.0872

!             CALL DEXAY(1,POL1)  ! Kept for tests
!             CALL DEXAY(2,POL2)  ! Kept for tests
 10   CONTINUE
         CALL ACRMAR(RRR,1)
         CALL TAUBRS(ITAUBR(1))
         CALL DEKAY(1,HH1)
         CALL TAUBRS(ITAUBR(2))
         CALL DEKAY(2,HH2)
         wt=wthiggs(IFPSEUDO,HH1,HH2)
      IF (RRR(1).GT.WT) GOTO 10
C(BPK)-> TAU DECAY LENGTH
      CALL ACRMAR(RRR,1)
c V.S.: June 23, 2008 made compiler happier
c     VTMP=-ATAUDEC*LOG(RRR(1))
      VTMP=-(ATAUDEC*LOG(RRR(1)))
      DO J=1,4
         VEORG(J)=VHEP(J,NP1)
         VETMP(J)=VTMP*PHEP(J,NP1)/PHEP(5,NP1)
         VHEP(J,NP1)=VHEP(J,NP1)+VETMP(J)
      ENDDO
C(BPK)-<
      CALL TAUBRS(ITAUBR(1))
      CALL DEKAY(1+10,HH1)
      CALL TAUPI0(0,1)
C(BPK)->  RESTORE TAU VERTEX
      DO J=1,4
         VHEP(J,NP1)=VEORG(J)
      ENDDO
      IF (ISHERWIG) THEN
         ISTHEP(NP1)=195
      ENDIF
C(BPK)-<
C(BPK)-> TAU DECAY LENGTH
      CALL ACRMAR(RRR,1)
c V.S.: June 23, 2008 made compiler happier
c      VTMP=-ATAUDEC*LOG(RRR(1))
      VTMP=-(ATAUDEC*LOG(RRR(1)))
      DO J=1,4
         VEORG(J)=VHEP(J,NP2)
         VETMP(J)=VTMP*PHEP(J,NP2)/PHEP(5,NP2)
         VHEP(J,NP2)=VHEP(J,NP2)+VETMP(J)
      ENDDO
C(BPK)-<
      CALL TAUBRS(ITAUBR(2))
      CALL DEKAY(2+10,HH2)
      CALL TAUPI0(0,2)
C(BPK)->  RESTORE TAU VERTEX
      DO J=1,4
         VHEP(J,NP2)=VEORG(J)
      ENDDO
      IF (ISHERWIG) THEN
         ISTHEP(NP2)=195
      ENDIF
C(BPK)-<
      END

      FUNCTION wthiggs(IFPSEUDO,HH1,HH2)
      LOGICAL IFPSEUDO
      common /pseudocoup/ csc,ssc
      REAL*4 csc,ssc
c      save pseudocoup
      save /pseudocoup/
      REAL*8 HH1(4),HH2(4),R(4,4),wthiggs
      DO K=1,4
       DO L=1,4
        R(K,L)=0
       ENDDO
      ENDDO
      WTHIGGS=0D0
      
      R(4,4)= 1D0    !  unpolarized part
      R(3,3)=-1D0    !  longitudinal
                     !  other missing
      IF (IFPSEUDO) THEN
        R(1,1)=-1
        R(2,2)= -1
        R(1,1)=(csc**2-ssc**2)/(csc**2+ssc**2)
        R(2,2)=(csc**2-ssc**2)/(csc**2+ssc**2)
        R(1,2)=2*csc*ssc/(csc**2+ssc**2)
c V.S.: June 23, 2008 made compiler happier  
c       R(2,1)=-2*csc*ssc/(csc**2+ssc**2)
        R(2,1)=-(2*csc*ssc/(csc**2+ssc**2))
      ELSE
        R(1,1)=1
        R(2,2)=1
      ENDIF



      DO K=1,4
       DO L=1,4
        WTHIGGS=WTHIGGS+R(K,L)*HH1(K)*HH2(L)
       ENDDO
      ENDDO
        WTHIGGS=WTHIGGS/4D0
      END

      FUNCTION PLZAPX(HOPEin,IM0,NP1,NP2)
C     IM0 NP1 NP2 are the positions of Z/gamma tau tau in hepevt common block.
C     the purpose of this routine is to calculate polarization of Z along tau direction.
C     this is highly non-trivial due to necessity of reading infromation from hard process
C     history in HEPEVT, which is often written not up to the gramatic rules.
      REAL*8 PLZAPX,PLZAP0,SVAR,COSTHE,sini,sfin,ZPROP2,
     $       P1(4),P2(4),Q1(4),Q2(4),QQ(4),PH(4),PD1(4),PD2(4),
     $       PQ1(4),PQ2(4),PB(4),PA(4)
      INTEGER IM
      LOGICAL HOPE,HOPEin
C this is the hepevt class in old style. No d_h_ class pre-name
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)    ! Photos flag
* ----------------------------------------------------------------------
c      SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/



C(BPK)--> BROTHERS OF TAU ALREADY FOUND
      INTEGER ISON
      COMMON /ISONS_TAU/ISON(2)

C DO NOTHING IF REQUIRED (DIBOSON CASES)
      IF (.NOT.HOPEin) THEN
         PLZAPX=0.5
         RETURN
      ENDIF

C(BPK)--<
C >>
C >> STEP 1: find where are particles in hepevent and pick them up
C >>
             HOPE=HOPEin
C sometimes shade Z of Z is its mother ...
            IM=IM0
            IM00=JMOHEP(1,IM0)
C to protect against check on mother of beam particles.
            IF (IM00.GT.0) THEN
              IF (IDHEP(IM0).EQ.IDHEP(IM00)) IM=JMOHEP(1,IM0)
            ENDIF                                                              
C
C find (host generator-level) incoming beam-bare-particles which form Z and co.
            IMO1=JMOHEP(1,IM)
            IMO2=JMOHEP(2,IM)

C(TJB)--> CHECK THE PARTICLES AREN'T MYSTERY ZERO PARTICLES
            IM00=IMO1
            IF (ISTHEP(IM00).EQ.155) THEN
               IMO1=JMOHEP(1,IM00)
               IMO2=JMOHEP(2,IM00)
            ENDIF
            IM00=IMO1
            IF (ISTHEP(IM00).EQ.144) THEN
               IMO1=JMOHEP(1,IM00)
               IMO2=JMOHEP(2,IM00)
            ENDIF
            IM00=IMO1
            IF (ISTHEP(IM00).EQ.124) THEN
               IMO1=JMOHEP(1,IM00)
               IMO2=JMOHEP(2,IM00)
            ENDIF
C(TJB)<--

C(BPK)--> IN HERWIG THE POINTER MIGHT BE TO HARD CMS
            IM00=IMO1
            IF (ISTHEP(IM00).EQ.120) THEN
               IMO1=JMOHEP(1,IM00)
               IMO2=JMOHEP(2,IM00)
            ENDIF
C(BPK)--<

            IFFULL=0
C case when it was like e+e- --> tau+tau- gammas and e+e- were 'first' in hepevt.
      IF (IMO1.EQ.0.AND.IMO2.EQ.0) THEN
         IMO1=JMOHEP(1,NP1)
C(BPK)-->
         IF (IDHEP(IMO1).EQ.IDHEP(NP1)) IMO1=JMOHEP(1,IMO1) ! PROTECT AGAINST COPIES
C(BPK)--<
         IMO2=JMOHEP(2,NP1)
C(BPK)-->
         IF (IDHEP(IMO2).EQ.IDHEP(NP2)) IMO2=JMOHEP(1,IMO2) ! PROTECT AGAINST COPIES
C(BPK)--<
         IFFULL=1
C case when it was like  qq --> tau+tau- gammas and qq were NOT 'first' in hepevt.

c >>--GA-->>
ccc      ELSEIF (IDHEP(IM).NE.22.AND.IDHEP(IM).NE.23) THEN
      ELSEIF (
     +         IDHEP(IM).NE.22 .AND. 
     +         IDHEP(IM).NE.23 .AND.
     +         IDHEP(IM).NE.32
     +       ) THEN
c <<--GA--<<


         IMO1=JMOHEP(1,NP1)
C(BPK)-->
         IF (IDHEP(IMO1).EQ.IDHEP(NP1)) IMO1=JMOHEP(1,IMO1) ! PROTECT AGAINST COPIES
C(BPK)--<
         IMO2=JMOHEP(2,NP1)
C(BPK)-->
         IF (IDHEP(IMO2).EQ.IDHEP(NP2)) IMO2=JMOHEP(1,IMO2) ! PROTECT AGAINST COPIES
C(BPK)--<
         IFFULL=1
      ENDIF

            
C and check if it really happened
            IF (IMO1.EQ.0) HOPE=.FALSE.
            IF (IMO2.EQ.0) HOPE=.FALSE.
            IF (IMO1.EQ.IMO2) HOPE=.FALSE.

C 
         DO I=1,4
            Q1(I)= PHEP(I,NP1)              !momentum of tau+
            Q2(I)= PHEP(I,NP2)              !momentum of tau-
         ENDDO

C corrections due to possible differences in 4-momentum of shadow vs true Z.
C(BPK)-->
c >>--GA-->>
      IF ( IM.EQ.JMOHEP(1,IM0).AND.
     $      ( 
     $        IDHEP(IM).EQ.22 .OR.
     $        IDHEP(IM).EQ.23 .OR. 
     $        IDHEP(IM).EQ.32
     $      )
     $    ) THEN
ccc     $     (IDHEP(IM).EQ.22.OR.IDHEP(IM).EQ.23)) THEN
c <<--GA--<<
         DO K=1,4
            PB(K)=PHEP(K,IM)
            PA(K)=PHEP(K,IM0)
         ENDDO
C(BPK)--<

               CALL BOSTDQ( 1,PA, Q1, Q1)
               CALL BOSTDQ( 1,PA, Q2, Q2)
               CALL BOSTDQ(-1,PB, Q1, Q1)
               CALL BOSTDQ(-1,PB, Q2, Q2)

              ENDIF

         DO I=1,4                                                                                   
            QQ(I)= Q1(I)+Q2(I)              !momentum of Z
            IF (HOPE) P1(I)=PHEP(I,IMO1)    !momentum of beam1
            IF (HOPE) P2(I)=PHEP(I,IMO2)    !momentum of beam2
            PH(I)=0D0
            PD1(I)=0D0
            PD2(I)=0D0
         ENDDO
!        These momenta correspond to  quarks, gluons photons or taus
                   IDFQ1=IDHEP(NP1)
                   IDFQ2=IDHEP(NP2)
         IF (HOPE) IDFP1=IDHEP(IMO1)
         IF (HOPE) IDFP2=IDHEP(IMO2)

         SVAR=QQ(4)**2-QQ(3)**2-QQ(2)**2-QQ(1)**2
         IF (.NOT.HOPE) THEN
C options which may be useful in some cases of two heavy boson production
C need individual considerations. To be developed.
C           PLZAPX=PLZAP0(11,IDFQ1,SVAR,0D0)  ! gamma/Z mixture as if produced from e beam
C           PLZAPX=PLZAP0(12,IDFQ1,SVAR,0D0)  ! pure Z
           PLZAPX=0.5                         ! pure gamma
           RETURN
         ENDIF
C >>
C >> STEP 2 look for brothers of Z which have to be included in effective incoming particles
C >>
C let us define beginning and end of particles which are produced in parallel to Z
C in principle following should work

C(BPK)--> ACCOMMODATE FOR HERWIG - IM00 POINTS TO BEAM PARTICLE OR HARD CMS
         NX1=JDAHEP(1,IM00)
         NX2=JDAHEP(2,IM00)
C but ...
         INBR=IM ! OK, HARD RECORD Z/GAMMA 
         IF (IFFULL.EQ.1) INBR=NP1  ! OK, NO Z/GAMMA
         IF (IDHEP(JMOHEP(1,INBR)).EQ.IDHEP(INBR)) INBR=JMOHEP(1,INBR) ! FORCE HARD RECORD
C(BPK)--<
         IF(NX1.EQ.0.OR.NX2.EQ.0) THEN
           NX1=INBR
           NX2=INBR
           DO K=1,INBR-1
             IF(JMOHEP(1,INBR-K).EQ.JMOHEP(1,INBR)) THEN
              NX1=INBR-K
             ELSE
                GOTO 7
             ENDIF
           ENDDO
 7         CONTINUE

           DO K=INBR+1,NHEP
             IF(JMOHEP(1,K).EQ.JMOHEP(1,INBR)) THEN
              NX2=K
             ELSE
                GOTO 8
             ENDIF
           ENDDO
 8         CONTINUE
         ENDIF

C case of annihilation of two bosons is hopeles
         IF (ABS(IDFP1).GE.20.AND.ABS(IDFP2).GE.20) HOPE=.FALSE.
C case of annihilation of non-matching flavors is hopeless
         IF (ABS(IDFP1).LE.20.AND.ABS(IDFP2).LE.20.AND.IDFP1+IDFP2.NE.0)
     $       HOPE=.FALSE.
         IF (.NOT.HOPE) THEN
C options which may be useful in some cases of two heavy boson production
C need individual considerations. To be developed.
C           PLZAPX=PLZAP0(11,IDFQ1,SVAR,0D0)  ! gamma/Z mixture as if produced from e beam
C           PLZAPX=PLZAP0(12,IDFQ1,SVAR,0D0)  ! pure Z
           PLZAPX=0.5                         ! pure gamma
           RETURN
         ENDIF
         IF (ABS(IDFP1).LT.20) IDE= IDFP1
         IF (ABS(IDFP2).LT.20) IDE=-IDFP2


C >>
C >> STEP 3  we combine gluons, photons  into incoming effective beams
C >>

C in the following we will ignore the possibility of photon emission from taus
C however at certain moment it will be necessary to take care of

           DO L=1,4
            PD1(L)=P1(L)
            PD2(L)=P2(L)
           ENDDO 

                DO L=1,4
                PQ1(L)=Q1(L)
                PQ2(L)=Q2(L)
                ENDDO 

         IFLAV=min(ABS(IDFP1),ABS(IDFP2))

*--------------------------------------------------------------------------
c                  IFLAV=min(ABS(IDFP1),ABS(IDFP2))
c that means that always quark or lepton i.e. process like
c      f CLHEP::g(gamma) --> f Z0 --> tau tau
c we glue  fermions to effective  beams that is f f --> Z0 --> tau tau
c  with  gamma/CLHEP::g emission from initial fermion.
*---------------------------------------------------------------------------
 
         IF (ABS(IDFP1).GE.20) THEN
           DO k=NX1,NX2
             IDP=IDHEP(k)
             IF (ABS(IDP).EQ.IFLAV) THEN
               DO L=1,4
                 PD1(L)=-PHEP(L,K)
               ENDDO
             ENDIF
           ENDDO
         ENDIF

         IF (ABS(IDFP2).GE.20) THEN
           DO k=NX1,NX2
             IDP=IDHEP(k)
             IF (ABS(IDP).EQ.IFLAV) THEN
               DO L=1,4
                 PD2(L)=-PHEP(L,K)
               ENDDO
             ENDIF
           ENDDO
         ENDIF

C if first beam was boson: gluing

         IF (ABS(IDFP1).GE.20) THEN
           DO L=1,4
             PH(L)=P1(L)
           ENDDO
           xm1=abs((PD1(4)+PH(4))**2-(PD1(3)+PH(3))**2
     $            -(PD1(2)+PH(2))**2-(PD1(1)+PH(1))**2)
           xm2=abs((PD2(4)+PH(4))**2-(PD2(3)+PH(3))**2
     $            -(PD2(2)+PH(2))**2-(PD2(1)+PH(1))**2)
          IF (XM1.LT.XM2) THEN
             DO L=1,4
               PD1(L)=PD1(L)+P1(L)
             ENDDO
           ELSE
             DO L=1,4
               PD2(L)=PD2(L)+P1(L)
             ENDDO
           ENDIF
         ENDIF

C if second beam was boson: gluing 


         IF (ABS(IDFP2).GE.20) THEN
           DO L=1,4
             PH(L)=P2(L)
           ENDDO
           xm1=abs((PD1(4)+PH(4))**2-(PD1(3)+PH(3))**2
     $            -(PD1(2)+PH(2))**2-(PD1(1)+PH(1))**2)
           xm2=abs((PD2(4)+PH(4))**2-(PD2(3)+PH(3))**2
     $            -(PD2(2)+PH(2))**2-(PD2(1)+PH(1))**2)
           IF (XM1.LT.XM2) THEN
             DO L=1,4
               PD1(L)=PD1(L)+P2(L)
             ENDDO
           ELSE
             DO L=1,4
               PD2(L)=PD2(L)+P2(L)
             ENDDO
           ENDIF
         ENDIF

C now spectators ...

C(BPK)-->
      NPH1=NP1
      NPH2=NP2
      IF (IDHEP(JMOHEP(1,NP1)).EQ.IDHEP(NP1)) NPH1=JMOHEP(1,NP1) ! SHOULD PUT US IN HARD REC.
      IF (IDHEP(JMOHEP(1,NP2)).EQ.IDHEP(NP2)) NPH2=JMOHEP(1,NP2) ! SHOULD PUT US IN HARD REC.
C(BPK)--<

         DO k=NX1,NX2
         IF (ABS(IDHEP(K)).NE.IFLAV.AND.K.NE.IM.AND.
C(BPK)-->
     $       K.NE.NPH1.AND.K.NE.NPH2) THEN 
C(BPK)--<
          IF(IDHEP(K).EQ.22.AND.IFFULL.EQ.1) THEN
            DO L=1,4
              PH(L)=PHEP(L,K)
            ENDDO
            xm1=abs((PD1(4)-PH(4))**2-(PD1(3)-PH(3))**2
     $             -(PD1(2)-PH(2))**2-(PD1(1)-PH(1))**2)
            xm2=abs((PD2(4)-PH(4))**2-(PD2(3)-PH(3))**2
     $             -(PD2(2)-PH(2))**2-(PD2(1)-PH(1))**2)
           xm3=abs((PQ1(4)+PH(4))**2-(PQ1(3)+PH(3))**2
     $            -(PQ1(2)+PH(2))**2-(PQ1(1)+PH(1))**2)
           xm4=abs((PQ2(4)+PH(4))**2-(PQ2(3)+PH(3))**2
     $            -(PQ2(2)+PH(2))**2-(PQ2(1)+PH(1))**2)

  
            sini=abs((PD1(4)+PD2(4)-PH(4))**2-(PD1(3)+PD2(3)-PH(3))**2
     $              -(PD1(2)+PD2(2)-PH(2))**2-(PD1(1)+PD2(1)-PH(1))**2)
            sfin=abs((PD1(4)+PD2(4)      )**2-(PD1(3)+PD2(3)      )**2
     $              -(PD1(2)+PD2(2)      )**2-(PD1(1)+PD2(1)      )**2)

           FACINI=ZPROP2(sini)
           FACFIN=ZPROP2(sfin)

           XM1=XM1/FACINI
           XM2=XM2/FACINI
           XM3=XM3/FACFIN
           XM4=XM4/FACFIN

           XM=MIN(XM1,XM2,XM3,XM4)
                  IF      (XM1.EQ.XM) THEN 
                     DO L=1,4
                       PD1(L)=PD1(L)-PH(L)
                     ENDDO
                  ELSEIF   (XM2.EQ.XM) THEN 
                     DO L=1,4
                       PD2(L)=PD2(L)-PH(L)
                     ENDDO
                  ELSEIF   (XM3.EQ.XM) THEN 
                     DO L=1,4
                        Q1(L)=PQ1(L)+PH(L)
                     ENDDO
                  ELSE
                     DO L=1,4
                        Q2(L)=PQ2(L)+PH(L)
                     ENDDO
                  ENDIF
           ELSE
            DO L=1,4
              PH(L)=PHEP(L,K)
            ENDDO
            xm1=abs((PD1(4)-PH(4))**2-(PD1(3)-PH(3))**2
     $             -(PD1(2)-PH(2))**2-(PD1(1)-PH(1))**2)
            xm2=abs((PD2(4)-PH(4))**2-(PD2(3)-PH(3))**2
     $             -(PD2(2)-PH(2))**2-(PD2(1)-PH(1))**2)
            IF (XM1.LT.XM2) THEN
              DO L=1,4
                PD1(L)=PD1(L)-PH(L)
              ENDDO
            ELSE
              DO L=1,4
                PD2(L)=PD2(L)-PH(L)
              ENDDO
            ENDIF
           ENDIF
          ENDIF
         ENDDO


C >>
C >> STEP 4 look for brothers of tau (sons of Z!) which have to be included in 
c >>          effective outcoming taus
C >>
C let us define beginning and end of particles which are produced in 
c  parallel to tau



C find outcoming particles which come from Z   

       
 

C(BPK)--> OK, IT WOULD HAVE TO BE ALONG TAUS IN HARD RECORD WITH THE SAME MOTHER       

c >>--GA-->>
C      IF (ABS(IDHEP(IM0)).EQ.22.OR.abs(IDHEP(IM0)).EQ.23) THEN
      IF ( 
     $     abs(IDHEP(IM0)) .eq. 22 .or. 
     $     abs(IDHEP(IM0)) .eq. 23 .or. 
     $     abs(IDHEP(IM0)) .eq. 32 
     $   ) THEN
c <<--GA--<<
         DO K=ISON(1),ISON(2)
c
c  VS Jan.12,09, just a comment: this, as I understand it, would be a radiative photon from gamma/Z/Z' production (but we now always run PHOTOS AFTER TAUOLA)
c
            IF(ABS(IDHEP(K)).EQ.22) THEN
C(BPK)--< 

              do l=1,4
              ph(l)=phep(l,k)
              enddo

           xm3=abs((PQ1(4)+PH(4))**2-(PQ1(3)+PH(3))**2
     $            -(PQ1(2)+PH(2))**2-(PQ1(1)+PH(1))**2)
           xm4=abs((PQ2(4)+PH(4))**2-(PQ2(3)+PH(3))**2
     $            -(PQ2(2)+PH(2))**2-(PQ2(1)+PH(1))**2)  

           XM=MIN(XM3,XM4) 

                  IF   (XM3.EQ.XM) THEN 
                     DO L=1,4
                        Q1(L)=PQ1(L)+PH(L)
                     ENDDO
                  ELSE
                     DO L=1,4
                        Q2(L)=PQ2(L)+PH(L)
                     ENDDO
                  ENDIF
            endif
          enddo
          ENDIF



*------------------------------------------------------------------------


C out of effective momenta we calculate COSTHE and later polarization
      CALL ANGULU(PD1,PD2,Q1,Q2,COSTHE)
     
      PLZAPX=PLZAP0(IDE,IDFQ1,SVAR,COSTHE)
      END

      SUBROUTINE ANGULU(PD1,PD2,Q1,Q2,COSTHE)
      REAL*8 PD1(4),PD2(4),Q1(4),Q2(4),COSTHE,P(4),QQ(4),QT(4)
C take effective beam which is less massive, it should be irrelevant
C but in case HEPEVT is particulary dirty may help.
C this routine calculate reduced system transver and cosine of scattering 
C angle.

      XM1=ABS(PD1(4)**2-PD1(3)**2-PD1(2)**2-PD1(1)**2)
      XM2=ABS(PD2(4)**2-PD2(3)**2-PD2(2)**2-PD2(1)**2)
      IF (XM1.LT.XM2) THEN
        SIGN=1D0
        DO K=1,4
          P(K)=PD1(K)
        ENDDO
      ELSE
        SIGN=-1D0
        DO K=1,4
          P(K)=PD2(K)
        ENDDO
      ENDIF
C calculate space like part of P (in Z restframe)
      DO K=1,4
       QQ(K)=Q1(k)+Q2(K)
       QT(K)=Q1(K)-Q2(K)
      ENDDO

       XMQQ=SQRT(QQ(4)**2-QQ(3)**2-QQ(2)**2-QQ(1)**2)

       QTXQQ=QT(4)*QQ(4)-QT(3)*QQ(3)-QT(2)*QQ(2)-QT(1)*QQ(1)
      DO K=1,4
       QT(K)=QT(K)-QQ(K)*QTXQQ/XMQQ**2
      ENDDO

       PXQQ=P(4)*QQ(4)-P(3)*QQ(3)-P(2)*QQ(2)-P(1)*QQ(1)
      DO K=1,4
       P(K)=P(K)-QQ(K)*PXQQ/XMQQ**2
      ENDDO
C calculate costhe
       PXP  =SQRT(p(1)**2+p(2)**2+p(3)**2-p(4)**2)
       QTXQT=SQRT(QT(3)**2+QT(2)**2+QT(1)**2-QT(4)**2)
       PXQT =P(3)*QT(3)+P(2)*QT(2)+P(1)*QT(1)-P(4)*QT(4)
       COSTHE=PXQT/PXP/QTXQT
       COSTHE=COSTHE*SIGN
      END

      FUNCTION PLZAP0(IDE,IDF,SVAR,COSTH0)
C this function calculates probability for the helicity +1 +1 configuration
C of taus for given Z/gamma transfer and COSTH0 cosine of scattering angle
      REAL*8 PLZAP0,SVAR,COSTHE,COSTH0
c
      external T_BORN_ANY
      real*8 T_BORN_ANY
c
      logical FIRST
      data FIRST / .true. /
c
      if ( FIRST ) then
c
        write(*,*)'Tauola_i PLZAP0 INFO '//
     +'gamma*/Z0 interference is ALWAYS taken into account in decays of tau pairs'
        write(*,*)'Tauola_i PLZAP0 INFO '//
     +'gamma*/Z0/Z''0 interference is also ALWAYS included for Z''0 produced by PYTHIA'
c
        FIRST = .false.
c
      endif
c
      COSTHE=COSTH0
C >>>>>      IF (IDE*IDF.LT.0) COSTHE=-COSTH0 ! this is probably not needed ID
C >>>>>      of first beam is used by T_GIVIZ0 including sign

      IF (IDF.GT.0) THEN
        CALL INITWK_ANY(IDE,IDF,SVAR)
      ELSE
        CALL INITWK_ANY(-IDE,-IDF,SVAR)
      ENDIF
      PLZAP0=T_BORN_ANY(0,SVAR,COSTHE,1D0,1D0)
     $  /(T_BORN_ANY(0,SVAR,COSTHE,1D0,1D0)+
     $    T_BORN_ANY(0,SVAR,COSTHE,-1D0,-1D0))

!      PLZAP0=0.5
      END
c
      FUNCTION T_BORN_ANY(MODE,SVAR,COSTHE,TA,TB)
      implicit NONE
c
      integer MODE
      real*8 SVAR, COSTHE, TA, TB
c
      real*8 T_BORN_ANY
c
      common / LOGPH / IS_Z0P, IS_PYTHIA
      logical IS_Z0P
      logical IS_PYTHIA
c
      external T_BORN
      external T_BORN_ZP
c
      real*8 T_BORN
      real*8 T_BORN_ZP
c
      if ( IS_Z0P .and. IS_PYTHIA ) then 
        T_BORN_ANY = T_BORN_ZP(MODE,SVAR,COSTHE,TA,TB)
      else
        T_BORN_ANY = T_BORN(MODE,SVAR,COSTHE,TA,TB)
      endif
c
      return
      end
c
      FUNCTION T_BORN(MODE,SVAR,COSTHE,TA,TB)
C ----------------------------------------------------------------------
C THIS ROUTINE PROVIDES BORN CROSS SECTION. IT HAS THE SAME         
C STRUCTURE AS FUNTIS AND FUNTIH, THUS CAN BE USED AS SIMPLER       
C EXAMPLE OF THE METHOD APPLIED THERE                               
C INPUT PARAMETERS ARE: SVAR    -- transfer
C                       COSTHE  -- cosine of angle between tau+ and 1st beam
C                       TA,TB   -- helicity states of tau+ tau-
C
C     called by : BORNY, BORAS, BORNV, WAGA, WEIGHT
C ----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
      COMMON / T_BEAMPM / ENE ,AMIN,AMFIN,IDE,IDF
      REAL*8              ENE ,AMIN,AMFIN
      COMMON / T_GAUSPM /SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI   ,XUPZI   ,XUPGF   ,XUPZF
     &                  ,NDIAG0,NDIAGA,KEYA,KEYZ
     &                  ,ITCE,JTCE,ITCF,JTCF,KOLOR
      REAL*8             SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI(2),XUPZI(2),XUPGF(2),XUPZF(2)
      REAL*8            SEPS1,SEPS2
C=====================================================================
      COMMON / T_GSWPRM /SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
      REAL*8             SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
C     SWSQ        = sin2 (theta Weinberg)
C     AMW,AMZ     = W & Z boson masses respectively
C     AMH         = the Higgs mass
C     AMTOP       = the top mass
C     GAMMZ       = Z0 width
      COMPLEX*16 ABORN(2,2),APHOT(2,2),AZETT(2,2)
      COMPLEX*16 XUPZFP(2),XUPZIP(2)
      COMPLEX*16 ABORNM(2,2),APHOTM(2,2),AZETTM(2,2)
      COMPLEX*16 PROPA,PROPZ
c      COMPLEX*16 XR,XI
      COMPLEX*16 XUPF,XUPI
c,XFF(4),XFEM,XFOTA,XRHO,XKE,XKF,XKEF
      COMPLEX*16 XTHING
c,XVE,XVF,XVEF
c      DATA XI/(0.D0,1.D0)/,XR/(1.D0,0.D0)/
      DATA MODE0 /-5/
      DATA IDE0 /-55/
      DATA SVAR0,COST0 /-5.D0,-6.D0/
c      DATA PI /3.141592653589793238462643D0/
      DATA SEPS1,SEPS2 /0D0,0D0/
C
c
C      write(*,*)
C     +'Tauola_i T_BORN INFO Taking gamma*/Z0 interference into account'
c
C MEMORIZATION =========================================================
      IF ( MODE.NE.MODE0.OR.SVAR.NE.SVAR0.OR.COSTHE.NE.COST0
     $    .OR.IDE0.NE.IDE)THEN
C
        KEYGSW=1
C ** PROPAGATORS
        IDE0=IDE
        MODE0=MODE
        SVAR0=SVAR
        COST0=COSTHE
        SINTHE=SQRT(1.D0-COSTHE**2)
        BETA=SQRT(MAX(0D0,1D0-4D0*AMFIN**2/SVAR))
C I MULTIPLY AXIAL COUPLING BY BETA FACTOR.
        XUPZFP(1)=0.5D0*(XUPZF(1)+XUPZF(2))+0.5*BETA*(XUPZF(1)-XUPZF(2))
        XUPZFP(2)=0.5D0*(XUPZF(1)+XUPZF(2))-0.5*BETA*(XUPZF(1)-XUPZF(2))
        XUPZIP(1)=0.5D0*(XUPZI(1)+XUPZI(2))+0.5*(XUPZI(1)-XUPZI(2))
        XUPZIP(2)=0.5D0*(XUPZI(1)+XUPZI(2))-0.5*(XUPZI(1)-XUPZI(2))
C FINAL STATE VECTOR COUPLING
        XUPF     =0.5D0*(XUPZF(1)+XUPZF(2))
        XUPI     =0.5D0*(XUPZI(1)+XUPZI(2))
        XTHING   =0D0

        PROPA =1D0/SVAR
        PROPZ =1D0/DCMPLX(SVAR-AMZ**2,SVAR/AMZ*GAMMZ)
        IF (KEYGSW.EQ.0) PROPZ=0.D0
        DO 50 I=1,2
         DO 50 J=1,2
          REGULA= (3-2*I)*(3-2*J) + COSTHE
c V.S.: June 23, 2008 made compiler happier  
c         REGULM=-(3-2*I)*(3-2*J) * SINTHE *2.D0*AMFIN/SQRT(SVAR)
          REGULM=-((3-2*I)*(3-2*J) * SINTHE *2.D0*AMFIN/SQRT(SVAR))
          APHOT(I,J)=PROPA*(XUPGI(I)*XUPGF(J)*REGULA)
          AZETT(I,J)=PROPZ*(XUPZIP(I)*XUPZFP(J)+XTHING)*REGULA
          ABORN(I,J)=APHOT(I,J)+AZETT(I,J)
          APHOTM(I,J)=PROPA*DCMPLX(0D0,1D0)*XUPGI(I)*XUPGF(J)*REGULM
          AZETTM(I,J)=
c V.S.: Jan. 16, 2009 (extended the line) 
     +PROPZ*DCMPLX(0D0,1D0)*(XUPZIP(I)*XUPF+XTHING)*REGULM
          ABORNM(I,J)=APHOTM(I,J)+AZETTM(I,J)
   50   CONTINUE
      ENDIF
C
C******************
C* IN CALCULATING CROSS SECTION ONLY DIAGONAL ELEMENTS
C* OF THE SPIN DENSITY MATRICES ENTER (LONGITUD. POL. ONLY.)
C* HELICITY CONSERVATION EXPLICITLY OBEYED
      POLAR1=  (SEPS1)
      POLAR2= (-SEPS2)
      BORN=0D0
      DO 150 I=1,2
       HELIC= 3-2*I
       DO 150 J=1,2
        HELIT=3-2*J
        FACTOR=KOLOR*(1D0+HELIC*POLAR1)*(1D0-HELIC*POLAR2)/4D0
        FACTOM=FACTOR*(1+HELIT*TA)*(1-HELIT*TB)
        FACTOR=FACTOR*(1+HELIT*TA)*(1+HELIT*TB)

        BORN=BORN+CDABS(ABORN(I,J))**2*FACTOR
C      MASS TERM IN BORN
        IF (MODE.GE.1) THEN
         BORN=BORN+CDABS(ABORNM(I,J))**2*FACTOM
        ENDIF

  150 CONTINUE
C************
      FUNT=BORN
      IF(FUNT.LT.0.D0)  FUNT=BORN

C
      IF (SVAR.GT.4D0*AMFIN**2) THEN
C PHASE SPACE THRESHOLD FACTOR
        THRESH=SQRT(1-4D0*AMFIN**2/SVAR)
        T_BORN= FUNT*SVAR**2*THRESH
      ELSE
        THRESH=0.D0
        T_BORN=0.D0
      ENDIF
C ZW HERE WAS AN ERROR 19. 05. 1989
!      write(*,*) 'KKKK ',PROPA,PROPZ,XUPGI,XUPGF,XUPZI,XUPZF
!      write(*,*) 'KKKK X',svar,costhe,TA,TB,T_BORN
      END
c
      FUNCTION T_BORN_ZP(MODE,SVAR,COSTHE,TA,TB)
C ----------------------------------------------------------------------
C THIS ROUTINE PROVIDES BORN CROSS SECTION. IT HAS THE SAME         
C STRUCTURE AS FUNTIS AND FUNTIH, THUS CAN BE USED AS SIMPLER       
C EXAMPLE OF THE METHOD APPLIED THERE                               
C INPUT PARAMETERS ARE: SVAR    -- transfer
C                       COSTHE  -- cosine of angle between tau+ and 1st beam
C                       TA,TB   -- helicity states of tau+ tau-
C
C     called by : BORNY, BORAS, BORNV, WAGA, WEIGHT
C ----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
      COMMON / T_BEAMPM / ENE ,AMIN,AMFIN,IDE,IDF
      REAL*8              ENE ,AMIN,AMFIN
      COMMON / T_GAUSPM /SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI   ,XUPZI   ,XUPGF   ,XUPZF
     &                  ,NDIAG0,NDIAGA,KEYA,KEYZ
     &                  ,ITCE,JTCE,ITCF,JTCF,KOLOR
c >>--GA-->> new variables analogous to xupzi and xupzf, but for the Z'
      COMMON / T_GAUSPM_ZP / xupzpi, xupzpf
c <<--GA--<<
      REAL*8             SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI(2),XUPZI(2),XUPGF(2),XUPZF(2)
c >>--GA-->>
      real *8 xupzpi(2), xupzpf(2)
c <<--GA--<<
      REAL*8            SEPS1,SEPS2
C=====================================================================
      COMMON / T_GSWPRM /SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
      REAL*8             SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
c >>--GA-->> add Z' mass and width
      common /zprm/ amzp, gammzp
      real*8 amzp, gammzp
c <<--GA--<<
C     SWSQ        = sin2 (theta Weinberg)
C     AMW,AMZ     = W & Z boson masses respectively
C     AMH         = the Higgs mass
C     AMTOP       = the top mass
C     GAMMZ       = Z0 width
      COMPLEX*16 ABORN(2,2),APHOT(2,2),AZETT(2,2)
      COMPLEX*16 XUPZFP(2),XUPZIP(2)
      COMPLEX*16 ABORNM(2,2),APHOTM(2,2),AZETTM(2,2)
c >>--GA-->> added variables required for Z'
      complex*16 xuppip(2), xuppfp(2)
      complex*16 azptt(2,2)
      complex*16 azpttm(2,2)
      complex*16 propzp, xuppi, xuppf
c <<--GA--<<
      COMPLEX*16 PROPA,PROPZ
c      COMPLEX*16 XR,XI
      COMPLEX*16 XUPF,XUPI
c,XFF(4),XFEM,XFOTA,XRHO,XKE,XKF,XKEF
      COMPLEX*16 XTHING
c,XVE,XVF,XVEF
c      DATA XI/(0.D0,1.D0)/,XR/(1.D0,0.D0)/
      DATA MODE0 /-5/
      DATA IDE0 /-55/
      DATA SVAR0,COST0 /-5.D0,-6.D0/
c      DATA PI /3.141592653589793238462643D0/
      DATA SEPS1,SEPS2 /0D0,0D0/
C
c
C      write(*,*)
C     +'Tauola_i T_BORN_ZP INFO Taking gamma*/Z0/Z''0 interference into account'
c
C MEMORIZATION =========================================================
      IF ( MODE.NE.MODE0.OR.SVAR.NE.SVAR0.OR.COSTHE.NE.COST0
     $    .OR.IDE0.NE.IDE)THEN
C
        KEYGSW=1
C ** PROPAGATORS
        IDE0=IDE
        MODE0=MODE
        SVAR0=SVAR
        COST0=COSTHE
        SINTHE=SQRT(1.D0-COSTHE**2)
        BETA=SQRT(MAX(0D0,1D0-4D0*AMFIN**2/SVAR))
C I MULTIPLY AXIAL COUPLING BY BETA FACTOR.
        XUPZFP(1)=0.5D0*(XUPZF(1)+XUPZF(2))+0.5*BETA*(XUPZF(1)-XUPZF(2))
        XUPZFP(2)=0.5D0*(XUPZF(1)+XUPZF(2))-0.5*BETA*(XUPZF(1)-XUPZF(2))
        XUPZIP(1)=0.5D0*(XUPZI(1)+XUPZI(2))+0.5*(XUPZI(1)-XUPZI(2))
        XUPZIP(2)=0.5D0*(XUPZI(1)+XUPZI(2))-0.5*(XUPZI(1)-XUPZI(2))
C FINAL STATE VECTOR COUPLING
        XUPF     =0.5D0*(XUPZF(1)+XUPZF(2))
        XUPI     =0.5D0*(XUPZI(1)+XUPZI(2))
        XTHING   =0D0

        PROPA =1D0/SVAR
        PROPZ =1D0/DCMPLX(SVAR-AMZ**2,SVAR/AMZ*GAMMZ)
c >>--GA-->>
        xuppfp(1)=0.5d0*(xupzpf(1)+xupzpf(2))
     &            + 0.5*beta*(xupzpf(1)-xupzpf(2))
        xuppfp(2)=0.5d0*(xupzpf(1)+xupzpf(2))
     &            - 0.5*beta*(xupzpf(1)-xupzpf(2))
        xuppip(1)=0.5d0*(xupzpi(1)+xupzpi(2))+0.5*(xupzpi(1)-xupzpi(2))
        xuppip(2)=0.5d0*(xupzpi(1)+xupzpi(2))-0.5*(xupzpi(1)-xupzpi(2))
        xuppf = 0.5D0 * (xupzpf(1)+xupzpf(2))
        xuppi = 0.5D0 * (xupzpi(1)+xupzpi(2))
        propzp = 1d0/DCMPLX(svar-amzp**2,svar/amzp*gammzp)
c <<--GA--<<
        IF (KEYGSW.EQ.0) PROPZ=0.D0
        DO 50 I=1,2
         DO 50 J=1,2
          REGULA= (3-2*I)*(3-2*J) + COSTHE
c V.S.: June 23, 2008 made compiler happier  
c         REGULM=-(3-2*I)*(3-2*J) * SINTHE *2.D0*AMFIN/SQRT(SVAR)
          REGULM=-((3-2*I)*(3-2*J) * SINTHE *2.D0*AMFIN/SQRT(SVAR))
          APHOT(I,J)=PROPA*(XUPGI(I)*XUPGF(J)*REGULA)
          AZETT(I,J)=PROPZ*(XUPZIP(I)*XUPZFP(J)+XTHING)*REGULA
          ABORN(I,J)=APHOT(I,J)+AZETT(I,J)
c >>--GA-->>
          azptt(i,j) = propzp * (xuppip(i)*xuppfp(j)+xthing) * regula
c          write(6,*)i,j,aborn(i,j),azptt(i,j)
          aborn(i,j) = aborn(i,j)+azptt(i,j)
c <<--GA--<<
          APHOTM(I,J)=PROPA*DCMPLX(0D0,1D0)*XUPGI(I)*XUPGF(J)*REGULM
          AZETTM(I,J)=
c V.S.: Jan. 16, 2009 (extended the line) 
     +PROPZ*DCMPLX(0D0,1D0)*(XUPZIP(I)*XUPF+XTHING)*REGULM
c
          ABORNM(I,J)=APHOTM(I,J)+AZETTM(I,J)
c >>--GA-->>
          azpttm(i,j) = propzp*DCMPLX(0d0,1d0)*(xuppip(i)*xuppf+xthing)
     &                 *regulm
c          write(6,*)i,j,abornm(i,j),azpttm(i,j)
          abornm(i,j) = abornm(i,j) + azpttm(i,j)
c <<--GA--<<
   50   CONTINUE
      ENDIF
C
C******************
C* IN CALCULATING CROSS SECTION ONLY DIAGONAL ELEMENTS
C* OF THE SPIN DENSITY MATRICES ENTER (LONGITUD. POL. ONLY.)
C* HELICITY CONSERVATION EXPLICITLY OBEYED
      POLAR1=  (SEPS1)
      POLAR2= (-SEPS2)
      BORN=0D0
      DO 150 I=1,2
       HELIC= 3-2*I
       DO 150 J=1,2
        HELIT=3-2*J
        FACTOR=KOLOR*(1D0+HELIC*POLAR1)*(1D0-HELIC*POLAR2)/4D0
        FACTOM=FACTOR*(1+HELIT*TA)*(1-HELIT*TB)
        FACTOR=FACTOR*(1+HELIT*TA)*(1+HELIT*TB)

        BORN=BORN+CDABS(ABORN(I,J))**2*FACTOR
C      MASS TERM IN BORN
        IF (MODE.GE.1) THEN
         BORN=BORN+CDABS(ABORNM(I,J))**2*FACTOM
        ENDIF

  150 CONTINUE
C************
      FUNT=BORN
      IF(FUNT.LT.0.D0)  FUNT=BORN

C
      IF (SVAR.GT.4D0*AMFIN**2) THEN
C PHASE SPACE THRESHOLD FACTOR
        THRESH=SQRT(1-4D0*AMFIN**2/SVAR)
        T_BORN_ZP= FUNT*SVAR**2*THRESH
      ELSE
        THRESH=0.D0
        T_BORN_ZP=0.D0
      ENDIF
C ZW HERE WAS AN ERROR 19. 05. 1989
!      write(*,*) 'KKKK ',PROPA,PROPZ,XUPGI,XUPGF,XUPZI,XUPZF
!      write(*,*) 'KKKK X',svar,costhe,TA,TB,T_BORN_ZP
      END

      SUBROUTINE INITWK_ANY(IDEX,IDFX,SVAR)
      implicit NONE
c
      integer IDEX
      integer IDFX
c
      real*8 SVAR
c
      common / LOGPH / IS_Z0P, IS_PYTHIA
      logical IS_Z0P
      logical IS_PYTHIA
c
      if ( IS_Z0P .and. IS_PYTHIA ) then
        call INITWK_ZP(IDEX,IDFX,SVAR)
      else
        call INITWK(IDEX,IDFX,SVAR)
      endif
c
      return
      end
c
      SUBROUTINE INITWK(IDEX,IDFX,SVAR)
! initialization routine coupling masses etc.
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON / T_BEAMPM / ENE ,AMIN,AMFIN,IDE,IDF
      REAL*8              ENE ,AMIN,AMFIN
      COMMON / T_GAUSPM /SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI   ,XUPZI   ,XUPGF   ,XUPZF
     &                  ,NDIAG0,NDIAGA,KEYA,KEYZ
     &                  ,ITCE,JTCE,ITCF,JTCF,KOLOR
      REAL*8             SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI(2),XUPZI(2),XUPGF(2),XUPZF(2)
      COMMON / T_GSWPRM /SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
      REAL*8             SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
C     SWSQ        = sin2 (theta Weinberg)
C     AMW,AMZ     = W & Z boson masses respectively
C     AMH         = the Higgs mass
C     AMTOP       = the top mass
C     GAMMZ       = Z0 width
c
C      write(*,*)
C     +'Tauola_i INITWK INFO Taking gamma*/Z0 interference into account'
c
C
      ENE=SQRT(SVAR)/2
      AMIN=0.511D-3
      SWSQ=0.23147
      AMZ=91.1882
      GAMMZ=2.4952
      IF     (IDFX.EQ. 15) then       
        IDF=2  ! denotes tau +2 tau-
        AMFIN=1.77703 !this mass is irrelevant if small, used in ME only
      ELSEIF (IDFX.EQ.-15) then
        IDF=-2  ! denotes tau -2 tau-
        AMFIN=1.77703 !this mass is irrelevant if small, used in ME only
      ELSE
c
c  V.S. and R.Y.:  June 17, 2008, just a comment:  notice that (on ATLAS) 
c  there are no (shared or not) libraries that contain "HWUEPR" anymore!
c  Also, executing "STOP" does not seem to be a good idea in production!
c  Actually, we successfully used an instance of a crash in this piece of 
c  code to diagnose a serious problem when both PYTHIA AND TAUOLA were 
c  asked to decay tau leptons! We protected the code against such scenario,
c  however, in principle, one may still be able to end up right here, where
c  ATHENA would "crash" because of FORTRAN's "STOP"! We leave modifying 
c  THIS piece of code for future work. 
c
        WRITE(*,*) 'INITWK: WRONG IDFX',IDFX

C Disable the call to the non-existent HWUEPR subroutine when using CMAKE for
C the release build since this treats unresolved symbols as an error.
#ifndef ATLAS_CMAKE
        CALL HWUEPR
#endif
        STOP
      ENDIF

      IF     (IDEX.EQ. 11) then      !electron
        IDE= 2
        AMIN=0.511D-3
      ELSEIF (IDEX.EQ.-11) then      !positron
        IDE=-2
        AMIN=0.511D-3
      ELSEIF (IDEX.EQ. 13) then      !mu+
        IDE= 2
        AMIN=0.105659
      ELSEIF (IDEX.EQ.-13) then      !mu-
        IDE=-2
        AMIN=0.105659
      ELSEIF (IDEX.EQ.  1) then      !d
        IDE= 4
        AMIN=0.05
      ELSEIF (IDEX.EQ.- 1) then      !d~
        IDE=-4
        AMIN=0.05
      ELSEIF (IDEX.EQ.  2) then      !u
        IDE= 3
        AMIN=0.02
      ELSEIF (IDEX.EQ.- 2) then      !u~
        IDE=-3
        AMIN=0.02
      ELSEIF (IDEX.EQ.  3) then      !s
        IDE= 4
        AMIN=0.3
      ELSEIF (IDEX.EQ.- 3) then      !s~
        IDE=-4
        AMIN=0.3
      ELSEIF (IDEX.EQ.  4) then      !c
        IDE= 3
        AMIN=1.3
      ELSEIF (IDEX.EQ.- 4) then      !c~
        IDE=-3
        AMIN=1.3
      ELSEIF (IDEX.EQ.  5) then      !b
        IDE= 4
        AMIN=4.5
      ELSEIF (IDEX.EQ.- 5) then      !b~
        IDE=-4
        AMIN=4.5
      ELSEIF (IDEX.EQ.  12) then     !nu_e
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 12) then     !nu_e~
        IDE=-1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.  14) then     !nu_mu
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 14) then     !nu_mu~
        IDE=-1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.  16) then     !nu_tau
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 16) then     !nu_tau~
        IDE=-1
        AMIN=0.1D-3

      ELSE
        WRITE(*,*) 'INITWK: WRONG IDEX',IDEX
        STOP
      ENDIF

C ----------------------------------------------------------------------
C
C     INITIALISATION OF COUPLING CONSTANTS AND FERMION-GAMMA / Z0 VERTEX
C
C     called by : KORALZ
C ----------------------------------------------------------------------
      ITCE=IDE/IABS(IDE)
      JTCE=(1-ITCE)/2
      ITCF=IDF/IABS(IDF)
      JTCF=(1-ITCF)/2
      CALL T_GIVIZO( IDE, 1,AIZOR,QE,KDUMM)
      CALL T_GIVIZO( IDE,-1,AIZOL,QE,KDUMM)
      XUPGI(1)=QE
      XUPGI(2)=QE
      T3E    = AIZOL+AIZOR
      XUPZI(1)=(AIZOR-QE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      XUPZI(2)=(AIZOL-QE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      CALL T_GIVIZO( IDF, 1,AIZOR,QF,KOLOR)
      CALL T_GIVIZO( IDF,-1,AIZOL,QF,KOLOR)
      XUPGF(1)=QF
      XUPGF(2)=QF
      T3F    =  AIZOL+AIZOR
      XUPZF(1)=(AIZOR-QF*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      XUPZF(2)=(AIZOL-QF*SWSQ)/SQRT(SWSQ*(1-SWSQ))
C
      NDIAG0=2
      NDIAGA=11
      KEYA  = 1
      KEYZ  = 1
C
C
      RETURN
      END
c
      SUBROUTINE INITWK_ZP(IDEX,IDFX,SVAR)
! initialization routine coupling masses etc.
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON / T_BEAMPM / ENE ,AMIN,AMFIN,IDE,IDF
      REAL*8              ENE ,AMIN,AMFIN
      COMMON / T_GAUSPM /SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI   ,XUPZI   ,XUPGF   ,XUPZF
     &                  ,NDIAG0,NDIAGA,KEYA,KEYZ
     &                  ,ITCE,JTCE,ITCF,JTCF,KOLOR
c >>--GA-->>
      COMMON / T_GAUSPM_ZP / xupzpi, xupzpf
c <<--GA--<<
      REAL*8             SS,POLN,T3E,QE,T3F,QF
     &                  ,XUPGI(2),XUPZI(2),XUPGF(2),XUPZF(2)
c >>--GA-->>
      real*8 xupzpi(2), xupzpf(2)
c <<--GA--<<
      COMMON / T_GSWPRM /SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
      REAL*8             SWSQ,AMW,AMZ,AMH,AMTOP,GAMMZ
C     SWSQ        = sin2 (theta Weinberg)
C     AMW,AMZ     = W & Z boson masses respectively
C     AMH         = the Higgs mass
C     AMTOP       = the top mass
C     GAMMZ       = Z0 width
C
c >>--GA-->> add Z' mass and width with default values, and PYTHIA common
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      common /zprm/ amzp, gammzp
      real*8 paru, parj
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      real*8 amzp, gammzp
      data amzp/1000./, gammzp/10./
c <<--GA--<<
c
C      write(*,*)
C     +'Tauola_i INITWK_ZP INFO Taking gamma*/Z0/Z''0 interference into account'
c
      ENE=SQRT(SVAR)/2
      AMIN=0.511D-3
      SWSQ=0.23147
      AMZ=91.1882
      GAMMZ=2.4952
c >>--GA-->> obtain mass and width from PYTHIA common
      amzp=pmas(32,1)
      gammzp=pmas(32,2)
c
C      write(*,*)
C     +'INITWK_ZP INFO AMZP, GAMMZP = ',AMZP,GAMMZP
c
c <<--GA--<<
      IF     (IDFX.EQ. 15) then       
        IDF=2  ! denotes tau +2 tau-
        AMFIN=1.77703 !this mass is irrelevant if small, used in ME only
      ELSEIF (IDFX.EQ.-15) then
        IDF=-2  ! denotes tau -2 tau-
        AMFIN=1.77703 !this mass is irrelevant if small, used in ME only
      ELSE
c
c  V.S. and R.Y.:  June 17, 2008, just a comment:  notice that (on ATLAS) 
c  there are no (shared or not) libraries that contain "HWUEPR" anymore!
c  Also, executing "STOP" does not seem to be a good idea in production!
c  Actually, we successfully used an instance of a crash in this piece of 
c  code to diagnose a serious problem when both PYTHIA AND TAUOLA were 
c  asked to decay tau leptons! We protected the code against such scenario,
c  however, in principle, one may still be able to end up right here, where
c  ATHENA would "crash" because of FORTRAN's "STOP"! We leave modifying 
c  THIS piece of code for future work. 
c
        WRITE(*,*) 'INITWK_ZP: WRONG IDFX',IDFX
                
C Disable the call to the non-existent HWUEPR subroutine when using CMAKE for
C the release build since this treats unresolved symbols as an error.
#ifndef ATLAS_CMAKE
        CALL HWUEPR
#endif
        STOP
      ENDIF

      IF     (IDEX.EQ. 11) then      !electron
        IDE= 2
        AMIN=0.511D-3
      ELSEIF (IDEX.EQ.-11) then      !positron
        IDE=-2
        AMIN=0.511D-3
      ELSEIF (IDEX.EQ. 13) then      !mu+
        IDE= 2
        AMIN=0.105659
      ELSEIF (IDEX.EQ.-13) then      !mu-
        IDE=-2
        AMIN=0.105659
      ELSEIF (IDEX.EQ.  1) then      !d
        IDE= 4
        AMIN=0.05
      ELSEIF (IDEX.EQ.- 1) then      !d~
        IDE=-4
        AMIN=0.05
      ELSEIF (IDEX.EQ.  2) then      !u
        IDE= 3
        AMIN=0.02
      ELSEIF (IDEX.EQ.- 2) then      !u~
        IDE=-3
        AMIN=0.02
      ELSEIF (IDEX.EQ.  3) then      !s
        IDE= 4
        AMIN=0.3
      ELSEIF (IDEX.EQ.- 3) then      !s~
        IDE=-4
        AMIN=0.3
      ELSEIF (IDEX.EQ.  4) then      !c
        IDE= 3
        AMIN=1.3
      ELSEIF (IDEX.EQ.- 4) then      !c~
        IDE=-3
        AMIN=1.3
      ELSEIF (IDEX.EQ.  5) then      !b
        IDE= 4
        AMIN=4.5
      ELSEIF (IDEX.EQ.- 5) then      !b~
        IDE=-4
        AMIN=4.5
      ELSEIF (IDEX.EQ.  12) then     !nu_e
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 12) then     !nu_e~
        IDE=-1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.  14) then     !nu_mu
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 14) then     !nu_mu~
        IDE=-1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.  16) then     !nu_tau
        IDE= 1
        AMIN=0.1D-3
      ELSEIF (IDEX.EQ.- 16) then     !nu_tau~
        IDE=-1
        AMIN=0.1D-3

      ELSE
        WRITE(*,*) 'INITWK_ZP: WRONG IDEX',IDEX
        STOP
      ENDIF

C ----------------------------------------------------------------------
C
C     INITIALISATION OF COUPLING CONSTANTS AND FERMION-GAMMA / Z0 VERTEX
C
C     called by : KORALZ
C ----------------------------------------------------------------------
      ITCE=IDE/IABS(IDE)
      JTCE=(1-ITCE)/2
      ITCF=IDF/IABS(IDF)
      JTCF=(1-ITCF)/2
      CALL T_GIVIZO( IDE, 1,AIZOR,QE,KDUMM)
      CALL T_GIVIZO( IDE,-1,AIZOL,QE,KDUMM)
      XUPGI(1)=QE
      XUPGI(2)=QE
      T3E    = AIZOL+AIZOR
      XUPZI(1)=(AIZOR-QE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      XUPZI(2)=(AIZOL-QE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
c
c >>--GA-->>
c
c  calculate zprime parameters v_f=xupzpi(1) and a_f=xupzpi(2)
c      call zppar(aizor,qe,kdumm,xzp)
c      xupzpi(1)=xzp
c      call zppar(aizol,qe,kdumm,xzp)
c      xupzpi(2)=xzp
c.. use PYTHIA common block values
      if(idex.eq.11) then
         xupzpi(1)=paru(125)-paru(126)
         xupzpi(2)=paru(125)+paru(126)
      else if(idex.eq.13)then
         xupzpi(1)=parj(184)-parj(185)
         xupzpi(2)=parj(184)+parj(185)
      else if (idex.eq.-11)then
         xupzpi(2)=-(paru(125)-paru(126))
         xupzpi(1)=-(paru(125)+paru(126))
      else if(idex.eq.-13)then
         xupzpi(2)=-(parj(184)-parj(185))
         xupzpi(1)=-(parj(184)+parj(185))
      elseif(idex.eq.1)then
         xupzpi(1)=paru(121)-paru(122)
         xupzpi(2)=paru(121)+paru(122)
      else if(idex.eq.3)then
         xupzpi(1)=parj(180)-parj(181)
         xupzpi(2)=parj(180)+parj(181)
      else if(idex.eq.5)then
         xupzpi(1)=parj(188)-parj(189)
         xupzpi(2)=parj(188)+parj(189)
      else if(idex.eq.-1)then
         xupzpi(2)=-(paru(121)-paru(122))
         xupzpi(1)=-(paru(121)+paru(122))
      else if(idex.eq.-3)then
         xupzpi(2)=-(parj(180)-parj(181))
         xupzpi(1)=-(parj(180)+parj(181))
      else if(idex.eq.-5)then
         xupzpi(2)=-(parj(188)-parj(189))
         xupzpi(1)=-(parj(188)+parj(189))
      else if(idex.eq.2)then
         xupzpi(1)=paru(123)-paru(124)
         xupzpi(2)=paru(123)+paru(124)
      else if(idex.eq.4)then
         xupzpi(1)=parj(182)-parj(183)
         xupzpi(2)=parj(182)+parj(183)
c      else if(idex.eq.6)then
c         xupzpi(1)=parj(190)-parj(191)
c         xupzpi(2)=parj(190)+parj(191)
      else if(idex.eq.-2)then
         xupzpi(2)=-(paru(123)-paru(124))
         xupzpi(1)=-(paru(123)+paru(124))
      else if(idex.eq.-4)then
         xupzpi(2)=-(parj(182)-parj(183))
         xupzpi(1)=-(parj(182)+parj(183))
c      else if(idex.eq.-6)then
c         xupzpi(2)=-(parj(190)-parj(191))
c         xupzpi(1)=-(parj(190)+parj(191))
      elseif(abs(idex).eq.12)then
         xupzpi(1)=paru(127)-paru(128)
         xupzpi(2)=paru(127)+paru(128)
      elseif(abs(idex).eq.14)then
         xupzpi(1)=parj(186)-parj(187)
         xupzpi(2)=parj(186)+parj(187)
      elseif(abs(idex).eq.16)then
         xupzpi(1)=parj(194)-parj(195)
         xupzpi(2)=parj(194)+parj(195)
      elseif(abs(idex).eq.-12)then
         xupzpi(2)=-(paru(127)-paru(128))
         xupzpi(1)=-(paru(127)+paru(128))
      elseif(abs(idex).eq.-14)then
         xupzpi(2)=-(parj(186)-parj(187))
         xupzpi(1)=-(parj(186)+parj(187))
      elseif(abs(idex).eq.-16)then
         xupzpi(2)=-(parj(194)-parj(195))
         xupzpi(1)=-(parj(194)+parj(195))
      endif
      xupzpi(1)=xupzpi(1)/(4.*SQRT(SWSQ*(1-SWSQ)))
      xupzpi(2)=xupzpi(2)/(4.*SQRT(SWSQ*(1-SWSQ)))
c
c <<--GA--<<
c
      CALL T_GIVIZO( IDF, 1,AIZOR,QF,KOLOR)
      CALL T_GIVIZO( IDF,-1,AIZOL,QF,KOLOR)
      XUPGF(1)=QF
      XUPGF(2)=QF
      T3F    =  AIZOL+AIZOR
      XUPZF(1)=(AIZOR-QF*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      XUPZF(2)=(AIZOL-QF*SWSQ)/SQRT(SWSQ*(1-SWSQ))
c
c >>--GA-->>
c
c  calculate zprime parameters v_f=xupzpf(1) and a_f=xupzpf(2)
c      call zppar(aizor,qf,kolor,xzp)
c      xupzpf(1)=xzp
c      call zppar(aizol,qf,kolor,xzp)
c      xupzpf(2)=xup
      if(idfx.eq.15)then
         xupzpf(1)=(parj(192)-parj(193))/(4.*SQRT(SWSQ*(1-SWSQ)))
         xupzpf(2)=(parj(192)+parj(193))/(4.*SQRT(SWSQ*(1-SWSQ)))
      else if(idfx.eq.-15)then
         xupzpf(2)= -((parj(192)-parj(193))/(4.*SQRT(SWSQ*(1-SWSQ))))
         xupzpf(1)= -((parj(192)+parj(193))/(4.*SQRT(SWSQ*(1-SWSQ))))
      end if
c      write(6,*)' '
c      write(6,*)idex,xupzi(1),xupzi(2),xupzpi(1),xupzpi(2)
c      write(6,*)idfx,xupzf(1),xupzf(2),xupzpf(1),xupzpf(2)
c
c <<--GA--<<
C
      NDIAG0=2
      NDIAGA=11
      KEYA  = 1
      KEYZ  = 1
C
C
      RETURN
      END

      SUBROUTINE T_GIVIZO(IDFERM,IHELIC,SIZO3,CHARGE,KOLOR)
C ----------------------------------------------------------------------
C PROVIDES ELECTRIC CHARGE AND WEAK IZOSPIN OF A FAMILY FERMION
C IDFERM=1,2,3,4 DENOTES NEUTRINO, LEPTON, UP AND DOWN QUARK
C NEGATIVE IDFERM=-1,-2,-3,-4, DENOTES ANTIPARTICLE
C IHELIC=+1,-1 DENOTES RIGHT AND LEFT HANDEDNES ( CHIRALITY)
C SIZO3 IS THIRD PROJECTION OF WEAK IZOSPIN (PLUS MINUS HALF)
C AND CHARGE IS ELECTRIC CHARGE IN UNITS OF ELECTRON CHARGE
C KOLOR IS A QCD COLOUR, 1 FOR LEPTON, 3 FOR QUARKS
C
C     called by : EVENTE, EVENTM, FUNTIH, .....
C ----------------------------------------------------------------------
      IMPLICIT REAL*8(A-H,O-Z)
C
      IF(IDFERM.EQ.0.OR.IABS(IDFERM).GT.4) GOTO 901
      IF(IABS(IHELIC).NE.1)                GOTO 901
      IH  =IHELIC
      IDTYPE =IABS(IDFERM)
      IC  =IDFERM/IDTYPE
      LEPQUA=INT(IDTYPE*0.4999999D0)
      IUPDOW=IDTYPE-2*LEPQUA-1
      CHARGE  =(-IUPDOW+2D0/3D0*LEPQUA)*IC
      SIZO3   =0.25D0*(IC-IH)*(1-2*IUPDOW)
      KOLOR=1+2*LEPQUA
C** NOTE THAT CONVENTIONALY Z0 COUPLING IS
C** XOUPZ=(SIZO3-CHARGE*SWSQ)/SQRT(SWSQ*(1-SWSQ))
      RETURN
 901  PRINT *,' STOP IN GIVIZO: WRONG PARAMS.'
      STOP
      END
c      SUBROUTINE PHYFIX(NSTOP,NSTART)
c      INTEGER N, NPAD, K 
c      DOUBLE PRECISION P, V		
c      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5) 
c      SAVE /PYJETS/ 
C NSTOP NSTART : when PHYTIA history ends and event starts.
c      NSTOP=0
c      NSTART=1
c      DO I=1, N
c       IF(K(I,1).NE.21) THEN
c           NSTOP = I-1
c           NSTART= I
c           GOTO 500
c       ENDIF
c      ENDDO
c 500  CONTINUE
c      END
      SUBROUTINE FILHEP(N,IST,ID,JMO1,JMO2,JDA1,JDA2,P4,PINV,PHFLAG)
C ----------------------------------------------------------------------
C this subroutine fills one entry into the HEPEVT common
C and updates the information for affected mother entries
C
C written by Martin W. Gruenewald (91/01/28)
C
C     called by : ZTOHEP,BTOHEP,DWLUxy
C ----------------------------------------------------------------------
C
C this is the hepevt class in old style. No d_h_ class pre-name
C this is the hepevt class in old style. No d_h_ class pre-name
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      DOUBLE PRECISION phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)    ! Photos flag
* ----------------------------------------------------------------------
c      SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/


      LOGICAL PHFLAG
C
      REAL*4  P4(4),PINV

      DOUBLE PRECISION XM2,XP2
C
C check address mode
      IF (N.EQ.0) THEN
C
C append mode
        IHEP=NHEP+1
      ELSE IF (N.GT.0) THEN
C
C absolute position
        IHEP=N
      ELSE
C
C relative position
        IHEP=NHEP+N
      END IF
C
C check on IHEP
      IF ((IHEP.LE.0).OR.(IHEP.GT.NMXHEP)) RETURN

C
C add entry
      NHEP=IHEP
      ISTHEP(IHEP)=IST
      IDHEP(IHEP)=ID
      JMOHEP(1,IHEP)=JMO1
      IF(JMO1.LT.0)JMOHEP(1,IHEP)=JMOHEP(1,IHEP)+IHEP
      JMOHEP(2,IHEP)=JMO2
      IF(JMO2.LT.0)JMOHEP(2,IHEP)=JMOHEP(2,IHEP)+IHEP
      JDAHEP(1,IHEP)=JDA1
      JDAHEP(2,IHEP)=JDA2

C
      DO I=1,4
        PHEP(I,IHEP)=P4(I)
C
C KORAL-B and KORAL-Z do not provide vertex and/or lifetime informations
C        VHEP(I,IHEP)=0.0
C(BPK) COPY THE VERTEX POSITION
        VHEP(I,IHEP)=VHEP(I,JMOHEP(1,IHEP))

      END DO
      PHEP(5,IHEP)=PINV


C(BPK) RE CALCULATE THE ENERGY DUE TO NUMERICAL PRECISION
      XM2=phep(5,IHEP)*phep(5,IHEP)
      XP2=PHEP(1,IHEP)*PHEP(1,IHEP)+PHEP(2,IHEP)*PHEP(2,IHEP)
      XP2=XP2+PHEP(3,IHEP)*PHEP(3,IHEP)
      PHEP(4,IHEP)=SQRT(XM2+XP2)

C FLAG FOR PHOTOS...
      QEDRAD(IHEP)=PHFLAG
C
C update process:
      DO IP=JMOHEP(1,IHEP),JMOHEP(2,IHEP)
        IF(IP.GT.0)THEN
C
C if there is a daughter at IHEP, mother entry at IP has decayed
          IF(ISTHEP(IP).EQ.1)ISTHEP(IP)=2
C
C and daughter pointers of mother entry must be updated
          IF(JDAHEP(1,IP).EQ.0)THEN
            JDAHEP(1,IP)=IHEP
            JDAHEP(2,IP)=IHEP
          ELSE
            JDAHEP(2,IP)=MAX(IHEP,JDAHEP(2,IP))
          END IF
        END IF
      END DO
C
      RETURN
      END


      FUNCTION IHEPDIM(DUM) 
C this is the hepevt class in old style. No d_h_ class pre-name
C this is the hepevt class in old style. No d_h_ class pre-name
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)    ! Photos flag
* ----------------------------------------------------------------------
c      SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/


      IHEPDIM=NHEP
      END
      FUNCTION ZPROP2(S)
      IMPLICIT REAL*8(A-H,O-Z)
      COMPLEX*16 CPRZ0,CPRZ0M
      AMZ=91.1882
      GAMMZ=2.49
      CPRZ0=DCMPLX((S-AMZ**2),S/AMZ*GAMMZ)
      CPRZ0M=1/CPRZ0
      ZPROP2=(ABS(CPRZ0M))**2
      END

      SUBROUTINE TAUPI0(MODE,JAK)
C no initialization required. Must be called once after every:
C   1)    CALL DEKAY(1+10,...)
C   2)    CALL DEKAY(2+10,...)
C   3)    CALL DEXAY(1,...)
C   4)    CALL DEXAY(2,...)
C subroutine to decay originating from TAUOLA's taus: 
C 1) etas (with CALL TAUETA(JAK))
C 2) later pi0's from taus.
C 3) extensions to other applications possible. 
C this routine belongs to >tauola universal interface<, but uses 
C routines from >tauola< utilities as well.  25.08.2005      
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)    ! Photos flag
* ----------------------------------------------------------------------
c      SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/

      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST

C position of taus, must be defined by host program:
      COMMON /TAUPOS/ NP1,NP2
c
      REAL  PHOT1(4),PHOT2(4)
      REAL*8  R,X(4),Y(4),PI0(4)
      DATA JEZELI /0/
      SAVE JEZELI
      IF (MODE.EQ.-1) JEZELI=JAK
C(BPK) BUG FIX IF MODE EQ -1 THEN RETURN (IT'S JUST INIT)
      IF (MODE.EQ.-1) RETURN


      IF (JEZELI.EQ.0) RETURN
      CALL TAUETA(JAK)
C position of decaying particle:
C(BPK) BUG FIX KTO=JAK
      KTO=JAK
      IF((KTO.EQ. 1).OR.(KTO.EQ.11)) THEN
        NPS=NP1
      ELSE
        NPS=NP2
      ENDIF
      nhepM=nhep                ! to avoid infinite loop
      DO K=JDAHEP(1,NPS),nhepM  ! we search for pi0's from tau till eor.

         xma=PHEP(4,K)*PHEP(4,K)-PHEP(1,K)*PHEP(1,K)
         xma=xma-PHEP(2,K)*PHEP(2,K)-PHEP(3,K)*PHEP(3,K)
         xmaS=sqrt(xma)

C(BPK) COPY THE VERTEX POSITION
         DO J=1,4
            VHEP(J,K)=VHEP(J,JMOHEP(1,K))
         ENDDO

c         PRINT *,'TDEC',NPS,K
C         PRINT *,'VHEP',NPS,K,JMOHEP(1,K),(VHEP(I,K),I=1,4)
      

       IF (IDHEP(K).EQ.111.AND.JDAHEP(1,K).LE.K) THEN ! IF we found pi0

        DO L=1,4
          PI0(L)= phep(L,K)
        ENDDO
! random 3 vector on the sphere, masless
        R=SQRT(PI0(4)**2-PI0(3)**2-PI0(2)**2-PI0(1)**2)/2D0
        CALL SPHERD(R,X)
        X(4)=R
        Y(4)=R
        
        Y(1)=-X(1)
        Y(2)=-X(2)
        Y(3)=-X(3)

! boost to lab and to real*4
        CALL bostdq(-1,PI0,X,X)
        CALL bostdq(-1,PI0,Y,Y)
        DO L=1,4
         PHOT1(L)=X(L)
         PHOT2(L)=Y(L)
        ENDDO
C to hepevt
        CALL FILHEP(0,1,22,K,K,0,0,PHOT1,0.0,.TRUE.)
        CALL FILHEP(0,1,22,K,K,0,0,PHOT2,0.0,.TRUE.)
       ENDIF
      ENDDO
C
      END
      SUBROUTINE TAUETA(JAK)
C subroutine to decay etas's from taus. 
C this routine belongs to tauola universal interface, but uses 
C routines from tauola utilities. Just flat phase space, but 4 channels.
C it is called at the beginning of SUBR. TAUPI0(JAK)
C and as far as hepevt search it is basically the same as TAUPI0.  25.08.2005    
      INTEGER NMXHEP
      PARAMETER (NMXHEP=10000)
      REAL*8  phep,  vhep ! to be real*4/ *8  depending on host
      INTEGER nevhep,nhep,isthep,idhep,jmohep,
     $        jdahep
      COMMON /hepevt/
     $      nevhep,               ! serial number
     $      nhep,                 ! number of particles
     $      isthep(nmxhep),   ! status code
     $      idhep(nmxhep),    ! particle ident KF
     $      jmohep(2,nmxhep), ! parent particles
     $      jdahep(2,nmxhep), ! childreen particles
     $      phep(5,nmxhep),   ! four-momentum, mass [GeV]
     $      vhep(4,nmxhep)    ! vertex [mm]
* ----------------------------------------------------------------------
      LOGICAL qedrad
      COMMON /phoqed/ 
     $     qedrad(nmxhep)    ! Photos flag
* ----------------------------------------------------------------------
c      SAVE hepevt,phoqed
      SAVE /hepevt/,/phoqed/

      COMMON / PARMAS / AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST
*
      REAL*4            AMTAU,AMNUTA,AMEL,AMNUE,AMMU,AMNUMU
     *                 ,AMPIZ,AMPI,AMRO,GAMRO,AMA1,GAMA1
     *                 ,AMK,AMKZ,AMKST,GAMKST

C position of taus, must be defined by host program:
      COMMON /TAUPOS/ NP1,NP2
c
      REAL  RRR(1),BRSUM(3), RR(2)
      REAL  PHOT1(4),PHOT2(4),PHOT3(4)
      REAL*8    X(4),    Y(4),    Z(4)
      REAL                                YM1,YM2,YM3
cc      REAL*8  R,RU,PETA(4),XM1,XM2,XM3,XM,XLAM
      REAL*8  R,RU,PETA(4),XM1,XM2,XM3,XLAM
      REAL*8  AM2 ! gfortran requires this
      REAL*8 a,b,c
      XLAM(a,b,c)=SQRT(ABS((a-b-c)**2-4.0*b*c))
C position of decaying particle:
C(BPK) BUG FIX KTO=JAK
      KTO=JAK
      IF((KTO.EQ. 1).OR.(KTO.EQ.11)) THEN
        NPS=NP1
      ELSE
        NPS=NP2
      ENDIF
      nhepM=nhep    ! to avoid infinite loop              


      DO K=JDAHEP(1,NPS),nhepM  ! we search for etas's from tau till eor.
       IF (IDHEP(K).EQ.221.AND.JDAHEP(1,K).LE.K) THEN ! IF we found eta

        DO L=1,4
          PETA(L)= phep(L,K)  ! eta 4 momentum
        ENDDO
C       eta cumulated branching ratios:
        BRSUM(1)=0.389  ! gamma gamma
        BRSUM(2)=BRSUM(1)+0.319  ! 3 pi0
        BRSUM(3)=BRSUM(2)+0.237  ! CLHEP::pi+ CLHEP::pi- pi0 rest is thus CLHEP::pi+CLHEP::pi-gamma
        CALL ACRMAR(RRR,1) 
        
        IF (RRR(1).LT.BRSUM(1)) THEN ! gamma gamma channel exactly like pi0
! random 3 vector on the sphere, masless   
         R=SQRT(PETA(4)**2-PETA(3)**2-PETA(2)**2-PETA(1)**2)/2D0
         CALL SPHERD(R,X) 
         X(4)=R
         Y(4)=R
        
         Y(1)=-X(1)
         Y(2)=-X(2)
         Y(3)=-X(3)
! boost to lab and to real*4
         CALL bostdq(-1,PETA,X,X)  
         CALL bostdq(-1,PETA,Y,Y)
         DO L=1,4
          PHOT1(L)=X(L)
          PHOT2(L)=Y(L)
         ENDDO
C to hepev

         CALL FILHEP(0,1,22,K,K,0,0,PHOT1,0.0,.TRUE.)
         CALL FILHEP(0,1,22,K,K,0,0,PHOT2,0.0,.TRUE.)
        ELSE ! 3 body channels
         IF(RRR(1).LT.BRSUM(2)) THEN  ! 3 pi0
          ID1= 111
          ID2= 111
          ID3= 111
          XM1=AMPIZ ! masses
          XM2=AMPIZ
          XM3=AMPIZ
         ELSEIF(RRR(1).LT.BRSUM(3)) THEN ! CLHEP::pi+ CLHEP::pi- pi0
          ID1= 211
          ID2=-211
          ID3= 111
          XM1=AMPI ! masses
          XM2=AMPI
          XM3=AMPIZ
         ELSE                            ! CLHEP::pi+ CLHEP::pi- gamma 
          ID1= 211
          ID2=-211
          ID3=  22
          XM1=AMPI ! masses
          XM2=AMPI
          XM3=0.0
         ENDIF
 7       CONTINUE  ! we generate mass of the first pair:
          CALL ACRMAR(RR,2)
          R=SQRT(PETA(4)**2-PETA(3)**2-PETA(2)**2-PETA(1)**2)
          AMIN=XM1+XM2
          AMAX=R-XM3
          AM2=SQRT(AMIN**2+RR(1)*(AMAX**2-AMIN**2))
C         weight for flat phase space
          WT=XLAM(R**2,AM2**2,XM3**2)*XLAM(AM2**2,XM1**2,XM2**2)
     &           /R**2                    /AM2**2
         IF (RR(2).GT.WT) GOTO 7

         RU=XLAM(AM2**2,XM1**2,XM2**2)/AM2/2  ! momenta of the
                                              ! first two products
                                              ! in the rest frame of that pair
         CALL SPHERD(RU,X)
         X(4)=SQRT(RU**2+XM1**2)
         Y(4)=SQRT(RU**2+XM2**2)
        
         Y(1)=-X(1)
         Y(2)=-X(2)
         Y(3)=-X(3)
C generate momentum of that pair in rest frame of eta:
         RU=XLAM(R**2,AM2**2,XM3**2)/R/2
         CALL SPHERD(RU,Z)
         Z(4)=SQRT(RU**2+AM2**2)
C and boost first two decay products to rest frame of eta.
         CALL bostdq(-1,Z,X,X)
         CALL bostdq(-1,Z,Y,Y)
C redefine Z(4) to 4-momentum of the last decay product: 
         Z(1)=-Z(1)
         Z(2)=-Z(2)
         Z(3)=-Z(3)
         Z(4)=SQRT(RU**2+XM3**2)
C boost all to lab and move to real*4; also masses
         CALL bostdq(-1,PETA,X,X)
         CALL bostdq(-1,PETA,Y,Y)
         CALL bostdq(-1,PETA,Z,Z)
         DO L=1,4
          PHOT1(L)=X(L)
          PHOT2(L)=Y(L)
          PHOT3(L)=Z(L)
         ENDDO
         YM1=XM1
         YM2=XM2
         YM3=XM3
C to hepev

         CALL FILHEP(0,1,ID1,K,K,0,0,PHOT1,YM1,.TRUE.)
         CALL FILHEP(0,1,ID2,K,K,0,0,PHOT2,YM2,.TRUE.)
         CALL FILHEP(0,1,ID3,K,K,0,0,PHOT3,YM3,.TRUE.)
        ENDIF

       ENDIF
      ENDDO
C
      END
c
      SUBROUTINE TAUOLA_SETGEN(IGEN) 
c
c  Introduced by V.S. on Jan.17, 2009 to facilitate access to PYTHIA common blocks
c
      implicit NONE
c
      integer IGEN
c
      common / LOGPH / IS_Z0P, IS_PYTHIA
      logical IS_Z0P
      logical IS_PYTHIA
c
      IS_PYTHIA = .false.
c
      if ( IGEN .eq. 1 ) IS_PYTHIA = .true. 
c
      return
      end

C(ET) Save tau decay details into common block
C     NP - location of tau in HEPEVT
C     JAK - TAUOLA decay mode as decayed
C     POLV - Polarization vector used in decay
      SUBROUTINE TDETSV(NP, JAK, POLV)
      IMPLICIT NONE
      INTEGER NP
      INTEGER JAK
      REAL*4 POLV(3)
      
#include "Tauola_i/taudet.inc"       

      IF (NTAUSAV.LT.NMXTAU) THEN
         NTAUSAV = NTAUSAV+1
         ITNP(NTAUSAV) = NP
         ITJAK(NTAUSAV) = JAK   ! Actual decay mode used (1 -> JAKP)
         FTPOLX(NTAUSAV) = POLV(1)
         FTPOLY(NTAUSAV) = POLV(2)
         FTPOLZ(NTAUSAV) = POLV(3)

c         PRINT *, 'TDETSV: saved tau ', NTAUSAV,' with ID ', NP

      ELSE
         PRINT *, 
     +'TAUOLA_HEPEVT-WARNING-: Too many taus decayed! Polarization info is stored only for ',
     +NMXTAU,' taus.'
      ENDIF

      END

