#! /usr/bin/env python
from __future__ import division

from argparse import ArgumentParser
from os import listdir, makedirs
from os.path import dirname, exists, join as pjoin
from pkg_resources import resource_filename
from shutil import copy2
from textwrap import dedent

from genshi import HTML
from genshi.output import HTMLSerializer, encode
from genshi.template import MarkupTemplate

from DQUtils import fetch_iovs
from DQUtils.general import timer
from DQUtils.logger import init_logger; init_logger(verbose=2)
from DQUtils.lumi import fetch_lumi_inputs, compute_lumi_many_channels

from DQDefects import DefectsDB

from IPython.Shell import IPShellEmbed; ip = IPShellEmbed(["-pdb"])

GRAPH_URL = "http://atlasdqm.web.cern.ch/atlasdqm/defect_graph/HEAD"

## Headings control the content which is displayed.
# (Defect.name, "Heading description")
# Blank identifier means it is constructed entirely inside Defect.content
headings = [
    ("", "V"),
    ("", "T"),
    ("", "U"),
    ("", "B"),
    ("name_with_url", "Name"),
    ("description", "Description"),
    ("nruns", "# Runs"),
    ("latest_run", "Latest Run"),
    ("rec_lb_frac", "Potentially Recoverable LB %"),
    ("lumi", HTML("Luminosity (pb<sup>-1</sup>)")),
    #("nlumi", "nlumi"),
    #("nlumi_rec", "nlumi"),
]

heading_names, heading_titles = zip(*headings)

content_string = "".join("<td>{{d.{0}}}</td>".format(x)
                         for x in heading_names if x)

class Defect(object):
    """
    A defect instance. Forms a row in the table.
    """
    def __init__(self, name, description, iovs, virtual, intolerable, lumi):
        if description == name:
            # Don't bother pretending that there is a description if it is just
            # equal to the name
            description = ""
            
        (self.name, self.description, self.iovs, self.lumi, self.virtual, 
         self.intolerable) = (
         name, description, iovs, lumi / 1e6, virtual, intolerable)
        self.nlumi = sum(iov.length for iov in iovs 
                    if iov.since.run == iov.until.run)
        self.nlumi_rec = sum(iov.length for iov in iovs 
                             if iov.recoverable and iov.since.run == iov.until.run)
        if self.nlumi and self.nlumi_rec:
            self.rec_lb_frac = "{0:.0%}".format(self.nlumi_rec / self.nlumi)
        else:
            self.rec_lb_frac = "-"
                    
        self.users = set(iov.user for iov in iovs)
        self.users -= set(["sys:virtual"]) # exclude the "virtual" user.
        self.bots = set(u for u in self.users if u.startswith("sys:"))
        self.users -= self.bots
        
        runs = iovs.runs
        self.nruns = len(runs) if runs else "-"
        self.latest_run = max(runs) if runs else "-"
        self.lumi = "{0:.3f}".format(self.lumi) if runs else "-"
    
    @property
    def name_with_url(self):
        if self.virtual or self.intolerable:
            return '<a href="{0}/{1}.svg">{1}</a>'.format(GRAPH_URL, self.name)
        else:
            return "{0}".format(self.name)
    
    @property
    def content(self):
        td = '<td class="icon {class_}"><span>{0}</span></td>'
        bot_cls = user_cls = ""
        if len(self.users) == 1: user_cls = "user"
        if len(self.users) > 1: user_cls = "users"
        if len(self.bots): bot_cls = "bots"
        
        virt_class = "virtual" if self.virtual else "primary"
        tol_class = "intolerable" if self.intolerable else "tolerable"
        virt_sort = 1 if self.virtual else 0
        tol_sort = 1 if self.virtual else 0
        
        c = [
            td.format(virt_sort, class_=virt_class),
            td.format(tol_sort, class_=tol_class),
            td.format(", ".join(self.users), class_=user_cls),
            td.format(", ".join(self.bots), class_=bot_cls),
            content_string.format(d=self)
        ]
        return "".join(c)

def build_table(**kwargs):
    """
    Build the HTML content
    """
    path = resource_filename("DQDefects.data", "table.html")
    with open(path) as fd:
        template = MarkupTemplate(fd, path)
     
    stream = template.generate(HTML=HTML, **kwargs)
    serializer = HTMLSerializer(doctype="html5")
    content = encode(serializer(stream))
    return content

def build_defects(descriptions, virtuals, intolerables, lbs, lumis, all_defects):
    
    with timer("Sort defects by channel"):
        dbc = all_defects.by_channel
        
    with timer("Compute luminosities"):
        lumi_by_defect = compute_lumi_many_channels(lbs, lumis, all_defects)
    
    defects = []
    
    for name, description in sorted(descriptions.iteritems()):
        virtual, intolerable = name in virtuals, name in intolerables
        defect = Defect(name, description, dbc[name], virtual, intolerable, 
                        lumi_by_defect.get(name, 0))
        defects.append(defect)

    defects.sort(key=lambda d: (d.virtual, not d.intolerable))

    return defects

def copy_art(target_dir):
    art_base = dirname(resource_filename("DQDefects.data", "table.html"))
    
    for filename in listdir(art_base):
        _, _, ext = filename.rpartition(".")
        if ext.lower() not in ("png", "js", "css"):
            continue
        copy2(pjoin(art_base, filename), pjoin(target_dir, filename))

def main():
    d = DefectsDB()
        
    iov_range = None, None #
    iov_range = 185000 << 32, 185500 << 32
    #iov_range = 177682 << 32, 200000 << 32
    
    with timer("Fetch defect info"):
        all_defects = d.retrieve(*iov_range)
    
    with timer("fetch peripheral information"):    
        descriptions = d.all_defect_descriptions
        intolerable = d.get_intolerable_defects()
        virtual = d.virtual_defect_names
        
    with timer("Fetch lumi inputs"):
        lbs, lumis = fetch_lumi_inputs(iov_range, "ONLINE")
    
    d = build_defects(descriptions, virtual, intolerable, lbs, lumis, all_defects)
    
    target_dir = "/afs/cern.ch/user/p/pwaller/www/defects/test"
    art_dir = pjoin(target_dir, "extern")
    
    if not exists(art_dir):
        makedirs(art_dir)
    
    copy_art(art_dir)
    
    content = build_table(headings=heading_titles, defects=d)
    with open(pjoin(target_dir, "test.html"), "w") as fd:
        fd.write(content)
    

if __name__ == "__main__":
    main()
