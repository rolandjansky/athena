# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# Miscellaneous utilites wrapping the (to me) unhandy matplotlib
# interface. Among other thing it allows plots via an object instance
# rather than global variables and figure numbers.
#
# Author: Thomas Kittelmann
# First version: April 2011

import PmbUtils.Misc as misc
import os

def _init_plot():
    #work around annoying matplotlib "feature":
    mpldir=os.environ.get('MPLCONFIGDIR','')
    if not mpldir or not os.path.isdir(mpldir) or not os.access(mpldir, os.W_OK):
        import tempfile,shutil,atexit
        mpldir=tempfile.mkdtemp(prefix='Atlas_PmbUtils_MPLCONFIGDIR_')
        os.environ['MPLCONFIGDIR']=mpldir
        def cleanup_mpldir(): shutil.rmtree(mpldir)
        atexit.register(cleanup_mpldir)
    import matplotlib,sys
    if not 'matplotlib.backends' in sys.modules:
        matplotlib.use( 'Agg' )

__mpl=None
def get_matplotlib():
    global __mpl
    if __mpl==None:
        _init_plot()
        import matplotlib
        __mpl=matplotlib
    return __mpl

#some global variables to toggle:
__plotdir=None
_default_bkgd_transp_orig=False
_default_bkgd_transp=_default_bkgd_transp_orig
_default_extension_orig='png'
_default_extension=_default_extension_orig

def set_plot_dir(d=None,create=False):
    global __plotdir
    if d==None:
        __plotdir=None
        return
    if create and not os.path.isdir(d):
        os.makedirs(d)
    assert os.path.isdir(d),'unknown directory %s'%d
    __plotdir=d

def set_default_bkgd_transp(t=None):
    global _default_bkgd_transp
    if t==None:
        _default_bkgd_transp=False
        return
    assert t in [True,False]
    _default_bkgd_transp=t

def set_default_extension(e=None):
    global _default_extension
    if e==None:
        _default_extension='png'
        return
    assert type(e) in [str,list]
    if isinstance(e,list): _default_extension=[(ext[1:] if ext.startswith('.') else ext) for ext in e]
    else: _default_extension=e

def get_plot_dir(): return __plotdir
def get_default_bkgd_transp(): return _default_bkgd_transp
def get_default_extension(): return _default_extension

def colours_misc():
    col_black='k'
    col_red='#F62217'
    col_yellow='#FBB117'
    col_invalid='invalidcol'
    col_blue='#151B8D'
    col_green='#4AA02C'
    col_orange='#F87217'
    col_purple='#B93B8F'
    col_darkgrey=3*(0.4,)
    return [col_yellow,col_blue,col_orange,col_green,col_purple,col_red]
def colours_gray():
    return map(lambda x:((x*0.1,)*3),range(2,10))
def markers_misc():
    #TODO: if using .scatter instead of .plot: return ['o','<','x','>','^','v','+','d','s','h','p','8']
    return ['o','<','x','>','^','v','d','D','*','h']#no '+'
def markers_all():
    return ['+','*',',','.','1','2','3','4','<','>','D','H','^','_','d','h','o','p','s','v','x','|','']
def linestyles_all():
    return ['-','--','-.',':','']
def colours_base():
    return ['b','g','r','c','m','y','k']
def generate_colour_marker_pairs(cols,markers):
    cols=map(lambda x:normcolor(x),cols)
    assert len(cols)!=len(markers)#important...
    ic,im,all,retries=0,0,[],0
    while True:
        if retries==10: break
        combi=(cols[ic],markers[im])
        if not combi in all:
            retries=0
            all+=[combi]
        else:
            ic+=1
            retries+=1
        ic,im=(ic+1)%len(cols),(im+1)%len(markers)
    return all
__colmark_std=None
def colmark_std():
    global  __colmark_std
    if __colmark_std==None:
        __colmark_std=generate_colour_marker_pairs(colours_misc(),markers_misc())
    return __colmark_std[:]

__colconv=None
def _colconv():
    global __colconv
    if __colconv==None:
        __colconv=get_matplotlib().colors.ColorConverter()
    return __colconv
def normcolor(c):
    return _colconv().to_rgba(c)
def set_alpha(c,alpha):
    return _colconv().to_rgba(c,alpha=alpha)

def darken_colour(c,r=0.5):
    #todo: map r->1-r?. Make brighten_colour as well.
    c=normcolor(c)
    return (c[0]*r,c[1]*r,c[2]*r,c[3])

