# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Miscellaneous hopefully handy utilities.
#
# Author: Thomas Kittelmann
# First version: April 2011

import os
def dictiter_bykeys(d,reverse=False,nmax=None):
    """Iterates in sorted order of keys"""
    keys=d.keys()[:]
    keys.sort()
    i=0
    if reverse: keys.reverse()
    for k in keys:
        i+=1
        if nmax and i>nmax: break
        yield k,d[k]

def dictiter_byvalues(d,reverse=False,nmax=None):
    """Iterates in sorted order of keys"""
    l=[]
    for k,v in d.items(): l+=[(v,k)]
    l.sort()
    if reverse: l.reverse()
    i=0
    for v,k in l:
        i+=1
        if nmax and i>nmax: break
        yield k,v

def locate(pattern, root=None):
    if root==None: root=os.curdir
    import fnmatch
    for path, dirs, files in os.walk(os.path.abspath(root)):
        for filename in fnmatch.filter(files, pattern):
            yield os.path.join(path, filename)


def gzip_fastopen(fn):
    """Alternative to gzip.open which is extremely slow in python 2.6"""
    #os.popen is deprecated after python 2.6, but if we move to 2.7 we
    #can hopefully use gzip.open again
    return os.popen("gzip -d -c %s"%fn)

def smart_open(f):
    if type(f)==str:
        if f.endswith('.gz'): return gzip_fastopen(f)
        else: return open(f)
    else: return f#might already be filehandle or string list.

def unpickle(filename):
    import cPickle
    return cPickle.load(smart_open(filename))

__unpickle_cache={}
def unpickle_with_cache(filename):
    global __unpickle_cache
    if not filename in __unpickle_cache.keys():
        __unpickle_cache[filename]=unpickle(filename)
        print "loaded",filename
    return __unpickle_cache[filename]

def mean(l):
    if not l: return None
    return sum(l)/float(len(l))

def dict_add(d,val,key1=None,key2=None,key3=None,key4=None,key5=None):
    if key2==None:
        if not key1 in d.keys(): d[key1]=val
        else: d[key1]+=val
    else:
        if not key1 in d.keys(): d[key1]={}
        dict_add(d[key1],val,key2,key3,key4,key5,None)

def lookup(container,keys,ignore_missing=False):
    for k in keys:
        if ignore_missing:
            if type(container)==dict and not k in container.keys(): return None
            elif type(container)==list and not k<len(container): return None
        container=container[k]
    return container

def print_dict(thedict,indent='|',shorten=True):
    if thedict=={}:
        print '(empty dict)'
        return
    keys=thedict.keys()[:]
    keys.sort()
    lastkey=keys[-1]
    for k in keys:
        if k==lastkey and '|' in indent:
            i=indent.rindex('|')
            indent=indent[:i]+'\\'+indent[i+1:]
        v=thedict[k]
        if type(v)==dict:
            if not v:
                print '%s-%s ==> dict with 0 keys'%(indent,k)
            else:
                print '%s-%s ==> dict with %i keys:'%(indent,k,len(v.keys()))
                if '\\' in indent:
                    indent=indent[:i]+' '+indent[i+1:]
                print_dict(v,indent+'     |')
        else:
            if shorten and (isinstance(v,str) or isinstance(v,unicode)) and len(v)>55:
                v=v[:25]+'<...>'+v[-25:]
            print '%s-%s ==>'%(indent,k),v

def mean_of_lookups(thelist,keys,ignore_missing=False):
    if len(thelist)==0: return None
    sum,n=0.0,0
    for e in thelist:
        v=lookup(e,keys,ignore_missing=ignore_missing)
        if ignore_missing and v==None: continue
        sum+=v
        n+=1
    if n==0: return None
    return sum/n

def fit_1d(points):
    if len(points)<2:
        return (None,None)
    sumx=0.
    sumy=0.
    sumxy=0.
    sumxx=0.
    for x,y in points:
        sumx+=x
        sumy+=y
        sumxy+=x*y
        sumxx+=x*x
    n=float(len(points))
    slope  = (sumxy*n-sumx*sumy)/(sumxx*n-sumx*sumx)
    offset = (sumy-slope*sumx)/n
    return (offset,slope)

