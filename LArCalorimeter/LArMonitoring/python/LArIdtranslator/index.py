#!/usr/bin/env python2.5

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
print 'Content-type: text/html;charset=utf-8\n'

########################################################################
cols = ['OFF_ID','ONL_ID','TT_COOL_ID','DET','AC','FT','SL','CH','SAM','ETA','PHI','HVLINES','HVCORR','CL','IETA','IPHI','R','Z','FTNAME','P1_ID','FEB','DSP','PU','ROL','ROS_ROL','ROD','FEB_ID']
DETlist = ['EMB','EMEC','HEC','FCAL']
ACmap = {1:'A+','1':'A+','A+':'A+',-1:'C-','-1':'C-','C-':'C-'}
BadChannels = ['deadReadout','deadCalib','deadPhys','almostDead','short','unstable','distorted','lowNoiseHG','highNoiseHG','unstableNoiseHG','lowNoiseMG','highNoiseMG','unstableNoiseMG','lowNoiseLG','highNoiseLG','unstableNoiseLG','missingFEB','peculiarCL','problematicFor?','sporadicBurstNoise']
BadFEBs = {0:'deadAllBit',1:'deadReadoutBit',2:'inErrorBit',3:'deactivatedInOKSBit',16:'maskParity',17:'maskBCID',18:'maskSampleHeader',19:'maskEVTID',20:'maskScacStatus',21:'maskScaOutOfRange',22:'maskGainMismatch',23:'maskTypeMismatch',24:'maskNumOfSamples',25:'maskEmptyDataBlock',26:'maskDspBlockSize',27:'maskCheckSum',28:'maskMissingHeader',29:'maskBadGain'}

########################################################################
def PrintFields():
  out = '<tr>\n'
  for i in cols[:expert]+['STATUSUPD1','STATUSUPD4']: out += '<td>'+i+'</td>\n'
  print out+'</tr>\n'

########################################################################
def PrintTopRow():
  colsWidths = {'OFF_ID':12,'ONL_ID':12,'TT_COOL_ID':12,'DET':1,'AC':1,'FT':1,'SL':1,'CH':1,'SAM':1,'ETA':4,'PHI':4,'HVLINES':10,'HVCORR':5,'CL':4,'IETA':3,'IPHI':3,'R':5,'Z':5,'FTNAME':5,'P1_ID':5,'FEB':16,'DSP':18,'PU':14,'ROL':23,'ROS_ROL':22,'ROD':12,'FEB_ID':9}
  out = '<tr>\n'

  for i in cols[:expert]:
    selected = False
    if query.has_key(i): selected = query[i]
    out += '<td>'
    if i=='DET':
      out += '<select name="DET" value="">\n'
      out += ' <option value=""></option>\n'
      for d in range(4):
        out += ' <option value="%d" ' % (d)
        if selected==str(d): out += ' selected'
        out += '>'+DETlist[d]+'</option>\n'
      out += '</select>'
    elif i=='AC':
      out += '<select name="AC" value="">\n'
      for ac in [['',''],['A+','1'],['C-','-1']]:
        if ac[1]==selected: out += ' <option value="'+ac[1]+'" selected>'+ac[0]+'</option>\n'
        else:               out += ' <option value="'+ac[1]+'">'+ac[0]+'</option>\n'
      out += '</select>'
    else:
      out += '<input size="%d" type="text" name="%s" ' % (colsWidths[i],i)
      if selected: out += 'value="'+selected+'" '
      out += ' />'
    out += '</td>\n'

  for i in ['STATUSUPD1','STATUSUPD4']:
    selected = False
    if query.has_key(i): selected = query[i]
    out += '<td><select name="'+i+'" value="">\n'
    out += ' <option value=""></option>\n'
    out += ' <option value="0x8FFFF"'
    if selected=='0x8FFFF': out += ' selected'
    out += '>any (!missingFEB)</option>\n'
    for p in range(20):
      out += ' <option value="%s" ' % (str(hex(1<<p)))
      if selected==str(hex(1<<p)): out += 'selected'
      out += '>'+BadChannels[p]+'</option>\n'
    out += '</select>'
    out += '</td>\n'

  print out+'</tr>\n'