class Plot(object):
    __figcount=0
    def __init__(self,width=1600,height=1200,autox=False,autoy=False,legendpos=None):
        self.__rec=misc.Recorder(self)
        self.__dpi=150
        self.__fallback_colmarks=colmark_std()
        Plot.__figcount+=1
        _init_plot()
        self.__fignum=Plot.__figcount
        import pylab
        self.__p=pylab
        self.__fig=pylab.figure(num=self.__fignum,
                                figsize=(width/self.__dpi,height/self.__dpi),dpi=self.__dpi,
                                facecolor='white',edgecolor='white')
        self.__p.clf()#clear figure
        self.__p.cla()#clear axes
        self.__p.axis()#'equal'=>circle is circular, 'scaled'=>
        self.__p.gca().set_autoscale_on(True)#keep autoscale on until we actually write the figure
        self.__defaults={'linewidth':1.5,'linestyle':'-','marker':None,'color':None,'extra_transp_fact':1.0}
        #self.__usedcolors=[]
        self.__xmin=None
        self.__xmax=None
        self.__ymin=None
        self.__ymax=None
        self.__legendpos=legendpos
        self.__legendobs=[]
        self.__thelegobj=None
        self.__functions=[]
        self.__grid=True
        #self.set_margins()#default margins
        self.__allow_autocolor=True
        self.__xlabels=None
        self.__ylabels=None
        self.__xlabels_rot=None
        self.__ylabels_rot=None
        self.__auto_colours=colours_misc()
        self.__auto_col_idx=-1
        self.__auto_markers=markers_misc()
        self.__auto_mrk_idx=-1
        self.__auto_colmrk=None
        self.__auto_colmrk_idx=-1
        self.__textlabels=[]
        self.__fig.canvas.mpl_connect('draw_event', self.__on_draw)
        self.__objs_rightside=[]
        self.__sidebox_text=None
        self.__textobj_xaxis_label=None
        self.__textobj_yaxis_label=None
        self.__textobj_title=None
        self.__no_on_draw_action=False
        self.__margin=0.02
        self.__legend_dist=0.01
        self.__legend_fontsize=None
        self.__vert_lines=[]
        self.__horiz_lines=[]
        self.__xlabels_fontsize=None

    def set_margin(self,m=0.02):
        self.__rec()
        self.__margin=m

    def set_legend_dist(self,ld=0.02):
        self.__rec()
        self.__legend_dist=ld

    def set_legend_fontsize(self,fs=None):
        self.__rec()
        self.__legend_fontsize=fs

    def set_xlabels_fontsize(self,fs=None):
        self.__rec()
        self.__xlabels_fontsize=fs

    def __on_draw(self,event):
        if self.__no_on_draw_action: return

        def getbb(o):
            bbox = o.get_window_extent()
            return bbox.inverse_transformed(self.__fig.transFigure)
        fig=self.__fig
        fa=fig.axes[0]

        margin_objs=[]
        margin_objs+=[(fa.get_xticklabels()+[self.__textobj_xaxis_label],'bottom','vert',False)]
        margin_objs+=[(fa.get_yticklabels()+[self.__textobj_yaxis_label],'left','horiz',False)]
        margin_objs+=[([self.__textobj_title],'top','vert',True)]
        #objright_nosidebox=[o for o in self.__objs_rightside if o!=self.__sidebox_text]
        #margin_objs+=[(objright_nosidebox,'right','horiz',True)]
        margin_objs+=[(self.__objs_rightside,'right','horiz',True)]

        changed=[]
        for objs,sizename,orientation,invert in margin_objs:
            bboxes=[getbb(o) for o in objs if o]
            if bboxes:
                b=get_matplotlib().transforms.Bbox.union(bboxes)
                target=min(0.45,b.height if orientation=='vert' else b.width)
            else: target=0.0
            target+=self.__margin
            if invert: target=1.0-target
            if target!=eval('fig.subplotpars.%s'%sizename):
                eval('fig.subplots_adjust(%s=target)'%sizename)
                changed+=[sizename]

        #special handling of sidebox text and legend:
        if self.__thelegobj:
            legpos=self.__legend_dist/(fig.subplotpars.right-fig.subplotpars.left)
            self.__thelegobj.set_bbox_to_anchor((1.0+legpos, 1))
        if self.__sidebox_text:
            h_sidebox=getbb(self.__sidebox_text).height
            h_legend=getbb(self.__thelegobj).height
            if h_legend: h_legend+=self.__legend_dist#padding
            h_axes=fig.subplotpars.top-fig.subplotpars.bottom
            avail=h_axes-h_legend
            if avail<h_sidebox:
                #limited space, sidebox text will flow under x-axis position (align to bottom of legend).
                self.__sidebox_text.set_verticalalignment('top')
                self.__sidebox_text.set_position((fig.subplotpars.right+self.__legend_dist,
                                                  fig.subplotpars.top-h_legend))
            else:
                #align to x-axis position.
                self.__sidebox_text.set_position((fig.subplotpars.right+self.__legend_dist,fig.subplotpars.bottom))

        #set title position (only now since we need the margin)
        if self.__textobj_title:
            self.__textobj_title.set_position((self.__margin,1.0))

        #one update (without triggering infinite loop)
        self.__no_on_draw_action=True
        fig.canvas.draw()
        self.__no_on_draw_action=False

    def add_text_labels(self,tl):
        self.__rec()
        self.__textlabels+=tl

    def __get_auto_colmrk(self):
        if not self.__auto_colmrk:
            self.__auto_colmrk=generate_colour_marker_pairs(colours_misc(),markers_misc())
        return self.__auto_colmrk

    def set_defaults(self,**kw):
        self.__rec()
        for k,v in kw.items():
            assert k in self.__defaults.keys()
            self.__defaults[k]=v

    def set_disallow_auto_color(self,t=True):
        self.__rec()
        #todo/fixme: also for markers!!!
        self.__allow_autocolor=not t
        return self

    def __styles(self,**kw):
        import copy
        styles=copy.deepcopy(self.__defaults)
        for k,v in kw.items():
            #assert k in styles.keys()
            styles[k]=v
        #lots of stuff for colours here:
        auto_col = styles['color'] in [None,'auto']
        auto_mrk = styles['marker'] in [None,'auto']

        if auto_col or auto_mrk:
            if auto_col and auto_mrk:
                colmrks=self.__get_auto_colmrk()
                self.__auto_colmrk_idx+=1
                if self.__auto_colmrk_idx>=len(colmrks):
                    print "WARNING: Ran out of auto colour/marker pairs! Restarting."
                    self.__auto_colmrk_idx=0
                styles['color'],styles['marker']=colmrks[self.__auto_colmrk_idx]
            elif auto_col:
                self.__auto_col_idx+=1
                if self.__auto_col_idx>=len(self.__auto_colours):
                    print "WARNING: Ran out of auto colours! Restarting."
                    self.__auto_col_idx=0
                styles['color']=self.__auto_colours[self.__auto_col_idx]
            else:
                assert auto_mrk
                self.__auto_mrk_idx+=1
                if self.__auto_mrk_idx>=len(self.__auto_markers):
                    print "WARNING: Ran out of auto markers! Restarting."
                    self.__auto_mrk_idx=0
                styles['marker']=self.__auto_markers[self.__auto_mrk_idx]
        if styles['extra_transp_fact']!=1.0:
            r,g,b,a=normcolor(styles['color'])
            styles['color']=set_alpha((r,g,b),alpha=a*styles['extra_transp_fact'])
        del styles['extra_transp_fact']
        return styles

    def set_xlim(self,xmin=None,xmax=None):
        self.__rec()
        self.__xmin,self.__xmax=xmin,xmax

    def set_ylim(self,ymin=None,ymax=None):
        self.__rec()
        self.__ymin,self.__ymax=ymin,ymax

    def add_function(self,func,legend=None,**kw):
        self.__rec()
        #NB: at points where the func returns None, nothing will be added.
        styles=self.__styles(**kw)
        styles['marker']=''
        if styles['linestyle'] in ['',' ','none']:
            styles['linestyle']='-'
        self.__functions+=[(func,legend,styles)]
        #delay actually adding anything

    def flush_function_drawing(self):
        self.__rec()
        self.__delayed_add_function()

    def __delayed_add_function(self):
        if not self.__functions: return
        self.__makecurrent()
        x0,x1,_,_=self.__p.axis()
        xvals=misc.range_float(1000,x0,x1)
        #for i in range(n): xvals+=[x0+i*(x1-x0)/float(n)]
        for func,legend,styles in self.__functions:
            x=[]
            y=[]
            for xi in xvals:
                yi=func(xi)
                if yi!=None:
                    x+=[xi]
                    y+=[yi]
            plotob=self.__p.plot(x,y,**styles)
            if legend: self.__legendobs+=[(legend,plotob)]
        self.__functions=[]

    def add_points(self,points,legend=None,addpolfit=None,polfitlinestyle='-',polfit_extrapolate=False,**kw):
        self.__rec()
        if not 'linestyle' in kw.keys(): kw['linestyle']=''
        styles=self.__styles(**kw)
        self.__makecurrent()
        x,y=misc.points2xyarrs(points)
        plotob = self.__p.plot(x,y,**styles)
        if legend: self.__legendobs+=[(legend,plotob)]
        if addpolfit!=None:
            f=misc.FuncPoly(order=addpolfit,fitpoints=points,noextrapolation=not polfit_extrapolate)
            styles['linestyle']=polfitlinestyle
            self.add_function(f,**styles)
        return styles#styles in case user needs to know which auto styles were chosen, and plotob for making a legend

    def set_xaxis_label(self,label):
        self.__rec()
        self.__makecurrent()
        self.__textobj_xaxis_label=self.__p.xlabel(label)
        return self

    def set_yaxis_label(self,label):
        self.__rec()
        self.__makecurrent()
        self.__textobj_yaxis_label=self.__p.ylabel(label)
        return self

    def set_axis_labels(self,x,y):
        self.__rec()
        self.__makecurrent()
        self.__p.xlabel(x)
        self.__p.ylabel(y)
        return self