def average_points(points):
    #third argument => weight for averaging
    if not points: return None
    if len(points)==1: return (points[0][0],points[0][1])
    has_average=len(points[0])==3
    x,y,f,fsum=0.,0.,1.0,0.
    for p in points:
        if has_average: f=p[2]
        x+=f*p[0]
        y+=f*p[1]
        fsum+=f
    return (x/fsum,y/fsum)

def average_yvals(points):
    #requires x-values to be similar
    xvals_seen=set()
    xvals=[]
    x2ys={}
    for p in points:
        w = 1 if len(p)==2 else p[2]
        if not p[0] in xvals_seen:
            x2ys[p[0]]=[(p[1],w)]
            xvals_seen.add(p[0])
            xvals+=[p[0]]
        else:
            x2ys[p[0]]+=[(p[1],w)]
    out=[]
    for x in xvals:
        out+=[(x,sum(y*w for y,w in x2ys[x])/sum(w for y,w in x2ys[x]))]
    return out

def bin_points(points,nbins):
    if  nbins==None:
        #special: don't bin. Just return the points (without weights)
        return map(lambda p: (p[0],p[1]),points)
    #make nbins in x-range, and average both x and y values within
    #each. If points have a third entry, it will be used as weight
    #in averaging.
    if not points: return []
    if len(points)==1: return [(points[0][0],points[0][1])]
    x0,_,x1,_=points_range(points)
    #todo: if x0==x1: return single point
    dx=float(x1-x0)/nbins
    points_per_bin=[]
    for i in range(nbins):points_per_bin+=[[]]#can't just do =[[]]*nbins due to sharing
    for p in points:
        if dx==0.0: ibin=0
        else: ibin=min(nbins-1,max(0,int((p[0]-x0)/dx)))
        points_per_bin[ibin]+=[p]
    out=[]
    for pbin in points_per_bin:
        if pbin: out+=[average_points(pbin)]
    return out

def points2xyarrs(points):
    x,y=[],[]
    for xi,yi in points:
        x+=[xi]
        y+=[yi]
    return x,y

def points_range(points):
    x0,x1,y0,y1=None,None,None,None
    for p in points:
        x,y=p[0],p[1]
        if x0==None or x<x0: x0=x
        if x1==None or x>x1: x1=x
        if y0==None or y<y0: y0=y
        if y1==None or y>y1: y1=y
    return [x0,y0,x1,y1]

def range_float(n,xmin,xmax):
    xvals=[]
    fact=(xmax-xmin)/float(n-1)
    for i in range(n): xvals+=[xmin+i*fact]
    return xvals

def polyfit(points=None,x=None,y=None,order=2):
    import numpy
    if not x:
        x,y=points2xyarrs(points)
    res=numpy.polyfit(numpy.array(x),numpy.array(y),order)
    l=list(res)
    l.reverse()
    return l

class FuncPoly(object):
    def __init__(self,pars=[],order=None,fitpoints=[],noextrapolation=False):
        if order==None:
            order=len(pars)-1
        if order and not pars:
            pars=[0]*(order+1)
        assert order>=0
        assert len(pars)==order+1
        self.__order=order
        self.__pars=pars
        import math
        self.__pow=math.pow
        self.__xmin=None
        self.__xmax=None
        if fitpoints:
            self.fit(fitpoints,noextrapolation)
    def order(self): return self.__order
    def name(self):
        def p(a):
            if a<0: return ''
            return '+'
        for i in range(self.__order+1):
            if i==0: s='%.4f'%self.__pars[i]
            elif i==1: s+='%s%.4f*x'%(p(self.__pars[i]),self.__pars[i])
            else: s+='%s%.4f*x^%i'%(p(self.__pars[i]),self.__pars[i],i)
        return s.replace('+',' + ').replace('-',' - ')
    def pars(self): return self.__pars
    def fit(self,points,noextrapolation=False):
        self.__pars=polyfit(points,order=self.__order)
        if noextrapolation:
            x0,y0,x1,y1=points_range(points)
            eps=(x1-x0)*0.05
            self.set_interval(x0-eps,x1+eps)
        return
    def xmin(self): return self.__xmin
    def xmax(self): return self.__xmax
    def set_interval(self,xmin,xmax):
        self.__xmin=xmin
        self.__xmax=xmax
    def __call__(self,x):
        if self.__xmin!=None and x<self.__xmin: return None
        if self.__xmax!=None and x>self.__xmax: return None
        y=0.0
        for i in range(self.__order+1):
            y+=self.__pow(x,i)*self.__pars[i]
        return y

