*
*
*
      subroutine findmaph8(nmap,tmi,tma,t,imap)
      implicit none
      integer imap,nmap
      real t,tmi(*),tma(*),eps
      if(nmap.gt.3) then
        write(6,*)'**** too many map !!!! ****',nmap
        stop
      endif
      imap = 0
      eps=0.001
      if(t.gt.tma(2)+eps)     then
        if(t.lt.tma(3)+eps)   then
          if(t.gt.tmi(3)-eps) then
            imap =3
          endif
        endif
      elseif(t.lt.tma(1)+eps) then
        if(t.gt.tmi(1)-eps)   then
          imap=1
        endif
      elseif(t.gt.tmi(2)-eps) then
        imap=2
      endif
      return
      end
