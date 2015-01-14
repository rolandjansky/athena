#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

years = ('2011','2012')
allDbNames = {'2012': 'trips.db', '2011': 'trips2011.db'}
allDatNames = {'2012': 'trips.dat', '2011':'trips2011.dat'}
allPicsDirNames = {'2012': 'HVHistoryPic', '2011': 'HVHistoryPic2011'}


print 'Content-type: text/html;charset=utf-8\n'
import cgitb
cgitb.enable()

import cgi, sqlite3, operator, datetime
import os, stat

form = cgi.FieldStorage()

selected_year = form.getvalue('YEAR', years[-1])
selected_module = form.getvalue('MODULE', '')
selected_channel = form.getvalue('CHANNEL', '')
selected_detector = form.getvalue('DETECTOR', '')
selected_side = form.getvalue('SIDE', '')
selected_sb = form.getvalue('SB', '')
selected_from = form.getvalue('FROM','')
selected_to = form.getvalue('TO', '')
selected_limit = int(form.getvalue('LIMIT', '100'))
selected_run = form.getvalue('RUN', '')
selected_status = form.getvalue('STATUS', '')

dbName = allDbNames[selected_year]
datName = allDatNames[selected_year]
picsDirName = allPicsDirNames[selected_year]

dt1 = datetime.datetime.fromtimestamp(os.stat(dbName).st_mtime)
dt2 = datetime.datetime.fromtimestamp(os.stat(datName).st_mtime)

cmd = 'select  trips.rowid,*  from trips INNER JOIN details ON trips.HVLINE = details.HVLINE order by trips.TripTimeStamp desc limit %d;' % selected_limit
if form.has_key('query'): 
    cmd = form.getvalue('query')

print '''
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<title>Query the HVTrips database</title>
<meta name="author" content="Nikiforos Nikiforou <nikiforo@cern.ch>, Ruggero Turra <ruggero.turra@cern.ch>"/>
<link rel="shortcut icon" href="favicon.ico" />
<link rel="Stylesheet" href="styles.css" type="text/css" />
<script>

function get_query(detail, stable_beam, module, channel, detector, side, from, to, run, status, limit) {
  var query = "select  trips.rowid,*  from trips";
  if (detail) query += " INNER JOIN details ON trips.HVLINE = details.HVLINE";
  selections = [];
  if (module)           selections.push("trips.HVLINE>" + module*1000 + " and trips.HVLINE<" + (parseInt(module)+1)*1000);
  if (channel)          selections.push("trips.HVLINE%1000=" + channel);
  if (detector)         selections.push("Det='" + detector + "'");
  if (side)             selections.push("Side='" + side + "'");
  if (stable_beam=='Y') selections.push("StableBeams='Y'");
  else if (stable_beam=='N') selections.push("StableBeams='N'");
  if (from)             selections.push("strftime('%s', '" + from + "') < strftime('%s', TripTimeStamp)");
  if (to)               selections.push("strftime('%s', '" + to + "') > strftime('%s', TripTimeStamp) + 24*3600");
  if (run)              selections.push("TripRunNumber=" + run);
  if (status)           selections.push("status='" + status + "'");
//  console.log(selections)
  if (selections.length) query += " where " + selections.join(" and ");
  query += " order by trips.TripTimeStamp desc";  
  if (limit) query += " limit " + limit;
  query += ';';
  return query;
};

function change_query() {
  stable_beam = document.getElementById("SB").value;
  module = document.getElementById("MODULE").value;
  channel = document.getElementById("CHANNEL").value;
  detector = document.getElementById("DETECTOR").value;
  side = document.getElementById("SIDE").value;
  from = document.getElementById("FROM").value;
  to = document.getElementById("TO").value;
  limit = document.getElementById("LIMIT").value;
  status = document.getElementById("STATUS").value;
  run = document.getElementById("RUN").value;  

  query = get_query(true, stable_beam, module, channel, detector, side, from, to, run, status, limit);
  query_field = document.getElementById("qq");
  query_field.value = query;
};
</script>
<base target="_self">
</head>
<body>
'''
print '''
<div class="wrapper">
<font style="font-size:20pt">Query the HVTrips database</font>'''

print '<form class="form" method="post">'

print '''
<p>
  <label for="YEAR">Database:</label>
  <select name="YEAR" id ="YEAR">
'''
for year in years:
    print '   <option value="%s" %s>%s</option>' % (year, (year==selected_year and "selected") or "", year) # why python 2.5??
print '''
  </select>
</p>
'''

