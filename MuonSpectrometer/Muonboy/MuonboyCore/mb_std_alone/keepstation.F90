!
 LOGICAL FUNCTION KeepStation(Typ, Jtyp,Jff,Jzz,Job)
 USE M_MB_MuGeom
 USE M_MB_StandAlone
 IMPLICIT NONE
 CHARACTER(3), INTENT(IN) :: Typ
 INTEGER,      INTENT(IN) :: Jtyp,Jff,Jzz,Job
 CHARACTER(4) :: chaf
 INTEGER      :: i, ifi, ic,inof,inoz, ict
!
   Keepstation = .TRUE.
   IF( Nchaf == 0 ) RETURN
!
   KeepStation = .FALSE.
   CALL GETIND(Jtyp,Jff,Jzz,Job, ic,inof,inoz, ict)
   DO i=1,Nchaf
     chaf = Cchaf(i)
     READ(chaf(2:2),'(I1)') ifi
     IF( ifi /= 0 .AND. Jff /= ifi )              CYCLE
     IF( chaf(4:4) == '+' .AND. Jzz < 0 )         CYCLE
     IF( chaf(4:4) == '-' .AND. Jzz > 0 )         CYCLE
     IF( ict == 0 ) THEN   ! MDT+CSC
       IF( chaf(1:1) == 'B' .AND. IC  >  NCOUBA ) CYCLE
       IF( chaf(1:1) == 'E' .AND. IC  <= NCOUBA ) CYCLE
     ELSE                  ! RPC+TGC
       IF( chaf(1:1) == 'B' .AND. ICT >  NTRIBA ) CYCLE
       IF( chaf(1:1) == 'E' .AND. ICT <= NTRIBA ) CYCLE
     ENDIF
     IF( Typ(1:1) /= 'T' ) THEN
       SELECT CASE ( chaf(3:3) )
         CASE ( 'S' )
           IF( Typ(3:3) == 'L' )                 CYCLE
           IF( Typ(3:3) == 'M' )                 CYCLE  ! BIM
           IF( Typ(3:3) == 'R' )                 CYCLE  ! BIR
         CASE ( 'L' )
           IF( Typ(3:3) == 'S' )                 CYCLE
           IF( Typ(3:3) == 'F' )                 CYCLE  ! BMF, BOF
           IF( Typ(3:3) == 'G' )                 CYCLE  ! BOG
           IF( Typ(3:3) == 'E' )                 CYCLE  ! BEE
         CASE DEFAULT                                   ! keep all
       END SELECT
     ENDIF
     KeepStation = .TRUE.
     EXIT
   ENDDO
!
 END FUNCTION KeepStation
!
