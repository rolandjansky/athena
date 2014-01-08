!
!> \author M.Virchaux
!
 SUBROUTINE NEWMAT(AAA,IMANAT,JFF,JZZ)
 USE M_MB_Increm_Index
 USE M_MB_COMATI
 IMPLICIT NONE
 CHARACTER(8), INTENT(IN) :: AAA
 INTEGER     , INTENT(IN) :: IMANAT, JFF, JZZ
 INTEGER :: IMANAM
!
#include "AmdcStand/comama.inc"
!
      DO IMANAM=1,NMANAM
        IF( AAA == CAMATI(IMANAM) ) GO TO 110
      ENDDO
      NMANAM = NMANAM + 1
      IMANAM = NMANAM
      CAMATI(IMANAM) = AAA
      IF( NMANAM >= KMANAM ) THEN
        WRITE(*,'(//" In NEWMAT : Parameter KMANAM is not big enough  =====>  STOP !!!")')
        STOP
      ENDIF
110   CONTINUE
!
      IF( IMANAT < 1 .OR. IMANAT > MAMAMA ) THEN
         WRITE(*,'(//" In NEWMAT : Argument IMANAT =",I4,"  is out of range  =====>  STOP !!!")') IMANAT
         WRITE(*,'("         a value 777 indicates that an unknow material is used")')
         STOP
      ENDIF
!
      IF( JFF <= -1 .OR. JFF >= KMAJFF ) THEN
         WRITE(*,'(//" In NEWMAT : Argument JFF =",I4,"  is out of range  =====>  STOP !!!")') JFF
         STOP
      ENDIF
!
      IF( JZZ <= -1 .OR. JZZ >= KMAJZZ ) THEN
         WRITE(*,'(//" In NEWMAT : Argument JZZ =",I6,"  is out of range  =====>  STOP !!!")') JZZ
         STOP
      ENDIF
!
      IF( X0MAMA(IMANAT) > 99999.D0 .OR. EAMAMA(IMANAT) < 0.01D0 ) THEN
         WRITE(*,'(/" Warning : In NEWMAT, matter type",I4,"  is used but is not defined in AMDC_SIMREC !!!")') IMANAT
      ENDIF
!
      CALL Increm_Index(NVMATI,KVOLU,'KVOLU')
      ITRANSMATI(NVMATI) = 0
      JMATI(NVMATI)      = IMANAT * KMAJFF*KMAJZZ*KMANAM   &
                         + IMANAM * KMAJFF*KMAJZZ          &
                         +   JZZ  * KMAJFF                 &
                         +   JFF
      NPMAT0(NVMATI)     = NPMATI + 1
!
 END SUBROUTINE NEWMAT
!