########################################################################
def PrintRows(Cmds):
  import sqlite3,operator
  #print Cmds,'<br>'

  # fetch badchannels
  fields = cols[:expert]
  B = [{},{},{},{}]
  run = opts['run']
  if run>0:
    if expert<12: fields.append(cols[-1])
    conn = sqlite3.connect(dbfile)
    c = conn.cursor()
    for u,upd in enumerate(['1','4']):
      for b,bad in enumerate(['BadChannels','MissingFEBs']):
        c.execute('select status from %s where %d>=since and %d<=until'%(bad+'UPD'+upd,run,run))
        for d in c.fetchall(): B[2*u+b].update(eval(str(d[0])))
    conn.close()

  # if STATUS query, get the ONL_ID/FEB_ID of the badchannels/missingfebs
  cmd = 'select '+','.join(fields)+' from LARID where '+Cmds[0]
  for u in range(2):
    if len(Cmds[1][u])>0:
      bit = eval(Cmds[1][u])
      theBad = []
      status_cmd = ''
      if len(Cmds[0])>0: status_cmd += ' and'
      status_cmd += ' ONL_ID in ('
      Bad = B[2*u]
      for b in Bad.keys():
        if Bad[b]&bit!=0x0: theBad.append(str(b))
      if len(theBad)>0: cmd += status_cmd+','.join(theBad)+')'

      if bit&(1<<16)!=0x0:
        status_cmd = ''
        if len(Cmds[0])>0: status_cmd += ' and'
        theBad = []
        status_cmd += ' FEB_ID in ('
        Bad = B[2*u+1]
        for b in Bad.keys(): theBad.append(str(b))
        if len(theBad)>0: cmd += status_cmd+','.join(theBad)+')'

  # connect to LArId and get the channels
  conn = sqlite3.connect('/afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArId.db')
  #conn = sqlite3.connect('/afs/cern.ch/user/l/larmon/public/LArIdtranslator/LArId_new.db')
  cursor = conn.cursor()
  cursor.execute(cmd+' limit 1000')
  r = cursor.fetchall()
  if len(r)==1000: print '<tr><td colspan="%d">**** more than 1000 results for this query ****</td></tr>\n' % (expert+2)
  global SortedChannels
  SortedChannels = sorted(r,key=operator.itemgetter(3,5,6,7))

  nRows = 0
  scan = [1,2,3,4,5,6,7,8,11,13]  
  T = []
  for j in scan: T.append(set())

  for i in SortedChannels:
    bec = 0
    if i[3]>0: bec = 1
    side = 0  
    if i[4]>0: side = 1
    fmt = "'%d %d %2d %2d %3d 0 %s # %s'" % (bec,side,i[5],i[6],i[7],'ProposedFlag',str(hex(i[1])))
    out = '<tr class="out'+str(nRows%2)+'" ondblclick="PrintFormat('+fmt+')" onclick="ChangeColor(this)" >\n'
    out += '<td>'+str(hex(i[0]))+'</td><td>'+str(hex(i[1]))+'</td><td>'+str(hex(i[2]))+'</td>\n'
    out += '<td align="right">'+DETlist[i[3]]+'</td><td align="left">'+ACmap[i[4]]+'</td>\n'
    for j in i[5:9]: out += '<td>'+str(j)+'</td>\n'
    for j in i[9:11]: out += '<td>%.4f</td>\n' % (j)
    for j,J in enumerate(scan[:8]): T[j].add(i[J])
    if expert>11:
      out += '<td>%s</td>\n' % (i[11])
      out += '<td>%.4f</td>\n' % (i[12])
      out += '<td>%s</td>\n' % (i[13])
      for j in i[14:16]: out += '<td>%d</td>\n' % (j)
      for j in i[16:18]: out += '<td>%.2f</td>\n' % (j)
      for j in range(8): out += '<td>%s</td>\n' % (i[j+18])
      out += '<td>'+str(hex(i[26])).rstrip('L')+'</td>\n'
      for k in i[11].split(): T[8].add(k)
      for k in i[13].split(): T[9].add(k)
    for k in range(2):
      status = ''
      if B[2*k].has_key(i[1]): 
        bit = B[2*k][i[1]]
        for j in range(20): 
          if bit&(1<<j)!=0x0: status += BadChannels[j]+' '
      if B[2*k+1].has_key(i[-1]):
        bit = B[2*k+1][i[-1]]
        for j in BadFEBs.keys():
          if bit&(1<<j)!=0x0: status += BadFEBs[j]+' '
      out += '<td>'+status.rstrip(' ')+'</td>\n'
    print out+'</tr>\n'

    nRows += 1
    if nRows%32==0: PrintFields()

  print '<tr>'
  for j in range(expert):
    if j in scan: print '<td>%d</td>'%(len(T[scan.index(j)]))
    else: print '<td></td>'
  print '<td></td><td></td></tr>'

