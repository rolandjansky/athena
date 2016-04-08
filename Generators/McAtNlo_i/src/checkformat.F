      FUNCTION CHECKFORMAT(IUNIT, JP)
      IMPLICIT NONE
C     Returns true if MC@NLO 4.0 format with x1,x2,qscale is detected,
C        false otherwise.
C     Checks records with momenta for MC@NLO <4.0 
      INTEGER CHECKFORMAT
      INTEGER CF
      INTEGER IUNIT 
      INTEGER JP
      LOGICAL WZ,WW

      INTEGER ITMP,IOS,I,J,CNT,NP
      DOUBLE PRECISION DTMP
      CHARACTER * 200 BUFFER

      INTEGER MAXNP                    
      PARAMETER(MAXNP=9)                   
      DOUBLE PRECISION MPUP(4,MAXNP)
      
      IF(JP.EQ.2870.OR.JP.EQ.2880)THEN
         WZ=.TRUE.
         WW=.FALSE.
C         print *, "WZ mode"
      ELSEIF (JP.EQ.2850)THEN
         WW=.TRUE.
         WZ=.FALSE.
      ELSE
         WZ=.FALSE.
         WW=.FALSE.
C         print *, "not WZ mode"
      ENDIF
      
      CNT=0

      CHECKFORMAT = 0 

      CNT=CNT+1
      READ(IUNIT,921) ITMP,ITMP,NP
      CNT=CNT+1
      READ(IUNIT,922) (ITMP,I=1,NP)
      CNT=CNT+1
      READ(IUNIT,923) DTMP,DTMP
      CNT=CNT+1
      READ(IUNIT,924) ((MPUP(J,I),J=1,4),I=1,NP)

      
C     -  EOF for 1 event record ( MC@NLO ver <4.0  )
      CNT=CNT+1
      READ(UNIT=IUNIT,END=913, IOSTAT=IOS, FMT=926) BUFFER


C     -  EOF for 1 event record ( MC@NLO ver >=4.0  )
      CNT=CNT+1
      READ(UNIT=IUNIT,END=911, IOSTAT=IOS, FMT=926) BUFFER


      CNT=CNT+1
      CF=-1
      IF(WZ.OR. WW) THEN
         CF=0
         READ(UNIT=IUNIT,END=910, IOSTAT=IOS, FMT=921)  ITMP,ITMP,NP
         CF=-1 
 910    CONTINUE      
      ELSE
         READ(UNIT=IUNIT, IOSTAT=IOS, FMT=926) BUFFER
         NP=2
      ENDIF

C     - MPUP(J,I) set to 0. -- to be on save side        
      
      DO J=3,4      
         DO I=1,MAXNP      
            MPUP(J,I)=0.D0            
         ENDDO
      ENDDO   
 
      IF(WZ) THEN
         CHECKFORMAT = 2 
      ELSEIF (WW) THEN
         CHECKFORMAT = 3
      ELSE
         CHECKFORMAT = 1 
      ENDIF

     
C     -  For MC@NLO ver <4.0 it is a line with momenta
      CNT=CNT+1;
      READ(UNIT=IUNIT,ERR=913,IOSTAT=IOS,FMT=924)
     &     ((MPUP(J,I),J=1,4),I=1,MAXNP) 
      

 911  CONTINUE

C     -  Checks if there are more than two non-zero variables 
      IF(CF.LT.0) THEN
         IF(IOS.GT.-1) THEN
            DTMP=1.0D-30
            DO J=1,4      
               DO I=1,MAXNP      
                  IF(NP*(I-1)+J>NP.AND.DABS(MPUP(J,I)).GT.DTMP) THEN
                     CHECKFORMAT = 0 
                  ENDIF
               ENDDO
            ENDDO   
         ENDIF
      ELSE 
         CHECKFORMAT=0
      ENDIF
C     -   Going back with reading


 913  DO I=1,CNT
         BACKSPACE(IUNIT)
      ENDDO
      
C  -  Formats
 920  FORMAT(1X,I8)

 921  FORMAT(1X,I3,2(1X,I2))
 922  FORMAT(9(1X,I3))
 923  FORMAT(2(1X,D14.8))
 924  FORMAT(36(1X,D14.8))
 925  FORMAT(3(1X,D14.8))

 926  FORMAT(1X,A)

      RETURN
      END
