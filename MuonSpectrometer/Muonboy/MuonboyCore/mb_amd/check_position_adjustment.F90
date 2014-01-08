!
 SUBROUTINE check_position_adjustment
 USE M_MB_Control
 USE M_MB_MuGeom
 USE M_MB_EventInfo, ONLY : ievt
 IMPLICIT NONE
 INTEGER :: IC, INOF, INOZ, IGEOM, JTYP, JFF, JZZ, JOB
 INTEGER :: Itran, IMDT, L, K, Jcsc, Ip, Nstr, Istr
 REAL(8) :: AA0, ZZ0, TT0, SS0, SIGT, DCF, DSF, DCA, DSA
 REAL(8) :: ZLO, szflag, SSamdc, TTamdc, ZZamdc, SSmboy, TTmboy, ZZmboy
 REAL(8) :: XYZabs(3,5), SZTloc(3,5)
 REAL(8) :: SS, ZZ, TT, X, Y, Z, SScsc, ZZcsc, TTcsc
 REAL(8), PARAMETER :: ecar=0.002d0 ! allowed difference between amdc and mboy in mm
 CHARACTER(3) :: Ctyp
 INTEGER :: intpos
!>
!> Check compatibility of geometry between Amdc and Mboy
!>
   WRITE(MBChkCsc,'(/," check positions for CSC with tolerance of ",F6.4," mm --- from event ",I9)') ecar,ievt
   DO IC=NCOUCH+1,NCOCSC
     DO INOF=1,8
       DO INOZ=1,NZM
         IGEOM = IGEOMA(IC,INOF,INOZ)
         IF( IGEOM <= 0 ) CYCLE
         Jcsc = intpos(IGEOM,IGEOMCSC,NChaCSC)
         AA0   = GCECHAV(IGEOM)
         ZZ0   = ZCECHAV(IGEOM)
         ZLO   = ZLOCHAV(IGEOM)
         TT0   = TCECHAV(IGEOM)
         SS0   = 0.25d0 * ( SPICHAV(IGEOM) + SNICHAV(IGEOM) + SPACHAV(IGEOM) + SNACHAV(IGEOM) ) 
         DCF   = COSCHAV(IGEOM)
         DSF   = SINCHAV(IGEOM)
         DCA   = COS(AA0)
         DSA   = SIN(AA0)
         Itran = ItransCHA(IGEOM)
         SIGT  = SIGN( 1.d0, TT0)
         CALL IDECI( ISTTYPV(IGEOM), JTYP,JFF,JZZ,JOB )
         CALL FGetStationType(JTYP, Ctyp)
         IMDT = ISTMDTV(IGEOM)
         DO Ip=1,NLTUB(IMDT)
           DO L=1,2
             IF( L == 1 ) THEN  ! eta strips
               szflag = 1.d0
               Nstr = ( ZLOCHAV(IGEOM) - 2.*OLTUB(IMDT,Ip,1) + PASTUB(IMDT) ) * PASTUB_inv(IMDT)
             ELSE
               szflag = -1.d0
               Nstr = NINT( (2.*ABS(OLTUB(IMDT,Ip,2))-RAYTUB(IMDT)) / RAYTUB(IMDT) )
             ENDIF
             DO K=1,3
               IF( K == 1 ) THEN
                 Istr = 1
               ELSE IF( K == 2 ) THEN
                 Istr = ( 1 + Nstr ) / 2
               ELSE
                 Istr = Nstr
               ENDIF
               CALL FGetStripPositionFromCSCindex(Ctyp,Jff,Jzz,Job,Ip,Istr,szflag, XYZabs, SZTloc )
               SSamdc =   DCF * XYZabs(2,5) - DSF * XYZabs(1,5)
               TTamdc =   DCF * XYZabs(1,5) + DSF * XYZabs(2,5) 
               ZZamdc = - DSA * XYZabs(3,5) + DCA * TTamdc
               TTamdc =   DCA * XYZabs(3,5) + DSA * TTamdc 
               IF( L == 1 ) THEN
                 SSmboy = SS0
                 ZZmboy = ZZ0 - 0.5*ZLO + (OLTUB(IMDT,Ip,1)+(Istr-1)*PASTUB(IMDT))
                 TTmboy = TT0 + SIGT*TLTUB(IMDT,Ip,1)
               ELSE               ! phi strips
                 SSmboy = OLTUB(IMDT,Ip,2) + Istr*RAYTUB(IMDT)
                 ZZmboy = SZTloc(2,5) / 10.d0    ! definition is ambiguous because of cutouts
                 TTmboy = TT0 + SIGT*TLTUB(IMDT,Ip,2)
               ENDIF
               CALL Pos_Csc_from_Layer(Jcsc,Ip,SSmboy,ZZmboy,TTmboy,SScsc,ZZcsc,TTcsc)
               CALL Pos_True_from_Ideal(Itran,SScsc,ZZcsc,TTcsc,SS,ZZ,TT)
               X = DCF * ( DCA * ZZ + DSA * TT ) - DSF * SS
               Y = DSF * ( DCA * ZZ + DSA * TT ) + DCF * SS
               Z =       - DSA * ZZ + DCA * TT
               IF( abs( XYZabs(1,5) - 10.d0*X ) > ecar .or.   &
                   abs( XYZabs(2,5) - 10.d0*Y ) > ecar .or.   &
                   abs( XYZabs(3,5) - 10.d0*Z ) > ecar ) then
                 WRITE(MBChkCsc,*) ' ------------------------------------------------- '
                 WRITE(MBChkCsc,*) ' Ctyp Jff Jzz Job Ip Istr szflag ',Ctyp,Jff,Jzz,Job,Ip,Istr,szflag
                 WRITE(MBChkCsc,*) ' loc positions szt : amdc ',SZTloc(1:3,5),         &
                                                   ' <-> mboy ',10.d0*SSmboy,10.d0*ZZmboy,10.d0*TTmboy
                 WRITE(MBChkCsc,*) ' abs positions szt : amdc ',SSamdc,ZZamdc,TTamdc,  &   
                                                   ' <-> mboy ',10.d0*SS,10.d0*ZZ,10.d0*TT
                 WRITE(MBChkCsc,*) ' abs positions xyz : amdc ',XYZabs(1:3,5),         &   
                                                   ' <-> mboy ',10.d0*X,10.d0*Y,10.d0*Z
               ENDIF
             ENDDO
           ENDDO
         ENDDO
       ENDDO
     ENDDO
   ENDDO
!     
 END SUBROUTINE check_position_adjustment
!
