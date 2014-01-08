!
 SUBROUTINE GIV_COLOR_NUM(Name,J)
 IMPLICIT NONE
 CHARACTER (len=*) :: Name
 INTEGER           :: I,J,K,L,P
!
   L = LEN_TRIM(Name)
! Check whether it is an integer or a string
   J = 0
   P = 1
   DO K = 1,L
      I = IACHAR(Name(L+1-K:L+1-K))
      IF ( I < 48 .OR. I > 57 ) THEN
         J = -1
         EXIT
      ENDIF
      J = J+P*(I-48)
      P = P*10
   ENDDO

! It is a number
   IF ( J > 0 ) THEN
! Warning : we suppose there are 20 color indices between 1 and 20
     J = MODULO(J,20)
     IF ( J == 0 ) J = 20
! Its is a string
   ELSE
     SELECT CASE( Name )
       CASE( 'Black' , 'Iron' )
         J = 1
       CASE( 'Red' ) 
         J = 2
       CASE( 'Magenta' )
         J = 3
       CASE( 'Blue' , 'Aluminium' , 'Silicon' )
         J = 4
       CASE( 'Yellow' )
         J = 5
       CASE( 'Purple' , 'Copper' ) 
         J = 6
       CASE( 'Azur' )
         J = 7
       CASE( 'Orange' , 'Honeycomb' )
         J = 8
       CASE( 'Green' ) 
         J = 9
       CASE( 'Brown' ) 
         J = 10
       CASE( 'Pink' ) 
         J = 11
       CASE( 'Aquamarine' ) 
         J = 12
       CASE( 'Turquoise' ) 
         J = 13
       CASE( 'Navy' )
         J = 14
       CASE( 'Khaki' ) 
         J = 15
       CASE( 'Salmon' ) 
         J = 16
       CASE( 'LightYellow' ) 
         J = 17
       CASE( 'Violet' ) 
         J = 18
       CASE( 'Gray' ) 
         J = 19
       CASE( 'AppleGreen' ) 
         J = 20
       CASE DEFAULT
         J = 1
     END SELECT
   ENDIF
!
 END SUBROUTINE GIV_COLOR_NUM
!
