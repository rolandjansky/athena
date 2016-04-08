#!/usr/bin/env python
import re, httplib, mimetypes
import base64, socket
import os, sys, time, socket
import getopt
import commands
import socket
from xml.dom import minidom

try:
    socket.setdefaulttimeout(3)
except:
    pass

__version__ = "$Revision: 1.3 $"[11:-1]

HELP="""KV collector post interface %s.
Usage: kvpost.py --rel=<num> --test=<test name> --desc=<test desc> --res=<result> [OPTION]...

Options:
  --help                      display this help and exit.
  --host=<hostname>           hostname of the KV collector
                              (current: %s)
  --sel=<selection>           selection (page) of the KV collector
                              (current: %s)
  --sid=<session id>          session ID (if zero it will be assigned)
  --test=<test name>          KV test name
  --desc=<test description>   KV test description
  --res=<test result>         KV test result
  --info=<test info>          KV test info
  --log=<log filename>        KV log file name
  --tag=<tag name>            KV tag
  --rand=<random seeds>       Random seeds, implies the --info option
  --user=<username>           Username
                              (default: %s)
  --ip=<ip number>            IP number of this machine
                              (current: %s)
  --hname=<host name>         Host name of this machine
                              (current: %s)

Alessandro De Salvo <Alessandro.DeSalvo@roma1.infn.it>.
"""

__CURLPOST__   = 'curl -s -k --connect-timeout 10 https://%s%s'
__CURLCERT__   = ' --cert %s --key %s'
__CURLFIELD__  = ' -F "%s=%s"'
__CURLFILE__   = ' -F "%s=@%s;filename=%s"'

class Fields:
  _fields = []

  def add(self, field, value):
    if not self.index(field): self._fields.append(field)
    setattr(self, field, value)

  def remove(self, field):
    if self.index(field): self._fields.remove(field)

  def index(self, field):
    return field in self._fields

  def get(self, field):
    return getattr(self,field)

  def getall(self):
    list = []
    for field in self._fields:
       list.append((field, getattr(self,field)))
    return list