########################################################################
def ProcessQuery():
  where = ''
  status = ['','']
  sql_query = ''

  if query.has_key('sql_query'):
    # parse the sql_query 
    sql_query = query['sql_query'].replace('  ',' ').replace('"',"'") 
    for i in ['OR','AND','BETWEEN','IN']:
      if i in sql_query: sql_query = sql_query.replace(i,i.lower())
    for i in cols:
      if i.lower() in sql_query: sql_query = sql_query.replace(i.lower(),i.upper())
    for i in [',','=']:
      sql_query = sql_query.replace(' '+i,i)
      sql_query = sql_query.replace(i+' ',i)

    # create the where of the sql query
    I = sql_query.split()
    for i in I:
      if   'UPD1' in i: 
        status[0] = (i.split('!')[0]).lstrip('STATUSUPD1&')
        where = where.rstrip('and ')+' '  
      elif 'UPD4' in i: 
        status[1] = (i.split('!')[0]).lstrip('STATUSUPD4&')
        where = where.rstrip('and ')+' '  
      elif '_ID' in i:
        if '=' in i:
          j=i.split('=')
          where += j[0]+'='+str(eval(j[1]))+' '
        elif I.pop(I.index(i)+1)=='in':
          tup = eval(I.pop(I.index(i)+1))
          if type(tup)=='tuple':
            for k in range(1,len(tup)): where += ','+str(tup[k])
          else: where += i+' in '+str(tup)
          where += ' '
      elif 'DET' in i:
        for key in ['=','<=','=>','<','>']:
          if key in i:
            j=i.split(key)
            if j[1] in DETlist: where += j[0]+key+str(DETlist.index(j[1]))+' '
            else: where += i+' '
            break                                                                   
      else: where += i+' '

    sql_query = where

    # from the sql_query, get the selection for the tabs
    if 'or' in sql_query: Q = sql_query.split('or')[0].split('and')
    else: Q = sql_query.split('and')
    for q in Q:
      #for i in ['<=','=>','<','>','=','like','between','in']:
      for i in ['=']:
        if i in q: 
          sq = q.split(i)
          key = sq[0].upper().replace(' ','')
          sq[1] = sq[1].replace(' ','')
          if i=='between':
            low = eval(sq[1].replace(' ',''))
            high = eval(Q.pop(Q.index(q)+1).replace(' ',''))
            query[key] = str(0.5*(high+low))
            if key=='ETA': opts['deta']=0.5*(high-low)
            if key=='PHI': opts['dphi']=0.5*(high-low)
          elif i!='in': query[key]=sq[1].replace("'",'')
          if key=='DET':
            if query[key] in DETlist: query[key]=str(DETlist.index(query[key]))
          break

  else:
    # build the "where" query from the tabs
    for i in cols:
      if not query.has_key(i): continue
      v = query[i]
      if (',' in v) and (i in ['FT','SL','CH','SAM','IETA','IPHI']): 
        w = v.split(',')
        where += '%s>=%s and %s<=%s' % (i,w[0],i,w[1])
      elif i=='AC': 
        if v[0]=='A': v = '1'
        elif v[0]=='C': v = '-1'
        where += '%s=%s' % (i,v)
      elif i=='DET':
        if v in DETlist: where += '%s=%s' % (i,DETlist.index(v))
        else: where += '%s=%s' % (i,v)
      elif i=='ETA': 
        if not opts.has_key('deta'): opts['deta'] = 0.025 
        where += '%s between %s and %s' % (i,str(float(v)-opts['deta']),str(float(v)+opts['deta']))
      elif i=='PHI': 
        if not opts.has_key('dphi'): opts['dphi'] = 0.05 
        where += '%s between %s and %s' % (i,str(float(v)-opts['dphi']),str(float(v)+opts['dphi']))
      elif i in ['HVLINES','FTNAME','CL','P1_ID','FEB','DSP','PU','ROL','ROS_ROL','ROD']: where += i+" like '%"+v+"%'"
      elif '_ID' in i: where += '%s=%s' % (i,str(eval(v)))
      else: where += '%s=%s' % (i,v)
      where += ' and '
    where = where.rstrip(' and ').rstrip(' ')

    sql_query = ''
    for i in where.split():
      if '_ID' in i:
        j=i.split('=')
        sql_query += j[0]+'='+str(hex(int(j[1])))+' '
      elif 'DET' in i:
        j=i.split('=')
        sql_query += j[0]+'='+DETlist[int(j[1])]+' '
      else: sql_query += i.replace('"',"'")+' '

    for t,T in enumerate(['STATUSUPD1','STATUSUPD4']):
      if query.has_key(T): status[t] = query[T]

  for t,T in enumerate(['STATUSUPD1','STATUSUPD4']):
    if len(status[t])>0: 
      if len(sql_query)>0: sql_query += 'and '
      sql_query += T+'&'+status[t]+'!=0x0 '
  print '<input type="text" name="sql_query" value="'+sql_query+'"/>\n</form></td>'
  print '<td style="text-align:right;width:40%"><form name="selection" id="selection" method="post">'
  n = {'deta':951,'dphi':966}
  for i in ['deta','dphi']:
    print '&#916;&#%d;=<input type="text" size="3" name="%s" ' % (n[i],i)
    if opts.has_key(i): print 'value="'+str(opts[i])+'" '
    print '/>'

  return [where,status]

