!
 SUBROUTINE cleanSegment(NDIM1, NBDRIT, segm)
 USE M_MB_Control
 USE M_MB_Types
 USE M_MB_PACONS, ONLY : CHIMA2, CHI1ML
 IMPLICIT NONE
 INTEGER      , INTENT(IN)    :: NDIM1
 INTEGER      , INTENT(INOUT) :: NBDRIT
 TYPE(Segment), INTENT(INOUT) :: segm(NDIM1)
 INTEGER :: i1, i2, n1, n2, k2, NSEG, irej
 INTEGER :: intpos
!
! I1DRIT = input = 1 for 1ML segments not on the edge, so dubious
!          internaly = 1 for segments to be removed 
!          no more used anymore on output
!
    irej = NBDRIT + 1
!
!>> Selection of segments in only one multi-layer
!       keep the ones with I1DRIT = 0 because could not cross another multi-layer
!       keep also others only if >= 3 tubes
!
    DO i1=1,NBDRIT
      IF( segm(i1)%i1ml == 0 ) CYCLE
      IF( segm(i1)%nu >= 3 .AND. segm(i1)%chi2 < CHI1ML+3.d0 ) THEN
        segm(i1)%i1ml = 0
      ELSE
        irej = MIN( irej , i1 )
      ENDIF
    ENDDO
!
! Remove ghosts (all digits in common with another better segment)
! if bad chi2 or segment with 2 digits
!
    DO i1=1,NBDRIT
      IF( segm(i1)%i1ml == 1 )                  CYCLE
      n1 = segm(i1)%nu
      DO i2=1,NBDRIT
        IF( i1 == i2 )                          CYCLE
        IF( segm(i2)%i1ml == 1 )                CYCLE
        n2 = segm(i2)%nu
        IF( segm(i2)%chi2 - segm(i1)%chi2 < CHIMA2/3.d0 ) THEN
          IF( segm(i2)%iw(1) /= 0 .AND. segm(i2)%iw(2) /= 0 ) THEN
            IF( n2 > 3 .OR. n1 <= 3 )           CYCLE
          ELSE
            IF( n2 > 2 .OR. n1 <= 2 )           CYCLE
          ENDIF           
        ENDIF
        DO k2=1,n2
           IF( intpos( segm(i2)%iu(k2), segm(i1)%iu(1:n1), n1 ) == 0 ) GO TO 100
        ENDDO
        segm(i2)%i1ml = 1
        irej = MIN( irej , i2 )
        IF( MBPri >= 3 ) THEN
          WRITE(*,*) ' ===== one ghost removed ===== chi2 ',segm(i1)%chi2,segm(i2)%chi2       &
                                                    ,' iw ',segm(i1)%iw(1:2),segm(i2)%iw(1:2)
          WRITE(*,*) segm(i1)%iu(1:n1)
          WRITE(*,*) segm(i2)%iu(1:n2)
        ENDIF
100     CONTINUE
      ENDDO
    ENDDO
!
    IF( irej <= NBDRIT ) THEN  ! at least one segment (irej) to be removed. Update the arrays
      NSEG = irej - 1
      DO i1=irej+1,NBDRIT
        IF( segm(i1)%i1ml ==  1 ) CYCLE
        NSEG = NSEG + 1
        segm(NSEG) = segm(i1)
      ENDDO
      NBDRIT = NSEG
    ENDIF
!
 END SUBROUTINE cleanSegment
!