class KVpost:

    # Options
    short_options = ""
    long_options  = ["help", "host=", "sel=",  "sid=", "test=", "desc="
                    ,"res=", "info=", "user=", "ip=",  "hname=", "rel="
                    ,"log=", "rand=", "tag=",  "debug"]

    # Defaults
    debug    = False
    pars     = Fields()
    if (os.environ.has_key('KVHOST')):
        host = os.environ['KVHOST'];
    else:
        host = "kv.roma1.infn.it"
    sel      = "/KV/kvcollect.php"
    ip       = socket.getaddrinfo(socket.gethostname(), None)[0][4][0]
    #hname    = socket.gethostname()
    hname    = socket.getfqdn()
    date     = time.strftime('%Y-%m-%d %H:%M:%S')
    sid      = '0'
    if (os.environ.has_key('USER')):
        user = os.environ['USER']
    else:
        user = 'NONE'
    
    def main(self):
        try:
            opts, args = getopt.getopt(sys.argv[1:],
                         self.short_options,
                         self.long_options,
                       )
        except getopt.GetoptError:
            # Print help
            print "GetOpt Error"
            self.help()
            sys.exit(-1)

        for cmd, arg in opts:
            if cmd in ('--help',):
                self.help()
                sys.exit()
            elif cmd in ('--debug',):
                self.debug = True
            elif cmd in ('--host',):
                self.host = arg
            elif cmd in ('--sel',):
                self.sel = arg
            elif cmd in ('--user',):
                self.user = arg
            elif cmd in ('--ip',):
                self.ip = arg
            elif cmd in ('--hname',):
                self.hname = arg
            elif cmd in ('--sid',):
                self.sid = arg
            elif cmd in ('--tag',):
                self.pars.add('tag', ("%s" % arg))
            elif cmd in ('--test',):
                self.pars.add('testname', ("%s" % arg))
            elif cmd in ('--desc',):
                self.pars.add('testdesc', ("%s" % arg))
            elif cmd in ('--res',):
                self.pars.add('testres', ("%s" % arg))
            elif cmd in ('--rand',):
                self.pars.add('rand', ("%s" % arg))
            elif cmd in ('--info',):
                self.pars.add('testinfo', ("%s" % arg))
            elif cmd in ('--rel',):
                self.pars.add('rel', ("%s" % arg))
            elif cmd in ('--log',):
                self.pars.add('log', ("%s" % arg))
        if (not self.pars.index('testdata')):
            self.pars.add('testdata', '')

        if (    not self.pars.index('testname')
            or  not self.pars.index('testdesc')
            or  not self.pars.index('testres')
            or  not self.pars.index('rel')
           ):
            print "Missing arguments. Exiting..."
            self.help();
            sys.exit(-1);
        if (self.pars.index('rand') and not self.pars.index('testinfo')):
            print "No --info specified when using --rand"
            self.help();
            sys.exit(-1);

        try:    osdist   = commands.getoutput("lsb_release -d 2>/dev/null").split(":")[1][1:]
        except: osdist   = "none"
        try:    compver  = commands.getoutput("gcc -v 2>&1 | grep '^gcc'")
        except: compver  = "none"
        try:    pyver    = sys.version.split()[0]
        except: pyver    = "none"
        try:    cpuname  = commands.getoutput("cat /proc/cpuinfo | grep '^model name' | tail -n 1").split(':')[1].lstrip()
        except: cpuname  = "none"
        try:    cpunum   = commands.getoutput("cat /proc/cpuinfo | grep '^processor' | wc -l")
        except: cpunum   = 0
        try:    bogomips = commands.getoutput("cat /proc/cpuinfo | grep '^bogomips' | tail -n 1").split(':')[1].lstrip()
        except: bogomips = 0
        try:    meminfo  = commands.getoutput("cat /proc/meminfo | grep '^Mem:'").split()[1]
        except: meminfo  = 0
        # Add the random seeds to the info data
        if (self.pars.index('rand')):
            try:
                xmlfile=open(self.pars.get('testinfo'))
                xmldata=xmlfile.read()
                xmlfile.close()
                try:
                    randfile=open(self.pars.get('rand'))
                    randdata=randfile.read()
                    randfile.close()
                except:
                    randdata=self.pars.get('rand')
                xmlout=self.xml_add(xmldata, "rndmseed", randdata, "   ")
                xmlfile=open(self.pars.get('testinfo'),'w')
                xmlfile.write(xmlout)
                xmlfile.close()
            except:
                pass
        # Prepare the info data
        infodata = ''
        if (self.pars.index('testinfo')):
            try:
                infofile = open(self.pars.get('testinfo'))
                infodata = " "+infofile.read()
                infodata = infodata.replace('"','\\"')
                infofile.close()
            except:
                pass

        # Prepare the data to send
        data = (
                ('submit','yes')
               ,('ip',self.ip)
               ,('hn',self.hname)
               ,('user',self.user)
               ,('date',self.date)
               ,('testname',self.pars.get('testname'))
               ,('testdesc',self.pars.get('testdesc'))
               ,('testres',self.pars.get('testres'))
               ,('testinfo',infodata)
               ,('rel',self.pars.get('rel'))
               ,('osname',os.uname()[0])
               ,('osvers',os.uname()[2])
               ,('osarch',os.uname()[4])
               ,('osdist',osdist)
               ,('compver',compver)
               ,('pyver',pyver)
               ,('cpunum',cpunum)
               ,('cpuname',cpuname)
               ,('bogomips',bogomips)
               ,('meminfo',meminfo)
               ,('sid',self.sid)
               )
        # Tag
        if (self.pars.index('tag')):
            data += (('tag',self.pars.get('tag')),)
        # Files
        files  = ()
        fnames = ()
        # Logfile
        if (self.pars.index('log')):
            try:
                logfile = open(self.pars.get('log'))
                logdata = logfile.read()
                logfile.close()
                files = (('upfile',os.path.basename(self.pars.get('log')),logdata),)
                fnames = (('upfile',self.pars.get('log'),os.path.basename(self.pars.get('log'))),)
            except:
                pass
        if (self.debug):
            print "DEBUG> host: %s" % self.host
            print "DEBUG> sel: %s" % self.sel
            print "DEBUG> data:"
            print data
            print "DEBUG> files:"
            print files
        # Send the data
        curlcmd = __CURLPOST__ % (self.host, self.sel)
        if (os.environ.has_key('X509_USER_PROXY')):
            curlcmd += __CURLCERT__ % (os.environ['X509_USER_PROXY'],os.environ['X509_USER_PROXY'])
        for field in data:
            curlcmd += __CURLFIELD__ % (field[0],field[1])
        for fname in fnames:
            curlcmd += __CURLFILE__ % (fname[0],fname[1],fname[2])
        (s,reply) = commands.getstatusoutput(curlcmd)
        if (s == 0):
            if (len(reply) > 2): print reply
        else:
            # Curl failed. Try with the embedded engine
            print "[KVPOST] Last command failed: %s" % curlcmd
            print "[KVPOST] %s" % reply
            print "[KVPOST] Failed to send data with curl. Trying with the embedded engine";
            try:
                reply = self.post_multipart(self.host, self.sel, data, files)
                if (len(reply) > 2): print reply
            except:
                print "Cannot send KV data";
                raise
                sys.exit(-1)

    def xml_add(self, xmldata, tagname, value, sep):
        xmldoc = minidom.parseString(xmldata)
        maintag = xmldoc.firstChild
        tag = xmldoc.createElement(tagname)
        maintag.appendChild(tag)
        tagtxt = xmldoc.createTextNode(value)
        tag.appendChild(tagtxt)
        xmlout=xmldoc.toxml()
        xmlout=xmlout.replace(("<%s>" % tagname),("%s<%s>" % (sep,tagname)))
        xmlout=xmlout.replace(("</%s>" % tagname),("</%s>\n" % tagname))
        return xmlout

    def post_multipart(self, host, selector, fields, files):
        """
        Post fields to an https host as multipart/form-data.
        fields is a sequence of (name, value) elements for regular form fields.
        files is a sequence of (name, filename, value) elements for data to be uploaded as files.
        Return the server's response page.
        """

        # HTTPS standard port to connect to the server
        port = 443
        if (os.environ.has_key('https_proxy')):
            # Get the proxy details
            p = re.compile("(\S+)://(\S+):(\d+)")
            match = p.match(os.environ['https_proxy'])
            if (match):
                phost = match.group(2)
                pport = int(match.group(3))
            #setup basic authentication
            user=self.user
            passwd=""
            user_pass=base64.encodestring(user+':'+passwd)
            proxy_authorization='Proxy-authorization: Basic '+user_pass+'\r\n'
            proxy_connect='CONNECT %s:%s HTTP/1.0\r\n'%(host,port)
            user_agent='User-Agent: python\r\n'
            proxy_pieces=proxy_connect+proxy_authorization+user_agent+'\r\n'

            #now connect, very simple recv and error checking
            proxy=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
            proxy.connect((phost,pport))
            proxy.sendall(proxy_pieces)
            response=proxy.recv(8192)
            status=response.split()[1]
            if status!=str(200):  raise 'Error status=',str(status)

            try:
                # trivial setup for ssl socket
                ssl = socket.ssl(proxy, None, None)
                sock = httplib.FakeSocket(proxy, ssl)

                #initalize httplib and replace with your socket
                h=httplib.HTTPConnection('localhost')
                h.sock=sock
            except:
                # revert back to standard socket
                port = 80
                h=httplib.HTTPConnection(phost,pport)
                selector = "http://%s:%d%s" % (host,port,selector)
        else:
            if (hasattr(socket,"ssl")):
                h = httplib.HTTPSConnection(host=host, port=port)
            else:
                port = 80
                h = httplib.HTTPConnection(host=host, port=port)
        content_type, body = self.encode_multipart_formdata(fields, files)
        h.putrequest('POST', selector)
        h.putheader('content-type', content_type)
        h.putheader('content-length', str(len(body)))
        h.endheaders()
        h.send(body)
        response = h.getresponse()
        return response.read()

    def encode_multipart_formdata(self, fields, files):
        """
        fields is a sequence of (name, value) elements for regular form fields.
        files is a sequence of (name, filename, value) elements for data to be uploaded as files.
        Return (content_type, body) ready for httplib.HTTP instance
        """
        BOUNDARY = '----------ThIs_Is_tHe_bouNdaRY_$'
        CRLF = '\r\n'
        L = []
        for (key, value) in fields:
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"' % key)
            L.append('')
            L.append(("%s" % value))
        for (key, filename, value) in files:
            L.append('--' + BOUNDARY)
            L.append('Content-Disposition: form-data; name="%s"; filename="%s"' % (key, filename))
            L.append('Content-Type: %s' % self.get_content_type(filename))
            L.append('')
            L.append(("%s" % value))
        L.append('--' + BOUNDARY + '--')
        L.append('')
        body = CRLF.join(L)
        content_type = 'multipart/form-data; boundary=%s' % BOUNDARY
        return content_type, body

    def get_content_type(self, filename):
        return mimetypes.guess_type(filename)[0] or 'application/octet-stream'

    def help(self):
        print HELP % (__version__.strip(),self.host,self.sel,self.user,self.ip,self.hname)


# main class
if __name__ == '__main__':
    kvpost=KVpost()
    kvpost.main()
