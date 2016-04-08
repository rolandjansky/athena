      SUBROUTINE USECOMPHEP
c     routine for Les Houches interface to external pythiA routines
c     adapted from base pythia code. 
c     Ian Hinchliffe (May 2002)
c 
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
C      from 
*************************************************************************
*                          UPEVNT routine                               *
*          for CompHEP 4.4.3  -  PYTHIA 6.2 interface                   *
*************************************************************************
*  25.11.2001       Ilyin V.A.                                          *
*  27.03.2006  Modified for version 4.4.3 by Vikas Bansal               *
*************************************************************************
 
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"
*...CompHEP-PYTHIA interface
      COMMON/SUBPROC/NNUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .            NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      COMMON/BEAMS/BENERGY(2),KFB(2),BMASS(2),BEAM(2)
      CHARACTER BEAM*16
      character chain*60, K1*1, K2*1, XN*2,COLON*1     
      COMMON/CCHAIN/ICC(100,2,20)
      COMMON/ISMEAR/ISMEAR
      COMMON /INPARM/ IS,NEVPY,PROCESS,EVFILE
      CHARACTER EVFILE*500,PROCESS*80,LINE*500
      REAL*8 PP(20,4)
      COMMON /QCDPAR/ QCDLF(6),NNL
      COMMON/RANDM0/IDUM

* A. Belyaev, tau-leptons treatment *
      INTEGER ITAU(10)      
      common/UTAU/KFUSER
      SAVE /UTAU/

*...read current event
      PP(1,1)=0d0
      PP(1,2)=0d0
      PP(2,1)=0d0
      PP(2,2)=0d0
      READ(IS,FMT='(A500)',END=1000,ERR=1000) LINE  
      DO M = 1,6
         READ(LINE(M:M),FMT='(A1)' ) COLON
         IF ( COLON .eq. ':' ) K = M - 1
      ENDDO
      READ(LINE(1:K),FMT='(I6)') NPRC 
      Num=3*(NNUP(NPRC)-2)+2
      CALL ITOCHR(XN,NUM)
      LINE = LINE((K+1):500)
      READ(LINE,FMT='('//XN//'(1x,G17.10),1x,G10.3,1x,A)'
     .,END=1000,
     .ERR=1000) PP(1,3),PP(2,3),((PP(N,J),J=1,3),N=3,NNUP(NPRC)),
     .QUP,chain

      DO 101 M = 1,60
      READ(chain(M:M),FMT='(A1)' ) COLON
      IF ( COLON .eq. ':' ) K = M - 1
101   CONTINUE
      chain = chain(1:K)

      pi=dacos(-1d0)

*...the number of particle entries in the current event
      NUP =  NNUP(NPRC)
*...the identity of the current subprocess
      IDPRUP = LPRUP(NPRC)
*...the event weight
      XWGTUP = 1.d0
*...scale Q of the event (factorization scale)
      SCALUP = QUP
*...the QED coupling used
      AQEDUP = -1    ! not QED process
*...the QCD coupling used for this event (ME) 
*VB Now calculating as per LHAPDF
      AQCDUP = ALPHASPDF(SCALUP,QCDLF(5),NNL)  ! the same as factorization scale
*...particle identity code
      NTAU=0
      DO i=1,NUP
         IDUP(i) = KFP(nprc,i)
         IF(abs(IDUP(i)).eq.15.or.abs(IDUP(i)).eq.16) THEN
            NTAU=NTAU+1
            ITAU(NTAU)=i
            IF(abs(IDUP(i)).eq.15.and.
     &           (abs(KFUSER).eq.24.or.abs(KFUSER).eq.37)) 
     &           KFUSER=KFUSER*SIGN(1,-IDUP(i))
            NTAUPAIR=(NTAU-1)/2+1
            IDUP(NUP+NTAUPAIR)=KFUSER
         ENDIF
      ENDDO
*...status code of particle
      ISTUP(1) = -1
      ISTUP(2) = -1
      DO i=3,NUP+NTAU
         ISTUP(i) = 1
         IF(i.gt.NUP)ISTUP(i) = 2
      ENDDO
*...position of first and last mother of particle
       MOTHUP(1,1) = 0 
       MOTHUP(2,1) = 0
       MOTHUP(1,2) = 0 
       MOTHUP(2,2) = 0 
      DO i=3,NUP
       MOTHUP(1,i) = 1 
       MOTHUP(2,i) = 2
      ENDDO
