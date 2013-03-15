*
*
*
*
* Author : Saclay Muon Software Group (SaMuSoG)
***************************************************************************
      SUBROUTINE BFELIXXX( SECFEL, NRR,NFF,NZZ, RRPOS,FFPOS,ZZPOS,
     +                   TABBR,TABBF,TABBZ, 
     +DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                   RFZSC,II312, BXYZ )
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
#include "BFieldCore/cospee.inc"
*
      INTEGER I,J,IJ,IB,ID
      INTEGER NRR,NFF,NZZ, II312
      INTEGER JRPOS(*),JFPOS(*),JZPOS(*), JRMAG(*),JFMAG(*),JZMAG(*)
      REAL    RRPOS(*),FFPOS(*),ZZPOS(*), TABBR(*),TABBF(*),TABBZ(*)
      REAL    DRPOS, RPOSMI, DFPOS, FPOSMI, DZPOS, ZPOSMI
      REAL    DRMAG, RMAGMI, RMAGMA, DFMAG, FMAGMI, DZMAG, ZMAGMI
      REAL    SECFEL, RFZSC(6), BXYZ(12)
*
      INTEGER JLINEA, JSECT, JR,JF,JZ, JR1,JF1,JZ1
      INTEGER JPPP,JPPM,JPMP,JPMM,JMPP,JMPM,JMMP,JMMM
      REAL    RR,F,Z,SINF,COSF,AZ, SIGNZ, FF, DR,DF,DZ, UMDR,UMDF,UMDZ
      REAL    BR11,BR12,BR1,BR21,BR22,BR2,BR
      REAL    BF11,BF12,BF1,BF21,BF22,BF2,BF
      REAL    BZ11,BZ12,BZ1,BZ21,BZ22,BZ2,BZ
      REAL    DRDInv,BR1DR,BR2DR,BF1DR,BF2DR,BZ1DR,BZ2DR,BRDR,BFDR,BZDR
      REAL    DFDInv,BRDF,BFDF,BZDF, DZDInv,BRDZ,BFDZ,BZDZ
      REAL    DBRDR,DBRDF,DBRDZ, DBFDR,DBFDF,DBFDZ, DBZDR,DBZDF,DBZDZ
      REAL    DBXDR,DBXDF,DBXDZ, DBYDR,DBYDF,DBYDZ, SINFRR,COSFRR
      REAL    BXYZ2
      REAL*8  derd,PI
*
      PI   = 3.141592654d0
      RR   = RFZSC(1)
      F    = RFZSC(2)
      Z    = RFZSC(3)
      AZ   =-RFZSC(6)
      derd = 180.d0/PI
      SINF = RFZSC(4)
      COSF = RFZSC(5)
      IF( Z .GT. 0. ) THEN
        F    = PI - F
        COSF = - COSF
      ENDIF
      IF( F .LT. 0. )  F = F  + 2*PI
      SIGNZ = 1.          !  Warning
*
      JLINEA = (RR-RPOSMI) / DRPOS
      IF( JLINEA.LT.1 .OR. JLINEA.GT.NADFEL ) GO TO 900
      JR = JRPOS(JLINEA)
      IF( JR.LT.0 ) THEN
        JR = - JR
10      IF( RR.LT.RRPOS(JR) ) GO TO 15
        JR = JR + 1
        IF( JR.LT.NRR )       GO TO 10
15      CONTINUE
      ENDIF      
*
*>> back to [0,45] whatever F
      JSECT  = INT( F / SECFEL)
      FF     =    F - JSECT*SECFEL
      JLINEA = (FF-FPOSMI) / DFPOS
      JLINEA = MAX( 1 , MIN( NADFEL , JLINEA ) )
      JF = JFPOS(JLINEA)
      IF( JF.LT.0 ) THEN
        JF = - JF
20      IF( FF.LT.FFPOS(JF) ) GO TO 25
        JF = JF + 1
        IF( JF.LT.NFF )       GO TO 20
25      CONTINUE
      ENDIF
*      write(85,'(2f12.5)') f*derd,ff*derd
*      write(85,'(3I5)') JSect,jlinea,jf
*
      JLINEA = (AZ-ZPOSMI) / DZPOS
      IF( JLINEA.LT.1 .OR. JLINEA.GT.NADFEL ) GO TO 900
      JZ = JZPOS(JLINEA)
      IF( JZ.LT.0 ) THEN
        JZ = - JZ