def image_dimensions(filename):
    assert os.path.exists(filename)
    try:
        import Image
        i=Image.open(filename)
        w,h=i.size
        #i.close()#NB: Do not uncomment this line unless you like to debug mysterious crashes.
    except ImportError,err:
        #No Image module...
        import subprocess
        i = subprocess.Popen(['identify','-format','%w %h',filename], stdout=subprocess.PIPE).communicate()[0]
        w,h=map(lambda x:int(x),i.split())
        pass
    return w,h

def html_link(content,target,style=None):
    if style: style='style="%s" '%style
    else: style=''
    return '<a %shref="%s">%s</a>'%(style,target,content)

def html_img_tag(filename,th_rel=None,th_width=None,th_height=None,thumbnail_format='jpg'):
    #Set one of th_xxx to get a thumbnail. Either specify the relative
    #size as a number between 0 and 1 or as a fixed height or width in
    #units of pixels.
    assert len(filter(lambda x:x,[th_rel,th_width,th_height]))<=1
    w,h=image_dimensions(filename)
    if th_width: th_rel=float(th_width)/w
    if th_height: th_rel=float(th_height)/h
    if th_rel:
        bn,en=os.path.splitext(filename)
        w_th,h_th=int(th_rel*w+0.5),int(th_rel*h+0.5)
        thfile=bn+'_tn%ix%i'%(w_th,h_th)+'.'+thumbnail_format
        if not os.path.exists(thfile) or os.path.getmtime(thfile)<os.path.getmtime(filename):
            th_cmd='convert -geometry %ix%i'%(w_th,h_th)
            if thumbnail_format.lower() in ['jpeg','jpg']: th_cmd+=' -quality 95'
            ec=os.system('%s %s %s'%(th_cmd,filename,thfile))
            #ec=os.system('cp -f %s %s && %s %s'%(filename,thfile,th_cmd,thfile))
            assert ec==0 and os.path.exists(thfile)
        assert w_th,h_th==image_dimensions(thfile)
    else:
        thfile=None
    filename=os.path.basename(filename)
    imghtml='<img alt="%s" width="%s" height="%s" src="%s"/>'
    if thfile:
        thfile=os.path.basename(thfile)
        return html_link(imghtml%(filename,w_th,h_th,thfile),filename)
    return imghtml%(filename,w,h,filename)

def add_html_index(dir,extra_lines_top=None,extra_lines_bottom=None):
    import glob
    assert os.path.exists(dir)
    l=[]
    l+=['<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">']
    l+=['<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">']
    l+=['  <head>']
    l+=['    <title>Plots</title>']
    l+=['    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-15" />']
    l+=['    <meta name="expires" content="0" />']
    l+=['  </head>']
    l+=['  <body>']

    if extra_lines_top: l+=extra_lines_top
    fs=glob.glob(os.path.join(dir,'*.png'))
    fs.sort()
    i=0
    l+=['  <ul>']
    def plotname(iplot,filename):
        pn=os.path.splitext(os.path.basename(f))[0]
        if pn.lower().startswith('plot%i_'%iplot): pn=pn[len('plot%i_'%iplot):]
        if pn.lower().startswith('plot%02i_'%iplot): pn=pn[len('plot%02i_'%iplot):]
        return pn.replace('_',' ')
    for f in fs:
        i+=1
        l+=['  <li><a href="#plot%i">plot %i: %s</a></li>'%(i,i,plotname(i,f))]
    l+=['  </ul>']
    i=0
    for f in fs:
        i+=1
        w,h=image_dimensions(f)
        f=os.path.basename(f)
        fdescr=os.path.join(dir,os.path.splitext(f)[0]+'.txt')
        l+=['    <h1><a name="plot%i">Plot %i</a>: %s</h1>'%(i,i,plotname(i,f))]
        feps=os.path.join(dir,os.path.splitext(f)[0]+'.eps')
        fsvg=os.path.join(dir,os.path.splitext(f)[0]+'.svg')
        fpdf=os.path.join(dir,os.path.splitext(f)[0]+'.pdf')
        ftable=os.path.join(dir,os.path.splitext(f)[0]+'_table.txt')
        if os.path.exists(fdescr): l+=['    <p>%s</p>'%(open(fdescr).read().strip())]
        l+=['    <img width="%i" height="%i" alt="The image %s" src="%s"/><br/>'%(w,h,f,f)]
        if os.path.exists(feps): l+=['    <a href="%s">(get as eps)</a>'%(os.path.basename(feps))]
        if os.path.exists(fsvg): l+=['    <a href="%s">(get as svg)</a>'%(os.path.basename(fsvg))]
        if os.path.exists(fpdf): l+=['    <a href="%s">(get as pdf)</a>'%(os.path.basename(fpdf))]
        if os.path.exists(ftable): l+=['    <a href="%s">(get info as table)</a>'%(os.path.basename(ftable))]

    if extra_lines_bottom: l+=extra_lines_bottom

    l+=['  </body>']
    l+=['</html>']
    open(os.path.join(dir,'index.html'),'w').write('\n'.join(l))