*...the particle momenta
      DO  NN=1,NUP
         PUP(1,NN)=PP(NN,1)
         PUP(2,NN)=PP(NN,2)
         PUP(3,NN)=PP(NN,3)
         PUP(4,NN)=SQRT(PP(NN,1)**2+PP(NN,2)**2+PP(NN,3)**2
     .        +XM(nprc,NN,1)**2)
         PUP(5,NN)=XM(nprc,NN,1)
      ENDDO
      DO NT=1,NTAUPAIR
         NN=NUP+NT
         PUP(1,NN)=PP(ITAU(2*NT-1) ,1)+PP(ITAU(2*NT),1)
         PUP(2,NN)=PP(ITAU(2*NT-1) ,2)+PP(ITAU(2*NT),2)
         PUP(3,NN)=(PP(ITAU(2*NT-1),3)+PP(ITAU(2*NT),3))
         PUP(4,NN)=PUP(4,ITAU(2*NT-1))+PUP(4,ITAU(2*NT))
         PUP(5,NN)=
     &        sqrt(PUP(4,NN)**2-PUP(1,NN)**2-PUP(2,NN)**2-PUP(3,NN)**2)
      ENDDO
*...invariant lifetime
      DO  i=1,NUP+NTAUPAIR
         VTIMUP(i) = 0.d0
      ENDDO
*...spin information
      DO  i=1,NUP+NTAUPAIR
         SPINUP(i) = 9
      ENDDO

*...Breit-Wigner smearing of masses for unstable particles in OUT state if yes
        CALL SMEARMASS
*...set color flows
      DO I=1,30
       IF (CHAIN(I:I+1).EQ.') ') LENGTH=I
      ENDDO
      ITAG = 500
      DO I=1,NUP
        ICOLUP(1,i) = 0
        ICOLUP(2,i) = 0
      ENDDO
      DO I=1,LENGTH,5
        K1=CHAIN(I+1:I+1)
        N1=NUMBER(K1)
        K2=CHAIN(I+3:I+3)
        N2=NUMBER(K2)
        IF (N1.le.2) THEN
          ICOLUP(1,N1) = ITAG
        ELSE
          ICOLUP(2,N1) = ITAG
        ENDIF
        IF (N2.le.2) THEN
          ICOLUP(2,N2) = ITAG
        ELSE
          ICOLUP(1,N2) = ITAG
        ENDIF
        ITAG = ITAG+1
      ENDDO

*     now we are insert intermediate resonanse(s) after 1,2 particles:************
      
      DO I1=NTAUPAIR,1,-1
         INDX=NUP+I1
         
         IDTMP  =IDUP( INDX)
         ISTTMP =ISTUP(INDX)
         ICOL1TMP=ICOLUP(1,INDX)
         ICOL2TMP=ICOLUP(2,INDX)
         P1TMP= PUP(1,INDX)
         P2TMP= PUP(2,INDX)
         P3TMP= PUP(3,INDX)
         P4TMP= PUP(4,INDX)
         P5TMP= PUP(5,INDX)
         
         DO I2=NUP+I1-1,3,-1
            IDUP(I2+1)=IDUP(I2)
            ISTUP(I2+1)   =ISTUP(I2)
            ICOLUP(1,I2+1)  =ICOLUP(1,I2)
            ICOLUP(2,I2+1)  =ICOLUP(2,I2)
            DO J1=1,5
               PUP(J1,I2+1)   =PUP(J1,I2)
            ENDDO
         ENDDO
         IDUP(3)      =IDTMP
         ISTUP(3)     =ISTTMP
         ICOLUP(1,3)  =ICOLTMP1
         ICOLUP(2,3)  =ICOLTMP2
         PUP(1,3)  =P1TMP 
         PUP(2,3)  =P2TMP 
         PUP(3,3)  =P3TMP 
         PUP(4,3)  =P4TMP 
         PUP(5,3)  =P5TMP 
      ENDDO
****************************************************************
      NUP=NUP+NTAUPAIR
      
      NTAU=0
      DO  i=3,NUP
         MOTHUP(1,i)=0
         MOTHUP(2,i)=0
         IF(abs(IDUP(I)).eq.15.or.abs(IDUP(I)).eq.16) THEN
            NTAU=NTAU+1
            NTAUPAIR=(NTAU-1)/2+1        
            MOTHUP(1,i)=3+NTAUPAIR-1
            MOTHUP(2,i)=0
         ENDIF
      ENDDO

      RETURN

