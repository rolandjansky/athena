!
!> \author SamuSog
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE AmDeWriChar(Chain,ChSize,CharIn,Itotal)
!
      CHARACTER (len=*)    :: Chain
      Integer              :: ChSize
      CHARACTER (len=*), INTENT(IN) :: CharIn
      Integer, INTENT(IN)  :: Itotal
!
      Character(100) :: FormatChain
      Character(100) :: FormatHead
      character(200) :: CharOut
      character(200) :: ChaSpac
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      FormatChain="(A"
      WRITE (FormatHead,FMT=7000) Itotal
      FormatChain=TRIM(FormatChain)//Trim(ADJUSTL(FormatHead))
      FormatChain=TRIM(FormatChain)//")"
7000  format(I10)    
      WRITE (CharOut,FMT=FormatChain) CharIn
!
      Lneed=ChSize - LEN_TRIM(Chain)
      if (Lneed.gt.200) then
        print *,'WARNING Lneed gt 200'
        Lneed=200
      endif
      if (Lneed.ge.1) then
        do i=1,Lneed
          ChaSpac(i:i)=' '
        enddo
        Chain = TRIM(Chain)//ChaSpac(1:Lneed)//CharOut(1:Itotal)
      else
        Chain = TRIM(Chain)//CharOut(1:Itotal)
      endif
!
      ChSize = ChSize + Itotal
!
      END SUBROUTINE AmDeWriChar
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmDeWriDef(LunOut,JTYP,JGEO)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LunOut
 INTEGER, INTENT(IN) :: JTYP,JGEO
 CHARACTER(1000) :: Chain
 INTEGER         :: ChSize
 INTEGER         :: JOB, JTEC, JCUT, ITOTCU
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
#include "AmdcStand/amdcsim_stfunc.inc"
!
      IF(JGEO.LE.0) return
!
      WRITE(LunOut,3100) CARTYP(JTYP),JGEO,NOBJ(JTYP,JGEO)
3100  FORMAT('D ',A3,I2,2X,I2)
!
      DO JOB=1,NOBJ(JTYP,JGEO)
        JTEC = ITEC(JTYP,JGEO,JOB)
        ChSize=0
        Chain=''
        call AmDeWriReal(Chain,ChSize,Geo_dx(JTYP,JGEO,JOB) ,7,3) 
        call AmDeWriReal(Chain,ChSize,Geo_dy(JTYP,JGEO,JOB) ,7,3)
        call AmDeWriReal(Chain,ChSize,Geo_dz(JTYP,JGEO,JOB) ,7,3)  
        call AmDeWriInt (Chain,ChSize,JOB                   ,2  )
        call AmDeWriSpac(Chain,ChSize                       ,1  )  
        call AmDeWriChar(Chain,ChSize,CARTEC(JTEC)          ,3  )  
        call AmDeWriInt (Chain,ChSize,ISTA(JTYP,JGEO,JOB)   ,2  ) 
        call AmDeWriInt (Chain,ChSize,ISPLIX(JTYP,JGEO,JOB) ,2  )
        call AmDeWriInt (Chain,ChSize,ISPLIY(JTYP,JGEO,JOB) ,2  ) 
        call AmDeWriInt (Chain,ChSize,ISHAPE(JTYP,JGEO,JOB) ,2  )
        call AmDeWriReal(Chain,ChSize,Geo_Ws(JTYP,JGEO,JOB) ,8,3) 
        call AmDeWriReal(Chain,ChSize,Geo_Wl(JTYP,JGEO,JOB) ,8,3)
        call AmDeWriReal(Chain,ChSize,Geo_Le(JTYP,JGEO,JOB) ,8,3) 
        call AmDeWriReal(Chain,ChSize,Geo_Ex(JTYP,JGEO,JOB) ,7,3)
        call AmDeWriReal(Chain,ChSize,Geo_D1(JTYP,JGEO,JOB) ,7,3) 
        call AmDeWriReal(Chain,ChSize,Geo_D2(JTYP,JGEO,JOB) ,7,3)
        call AmDeWriReal(Chain,ChSize,Geo_D3(JTYP,JGEO,JOB) ,7,3)
        call AmDeWriteOiutChain(LunOut,Chain,ChSize)
      enddo
