# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
This module provides a simple framework for generating CSS-based dynamic web
pages with a typical layout consisting of a header, navigation bar, content area
and a footer. It is compatible with different web servers using e.g. CGI, mod_python,
or CherryPy.
"""
__author__  = 'Juerg Beringer'
__version__ = 'WebPage.py atlas/athena'


import time
from cgi import escape


#
# HTML templates
#
startPage = """\
%(contentType)s<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>%(pageTitle)s</title>
<link href="%(cssName)s" rel="stylesheet" type="text/css" />
%(pageHeaderSnippets)s\
</head>
<body>
"""

endPage = """\
</body>
</html>\
"""


#
# Utilities for generating HTML snippets
#
def sep(s):
    """Add separator to string s unless s is empty."""
    return ' '+s if s else s

def htmlDiv(id, contents='', attr='', keepEmptyDiv=True):
    """Make a named div element containing contents. If
       contents is empty, an empty string is returned,
       unless keepEmtpyDiv is set True."""
    if contents or keepEmptyDiv:
        if id:
            return '<div id="%s"%s>\n%s</div>\n' % (id,sep(attr),contents)
        else:
            return '<div%s>\n%s</div>\n' % (sep(attr),contents)
    else:
        return ''

def htmlText(text, attr='', escapeText=False):
    """Make a text consisting of an unnamed div. Special HTML characters
       in the text are properly replaced (using escape from cgi) if
       escapeText is set True."""
    return '<div%s>%s</div>\n' % (sep(attr),escape(text) if escapeText else text)

def htmlPre(text, attr='', escapeText=False):
    """Make a preformatted text section. Special HTML characters
       in the text are properly replaced (using escape from cgi) if
       escapeText is set True."""
    return '<pre%s>\n%s\n</pre>\n' % (sep(attr),escape(text) if escapeText else text)

def htmlPara(text='', attr='', escapeText=False):
    """Make a paragraph."""
    if text or attr:
        return '<p%s>%s</p>\n' % (sep(attr), escape(text) if escapeText else text)
    else:
        return '<p>\n'

def htmlLink(text, link, attr='', escapeText=False):
    return '<a href="%s"%s>%s</a>' % (link,sep(attr),escape(text) if escapeText else text)

def htmlList(contents, attr='', listType='ul'):
    """Enclose list contents (a string with one or more list items) with
       the proper list tag. The type of the list is given by listType."""
    return '<%s%s>\n%s</%s>\n' % (listType,sep(attr),contents,listType)

def htmlLI(text, attr='', escapeText=False):
    """Make a list item.  Special HTML characters
       in the text are properly replaced (using escape from cgi) if
       escapeText is set True."""
    return '<li%s>%s</li>\n' % (sep(attr), escape(text) if escapeText else text)

class htmlTable:
    """Make a table. Table row data is accumulated internally in a list.
       The table can be made sortable (using jQuery's plugin Tablesorter)
       by setting useTableSorter to True. Special HTML characters in cell
       data are properly replaced (using escape from cgi) if escapeText
       is set True.
       """
    def __init__(self, tableAttr='', defaultCellAttr = [], useTableSorter=False, escapeText=False):
        self.tableAttr = tableAttr
        self.defaultCellAttr = defaultCellAttr
        self.useTableSorter = useTableSorter
        self.escapeText = escapeText
        self.rows = []   # list of HTML strings with complete rows of the table

    def __str__(self):
        return self.getHtml()

    def appendRow(self, cellData, rowAttr=''):
        """Append a row to the table. cellData is a list of the contents of
           the cells in the row. The data for each cell is either a string
           with the contents of the cell, or a tuple where the first element
           is the contents of the cell and the second element contains any
           HTML tags. Special HTML characters are properly replaced (using
           escape from cgi) if escapeText was set True when creating the
           table. If the table uses Tablesorter, <th> is used instead of
           <td> in the first row."""
        r = '<tr%s>\n' % sep(rowAttr)
        if self.useTableSorter and len(self.rows)==0:
            # First row using Tablesorter - must use <th> instead of <td>
            cellFormat = '<th%s>%s</th>\n'
        else:
            cellFormat = '<td%s>%s</td>\n'
        iCell = 0
        for c in cellData:
            cellAttr = self.defaultCellAttr[iCell] if iCell<len(self.defaultCellAttr) else ''
            if isinstance(c,tuple):
                r += cellFormat % (sep(c[1]),escape(str(c[0])) if self.escapeText else str(c[0]))
            else:
                r += cellFormat % (sep(cellAttr),escape(str(c)) if self.escapeText else str(c))
            iCell += 1
        r += '</tr>'
        self.rows.append(r)

    def getHtml(self):
        """Return the HTML code for the table."""
        if len(self.rows)<1:
            return ''
        if self.useTableSorter:
            if self.tableAttr:
                h = '<table %s>\n' % self.tableAttr
            else:
                h = '<table class="tablesorter">\n'
            h += '<thead>\n'
            h += self.rows[0]
            h += '\n</thead><tbody>\n'
            h += '\n'.join(self.rows[1:])
            h += '\n</tbody></table>\n'
        else:
            h = '<table%s>\n' % sep(self.tableAttr)
            h += '\n'.join(self.rows)
            h += '\n</table>\n'
        return h

def htmlFoldingSection(header, content, isClosed=True,
                       headerClass='section-header',
                       contentClass='section-content'):
    """Generate the html for a folding section using the toggleSection JavaScript utility
       from WebPageUtils.js and CSS classes section-closed, section-open, and hidden."""
    if isClosed:
        s = '<div class="section-closed" onclick="toggleSection(this);">'
    else:
        s = '<div class="section-open" onclick="toggleSection(this);">'
    s += '<span class="%s">%s</span></div>\n' % (headerClass,header)
    h = ' hidden' if isClosed else ''
    s += '<div class="%s%s">\n%s</div>\n' % (contentClass,h,content)
    return s

def htmlForm(contents, action='', method='post', attr=''):
    snippet = '<form action="%s" method="%s"%s>\n' % (action,method,sep(attr))
    snippet += '<fieldset>\n'
    snippet += contents
    snippet += '</fieldset>\n</form>\n'
    return snippet

def htmlLabel(labelText, parName, attr=''):
    """Make a label for parName. If labelText is None,
       an empty string is returned."""
    if labelText!=None:
        return '<label for="%s"%s>%s</label>' % (parName,sep(attr),labelText)
    else:
        return ''

def htmlSelect(labelText, parName, args, choiceList, hint=None, descriptionSeparator='::',
               labelAttr='', attr=''):
    """Make a select statement (including label with text)."""
    snippet = htmlLabel(labelText,parName,labelAttr)
    default = args[parName] if parName in args else ''
    if not isinstance(default,list):
        default = [default]
    snippet += '<select name="%s"%s>\n' % (parName,sep(attr))
    if hint:
        snippet += '<option value="">%s</option>\n' % hint
    for c in choiceList:
        p = c.split(descriptionSeparator)
        if len(p)==2:
            (desc,val) = p
        else:
            (desc,val) = (c,c)
        if val in default:
            snippet += '<option selected="yes" value="%s">%s</option>\n' % (val,desc)
        else:
            snippet += '<option value="%s">%s</option>\n' % (val,desc)
    snippet += '</select>\n'
    return snippet

def htmlCheckbox(labelText, parName, args, labelAttr='', attr=''):
    """Make a checkbox (including label with text)."""
    snippet = htmlLabel(labelText,parName,labelAttr)
    checked = 'checked="checked"' if parName in args else ''
    snippet += '<input type="checkbox" name="%s"%s%s/>\n' % (parName,sep(checked),sep(attr))
    return snippet

def htmlTextInput(labelText, parName, args, size=None, maxLength = None, labelAttr='', attr=''):
    """Make a text input area (including label with text). Special HTML
       characters in any default text are properly replaced."""
    snippet = htmlLabel(labelText,parName,labelAttr)
    snippet += '<input type="text" name="%s"' % parName
    if parName in args:
        snippet += ' value="%s"' % escape(args[parName],True)
    if size!=None:
        snippet += ' size="%s"' % size
    if maxLength!=None:
        snippet += ' maxlength="%s"' % maxLength
    snippet += '%s/>\n' % sep(attr)
    return snippet

def htmlSubmit(text, parName, attr='', onlyOnce=False):
    """Make a submit button. If onlyOnce is true, the button can only
       be clicked once in order to prevent multiple clicking of the
       submit button."""
    if onlyOnce:
        # FIXME: this doesn't work yet - it disables submission
        s = '<input type="button" name="%s" value="%s"' % (parName,text)
        s += ' onclick="this.form.submit()"'
        s += '%s />\n' % sep(attr)
        return s
    else:
        return '<input type="submit" name="%s" value="%s"%s />\n' % (parName,text,sep(attr))


#
# Utility classes
#
class WebPageConfigurationError(Exception):
    def __init__(self,*args):
        Exception.__init__(self,args)


class GlobalConfiguration(dict):
    """Class to store global configuration data for a tree of web pages. Data members
       are used to store information used by the framework, while the dict is
       intended to be used for application-specific configuration data."""

    def __init__(self, **args):
        self.baseUrl = 'webapp'    # Common base path
        self.pageList = []         # List of all web pages
        self.pageAttributes = {}   # Optional attributes for web pages, such as specific link styles

        # Initialize application-specific configuration data, if any
        for k in args:
            self[k] = args[k]


#
# Base class for all web pages
#
class WebPage:
    """Base class for creating CSS-based dynamic web pages."""

    def __init__(self, **args):
        """Constructor. You may override any of the default values in self.pageConfig by
           passing the corresponding value in a named variable."""
        self.pageConfig = { 'contentType':         '',    # For plain CGI need:  'Content-type: text/html\n\n'
                            'pageName':            '',
                            'pageTitle':           '',
                            'pageHeaderSnippets':  '',
                            'cssName':             'default.css',
                            'css_currentLink':     'acurrentlink',
                            'header':              '',
                            'navigation':          '',
                            'content':             '',
                            'footer':              '',
                            'keepEmptyHeader':     False, # Set to true to anyway generate div if empty
                            'keepEmptyNavigation': False,
                            'keepEmptyContent':    False,
                            'keepEmptyFooter':     False
                            }
        for k in args:
            self.pageConfig[k] = args[k]
        self.globalConfig = GlobalConfiguration()
        self.isConfigured = False
        pass

    def pageName(self):
        """Short cut to retrieve the name (the last element in the URL)
           of the current page. This works only if the page has been
           added into the page tree using addPage."""
        return self.pageConfig['pageName']

    def addPage(self, name, page, **attrs):
        """Add a new web page to the page tree. All pages added via addPage
           share the same GlobalConfiguration object."""
        page.globalConfig = self.globalConfig
        page.pageConfig['pageName'] = name
        self.globalConfig.pageList.append(name)
        self.globalConfig.pageAttributes[name] = dict(attrs)
        setattr(self,name,page)   # Link page into page tree (for CherryPy)

    def addLink(self, name, alias, **attrs):
        """Add a new link to the page tree. Links share page objects with
           other pages through aliasing. This allows using different links
           with different queries to the same page as if they were separate
           pages. For links, highlighting of the current page is disabled
           (if enabled, all links leading to the same page would be
           highlighted)."""
        self.globalConfig.pageList.append(name)
        self.globalConfig.pageAttributes[name] = dict(attrs)
        self.globalConfig.pageAttributes[name]['alias'] = alias
       
    def addToPageHeader(self, snippet):
        """Add a snippet of code to the page header. Use this e.g. to include JavaScript libraries."""
        self.pageConfig['pageHeaderSnippets'] += snippet

    def index(self, **args):
        """Return the complete page."""
        if not self.isConfigured:
            self.configure()
        s = self.override()
        if not s:
            self.pageConfig['timeStamp'] = time.strftime('%a %b %d %X %Z %Y')
            contents = self.content(**args)  # Make sure contents is run first (so it
                                             # can change any pageConfig entries if desired
            s = startPage % self.pageConfig
            s = s + htmlDiv('header', self.header(**args),
                            keepEmptyDiv=self.pageConfig['keepEmptyHeader'])
            s = s + htmlDiv('navigation', self.navigation(**args),
                            keepEmptyDiv=self.pageConfig['keepEmptyNavigation'])
            s = s + htmlDiv('content', contents,
                            keepEmptyDiv=self.pageConfig['keepEmptyContent'])
            s = s + htmlDiv('footer', self.footer(**args),
                            keepEmptyDiv=self.pageConfig['keepEmptyFooter'])
            s = s + endPage
        return s
    # For CherryPy
    index.exposed = True

    def configure(self):
        """Final configuration of web application after all data is initialized."""
        self.configureLinks()
        self.isConfigured = True
        return

    def configureLinks(self):
        """Based on configuration data, for each web page create two link entries in pageConfig:
           The first entry has the form url_PAGENAME and contains the complete URL to link to
           a page. The second entry is named href_PAGENAME and contains style information and a
           href= prefix with proper quotation marks in addition to the URL. These link entries
           should be used to generate links in HTML pages with snippets like <a %(href_PAGENAME)s>
           or <a href="%(url_PAGENAME)>. The link entries can only be generated once all
           configuration data is available, ie configureLinks must be called from configure
           and not from __init__."""
        for p in self.globalConfig.pageList:
            page = self.globalConfig.pageAttributes[p].get('alias',p)
            query = self.globalConfig.pageAttributes[p].get('query','')
            style = self.globalConfig.pageAttributes[p].get('style',None)
            self.pageConfig['url_'+p] = '%s/%s/%s' % (self.globalConfig.baseUrl,page,query)
            if p==self.pageName():
                self.pageConfig['href_'+p] = 'href="%s/%s/%s" class="%s"' % (self.globalConfig.baseUrl,page,query,self.pageConfig['css_currentLink'])
            else:
                if style:
                    self.pageConfig['href_'+p] = 'href="%s/%s/%s" class="%s"' % (self.globalConfig.baseUrl,page,query,style)
                else:
                    self.pageConfig['href_'+p] = 'href="%s/%s/%s"' % (self.globalConfig.baseUrl,page,query)

    def override(self):
        """Override provides a hook where code to generate or redirect to an alternative
           page can be placed by derived classes. If not override of the normal page is
           desired, override should return None. Otherwise it should either raise an
           appropriate exception or return a string containing the complete alternate page
           to display."""
        return None

    def header(self, **args):
        """Generate the page header. Default value comes from self.pageConfig['header']."""
        return self.pageConfig['header'] % self.pageConfig

    def navigation(self, **args):
        """Generate the navigation bar. Default value comes from self.pageConfig['navigation']."""
        return self.pageConfig['navigation'] % self.pageConfig

    def content(self, **args):
        """Generate the page content. Default value comes from self.pageConfig['content']."""
        return self.pageConfig['content'] % self.pageConfig

    def footer(self, **args):
        """Generate the footer. Default value comes from self.pageConfig['footer']."""
        return self.pageConfig['footer'] % self.pageConfig


# Example configuration file on how to run CherryPy with Apache mod_python:
#
#  <Location "/">
#      PythonPath "sys.path+['/whatever']"
#      SetHandler python-program
#      PythonHandler cherrypy._cpmodpy::handler
#      PythonOption cherrypy.setup WebPage::setup_server
#      PythonDebug On
#  </Location>
#
#import cherrypy
#def setup_server():
#    cherrypy.config.update({'environment': 'production',
#                            'log.screen': False,
#                            'server.socket_host': '127.0.0.1',
#                            'log.error_file': '/tmp/site.log',
#                            'show_tracebacks': False})
#    cherrypy.tree.mount(WebPage())


# Test code
if __name__ == '__main__':

    class HelloWorld(WebPage):
        def content(self):
            return "Hello, world!"

    # For plain CGI
    #import cgi
    #import cgitb; cgitb.enable()
    #p = HelloWorld(pageTitle = 'HelloWorld Test', contentType='Content-type: text/html\n\n')
    #print p.index()

    # For local CherryPy (run as `python WebPage.py')
    # NOTE: see http://www.cherrypy.org/wiki/StaticContent on how to
    #       serve static content such as the css file
    import cherrypy
    cherrypy.quickstart(HelloWorld(pageTitle='HelloWorld Test',contentType=''))
