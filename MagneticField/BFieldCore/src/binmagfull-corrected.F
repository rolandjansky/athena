* Author : Saclay Muon Software Group (SaMuSoG)
*
*
*
*
************************************************************************
      SUBROUTINE BINMAGFULL2( SECMAG, NRMAG,NFMAG,NZMAG, RMAG,FMAG,ZMAG,
     +                   BRMAG,BFMAG,BZMAG, 
     +DRPOS,RPOSMI,DFPOS,FPOSMI,DZPOS,ZPOSMI,JRPOS,JFPOS,JZPOS,
     +DRMAG,RMAGMI,RMAGMA,DFMAG,FMAGMI,DZMAG,ZMAGMI,JRMAG,JFMAG,JZMAG,
     +                   RFZSC,II312, BXYZ )
*
*>> Interpolates tabulated 3D mag. field of toroids
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
      INTEGER I,J,IJ,I22,I33
      INTEGER NRMAG(*),NFMAG,NZMAG,II312
      INTEGER JRPOS(*),JFPOS(*),JZPOS(*), JRMAG(*),JFMAG(*),JZMAG(*)
      REAL     RMAG(*), FMAG(*), ZMAG(*), BRMAG(*),BFMAG(*),BZMAG(*)
      REAL    DRPOS, RPOSMI, DFPOS, FPOSMI, DZPOS, ZPOSMI
      REAL    DRMAG, RMAGMI, RMAGMA, DFMAG, FMAGMI, DZMAG, ZMAGMI
      REAL    SECMAG, RFZSC(6), BXYZ(12)
*
      INTEGER JLINEA, JF,JZ, JF1,JZ1
      INTEGER JDEB,JFIN, JRPP,JRMP,JRPM,JRMM, JRPP1,JRMP1,JRPM1,JRMM1
      REAL    RR,F,Z,SINF,COSF,AZ, SIGNR, SIGNZ, FF
      REAL    DRPP,DRMP,DRPM,DRMM, DF,DZ
      REAL    UMDRMM,UMDRPM,UMDRMP,UMDRPP,UMDF,UMDZ
      REAL    BR11,BR12,BR1,BR21,BR22,BR2,BR
      REAL    BF11,BF12,BF1,BF21,BF22,BF2,BF
      REAL    BZ11,BZ12,BZ1,BZ21,BZ22,BZ2,BZ
      REAL    DRMMDR,BR11DR,BF11DR,BZ11DR
      REAL    DRPMDR,BR12DR,BF12DR,BZ12DR
      REAL    DRMPDR,BR21DR,BF21DR,BZ21DR
      REAL    DRPPDR,BR22DR,BF22DR,BZ22DR
      REAL    DBR1DR,DBF1DR,DBZ1DR,DBR2DR,DBF2DR,DBZ2DR
      REAL    DFD,BRDF,BFDF,BZDF, DZD,BRDZ,BFDZ,BZDZ
      REAL    DBRDR,DBRDF,DBRDZ, DBFDR,DBFDF,DBFDZ, DBZDR,DBZDF,DBZDZ
      REAL    DBXDR,DBXDF,DBXDZ, DBYDR,DBYDF,DBYDZ, SINFRR,COSFRR
      REAL    BXYZ2
      REAL*8  PI,derd
*
      INTEGER JTROUV
*
      integer lc,mc,nc, JRMAGST
*     JRMAGST(lc,mc,nc) = JRMAG(nc + ncmax*(mc-1) + ncmax*mcmax*(lc-1))
      JRMAGST(lc,mc,nc)=
     + JRMAG(nc+(nadmig+1)*(mc-1)+ (nadmig+1)*mamagz*(lc-1))
*
*
      PI   = 3.141592654d0
      RR   = RFZSC(1)
      F    = RFZSC(2)
      Z    = RFZSC(3)
      AZ   = RFZSC(6)
      derd = 180.d0/3.141592659d0
      SINF = RFZSC(4)
      COSF = RFZSC(5)
      IF( Z .LT. 0. ) THEN
        F    = PI - F
        COSF = - COSF
      ENDIF
      IF( F .LT. 0. )  F = F  + 2*PI
*>> Here, FF [0,SECMAG]
      SIGNR  =  1.
      SIGNZ  =  1.                !  Warning
      JF1    = INT(F / SECMAG)
      FF     = F - JF1*SECMAG
      JLINEA = (FF-FMAGMI) / DFMAG
      JLINEA = MAX( 1 , MIN( NADMAG , JLINEA ) )
      JF = JFMAG(JLINEA)
      IF( JF.LT.0 ) THEN
         JF = - JF
10       IF( FF.LT.FMAG(JF) ) GO TO 15
            JF = JF + 1
            IF( JF.LT.NFMAG ) GO TO 10
15       CONTINUE
      ENDIF
*
      JLINEA = (AZ-ZMAGMI) / DZMAG
      IF( JLINEA.LT.1 .OR. JLINEA.GT.NADMAG ) GO TO 900
      JZ = JZMAG(JLINEA)
      IF( JZ.LT.0 ) THEN
         JZ = - JZ
