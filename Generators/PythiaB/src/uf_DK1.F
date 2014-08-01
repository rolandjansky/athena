*CMZ :          
*-- Author :    Sagi Ben-Ami 2006-05-15
      SUBROUTINE uf_DK1(IRET)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_BBbarmu                                                    *
*        (module)                                                      *
*                                                                      *
*Description :B0->D0 K* ; D0->K+ Pi- ; K*->K+ Pi- (and conjugate)      *
*             Muon included with Pt cut 6 Gev                          *
*             Final state Particles with Pt cut of 0.5 Gev             *
* Arguments   :  IRET (in/out) accepting(0)/rejecting(1) event         *
*                                                                      *
*                                                                      *
* Author : Yoram Rozen                                                 *
*          Sagi Ben-Ami              Date : 2006-05-29                 *
*          sagiph@tx.technion.ac.il                                    *
*                                                                      *
* Last modifications :                                                 *
*  Sagi Ben-Ami                               Date :2006-11-05         *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*
*+CDE, TYPING.
C ----------- end CDE --------------------------------------------------
*
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
c      INTEGER PYK,PYCHGE,PYCOMP,Bcandidate1,Bcandidate2,Bcandidate3,Bcandidate4,stFlag,aflag,counter
      INTEGER Bcandidate1,stFlag,GenerationCounter
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,LUDAT1.
       COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
*KEEP,LUDAT2.
       COMMON/PYDAT2/KCHG(500,4),PMAS(500,4),PARF(2000),VCKM(4,4)
*KEEP,PYPARS.
       COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
*KEEP,PYINT1.
       COMMON/PYINT1/MINT(400),VINT(400)
*KEEP,PYINT2.
       COMMON/PYINT2/ISET(500),KFPR(500,2),COEF(500,20),ICOL(40,4,2)
*KEEP,PYINT5.
       COMMON/PYINT5/NGENPD,NGEN(0:500,3),XSEC(0:500,3)
*KEEP,PYUPPR.
       COMMON/PYUPPR/NUP,KUP(20,7),NFUP,IFUP(10,2),PUP(20,5),Q2UP(0:10)

*     user finsel decay selection and decay parameters
       common /ufdecay/ cufdcp(10),iufdc
       save   /ufdecay/
       common // GenerationCounter
*KEND.
*      SAVE /LUJETS/,/LUDAT1/,/LUDAT2/
      SAVE /PYJETS/,/PYDAT1/,/PYDAT2/
      SAVE /PYPARS/,/PYINT1/,/PYINT2/,/PYINT5/,/PYUPPR/
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      integer tree
      SAVE /btree/
*KEND.
c      SAVE IFIRST,IIPRNT
      SAVE IFIRST
      DATA IFIRST /1/
c      DATA IIPRNT /0/

*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)
        
       
       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.
C ----------- end CDE --------------------------------------------------

       idebug=1

       IF(IFIRST.EQ.1) THEN
          IFIRST =0
       ENDIF

      if ( iret.eq.1 ) return

      nintree = 0

c     preset
      iret = 1
      bFlag = 0
      MuFlag = 0
c     particle loop  -- modify content as needed
      do 111  I=1,N
         IF(name_selv(I) .eq. 511) then
      Bcandidate1 = I
      stFlag=0
      if(K(I,5)-K(I,4) .ne. 1) goto 111
      IF(NAME_DAUGHTER1(Bcandidate1)*NAME_DAUGHTER2(Bcandidate1) 
     +            .ne. -131773) goto 111
      IF(ABS(NAME_DAUGHTER1(Bcandidate1))
     &     .GT.ABS(NAME_DAUGHTER2(Bcandidate1))) 
     +   THEN
         IB0_D0=link_daughter1(Bcandidate1) ! THIS IS THE D
         IB0_kStar=link_daughter2(Bcandidate1)
      ELSE
         IB0_D0=link_daughter2(Bcandidate1) ! THIS IS THE D
         IB0_kStar=link_daughter1(Bcandidate1)
      ENDIF
      IF(abs(K(IB0_D0,5)-K(IB0_D0,4)) .NE. 1) GOTO 111
      IF(abs(K(IB0_kStar,5)-K(IB0_kStar,4)) .NE. 1) GOTO 111
        IF( K(K(IB0_kStar,4),2)*K(K(IB0_kStar,5),2) .ne. -67731) 
     &     GOTO 111
         if( (P(K(IB0_kStar,4),1)**2)+(P(K(IB0_kStar,4),2)**2) .lt. 0.25
     &       .or.(P(K(IB0_kStar,5),1)**2)+(P(K(IB0_kStar,5),2)**2) 
     &       .lt. 0.25) then
         print *,'failed kStar children: ',P(K(IB0_kStar,4),1),' ',
     &           P(K(IB0_kStar,4),2)
         print *,'failed kStar children: ',P(K(IB0_kStar,5),1),' ',
     &        P(K(IB0_kStar,5),2)
         print *,'kStar children Failed Pt threshold'
         goto 111
         endif
        IF( K(K(IB0_D0,4),2)*K(K(IB0_D0,5),2) .ne. -67731) GOTO 111
         if( (P(K(IB0_D0,4),1)**2)+(P(K(IB0_D0,4),2)**2) .lt. 0.25  .or.
     +      (P(K(IB0_D0,5),1)**2)+(P(K(IB0_D0,5),2)**2) .lt. 0.25) then
         print *,'failed D0 children: ',P(K(IB0_D0,4),1),' ',
     &           P(K(IB0_D0,4),2)
         print *,'failed D0 children: ',P(K(IB0_D0,5),1),' '
     &        ,P(K(IB0_D0,5),2)
         print *,'D0 children Failed Pt threshold'
         goto 111
         endif
           bFlag=1
           MuFlag=0
      endif 
 111  CONTINUE

       Do I=1,N
      If(ABS(K(I,2)) .eq. 13 .and.
     +(P(I,1)**2)+(P(I,2)**2) .gt. 36) then
             MuFlag = 1
             GOTO 1000
          endif
       enddo
         
       GOTO 1000

          
      
1000  continue

      if(MuFlag .eq. 0 .and. idebug.gt.1 ) then
      GenerationCounter=GenerationCounter+1
      print *,'No Muon, GenerationCounter= ',GenerationCounter
      endif

      if(MuFlag .eq. 1 .and. bFlag .eq. 0 .and. idebug.gt.1 ) then
      GenerationCounter=GenerationCounter+1
      print *,'No required Topology found, GenerationCounter= ',G
     &     enerationCounter
      endif
      
      if(MuFlag .eq. 1 .and. bFlag .eq. 1 .and. idebug.gt.0 ) then
      iret=0
      GenerationCounter=GenerationCounter+1
      print *,' GenerationCounter= ',GenerationCounter
      print *,' Muon Father is ', name_father(I)
      print *,' Muon is ', name_selv(I)
      endif     
      
      return
*     
      END


                       ! USER_FINSEa
