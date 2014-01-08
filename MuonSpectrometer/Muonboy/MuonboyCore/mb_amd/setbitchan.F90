!
 SUBROUTINE SetBitChan(JTYP,JFF,JZZ,JOB,Nlay,Nchan,Nword,Iword,Jbegin,Jend)
 IMPLICIT NONE
!
#include "AmdcStand/amdcsim_com.inc"
!
      INTEGER, INTENT(IN)  :: JTYP, JFF, JZZ, JOB, Nlay, Nchan
      INTEGER, INTENT(IN)  :: Jbegin, Jend
      INTEGER, INTENT(OUT) :: Nword, Iword(*)
      INTEGER :: J, Icode, JTYPj,JFFj,JZZj,JOBj,JLAYj, IP0, IP1, IP
      INTEGER :: Ichan0, I5, Ichan1, Ichan, Iaddress, I32a, I32b
!
      Nword = 0
!
      DO J=Jbegin,Jend
        Icode = JTYPFZOLdeadChan(J)
        CALL DECO_JTYP_F_Z_O_L(Icode, JTYPj,JFFj,JZZj,JOBj,JLAYj)
        IF( JTYPj /= JTYP ) CYCLE
        IF( JFFj  /= JFF  ) CYCLE
        IF( JZZj  /= JZZ  ) CYCLE
        IF( JOBj  /= JOB  ) CYCLE
        IF( 1 <= JLAYj .AND. JLAYj <= Nlay ) THEN
          IP0 = JLAYj
          IP1 = JLAYj
        ELSEIF( JLAYj == 0 ) THEN
          IP0 = 1
          IP1 = Nlay
        ELSE
          IP0 = 1
          IP1 = 0
        ENDIF
        DO IP=IP0,IP1
          Ichan0 = 0
          DO I5=1,5
            Ichan1 = JCHANdeadChan(I5,J)
            IF( Ichan1 < 0 ) THEN
              Ichan1 = - Ichan1
            ELSE
              Ichan0 = Ichan1
            ENDIF
            IF( Ichan0 > 0 ) THEN
              DO Ichan=Ichan0,Ichan1
                Iaddress = (IP-1)*Nchan + Ichan - 1
                I32a = Iaddress / 32
                I32b = Iaddress - I32a*32
                Iword(I32a+1) = IBSET( Iword(I32a+1) , I32b )
              ENDDO
              Nword = (Nlay*Nchan-1)/32 + 1
            ENDIF
            Ichan0 = Ichan1
          ENDDO
        ENDDO
      ENDDO
!
 END SUBROUTINE SetBitChan
!
