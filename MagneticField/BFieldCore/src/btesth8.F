*
*
*
*

* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE BTESTH8(  NCX,NCY,NCZ,  XCPOS,YCPOS,ZCPOS,
     +               TACBX,TACBY,TACBZ,  XYZ,II312, BXYZ)
*
*>> Interpolates tabulated nonlinear 3D mag. field computed by F.Bergsma
*>> Input  : RFZSC(1 to 5)  = point coordinates (R,Phi,Z,Sin(Phi),Cos(Phi))
*>> Output : BXYZ(1) = Bx  field coordinates at this point (in Tesla !!!)
*>>          BXYZ(2) = By                                     *******
*>>          BXYZ(3) = Bz
*>> + (when II312=12) : BXYZ( 4) = bBx/dx  field derivatives at this point
*>>                     BXYZ( 5) = bBx/dy
*>>                     BXYZ( 6) = bBx/dz
*>>                     BXYZ( 7) = bBy/dx
*>>                     BXYZ( 8) = bBy/dy
*>>                     BXYZ( 9) = bBy/dz
*>>                     BXYZ(10) = bBz/dx
*>>                     BXYZ(11) = bBz/dy
*>>                     BXYZ(12) = bBz/dz
*>> Author : Saclay Muon Software Group (SaMuSoG)
*>> Date   : 11/08/98
*
      IMPLICIT NONE
*
*
      INTEGER NCX,NCY,NCZ, II312
      REAL    XCPOS(*),YCPOS(*),ZCPOS(*)
      REAL    TACBX(*),TACBY(*),TACBZ(*)
      REAL    XYZ(3),BXYZ(12)
*
      INTEGER JX,JY,JZ, JX1,JY1,JZ1
      INTEGER JPPP,JPPM,JPMP,JPMM,JMPP,JMPM,JMMP,JMMM
      REAL    PASX,PASY,PASZ, XMIN,YMIN,ZMIN, XMAX,YMAX,ZMAX
      REAL    XX,YY,ZZ, SIGNZ, DX,DY,DZ, UMDX,UMDY,UMDZ
      REAL    BX11,BX12,BX1,BX21,BX22,BX2
      REAL    BY11,BY12,BY1,BY21,BY22,BY2
      REAL    BZ11,BZ12,BZ1,BZ21,BZ22,BZ2
      REAL    DXD,BX1DX,BX2DX,BY1DX,BY2DX,BZ1DX,BZ2DX,BXDX,BYDX,BZDX
      REAL    DYD,BXDY,BYDY,BZDY, DZD,BXDZ,BYDZ,BZDZ
      REAL    PRECI
      DATA PRECI /0.0001/
*
      PASX =(XCPOS(NCX)-XCPOS(1))/(NCX-1)
      PASY =(YCPOS(NCY)-YCPOS(1))/(NCY-1)
      PASZ =(ZCPOS(NCZ)-ZCPOS(1))/(NCZ-1)
      XMIN = XCPOS(1)
      YMIN = YCPOS(1)
      ZMIN = ZCPOS(1)       
      XMAX = XCPOS(NCX)
      YMAX = YCPOS(NCY)
      ZMAX = ZCPOS(NCZ)       
      XX   = XYZ(1)
      YY   = XYZ(2)
      ZZ   = XYZ(3)
*
      IF(XX.LT.XMIN-PRECI.OR.XX.GT.XMAX+PRECI.OR.
     +   YY.LT.YMIN-PRECI.OR.YY.GT.YMAX+PRECI.OR.
     +   ZZ.LT.ZMIN-PRECI.OR.ZZ.GT.ZMAX+PRECI   ) THEN
*        WRITE(6,*)'******** OUT OF DTBH8 *** B = 0 *******'
*        WRITE(6,'(3F12.2)')XX,XMIN,XMAX
*        WRITE(6,'(3F12.2)')YY,YMIN,YMAX
*        WRITE(6,'(3F12.2)')ZZ,ZMIN,ZMAX
        BXYZ(1) = 0.
        BXYZ(2) = 0.
        BXYZ(3) = 0.
        RETURN        
      ENDIF
      JX   = (XX-XMIN)/PASX + 2
      JY   = (YY-YMIN)/PASY + 2
      JZ   = (ZZ-ZMIN)/PASZ + 2
      SIGNZ =  1.
