!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 SUBROUTINE DEF_GSLICEXY_NXYDZ( Name, Jcolor, NZ, ZZ, NXYZ, XXZ, YYZ )
 USE M_MB_General_CONSTANTS
 USE M_MB_General_Volumes
 IMPLICIT NONE
!
 CHARACTER (len=*), INTENT(IN) :: Name
 INTEGER,           INTENT(IN) :: Jcolor,NZ,NXYZ(*)
 REAL(8),           INTENT(IN) :: ZZ(*),XXZ(*),YYZ(*)
!
 INTEGER              :: NadgeDeb, IzA,IzB, NXYmin,NXYmax, I
 INTEGER              :: I0A,NbA,I0B,NbB, JA0,JB0,JA1,JB1,JA2,JB2, JAmin,JBmin
 INTEGER, ALLOCATABLE :: Iadd(:),Ivue(:)
 REAL(8)              :: Diff1,Diff2,Diffmin
!
!
   IF( NZ < 2 )     RETURN
!
   NXYmin = NXYZ(1)
   NXYmax = NXYZ(1)
   DO IzB=2,NZ
    IzA = IzB - 1
    NXYmin = MIN( NXYmin , NXYZ(IzB)-NXYZ(IzA) )
    NXYmax = MAX( NXYmax , NXYZ(IzB)-NXYZ(IzA) )
   ENDDO
   IF( NXYmin < 3 ) RETURN
!
   CALL NEWVOL( Name, 1 )
   CALL DEF_CENVOL( Zero,Zero,Zero )
!
   NadgeDeb = Nadge
   IF( SIZE(XYZadge)/3 < Nadge+NXYZ(NZ) ) CALL Push_MB_Gen_Vol_MADGE
   DO IzA=1,NZ
    I0A = 1
    IF( IzA > 1 ) I0A = NXYZ(IzA-1) + 1
    DO I=I0A,NXYZ(IzA)
     Nadge = Nadge + 1
     XYZadge(1:3,Nadge) = (/ XXZ(I), YYZ(I), ZZ(IzA) /)
    ENDDO
   ENDDO
!
   ALLOCATE( Iadd(NXYmax) )
   ALLOCATE( Ivue(NXYmax) )
   Ivue(1:NXYmax) = 1
!
   DO I=1,NXYZ(1)
    Iadd(I) = NadgeDeb + I
   ENDDO
   CALL DEFPLA( Jcolor, NXYZ(1), Ivue, Iadd )
!
   I0A = 0
   NbA = NXYZ(1)
   DO IzB=2,NZ
    IzA = IzB - 1
    I0B = NXYZ(IzA)
    NbB = NXYZ(IzB) - NXYZ(IzA)
    Diffmin = 999999999.D0**2
    JAmin   = 1
    JBmin   = 1
    DO JA1=1,NbA
     DO JB1=1,NbB
      Diff1 = (XXZ(I0A+JA1)-XXZ(I0B+JB1))**2 + (YYZ(I0A+JA1)-YYZ(I0B+JB1))**2
      IF( Diff1 > Diffmin ) CYCLE
         Diffmin = Diff1
         JAmin   = JA1
         JBmin   = JB1
     ENDDO
    ENDDO
    JA0 = 0
    JB0 = 0
    JA1 = JAmin
    JB1 = JBmin
    DO
     JA2 = MOD(JA1,NbA) + 1
     JB2 = MOD(JB1,NbB) + 1
     Diff1 = (XXZ(I0A+JA1)-XXZ(I0B+JB2))**2 + (YYZ(I0A+JA1)-YYZ(I0B+JB2))**2
     Diff2 = (XXZ(I0A+JA2)-XXZ(I0B+JB1))**2 + (YYZ(I0A+JA2)-YYZ(I0B+JB1))**2
     IF( ( Diff1 < Diff2 .AND. JB0 /= JBmin ) .OR. JA0 == JAmin ) THEN
        Iadd(1) = NadgeDeb + I0B + JB1
        Iadd(2) = NadgeDeb + I0B + JB2
        Iadd(3) = NadgeDeb + I0A + JA1
        CALL DEFPLA( Jcolor, 3, Ivue, Iadd )
        JB1 = JB2
        JB0 = JB2
     ELSE
        Iadd(1) = NadgeDeb + I0A + JA1
        Iadd(2) = NadgeDeb + I0A + JA2
        Iadd(3) = NadgeDeb + I0B + JB1
        CALL DEFPLA( Jcolor, 3, Ivue, Iadd )
        JA1 = JA2
        JA0 = JA2
     ENDIF
     IF( JA0 == JAmin .AND. JB0 == JBmin ) EXIT
    ENDDO
    I0A = I0B
    NbA = NbB
   ENDDO
!
   DO I=1,NXYZ(NZ)-NXYZ(NZ-1)
    Iadd(I) = NadgeDeb + NXYZ(NZ-1) + I
   ENDDO
   CALL DEFPLA( Jcolor, NXYZ(NZ)-NXYZ(NZ-1), Ivue, Iadd )
!
   CALL ENDVOL_SUV
!
   DEALLOCATE( Iadd )
   DEALLOCATE( Ivue )
!
 END
