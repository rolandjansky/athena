      subroutine uf_BdJpsimumuK0starKpi ( iret )

*----------------------------------------------------------------------*
*                                                                      *
* Name : uf_BdJpsimumuK0starKpi                                        *
*        (module)                                                      *
*                                                                      *
* Description : User subroutine to define requirements for event       *
*               accepting/rejecting additional or other that in        *
*               subroutine finsel for Bd->J/psi(mu,mu)K0star(K,pi)     *
*               events. Additional settings from cufdcp(10) array.     *
* Arguments   : iret (in/out) accepting(0)/rejecting(1) event          *
*                                                                      *
*                                                                      *
* Author : Pavel Reznicek               Date : 2010-08-26              *
*                                                                      *
*                                                                      *
* Last modifications :                                                 *
*          Pavel Reznicek               Date : 2010-08-26              *
*                                                                      *
* Keywords :                                                           *
*          [keywords]                                                  *
*----------------------------------------------------------------------*

C...Double precision and integer declarations.
      implicit double precision (a-h,o-z)
      implicit integer (i-n)

C...Decay tree genealogy in Pythia
      common /PYJETS/ N,NPAD,K(4000,5),P(4000,5),V(4000,5)
      save   /PYJETS/

C...User finsel decay selection and decay parameters
      common /ufdecay/ cufdcp(10),iufdc
      save   /ufdecay/

C...First event
      save ifirst
      data ifirst /1/

C...Family definition from Pythia
      name_self(i) = K(i,2)
      name_selv(i) = abs(K(i,2))
      name_father(i) = K(K(i,3),2)
      name_grandpa(i) = K(K(K(i,3),3),2)
      name_daughter1(i) = K(K(i,4),2)
      name_daughter2(i) = K(K(i,5),2)

      link_self(i) = i
      link_father(i) = K(i,3)
      link_grandpa(i) = K(K(i,3),3)
      link_daughter1(i) = K(i,4)
      link_daughter2(i) = K(i,5)

C...Variable to be used for debugging the finsel algorithm
      idebug = 0

      if ( ifirst.eq.1 ) then
        ifirst = 0
      endif

C...Return immediately if already rejected
      if ( iret.eq.1 ) return

C...Parameters:
c     cufdcp( 1) :
c     cufdcp( 2) :
c     cufdcp( 3) :
c     cufdcp( 4) :
c     cufdcp( 5) :
c     cufdcp( 6) :
c     cufdcp( 7) :
c     cufdcp( 8) :
c     cufdcp( 9) :
c     cufdcp(10) :

C...Preset the acceptance (1 ... event is rejected)
      iret = 1

      if ( idebug.eq.1 )
     +  print *,'USER_FINSEL: BEGINNING - BdJpsimumuK0starKpi'

C...Loop over all the particles in the event
      do i = 1,N
c       if ( tree(i).eq.1 ) then
C...Accept only Kstar0->Kpi decays (in case K0star comes from Bd)
        if ( name_selv(i) .eq.313.and.
     +       name_father(i).eq.511.and.
     +       ((abs(name_daughter1(i)).eq.211.and.
     +         abs(name_daughter2(i)).eq.321).or.
     +        (abs(name_daughter1(i)).eq.321.and.
     +         abs(name_daughter2(i)).eq.211)) ) then
          if ( idebug.eq.1 ) print *,'USER_FINSEL: ',name_self(i)
          iret = 0
        endif
      enddo

C...Evaluate the filtering output
      if ( iret.ne.0 ) then
        if ( idebug.eq.1 ) print *, 'USER_FINSEL: event rejected'
      endif

      return
      end
