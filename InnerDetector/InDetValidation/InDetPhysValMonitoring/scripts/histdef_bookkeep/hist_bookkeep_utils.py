#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# utilities for  book-keeping xml histograms

import re

## --------------------------------------------------------
def weed(line):
    line=line.strip()
    line=re.sub(' +',' ',line)
    line=re.sub(' =','=',line)
    line=re.sub('= ','=',line)
    return line
## --------------------------------------------------------

## --------------------------------------------------------
def get_val(_key,line):
    _val=""
    line=weed(line)
    pieces=line.split(" ")
    for piece in pieces:
        if piece.startswith(_key+"="):
            _val=piece.split("\"")[1]
    return _val 
## --------------------------------------------------------

## --------------------------------------------------------
def get_hbuff(_id,_infname,_withcomment=True):
    # buffering the xml definition lines:
    _buff=[]
    _dobuff=False
    # buffering the comment lines
    _commbuff=[]
    _docommbuff=False
    
    # loop over file to find histo-s with id xml and comment:
    with open(_infname, 'r') as _f:
        for _line in _f:
            _wline=weed(_line)
            if _wline.startswith("<h id=\""+_id+"\""):
                _dobuff=True
                _buff=_commbuff
            elif _wline.startswith("</h>"):
                if (_dobuff):
                    _buff.append(_line)
                    # prepend comment
                _dobuff=False
                _commbuff=[]
                _docommbuff=False
            elif _withcomment and _wline.startswith("<!--"):
                if not _dobuff:
                    _commbuff.append(_line)
                    if ( not re.search("-->",_wline) ):
                        # multi-line comment
                        _docommbuff=True
                    else:
                        _docommbuff=False
            else:
                if (_docommbuff):
                    _commbuff.append(_line)
                    if ( re.search("-->",_wline) ):
                        _docommbuff=False
                elif not is_xml_form(_wline): 
                    print 'Warning', _infname, 'non-xml formatted line :', _line
                    
            # buffer histo lines here:
            if (_dobuff):
                _buff.append(_line)
                
    return _buff
## --------------------------------------------------------

## --------------------------------------------------------
## take a buffer of histogram + comment, and split it in comment+xml def
def get_comm_def(_buff):
    _comm_def=[[],[]]
    _iscomm=False
    for _bitem in _buff:
        if _bitem.strip().startswith("<!--"):
            _iscomm=True
            _comm_def[0].append(_bitem)
            if ( re.search("-->",_bitem) ):
                _iscomm=False
        elif ( _iscomm):
            _comm_def[0].append(_bitem)
            if re.search("-->",_bitem):
                _iscomm=False
        else:
            _comm_def[1].append(_bitem)

    return _comm_def
    
## --------------------------------------------------------
## check for non-xml formated lines:
def is_xml_form(_line):
    _line=_line.strip()
    if _line.startswith("<") or _line.endswith(">"):
        # assume these are .xml-s
        return True
    if not _line:
        # empty line, that is '' after strip 
        return True
    # any other exceptions?
    if _line.startswith("&"):
        # including daughters
        return True
    else:
        return False
