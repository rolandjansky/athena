*CMZ :          20/09/2001  18.42.30  by  Unknown
*-- Author :    Maria Smizanska   22/09/96

      SUBROUTINE FIND_CHILDREN(I)
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Name : FIND_CHILDREN                                                 *
*        (module)                                                      *
*                                                                      *
* Description : find full decay tree  of initial particle descendants  *
*               and check if each one fullfills requirements defined   *
*               by FUNCTION QUALIFY according to user datacard CUTOFF  *
*               This subroutine has recursive call                     *
* Arguments   :                                                        *
*                input:    I               particle line # in LUJETS   *
*               output:   inside   =0      B-hadron chain rejected     *
*                                  =1      B-hadron chain accepted     *
*                                                                      *
* Banks/Tables:                                                        *
*                                                                      *
*----------------------------------------------------------------------*
*                                                                      *
* Author : Maria Smizanska                    Date : 20/09/96          *
*                                                                      *
*                                                                      *
* Last modifications :  Chafik Driouichi                               *
*        [name]                               Date : 15/09/ 2001       *
*                                                                      *
* Keywords :                                                           *
*        [keywords]                                                    *
*----------------------------------------------------------------------*
*                                                                       
c +CDE, TYPING.
*KEEP,DOUB.
C...Double precision and integer declarations.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IMPLICIT INTEGER(I-N)
*KEEP,LUJETS.
       COMMON/PYJETS/N,NPAD,K(4000,5),P(4000,5),V(4000,5)
*KEEP,BTREE.
      common/btree/ntree,tree(4000)
      SAVE /btree/
*KEND.
       INTEGER inside,I,NCHI
*KEEP,FAMILY.
       name_self(I)=K(I,2)
       name_selv(I)=abs(K(I,2))
       name_father(I)=K(K(I,3),2)
       name_grandpa(I)=K(K(K(I,3),3),2)
       name_daughter1(I)=K(K(I,4),2)
       name_daughter2(I)=K(K(I,5),2)

       link_self(I)=I
       link_father(I)=K(I,3)
       link_grandpa(I)=K(K(I,3),3)
       link_daughter1(I)=K(I,4)
       link_daughter2(I)=K(I,5)
*KEND.
C ----------- end CDE --------------------------------------------------
C ----------- end CDE --------------------------------------------------

c local debugging control (inactive for idebug=0)
      idebug=0


c is particle inside cuts?
      inside=qualify(I)

      if(inside.ne.0) then
c  store  particle number

      tree(I)=1


c   find children  of particle I

      NCHI=children(I) 
      if(NCHI.ne.0) then
       l1=link_daughter1(I)   
       do i1=1,NCHI
        il1=l1+i1-1
        inside=qualify(il1)
        tree(il1)=-1
        if(qualify(il1).eq.1) then
         tree(il1)=1
         inside=qualify(il1)  
      
         nchi2=children(il1)
         if(nchi2.ne.0) then
          l2=link_daughter1(il1)
          do i2=1,nchi2
           il2=l2+i2-1  
           inside=qualify(il2)
           tree(il2)=-1
           if(qualify(il2).eq.1) then
            tree(il2)=1
            inside=qualify(il2)
        
            nchi3=children(il2)
            if(nchi3.ne.0)then
             l3=link_daughter1(il2)
             do i3=1,nchi3  
              il3=l3+i3-1
              inside=qualify(il3)
              tree(il3)=-1
              if(qualify(il3).eq.1) then
               tree(il3)=1
               inside=qualify(il3)   
           
               nchi4=children(il3)
               if(nchi4.ne.0) then
                l4=link_daughter1(il3)
                do i4=1,nchi4  
                 il4=l4+i4-1  
                 inside=qualify(il4)
                 tree(il4)=-1
                 if(qualify(il4).eq.1) then
                  tree(il4)=1
                  inside=qualify(il4)
              
                  nchi5=children(il4)
                  if(nchi5.ne.0) then
                   l5=link_daughter1(il4)
                   do i5=1,nchi5  
                    il5=l5+i5-1   
                    inside=qualify(il5)
                    tree(il5)=-1
                    if(qualify(il5).eq.1) then
                     tree(il5)=1
                     inside=qualify(il5)
                 
                     nchi6=children(il5)
                     if(nchi5.ne.0) then
                      l6=link_daughter1(il5)
                      do i6=1,nchi6  
                       il6=l6+i6-1   
                       inside=qualify(il6)
                       tree(il6)=-1
                       if(qualify(il6).eq.1) then
                        tree(il6)=1
                        inside=qualify(il6)
                       endif
                      enddo
                     endif
                 
                    endif
                   enddo
                  endif
                      
                 endif
                enddo
               endif
                       
              endif
             enddo
            endif

           endif
          enddo
         endif
                   
        endif
       enddo
      endif   ! if inside=0
           
      else
                       
      tree(I)=-1   
             
      endif
      return
           
      END          ! FIND_CHILDREN

