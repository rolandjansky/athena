!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CCDBVar( Name, I1,I2,I3, CharVar )
 USE M_MB_CondDB
 IMPLICIT NONE
!
 CHARACTER (len=*),  INTENT(IN)  :: Name
 INTEGER,            INTENT(IN)  :: I1,I2,I3
 CHARACTER (len=80), INTENT(OUT) :: CharVar
!--------------------------------------------------------------------
! Return the values of chars from M_MB_CondDB
!--------------------------------------------------------------------
!
   SELECT CASE( Name )
     CASE('cdbfoldername')
       CharVar = cdbfoldername(I1)
     CASE('cdbtagname')
       CharVar = cdbtagname(I1)
     CASE('cdbalifoldername')
       CharVar = cdbalifoldername(I1)
     CASE('cdbalitagname')
       CharVar = cdbalitagname(I1)
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
1000 FORMAT(/' CCDBVar was called with the ',   &
              'following arguments :   ',A,3I6,'  =====>  STOP !')
!
 END SUBROUTINE CCDBVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 INTEGER FUNCTION ICDBVarBaz( Istop, IStatus, Name, I1,I2,I3 )
 USE M_MB_CondDB
 IMPLICIT NONE
!
 INTEGER,           INTENT(IN)  :: Istop
 INTEGER,           INTENT(OUT) :: IStatus
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
!
!--------------------------------------------------------------------
! Return the values of int from M_MB_CondDB
!--------------------------------------------------------------------
!
   IStatus = 1
!
   SELECT CASE( Name )
   CASE('ncdbtag')
      ICDBVarBaz = ncdbtag
      GO TO 900         
   CASE('ncdbmax')
      ICDBVarBaz = ncdbmax
      GO TO 900         
   CASE('ncdbalitag')
      ICDBVarBaz = ncdbalitag
      GO TO 900         
   END SELECT
!
890 CONTINUE
    IStatus = 0
    if (Istop.eq.1) then
      PRINT 1000,TRIM(Name),I1,I2,I3
1000  FORMAT(/' ICDBVarBaz was called with the following arguments :   ',A,3I6,'  =====>  STOP !')
      STOP
    endif
    return
!
900 IF( I1 /= 0 .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
    RETURN
!
910 IF( I1 < 1  .OR. I2 /= 0 .OR. I3 /= 0 ) GO TO 890
    RETURN
!
920 IF( I1 < 1  .OR. I2 < 1  .OR. I3 /= 0 ) GO TO 890
    RETURN
!
 END FUNCTION ICDBVarBaz
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetCCDBVar( Long, Name1, I1, I2, I3, Lvar,Cvar1 )
 IMPLICIT NONE
!
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Lvar
 CHARACTER (Len=1), INTENT(OUT) :: Cvar1(80)
!
 CHARACTER (len=Long) :: VarName
 CHARACTER (len=80)   :: Cvar
 INTEGER              :: I
!--------------------------------------------------------------------
! Return the values of chars from M_MB_CondDB
!--------------------------------------------------------------------
!
   DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    CALL CCDBVar( VarName, I1,I2,I3, Cvar )
    Lvar = MIN( LEN_TRIM(Cvar) , 60 )
    DO I=1,Lvar
      Cvar1(I) = Cvar(I:I)
    ENDDO
!
 END SUBROUTINE F1GetCCDBVar
! 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 INTEGER FUNCTION ICDBVar( Name, I1,I2,I3 )
 IMPLICIT NONE
! 
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: I1,I2,I3
!
 INTEGER, EXTERNAL    :: ICDBVarBaz
 INTEGER :: Istop, IStatus
!--------------------------------------------------------------------
! Return the values of int from M_MB_CondDB
!--------------------------------------------------------------------
!
 Istop = 1
 ICDBVar = ICDBVarBaz( Istop, IStatus, Name, I1,I2,I3 )
!
 END FUNCTION ICDBVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE F1GetICDBVar( Long, Name1, I1, I2, I3, Ivar )
 IMPLICIT NONE
!
 CHARACTER (Len=1), INTENT(IN)  :: Name1(*)
 INTEGER,           INTENT(IN)  :: Long,I1,I2,I3
 INTEGER,           INTENT(OUT) :: Ivar
!
 INTEGER, EXTERNAL    :: ICDBVar
 CHARACTER (len=Long) :: VarName
 INTEGER              :: I
!--------------------------------------------------------------------
! Return the values of int from M_MB_CondDB
!--------------------------------------------------------------------
!
    DO I=1,Long
      VarName(I:I) = Name1(I)
    ENDDO
    Ivar = ICDBVar( VarName, I1,I2,I3 )
!
 END SUBROUTINE F1GetICDBVar
!
