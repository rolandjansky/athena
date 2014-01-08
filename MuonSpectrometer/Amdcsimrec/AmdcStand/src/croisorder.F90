!
!> \author SamuSog
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE CROIST(TT,NLONG)
!
 IMPLICIT NONE
 REAL(8), INTENT(INOUT) :: TT(*)
 INTEGER, INTENT(IN)    :: NLONG
 INTEGER :: I, IM1, NHI, NLO, N, NTEST, J
!
 REAL(8) :: TCUR
!
!-- Sorts real*8 array TT in increasing order
!
    IF( NLONG <= 1 ) RETURN
!
    DO I=2,NLONG
      IM1=I-1
      TCUR = TT(I)
      IF( TCUR >= TT(IM1) ) CYCLE
      NHI=IM1
      NLO=0
      N=NHI-NLO-1
      DO WHILE( N >= 1 )
        NTEST=NLO+1+N/2
        IF( TCUR < TT(NTEST) ) THEN
          NHI=NTEST
        ELSE
          NLO=NTEST
        ENDIF
        N=NHI-NLO-1
      ENDDO
      DO J=IM1,NHI,-1
        TT(J+1) = TT(J)
      ENDDO
      TT(NHI) = TCUR
    ENDDO
!
 END SUBROUTINE CROIST
!
!--------------------------------------------------------------------
! 
!--------------------------------------------------------------------
 SUBROUTINE CROISS(TT,NLONG,NN)
!
 IMPLICIT NONE
 REAL(8), INTENT(IN)  :: TT(*)
 INTEGER, INTENT(IN)  :: NLONG
 INTEGER, INTENT(OUT) :: NN(*)
!
 INTEGER :: I, IM1, NHI, NLO, N, NTEST, J
!
!-- Gives increasing order of real(8) array TT in array NN
!
    NN(1)=1
    IF( NLONG <= 1 ) RETURN
    IF( TT(2) >=  TT(1) ) THEN
      NN(1) = 1
      NN(2) = 2
    ELSE
      NN(1) = 2
      NN(2) = 1
    ENDIF
    IF( NLONG <= 2 ) RETURN
!
    DO I=3,NLONG
      IM1=I-1
      IF( TT(I) >= TT(NN(IM1)) ) THEN
        NN(I)=I
      ELSE
        NHI=IM1
        NLO=0
        N=NHI-NLO-1
        DO WHILE( N >= 1 )
          NTEST=NLO+1+N/2
          IF( TT(I) < TT(NN(NTEST)) ) THEN
            NHI=NTEST
          ELSE
            NLO=NTEST
          ENDIF
          N=NHI-NLO-1
        ENDDO
        DO J=IM1,NHI,-1
          NN(J+1)=NN(J)
        ENDDO
        NN(NHI)=I
      ENDIF
    ENDDO
!
 END SUBROUTINE CROISS
!
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
 SUBROUTINE KROISS(II,NLONG,NN)
!
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: II(*), NLONG
 INTEGER, INTENT(OUT) :: NN(*)
!
 INTEGER :: I, IM1, NHI, NLO, N, NTEST, J
!
!-- Gives increasing order of integer array II in array NN
!
    NN(1)=1
    IF( NLONG <= 1 ) RETURN
    IF( II(2) >= II(1) ) THEN
      NN(1)=1
      NN(2)=2
    ELSE
      NN(1)=2
      NN(2)=1
    ENDIF
    IF( NLONG <= 2 ) RETURN
!
    DO I=3,NLONG
      IM1=I-1
      IF( II(I) >= II(NN(IM1)) ) THEN
        NN(I)=I
      ELSE
        NHI=IM1
        NLO=0
        N=NHI-NLO-1
        DO WHILE( N >= 1 )
          NTEST=NLO+1+N/2
          IF( II(I) < II(NN(NTEST)) ) THEN
            NHI=NTEST
          ELSE
            NLO=NTEST
          ENDIF
          N=NHI-NLO-1
        ENDDO
        DO J=IM1,NHI,-1
          NN(J+1)=NN(J)
        ENDDO
        NN(NHI)=I
      ENDIF
    ENDDO
!
 END SUBROUTINE KROISS
!