###################################################################################################
### Various methods for slicing and dicing data such as alg cpu numbers as a function of labels ###

def date_format(dt,show_year=True):
    if show_year: return dt.strftime('%Y-%m-%d')
    return dt.strftime('%m-%d')

def date_decode(s):
    #We only accept formats: 'yyyy-mm-dd'
    if len(s)!=10 or s[4]!='-' or s[7]!='-':
        return None
    import datetime
    res=None
    try:
        res=datetime.datetime.strptime(s,'%Y-%m-%d').date()
    except ValueError:
        res=None
        pass
    return res

def databyx_to_databyxval_plus_xlabels(data,dates_as_ndays_from=None,show_date_years=False):
    import datetime
    assert dates_as_ndays_from in [None,'last'] or isinstance(dates_as_ndays_from,datetime.date)
    #Data must be of format [(label,[(name1,val1),...]),...
    #label can be either strings, dates or floats.
    #If dates, one can set datas_abs_pos to get the distance between
    #points as actual distance between dates.
    #
    #Returns dataout,xstrings where xstrings is a list of (val,str)
    #with any string labels needed on the x-axis (can be none
    #indicating the need to just show normal values). dataout is a
    #list of: [(xval,[(name1,val1),...]),...
    if not data: return [],[]
    xstrings=[]
    if isinstance(data[0][0],str):
        newdata=[]
        for strlabel,pts in data:
            xval=len(xstrings)
            xstrings+=[(xval,strlabel)]
            newdata+=[(xval,pts)]
        data=newdata
    elif isinstance(data[0][0],datetime.date):
        newdata=[]
        dates_sorted=map(lambda x:x[0],data)
        dates_sorted.sort()
        if dates_as_ndays_from=='last':
           dates_as_ndays_from=dates_sorted[-1]
        assert len(dates_sorted)==len(set(dates_sorted))#Should be unique!
        for dt,pts in data:
            if dates_as_ndays_from: xval=(dt-dates_as_ndays_from).days
            else: xval=dates_sorted.index(dt)
            xstrings+=[(xval,date_format(dt,show_year=show_date_years))]
            newdata+=[(xval,pts)]
        data=newdata
    else:
        xstrings=None
    data.sort()
    if xstrings: xstrings.sort()
    return data,xstrings

def databyxval_to_databyname(data,discard_empty=True):
    #Takes data as value:
    #[xval,[(name1,val1),...]),...
    #And transforms it into:
    #[('name1',[(x1,y1),...,(xn,yn)]),...]
    name2pts={}
    for xval,name2yval in data:
        for name,yval in name2yval:
            if yval==None: continue#allow user to "disable" certain points by letting them by None
            if not name in name2pts.keys(): name2pts[name]=[(xval,yval)]
            else: name2pts[name]+=[(xval,yval)]
    l=[]
    for name,pts in name2pts.items():
        if pts or not discard_empty:
            l+=[(name,pts)]
    l.sort()
    return l

def find_comp2max(data):
    comp2max={}
    for label,comp2val in data:
        for comp,val in comp2val:
            comp2max[comp]=max(val,comp2max.setdefault(comp,val))
    return comp2max

def find_comp2last(data):
    comp2last={}
    for label,comp2val in data:
        for comp,val in comp2val:
            comp2last[comp]=val
    return comp2last