#obsolete    def set_margins(self,left=None,right=None,top=None,bottom=None):
#obsolete        self.__rec()
#obsolete        #These are our defaults:
#obsolete        if left==None: left=0.1
#obsolete        if right==None: right=0.1
#obsolete        if top==None: top=0.1
#obsolete        if bottom==None: bottom=0.1
#obsolete        self.__makecurrent()
#obsolete        self.__p.subplots_adjust(left=left,right=1.0-right,top=1.0-top,bottom=bottom)
#obsolete        return self

    def __set_labels(self,axisobj,labels):
        self.__rec()
        axisobj.set_major_formatter(get_matplotlib().ticker.FuncFormatter(lambda x,n:labels[n][1]))
        axisobj.set_major_locator(get_matplotlib().ticker.FixedLocator(map(lambda e:e[0],labels)))

    def __adjust_lims_based_on_label(self,limfct,labels,rel_margin):
        if labels and rel_margin:
            firstval=labels[0][0]
            if len(labels)==1:
                limfct(firstval-0.1,firstval+0.1)
            else:
                lastval=labels[-1][0]
                dx=rel_margin*abs(lastval-firstval)
                limfct(firstval-dx,lastval+dx)

    def set_xlabels(self,xlabels=None,rotation=None,rel_margin=0.05,fontsize=(None,)):
        self.__rec()
        self.__xlabels_rot=rotation
        self.__xlabels=xlabels[:] if xlabels!=None else None
        self.__adjust_lims_based_on_label(self.set_xlim,xlabels,rel_margin)
        if fontsize!=(None,):
            self.__xlabels_fontsize=fontsize

    def set_ylabels(self,ylabels=None,rotation=None,rel_margin=0.05):
        self.__rec()
        self.__ylabels_rot=rotation
        self.__ylabels=ylabels[:] if ylabels!=None else None
        self.__adjust_lims_based_on_label(self.set_ylim,ylabels,rel_margin)

    def add_horiz_line(self,y,**opts):
        import copy
        self.__horiz_lines+=[(y,copy.deepcopy(opts))]

    def add_vert_line(self,x,**opts):
        import copy
        self.__vert_lines+=[(x,copy.deepcopy(opts))]

    def write(self,filename,transparent=None,quiet=False):
        #after first call to write, the legend won't be updated again (fixme: keep leg. object and update on the fly)
        self.__rec()
        self.__makecurrent()

        if self.__xlabels!=None: self.__set_labels(self.__fig.axes[0].xaxis,self.__xlabels)
        if self.__ylabels!=None: self.__set_labels(self.__fig.axes[0].yaxis,self.__ylabels)

        if self.__xlabels_rot:
            self.__p.xticks(rotation=self.__xlabels_rot)
            if self.__xlabels_rot>0 and self.__xlabels_rot<90: self.__p.xticks(ha='right',va='top')
            if self.__xlabels_rot<0 and self.__xlabels_rot>-90: self.__p.xticks(ha='left',va='top')

        if self.__xlabels_fontsize:
            self.__p.xticks(size=self.__xlabels_fontsize)
            
        #desired functionality: if rotation != 0 or 90, align at right top (if positive).
        #however horizontal alignment doesn't seem to change after call above with ha and va args.
        if self.__ylabels_rot: self.__p.yticks(rotation=self.__ylabels_rot)#todo: think about alignment of y-labels

        for x,lineopts in self.__vert_lines:
            if not 'zorder' in lineopts: lineopts['zorder']=-99999
            self.__p.axvline(x=x,**lineopts)
        for y,lineopts in self.__horiz_lines:
            if not 'zorder' in lineopts: lineopts['zorder']=-99999
            self.__p.axhline(y=y,**lineopts)

        if (self.__xmin,self.__xmax,self.__ymin,self.__ymax)!=(None,None,None,None):
            self.__p.gca().set_autoscale_on(False)
            if (self.__xmin,self.__xmax)!=(None,None)!=2:
                self.__p.xlim(xmin=self.__xmin,xmax=self.__xmax)
            if (self.__ymin,self.__ymax)!=(None,None):
                self.__p.ylim(ymin=self.__ymin,ymax=self.__ymax)

        if self.__textlabels:
            lims={}
            lims['xmin'],lims['xmax']=self.__p.xlim()
            lims['ymin'],lims['ymax']=self.__p.ylim()
            epsx=0.005*(lims['xmax']-lims['xmin'])
            epsy=0.005*(lims['ymax']-lims['ymin'])
            lims['ymin+'],lims['ymin-']=lims['ymin']+epsy,lims['ymin']-epsy
            lims['ymax+'],lims['ymax-']=lims['ymax']+epsy,lims['ymax']-epsy
            lims['xmin+'],lims['xmin-']=lims['xmin']+epsx,lims['xmin']-epsx
            lims['xmax+'],lims['xmax-']=lims['xmax']+epsx,lims['xmax']-epsx
            sideboxtext=[]
            sideboxtext_opts={}

            for tl in self.__textlabels:
                has_opts = 1 if isinstance(tl[-1],dict) else 0
                if len(tl)==2+has_opts:
                    if has_opts: pos,txt,opts=tl
                    else: (pos,txt),opts=tl,{}
                    assert pos in ['lowerright']
                    if sideboxtext_opts:
                        assert opts==sideboxtext_opts,'must have same options'
                    else: sideboxtext_opts=opts
                    sideboxtext+=[txt]
                else:
                    assert len(tl)==3+has_opts
                    if has_opts: xpos,ypos,txt,opts=tl
                    else: (xpos,ypos,txt),opts=tl,{}
                    if xpos in lims.keys(): xpos=lims[xpos]
                    if ypos in lims.keys(): ypos=lims[ypos]
                    self.__p.text(xpos,ypos,txt,**opts)
            if sideboxtext:
                sideboxtext_opts['ha']='left'
                sideboxtext_opts['va']='bottom'
                #if not 'ha' in sideboxtext_opts.keys(): sideboxtext_opts['ha']='left'
                #if not 'va' in sideboxtext_opts.keys(): sideboxtext_opts['va']='bottom'
                tobj=self.__p.figtext(0.9,0.1,'\n'.join(sideboxtext),**sideboxtext_opts)
                self.__sidebox_text=tobj
                self.__objs_rightside+=[tobj]

        if transparent==None:
            transparent=get_default_bkgd_transp()

        self.__delayed_add_function()

        if self.__grid!='<done>' and self.__grid:
            #kind of redundant way to make if statement. Better to
            #store the grid object and then delete it later if
            #disabled
            #NB: matplotlib bug means the zorder is not actually respected for the grid (currently).
            #One could do: self.__p.gca().set_axisbelow() but then the ticks would get below as well.
            self.__p.grid(color='gray',zorder=-999999)
            self.__grid='<done>'

        if self.__legendpos!='<done>':
            if not self.__legendpos:
                self.__legendpos='upper right'
            if self.__legendobs:
                titles,plotobs=misc.points2xyarrs(self.__legendobs)
                if self.__legend_fontsize!=None: leg_prop=get_matplotlib().font_manager.FontProperties(size=self.__legend_fontsize)
                else: leg_prop=None
                if self.__legendpos=='outside right':
                    self.__thelegobj=self.__p.legend(plotobs,titles,numpoints=1,loc=2,bbox_to_anchor=(1.05, 1),borderaxespad=0.,prop=leg_prop)
                    self.__objs_rightside+=[self.__thelegobj]
                else:
                    self.__thelegobj=self.__p.legend(plotobs,titles,numpoints=1,loc=self.__legendpos,prop=leg_prop)
            self.__legendpos='<done>'


        #Writing
        pd=get_plot_dir()
        if pd and not filename.startswith(pd):
            filename=os.path.join(pd,filename)
        fn,ext=os.path.splitext(filename)
        if len(ext)>5:#must be a spurious period somewhere
            fn,ext=filename,None
        transp_orig=transparent
        if not ext:
            de=get_default_extension()
            if type(de)==str:
                if de.startswith('transp:'):
                    de,transparent=de[7:],True
                    filename+='_transp.'+de
                else:
                    filename+='.'+de
            else:
                first=None
                for e in de:
                    if e.startswith('transp:'):
                        e,transparent=e[7:],True
                        fn=filename+'_transp.'+e
                    else:
                        transparent=transp_orig
                        fn=filename+'.'+e
                    if not first: first=fn
                    if not quiet: print "Writing",fn
                    if os.path.exists(fn): print "WARNING: Overwriting plot %s"%fn
                    if os.path.exists(fn): os.remove(fn)
                    self.__p.savefig(fn,transparent=transparent,antialiased=True)
                return first
        if not quiet: print "Writing",filename
        if os.path.exists(filename): print "WARNING: Overwriting plot %s"%fn
        if os.path.exists(filename): os.remove(filename)
        self.__p.savefig(filename,transparent=transparent,antialiased=True)
        return filename

    def set_title(self,t):
        self.__rec()
        self.__makecurrent()
        self.__textobj_title=self.__p.title(t,ha='left')
        return self

    def disable_grid(self):
        self.__rec()
        assert self.__grid!='<done>'
        self.__grid=False

    def enable_grid(self):
        self.__rec()
        assert self.__grid!='<done>'
        self.__grid=True

    def remove_legend(self):
        self.__rec()
        self.__makecurrent()
        self.__p.gca().legend_=None

    def __makecurrent(self):
        self.__p.figure(num=self.__fignum)

    def __del__(self):
        self.__p.close(self.__fig)

    def recorded_calls(self,objname='plot'):
        for c in self.__rec.to_code(objname=objname): yield c

    def write_script(self,output,initial_comments=None):
        global _default_bkgd_transp_orig
        global _default_bkgd_transp
        global _default_extension_orig
        global _default_extension
        #output can be either string or any object with a write method accepting strings.
        pd=get_plot_dir()
        if isinstance(output,str):
            if pd and not output.startswith(pd): output=os.path.join(pd,output)
            if not output.endswith('.py'): output+='.py'
            if os.path.exists(output): print "WARNING: Overwriting plot script %s"%output
            output=open(output,'w')
        output.write('#!/usr/bin/env python\n\n')
        if initial_comments:
            if '<std>' in initial_comments:
                lstd=['','This script should normally run out of the box in a recent ATLAS release.',
                      'If it fails, you might need to checkout and compile Tools/PmbUtils from trunk.',
                      '','Alternatively, to run locally outside an atlas release, you simply need to make',
                      'sure you have access to the python modules from PmbUtils. Easiest is to simply',
                      'get them from the repository by running the following command somewhere in your',
                      'python path (like in your working directory):','',
                      '  svn co svn+ssh://svn.cern.ch/reps/atlasoff/Tools/PmbUtils/trunk/python PmbUtils','',
                      'You also need to make sure you have the relevant python system packages installed',
                      'on your machine (including matplotlib and possibly the matplotlib svg backend).']
                initial_comments=reduce(lambda x,y:x+y,[(lstd if e=='<std>' else [e]) for e in initial_comments])
            for c in initial_comments: output.write('# %s\n'%c)
            output.write('\n')
        output.write('import PmbUtils.Plot\n')
        if _default_bkgd_transp!=_default_bkgd_transp_orig:
            output.write('\nPmbUtils.Plot.set_default_bkgd_transp(%s)\n'%repr(_default_bkgd_transp))
        if _default_extension!=_default_extension_orig:
            output.write('\nPmbUtils.Plot.set_default_extension(%s)\n'%repr(_default_extension))
        for r in self.recorded_calls():
            if not r.startswith('import PmbUtils.Plot'):
                output.write(r+'\n')


