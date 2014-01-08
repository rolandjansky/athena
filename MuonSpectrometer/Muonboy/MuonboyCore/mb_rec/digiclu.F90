!
!> Put the cluster information in a form suitable for Muonboy,
!> and store it in Muonboy commons by calling atubhi.F90
!
 SUBROUTINE DigiClu(Ident,Jtyp,Jff,Jzz,Job,Jsl,SZflag,XXtrue,YYtrue,ZZtrue,Reso,JJJ,DepositedCharge,Time,Status,TimeStatus)
 USE M_MB_Control
 USE M_MB_Digis
 USE M_MB_MuGeom
 IMPLICIT NONE
 INTEGER, INTENT(IN) :: Ident, Jtyp, Jff, Jzz, Job, Jsl
 REAL(8), INTENT(IN) :: SZflag
 REAL(8), INTENT(IN) :: XXtrue, YYtrue, ZZtrue !< global coordinates, in cm
 REAL(8), INTENT(IN) :: Reso
 INTEGER, INTENT(IN) :: JJJ
 REAL(8), INTENT(IN) :: DepositedCharge
 REAL(8), INTENT(IN) :: Time
 INTEGER, INTENT(IN) :: Status,TimeStatus
 INTEGER, SAVE :: NbWarn = 0, MaxWarn = 200
 CHARACTER(3) :: NameTYP, NameTEC
 INTEGER :: IC,INOF,INOZ, ICT, IGEOM, IMDT, IP, ISC
 INTEGER :: IW, Iquali, i, Itran, Jcsc
 REAL(8) :: SStrue, TTtrue, ZTtrue, TZtrue
 REAL(8) :: SSidea, ZTidea, TZidea, TZdigi
 REAL(8) :: SSlayer, ZZlayer, TTlayer
 REAL(8) :: AA0, TT0, DCF, DSF, DSA, DCA, SIGT
 REAL(8) :: Resc, Xflag, ResMax
 INTEGER :: intpos
!
    CALL FGetStationType(Jtyp, NameTYP)
    CALL FGetObjectTech(Jtyp,Jff,Jzz,Job, NameTEC)
!- Get the MUONBOY MDT indexes from the AMDC_SIMREC ones
    CALL GETIND(Jtyp,Jff,Jzz,Job, IC,INOF,INOZ, ICT)
!
    IF( IC+INOF+INOZ <= 0 .OR. IC <= NCOUCH .OR. NameTEC /= 'CSC' ) THEN
      WRITE(*,7000) NameTYP,Jff,Jzz,Job,NameTEC
7000  FORMAT(/' In Muonboy/DigiClu : The object Typ,Jff,Jzz,Job,Tec =  ',A3,3I4,2X,A3,   &
              '  does not correspond to a Muonboy CSC multilayer  ===>  STOP !!!',/)
      RETURN
    ENDIF
    IGEOM = IGEOMA(IC,INOF,INOZ)
    IF( IGEOM <= 0 ) THEN
      WRITE(*,7000) NameTYP,Jff,Jzz,Job,NameTEC
      RETURN
    ENDIF
    IF( isOffChav(IGEOM) ) THEN
      WRITE(*,*) ' Muonboy/digiclu ERROR of CSC station Off '  &
             ,IC,INOF,INOZ,' which has clusters !!!'
    ENDIF
    IMDT  = ISTMDTV(IGEOM)
    IP    = Jsl
    IF( IP < 1 .OR. IP > NLTUB(IMDT) ) THEN
      PRINT 7010,NameTYP,Jff,Jzz,Job,NameTEC,Jsl,NLTUB(IMDT)
7010  FORMAT(/' In Muonboy/DigiClu : For  Typ,Jff,Jzz,Job,Tec =  ',A3,3I4,2X,A3,             &
              '  the index  Jsl =',I2,'  is outside range :  1  to',I3,'  ===>  STOP !!!',/)
      RETURN
    ENDIF
!
    IF( ndeadChan(IGEOM) > 0 ) THEN
      DO i=ideadChan(IGEOM)+1,ideadChan(IGEOM)+ndeadChan(IGEOM)
        IF( ldeadChan(i) == IP ) THEN
          WRITE(*,*) ' Muonboy/digiclu problem of dead CSC layer ',NameTyp,Jff,Jzz,Jsl,' which has hits !!!'
          EXIT
        ENDIF
      ENDDO
    ENDIF
!
!- Get some Muonboy variables
    AA0   = GCECHAV(IGEOM)
    TT0   = TCECHAV(IGEOM)
    Itran = ItransCHA(IGEOM)
    DCF   = COSCHAV(IGEOM)
    DSF   = SINCHAV(IGEOM)
    DCA   = COS(AA0)
    DSA   = SIN(AA0)