def remove_least_significant_comps(data,nshown,ycut=None,otherstr='Other %i'):
    #Data must be of format [(label,[(name1,val1),...]),...
    #Removes those nameXXX with least significant contributions based on nshown and ycut.
    #Find out which we should ignore:
    valcomp=[(val,comp) for comp,val in find_comp2max(data).items()]
    valcomp.sort(reverse=True)
    ignore=set(comp for val,comp in valcomp[nshown:])
    if ycut: ignore=ignore.union(comp for val,comp in valcomp if val<ycut)
    if len(ignore)==1: ignore=set()
    #Create output data:
    dataout=[]
    for label,comp2val in data:
        show,ignoredvals=[],[]
        for comp,val in comp2val:
            if comp in ignore: ignoredvals+=[val]
            else: show+=[(comp,val)]
        if otherstr and ignoredvals:
            other = otherstr%len(ignore) if '%i' in otherstr else otherstr
            show+=[(other,sum(ignoredvals))]
        dataout+=[(label,show)]
    return dataout

def max_length(it):
    m=0
    for e in it: m=max(m,len(e))
    return m

def format_table(rows,linechar='-',vertchar='|',crosschar='+',colsep=' ',format_fct='auto',align_fct=lambda x,i,ie,w: str(x).ljust(w)):
    #rows is a list. Each entry must either be a list (of ncolumns entries) or a
    #string (prefix with 'title:' to center a title, or equal to 'line' for a
    #horizontal line.
    if format_fct=='auto':
        import math
        maxval=1.0e-99
        for row in rows:
            for e in row:
                if isinstance(e,float): maxval=max(maxval,abs(e))
        format_str='%%.%if'%min(15,max(1,int(4-math.log10(abs(maxval))+0.5)))
        format_fct=lambda x:format_str%x if isinstance(x,float) else str(x)
    ncols=set(len(row) for row in rows if isinstance(row,list))
    colwidths=[0]*(ncols.pop() if len(ncols)==1 else None)
    for row in (row for row in rows if isinstance(row,list)):
        for i,e in enumerate(row): colwidths[i]=max(colwidths[i],len(format_fct(e)))
    tablewidth=(len(colwidths)-1)*len(colsep)+max(sum(colwidths),max([len(row) for row in rows if not isinstance(row,list)]+[0]))
    for row in rows:
        if isinstance(row,list):
            s=[]
            for i,e in enumerate(row): s+=[align_fct(format_fct(e),i,i-len(row),colwidths[i])]
            res=vertchar+colsep.join(s)
            if len(res)<tablewidth+1:
                res+=' '*(tablewidth-len(res)+1)
            yield res+vertchar
        elif row=='line':
            yield crosschar+tablewidth*linechar+crosschar
        else:
            assert row.startswith('title:')
            yield vertchar+(' '+row[6:]+' ').center(tablewidth)+vertchar

def data_table(data,label_order=None,title=None,**kw):
    assert not label_order,'fixme'
    valcomp=[(val,comp) for comp,val in find_comp2last(data).items()]
    valcomp.sort(reverse=True)
    comps2pts=dict((comp,[]) for val,comp in valcomp)
    for i,(label,n2v) in enumerate(data):
        for n,v in n2v: comps2pts[n]+=[v]
        #fill in blanks:
        for c,pts in comps2pts.items():
            if len(pts)<i+1: pts+=['-']
            assert len(comps2pts[c])==i+1
    rows=[[str(label) for label,_ in data]+['<comp>'],'line']
    for _,comp in valcomp:
        rows+=[comps2pts[comp]+[comp]]
    header=['line']
    if title: header+=['title:'+title,'line']
    return [line for line in format_table(header+rows+['line'],**kw)]


########################################################################################
#
# For recording all the calls to a given object (for later playback or code export).
#
# Example of how to instrument a class MyClass:
#
# class MyClass(object):
#     def __init__(self,<whatever vars>):
#         import recorder
#         self.__recorder=recorder.Recorder(self)
#         <do whatever>
#     def method_a(self,<whatever vars>):
#         self.__recorder.record()
#         <do whatever>
#     def method_b(self,<whatever vars>):
#         self.__recorder.record()
#         <do whatever>
#     def print_records(self,objname='mcobj'):
#         for l in self.__recorder.to_code(objname): print l
#
# NB: There is no need to monitor purely const 'getter' methods (or special methods like print_records above).
#
########################################################################################

