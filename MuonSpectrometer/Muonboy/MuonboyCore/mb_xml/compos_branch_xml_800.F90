!
!
!
!
! Author : SaMuSoG
!***********************************************************************
 CHARACTER(800) FUNCTION Compos_Branch_xml_800(K)
 USE M_MB_Xml_TREE
 IMPLICIT NONE
 INTEGER :: K,KP
!
   Compos_Branch_xml_800 = ''
   IF( K < 1 .OR. K > NTree ) RETURN
!
   Compos_Branch_xml_800 = TRIM(NamTree(K))  // '/'
!
   KP = K
10 KP = KpreLev(KP)
   IF( KP < 1 ) RETURN
   Compos_Branch_xml_800 = TRIM(NamTree(KP)) // '/' // TRIM(Compos_Branch_xml_800)
   GO TO 10
!
 END