def create_plot_from_label2compandvals(data,name2plotopts=None,dates_as_ndays_from=None,
                                       never_string_labels=False,xlabel_rotation=None,
                                       comments=None,extra_text=None,
                                       label2comment=None,comment_opts=None,
                                       mark_transitions=[],transition_opts=None,):
    import datetime
    show_date_years=False
    #Data must be of format [(label,[(name1,val1),...]),...
    #label can be either strings, dates or floats.
    #Orders names by last y-value and assigns plot options (possibly overwritten)

    data,xlabels=misc.databyx_to_databyxval_plus_xlabels(data,
                                                         dates_as_ndays_from=dates_as_ndays_from,
                                                         show_date_years=show_date_years)

    marked_xvals=[0.5*(xlabels[t1][0]+xlabels[t2][0]) for t1,t2 in mark_transitions]

    if comment_opts==None:
        comment_opts={'ha':'left','size':'small','va':'top','color':(0.6,0.0,0.0)}

    textlabels=[]

    if label2comment and xlabels:
        deltax=max([x[0] for x in xlabels])-min([x[0] for x in xlabels])
        #turn labels into strings so they compare with values in xlabels (fixme, a bit messy):
        label2str=lambda lbl: misc.date_format(lbl,show_year=show_date_years) if isinstance(lbl,datetime.date) else str(lbl)
        label2comment=dict((label2str(k),v) for k,v in label2comment.items())

        xlabels.sort()
        comment_nfo={}
        prev_idx=None
        for xval,label in xlabels:
            if not label in label2comment.keys(): continue
            comment=label2comment[label]
            if not comment in comment_nfo.keys():
                idx=len(comment_nfo)
                comment_nfo[comment]={'xvals':[xval],'idxstr':'[%i]'%(idx+1),'idx':idx}
            else:
                idx=comment_nfo[comment]['idx']
                if prev_idx!=idx:
                    comment_nfo[comment]['xvals']+=[xval]
            prev_idx=idx

        l=[(v['idx'],k,v['xvals'],v['idxstr']) for k,v in comment_nfo.items()]
        l.sort()

        textlabels+=[('lowerright','Setup%s:'%('' if len(l)==1 else 's'),comment_opts)]
        for _,label,xvals,idxstr in l:
            if len(l)!=1:
                for xval in xvals:
                    textlabels+=[(xval,'ymax-',idxstr,comment_opts)]
            textlabels+=[('lowerright',('%s: %s'%(idxstr,label)) if len(l)!=1 else label,comment_opts)]

    if comments:
        for c in ['']+comments:
            textlabels+=[('lowerright',c,comment_opts)]

    if never_string_labels: xlabels=None
    data=misc.databyxval_to_databyname(data)
    data.sort(lambda e1,e2:cmp(e2[1][-1][1],e1[1][-1][1]))#sort by last y value
    data=map(lambda x:(x[0],x[1]),data)

    p=Plot(legendpos='outside right')
    max_name=0
    for name,pts in data:
        max_name=max(max_name,len(name))
        p.add_points(legend=name,linestyle='-',points=pts)

    if len(data)>35 or max_name>67: p.set_legend_fontsize('xx-small')
    elif len(data)>28 or max_name>52: p.set_legend_fontsize('x-small')
    elif len(data)>19 or max_name>33: p.set_legend_fontsize('small')

    if len(xlabels)>28: p.set_xlabels_fontsize('xx-small')
    elif len(xlabels)>35: p.set_xlabels_fontsize('xxx-small')
    elif len(xlabels)>45: p.set_xlabels_fontsize('xxxx-small')

    p.set_xlabels(xlabels,rotation=xlabel_rotation)
    p.add_text_labels(textlabels)
    if not transition_opts: transition_opts={}
    for x in marked_xvals:
        p.add_vert_line(x,**transition_opts)
    return p

