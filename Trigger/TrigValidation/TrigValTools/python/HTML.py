# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""Classes to generate HTML in Python

This class was adapted from http://code.activestate.com/recipes/366000/
The changes was mostly related to generate valid XHTML

The HTMLTags module defines a class for all the valid HTML tags, written in
 uppercase letters. To create a piece of HTML, the general syntax is :

    t = TAG(innerHTML, key1=val1,key2=val2,...)

so that "print t" results in :

    <TAG key1="val1" key2="val2" ...>innerHTML</TAG>

For instance :

    print A('bar', href="foo") ==> <A href="foo">bar</A>

To generate HTML attributes without value, give them the value True :

    print OPTION('foo',SELECTED=True,value=5) ==> 
            <OPTION value="5" SELECTED>

For non-closing tags such as <IMG> or <BR>, the print statement does not 
generate the closing tag

The innerHTML argument can be an instance of an HTML class, so that you can nest 
tags, like this :

    print B(I('foo')) ==> <B><I>foo</I></B>

Instances of the HTML classes support the addition :

    print B('bar')+INPUT(name="bar") ==> <B>bar</B><INPUT name="bar">

and also repetition :

    print TH('&nbsp')*3 ==> <TD>&nbsp;</TD><TD>&nbsp;</TD><TD>&nbsp;</TD>

If you have a list (or any iterable) of instances, you can't concatenate the items with 
sum(instanceList) because sum takes only numbers as arguments. So there is a 
function called Sum which will do the job :

    Sum( TR(TD(i)+TD(i*i)) for i in range(100) )

generates the rows of a table showing the squares of integers from 0 to 99

A simple document can be produced by :

    print HTML( HEAD(TITLE('Test document')) +
        BODY(H1('This is a test document')+
             TEXT('First line')+BR()+
             TEXT('Second line')))

This will produce :

    <HTML>
    <HEAD>
    <TITLE>Test document</TITLE>
    </HEAD>
    <BODY>
    <H1>This is a test document</H1>
    First line
    <BR>
    Second line
    </BODY>
    </HTML>

If the document is more complex it is more readable to create the elements 
first, then to print the whole result in one instruction. For example :

stylesheet = LINK(rel="Stylesheet",href="doc.css")

head= HEAD(TITLE('Record collection')+stylesheet)
title = H1('My record collection')
rows = Sum ([TR(TD(rec.title,Class="title")+TD(rec.artist,Class="Artist")) 
    for rec in records])
table = TABLE(TR(TH('Title')+TH('Artist')) + rows)

print HTML(head + BODY(title + table))
"""

import cStringIO

def XHTML(document):
    head="""<?xml version="1.0" encoding="iso-8859-1"?>
<!DOCTYPE html
        PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
         "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
"""
    return head+HTML(document,
                     xmlns="http://www.w3.org/1999/xhtml",
                     lang="en-US",
                     xml__lang="en-US")



class TAG:
    """Generic class for tags"""
    def __init__(self, innerHTML="", **attrs):
        self.tag = [self.__class__.__name__]
        self.innerHTML = [innerHTML]
        self.attrs = [attrs]
    
    def __str__(self):
        res=cStringIO.StringIO()
        w=res.write
        for tag, innerHTML, attrs in zip(self.tag, 
            self.innerHTML, self.attrs):
            if not tag=='TEXT':
                if tag in _WS_INSENSITIVE:
                    w('\n')
                w("<%s" %tag.lower())
                # attributes which will produce arg = "val"
                attr1 = [ k for k in attrs if not isinstance(attrs[k],bool) ]
                w("".join([' %s="%s"' 
                    %(k.replace('__',':').replace('_','-').replace('Class','class'),attrs[k]) for k in attr1]))
                # attributes with no argument
                # if value is False, don't generate anything
                attr2 = [ k for k in attrs if attrs[k] is True ]
                w("".join([' %s="%s"' %(k,k) for k in attr2]))
                if not tag in ClosingTags:
                    w(" /")
                w(">")
            if type(innerHTML)==unicode:
                w(innerHTML.encode('ascii','ignore'))
            else:
                w(str(innerHTML))
            if tag in ClosingTags:
                w("</%s>" %tag.lower())
        return res.getvalue()

    def __add__(self,other):
        """Concatenate another tag to self"""
        self.tag += other.tag
        self.innerHTML += other.innerHTML
        self.attrs += other.attrs
        return self

    def __radd__(self,other):
        """Used to add a tag to a string"""
        if isinstance(other,str):
            return TEXT(other)+self
        else:
            raise ValueError,"Can't concatenate %s and instance" %other

    def __mul__(self,n):
        """Replicate self n times"""
        res = TAG()
        res.tag = self.tag*n
        res.innerHTML = self.innerHTML*n
        res.attrs = self.attrs*n
        return res

# list of tags, from the HTML 4.01 specification

ClosingTags =  ['A', 'ABBR', 'ACRONYM', 'ADDRESS', 'APPLET',
            'B', 'BDO', 'BIG', 'BLOCKQUOTE', 'BUTTON',
            'CAPTION', 'CENTER', 'CITE', 'CODE',
            'DEL', 'DFN', 'DIR', 'DIV', 'DL',
            'EM', 'FIELDSET', 'FONT', 'FORM', 'FRAMESET',
            'H1', 'H2', 'H3', 'H4', 'H5', 'H6',
            'I', 'IFRAME', 'INS', 'KBD', 'LABEL', 'LEGEND',
            'MAP', 'MENU', 'NOFRAMES', 'NOSCRIPT', 'OBJECT',
            'OL', 'OPTGROUP', 'PRE', 'Q', 'S', 'SAMP',
            'SCRIPT', 'SELECT', 'SMALL', 'SPAN', 'STRIKE',
            'STRONG', 'STYLE', 'SUB', 'SUP', 'TABLE',
            'TEXTAREA', 'TITLE', 'TT', 'U', 'UL',
            'VAR', 'BODY', 'COLGROUP', 'DD', 'DT', 'HEAD',
            'HTML', 'LI', 'P', 'TBODY','OPTION', 
            'TD', 'TFOOT', 'TH', 'THEAD', 'TR']

NonClosingTags = ['AREA', 'BASE', 'BASEFONT', 'BR', 'COL', 'FRAME',
            'HR', 'IMG', 'INPUT', 'ISINDEX', 'LINK',
            'META', 'PARAM']

# create the classes
for tag in ClosingTags + NonClosingTags + ['TEXT']:
    exec("class %s(TAG): pass" %tag)
    
def Sum(iterable):
    """Return the concatenation of the instances in the iterable
    Can't use the built-in sum() on non-integers"""
    it = [ item for item in iterable ]
    if it:
        return reduce(lambda x,y:x+y, it)
    else:
        return ''

def SP():
    return TEXT('&nbsp;')


def htmlify(text):
    """
    convert special characters to html symbols
    """
    html=text.replace("&",'&#38;')
    html=html.replace('"','&#34;')
    html=html.replace("'",'&#39;')
    html=html.replace("<",'&#60;')
    html=html.replace(">",'&#62;')
    return html


# whitespace-insensitive tags, determines pretty-print rendering
_WS_INSENSITIVE = NonClosingTags + ['HTML','HEAD','BODY',
    'FRAMESET','FRAME',
    'TITLE','SCRIPT',
    'TABLE','TR','TD','TH','SELECT','OPTION',
    'FORM']

_WS_INSENSITIVE.remove('INPUT')