########################################################################
import os,time,cgi
#import cgitb; cgitb.enable()

dbfile = '/afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArBad.db'
f0 = os.stat(dbfile)
f1 = os.stat('index.py')
from datetime import datetime
f0 = os.stat(dbfile)

print '''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>LArIdtranslator</title>
<meta name="author" content="Mathieu Plamondon [Mathieu.Plamondon@cern.ch]"/>
<link rel="icon" type="image/vnd.microsoft.icon" href="favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />
<script type="text/javascript" src="scripts.js"></script>
<base target="_self">
</head>
<body>
<div class="wrapper"><br />
<a href="https://atlas-larmon.cern.ch/LArIdtranslator" style="color:#000000;font-size:20pt;font-weight:bold" target="_self">LArID translator browser</a>'''
#<br><font style="color:#FF0000">Is using a new db file (new ROS_ROL mapping, new FCAL2 hv mapping, hospital hv lines): please report any problem</font><br>
#td = (datetime.today()-datetime.fromtimestamp(f0.st_ctime))
#if (td.microseconds + (td.seconds + td.days * 24 * 3600) * 10**6) / 10**6 > 12*3600:
#  print '<font style="color:#FF0000">DB file is old: regenerate it!</font>'
#  #print '<font style="color:#FF0000">Development in progress (1 hour)</font>'

print '''
<table style="text-align:right;width:70%">
<tr>
<td style="text-align:right;width:50%">
<form name="query" id="query" method="post">'''

global query
global opts
query = {}
opts = {'run':999999}
c = cgi.FieldStorage()
expert = 11
for i in c.keys():
  a = c.getvalue(i)
  if type(a)==type([]): a = a[0]
  if i in ['deta','dphi','run']: opts[i] = float(a)
  elif i=='expert': expert=28
  elif i=='expert1': expert=28
  elif i=='sql_query': query[i] = a
  else: query[i.upper()] = a
the_url='https://atlas-larmon.cern.ch/LArIdtranslator/index.py'

