!
!
!
!
! Author : M.Virchaux
!***********************************************************************
 SUBROUTINE ELIMAT_DIM( NADMA, IV0 )
 USE M_MB_COMATI
 IMPLICIT NONE
 INTEGER :: NADMA, IV0
 INTEGER :: JAA(NADMA)
 INTEGER :: IEDEBV, IEFINV, IP, IE, IA, IV
 INTEGER :: IADNEW, IADOLD, IEDIFF, IPDIFF
!
!>>  WARNING : This routine changes the volume numbering (IV)
!>> *********
!
   IEDEBV = NEMATI
   IEFINV = 1
   JAA(:) = 1
   DO IP=NPMAT0(IV0),NPMAT1(IV0)
     DO IE=NEMAT0(IP),NEMAT1(IP)
       IEDEBV = MIN( IEDEBV , IE )
       IEFINV = MAX( IEFINV , IE )
       IA = IAMATI(IE)
       JAA(IA) = 0
     ENDDO
   ENDDO
   DO IV=1,NVMATI
     IF( IV /= IV0 ) THEN
       DO IP=NPMAT0(IV),NPMAT1(IV)
         DO IE=NEMAT0(IP),NEMAT1(IP)
           IA = IAMATI(IE)
           JAA(IA) = 1
         ENDDO
       ENDDO
     ENDIF
   ENDDO
!
!>> NAMATI -------------------------------------
   IADNEW = 0
   DO IADOLD=1,NADMA
     IF( JAA(IADOLD) >= 1 ) THEN
       IADNEW = IADNEW + 1
       JAA(IADOLD) = IADNEW
       XYZMATI(1:3,IADNEW) = XYZMATI(1:3,IADOLD)
     ENDIF
   ENDDO
   NAMATI = IADNEW
!
!>> NEMATI -------------------------------------
   IEDIFF = IEFINV - IEDEBV + 1
   DO IE=1,IEDEBV-1
     IA = IAMATI(IE)
     IAMATI(IE)        = JAA(IA)
   ENDDO
   DO IE=IEFINV+1,NEMATI
     IA = IAMATI(IE)
     IAMATI(IE-IEDIFF) = JAA(IA)
   ENDDO
   NEMATI = NEMATI - IEDIFF
!
!>> NPMATI -------------------------------------
   IPDIFF = NPMAT1(IV0) - NPMAT0(IV0) + 1
   DO IP=NPMAT1(IV0)+1,NPMATI
     NEMAT0(IP-IPDIFF) = NEMAT0(IP) - IEDIFF
     NEMAT1(IP-IPDIFF) = NEMAT1(IP) - IEDIFF
   ENDDO
   NPMATI = NPMATI - IPDIFF
!
!>> NVMATI -------------------------------------
   DO IV=IV0+1,NVMATI
     NPMAT0    (IV-1) = NPMAT0    (IV) - IPDIFF
     NPMAT1    (IV-1) = NPMAT1    (IV) - IPDIFF
     ITRANSMATI(IV-1) = ITRANSMATI(IV)
     JMATI     (IV-1) = JMATI     (IV)
   ENDDO
   NVMATI = NVMATI - 1
!
 END