detectors = ("", "EMB", "EMEC", "HEC", "EMBPS", "EMECPS")
sides = ("", "A", "C")
hv_statuses = ("", "current", "new")

def print_options(options, default=None):
    result = ""
    for option in options:
        result += ' <option value="%s" %s>%s</option>' % (option, (option==default and "selected") or "", option) # why python 2.5??
    return result


print '''
<p>
<label for="MODULE">Module: </label>
<input oninput="javascript:change_query()" type="text" id="MODULE" name="MODULE" size="10" value="%(module)s" />
<label for="CHANNEL">Channel: </label>
<input oninput="javascript:change_query()" type="text" id="CHANNEL" name="CHANNEL" size="10" value="%(channel)s" />
</p>
<p>
<label for="DETECTOR">Detector: </label>
<select onchange="javascript:change_query()" id="DETECTOR" name="DETECTOR">
''' % {"module": selected_module, "channel": selected_channel}
print print_options(detectors, selected_detector)
print '''
</select>
<label for="SIDE">Side: </label>
<select onchange="javascript:change_query()" id="SIDE" name="SIDE">
'''
print print_options(sides, selected_side)
print '''
</select></p>
<p>
<label for="STATUS">Status</label>
<select onchange="javascript:change_query()" id="STATUS" name="STATUS">
'''
print print_options(hv_statuses, selected_status)
print '''
</select>
</p>
<p>
<label for="RUN">Trip RunNumber</label>
<input oninput="javascript:change_query()" type="text" id="RUN" name="RUN" size="10" value="%(run)s" />
''' % {"run": selected_run}
print '''
<label for="SB">Stable beam:</label>
<select onchange="javascript:change_query()" id="SB" name="SB">
'''
print print_options(("", "Y", "N"), selected_sb)
print '''
</select></p>
'''
print '''
<p>
<label for="qds">From (yyyy-mm-dd):</label>
<input oninput="javascript:change_query()" type="text" id="FROM" name="FROM" size="10" value="%(from)s" />
<label for="qdt">To (yyyy-mm-dd):</label>
<input oninput="javascript:change_query()" type="text" id="TO" name="TO" size="10" value="%(to)s" />
</p>
''' % { "from": selected_from, "to": selected_to}
print '''
<p>
<label for="LIMIT">Limit: </label>
<input oninput="javascript:change_query()" type="text" id="LIMIT" name="LIMIT" size="10" value="%d" />
</p>''' % int(selected_limit)
print '<p><textarea id="qq" name="query" rows="5" cols="60">%s</textarea></p>' % cmd
print '''
<p>
<input type="submit" value="Send">
</p>
</form>(Enter an SQL query and hit "Enter". The default query returns the 100 most recently recorded trips in descending TripTimeStamp order)
'''
#print '<font style="font-size:18pt" color="red">DEVELOPMENT IN PROGRESS! DONT USE!</font><br>'
print '<br> Db last updated:%s, dat file last updated: %s'%(dt1,dt2)

print '<br> <font style="font-size:12pt">Instructions:</font> <br> Use sqlite commands (Local copy of the <a href="./sqlite-doc/index.html" target="_blank"> sqlite reference </a> or visit the <a href="http://www.sqlite.org/lang.html" target="_blank"> sqlite website </a>). I suggest to use the <b>limit</b> keyword to limit the output and avoid slow rendering of the page.<br>'

print 'The trips themselves are stored in a table named "trips" and the static additional details are stored in a table named "details". Use the "INNER JOIN details ON trips.HVLINE = details.HVLINE" command as indicated to correlate the two tables using the common "HVLINE" entry.<br>'

print 'The column names are the ones shown below (no spaces between words).<br>'

print '<b> Example queries:</b><br>'
print 'A. <b> select  trips.rowid,*  from trips INNER JOIN details ON trips.HVLINE = details.HVLINE where Det=\'EMBPS\' order by trips.TripTimeStamp desc </b>. Select all EMBPS trips.<br> '
print 'B. <b> select  trips.rowid,*  from trips INNER JOIN details ON trips.HVLINE = details.HVLINE where (strftime(\'%M\',RecoveryTimeStamp) - strftime(\'%M\',TripTimeStamp)) > 10 order by trips.TripTimeStamp desc </b>. Select all trips that lasted more than 10  minutes.<br> '


#Also uncomment the print statements in the bottom to show os.system output
#log='Log: os.system("ls -l) result:'
#log+= str(os.system("ls -l"))