*...EOF or error in reading events from CompHEP output
1000  NUP = 0

      RETURN
      END


      FUNCTION NUMBER(PARTON)
      CHARACTER PARTON*1
      IF (PARTON.EQ.'1') THEN 
      NUMBER=1
       ELSEIF(PARTON.EQ.'2') THEN        
       NUMBER=2
        ELSEIF(PARTON.EQ.'3') THEN         
         NUMBER=3
         ELSEIF(PARTON.EQ.'4') THEN          
          NUMBER=4
          ELSEIF(PARTON.EQ.'5') THEN
           NUMBER=5
           ELSEIF(PARTON.EQ.'6') THEN
            NUMBER=6
            ELSEIF(PARTON.EQ.'7') THEN
             NUMBER=7
             ELSEIF(PARTON.EQ.'8') THEN
              NUMBER=8
              ELSEIF(PARTON.EQ.'9') THEN
               NUMBER=9  
              ELSE
               NUMBER=10
      ENDIF
      END

*******************************************************
*                    FUNCTION                         *
* Conversion of integer number into character string  *
*******************************************************
*   Input:  ICHR - output string,                     *
*           JJ - integer number under the conversion; *
*   Output: ITOCHR - the character string,            *
*             declare ICHR at least as                *
*                CHARACTER*LENINT(JJ)                 *
*******************************************************
      SUBROUTINE ITOCHR(ICHR,JJ)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      CHARACTER ICHR*(*)
      CHARACTER XFMT*10,XCC

      ICHR=' '
      WRITE(XCC,FMT='(I1)') LENINT(JJ)
      XFMT='(I'//XCC//')'
      WRITE(ICHR,FMT=XFMT,err=1) JJ

      goto 9000
1     write(*,*) 'Error in ITOCHR.'
      read(*,*)

9000  RETURN
      END

********************************************
*           FUNCTION                       *
*     Length of the integer number         *
********************************************
      INTEGER FUNCTION LENINT(JJ)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      KK=1
10    CONTINUE
      IF (JJ .LT. 10**KK) THEN
         LLL=KK
         GOTO 20
      ENDIF
      KK=KK+1 
      GOTO 10

20    LENINT=LLL


      RETURN
      END

*************************************************************************     
*        Routine to smear the resonance mass according  to Breit-Wigner *            
*                  CompHEP 4.4.3 -  PYTHIA 6.2 interface                *         
*************************************************************************      
*  15.12.99         A. Belyaev, V.Ilyin                                 *      
*  06.11.2000       some corrections (XM -> PMAS), V.Ilyin              *           
*  25.11.2001       update to Pythia 6.2 by V.Ilyin                     *         
*  28.03.2002       corrected ABS(IDUP(I))-->pycomp(IDUP(I)) A.Belyaev  *         
*  20.12.2005       cosmetic changes  by R.Mehdiyev                     *     
*  27.03.2006       compatability change by V. Bansal                   *
*************************************************************************
      SUBROUTINE SMEARMASS

C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
#include "GeneratorFortranCommon/hepeup.inc"
#include "GeneratorFortranCommon/heprup.inc"
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
      INTEGER PYCOMP
*...CompHEP-PYTHIA interface
      COMMON/SUBPROC/NNUP(100),KFP(100,20),SIGMAERR(100),IDPR(100),
     .            NCUT(100),NEV(100),XM(100,20,3),SIGMA(100),NPROC
      SAVE /SUBPROC/
      
      REAL*8 PX,PX1(20,5),PX2(20,5),PX3(20,5),PXX(20,5)

*...some values in lab frame                                                                      
      PLAB=PUP(3,1)+PUP(3,2)
      ELAB=PUP(4,1)+PUP(4,2)
      SHAT=(ELAB-PLAB)*(ELAB+PLAB)

*...loop over the particles in final state                                                        
      DO I=3,NUP

*...if unstable particle then smeare its mass                                                     

      IF(PMAS(pycomp(IDUP(I)),2).gt.0.) THEN

*...transfer out-momenta to REAL*8                                                                
      DO 100 j=1,NUP
      DO 100 k=1,5
 100          PX1(j,k)=PUP(k,j)

*...invariant mass of the cluster recoiled the Ith particle                                       
       IF(NUP.EQ.4) THEN              ! 2->2 case                                                 
         IF(I.eq.3) PM2=PX1(4,5)
         IF(I.eq.4) PM2=PX1(3,5)
       ELSE
         PX = sqrt(PX1(I,1)**2+PX1(I,2)**2+(PLAB-PX1(I,3))**2)
         PM2=(ELAB-PX1(I,4)-PX)*(ELAB-PX1(I,4)+PX)
         IF(PM2.lt.0.) PM2=abs(PM2)
         PM2=sqrt(PM2)
       ENDIF

