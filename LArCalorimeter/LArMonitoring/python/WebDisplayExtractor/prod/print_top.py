# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


cols = ['OFF_ID','ONL_ID','DET','AC','FT','SL','CH','SAM','ETA','PHI','HVLINES','HVCORR','CL','TT_COOL_ID']
DETlist = ['EMB','EMEC','HEC','FCAL']

########################################################################
def PrintFields(expert):
  out = '<tr>\n'
  for i in cols[:expert]+['STATUSUPD1','STATUSUPD4','RawChannels','CaloCells','Sporadic','LArCellEmpty','Clusters','Proposed Flag']: out += '<td>'+i+'</td>\n'
  print out+'</tr>\n'


########################################################################
def PrintTopRow(expert,opts, rawchannels,calocells,sporadic,larcellempty,clusters):
  colsWidths = {'OFF_ID':6,'ONL_ID':6,'DET':1,'AC':1,'FT':2,'SL':2,'CH':2,'SAM':1,'ETA':4,'PHI':4,'STATUSUPD1':6,'STATUSUPD4':6,'HVLINES':7,'HVCORR':5,'CL':2,'TT_COOL_ID':6}

  out = '<tr>\n'
  
  for i in cols[:expert]+['STATUSUPD1','STATUSUPD4']:
    selected = False
    if opts.has_key(i): selected = opts[i]

    out += '<td style="width:%dpx;">' % (10*colsWidths[i])
    if i=='DET':
      out += '<select name="DET" value="">\n'
      out += ' <option value=""></option>\n'
      for d in range(4):
        out += ' <option value="%d" ' % (d)
        if str(d)==selected: out += ' selected'
        out += '>'+DETlist[d]+'</option>\n'
      out += '</select>'
    elif i=='AC':
      out += '<select name="AC" value="">\n'
      for ac in [['',''],['A+','1'],['C-','-1']]:
        if ac[1]==selected: out += ' <option value="'+ac[1]+'" selected>'+ac[0]+'</option>\n'
        else:               out += ' <option value="'+ac[1]+'">'+ac[0]+'</option>\n'
      out += '</select>'
    else:
      out += '&nbsp;'
    out += '</td>\n'

  out +='<td>'
  for i,j in enumerate(['&gt;3&#963','&lt;-3&#963','&lt;E&gt','Sat.Q']):out += '<a class="det%d">%s</a>\n' % (i,j)
  out += '<input type="checkbox" name="rawchannels"'
  if rawchannels: out += ' checked="checked"'
  out += '></td><td>\n'
  
  for i,j in enumerate(['&gt;Eth','&gt;5&#963','DB','Q&gt;4000']):out += '<a class="det%d">%s</a>\n' % (i,j)
  out += '<input type="checkbox" name="calocells"'
  if calocells: out += ' checked="checked"'
  out += '></td><td>\n'

  for i,j in enumerate(['','E&gt;5-15','LB','Q&gt;4000']):
    if j != '': out += '<a class="det%d">%s</a>\n' % (i,j)
  out += '<input type="checkbox" name="sporadic"' 
  if sporadic: out += ' checked="checked"'
  out += '></td><td>\n'

  for i,j in enumerate(['&lt;E&gt;','E>1','LB','Q&gt;4000']):
    if j != '': out += '<a class="det%d">%s</a>\n' % (i,j)
  out += '<input type="checkbox" name="larcellempty"' 
  if larcellempty: out += ' checked="checked"'

  out += '></td>\n<td>&#916;R <input type="checkbox" name="clusters"'
  if clusters: out += ' checked="checked"'
  out += '></td>\n<td></td>\n</tr>\n'
  
  print out