!
      DO JCUT=1,4
       IF(INOCUT(JTYP,JGEO,JCUT).GE.1) THEN
!
        ITOTCU = INOCUT(JTYP,JGEO,JCUT)
        WRITE(LunOut,6200) CARTYP(JTYP),JGEO,JCUT,NOBCUT(ITOTCU)
6200    FORMAT('H ',A3,3I2)
!
        DO JOB=1,NOBCUT(ITOTCU)
          ChSize=0
          Chain=''
          call AmDeWriReal(Chain,ChSize,Cut_dx(ITOTCU,JOB) ,7,3)
          call AmDeWriReal(Chain,ChSize,Cut_dy(ITOTCU,JOB) ,7,3)
          call AmDeWriSpac(Chain,ChSize                    ,7  )  
          call AmDeWriInt (Chain,ChSize,IOBCUT(ITOTCU,JOB) ,2  )
          call AmDeWriSpac(Chain,ChSize                    ,12 )  
          call AmDeWriReal(Chain,ChSize,Cut_Ws(ITOTCU,JOB) ,8,3) 
          call AmDeWriReal(Chain,ChSize,Cut_Wl(ITOTCU,JOB) ,8,3)
          call AmDeWriReal(Chain,ChSize,Cut_Le(ITOTCU,JOB) ,8,3)
          call AmDeWriReal(Chain,ChSize,Cut_Ex(ITOTCU,JOB) ,7,3) 
          call AmDeWriReal(Chain,ChSize,Cut_An(ITOTCU,JOB) ,7,3)
          call AmDeWriteOiutChain(LunOut,Chain,ChSize)
        enddo
!
       ENDIF
      enddo
!
 END SUBROUTINE AmDeWriDef
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmDeWriInt(Chain,ChSize,IntIn,Itotal)
 IMPLICIT NONE
 CHARACTER (len=*)    :: Chain
 Integer              :: ChSize
 INTEGER,  INTENT(IN) :: IntIn
 Integer, INTENT(IN)  :: Itotal
 Character(100) :: FormatChain
 Character(100) :: FormatHead
 character(200) :: CharOut
 character(200) :: ChaSpac
 INTEGER :: Lneed, i
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      FormatChain="(I"
      WRITE (FormatHead,FMT=7000) Itotal
      FormatChain=TRIM(FormatChain)//Trim(ADJUSTL(FormatHead))
      FormatChain=TRIM(FormatChain)//")"
7000  format(I10)    
      WRITE (CharOut,FMT=FormatChain) IntIn
!
      Lneed=ChSize - LEN_TRIM(Chain)
      if (Lneed.gt.200) then
        print *,'WARNING Lneed gt 200'
        Lneed=200
      endif
      if (Lneed.ge.1) then
        do i=1,Lneed
          ChaSpac(i:i)=' '
        enddo
        Chain = TRIM(Chain)//ChaSpac(1:Lneed)//CharOut(1:Itotal)
      else
        Chain = TRIM(Chain)//CharOut(1:Itotal)
      endif
!
      ChSize = ChSize + Itotal
!
 END SUBROUTINE AmDeWriInt
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      SUBROUTINE AmDeWriReal(Chain,ChSize,RealIn,Itotal,Ipreci)
!
      CHARACTER (len=*)    :: Chain
      Integer              :: ChSize
      REAL(8), INTENT(IN)  :: RealIn
      Integer, INTENT(IN)  :: Itotal
      Integer, INTENT(IN)  :: Ipreci
!
      Character(100) :: FormatChain
      Character(100) :: FormatHead
      character(200) :: CharOut
      character(200) :: ChaSpac
      Integer IpreciLoc
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      IpreciLoc = Ipreci
      if (IpreciLoc.gt.Itotal) IpreciLoc=Itotal
!
10    continue
      FormatChain="(F"
      WRITE (FormatHead,FMT=7000) Itotal
      FormatChain=TRIM(FormatChain)//Trim(ADJUSTL(FormatHead))
      WRITE (FormatHead,FMT=7000) IpreciLoc
      FormatChain=TRIM(FormatChain)//"."//Trim(ADJUSTL(FormatHead))
      FormatChain=TRIM(FormatChain)//")"