!
!- Get the input cluster position both after (true) and before (idea) displacements+deformations
    SStrue =   DCF * YYtrue - DSF * XXtrue
    TTtrue =   DCF * XXtrue + DSF * YYtrue
    ZTtrue = - DSA * ZZtrue + DCA * TTtrue
    TZtrue =   DCA * ZZtrue + DSA * TTtrue
    CALL Pos_Ideal_from_True(Itran,SStrue,ZTtrue,TZtrue,SSidea,ZTidea,TZidea)
!- and taking into account possible layer-to-layer misalignement
    Jcsc = intpos(IGEOM,IGEOMCSC,NChaCSC)
    CALL Pos_Layer_from_Csc(Jcsc,Ip,SSidea,ZTidea,TZidea,SSlayer,ZZlayer,TTlayer)
!
    IF( MBChkDig > 0 ) THEN
      IF( NbWarn < MaxWarn ) THEN
        SIGT   = SIGN( 1.D0, TT0 )
        TZdigi = TT0 + 0.5d0*SIGT*(TLTUB(IMDT,IP,1)+TLTUB(IMDT,IP,2))
        IF( abs(TZdigi-TTlayer) > 0.0003d0 ) THEN
          NbWarn = NbWarn + 1
          WRITE(*,7020) NameTYP,Jff,Jzz,Job,Jsl,SZflag,10.d0*TZdigi,10.d0*TTlayer
7020      FORMAT(/,'==>  CSC cluster Typ  Jff Jzz Job Jsl SZflag'   &
                  ,/,17X,A3,4I3,F6.1,/,' Tmboy Tmgm  ',2F12.3)
        ENDIF
      ENDIF
    ENDIF
!
!- One stores the digi in module M_MB_Digis
    IF( IWCHA(IC,INOF,INOZ) >= 1 ) THEN
!- Here, the multilayer was already hit
      IW = IWCHA(IC,INOF,INOZ)
    ELSE
!- Here, on the contrary, the multilayer was not hit up to now
      IF( NCHAHI(IC) >= NCHAMA ) THEN
        PRINT 7030,IC,NCHAMA
7030    FORMAT(/' WARNING : The number of Muonboy hit chambers in m.l.',I3,  &
                ' is limited to the parameter NCHAMA =',I8/)
        RETURN
      ENDIF
      NCHAHI(IC) = NCHAHI(IC) + 1
      IW = NCHAHI(IC)
      IFCHA(IC,IW) = INOF
      IZCHA(IC,IW) = INOZ
      IWCHA(IC,INOF,INOZ) = IW
    ENDIF
!
    Iquali = Status
    IF( SZflag > 0.d0 ) THEN   ! Z cluster
      IF( ExternalClust == 1 .OR. ExternalClust == -1 ) THEN
        Xflag = -1.D0
      ELSE
        Xflag = 111.D0
      ENDIF
    ELSE                       ! S cluster
      IF( ExternalClust == 1 .OR. ExternalClust == -2 ) THEN
        Xflag = -11.D0
      ELSE
        Xflag = 101.D0
      ENDIF
    ENDIF
    Resc = MAX( Reso, 0.0040d0 )   ! minimum error of 40 µm
    IF( Iquali == 0 .OR. Iquali == 10 ) THEN  ! increase error to take layer alignment into account
      IF( SZflag > 0.d0 ) THEN
        ISC = ( IC + 1 ) / 2
        Resc = SQRT( Resc**2 + AligML(ISC)**2 )
      ENDIF
    ELSE IF( Iquali == 4 ) THEN  ! increase error of clusters too narrow(4)
      IF( SZflag > 0.d0 ) THEN
        Resc = Resc + PASTUB(IMDT)/sqrt(12.d0)
      ELSE
        Resc = Resc + RAYTUB(IMDT)/sqrt(12.d0)
      ENDIF
    ENDIF
    IF( SZflag > 0.d0 ) THEN
      ResMax = ZLOCHAV(IGEOM)/sqrt(12.d0)
      IF( Resc > ResMax ) THEN
        WRITE(*,*) ' ERROR Muonboy/digiclu : eta-cluster error =',Reso
        Resc = ResMax
      ENDIF
    ELSE
      ResMax = (SPACHAV(IGEOM)-SNACHAV(IGEOM))/sqrt(12.d0)
      IF( Resc > ResMax ) THEN
        WRITE(*,*) ' ERROR Muonboy/digiclu : phi-cluster error =',Reso
        Resc = ResMax      
      ENDIF
    ENDIF
    CALL ATUBHI(Ident,IC,IW,IP,ZZlayer,Resc,Xflag,SSlayer,JJJ,Iquali)
!
 END SUBROUTINE DigiClu
!