class Recorder(object):

    def __init__(self,parent):
        import inspect,copy
        self.__inspect=inspect
        self.__deepcopy=copy.deepcopy
        self.__recorded_class=parent.__class__
        frame=inspect.currentframe().f_back
        self.__recs=[]
        self.__monitored_methods=set()
        self.record(frame)

    def __call__(self):
        self.record(self.__inspect.currentframe().f_back)

    def __arg_default_fits(self,func, arg_name, arg_value):
        args, varargs, varkwargs, defaults = self.__inspect.getargspec(func)
        have_defaults = args[-len(defaults):] if defaults else []
        assert arg_name in args
        return arg_name in have_defaults and defaults[list(have_defaults).index(arg_name)]==arg_value

    def record(self,frame=None):
        if not frame: frame=self.__inspect.currentframe().f_back
        def frame_id(aframe):return str(aframe.f_code)#bullet proof?
        id_this,id_caller=frame_id(frame),frame_id(frame.f_back)
        fi=self.__inspect.getframeinfo(frame)
        self.__monitored_methods.add(id_this)
        if id_caller in self.__monitored_methods: return#internal call
        vals=frame.f_locals
        arg_names,pos_name,kw_name,values=self.__inspect.getargvalues(frame)
        assert 'self' in arg_names
        normal_args=[(a,self.__deepcopy(values[a])) for a in arg_names if a!='self']
        positional_vals = self.__deepcopy(vals[pos_name]) if pos_name else None
        keyword_dict = self.__deepcopy(vals[kw_name]) if kw_name else None
        try:
            func=fi.function
        except AttributeError:
            #python 2.5 does not have named tuples
            func=fi[2]
        self.__recs+=[(func,normal_args,positional_vals,keyword_dict)]
    def to_code(self,objname='obj'):
        yield 'import %s'%self.__recorded_class.__module__
        for f,args,pos,kw in self.__recs:
            if pos:
                args=['%s'%repr(v) for k,v in args]
                for p in pos: args+=[repr(p)]
            else:
                args=['%s=%s'%(k,repr(v)) for k,v in args if not self.__arg_default_fits(self.__recorded_class.__dict__[f],k,v)]
            if kw:
                for k,v in kw.items():
                    args+=['%s=%s'%(k,repr(v))]
            args=','.join(args)
            if f=='__init__':
                yield '%s=%s.%s(%s)'%(objname,self.__recorded_class.__module__,self.__recorded_class.__name__,args)
            else:
                yield '%s.%s(%s)'%(objname,f,args)

def get_load():
    import subprocess
    p=subprocess.Popen('/bin/ps -eo pcpu --no-headers'.split(), stdout=subprocess.PIPE).communicate()[0]
    return 0.01*sum(map(lambda x:float(x),p.split()))

def get_n_cores():
    n=0
    for l in open('/proc/cpuinfo'):
        if l.startswith('processor') and l.split()[0:2]==['processor',':']:
            n+=1
    return n

if __name__=='__main__':
    d={}
    dict_add(d,[3],'some','keys')
    dict_add(d,[4],'some','keys')
    print d

    l=[(1,1*1.56+0.34),(2,2*1.56+0.34),(5,5*1.56+0.34),(8,8*1.56+0.34)]
    print fit_1d(l)


    l=[(-1,1),(0.1,3),(0.9,2),(1.9,1.8)]


    print range_float(5,1.234,10.234)

    x = [1, 2, 3, 4, 5]
    y = [6, 11, 18, 27, 38]
    print polyfit(x=x,y=y,order=2)

    points=[(1,2),(3,4),(5,6),(7,8),(67,3),(2,3)]
    f=FuncPoly(order=3,fitpoints=points)
    print f.name()
    print f(0)

    mydict={'hej':{'bla':[1,2,{3:'hurra'}]}}
    print lookup(mydict,['hej','bla',2,3])
    print lookup(mydict,['hej','bla',17,3],ignore_missing=True)
    print lookup(mydict,[0],ignore_missing=True)
    print mydict

    l=[(0,0),(1.5,2),(1.6,2.2),(7.5,3),(10,10)]
    print l
    print bin_points(l,10)