30      IF( AZ.LT.ZZPOS(JZ) ) GO TO 35
        JZ = JZ + 1
        IF( JZ.LT.NZZ )       GO TO 30
35      CONTINUE
      ENDIF
*
*>> Compute magnetic field :   BXYZ(1) = Bx,   BXYZ(2) = By,   BXYZ(3) = Bz
      JR1   = JR - 1
      JF1   = JF - 1
      JZ1   = JZ - 1
      JPPP = JR + NRR*JF1 + NRR*NFF*JZ1
      JPPM = JPPP - NRR*NFF
      JPMP = JPPP - NRR
      JPMM = JPPM - NRR
      JMPP = JPPP - 1
      JMPM = JPPM - 1
      JMMP = JPMP - 1
      JMMM = JPMM - 1
      DR = ( RR - RRPOS(JR1) ) / ( RRPOS(JR) - RRPOS(JR1) )
      DF = ( FF - FFPOS(JF1) ) / ( FFPOS(JF) - FFPOS(JF1) )
      DZ = ( AZ - ZZPOS(JZ1) ) / ( ZZPOS(JZ) - ZZPOS(JZ1) )
      UMDR   = 1. - DR
      UMDF   = 1. - DF
      UMDZ   = 1. - DZ
      BR11 = TABBR(JMMM)*UMDR + TABBR(JPMM)*DR
      BR12 = TABBR(JMPM)*UMDR + TABBR(JPPM)*DR
      BR1 = BR11*UMDF + BR12*DF
      BR21 = TABBR(JMMP)*UMDR + TABBR(JPMP)*DR
      BR22 = TABBR(JMPP)*UMDR + TABBR(JPPP)*DR
      BR2 = BR21*UMDF + BR22*DF
      BR = SIGNZ * ( BR1*UMDZ + BR2*DZ )
      BF11 = TABBF(JMMM)*UMDR + TABBF(JPMM)*DR
      BF12 = TABBF(JMPM)*UMDR + TABBF(JPPM)*DR
      BF1 = BF11*UMDF + BF12*DF
      BF21 = TABBF(JMMP)*UMDR + TABBF(JPMP)*DR
      BF22 = TABBF(JMPP)*UMDR + TABBF(JPPP)*DR
      BF2 = BF21*UMDF + BF22*DF
      BF = BF1*UMDZ + BF2*DZ
      BXYZ(1) = BR*COSF - BF*SINF
      BXYZ(2) = BR*SINF + BF*COSF
      BZ11 = TABBZ(JMMM)*UMDR + TABBZ(JPMM)*DR
      BZ12 = TABBZ(JMPM)*UMDR + TABBZ(JPPM)*DR
      BZ1 = BZ11*UMDF + BZ12*DF
      BZ21 = TABBZ(JMMP)*UMDR + TABBZ(JPMP)*DR
      BZ22 = TABBZ(JMPP)*UMDR + TABBZ(JPPP)*DR
      BZ2 = BZ21*UMDF + BZ22*DF
      BXYZ(3) = BZ1*UMDZ + BZ2*DZ
*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*!!!! true only if tanja calculation is done for z<0
      BXYZ2 = BXYZ(2)
      IF( Z .GT. 0. ) BXYZ(2) = -BXYZ(2)  
