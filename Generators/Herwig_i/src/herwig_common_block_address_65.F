c  For herwig version 6.4 , Jan 2002
c  Atlas version Ian Hinchliffe
      FVOIDP function herwig_common_block_address(common_block_name)
c-----------------------------------------------------------------------
c  returns address of HERWIG common-block by name
c  revision history:
c  -----------------
c Based on CDF code by  P.Murat, J Lys  et al
c-----------------------------------------------------------------------
C      implicit      none
c
c 2 Nov 2000 changed to adressi adressc and adressf to remove compile warnings
c
c 
#include "HERWIG65.INC"
#include "jimmy.inc"
c     For parton desnity info
      DOUBLE PRECISION X1,X2,Q
      INTEGER ID1,ID2,IDPDF1,IDPDF2
      COMMON /HRWPDF/ X1,X2,Q,ID1,ID2,IDPDF1,IDPDF2

      character*(*) common_block_name
c
c-----------------------------------------------------------------------
      FVOIDP adressi
      FVOIDP adressf
      FVOIDP adressc
      FVOIDP adrescx
      FVOIDP adreslg
c-----------------------------------------------------------------------
      if (common_block_name.eq.'HWBEAM')then
       herwig_common_block_address = adressi(ipart1)

      else if (common_block_name.eq.'HWBMCH')then
       herwig_common_block_address = adressc(part1)

      else if (common_block_name.eq.'HWPROC')then
       herwig_common_block_address = adressf(ebeam1)

      else if (common_block_name.eq.'HWPRAM')then
       herwig_common_block_address = adressf(afch(1,1))

      else if (common_block_name.eq.'HWPRCH')then
       herwig_common_block_address = adressc(autpdf)

      else if (common_block_name.eq.'HWPART')then
       herwig_common_block_address = adressi(nevpar)
    
      else if (common_block_name.eq.'HWPARP')then
       herwig_common_block_address = adressf(decpar(1,1))

      else if (common_block_name.eq.'HWBOSC')then
       herwig_common_block_address = adressf(alpfac)

      else if (common_block_name.eq.'HWPARC')then
       herwig_common_block_address = adressi(jcopar(1,1))

      else if (common_block_name.eq.'HWBRCH')then
       herwig_common_block_address = adressf(anomsc(1,1))

      else if (common_block_name.eq.'HWEVNT')then
       herwig_common_block_address = adressf(avwgt)
 
      else if (common_block_name.eq.'HWHARD')then
       herwig_common_block_address = adressf(asfixd)

      else if (common_block_name.eq.'HWPROP')then
       herwig_common_block_address = adressf(rltim(0))

      else if (common_block_name.eq.'HWUNAM')then
       herwig_common_block_address = adressc(rname(0))
 
      else if (common_block_name.eq.'HWUPDT')then
       herwig_common_block_address = adressf(brfrac(1))

      else if (common_block_name.eq.'HWUWTS')then
       herwig_common_block_address = adressf(repwt(0,0,0))
 
      else if (common_block_name.eq.'HWUCLU')then
       herwig_common_block_address = adressf(cldkwt(1))
 
      else if (common_block_name.eq.'HWDIST')then
       herwig_common_block_address = adressf(exag)

      else if (common_block_name.eq.'HWQDKS')then
       herwig_common_block_address = adressf(vtxqdk(1,1))

      else if (common_block_name.eq.'HWUSUD')then
       herwig_common_block_address = adressf(accur)     

cc v6.1
      else if (common_block_name.eq.'HWSUSY')then
       herwig_common_block_address = adressf(tanb)
 
      else if (common_block_name.eq.'HWRPAR')then
       herwig_common_block_address = adressf(lamda1(1,1,1))
 
      else if (common_block_name.eq.'HWMINB')then
       herwig_common_block_address = adressf(pmbn1)
 
      else if (common_block_name.eq.'HWCLUS')then
       herwig_common_block_address = adressf(ppcl(1,1))
 
cc v6.202
      else if (common_block_name.eq.'HWGRAV')then
       herwig_common_block_address = adressf(grvlam)
 
      else if (common_block_name.eq.'HW6202')then
       herwig_common_block_address = adressf(vipwid(1))
cc v6.203

      else if (common_block_name.eq.'HW6203')then
       herwig_common_block_address = adressf(abwgt)

cc v6.301
      else if (common_block_name.eq.'HW6300')then
       herwig_common_block_address = adressf(mjjmin)
 
      else if (common_block_name.eq.'HWPMRS')then
       herwig_common_block_address = adressf(FMRS(1,1,1,1))

      else if (common_block_name.eq.'HWCIRC')then
       herwig_common_block_address = adressi(circop)

cc v6.400
      else if (common_block_name.eq.'HWDSPB')then
       herwig_common_block_address = adressf(abmode(1,1))

      else if (common_block_name.eq.'HWDSP2')then
         herwig_common_block_address = adressf(a2mode(1,1))

      else if (common_block_name.eq.'HWDSP3')then
         herwig_common_block_address = adressf(a3mode(1,1,1)) 

      else if (common_block_name.eq.'HWDSP4')then
         herwig_common_block_address = adressf(a4mode(1,1,1)) 

      else if (common_block_name.eq.'HWDSPN')then
         herwig_common_block_address = adressi(ndecsy) 

      else if (common_block_name.eq.'HWSPIN')then
         herwig_common_block_address = adrescx(mespn(1,1,1,1,1,1)) 

      else if (common_block_name.eq.'HWSTAU')then
         herwig_common_block_address = adressi(jak1)

cc v6.500
      else if (common_block_name.eq.'HWGUPR')then
         herwig_common_block_address = adressf(lhwgt(1))

      else if (common_block_name.eq.'HW6500')then
         herwig_common_block_address = adreslg(prespl)

cc v6.504
      else if (common_block_name.eq.'HW6504')then
         herwig_common_block_address = adressi(itoprd)

cc v6.506
      else if (common_block_name.eq.'HW6506')then
         herwig_common_block_address = adressf(pdfx0)

cc v6.510
      else if (common_block_name.eq.'HW6510')then
         herwig_common_block_address = adressi(ndetry)

cc v6.520
      else if (common_block_name.eq.'HW6520')then
         herwig_common_block_address = adressf(dqxtra)

cc Jimmy Common blocks
      else if (common_block_name.eq.'JMPARM')then
         herwig_common_block_address = adressf(ptjim)

cc special for atlas (PDF information)

      else if (common_block_name.eq.'HRWPDF')then
         herwig_common_block_address = adressf(x1)  
    
     

      else  

       write(6,*) " Unknown Common Block Name:  " , common_block_name 
       herwig_common_block_address = -99
      end if

 
      return
      end


