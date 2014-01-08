!
!> Return the values of MuonBoy CHARACTER parameters and variables
!> \author M.Virchaux
!
 SUBROUTINE CMuboVar( Name, I1,I2,I3, CharVar )
 USE M_MB_Reco_Segments
 USE M_MB_MuGeom
 IMPLICIT NONE
 CHARACTER (len=*),  INTENT(IN)  :: Name
 INTEGER,            INTENT(IN)  :: I1,I2,I3
 CHARACTER (len=32), INTENT(OUT) :: CharVar
!
   SELECT CASE( Name )
     CASE('SNINTR')
       CharVar = CARISC( ISCINTR(I1) )
     CASE('CARISC')
       CharVar = CARISC(I1)
     CASE DEFAULT
       CharVar = ''
       GOTO 90
   END SELECT
!
   IF( I1 >= 1  .AND. I2 /= 0 .AND. I3 /= 0 )  RETURN
   RETURN
!
90 PRINT 1000,TRIM(Name),I1,I2,I3
   STOP
!
1000 FORMAT(/' MuonBoy subroutine CMuboVar was called with the ',   &
              'following arguments :   ',A,3I6,'  =====>  STOP !')
!
 END SUBROUTINE CMuboVar
!
!
!
 SUBROUTINE F1GetCMuboVar( Long, Name1, I1, I2, I3, Lvar,Cvar1 )
 IMPLICIT NONE
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Lvar
 CHARACTER (Len=1), INTENT(OUT) :: Cvar1(32)
 CHARACTER (len=Long) :: VarName
 CHARACTER (len=32)   :: Cvar
 INTEGER              :: I
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    CALL CMuboVar( VarName, I1,I2,I3, Cvar )
    Lvar = MIN( LEN_TRIM(Cvar) , 32 )
    DO I=1,Lvar
      Cvar1(I) = Cvar(I:I)
    ENDDO
!
 END SUBROUTINE F1GetCMuboVar
! 