if __name__=='__main__':
    set_plot_dir('testout',create=True)

    plot_markers=Plot(height=1400,width=600)
    offset=0.0
    for m in markers_all():
        offset+=0.5
        l=[(-1,1+offset),(0.1,3+offset),(0.9,2+offset),(1.9,1.8+offset)]
        styles=plot_markers.add_points(l,marker=m,color='auto')
        styles['linestyle']=''
        plot_markers.add_points([(2.5,1.8+offset),(2.3,1.8+offset)],**styles)
    for ls in linestyles_all():
        offset+=0.5
        l=[(-1,1+offset),(0.1,3+offset),(0.9,2+offset),(1.9,1.8+offset)]
        styles=plot_markers.add_points(l,marker='',linestyle=ls,color='auto')

    plot_markers.write('markers.png')
    plot_markers.write('markers.eps')

    myplot=Plot(autox=True)
    myplot2=Plot()
    myplot.add_points(l,color='red')
    def myfunc(x): return 1+x
    myplot.add_function(myfunc,'1+x')
    f=misc.FuncPoly([2.4,3.12])
    myplot.add_function(f,f.name())

    myplot.write('test1.png')
    myplot2.write('test2.png')
    set_default_extension('.svg')
    myplot2.write('test_default_extension')
    set_default_bkgd_transp(True)
    myplot2.write('test_default_transparent.png')
    #########################################
    #  slice and dice:


    #Bogus points:
    point1=[('Alg1',10),('Alg2',20),('Alg3',30)]
    point2=[('Alg1',11),('Alg2',21),('Alg3',31)]
    point3=[]
    point4=[('Alg1',12),('Alg2',22),('Alg4',42)]
    point5=[('Alg1',9),('Alg2',19),('Alg3',29),('Alg4',-5)]

    #Various types of labels:
    data_vs_float=[(10.0,point1),(20.0,point2),(25.0,point3),(28.0,point4),(30.0,point5)]
    data_vs_string=[('Point1',point1),('Point2',point2),('Point3',point3),('Point4',point4),('Point5',point5)]
    import datetime
    def date(n): return datetime.date(2011, 4, n)
    data_vs_dates=[(date(1),point1),(date(2),point2),(date(5),point3),(date(9),point4),(date(13),point5)]

    create_plot_from_label2compandvals(data_vs_float).set_margins(right=0.2,bottom=0.2).write('data_vs_float.png')
    create_plot_from_label2compandvals(data_vs_string).set_margins(right=0.2,bottom=0.2).write('data_vs_string.png')
    create_plot_from_label2compandvals(data_vs_dates,dates_as_ndays_from=None,xlabel_rotation='vertical').set_margins(right=0.2,bottom=0.2).write('data_vs_dates_1.png')
    create_plot_from_label2compandvals(data_vs_dates,dates_as_ndays_from='last',xlabel_rotation='vertical').set_margins(right=0.2,bottom=0.2).write('data_vs_dates_2.png')
    create_plot_from_label2compandvals(data_vs_dates,dates_as_ndays_from='last',never_string_labels=True,xlabel_rotation='vertical').set_margins(right=0.2,bottom=0.2).write('data_vs_dates_3.png')
    create_plot_from_label2compandvals(data_vs_dates,dates_as_ndays_from=date(5),never_string_labels=True,xlabel_rotation='vertical').set_margins(right=0.2,bottom=0.2).write('data_vs_dates_4.png')

    misc.add_html_index(get_plot_dir())

    #quick refs:
    #markers: ['+','*',',','.','1','2','3','4','<','>','D','H','^','_','d','h','o','p','s','v','x','|','']
        #character 	description
        #'.' 	point marker
        #',' 	pixel marker
        #'o' 	circle marker
        #'v' 	triangle_down marker
        #'^' 	triangle_up marker
        #'<' 	triangle_left marker
        #'>' 	triangle_right marker
        #'1' 	tri_down marker
        #'2' 	tri_up marker
        #'3' 	tri_left marker
        #'4' 	tri_right marker
        #'s' 	square marker
        #'p' 	pentagon marker
        #'*' 	star marker
        #'h' 	hexagon1 marker
        #'H' 	hexagon2 marker
        #'+' 	plus marker
        #'x' 	x marker
        #'D' 	diamond marker
        #'d' 	thin_diamond marker
        #'|' 	vline marker
        #'_' 	hline marker
    #linestyles: ['-','--','-.',':','']
        #character 	description
        #'-' 	solid line style
        #'--' 	dashed line style
        #'-.' 	dash-dot line style
        #':' 	dotted line style
    #colours:
    #       * b : blue
    #       * g : green
    #       * r : red
    #       * c : cyan
    #       * m : magenta
    #       * y : yellow
    #       * k : black
    #       * w : white
    #Gray shades can be given as a string encoding a float in the 0-1 range, e.g.:
    #color = '0.75'
    #'#eeefff'
    #R , G , B tuple, where each of R , G , B are in the range [0,1] (also RGBA tuples)
    #Finally, legal html names for colors, like 'red', 'burlywood' and 'chartreuse' are supported.
    #
    #todo: make gallery