*>> Compute magnetic field :   BXYZ(1) = Bx,   BXYZ(2) = By,   BXYZ(3) = Bz
      JX1  = JX - 1
      JY1  = JY - 1
      JZ1  = JZ - 1
      JPPP = JX + NCX*JY1 + NCX*NCY*JZ1
      JPPM = JPPP - NCX*NCY
      JPMP = JPPP - NCX
      JPMM = JPPM - NCX
      JMPP = JPPP - 1
      JMPM = JPPM - 1
      JMMP = JPMP - 1
      JMMM = JPMM - 1
      DX   = ( XX - XCPOS(JX1) ) / ( XCPOS(JX) - XCPOS(JX1) )
      DY   = ( YY - YCPOS(JY1) ) / ( YCPOS(JY) - YCPOS(JY1) )
      DZ   = ( ZZ - ZCPOS(JZ1) ) / ( ZCPOS(JZ) - ZCPOS(JZ1) )
      UMDX = 1. - DX
      UMDY = 1. - DY
      UMDZ = 1. - DZ
      BX11 = TACBX(JMMM)*UMDX + TACBX(JPMM)*DX
      BX12 = TACBX(JMPM)*UMDX + TACBX(JPPM)*DX
      BX1  = BX11*UMDY + BX12*DY
      BX21 = TACBX(JMMP)*UMDX + TACBX(JPMP)*DX
      BX22 = TACBX(JMPP)*UMDX + TACBX(JPPP)*DX
      BX2  = BX21*UMDY + BX22*DY
      BY11 = TACBY(JMMM)*UMDX + TACBY(JPMM)*DX
      BY12 = TACBY(JMPM)*UMDX + TACBY(JPPM)*DX
      BY1  = BY11*UMDY + BY12*DY
      BY21 = TACBY(JMMP)*UMDX + TACBY(JPMP)*DX
      BY22 = TACBY(JMPP)*UMDX + TACBY(JPPP)*DX
      BY2  = BY21*UMDY + BY22*DY
      BZ11 = TACBZ(JMMM)*UMDX + TACBZ(JPMM)*DX
      BZ12 = TACBZ(JMPM)*UMDX + TACBZ(JPPM)*DX
      BZ1  = BZ11*UMDY + BZ12*DY
      BZ21 = TACBZ(JMMP)*UMDX + TACBZ(JPMP)*DX
      BZ22 = TACBZ(JMPP)*UMDX + TACBZ(JPPP)*DX
      BZ2  = BZ21*UMDY + BZ22*DY
      BXYZ(1) = BX1*UMDZ + BX2*DZ
      BXYZ(2) = BY1*UMDZ + BY2*DZ
      BXYZ(3) = BZ1*UMDZ + BZ2*DZ
      IF( II312.LT.12 ) RETURN
*
*>> Compute derivatives of magnetic field : BXYZ(i)
*>>  i =    4      5      6      7      8      9     10     11     12
*>>      dBx/dx dBx/dy dBx/dz dBy/dx dBy/dy dBy/dz dBz/dx dBz/dy dBz/dz
      IF( DX.GT.0.5 ) THEN
         DXD   = XX - XCPOS(JX1)
         BX1DX = TACBX(JMMM)*UMDY + TACBX(JMPM)*DY
         BX2DX = TACBX(JMMP)*UMDY + TACBX(JMPP)*DY
         BY1DX = TACBY(JMMM)*UMDY + TACBY(JMPM)*DY
         BY2DX = TACBY(JMMP)*UMDY + TACBY(JMPP)*DY
         BZ1DX = TACBZ(JMMM)*UMDY + TACBZ(JMPM)*DY
         BZ2DX = TACBZ(JMMP)*UMDY + TACBZ(JMPP)*DY
      ELSE
         DXD   = XX - XCPOS(JX)
         BX1DX = TACBX(JPMM)*UMDY + TACBX(JPPM)*DY
         BX2DX = TACBX(JPMP)*UMDY + TACBX(JPPP)*DY
         BY1DX = TACBY(JPMM)*UMDY + TACBY(JPPM)*DY
         BY2DX = TACBY(JPMP)*UMDY + TACBY(JPPP)*DY
         BZ1DX = TACBZ(JPMM)*UMDY + TACBZ(JPPM)*DY
         BZ2DX = TACBZ(JPMP)*UMDY + TACBZ(JPPP)*DY
      ENDIF
      BXDX = SIGNZ * ( BX1DX*UMDZ + BX2DX*DZ )
      BYDX = BY1DX*UMDZ + BY2DX*DZ
      BZDX = BZ1DX*UMDZ + BZ2DX*DZ
      IF( DY.GT.0.5 ) THEN
         DYD  = SIGNZ * (YY - YCPOS(JY1))
         BXDY = SIGNZ * (BX11*UMDZ + BX21*DZ )
         BYDY = BY11*UMDZ + BY21*DZ
         BZDY = BZ11*UMDZ + BZ21*DZ
      ELSE
         DYD  = SIGNZ * (YY - YCPOS(JY))
         BXDY = SIGNZ * (BX12*(1.-DZ) + BX22*DZ )
         BYDY = BY12*UMDZ + BY22*DZ
         BZDY = BZ12*UMDZ + BZ22*DZ
      ENDIF
      IF( DZ.GT.0.5 ) THEN
         DZD  = ZZ - ZCPOS(JZ1)
         BXDZ = SIGNZ * BX1
         BYDZ = BY1
         BZDZ = BZ1
      ELSE
         DZD  = ZZ- ZCPOS(JZ)
         BXDZ = SIGNZ * BX2
         BYDZ = BY2
         BZDZ = BZ2
      ENDIF
      IF( ZZ.GT.0. ) DZD = -DZD
      BXYZ( 4) = (BXYZ(1)-BXDX)/DXD
      BXYZ( 5) = (BXYZ(1)-BXDY)/DYD
      BXYZ( 6) = (BXYZ(1)-BXDZ)/DZD
      BXYZ( 7) = (BXYZ(2)-BYDX)/DXD
      BXYZ( 8) = (BXYZ(2)-BYDY)/DYD
      BXYZ( 9) = (BXYZ(2)-BYDZ)/DZD
      BXYZ(10) = (BXYZ(3)-BZDX)/DXD
      BXYZ(11) = (BXYZ(3)-BZDY)/DYD
      BXYZ(12) = (BXYZ(3)-BZDZ)/DZD
      RETURN
*
*
900   BXYZ( 1) = 0.000
      BXYZ( 2) = 0.000
      BXYZ( 3) = 0.000
      IF( II312.LT.12 ) RETURN
*
      BXYZ( 4) = 0.000
      BXYZ( 5) = 0.000
      BXYZ( 6) = 0.000
      BXYZ( 7) = 0.000
      BXYZ( 8) = 0.000
      BXYZ( 9) = 0.000
      BXYZ(10) = 0.000
      BXYZ(11) = 0.000
      BXYZ(12) = 0.000
      RETURN
*
*
      END
