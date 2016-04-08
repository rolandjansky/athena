      FUNCTION FILEVER(IUNIT, JP)

      IMPLICIT NONE
      INTEGER FILEVER

      INTEGER IVERSION

      INTEGER IUNIT
      INTEGER JP, IVER, I, J, CNT, IOS 

      INTEGER NP, IERR1, IERR2
      LOGICAL WZ,WW

      INTEGER MAXNP
      PARAMETER(MAXNP=9)
      DOUBLE PRECISION MPUP(4,MAXNP)
      
      DOUBLE PRECISION DTMP

      DOUBLE PRECISION DX1, DX2, DQSCALE
      INTEGER IDX1, IDX2, IDQSCALE
      
      DOUBLE PRECISION PID1, PID2, PID3 
      INTEGER IPID1, IPID2, IPID3 
      
      INTEGER ITMP, ITMP1, ITMP2

      ierr1 = 3
      ierr2 = 3
      
      IF(JP.EQ.2870.OR.JP.EQ.2880)THEN
         WZ=.TRUE.
         WW=.FALSE.
         print *, "WZ mode"
      ELSEIF (JP.EQ.2850)THEN
         WW=.TRUE.
         WZ=.FALSE.
      ELSE
         WZ=.FALSE.
         WW=.FALSE.
         print *, "not WZ mode"
      ENDIF
      
      FILEVER = 0 

      CNT=0
      
      CNT=CNT+1
      READ(IUNIT,921) ITMP1, ITMP2, NP
      
      CNT=CNT+1
      READ(IUNIT,922) (ITMP,I=1,NP)
     
c cross-section?       
      CNT=CNT+1
      READ(IUNIT,923) DTMP

c 4-vectors?      
      CNT=CNT+1
      READ(IUNIT,924) ((MPUP(J,I),J=1,4),I=1,NP)

c we assume integers first
      CNT=CNT+1
      READ(UNIT=IUNIT,END=910, IOSTAT=IOS, FMT=927) IDX1, IDX2, IDQSCALE

      ierr1 = 0

      if (idx1.eq.0.and.idx2.eq.0.and.idqscale.eq.0) then
        ierr1 = 3
      endif  
       
 910  if (ierr1.eq.3) then
        write(*,*) "FileVer: float instead of integer detected! (A)"

        BACKSPACE(IUNIT)
  
        READ(UNIT=IUNIT,FMT=925) DX1, DX2, DQSCALE
        
        idx1 = int(dx1)
        idx2 = int(dx2)
        idqscale = int(dqscale)
      endif
 

      CNT=CNT+1
      READ(UNIT=IUNIT,END=911, IOSTAT=IOS, FMT=927) ipid1, ipid2, ipid3
      ierr2 = 0
  
 911  if (ierr2.eq.3) then
        write(*,*) "FileVer: float instead of integer detected! (B)"

        BACKSPACE(IUNIT)
  
        READ(UNIT=IUNIT, FMT=925) PID1, PID2, PID3 
        
        ipid1 = int(pid1)
        ipid2 = int(pid2)
        ipid3 = int(pid3)
      endif
      
      IVER = IVERSION(IDX1, IDX2, IDQSCALE, IPID1, IPID2, IPID3)

      if (IVER.EQ.3) FILEVER = 0
      
      if ((IVER.EQ.0).AND.(WZ)) FILEVER = 2
      
      if ((IVER.EQ.0).AND.(WW)) FILEVER = 3
      
      if (IVER.EQ.4) FILEVER = 1 
      write(*,*) "McAtNlo file version: ", FILEVER

 913  DO I=1,CNT
        BACKSPACE(IUNIT)
      ENDDO

 920  FORMAT(1X,I8)

 921  FORMAT(1X,I3,2(1X,I2))
 922  FORMAT(9(1X,I3))
 923  FORMAT(2(1X,D14.8))
 924  FORMAT(36(1X,D14.8))
 925  FORMAT(3(1X,D14.8))

 927  FORMAT(3(1X,i3))

      RETURN
      END

c
c
c 
      FUNCTION IVERSION(IDX1, IDX2, IDQSCALE, IPID1, IPID2, IPID3)
      
      IMPLICIT NONE
      integer IVERSION 
      
      DOUBLE PRECISION IDX1, IDX2, IDQSCALE
      INTEGER IPID1, IPID2, IPID3
      INTEGER IPIDSUM 

      IVERSION = 0 
      
      IPIDSUM = IPID1 + IPID2

C check if version <= 4.0      
      if (IDX1.GT.400.AND.IDX1.LT.410) then 
        if ((IDX1.EQ.401.OR.IDX1.EQ.403).AND.
     &     (IPIDSUM.GT.-4.AND.IPIDSUM.LT.4)) then
          IVERSION = 3
c          return
        
        elseif ((IDX1.EQ.402.OR.IDX1.EQ.405).AND.
     &          (IPIDSUM.GT.21.AND.IPIDSUM.LT.25)) then
          IVERSION = 3
           
        elseif ((IDX1.EQ.404.OR.IDX1.EQ.406).AND.
     &          (IPIDSUM.LT.21.AND.IPIDSUM.GT.16)) then
          IVERSION = 3
        
        elseif (((IDX1.EQ.407).AND.IPIDSUM.EQ.42).OR. 
     &         ((IDX1.EQ.408).AND.(IPIDSUM.GT.1.AND.IPIDSUM.LT.9)).OR.
     &         ((IDX1.EQ.409).AND.(IPIDSUM.GT.-9.AND.IPIDSUM.LT.-1))) then
          IVERSION = 3

        endif 
      endif   

c format 4.0 or higher without weights
      if ((IPID1.GT.400.AND.IPID1.LT.410).AND.
c     &    (IPID2.EQ.0).AND.
     &    (IPID3.GT.0.AND.IPID3.LT.10)) then   

        IVERSION = 4    
      endif

      write(*,*) "Iversion: ", IVERSION

      RETURN 
      END