*...boost of all momenta to partonic CMS
       GAMMA = 1.d0/(sqrt(ELAB-PLAB)*sqrt(ELAB+PLAB))
       DO j=1,NUP
          PX2(j,1)=PX1(j,1)
          PX2(j,2)=PX1(j,2)
          PX2(j,3) = GAMMA*(-(PLAB*PX1(j,4))+ELAB*PX1(j,3))
          PX2(j,4) = GAMMA*( ELAB*PX1(j,4)-PLAB*PX1(j,3))
          PX2(j,5)=PX1(j,5)
       ENDDO

*...defs for Breit-Wigner measure                                                                 
       PMAV = PX2(I,5)
       WIDTH = PMAS(pycomp(IDUP(I)),2)
       WBAND = PMAS(pycomp(IDUP(I)),3)
       PMUP = WBAND+PMAV
       PMDN =-WBAND+PMAV
       ATL=ATAN((PMDN**2-PMAV**2)/(PMAV*WIDTH))
       ATU=ATAN((PMUP**2-PMAV**2)/(PMAV*WIDTH))

*...BW smearing of the mass of Ith particle                                                       
 555       PM1=sqrt(PMAV**2+PMAV*WIDTH*TAN(ATL+PYR(0)*(ATU-ATL)))
       PX3(I,5)=PM1                         ! new mass of Ith particle                            

*...new energy of the Ith particle in partonic CMS                                                
       E1N = (PM1**2-PM2**2 + SHAT)/2.d0/SQRT(SHAT)
       IF(E1N.le.PM1) goto 555   !check masses                                                    
       PX3(I,4)=E1N
*...old and new energy of the recoiled cluster in partonic CMS                                    
       E2  = sqrt(SHAT)-PX2(I,4)
       E2N = sqrt(SHAT)-E1N
       IF(E2N.le.PM2) goto 555   !check masses                                                    

*...old space momenta of the Ith particle and recoiled cluster in partonic CMS                    
       PMOD=sqrt(PX2(I,1)**2+ PX2(I,2)**2+PX2(I,3)**2)
*...new space momenta of the Ith particle and recoiled cluster in partonic CMS                    
       PMODN=sqrt(E1N-PM1)*sqrt(E1N+PM1)
*...new space components of the Ith momentum in partonic CMS via rescaling                        
       DO k=1,3
         PXX(I,k)=PX2(I,k)/PMOD
         PX3(I,k)=PXX(I,k)*PMODN
       ENDDO

*...new momentum of the recoiled particle in 2->2 case                                            
       IF(NUP.eq.4) THEN
         IF(I.eq.3) I2=4
         IF(I.eq.4) I2=3
         PX3(I2,1)=-PX3(I,1)
         PX3(I2,2)=-PX3(I,2)
         PX3(I2,3)=-PX3(I,3)
         PX3(I2,4)= E2N
         PX3(I2,5)= PM2
         GOTO 333
       ENDIF

*...new momenta for other out particles (neq Ith)
       GAMMA = 1.d0/sqrt((E2-PMOD)*(E2N+PMODN))
     &        *1.d0/sqrt((E2+PMOD)*(E2N-PMODN))
       DO j=3,NUP
        IF(j.ne.I) then
          PJL=-(PX2(j,1)*PXX(I,1)+PX2(j,2)*PXX(I,2)+PX2(j,3)*PXX(I,3))
          PJLN=GAMMA*((-(E2N*PMOD-E2*PMODN))*PX2(j,4)
     &                +(E2*E2N-PMOD*PMODN)*PJL)
          PX3(j,4)=GAMMA*((E2*E2N-PMOD*PMODN)*PX2(j,4)
     &                   -(E2N*PMOD-E2*PMODN)*PJL)
          PX3(j,1)=PX2(j,1)-(PJLN-PJL)*PXX(I,1)
          PX3(j,2)=PX2(j,2)-(PJLN-PJL)*PXX(I,2)
          PX3(j,3)=PX2(j,3)-(PJLN-PJL)*PXX(I,3)
          PX3(j,5)=PX2(j,5)
        ENDIF
       ENDDO

 333       CONTINUE
*...boost from partonic CMS to the lab frame                                                      
       GAMMA = 1.d0/(sqrt(ELAB-PLAB)*sqrt(ELAB+PLAB))
       DO j=3,NUP
          PUP(1,j)=PX3(j,1)
          PUP(2,j)=PX3(j,2)
          PUP(3,j) = GAMMA*(PLAB*PX3(j,4)+ELAB*PX3(j,3))
          PUP(4,j) = GAMMA*(ELAB*PX3(j,4)+PLAB*PX3(j,3))
          PUP(5,j)=PX3(j,5)
       ENDDO

      ENDIF                     ! IF(PMAS(pycomp(IDUP(I)),2).gt.0.)                               
      ENDDO                     ! DO I=3,NUP                                                      

      RETURN
      END
