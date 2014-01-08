!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 REAL(8) FUNCTION RAmdcVar( Name, I1,I2,I3 )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
!--------------------------------------------------------------------
! 
! Return the values of Amdcsimrec REAL(8) parameters and variables
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
#include "AmdcStand/amdcsim_stfunc.inc"

      SELECT CASE( Name )
      CASE('Pos_Phi')
         RAmdcVar = Pos_Phi(I1,I2,I3)
         RETURN
      CASE('Pos_Z')
         RAmdcVar = Pos_Z(I1,I2,I3)
         RETURN
      CASE('Pos_R')
         RAmdcVar = Pos_R(I1,I2,I3)
         RETURN
      CASE('Pos_S')
         RAmdcVar = Pos_S(I1,I2,I3)
         RETURN
      CASE('Pos_Alfa')
         RAmdcVar = Pos_Alfa(I1,I2,I3)
         RETURN
      CASE('Pos_Beta')
         RAmdcVar = Pos_Beta(I1,I2,I3)
         RETURN
      CASE('Pos_Gama')
         RAmdcVar = Pos_Gama(I1,I2,I3)
         RETURN
      CASE('Geo_dx')
         RAmdcVar = Geo_dx(I1,I2,I3)
         RETURN
      CASE('Geo_dy')
         RAmdcVar = Geo_dy(I1,I2,I3)
         RETURN
      CASE('Geo_dz')
         RAmdcVar = Geo_dz(I1,I2,I3)
         RETURN
      CASE('Geo_Ws')
         RAmdcVar = Geo_Ws(I1,I2,I3)
         RETURN
      CASE('Geo_Wl')
         RAmdcVar = Geo_Wl(I1,I2,I3)
         RETURN
      CASE('Geo_Le')
         RAmdcVar = Geo_Le(I1,I2,I3)
         RETURN
      CASE('Geo_Ex')
         RAmdcVar = Geo_Ex(I1,I2,I3)
         RETURN
      CASE('Geo_D1')
         RAmdcVar = Geo_D1(I1,I2,I3)
         RETURN
      CASE('Geo_D2')
         RAmdcVar = Geo_D2(I1,I2,I3)
         RETURN
      CASE('Geo_D3')
         RAmdcVar = Geo_D3(I1,I2,I3)
         RETURN
      CASE('Cut_dx')
         RAmdcVar = Cut_dx(I1,I2)
         RETURN
      CASE('Cut_dy')
         RAmdcVar = Cut_dy(I1,I2)
         RETURN
      CASE('Cut_Ws')
         RAmdcVar = Cut_Ws(I1,I2)
         RETURN
      CASE('Cut_Wl')
         RAmdcVar = Cut_Wl(I1,I2)
         RETURN
      CASE('Cut_Le')
         RAmdcVar = Cut_Le(I1,I2)
         RETURN
      CASE('Cut_Ex')
         RAmdcVar = Cut_Ex(I1,I2)
         RETURN
      CASE('Cut_An')
         RAmdcVar = Cut_An(I1,I2)
         RETURN
      CASE('STAX0')
         RAmdcVar = STAX0(I1,I2)
         RETURN
      CASE('STAPP')
         RAmdcVar = STAPP(I1,I2)
         RETURN
      CASE('STARR')
         RAmdcVar = STARR(I1,I2)
         RETURN
      CASE('STAEE')
         RAmdcVar = STAEE(I1,I2)
         RETURN
      CASE('STAPG')
         RAmdcVar = STAPG(I1,I2)
         RETURN
      CASE('STATT')
         RAmdcVar = STATT(I1,I2,I3)
         RETURN
      CASE('STAOO')
         RAmdcVar = STAOO(I1,I2,I3)
         RETURN
      CASE('XtgcAdd')
         RAmdcVar = XtgcAdd(I1,I2)
         RETURN
      CASE('XtgcStrp')
         RAmdcVar = XtgcStrp(I1,I2,I3)
         RETURN

      CASE('XtomoML1NYtub')
         RAmdcVar = XtomoML1NYtub(I1)
         RETURN
      CASE('XtomoML1NZtub')
         RAmdcVar = XtomoML1NZtub(I1)
         RETURN
      CASE('XtomoML1NDely')
         RAmdcVar = XtomoML1NDely(I1)
         RETURN
      CASE('XtomoML1NDelz')
         RAmdcVar = XtomoML1NDelz(I1)
         RETURN
      CASE('XtomoML1NDela')
         RAmdcVar = XtomoML1NDela(I1)
         RETURN
      CASE('XtomoML1NYpit')
         RAmdcVar = XtomoML1NYpit(I1)
         RETURN
      CASE('XtomoML1NZpit')
         RAmdcVar = XtomoML1NZpit(I1)
         RETURN
      CASE('XtomoML1PYtub')
         RAmdcVar = XtomoML1PYtub(I1)
         RETURN
      CASE('XtomoML1PZtub')
         RAmdcVar = XtomoML1PZtub(I1)
         RETURN
      CASE('XtomoML1PDely')
         RAmdcVar = XtomoML1PDely(I1)
         RETURN
      CASE('XtomoML1PDelz')
         RAmdcVar = XtomoML1PDelz(I1)
         RETURN
      CASE('XtomoML1PDela')
         RAmdcVar = XtomoML1PDela(I1)
         RETURN
      CASE('XtomoML1PYpit')
         RAmdcVar = XtomoML1PYpit(I1)
         RETURN
      CASE('XtomoML1PZpit')
         RAmdcVar = XtomoML1PZpit(I1)
         RETURN
      CASE('XtomoML2NYtub')
         RAmdcVar = XtomoML2NYtub(I1)
         RETURN
      CASE('XtomoML2NZtub')
         RAmdcVar = XtomoML2NZtub(I1)
         RETURN
      CASE('XtomoML2NDely')
         RAmdcVar = XtomoML2NDely(I1)
         RETURN
      CASE('XtomoML2NDelz')
         RAmdcVar = XtomoML2NDelz(I1)
         RETURN
      CASE('XtomoML2NDela')
         RAmdcVar = XtomoML2NDela(I1)
         RETURN
      CASE('XtomoML2NYpit')
         RAmdcVar = XtomoML2NYpit(I1)
         RETURN
      CASE('XtomoML2NZpit')
         RAmdcVar = XtomoML2NZpit(I1)
         RETURN
      CASE('XtomoML2PYtub')
         RAmdcVar = XtomoML2PYtub(I1)
         RETURN
      CASE('XtomoML2PZtub')
         RAmdcVar = XtomoML2PZtub(I1)
         RETURN
      CASE('XtomoML2PDely')
         RAmdcVar = XtomoML2PDely(I1)
         RETURN
      CASE('XtomoML2PDelz')
         RAmdcVar = XtomoML2PDelz(I1)
         RETURN
      CASE('XtomoML2PDela')
         RAmdcVar = XtomoML2PDela(I1)
         RETURN
      CASE('XtomoML2PYpit')
         RAmdcVar = XtomoML2PYpit(I1)
         RETURN
      CASE('XtomoML2PZpit')
         RAmdcVar = XtomoML2PZpit(I1)
         RETURN

      END SELECT

      PRINT 1000
      PRINT 1002,TRIM(Name),I1,I2,I3
