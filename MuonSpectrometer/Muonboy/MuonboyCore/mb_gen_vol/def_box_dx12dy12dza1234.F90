!
 SUBROUTINE DEF_BOX_DX12DY12DZA1234( Name, Jcolor, XDX )
 USE M_MB_General_CONSTANTS
 IMPLICIT NONE
!
!>> Generalised 'TRD' :
!>>
!>>              1    2    3    4   5     6      7     8    9
!>> XDX(.)  =   xm   xp   ym   yp   z   theta   phi   a1   a2
!>>
!>>
!>> Face at z = -z/2  :     ^ y
!>>                         .
!>>                         .
!>>                         .
!>>                         .
!>>     ^  ****************************************
!>>     |  *    .           .                    *
!>>     |   *   .           .           .       *
!>>     |   * a1.           .           .      *
!>>     |    *--.           .           . a2  *
!>>     |    *  .           .           .----*
!>>     |     * .           .           .   *
!>>     |     * .           .           .  *
!>>     |      *.           .           . *
!>>     |      *.           .           .*                x
!>>  ym |       *<----------O---------->* . . . . . . . . >
!>>     |       *          xm          *
!>>     |        *                    *
!>>     |        *                   *
!>>     |         *                 *
!>>     |         *                *
!>>     |          *              *
!>>     |          *             *
!>>     |           *           *
!>>     |           *          *
!>>     v            **********
!>>
!>>
!>>  Face at z = +z/2  :        ^ y
!>>                             .
!>>                             .
!>>                             .
!>>                             .
!>>     ^   *********************************************
!>>     |   * a1.               .               .      *
!>>     |    *--.               .               . a2  *
!>>     |    *  .               .               .----*
!>>     |     * .               .               .   *
!>>     |     * .               .               .  *
!>>     |      *.               .               . *
!>>     |      *.               .               .*            x
!>>  yp |       *<--------------O-------------->* . . . . . . >
!>>     |       *              xp              *
!>>     |        *                            *
!>>     |        *                           *
!>>     |         *                         *
!>>     |         *                        *
!>>     |          *                     *
!>>     v           *********************
!>>
!>>
!>>  theta, phi = polar and azimuthal angles of the line connecting the center (O)
!>>               of the face at z = -z/2 to the one of the face at z = +z/2 .
!>>
 CHARACTER (len=*) :: Name
 INTEGER           :: Jcolor
 REAL(8)           :: XDX(*)
 INTEGER :: I,J_Atlas_Ref_Syst
 REAL(8) :: XM1,XM2,XM3,XM4,XP1,XP2,XP3,XP4,DX,DY
 REAL(8) :: YM1,YM2,YM3,YM4,YP1,YP2,YP3,YP4,ZM,ZP
!
   DO I=1,5
     IF( ABS(XDX(I)) < Eps ) RETURN
   ENDDO
   DX  =  XDX(5) * TAN(XDX(6)*PiS180) * COS(XDX(7)*PiS180)
   DY  =  XDX(5) * TAN(XDX(6)*PiS180) * SIN(XDX(7)*PiS180)
   XM1 = ( - XDX(1) + XDX(3)*TAN(XDX(8)*PiS180) - DX ) / 2.D0
   XM2 = (   XDX(1) + XDX(3)*TAN(XDX(9)*PiS180) - DX ) / 2.D0
   XM3 = (   XDX(1) - XDX(3)*TAN(XDX(9)*PiS180) - DX ) / 2.D0
   XM4 = ( - XDX(1) - XDX(3)*TAN(XDX(8)*PiS180) - DX ) / 2.D0
   YM1 = (          - XDX(3)                    - DY ) / 2.D0
   YM2 = (          - XDX(3)                    - DY ) / 2.D0
   YM3 = (            XDX(3)                    - DY ) / 2.D0
   YM4 = (            XDX(3)                    - DY ) / 2.D0
   XP1 = ( - XDX(2) + XDX(4)*TAN(XDX(8)*PiS180) + DX ) / 2.D0
   XP2 = (   XDX(2) + XDX(4)*TAN(XDX(9)*PiS180) + DX ) / 2.D0
   XP3 = (   XDX(2) - XDX(4)*TAN(XDX(9)*PiS180) + DX ) / 2.D0
   XP4 = ( - XDX(2) - XDX(4)*TAN(XDX(8)*PiS180) + DX ) / 2.D0
   YP1 = (          - XDX(4)                    + DY ) / 2.D0
   YP2 = (          - XDX(4)                    + DY ) / 2.D0
   YP3 = (            XDX(4)                    + DY ) / 2.D0
   YP4 = (            XDX(4)                    + DY ) / 2.D0
   ZM  = -XDX(5) / 2.D0
   ZP  =  XDX(5) / 2.D0
!
   CALL NEWVOL(Name,1)
   CALL DEF_CENVOL(Zero,Zero,Zero)
   CALL Get_Atlas_Ref_Syst( J_Atlas_Ref_Syst )
   IF( J_Atlas_Ref_Syst >= 1 ) THEN
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XM1,XM2,XM3,XM4, -ZM,-ZM,-ZM,-ZM, YM1,YM2,YM3,YM4 )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XP1,XP2,XP3,XP4, -ZP,-ZP,-ZP,-ZP, YP1,YP2,YP3,YP4 )
   ELSE
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XM1,XM2,XM3,XM4, YM1,YM2,YM3,YM4, ZM,ZM,ZM,ZM )
     CALL DEFPL4( Zero, Jcolor, 1,1,1,1, XP1,XP2,XP3,XP4, YP1,YP2,YP3,YP4, ZP,ZP,ZP,ZP )
   ENDIF
   CALL CLOSE(Jcolor,1)
   CALL ENDVOL_SUV
!
 END SUBROUTINE DEF_BOX_DX12DY12DZA1234
!