7000  format(I10)    
      WRITE (CharOut,FMT=FormatChain) RealIn
      Iproblem = 0
      do i=1,Itotal
        if (Iproblem.eq.0) then
          if (CharOut(i:i).eq.'*') Iproblem=1
        endif
      enddo
      if (Iproblem.eq.1) then
        IpreciLoc=IpreciLoc-1
        if (IpreciLoc.ge.1) goto 10
      endif
!
      Lneed=ChSize - LEN_TRIM(Chain)
      if (Lneed.gt.200) then
        print *,'WARNING Lneed gt 200'
        Lneed=200
      endif
      if (Lneed.ge.1) then
        do i=1,Lneed
          ChaSpac(i:i)=' '
        enddo
        Chain = TRIM(Chain)//ChaSpac(1:Lneed)//CharOut(1:Itotal)
      else
        Chain = TRIM(Chain)//CharOut(1:Itotal)
      endif
!
      ChSize = ChSize + Itotal
!
      END SUBROUTINE AmDeWriReal
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmDeWriSpac(Chain,ChSize,NbSpace)
 IMPLICIT NONE
 CHARACTER (len=*)    :: Chain
 Integer              :: ChSize
 Integer, INTENT(IN)  :: NbSpace
 character(200) :: ChaSpac
 INTEGER        :: i
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      do i=1,NbSpace
        ChaSpac(i:i)=' '
      enddo
      Chain = TRIM(Chain)//ChaSpac(1:NbSpace)
!
      ChSize = ChSize + NbSpace
!
 END SUBROUTINE AmDeWriSpac
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmDeWriSta(LunOut,IPosLineFormat,                     &
                       JTYP,JFF,JZZ,                              &
                       PHI0_New,                                  &
                       Pos_Z_New   , Pos_R_New   , Pos_S_New,     &
                       Pos_Alfa_New, Pos_Beta_New, Pos_Gama_New)
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LunOut
 INTEGER, INTENT(IN) :: IPosLineFormat
 INTEGER, INTENT(IN) :: JTYP,JFF,JZZ
 REAL(8), INTENT(IN) :: PHI0_New
 REAL(8), INTENT(IN) :: Pos_Z_New   ,Pos_R_New   ,Pos_S_New
 REAL(8), INTENT(IN) :: Pos_Alfa_New,Pos_Beta_New,Pos_Gama_New
 CHARACTER(1000) :: Chain
 INTEGER         :: ChSize
 INTEGER         :: LFI(8)
 INTEGER         :: JGEO, JCUT 
 LOGICAL, SAVE   :: first=.true.
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      if (first) then
        first=.false.
        WRITE (LunOut,1006) 
        if (IPosLineFormat.eq.0) WRITE (LunOut,1000) 'P'
        if (IPosLineFormat.eq.1) WRITE (LunOut,1000) 'Q'
        WRITE (LunOut,1006) 
1000    format('*AmDeWriSta with lines ',A30)
1006    format('*')
      endif
!
      JGEO = IGEO_V(JTYP_F_Z(JTYP,JFF,JZZ))
      IF(JGEO.LE.0) return
!
      JCUT = ICUT_V(JTYP_F_Z(JTYP,JFF,JZZ))
      LFI(1:8) = 0
      LFI(JFF) = 1
      IF( IVERSION_AMDC > 6 ) LFI(JFF) = 2
!
      ChSize=0
      Chain=''
      if (IPosLineFormat.eq.0) call AmDeWriChar(Chain,ChSize,'P '         ,2  )
      if (IPosLineFormat.eq.1) call AmDeWriChar(Chain,ChSize,'Q '         ,2  )
!      
      call AmDeWriChar(Chain,ChSize,CARTYP(JTYP) ,3  )
      call AmDeWriInt (Chain,ChSize,JGEO         ,2  )
      call AmDeWriInt (Chain,ChSize,JCUT         ,2  )
      call AmDeWriSpac(Chain,ChSize              ,1  )  
      call AmDeWriInt (Chain,ChSize,LFI(1)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(2)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(3)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(4)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(5)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(6)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(7)       ,1  )
      call AmDeWriInt (Chain,ChSize,LFI(8)       ,1  )
      call AmDeWriInt (Chain,ChSize,JZZ          ,3  )