1000  FORMAT(/' AmdcSimrec function RAmdcVar was called with ')
1002  FORMAT('the following arguments :   ',A,3I6,'  =====>  STOP !')
!
 END FUNCTION RAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 INTEGER FUNCTION IAmdcVar( Name, I1,I2,I3 )
 IMPLICIT NONE
 CHARACTER (len=*), INTENT(IN)  :: Name
 INTEGER,           INTENT(IN)  :: I1,I2,I3
!--------------------------------------------------------------------
! 
! Return the values of Amdcsimrec INTEGER parameters and variables
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"
#include "AmdcStand/amdcsim_stfunc.inc"

      SELECT CASE( Name )
      CASE('I_AMDC_VERSION')
         IAmdcVar = I_AMDC_VERSION
         RETURN
      CASE('IVERSION_AMDC')
         IAmdcVar = IVERSION_AMDC
         RETURN
      CASE('NBadjust')
         IAmdcVar = NBadjust
         RETURN
      CASE('NBdwnlay')
         IAmdcVar = NBdwnlay
         RETURN
      CASE('NBdeform')
         IAmdcVar = NBdeform
         RETURN
      CASE('MTEC')
         IAmdcVar = MTEC
         RETURN
      CASE('MSTA')
         IAmdcVar = MSTA
         RETURN
      CASE('MTYP')
         IAmdcVar = MTYP
         RETURN
      CASE('MGEO')
         IAmdcVar = MGEO
         RETURN
      CASE('MPHI')
         IAmdcVar = 8
         RETURN
      CASE('MZZ')
         IAmdcVar = MZZ
         RETURN
      CASE('Mstrp')
         IAmdcVar = Mstrp
         RETURN
      CASE('Mgang')
         IAmdcVar = Mgang
         RETURN
      CASE('IGEO')
         IAmdcVar = IGEO(I1,I2,I3)
         RETURN
      CASE('ICUT')
         IAmdcVar = ICUT(I1,I2,I3)
         RETURN
      CASE('IBAREL')
         IAmdcVar = IBAREL(I1,I2,I3)
         RETURN
      CASE('NOBJ')
         IAmdcVar = NOBJ(I1,I2)
         RETURN
      CASE('ITEC')
         IAmdcVar = ITEC(I1,I2,I3)
         RETURN
      CASE('ISTA')
         IAmdcVar = ISTA(I1,I2,I3)
         RETURN
      CASE('ISPLIX')
         IAmdcVar = ISPLIX(I1,I2,I3)
         RETURN
      CASE('ISPLIY')
         IAmdcVar = ISPLIY(I1,I2,I3)
         RETURN
      CASE('ISHAPE')
         IAmdcVar = ISHAPE(I1,I2,I3)
         RETURN
      CASE('IOBCUT')
         IAmdcVar = IOBCUT(I1,I2)
         RETURN
      CASE('NOBCUT')
         IAmdcVar = NOBCUT(I1)
         RETURN
      CASE('INOCUT')
         IAmdcVar = INOCUT(I1,I2,I3)
         RETURN
      CASE('NLAZ')
         IAmdcVar = NLAZ(I1,I2)
         RETURN
      CASE('NLAS')
         IAmdcVar = NLAS(I1,I2)
         RETURN
      CASE('ISTAMA')
         IAmdcVar = ISTAMA(I1,I2,I3)
         RETURN
      CASE('IFORMA')
         IAmdcVar = IFORMA(I1,I2)
         RETURN
      CASE('IVALIDSTA')
         IAmdcVar = IVALIDSTA(I1,I2)
         RETURN
      CASE('NtgcAdd')
         IAmdcVar = NtgcAdd(I1,I2)
         RETURN
      CASE('NtgcGang')
         IAmdcVar = NtgcGang(I1,I2,I3)
         RETURN
         
      CASE('HARDENTRIES')
         IAmdcVar = HARDENTRIES
         RETURN
      CASE('SOFTOCTANT')
         IAmdcVar = SOFTOCTANT(I1)
         RETURN
      CASE('SOFTIZ')
         IAmdcVar = SOFTIZ(I1)
         RETURN
      CASE('IentryHardSoft')
         IAmdcVar = IentryHardSoft(I1,I2,I3)
         RETURN
         
      CASE('XTOMOENTRIES')
         IAmdcVar = XTOMOENTRIES 
         RETURN
      CASE('XtomoSiteId')
         IAmdcVar = XtomoSiteId(I1)
         RETURN
      CASE('XtomoTime')
         IAmdcVar = XtomoTime(I1)
         RETURN
      CASE('XtomoPassed')
         IAmdcVar = XtomoPassed(I1)
         RETURN
      CASE('XtomoNberTube1')
         IAmdcVar = XtomoNberTube1(I1)
         RETURN
      CASE('XtomoNberTube2')
         IAmdcVar = XtomoNberTube2(I1)
         RETURN
      CASE('XtomoNberML')
         IAmdcVar = XtomoNberML(I1)
         RETURN
      CASE('XtomoNberLayer')
         IAmdcVar = XtomoNberLayer(I1)
         RETURN
      CASE('IentryXtomo')
         IAmdcVar = IentryXtomo(I1,I2,I3) 
         RETURN
         
      END SELECT

      PRINT 1000
      PRINT 1002,TRIM(Name),I1,I2,I3
