        subroutine parsel(iret,jbq,jcq)
C...Reject events without b(c) quark within pT cuts 

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      common/cutbq/pt_b_cut,eta_b_cut,pt_ab_cut,eta_ab_cut,andor
      CHARACTER*(2) andor
      common/flavour/flav
      common/multif/ibq,icq
      DATA IFIRST /1/
C ----------- end CDE --------------------------------------------------
      
      
      IF(IFIRST.EQ.1) THEN
c      ihard=0
        IFIRST =0
      ENDIF
      
      
      idebug=0
      
      iret=1
      NBGQ=0
      NABGQ=0
      icq=0.      ! number of c quarks
      ibq=0.      ! number of b quarks
      if(idebug.ne.0) then
      print *,'PARSEL: starts'
      print *,'PARSEL: iret, flav ',iret,flav
      print *,'PARSEL: cutbq',
     +      pt_b_cut,eta_b_cut,pt_ab_cut,eta_ab_cut,andor
      if((andor.EQ.'an').or.(andor.EQ.'AN')) 
     +      print *,'PARSEL: andor = and or AND ',andor
      if((andor.EQ.'or').or.(andor.EQ.'OR')) 
     +      print *,'PARSEL: andor = or or OR ',andor
      endif
       
      
      DO 3 I=3,N
       if(finf_quark(I,4.D0).ne.0) icq=icq+1
       if(finf_quark(I,5.D0).ne.0) ibq=ibq+1
       if(find_quark(I,flav).eq.0) go to 3     
          
      
C...PARAMETERS OF B(c)-QUARK
          PT = DSQRT(P(I,1)**2+P(I,2)**2)
          TH = DACOS(P(I,3)/DSQRT(PT**2+P(I,3)**2)) 
          ETA = -DLOG(MAX(.0001,DABS(DTAN(.5*TH))))
	if(idebug.ne.0) print *,'PARSEL: after eta i',i  
          PHI = atan2(P(I,1),P(I,2))
	if(idebug.ne.0) print *,'PARSEL: after phi i',i
	
          IF(K(I,2).eq.flav.and.PT.GT.pt_b_cut.
     +	  and.abs(eta).lt.eta_b_cut) NBGQ=NBGQ+1
          IF(K(I,2).eq.-flav.and.PT.GT.pt_ab_cut.
     +	  and.abs(eta).lt.eta_ab_cut) NABGQ=NABGQ+1
     

    3 continue
    
            jbq=ibq
	    jcq=icq
       

c the event is selected if both b and antib are inside cuts    
      if(    ((andor.EQ.'an').or.(andor.EQ.'AN'))
     +         .and.(NBGQ * NABGQ).ne.0) iret=0
     
c the event is selected if  b OR antib are inside cuts    
      if(    ((andor.EQ.'or').or.(andor.EQ.'OR'))
     +         .and.(NBGQ + NABGQ).ne.0) iret=0       
     
     
     
      if(idebug.ne.0) then
      if(    ((andor.EQ.'an').or.(andor.EQ.'AN'))
     +         .and.(NBGQ * NABGQ).ne.0) 
     +   print *,'PARSEL: posluchol andor iret ',andor,iret
      if(    ((andor.EQ.'or').or.(andor.EQ.'OR'))
     +         .and.(NBGQ + NABGQ).ne.0)       
     +   print *,'PARSEL: posluchol andor iret ',andor,iret 
      endif     
     

     
         
      if(idebug.ne.0.and.iret.eq.0) 
     +      print *,'PaRSEL: beauty or charm found in event:'
     
      return
      end
C --------------------------------------------------------------------------	     
       double precision function find_quark(i,fla)
c  find the first quark in lujets list  of flavour fla

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
        idebug=0
        find_quark=0.
      if(idebug.ne.0) print *,'find_quark: started'	
	
          IF(abs(K(I,2)).EQ.fla.AND.K(I,1).LE.12) THEN
            IF(abs(K(K(I,3),2)).NE.fla) THEN
            find_quark=1.
            ELSEIF(K(I,3).LE.8) THEN
            find_quark=1.
            ENDIF
          ENDIF
      if(idebug.ne.0) print *,'find_quark: ended'	  
	  
      return
      end
      
C --------------------------------------------------------------------------	     
       double precision function finf_quark(i,fla)
c  find the first quark in lujets list  of flavour fla

      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
      COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
        idebug=0
        finf_quark=0.
      if(idebug.ne.0) print *,'finf_quark: started'	
	
          IF(K(I,2).EQ.fla.AND.K(I,1).LE.12) THEN
            IF(K(K(I,3),2).NE.fla) THEN
            finf_quark=1.
            ELSEIF(K(I,3).LE.8) THEN
            finf_quark=1.
            ENDIF
          ENDIF
      if(idebug.ne.0) print *,'finf_quark: ended'	  
	  
      return
      end
      