20       IF( AZ.LT.ZMAG(JZ) ) GO TO 25
            JZ = JZ + 1
            IF( JZ.LT.NZMAG ) GO TO 20
25       CONTINUE
      ENDIF
*
      IF( RR.GT.RMAGMA ) GO TO 900
      JLINEA = (RR-RMAGMI) / DRMAG
      IF( JLINEA.LT.1 )  GO TO 900
      JRPP = JRMAGST(JF  ,JZ  ,JLINEA)
      IF( JRPP.LT.0 ) THEN
         JDEB = - JRPP - 1
         JFIN = IABS( JRMAGST(JF  ,JZ  ,JLINEA+1) )
         JRPP = JTROUV(RMAG,JDEB,JFIN,RR)
      ENDIF
      JRMP = JRMAGST(JF-1,JZ  ,JLINEA)
      IF( JRMP.LT.0 ) THEN
         JDEB = - JRMP - 1
         JFIN = IABS( JRMAGST(JF-1,JZ  ,JLINEA+1) )
         JRMP = JTROUV(RMAG,JDEB,JFIN,RR)
      ENDIF
      JRPM = JRMAGST(JF  ,JZ-1,JLINEA)
      IF( JRPM.LT.0 ) THEN
         JDEB = - JRPM - 1
         JFIN = IABS( JRMAGST(JF  ,JZ-1,JLINEA+1) )
         JRPM = JTROUV(RMAG,JDEB,JFIN,RR)
      ENDIF
      JRMM = JRMAGST(JF-1,JZ-1,JLINEA)
      IF( JRMM.LT.0 ) THEN
         JDEB = - JRMM - 1
         JFIN = IABS( JRMAGST(JF-1,JZ-1,JLINEA+1) )
         JRMM = JTROUV(RMAG,JDEB,JFIN,RR)
      ENDIF
*
*>> Compute magnetic field :   BXYZ(1) = Bx,   BXYZ(2) = By,   BXYZ(3) = Bz
      JRPP1 = JRPP - 1
      JRMP1 = JRMP - 1
      JRPM1 = JRPM - 1
      JRMM1 = JRMM - 1
      JF1   = JF - 1
      JZ1   = JZ - 1
      DRPP = ( RR - RMAG(JRPP1) ) / ( RMAG(JRPP) - RMAG(JRPP1) )
      DRMP = ( RR - RMAG(JRMP1) ) / ( RMAG(JRMP) - RMAG(JRMP1) )
      DRPM = ( RR - RMAG(JRPM1) ) / ( RMAG(JRPM) - RMAG(JRPM1) )
      DRMM = ( RR - RMAG(JRMM1) ) / ( RMAG(JRMM) - RMAG(JRMM1) )
      DF   = ( FF - FMAG(JF1) ) / ( FMAG(JF) - FMAG(JF1) )
      DZ   = ( AZ - ZMAG(JZ1) ) / ( ZMAG(JZ) - ZMAG(JZ1) )
      UMDRMM = 1. - DRMM
      UMDRPM = 1. - DRPM
      UMDRMP = 1. - DRMP
      UMDRPP = 1. - DRPP
      UMDF   = 1. - DF
      UMDZ   = 1. - DZ
      BR11 = BRMAG(JRMM1)*UMDRMM + BRMAG(JRMM)*DRMM
      BR12 = BRMAG(JRPM1)*UMDRPM + BRMAG(JRPM)*DRPM
      BR1 = BR11*UMDF + BR12*DF
      BR21 = BRMAG(JRMP1)*UMDRMP + BRMAG(JRMP)*DRMP
      BR22 = BRMAG(JRPP1)*UMDRPP + BRMAG(JRPP)*DRPP
      BR2 = BR21*UMDF + BR22*DF
      BR = SIGNR * ( BR1*UMDZ + BR2*DZ )
      BF11 = BFMAG(JRMM1)*UMDRMM + BFMAG(JRMM)*DRMM
      BF12 = BFMAG(JRPM1)*UMDRPM + BFMAG(JRPM)*DRPM
      BF1 = BF11*UMDF + BF12*DF
      BF21 = BFMAG(JRMP1)*UMDRMP + BFMAG(JRMP)*DRMP
      BF22 = BFMAG(JRPP1)*UMDRPP + BFMAG(JRPP)*DRPP
      BF2 = BF21*UMDF + BF22*DF
      BF = BF1*UMDZ + BF2*DZ
      BXYZ(1) = BR*COSF - BF*SINF
      BXYZ(2) = BR*SINF + BF*COSF
      BZ11 = BZMAG(JRMM1)*UMDRMM + BZMAG(JRMM)*DRMM
      BZ12 = BZMAG(JRPM1)*UMDRPM + BZMAG(JRPM)*DRPM
      BZ1 = BZ11*UMDF + BZ12*DF
      BZ21 = BZMAG(JRMP1)*UMDRMP + BZMAG(JRMP)*DRMP
      BZ22 = BZMAG(JRPP1)*UMDRPP + BZMAG(JRPP)*DRPP
      BZ2 = BZ21*UMDF + BZ22*DF
      BXYZ(3) = SIGNR * SIGNZ * ( BZ1*UMDZ + BZ2*DZ )
