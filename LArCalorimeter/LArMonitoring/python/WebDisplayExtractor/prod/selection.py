# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

########################################################################
def PrintSelection(run_spec,info,cluster_threshold,beambkg,expert,includenotready,ladies,cluster_E):

  out = '<form name="selection" id="selection" method="post" target="_self">\n'
  out += '<div style="text-align:left" id="queryrow">\n'

  # runs menu
  out +=  '<br><b>LADIeS Options -- </b>'
  out += '<br> &nbsp;&nbsp; Run Number:<input type="text" size="6" name="low_run" '
  if run_spec.has_key('low_run'): out += 'value="'+run_spec['low_run']+'" '

   # job menu
  out += '/>&nbsp;&nbsp;\n Report Type: '
  out += '<select name="ladies" style="text-align:left">'
  for j,job in enumerate(['LADIeS-express','LADIeS-bulk', 'ExpertOptions-express','ExpertOptions-bulk']):
    out += '<option value="'+str(j)+'"'
    if j==ladies: out+= ' selected'
    out += '>'+job+'</option>'
  out += '</select>\n'


  # job menu
  out += '&nbsp;&nbsp;\n  '
  out += '<select name="info" style="text-align:left">'
  for j,job in enumerate(['Run Report','Noisy LAr Cells']):
    out += '<option value="'+str(j)+'"'
    if j==info: out+= ' selected'
    out += '>'+job+'</option>'
  out += '</select>\n'

  
  #out += '<br>'

  # buttons
  out += '&nbsp;<input type="submit" value="Submit"/>\n'
  out += '<input type="reset" value="Reset" onclick="ResetForm()" />\n'
  out += '<input type="button" value="Help" onclick="Helper()" />\n'
  

  # end option section
  out += '</div>\n'
  
  print out+'<br />'

  
  out = '<div style="text-align:left" id="expertrow">\n'

  # stream menu
  out += '<b>Expert Options (Stream, source and proc. version are defined by "Report Type" unless "ExpertOptions" is selected) -- <br></b>'
  #out += '/>&nbsp;&nbsp;Data stream:<select name="stream" value="CosmicCalo" style="text-align:left;">'
  out += '&nbsp;&nbsp;Data stream:<select name="stream" style="text-align:left;">'
  ST = run_spec['stream'].split('_')[1]
  for st in ['CosmicCalo','JetTauEtmiss','Egamma','express','MinBias','HardProbes']:
    out += '<option value="'+st+'"'
    if st==ST: out+= ' selected'
    out += '>'+st+'</option>'
  out += '</select>&nbsp;&nbsp;\n'
    
  # source menu
  out += 'Source:<select name="source" style="text-align:left;">'
  for source in ['tier0','reproc','larcomm','test']: 
    out += '<option value="'+source+'"'      
    if source==run_spec['source']: out+= ' selected'
    out += '>'+source+'</option>' 
  out += '</select>&nbsp;&nbsp;'
      
  # proc_ver menu
  out += 'Processing_version:<input name="proc_ver" size="5" style="text-align:left" value="%d" />&nbsp;&nbsp;' % (run_spec['proc_ver'])

  # end expert section
  out += '</div>\n'
  
  print out+'<br />'

  out = '<div style="text-align:left" id="Advanced">\n'

  # stream menu
  out += '<b> Advanced options (do not touch unless you know what you do!) -- <br></b>'     
      
  # cluster theshold
  out += 'Clusters [evts]:<input name="cluster_threshold" size="2" style="text-align:left" value="%d" />&nbsp;&nbsp;' % (cluster_threshold)

  # cluster E menu
  out += 'Cluster E:<select name="cluster_E" style="text-align:left;">'
  for clus_e in ['Et>10GeV','Et>5GeV','E>10GeV','E>5GeV']: 
    out += '<option value="'+clus_e+'"'
    if clus_e==cluster_E: out+= ' selected'
    out += '>'+clus_e+'</option>' 
  out += '</select>&nbsp;&nbsp;'
  
  # options
  out += 'BeamBkg:<input type="checkbox" name="beambkg" '
  if beambkg: out += 'checked="checked" />\n'
  else:out += '/>\n'
    
    
  #out += '&nbsp;Expert Output:<input type="checkbox" name="expert" '
  #if expert>10:out += 'checked="checked" />\n'
  #else:out += '/>\n'
        
        
  # option to include non ATLAS ready LBs  &nbsp;
  out += '&nbsp;&nbsp;&nbsp;Include ATLAS not ready LBs:<input type="checkbox" name="includenotready" '
  if includenotready: out += 'checked="checked" />'
  else:out += '/>\n'

  out += '</div>\n'
  
  # end option section

  print out+'<br />'

  
########################################################################
def AddReportButton(defects,run_spec,info,beambkg):
  dbdefects="'"+'<br>'.join(defects)+"'"
  if not dbdefects:
    dbdefects="''"
  #if beambkg: dbdefects +="B"  
  out = '<hr><input type="button" value="Get Report" onclick="Report(%s,%s,%s,%s,%i)" />\n'%(info,run_spec['low_run'],run_spec['proc_ver'],dbdefects,beambkg)
  out += '<input type="button" value="Defect DB" onclick="DefectDB(%s)" />\n'%(run_spec['low_run'])
  out += '<input type="button" value="LAr DB log" onclick="LArDBlog()" />'
  print out+'<br />'
