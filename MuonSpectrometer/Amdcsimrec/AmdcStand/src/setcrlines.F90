!
!> \author SamuSog
!
!--------------------------------------------------------------------
!> Reset C and R lines
!--------------------------------------------------------------------
 SUBROUTINE resetcrlines
 IMPLICIT NONE
 INTEGER JVERIF
 DATA JVERIF/ 7777770 /
!
#include "AmdcStand/amdcsim_com.inc"
!
!
      NBdeadChan = JVERIF+7
      CALL SETNIL( JTYPFZOLdeadChan(1), 6*MdeadChan+2, JVERIF+7 )
!
 END SUBROUTINE resetcrlines
!
!--------------------------------------------------------------------
!> Transfert C lines
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert C lines: This reproduces code originaly in readmdb
!--------------------------------------------------------------------
 SUBROUTINE setclines (JTYP,JFF,JZZ,JOB,JLAY, C, ier)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB, JLAY 
 INTEGER, INTENT(IN)  :: C(20)
 INTEGER, INTENT(OUT) :: ier
 INTEGER :: Inew
 INTEGER :: Icode
 INTEGER :: L5, L
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!
      NBdeadChan = NBdeadChan + 1
      IF( NBdeadChan.GT.MdeadChan ) THEN
        ier = 1
        PRINT 7032
7032    FORMAT(/' In setclines, Number of dead (or removed) MDT ',   &
                'channels entries is outside range =====> STOP !')
        STOP
      ENDIF
!
      Inew = NBdeadChan
      CALL CODE_JTYP_F_Z_O_L(JTYP,JFF,JZZ,JOB,JLAY, Icode)
      JTYPFZOLdeadChan(Inew) = Icode
      L5 = 0
      DO L=1,20
        IF( C(L).EQ.0 ) CYCLE
        L5 = L5 + 1
        IF( L5.GT.5 .OR. (L.LT.20 .AND. L5.GT.4 .AND. C(L+1).LT.0) ) THEN
          NBdeadChan = NBdeadChan + 1
          IF( NBdeadChan.GT.MdeadChan ) THEN
            ier = 1
            PRINT 7032
            STOP
          ENDIF
          Inew = NBdeadChan
          JTYPFZOLdeadChan(Inew) = Icode
          L5 = 1
        ENDIF
        JCHANdeadChan(L5,Inew) = C(L)
      ENDDO
!
 END SUBROUTINE setclines
!
!--------------------------------------------------------------------
!> Transfert R lines
!--------------------------------------------------------------------
!--------------------------------------------------------------------
! Transfert R lines: This reproduces code originaly in readmdb
!--------------------------------------------------------------------
 SUBROUTINE setrlines (JTYP,JFF,JZZ,JOB,JLAY, R, ier)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: JTYP,JFF,JZZ,JOB,JLAY 
 INTEGER, INTENT(IN)  :: R(20)
 INTEGER, INTENT(OUT) :: ier
 INTEGER :: Inew
 INTEGER :: Icode
 INTEGER :: L5, L, I,J
!
#include "AmdcStand/amdcsim_com.inc"
!
      ier = 0
!
      NBdeadChan = NBdeadChan + 1
      IF( NBdeadChan.GT.MdeadChan ) THEN
        ier = 1
        PRINT 7032
7032    FORMAT(/' In setrlines, Number of dead (or removed) MDT ',   &
                'channels entries is outside range =====> STOP !')
        STOP
      ENDIF
!
      DO I=NBdeadChan,NBdisaChan+2,-1
       JTYPFZOLdeadChan(I) = JTYPFZOLdeadChan(I-1)
       DO J=1,5
        JCHANdeadChan(J,I) =  JCHANdeadChan(J,I-1)
       ENDDO
      ENDDO
      NBdisaChan = NBdisaChan + 1
      Inew = NBdisaChan
      CALL CODE_JTYP_F_Z_O_L(JTYP,JFF,JZZ,JOB,JLAY, Icode)
      JTYPFZOLdeadChan(Inew) = Icode
      L5 = 0
      DO 500 L=1,20
        IF( R(L).EQ.0 ) CYCLE
        L5 = L5 + 1
        IF( L5.GT.5 .OR. (L.LT.20 .AND. L5.GT.4 .AND. R(L+1).LT.0) ) THEN
          NBdeadChan = NBdeadChan + 1
          IF( NBdeadChan.GT.MdeadChan ) THEN
            ier = 1
            PRINT 7032
            STOP
          ENDIF
          DO I=NBdeadChan,NBdisaChan+2,-1
           JTYPFZOLdeadChan(I) = JTYPFZOLdeadChan(I-1)
           DO J=1,5
            JCHANdeadChan(J,I) =  JCHANdeadChan(J,I-1)
           ENDDO
          ENDDO
          NBdisaChan = NBdisaChan + 1
          Inew = NBdisaChan
          JTYPFZOLdeadChan(Inew) = Icode
          L5 = 1
        ENDIF
        JCHANdeadChan(L5,Inew) = R(L)
500   ENDDO
!
 END SUBROUTINE setrlines
!
