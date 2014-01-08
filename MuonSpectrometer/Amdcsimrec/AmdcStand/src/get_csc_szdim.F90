!
!> \author SaMuSoG   13/11/96
!
 SUBROUTINE Get_CSC_SZdim(Jtyp,Jff,Jzz,Jobj, S1P, S2Pi, S2Po1,S2Po2, S2Pa, Zi,Zo,Za )
 IMPLICIT REAL(8) (A-H,O-Z)
!
!> Returns the S,Z dimensions of the CSC object  Jobj
!> of the station  Jtyp,Jff,Jzz   (in cm)  :
!>                                   ****
!>
!>
!> :                             :
!> :<--------- S2Pa ------------>:
!> :                             :
!> :      :                      :
!> :      :<------ S2Po2 ------->:
!> :      :                      :
!> :      :  :                   :
!> :      :  :<------ S2Pi ----->:
!> :      :  :                   :
!> :      :  :     :             :
!> :      :  :     :<-- S2Po1 -->:
!> :      :  :     :             :
!> :      :  :     :             :
!> :......:..:.....:.............:....................................
!>  .     :  :     :           . : .                               ^
!>   .    :  :     :         .   :   .                             |
!>    .   :  :     :       .     :     .                           |
!>     .  :  :     :     .       :       .                         |
!>      . :  :     :   .         :         .                       |
!>       .:  :     : .           :           .                     |
!>        :........:***************************.................   |
!>         . :    *******************************            ^     |
!>          .:  ***********************************          |     |
!>           :***************************************.....   |     | Za
!>            ***************************************  ^     |     |
!>             ***************       ***************   |     |     |
!>              **************  CSC  **************    |     | Zo  |
!>   ____________*************       *************_____|_____|_____|____
!>  /             *******************************      |     |     |    \
!>  |              *****************************       | Zi  |     |    |
!>  |               ***************************        |     |     |    |
!>  |                *************************         |     |     |    |
!>  |                 ***********************          |     |     |    |
!>  |                  *********************           |     |     |    |
!>  |                   *******************            v     v     v    |
!>  |                   :*****************............................  |
!>  |                   :        :                                      |
!>  |                   :        :                                      |
!>  |                   :<- S1P->:                                      |
!>  |                   :        :                                      |
!>  |                                                                   |
!>  |                                                                   |
!>  |                                                                   |
!>  +---------------------->  Zo / 2  <---------------------------------+
!>                           ========
!>   The Z line where the Phi strip number calculation is made is Zo/2
!>
!
#include "AmdcStand/amdcsim_com.inc"
!
!-- Statement functions
#include "AmdcStand/amdcsim_stfunc.inc"
!
      Jgeo = IGEO(Jtyp,Jff,Jzz)
      Jcut = ICUT(Jtyp,Jff,Jzz)
      IF( Jgeo.LE.0 )                            GO TO 900
!
      Jtec = ITEC(Jtyp,Jgeo,Jobj)
      Jsha = ISHAPE(Jtyp,Jgeo,Jobj)
      IF( CARTEC(Jtec).NE.'CSC' .OR. Jsha.NE.2 ) GO TO 900
!
      Zi    = Geo_Le(Jtyp,Jgeo,Jobj) / 10.D0
      Za    = Geo_Ex(Jtyp,Jgeo,Jobj) / 10.D0
      S1P   = Geo_Ws(Jtyp,Jgeo,Jobj) / 20.D0
      S2Pi  = Geo_Wl(Jtyp,Jgeo,Jobj) / 20.D0
      S2Pa  = S1P + (S2Pi-S1P)*Za/Zi
      Zo    = Za
      IF( Jcut.GT.0 ) THEN
        JTOTCU = INOCUT(Jtyp,Jgeo,Jcut)
        DO 100 JOBCUT=1,NOBCUT(JTOTCU)
          IF( IOBCUT(JTOTCU,JOBCUT).NE.Jobj )  CYCLE
          Zo = (Cut_dy(JTOTCU,JOBCUT)-Geo_dy(Jtyp,Jgeo,Jobj)) / 10.D0
          IF( Zo.LT.Zi .OR. Zo.GT.Za )         GO TO 900
          Ztest = Zo + Cut_Le(JTOTCU,JOBCUT) / 10.D0
          IF( Ztest.LT.Za )                    GO TO 900
          Wstest = Cut_Ws(JTOTCU,JOBCUT) / 20.D0
          Wltest = Cut_Wl(JTOTCU,JOBCUT) / 20.D0
          Wtest = MIN( Wstest , Wltest )
          IF( Wtest.LT.S2Pi )                  GO TO 900
          GO TO 110
100     ENDDO
      ENDIF
110   S2Po2 = S1P + (S2Pi-S1P)*Zo/Zi
      IF( ABS(Za-Zi).GT.0.001D0 ) THEN
        S2Po1 = S2Pi*(Za-Zo)/(Za-Zi)
      ELSE
        S2Po1 = S2Pi
      ENDIF
!
      RETURN
!
900   S1P   = 0.D0
      S2Pi  = 0.D0
      S2Po1 = 0.D0
      S2Po2 = 0.D0
      S2Pa  = 0.D0
      Zi    = 0.D0
      Zo    = 0.D0
      Za    = 0.D0
      PRINT 1000,Jobj,Jtyp,Jff,Jzz
1000  FORMAT(/' Subroutine Get_CSC_SZdim was called for the object  ',   &
              'Jobj =',I4,'  of the station  Jtyp,Jff,Jzz =',3I4,        &
             /' which is NOT of Standard CSC Type !!!')
!
 END SUBROUTINE Get_CSC_SZdim
!
