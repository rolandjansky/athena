!
 SUBROUTINE GIV_MATTER_NUM(Name,J)
 USE M_MB_Control, ONLY : MBPri
 IMPLICIT NONE
 CHARACTER(*), INTENT(IN)  :: Name
 INTEGER     , INTENT(OUT) :: J
 INTEGER :: I,L
!
   L = LEN_TRIM(Name)
   I = IACHAR( Name(L:L) )
!>> If last character of  Name  is '1' or '2' or '3' or ... or '9',
!>> do not consider it !
   IF( I >= 49 .AND. I <= 57 ) L = L - 1
   SELECT CASE( Name(1:L) )
     CASE( 'Iron' )
       J = 1
     CASE( 'Carbon' )
       J = 3
     CASE( 'Aluminium' )
       J = 4
     CASE( 'Silicon' )
       J = 6
     CASE( 'Copper' )
       J = 8
     CASE( 'Honeycomb' )
       J = 7
     CASE( 'Brass' )             ! temporary consider it as Copper
       J = 8
     CASE( 'Lead' )
       J = 99
     CASE( 'PolyBoron5percent' ) ! temporary consider it as Carbon
       J = 3
     CASE( 'Polylithium' ) ! temporary consider it as Carbon
       J = 3
     CASE( 'ShieldBrass' ) ! temporary consider it as Copper
       J = 8
     CASE( 'ShieldIron' ) ! temporary consider it as common Iron
       J = 1
     CASE( 'ShieldSteel' ) ! temporary consider it as common Iron
       J = 1
     CASE( 'PolyBoronB4C' ) ! temporary consider it as Carbon
       J = 3
     CASE( 'PolyBoronB2O' ) ! temporary consider it as Carbon
       J = 3
     CASE( 'PolyBoronH3B0' ) ! temporary consider it as Carbon
       J = 3
     CASE DEFAULT
       IF( MBPri >= 1 ) WRITE(*,*) ' Muonboy/giv_matter_num unknow material ',TRIM(Name)
       J = 777
   END SELECT
!
 END SUBROUTINE GIV_MATTER_NUM
!
