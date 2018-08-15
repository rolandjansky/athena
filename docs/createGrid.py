import os
indexfile = "html/index.html"
docsfile  = "html/doxygen.css"

rootdir = os.path.abspath('./')
rootdir+='/'
print rootdir

path = rootdir + '/docs/'

def ReadFile(fileName, pathFlag = True):
#   """This method reads file directly or from path."""
   if pathFlag:
     print("Read:", path+fileName)
     f = open(path+fileName)
   else:
     f = open(path+fileName)
     print("Read:", path+fileName)
   data = f.read()
   f.close()
   return data


def HTMLTreeBegin( title, links = {}):
   html = '\n<li>\n<div class="hitarea expandable-hitarea"></div>\n'
   html = html + '<span class="folder">%s\n' % title
   for i in links.keys():
     html = html + '<a target="_blank" href="%s">[%s]</a> \n' % (links[i], i)
   html = html + '</span>\n'
   html = html + '<ul style="display: block;">\n'
   return html

def WriteFile( fileName, data):
#   This method writes data
   print("Write:", path + fileName)
   f = open(path + fileName, "w")
   f.write(data)
   f.close()

def HTMLTreeEnd():
   return '</li></ul>\n\n'

def Fixcssfile( filename ):
   addition = """/** atlas insert **/
.doxyTable{
    border-collapse:collapse;
    border:1px solid lightBlue;
    width: 70%;
    margin:0 auto;
}

.doxyTable tr:not(:first-child) {
    background: #eeeeff;
    border-spacing: 0px;
    padding: 0px;
    border-bottom: 1px solid #c1c1dc;
}

.doxyTable tr:hover {
    background: #cde4ec;
}

.doxyTable td {
    padding:8px;
    cursor:pointer;
}

.accordion {
    display: none;
    width: 100%;
}

div.content {}

div.content h1, h2 {
    text-align: center;
}
"""
   text = ReadFile(filename)
   if("/** atlas insert **/" in text):
      print "Skipping rewriting css"
      return
   text += addition
   WriteFile(filename, text)

def __NewTreePage( domain, package):
   content = ''
   content += HTMLTreeBegin(domain)
   for i in package[domain]:
     content += HTMLTreeAddItem(i, 'https://gitlab.cern.ch/atlas/athena/tree/master/' + domain + '/'+ i, folder = True)
   content += HTMLTreeEnd()

   temp = treePageTamplate % (content)
   WriteFile("html/iframes/%s.html" % domain.lower().replace(' ', '_'), temp)

def CreateNewMainPage(outputFileName, packagemap):
   contents = """
<script>function changeState(item) {
    item.slideToggle("slow");
}

$( document ).ready(function() {
    $(".doxyTable > tbody > tr").each(function(){
        var row = $(this);
        // skip first row (header), you dont need to assing it accordion
        // trigger function. it is also the same for rows which doesn't have
        // id attribute. if a row doesn't have id attr, that means it is
        // accordion row
        if(row.attr('id') == 'header' || !row.attr('id')) return;

        // if it has id attribute, assign trigger function
        row.click(function(){
            $('div#'+row.attr('id')).slideToggle(256);
            // we dont want to load whole iframes before they are showed
            var iframe = $('div#'+row.attr('id')).find('iframe');
            // no need to set the source again again if already set
            if(iframe.attr('src')) return;

            iframe.attr('src', iframe.attr('data-src'));
        });
    });
});</script>

   <table class="doxyTable" border="0" cellpadding="0" cellspacing="0">
   <tr class="top" valign="top">
   <th class="domain">Domain</th>
   </tr>
   """
   keysI = sorted(packagemap.keys())


   for i in keysI:
      if i=='': continue
#      contents = contents + '\n<tr class="DCRow">\n'    ######### TAG: TR1
#      contents = contents + """<td width="50%%" style="padding:8px;cursor:pointer" onclick="toggleHoba('hoba_%s', 'iframes/%s.html')" id="hoba_%s"><a>%s</a></td>\n""" % (i.replace(' ', '_'), i.lower().replace(' ', '_'), i.replace(' ', '_'), i)
#      contents = contents + '</td>\n'
#      contents = contents + '</tr>\n\n'               ######### TAG: TR1
#      contents = contents + """<tr><td colspan="2" style="background:#d7dbe3">
#        <div style="display:none;" id="hoba_%s_div"><iframe width="100%%" frameborder="0"></iframe></div>
#        </td></tr>""" % (i.replace(' ', '_'))
      tagsafe = i.replace(' ', '_')
      tagsafelc = i.lower().replace(' ', '_')

      basic = """<tr id="{0}"><td width="50%">{0}</td></tr><tr style="padding:0"><td colspan="2" style="padding:0">
<div class="accordion" id="{0}">
<iframe width="100%" height="250px" frameborder="0" data-src="iframes/{1}.html"> </iframe>
</div></td></tr>"""
      basic = basic.format(tagsafe, tagsafelc)
      contents += basic

   contents += "</table>"
   stamp = "</div></div><!-- contents -->"
   contents += stamp
   mainPageTemplate = ReadFile(indexfile)
#   print mainPageTemplate
   WriteFile(outputFileName, mainPageTemplate.replace(stamp, contents))

def HTMLTreeAddItem( title, links = None, endNode = False, folder = False):
    if endNode: html = '\t<li class="last">'
    else: html = '\t<li>'
    
    if isinstance(links, str) or isinstance(links, type(u'')):
        if folder:
            html = html + '\t<a href="%s" target="_blank" class=""><span class="emptyFolder">%s</span></a>\n' % (links, title)
        else:
            html = html + '\t<a href="%s" target="_blank" class=""><span class="file">%s</span></a>\n' % (links, title)
    elif isinstance(links, dict):
         if folder:
            html = html + '<span class="emptyFolder">%s ' % title
         else:
             html = html + '<span class="file">%s ' % title
         for i in links.keys():
             html = html + '<a target="_blank" href="%s">[%s]</a> \n' % (links[i], i)
         html = html + '</span>'
    else:
        html = html + '\t<span class="file">%s</span>\n' % title
    return html + '\t</li>\n'
        




#packagelist=[]
packagemap ={}

for subdir, dirs, files in os.walk(rootdir):
    if 'CMakeLists.txt' in files :
#       packagelist.append(subdir);
       package =  subdir[len(rootdir):]
       i = package.find('/')
       if i<0 : i=0
       category = package[0: i ]
       if category in packagemap:
          packagemap[category].append(package[i+1:])
       else:
          packagemap[category] = [package[i+1:]]
#       print package

#for st in packagelist:
#   print st;
#print packagemap
#print treePageTamplate
framesdir = path+ 'html/iframes'
if not os.path.exists(framesdir):
    os.makedirs(framesdir)
#category = 
treePageTamplate=ReadFile("TreeViewTemplate.html", pathFlag = True)
for k in packagemap.keys():
   __NewTreePage(str(k), packagemap);



CreateNewMainPage(indexfile, packagemap)
Fixcssfile(docsfile)

