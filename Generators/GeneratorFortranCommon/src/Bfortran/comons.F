        subroutine comons(cc0,andors,cc1,cc2,cc3,cc4,cc5,cc6)
C...Reject events without b(c) quark within pT cuts 

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      CHARACTER*(*) andors
      common/cutbq/cut0(4),andor
      CHARACTER*(2) andor
      common/lvl1/cut1(3)
      common/lvl2/cut2(4)
      common/cutoff/cut3(7)
      common/repeat/NTB
      common/flavour/flav
      common/nt5000/ntup_5000
*KEEP,PRIVEVN.
* numbering of accepted b-events
       common/bevent/ieve,keve
       SAVE/bevent/      
      dimension cc0(4),cc1(3),cc2(4),cc3(7)
      DATA IFIRST /1/
C ----------- end CDE --------------------------------------------------
      
      idebug=0
      if(idebug.ne.0) print *,'comons: starts'
      
      IF(IFIRST.EQ.1) THEN
      call uinit
      if(idebug.ne.0) 
     +      print *,' comons: first time IFIRST =',IFIRST
        IFIRST =0
	if(idebug.ne.0) print *,' comons: cc0', cc0
      call dcopy(cc0,cut0,4)
      call dcopy(cc1,cut1,3)
      call dcopy(cc2,cut2,4)
      call dcopy(cc3,cut3,7)
      NTB=cc4
      flav=cc5
      ntup_5000=cc6
      andor=andors
      ieve=0
       print *,' comons: cut0', cut0
       print *,' comons: andor',andor
       print *,' comons: cut1', cut1
       print *,' comons: cut2', cut2
       print *,' comons: cut3', cut3
       print *,' comons: ntb flav ntup',NTB,flav,ntup_5000 
      
      
      else 
c       if(idebug.ne.0) print *,'comons:return IFIRST =',IFIRST
      return
      
      
      ENDIF
      return
      end
      
      subroutine dcopy(a,b,N)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      DIMENSION a(*),b(*)
      IF (N.EQ.0) RETURN
         DO 21 I=1,N
   21 B(I)=A(I)
      return 
      end
      
