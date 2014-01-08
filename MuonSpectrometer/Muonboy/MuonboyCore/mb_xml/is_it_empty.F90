!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 LOGICAL FUNCTION IS_IT_EMPTY(CAR80)
 USE M_MB_Xml_TREE
 IMPLICIT NONE
!>> NamePANIC is defined because of Linux F90 compiler bug !!!
 CHARACTER(80) :: CAR80,NamePANIC
 LOGICAL       :: IS_IT_REMOVED
 INTEGER       :: JV0,L0,L1,JV
!
   IS_IT_EMPTY = .True.
!
LoopJV0: DO JV0=1,NTree
           NamePANIC = NamTree(JV0)
           IF( NamePANIC /= CAR80 ) CYCLE LoopJV0
!
           L0 = LevTree(JV0)
           L1 = 999999
LoopJV:    DO JV=JV0+1,NTree
             IF( LevTree(JV) <= L0 ) EXIT LoopJV0
             IF( LevTree(JV) > L1 ) CYCLE LoopJV
             L1 = 999999
             NamePANIC = NamTree(JV)
             IF( IS_IT_REMOVED(NamePANIC) .OR. ABS(IflTree(JV)) == 2 .OR. ABS(IflTree(JV)) == 4 ) THEN
               L1 = LevTree(JV)
             ELSEIF( LevTree(JV+1) <= LevTree(JV) ) THEN
               IS_IT_EMPTY = .False.
               EXIT LoopJV0
             ENDIF
           ENDDO LoopJV
!
         ENDDO LoopJV0
!
 END