#log='Log: os.system("ln -s /afs/cern.ch/user/l/larmon/public/TripVisualizer/HVHistoryPic2011") result:'
#log+= str(os.system("ln -s /afs/cern.ch/user/l/larmon/public/TripVisualizer/HVHistoryPic2011"))
#Do not use!
#os.remove("trips.db")
#execfile("createDB.py")
#os.chmod("trips.db",stat.S_IRWXU|stat.S_IRWXG|stat.S_IRWXO)

conn = sqlite3.connect(dbName,detect_types=sqlite3.PARSE_DECLTYPES|sqlite3.PARSE_COLNAMES)
#conn = sqlite3.connect('trips.db')

c = conn.cursor()




detcols = ['HVLINE','Trip\nTimeStamp','Trip\nRunNumber','Trip\nLB','Stable\nZero','Ramp\nUp','Recovery\nTimeStamp','Recovery\nRunNumber','Recovery\nLB', 'Stable\nBeams','Fill\nNumber','Num\nCollBunches','Solenoid','Toroid','Module','Channel','Det','Side','Phi\nWedge','SubPhi\nWedge','Eta\nSector','Electrode\nSide','Map', 'Powered\nElectrodes','HVLINE\nStatus',]


print '<br> Executing query command <b>'+cmd+' </b><br>'

c.execute(cmd)
r = c.fetchall()

print '<br><font style="font-size:20pt">Query result:</font><p font style="font-size:10pt">%d results returned</font></p>' % (len(r))
print 'HINT1: Clicking on the HVLINE entry pops up the relevant LArIdtranslator query page showing the affected channels. The affected eta-phi range is visible at the very bottom of the page. <br>'
print 'HINT2: Clicking on the RunNumber entry pops up the relevant Atlas Query page. <br>'
print 'HINT3: Clicking on the TripId entry pops up a visualization of the trip. <br>'

print '<table class="list">\n<tr>\n'
print '</tr>\n<tr>\n'
print '<td>TripId</td>'
for i in detcols: 
    print '<td>%s</td>\n' % (i)

print '</tr>\n<tr>\n'

for i in range(len(r)):
  #incompleteRow = False
  #for k in r[i]:
    #if k == 'Not entered' or k =='9998.12.31 00:00:00': 
      #incompleteRow = True
      #break

  #if incompleteRow:   print '<tr class="g%d">\n' % (2)
  #else: 
    print '<tr class="g%d">\n' % (i%2)
    curColumn = 0
    for j in r[i]:
        if curColumn == 3 or curColumn == 8:
          print '<td><a href="http://atlas-runquery.cern.ch/query.py?q=find+run+%s+and+ready+/+show+all+and+dq+lar+DCSOFL+" target="_blank">%s</a></td>' % (j,j)
        elif curColumn == 1:
            print '<td><a href="https://atlas-larmon.cern.ch/LArIdtranslator/?HVLINES=%s&expert=1&run=999999" target="_blank">%s</a></td>' % (j,j)
        elif curColumn == 15:
            print '' #Do not display second instance of HVLINE
        elif curColumn == 18 or curColumn == 19 or curColumn == 10:
            print '<td class="%s">%s</td>' % (j,j)
        elif curColumn == 0 and len(r[i])>10:
            if r[i][10]=='Y':
                picPath = picsDirName+'/trip_%s.png' % (j)
                if os.path.isfile(picPath):
                    print '<td><a href="%s" target="_blank">%s</a></td>' % (picPath,j)
                else:
                    print'<td>%s</td>' % (j)
            else:
                print'<td>%s</td>' % (j)

        else: print '<td>%s</td>' % (j)
        curColumn = curColumn +1;

    print '</tr>\n'
print '</tr>\n</table>\n'

#for i in list_files: print '<a href="%s">%s</a><br>' % (i,i)
print '<div class="push"></div>\n</div>\n'
print '<div class="footer">Developed very badly by Nikiforos K. Nikiforou <IMG SRC="http://www.cern.ch/mmmservices/AntiSpam/nospam.aspx?email=6voXPH0hIQhzOwUVkFEqM4k4jmbV8zQiqLND%2bmWxRn%2bFh%2fqDibWQA6qs6WMxqC77">, updated by R. Turra <IMG SRC="http://www.cern.ch/mmmservices/AntiSpam/nospam.aspx?email=CBP83JQFQq6%2fTOtBtq8e2lwa0GC8GQlRI5%2fPVbpSWP2FRDSZUVaQw90xXvaZA0DH"></div>\n'

#Uncomment these two lines to print log for os.system operations 
#print '<br> <font style="font-size:8pt"> Debug:',log
#print '</font>'

print '</body>\n</html>'

c.close()