if expert>11: print '<input type="hidden" name="expert1" value="1" />'
if len(query)>0:
  the_url += '?'
  for k in query.keys(): the_url += k+'='+query[k]+'&'
  if expert>11: the_url += 'expert=1&'
  the_url += 'run=%d'%(opts['run'])
  the_url = the_url.rstrip('&').replace(' ','%20')
  if query.has_key('expert'): expert=27

  if c.has_key('info'):
    the_url = 'https://atlas-larmon.cern.ch/WebDisplayExtractor/LArWebDisplayExtractor?'
    for k in query.keys(): the_url += k+'='+query[k]+'&'
    the_url = the_url.rstrip('&').replace(' ','%20')
    info = int(c.getvalue('info'))
    if info in [0,4]: the_url='&info=0' 
    elif info==1: the_url='&info=1&rawchannels=on' 
    elif info==2: the_url='&info=1&calocells=on' 
    elif info==3: the_url='&info=1&sporadic=on' 
    print '<a href="%s">Click here to be redirected to the new WebExtractor</a>' % (the_url) 
    import sys
    sys.exit()  

  Cmds = ProcessQuery()
else:
  print '''
<input type="text" name="sql_query"/>
</form>
</td>
<td style="text-align:right;width:50%">
<form name="selection" id="selection" method="post">
&#916;&#951;=<input type="text" size="3" name="deta" />
&#916;&#966;=<input type="text" size="3" name="dphi" />'''

out = 'run:<input type="text" size="6" name="run" value="%d"/>' % (opts['run'])
out += '&nbsp;expert:<input type="checkbox" name="expert" '
if expert>11: out += 'checked="checked"'
out += ' />&nbsp;'
out += '<input type="submit" value="Submit"/>\n'
out += '<input type="reset" value="Reset" onclick="ResetForm(1)" />\n'
out += '<input type="reset" value="Help" onclick="putHelper()" />\n'
print out+'</td>\n'
print '</tr></table>\n<br />'
print '<table class="list">\n' 
PrintFields()
PrintTopRow()
if 'Cmds' in dir(): PrintRows(Cmds)

print '''
</form>
</table>
<div id="help">'''
if len(query)>0:
  from operator import itemgetter
  print 'eta-phi range: [%.4f,%.4f] x [%.4f,%.4f]<br />' %(min(SortedChannels,key=itemgetter(9))[9],max(SortedChannels,key=itemgetter(9))[9],min(SortedChannels,key=itemgetter(10))[10],max(SortedChannels,key=itemgetter(10))[10])
  #print '&#951;-&#966; range: [%.4f,%.4f] x [%.4f,%.4f]<br />' %(min(SortedChannels,key=itemgetter(9))[9],max(SortedChannels,key=itemgetter(9))[9],min(SortedChannels,key=itemgetter(10))[10],max(SortedChannels,key=itemgetter(10))[10])
  print '<a href="%s" target="_blank">Link to this query</a><br />'%(the_url)


tag_upd1 = ""
tag_upd4 = ""
ftag = open("/afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/usedTags.txt")
for line in ftag:
#    print line
    if 'LArBad.db UPD1' in line : tag_upd1 = line.split(" = ")[1]
    if 'LArBad.db UPD4' in line : tag_upd4 = line.split(" = ")[1]

print '''
</div>
<div class="push"></div>
</div>
<div class="footer">
Developer: <a href="mailto:???">???</a><br />
Last updates:&nbsp &nbsp
LArBad.db file (%s)
&nbsp &nbsp &nbsp &nbsp Tags : UPD1=%s,  UPD4=%s
&nbsp &nbsp &nbsp &nbsp <a href="https://svnweb.cern.ch/trac/atlasoff/browser/LArCalorimeter/LArMonitoring/trunk/python/LArIdtranslator" target="_blank">script file</a> (%s)
</div>
</body>
</html>''' % (time.ctime(f0.st_ctime),tag_upd1,tag_upd4,time.ctime(f1.st_ctime))

#if len(query)>0:
  #ip = os.environ['HTTP_X_FORWARDED_FOR']
  #import urllib2
  #req = urllib2.Request("http://www.ipmap.com/"+ip)
  #response = urllib2.urlopen(req)
  #pg = response.read()
  #pg = pg[pg.find('<table'):pg.find('<div id="footer"')]
  #os.system('echo "%s" | mail -s "LArIdtranslator query %s" Mathieu.Plamondon@cern.ch' % (the_url,ip))