*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      BXYZ2 = BXYZ(2)
      IF( Z .LT. 0. ) BXYZ(2) = -BXYZ(2)
*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      IF( II312.LT.12 ) RETURN
*
*>> Compute derivatives of magnetic field : BXYZ(i)
*>>  i =    4      5      6      7      8      9     10     11     12
*>>      dBx/dx dBx/dy dBx/dz dBy/dx dBy/dy dBy/dz dBz/dx dBz/dy dBz/dz
      BZ = BXYZ( 3)
      DRMMDR =   RMAG(JRMM)- RMAG(JRMM1)
      BR11DR = (BRMAG(JRMM)-BRMAG(JRMM1)) / DRMMDR
      BF11DR = (BFMAG(JRMM)-BFMAG(JRMM1)) / DRMMDR
      BZ11DR = (BZMAG(JRMM)-BZMAG(JRMM1)) / DRMMDR
      DRPMDR =   RMAG(JRPM)- RMAG(JRPM1)
      BR12DR = (BRMAG(JRPM)-BRMAG(JRPM1)) / DRPMDR
      BF12DR = (BFMAG(JRPM)-BFMAG(JRPM1)) / DRPMDR
      BZ12DR = (BZMAG(JRPM)-BZMAG(JRPM1)) / DRPMDR
      DRMPDR =   RMAG(JRMP)- RMAG(JRMP1)
      BR21DR = (BRMAG(JRMP)-BRMAG(JRMP1)) / DRMPDR
      BF21DR = (BFMAG(JRMP)-BFMAG(JRMP1)) / DRMPDR
      BZ21DR = (BZMAG(JRMP)-BZMAG(JRMP1)) / DRMPDR
      DRPPDR =   RMAG(JRPP)- RMAG(JRPP1)
      BR22DR = (BRMAG(JRPP)-BRMAG(JRPP1)) / DRPPDR
      BF22DR = (BFMAG(JRPP)-BFMAG(JRPP1)) / DRPPDR
      BZ22DR = (BZMAG(JRPP)-BZMAG(JRPP1)) / DRPPDR
      DBR1DR = BR11DR*UMDF + BR12DR*DF
      DBF1DR = BF11DR*UMDF + BF12DR*DF
      DBZ1DR = BZ11DR*UMDF + BZ12DR*DF
      DBR2DR = BR21DR*UMDF + BR22DR*DF
      DBF2DR = BF21DR*UMDF + BF22DR*DF
      DBZ2DR = BZ21DR*UMDF + BZ22DR*DF
      DBRDR = SIGNR *         ( DBR1DR*UMDZ + DBR2DR*DZ )
      DBFDR =                   DBF1DR*UMDZ + DBF2DR*DZ
      DBZDR = SIGNR * SIGNZ * ( DBZ1DR*UMDZ + DBZ2DR*DZ )
      IF( DF.GT.0.5 ) THEN
         DFD = SIGNR * (FF - FMAG(JF1))
         BRDF = SIGNR *         ( BR11*UMDZ + BR21*DZ )
         BFDF =                   BF11*UMDZ + BF21*DZ
         BZDF = SIGNR * SIGNZ * ( BZ11*UMDZ + BZ21*DZ )
      ELSE
         DFD = SIGNR * (FF - FMAG(JF))
         BRDF = SIGNR *         ( BR12*UMDZ + BR22*DZ )
         BFDF =                   BF12*UMDZ + BF22*DZ
         BZDF = SIGNR * SIGNZ * ( BZ12*UMDZ + BZ22*DZ )
      ENDIF
      IF( DZ.GT.0.5 ) THEN
         DZD = SIGNZ * (AZ - ZMAG(JZ1))
         BRDZ = SIGNR * BR1
         BFDZ = BF1
         BZDZ = SIGNR * SIGNZ * BZ1
      ELSE
         DZD = SIGNZ * (AZ - ZMAG(JZ))
         BRDZ = SIGNR * BR2
         BFDZ = BF2
         BZDZ = SIGNR * SIGNZ * BZ2
      ENDIF
      DBRDF = (BR-BRDF)/DFD
      DBRDZ = (BR-BRDZ)/DZD
      DBFDF = (BF-BFDF)/DFD
      DBFDZ = (BF-BFDZ)/DZD
      DBZDF = (BZ-BZDF)/DFD
      DBZDZ = (BZ-BZDZ)/DZD
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
      IF(z.lt.0.)THEN
         I22 = 1
        DO I=1,3
           I22=-I22
           I33=1
         DO J=1,3
            I33=-I33
            IJ = 3+(I-1)*3+J
            BXYZ(IJ) = (-1)*BXYZ(IJ)*I22*I33 !  factor 1 for x,z, -1 for y
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