*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( II312.LT.12 ) RETURN
*
*>> Compute derivatives of magnetic field : BXYZ(i)
*>>  i =    4      5      6      7      8      9     10     11     12
*>>      dBx/dx dBx/dy dBx/dz dBy/dx dBy/dy dBy/dz dBz/dx dBz/dy dBz/dz
      BZ = BXYZ( 3)
      IF( DR.GT.0.5 ) THEN
        DRDInv =  1.d0 / (RR - RRPOS(JR1))
        BR1DR = TABBR(JMMM)*UMDF + TABBR(JMPM)*DF
        BR2DR = TABBR(JMMP)*UMDF + TABBR(JMPP)*DF
        BF1DR = TABBF(JMMM)*UMDF + TABBF(JMPM)*DF
        BF2DR = TABBF(JMMP)*UMDF + TABBF(JMPP)*DF
        BZ1DR = TABBZ(JMMM)*UMDF + TABBZ(JMPM)*DF
        BZ2DR = TABBZ(JMMP)*UMDF + TABBZ(JMPP)*DF
      ELSE
        DRDInv =  1.d0 / (RR - RRPOS(JR))
        BR1DR = TABBR(JPMM)*UMDF + TABBR(JPPM)*DF
        BR2DR = TABBR(JPMP)*UMDF + TABBR(JPPP)*DF
        BF1DR = TABBF(JPMM)*UMDF + TABBF(JPPM)*DF
        BF2DR = TABBF(JPMP)*UMDF + TABBF(JPPP)*DF
        BZ1DR = TABBZ(JPMM)*UMDF + TABBZ(JPPM)*DF
        BZ2DR = TABBZ(JPMP)*UMDF + TABBZ(JPPP)*DF
      ENDIF
      BRDR = SIGNZ * ( BR1DR*UMDZ + BR2DR*DZ )
      BFDR = BF1DR*UMDZ + BF2DR*DZ
      BZDR = BZ1DR*UMDZ + BZ2DR*DZ
      IF( DF.GT.0.5 ) THEN
        DFDInv = SIGNZ / (FF - FFPOS(JF1))
        BRDF = SIGNZ * ( BR11*UMDZ + BR21*DZ )
        BFDF = BF11*UMDZ + BF21*DZ
        BZDF = BZ11*UMDZ + BZ21*DZ
      ELSE
        DFDInv =  SIGNZ / (FF - FFPOS(JF))
        BRDF = SIGNZ * ( BR12*UMDZ + BR22*DZ )
        BFDF = BF12*UMDZ + BF22*DZ
        BZDF = BZ12*UMDZ + BZ22*DZ
      ENDIF
      IF( DZ.GT.0.5 ) THEN
        DZDInv = 1.d0 / ( AZ - ZZPOS(JZ1) )
        BRDZ = SIGNZ * BR1
        BFDZ = BF1
        BZDZ = BZ1
      ELSE
        DZDInv =  1.d0 / (AZ- ZZPOS(JZ))
        BRDZ = SIGNZ * BR2
        BFDZ = BF2
        BZDZ = BZ2
      ENDIF
ccc      IF( Z.GT.0. ) DZD = -DZD
      
      DBRDR = (BR-BRDR)*DRDInv
      DBRDF = (BR-BRDF)*DFDInv
      DBRDZ = (BR-BRDZ)*DZDInv
      DBFDR = (BF-BFDR)*DRDInv
      DBFDF = (BF-BFDF)*DFDInv
      DBFDZ = (BF-BFDZ)*DZDInv
      DBZDR = (BZ-BZDR)*DRDInv
      DBZDF = (BZ-BZDF)*DFDInv
      DBZDZ = (BZ-BZDZ)*DZDInv
      DBXDR = DBRDR*COSF - DBFDR*SINF
      DBXDF = DBRDF*COSF - DBFDF*SINF - BXYZ2
      DBXDZ = DBRDZ*COSF - DBFDZ*SINF
      DBYDR = DBRDR*SINF + DBFDR*COSF
      DBYDF = DBRDF*SINF + DBFDF*COSF + BXYZ(1)
      DBYDZ = DBRDZ*SINF + DBFDZ*COSF
      SINFRR = SINF/RR
      COSFRR = COSF/RR
      BXYZ( 4) = COSF * DBXDR - SINFRR * DBXDF
      BXYZ( 5) = SINF * DBXDR + COSFRR * DBXDF
      BXYZ( 6) = DBXDZ
      BXYZ( 7) = COSF * DBYDR - SINFRR * DBYDF
      BXYZ( 8) = SINF * DBYDR + COSFRR * DBYDF
      BXYZ( 9) = DBYDZ
      BXYZ(10) = COSF * DBZDR - SINFRR * DBZDF
      BXYZ(11) = SINF * DBZDR + COSFRR * DBZDF
      BXYZ(12) = DBZDZ
*
      IF( z .gt. 0. ) THEN
        IJ =  3
        IB = -1
        DO I=1,3
          IB = -IB
          ID = 1
          DO J=1,3
            ID = -ID
            IJ = IJ + 1
            BXYZ(IJ) = BXYZ(IJ)*IB*ID !  factor 1 for x,z, -1 for y
          END DO
        END DO
      ENDIF   
*   
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

