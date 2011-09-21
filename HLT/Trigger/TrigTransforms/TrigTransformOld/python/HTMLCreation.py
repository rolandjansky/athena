# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# vim: set fileencoding=utf-8 :
# Created by Hegoi Garitaonandia <hegoi.garitaonandia@cern.ch>
from TrigTransformOld.WebLayout import * 
import os
from TrigTransformOld.HTMLTags import * 
from TrigTransformOld.Utils import *

def hateMail():        
        return SMALL(          mailto['string']   + str(A(mailto['name'],HREF=mailto['to']))               )

def links(runno):
        acumulative = ''
        for l1,l2 in all_inside_links:
                        ll = os.path.join(os.path.join(D_WEBBASE,runno),l2)
                        acumulative += str( LI(SMALL(A(l1,HREF=ll)) ))
        for l1,l2 in all_outside_links:
                        acumulative += str( LI(SMALL(A(l1,HREF=l2)) ))
        summary = TABLE( acumulative )
        return summary

def table2D(array2):
        acumulative = ''
        for  line in array2:
            textline = ''        
            for element in line:
                textline += str(  TD(element,ALIGN="LEFT") ) 
            line = TR (textline, VALIGN="MIDLE"   )
            acumulative += str( line )
        summary = TABLE( acumulative ,BORDER="2" , CELLSPACING="0" , CELLPADING="7" )
        return summary 

def tableLink(array,name,runno,doff):
        acumulative = ''
        for  (gid,file,offset,logfile,ref) in array:
                link  = os.path.join (os.path.join(os.path.join(D_WEBBASE, runno), doff), ref)
                line = TR (      TD(gid,ALIGN="LEFT")     +    TD(file,ALIGN="LEFT")  +  TD(offset,ALIGN="LEFT") +  TD(A(logfile,HREF=link),ALIGN="LEFT") , VALIGN="MIDLE"   )
                acumulative += str( line )
        summary = TABLE( acumulative ,BORDER="2" , CELLSPACING="0" , CELLPADING="7" )
        return summary 

def table(array):
        acumulative = ''
        for (l1,l2) in array:
                        line = TR (      TD(l1,ALIGN="LEFT")     +    TD(l2,ALIGN="LEFT")  , VALIGN="MIDLE"   )
                        acumulative += str( line )
        summary = TABLE( acumulative , BORDER="2" , CELLSPACING="0" , CELLPADING="7"  )
        return summary 

def _list(array):
        acumulative = ''
        for l in array:
                        line = TR (      TD(l,ALIGN="LEFT")        )
                        acumulative += str( line )
        summary = TABLE( acumulative   ,BORDER="2" , CELLSPACING="0" , CELLPADING="7")
        return summary 

def textPage(filename,mytitle,myhead,mytext,runno):
        logging.info('creating web page %s' + filename)
        _page(filename,mytitle,myhead,mytext,runno)
        
def listPage(filename,mytitle,myhead,mylist,runno):
        logging.info('creating web page %s' + filename)
        _page(filename,mytitle,myhead,_list(mylist),runno)

def tablePage(filename,mytitle,myhead,mytable,runno):
        logging.info('creating web page %s' + filename)
        _page(filename,mytitle,myhead,table(mytable),runno)
                
def _page(filename,mytitle,myinit,payload,runno):
        """Takes mybody which is a piece of HTML and sets it inside an EMPTY webpage""" 
        #####################################################
        # file
        logging.info('creating web page %s' + filename)
        thefile =         file(filename,'w')        
        

        #####################################################
        # head
        reload         =         "" #REFRESH_META(300)
        title         =          TITLE(mytitle)
        head         =         HEAD(reload+str(title))

        
        #####################################################
        # body
        image         =         IMG( SRC="/img/gear.jpg",ALT="gear",WIDTH="90",HEIGHT="90" )
        #top        =         CENTER( H2(myinit) + BR() + image )
        top        =         CENTER( H2(myinit) + BR()  )
        list        =         [ top  , payload , BR() , CENTER(links(runno)),  hateMail() , search ]
        txt = ''
        for element in list:
                txt += str(element)

        body         =         BODY(TABLE( txt  , BORDER='5', WIDTH="640", ALIGN="LEFT"),  BGCOLOR="#ffffff" , TEXT="#000000", VLINK="#292555", LINK="#292555", ALINK="#cc0033",style="margin-top: 0") 

        #####################################################
        # html
        page         =         HTML( head + body ) 
        thefile.write(str(page))
        thefile.close()

def Root2HTML(directory,rootfile):
        my_mkdir(directory)        
        cmd = "root2html.py --outDir=%s %s" % (directory,rootfile)
        logging.warning( "executing %s" % cmd)
        (ret,out,err)=runcmd(cmd.split(' '))
        if ret !=0 :
                logging.warning( "can't create web page for %s" % rootfile)
                logging.warning( err)