!      
      if (IPosLineFormat.eq.0) then
      call AmDeWriReal(Chain,ChSize,PHI0_New     ,9,2)
      call AmDeWriReal(Chain,ChSize,Pos_Z_New    ,9,3)
      call AmDeWriReal(Chain,ChSize,Pos_R_New    ,9,3)
      call AmDeWriReal(Chain,ChSize,Pos_S_New    ,8,3)
      call AmDeWriReal(Chain,ChSize,Pos_Alfa_New ,8,5)
      call AmDeWriReal(Chain,ChSize,Pos_Beta_New ,8,5) 
      call AmDeWriReal(Chain,ChSize,Pos_Gama_New ,8,5)
      endif
      if (IPosLineFormat.eq.1) then
      call AmDeWriReal(Chain,ChSize,PHI0_New     ,10,5)
      call AmDeWriReal(Chain,ChSize,Pos_Z_New    ,10,3)
      call AmDeWriReal(Chain,ChSize,Pos_R_New    ,10,3)
      call AmDeWriReal(Chain,ChSize,Pos_S_New    ,10,3)
      call AmDeWriReal(Chain,ChSize,Pos_Alfa_New ,10,5)
      call AmDeWriReal(Chain,ChSize,Pos_Beta_New ,10,5) 
      call AmDeWriReal(Chain,ChSize,Pos_Gama_New ,10,5)
      endif
!      
      call AmDeWriteOiutChain(LunOut,Chain,ChSize)
!
 END SUBROUTINE AmDeWriSta
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmDeWriteOiutChain(LunOut,Chain,ChSize)
 IMPLICIT NONE
 INTEGER, INTENT(IN)  :: LunOut
 CHARACTER (len=*)    :: Chain
 INTEGER              :: ChSize
 Character(100) :: FormatChain
 Character(100) :: FormatHead
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
      FormatChain="(A"
      WRITE (FormatHead,FMT=7000) ChSize
      FormatChain=TRIM(FormatChain)//Trim(ADJUSTL(FormatHead))
      FormatChain=TRIM(FormatChain)//")"
7000  format(I10)    
      WRITE (LunOut,FMT=FormatChain) Chain(1:ChSize)
!
 END SUBROUTINE AmDeWriteOiutChain
!            
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 SUBROUTINE AmRaWriteLineA(LunOut,                 &
                           JTYP,JFF,JZZ,           &
                           Ts_New,Tz_New,Tt_New,   &
                           Rs_New,Rz_New,Rt_New )
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: LunOut
 INTEGER, INTENT(IN) :: JTYP,JFF,JZZ
 REAL(8), INTENT(IN) :: Ts_New,Tz_New,Tt_New
 REAL(8), INTENT(IN) :: Rs_New,Rz_New,Rt_New
 CHARACTER(1000) :: Chain
 Integer         :: ChSize
 INTEGER :: JGEO, JOB
!--------------------------------------------------------------------
!
!--------------------------------------------------------------------
!
#include "AmdcStand/amdcsim_com.inc"
!
      JGEO = IGEO_V(JTYP_F_Z(JTYP,JFF,JZZ))
      IF(JGEO.LE.0) return
!
      JOB = 0
!
      ChSize=0
      Chain=''
      call AmDeWriChar(Chain,ChSize,'A '         ,2  )
      call AmDeWriChar(Chain,ChSize,CARTYP(JTYP) ,3  )
      call AmDeWriInt (Chain,ChSize,JFF          ,4  )
      call AmDeWriInt (Chain,ChSize,JZZ          ,4  )
      call AmDeWriInt (Chain,ChSize,JOB          ,4  )
      call AmDeWriReal(Chain,ChSize,Ts_New       ,10,3)
      call AmDeWriReal(Chain,ChSize,Tz_New       ,10,3)
      call AmDeWriReal(Chain,ChSize,Tt_New       ,10,3)
      call AmDeWriReal(Chain,ChSize,Rs_New       ,10,6)
      call AmDeWriReal(Chain,ChSize,Rz_New       ,10,6)
      call AmDeWriReal(Chain,ChSize,Rt_New       ,10,6)
      call AmDeWriteOiutChain(LunOut,Chain,ChSize)
!
 END SUBROUTINE AmRaWriteLineA
!            