1000  FORMAT(/' AmdcSimrec function IAmdcVar was called with ')
1002  FORMAT('the following arguments :   ',A,3I6,'  =====>  STOP !')
      STOP
!
 END FUNCTION IAmdcVar
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE CAmdcVar( Name, I1,I2,I3, CharVar )
 IMPLICIT NONE
 CHARACTER (len=*),  INTENT(IN)  :: Name
 INTEGER,            INTENT(IN)  :: I1,I2,I3
 CHARACTER (len=32), INTENT(OUT) :: CharVar
!--------------------------------------------------------------------
! 
! Return the values of AmdcSimrec CHARACTER parameters and variables
!
!--------------------------------------------------------------------
#include "AmdcStand/amdcsim_com.inc"

      CharVar = ''

      SELECT CASE( Name )
      CASE('CARTYP')
         CharVar = CARTYP(I1)
         RETURN
      CASE('CARTEC')
         CharVar = CARTEC(I1)
         RETURN
      CASE('C_AMDC_NAME')
         CharVar = C_AMDC_NAME
         RETURN
      CASE('HARDNAME')
         CharVar = HARDNAME(I1)
         RETURN
      CASE('SOFTNAME')
         CharVar = SOFTNAME(I1)
         RETURN
         
      CASE('XtomoChberName')
         CharVar = XtomoChberName(I1)
         RETURN
      CASE('XtomoSite')
         CharVar = XtomoSite(I1)
         RETURN
      CASE('XtomoSide')
         CharVar = XtomoSide(I1)
         RETURN
         
      END SELECT


      PRINT 1000
      PRINT 1002,TRIM(Name),I1,I2,I3
1000  FORMAT(/' AmdcSimrec function CAmdcVar was called with ')
1002  FORMAT('the following arguments :   ',A,3I6,'  =====>  STOP !')
      STOP
!
 END SUBROUTINE CAmdcVar
!
