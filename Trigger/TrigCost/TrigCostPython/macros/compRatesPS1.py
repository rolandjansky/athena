#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from TrigCostPython import TrigCostAnalysis, TrigCostXML 
from optparse import OptionParser
import os, sys, string, math, shutil, re
from TrigCostPython.TrigCostAnalysis import search_file
import TrigCostPython.compRates_head

#-------------------------------------------------------------------
# Command line options
    
p = OptionParser(usage="usage: <XML rate file> <XML rate file>  -o <output path>")
p.add_option( "-o", type    = "string", default = "",
              dest = "output_path",
              help = "path to output directory" )

p.add_option( "", "--key-list", type    = "string", default = "",
              dest = "key_list",
              help = "list of keys for matching chain names" )

p.add_option( "", "--ignore", type    = "string", default = "EMPTY,BGRP,win",
              dest = "ignore",
              help = "list of keys for ignoring chain names" )

p.add_option( "-t", "--text", action  = "store_true", default = False,
              dest = "text",
              help = "produce text only" )

p.add_option( "", "--show-pspt", action  = "store_true", default = False,
              dest = "show_pspt",
              help = "show PS and PT" )

p.add_option( "", "--show-ps", action  = "store_true", default = False,
              dest = "show_ps",
              help = "show PS" )

p.add_option( "", "--show-pspt-tot", action  = "store_true", default = False,
              dest = "show_pspt_tot",
              help = "show PS and PT, the PS is the total over the lower levels " )

p.add_option( "", "--show-err", action  = "store_true", default = False,
              dest = "show_err",
              help = "show errors")

p.add_option( "", "--hlt-rejection", action  = "store_true", default = False,
              dest = "hlt_rej",
              help = "Displays HLT/L1  instead of plain rates")

p.add_option( "", "--use-mhz", action  = "store_true", default = False,
              dest = "use_mhz",
              help = "use mHz unnits" )

p.add_option( "", "--latex", action  = "store_true", default = False,
              dest = "latex",
              help = "make latex table" )

p.add_option( "", "--show-diff", action  = "store_true", default = False,
              dest = "show_diff",
              help = "show difference" )

p.add_option( "", "--apply-PS", action  = "store_true", default = False,
              dest = "apply_PS",
              help = "apply prescales" )

p.add_option( "", "--undo-PS", action  = "store_true", default = False,
              dest = "undo_PS",
              help = "undo prescales" )

p.add_option( "", "--hide-zero-PS", action  = "store_true", default = False,
              dest = "hide_zero_PS",
              help = "hide PS of 0 and -1" )

p.add_option( "", "--show-zero-rates", action  = "store_true", default = False,
              dest = "show_zero_rates",
              help = "show chain when all rates are zero" )

p.add_option( "", "--copy-xml", action  = "store_true", default = False,
              dest = "copy_xml",
              help = "Make a copy of input xml files in output directory" )
            
p.add_option( "", "--do-multi", action  = "store_true", default = False,
              dest = "do_multi",
              help = "Turn on comparison of multiple rates" )
            
p.add_option( "", "--do-groups", action  = "store_true", default = False,
              dest = "do_groups",
              help = "Turn on comparison of group rates" )
            
p.add_option( "", "--do-plots", action  = "store_true", default = False,
              dest = "do_plots",
              help = "Turn on plotting of comparison results" )

p.add_option( "", "--ratio-columns", type    = "string", default = "",
              dest = "ratio_columns",
              help = "Used with do-multi, comma separated list" 
              +"of columns of which ratios are made (starting with 1)," 
              +"using more than one results in large output tables" )

p.add_option( "", "--labels", type    = "string", default = "",
              dest = "labels",
              help = "comma separated list of column labels" )

p.add_option( "", "--summary-html", action  = "store_true", default = False,
              dest = "summary_html",
              help = "Create a summary html containing links to all outputs" )
            
p.add_option( "", "--summary-name", type  = "string", default = "",
              dest = "summary_name",
              help = "Name of summary html, first default is compilation of column labels, second is date,time" )

p.add_option( "", "--scale", type  = "string", default = "",
              dest = "scale",
              help = "Scale the rates, using comma separated values" )

p.add_option( "", "--regex", type  = "string", default = "",
              dest = "regex",
              help = "Select only chains which match the regex pattern" )

p.add_option( "", "--err-log", type  = "string", default = "",
              dest = "errlog",
              help = "Give error log")

p.add_option( "--html-snippet", type    = "string", default = "",
              dest = "html_snippet",
              help = "path to HTML snippet" )

p.add_option( "--hide-chains", type="string", default="",
              dest="hide_chains",
              help="Comma separated list of regex expressions for triggers to hide in the output")

p.add_option( "--hide-empty-cosmic", action  = "store_true", default = False,
              dest="hide_empty_cosmic",
              help="Hide _EMPTY, _empty, _cosmic chains.")

p.add_option( "--hide-firstempty", action  = "store_true", default = False,
              dest="hide_firstempty",
              help="Hide _FIRSTEMPTY, _firstempty chain")

p.add_option( "--hide-unpaired", action  = "store_true", default = False,
              dest="hide_unpaired",
              help="Hide _unpaired chains")

p.add_option( "--css", type="string", default="",
              dest="css",
              help="Comma separated list of stylesheets to include")

p.add_option( "--favicon", type="string", default="",
              dest="favicon",
              help="Address of favicon")

p.add_option( "--js-prefix", type="string", default="",
              dest="js_prefix",
              help="Prefix for js files")

(options, args) = p.parse_args()

if options.show_pspt_tot:
    options.show_pspt=True

#print options

#---------------------------------------------------------------------
def ReadTextFile(filename, key_list, ignore_list):

    # Copy results to chain objects
    rates={}

    # Create and initialize parser
    try:
        myf = open(filename)
    except:
        print "Failed to open file:",filename
        return rates

    print 'Parsing Text file:',filename
    print 'Expected format :'
    print 'Trigger_i  Trig_name  Trig_events   Trig_events/MBTS_1_1-events   error'
    
    for line in myf :
      
        tok = line.split();
        if len(tok) < 5 :
            continue
        if tok[0].isalpha() :
            continue 
        
        ch=chain()
    
        ch.name     = tok[1];
        ch.ps       = 1
        ch.pt       = 0
        ch.nevt     = string.atof(tok[2])
        ch.rate     = string.atof(tok[3])
        ch.rate_err = string.atof(tok[4])
        ch.eff      = 0.0
        ch.pseff    = 0.0
      
        if ch.nevt < 0:
            ch.nevt_err = math.sqrt(ch.nevt)
        else:
            ch.nevt_err = 0
            
        match_chain  = len(key_list) < 1
        ignore_chain = False
        
        for key in key_list:
            if ch.name == key or ch.name.count(key) > 0:
                match_chain = True
                break

        for ikey in ignore_list:
            if ch.name == ikey or ch.name.count(ikey) > 0:
                ignore_chain = True
                break

        if match_chain and not ignore_chain:
            rates[ch.name]=ch

    return rates

#---------------------------------------------------------------------
# Object to store tables

class ResultTable :

    def __init__(self) :
        self.headers = []
        self.column_types = []
        self.displays = []
        self.entries = dict()

    def add_header(self, name) :
        self.headers.append(name)

    def add_column_type(self, name) :
        self.column_types.append(name)
        
    def add_display(self, value) :
        self.displays.append(value)

    def add_entry(self, header, val) :
        if header not in self.headers :
            print 'Provided header, %s, was not inserted' %header
            print 'Possible headers are ' + ', '.join(self.headers)
        else :
            self.entries.setdefault(header, []).append(val)

    def check_rows(self) :
        #consistency check
        row_count = set()
        for val in self.entries.values() :
            row_count.add(len(val))

        if len(row_count) > 1 :
            
            print "ERROR - columns have different numbers of rows"
            for header, vals in self.entries.iteritems() :
                print '%s has %d rows' %(header, len(vals))
            sys.exit(-1)
        return row_count.pop()
    
    def write_html(self, file) :

        if len(self.entries) == 0 :
            print "Table has no entries"
            return

        nrows = self.check_rows()

        file.write('<html>\n<head>\n')
        file.write('<script type="text/ecmascript">\n')
        
        if options.show_pspt:
            file.write('var show_ps         = true ;\n')
            file.write('var show_pt         = true ;\n')
        elif options.show_ps:
            file.write('var show_ps         = true ;\n')
            file.write('var show_pt         = false ;\n')
        else:
            file.write('var show_ps         = false ;\n')
            file.write('var show_pt         = false ;\n')
        if options.show_diff:
            file.write('var show_diff       = true ;\n')
        else:
            file.write('var show_diff       = false ;\n')
        if options.show_zero_rates:
            file.write('var show_isZero     = true ;\n')
        else:
            file.write('var show_isZero     = false ;\n')

        if options.hide_empty_cosmic:
            file.write('var show_empty      = false ;\n')
            file.write('var show_cosmic     = false ;\n')
        else:
            file.write('var show_empty      = true ;\n')
            file.write('var show_cosmic     = true ;\n')
        if options.hide_firstempty:
            file.write('var show_firstempty = false ;\n')
        else:
            file.write('var show_firstempty = true ;\n')
        if options.hide_unpaired:
            file.write('var show_unpaired   = false ;\n')
        else:
            file.write('var show_unpaired   = true ;\n')
            
        file.write('</script>\n')

        file.write('<title>Rates comparison</title>\n')
        file.write('<script src="%ssorttable.js"></script>\n'%options.js_prefix)
        file.write('<script src="%shighlight.js"></script>\n'%options.js_prefix)

        if options.favicon !='':
        	file.write('<link rel="shortcut icon" href="%s"/>\n'%options.favicon)
        csss = options.css.split(',')
        for css in csss:
        	if css!='':
        		file.write('<link type="text/css" rel="stylesheet" media="all" href="%s" />\n'%css)

        datapath=os.environ.get("DATAPATH")
        htmlfile=search_file("TrigCostPython/compRates_head.html",datapath)
        fin = open(htmlfile, 'r')
        file.write( fin.read() )        
        file.write('</div>\n')

        if options.html_snippet!='':
            file.write('<div class="h2_wrapper"><h2 class="compRates" style="display:inline">Meta information:</h2> <input type="submit" id="expandCollapse_snippet" onclick="expandCollapse(\'collapsible_snippet\', \'expandCollapse_snippet\')" value="Expand"/></div>\n')
            file.write('<div class="collapsed" id="collapsible_snippet">\n')
            fin = open(options.html_snippet, 'r')
            file.write( fin.read() )
            file.write('</div>')
        
        file.write('<table id="chain_table" border="1" frame="box" rules="cols" class="sortable">\n')

        file.write('<thead><tr>\n')
        indices = range(0,len(self.headers))
        for i in indices:
            h = self.headers[i]
            
            className = self.column_types[i]
            if self.displays[i] == False:
                className = className + self.column_types[i].replace('rate_', ' ') + '_hide'
            
            file.write('  <th class="%s">%s</th>\n' %(className, self.headers[i]))
        file.write('</tr></thead>\n<tbody>\n')
        
        for row in xrange(0,nrows) :
            for i in indices:
                h = self.headers[i]
                if h!='rate_isZero':
                    continue
                if self.entries[h][row]==True:
                    if options.show_zero_rates==True:
                        file.write('<tr class="rate_isZero">\n')
                    else:
                        file.write('<tr class="rate_isZero isZero_hide">\n')
                else:
                    file.write('<tr >\n')
            
            indices = range(0,len(self.headers))
            for i in indices:
                h = self.headers[i]
                entry = self.entries[h][row]

                # Sort out display
                className = self.column_types[i]
                if self.displays[i] == False:
                    className = className + self.column_types[i].replace('rate_', ' ') + '_hide'

                # Sort out format
                format = '%s'
                if type(entry) == type(0.0) :
                    format = '%.3g'
                elif type(entry) == type(0) :
                    format = '%d'

                # Write output
                output_string = '  <td class="%s">'+format+'</td>\n'
                file.write(output_string %(className, entry))
            file.write('</tr>\n')
        file.write('</tr>\n</tbody>\n<tfoot></tfoot>\n')
        file.write('</table>\n')

        file.write('</body>\n</html>\n')
        
    def write_latex(self, file, format = '') :

        nrows = self.check_rows()

        # Use default format
        if format == '' :
            format = '| l'
            for i in xrange(0,len(self.headers) - 1) :
                format += ' |c' 
            format += ' |'
            
        file.write(r'\begin{tabular}{%s}' %format + '\n')
        line = ''
        for h in self.headers :
            line += r'\textbf{%s}  &  ' %h

        line = line.replace('_',r'\_')
        line = line.replace('+-', r'$\pm$')
        line = line.rstrip().rstrip('&')
        file.write(line + r'  \\ ' + '\n')
        
        for row in xrange(0,nrows) :
            line = ''
            for h in self.headers :
                line += r'%s  &  ' %self.entries[h][row]
            line = line.replace('_',r'\_')
            line = line.replace('+-', r'$\pm$')
            line = line.rstrip().rstrip('&')
            file.write(line + r'  \\ ' + '\n')
               
        file.write('\end{tabular}\n')
        
    headers = []
    column_types = []
    entries = dict()
    
#---------------------------------------------------------------------
# Functions to access ROOT modules
#
def GetCanvas(name, title = "", sizex=1000, sizey=500):

    global options

    if options.text: return None

    import ROOT

    ROOT.gStyle.SetOptStat('rmei')
    ROOT.gStyle.SetFillColor(10)
    ROOT.gStyle.SetFrameFillColor(10)
    ROOT.gStyle.SetCanvasColor(10)
    ROOT.gStyle.SetPadColor(10)
    ROOT.gStyle.SetTitleFillColor(10)
    ROOT.gStyle.SetStatColor(10)
    ROOT.gROOT.SetBatch(True)
    
    canv = ROOT.TCanvas(name, title, sizex, sizey)
    return canv

def GetHist(name, varname, option):

    if options.text: return None

    import ROOT
    h = ROOT.TH1D() 
    if varname == 'rate' and option == 'diff':
        h = ROOT.TH1D(name, name, 100, -20.0, 20.0)
        h.GetXaxis().SetTitle('rate difference (Hz)')
    
    if varname == 'rate' and option == 'ratio':
        h = ROOT.TH1D(name, name, 100, -3.0, 3.0)
        h.GetXaxis().SetTitle('rate difference/statistical error')
    
    if varname == 'nevt' and option == 'diff':
        h = ROOT.TH1D(name, name, 100, -2.0e2, 2.0e2)
        h.GetXaxis().SetTitle('number of events difference')        
    
    if varname == 'nevt' and option == 'ratio':
        h = ROOT.TH1D(name, name, 100, -3.0, 3.0)
        h.GetXaxis().SetTitle('number of events difference/statistical error')

    if option == 'frac':
        h = ROOT.TH1D(name, name, 50, 0.0, 5.0)
        h.GetXaxis().SetTitle('Fractional rate difference')

    if option == 'sigma_frac':
        h = ROOT.TH1D(name, name, 50, -10.0, 10.0)
        h.GetXaxis().SetTitle('Rate difference (in sigma)')

    if option == 'chain_frac':
        h = ROOT.TH1D(name, name, 1000, 0.0, 1.0)
        h.GetXaxis().SetTitle('')
        h.GetYaxis().SetTitle('Rate Fraction')
        
    h.GetXaxis().CenterTitle()
    return h

def GetLine():
    import ROOT
    line = ROOT.TLine()
    line.SetLineWidth(3)

    return line

            
#---------------------------------------------------------------------
# Compare two results
def fillBlanks(var, nblanks):
    rstr = str(var)
    for i in range(len(str(var)), nblanks+1):
        rstr += ' '

    return rstr


#---------------------------------------------------------------------
# Compare multiple results
def CompareRates(opath, level, varname, input_rates, index_html, ratetype=""):

    if options.latex :
        ofile = open(opath+'compare_'+ratetype+varname+'_'+level+'.tex', 'w')
    else :
        ofile = open(opath+'compare_'+ratetype+varname+'_'+level+'.html', 'w')

    table =  ResultTable()

    if options.do_plots:
        hname_frac  = ratetype+varname+'_fraction_'+level    
        hist_frac   = GetHist(hname_frac, varname, 'frac')
        hname_sfrac = ratetype+varname+'_sigma_fraction_'+level    
        hist_sfrac  = GetHist(hname_sfrac, varname, 'sigma_frac')
        canv_frac   = GetCanvas(hname_frac, hname_frac)

        #canv_sfrac = GetCanvas(hname_sfrac, hname_sfrac)

        hname_chfrac = ratetype+varname+'_chain_fraction_'+level    
        hist_chfrac  = GetHist(hname_chfrac, varname, 'chain_frac')
        canv_chfrac  = GetCanvas(hname_chfrac, hname_chfrac, 5000, 2500)
        chfrac_maxchains = 0
    
    column_labels = []

    if options.labels != '' :
        column_labels = options.labels.split(',')
    else :
        for i in xrange(0, len(input_rates)) :
            column_labels.append(str(i))
    
    if len(column_labels) != len(input_rates) :
        print "Number of column labels must be equal to the number of inputs"
        for i in xrange(0, len(input_rates)) :
            column_labels.append(str(i))
    
    # Write headers   
    counter = -1
    table.add_header('Chain')
    table.add_column_type('chain_name')
    table.add_display(True)
    for result in input_rates :
        counter += 1
        label1 = column_labels[counter]
        table.add_header(label1)
        table.add_column_type('rate')
        table.add_display(True)
        
        table.add_header('%s_PS'%label1)
        table.add_column_type('rate_ps')
        table.add_display( (options.show_pspt or options.show_ps) )
        
        table.add_header('%s_PT'%label1)
        table.add_column_type('rate_pt')
        table.add_display( options.show_pspt )
        
    if options.ratio_columns != '' :
        for col in options.ratio_columns.split(',') :
            label_denom = column_labels[int(col)-1]
            counter = 0 # counts the results
            for rate in input_rates :
                counter += 1
                label_num = column_labels[counter-1]
                if counter == int(col) : # Don't make raio with self
                    continue
                table.add_header('%s/%s' %(label_num, label_denom))
                table.add_column_type('rate_ratio')
                table.add_display(True)
                
                table.add_header('%s-%s' %(label_num, label_denom))
                table.add_column_type('rate_diff')
                table.add_display( options.show_diff )
    
    table.add_header('rate_isZero')
    table.add_column_type('rate_isZero')
    table.add_display(False)
                    
    # Loop over all results and get a complete list of triggers
    all_chains = set()
    for result in input_rates :        
        #for chain in result.GetChainNames(level) :
        #    all_chains.add(chain)
        
        for ch in result.GetChains(level):
            if string.count(ratetype,"Groups"):
                if string.count(ch.GetName(),"str"):
                    pass
                    #print "STREAM",ch.GetName(),ch.IsGroup()
                if ch.IsGroup():
                    if options.regex!='':
                        if re.match(options.regex, ch.GetName()):
                            all_chains.add(ch.GetName())
                    else:
                        all_chains.add(ch.GetName())
            else:
                if not ch.IsGroup():
                    if options.regex!='':
                        if re.match(options.regex, ch.GetName()):
                            all_chains.add(ch.GetName())
                    else:
                        all_chains.add(ch.GetName())
    
    
    rateTable = []
    # Write values
    for chain in all_chains :
        
        # Check for PS of 0 and -1
        # Check ALL input rates for this condition
        
        skip_chain = False
        if options.hide_zero_PS:
            rate_counter = 0
            skip_counter = 0
            thischain = result.GetChain(chain)
            for result in input_rates :
                rate_counter += 1
                var = getattr(thischain, "prescale")
                if var == None  :
                    var = getattr(thischain, "chain_prescale")                    
                if var==0  :
                    skip_counter+=1
                if var<0:
                    skip_counter+=1
            if skip_counter==rate_counter:
                skip_chain = True
        if skip_chain:
            continue
        
        table.add_entry('Chain', chain)
        counter = -1
        isZero = False
        if varname=='rate':
            # Set isZero to true now.  Set it to false if at least one of the inputs has a non zero rate
            isZero = True
        for result in input_rates :
            counter += 1
            
            if chain not in result.GetChainNames(level) :
                nblanks = 1
                table.add_entry('%s_PS'%column_labels[counter],0)
                table.add_entry('%s_PT'%column_labels[counter],0)
                    
                for i in xrange(0,nblanks) :
                    table.add_entry(column_labels[counter+i], " ")
                continue
            
            thischain = result.GetChain(chain)
            
            Var = getattr(thischain, varname)
            # Check for zero rate
            if varname=='rate':
                if Var > 0:
                    isZero = False
            Err = 0
            if varname=='rate':
                Err = getattr(thischain, varname+"err")
            
            if options.use_mhz:
                Var *= 1000
                Err *= 1000
            
            scale_factor = 1
            if options.scale!='':
                scales = options.scale.split(',')
                scale_counter = -1
                for scale in scales:
                    scale_counter += 1
                    if counter==scale_counter:
                        scale_factor = string.atof(scale)
            
            if varname=="rate":
                Var = Var*scale_factor
                Err = Err*scale_factor
            
            if options.hlt_rej and level!='L1':
                try:
                    # Ignoring error on lower chain (probably smaller and correlated)                   
                    lower_chain = thischain.GetLowerChain()
                    if not result.HasChain(lower_chain):
                        raise  TrigCostAnalysis.VarNotAvailable("Empty lower chain for "+chain)
                    Var_lower = result.GetChain(lower_chain).GetAttrWithCheck(varname)
                    if Var_lower>0:
                        Var /= Var_lower
                        Err /= Var_lower
                    else:
                        Var = 0
                        Err = 0
                        if Var > 0:
                            print "Zero lower chain (1)?! ",chain,lower_chain,Var,Var_lower
                except TrigCostAnalysis.VarNotAvailable,e:
                    Var = 0
                    Err = 0
                    print "Missing lower chain for ",chain
            
            ps = thischain.GetPrescale()
            pt = thischain.GetAttrWithDefault("passthrough", -1)
            if level != 'L1' and result.source!='XML_prediction':
                lowerchain = thischain.GetAttrWithDefault("lowerchain", "none")
                
                if lowerchain!='none' :
                    if result.HasChain(lowerchain):
                        lchain = result.GetChain(thischain.lowerchain)
                        ps *= lchain.prescale
                
                        if level == 'EF':
                            if result.HasChain(lchain.lowerchain) :
                                llchain = result.GetChain(lchain.lowerchain)
                                ps *= llchain.prescale
            
            table.add_entry('%s_PS'%column_labels[counter],'%.2f' %ps)
            table.add_entry('%s_PT'%column_labels[counter],'%.2f' %pt)
            
            if options.show_err :
                table.add_entry(column_labels[counter], '%6.2f +- %6.2f' %(Var,Err))
            else : 
                table.add_entry(column_labels[counter], '%6.2f' %Var)
            
        # Write ratios and differences
        if options.ratio_columns != '' :
            for col in options.ratio_columns.split(',') :
                counter = 0 # counts the results
                ratio_rate = input_rates[int(col)-1] 
                label_denom = column_labels[int(col)-1]
                for rate in input_rates :
                    counter += 1
                    label_num = column_labels[counter-1]
                    if counter == int(col) : # Don't make raio with self
                        continue
                    # Get labels                    
                    if chain not in ratio_rate.GetChainNames(level) \
	                      or chain not in rate.GetChainNames(level) :
                        nblanks = 1
                        for i in xrange(0,nblanks) :
                            table.add_entry('%s/%s' %(label_num, label_denom)," ")
                            table.add_entry('%s-%s' %(label_num, label_denom)," ")
                        continue
                    
                    Var1 = rate.GetChain(chain).GetAttrWithCheck(varname)
                    Var2 = ratio_rate.GetChain(chain).GetAttrWithCheck(varname)
                    Err1 = 0
                    Err2 = 0
                    if varname=='rate':
                        Err1 = rate.GetChain(chain).GetAttrWithCheck(varname+"err")
                        Err2 = ratio_rate.GetChain(chain).GetAttrWithCheck(varname+"err")
                    
                    scale_factor_1 = 1
                    scale_factor_2 = 1
                    if options.scale!='':
                        scales = options.scale.split(',')
                        scale_counter = -1
                        for scale in scales:
                            scale_counter += 1
                            if int(col)==scale_counter:
                                scale_factor_1 = string.atof(scale)
                            if counter==scale_counter:
                                scale_factor_2 = string.atof(scale)
                    if options.apply_PS:
                        if rate.GetChain(chain).GetAttrWithCheck("prescale")>0:
                            scale_factor_1 = scale_factor_1/rate.GetChain(chain).GetAttrWithCheck("prescale")
                        if ratio_rate.GetChain(chain).GetAttrWithCheck("prescale")>0:
                            scale_factor_2 = scale_factor_2/ratio_rate.GetChain(chain).GetAttrWithCheck("prescale")

                    
                    if options.hlt_rej and level!='L1':
                    # Ignoring error on lower chain (probably smaller and correlated)
                        try:
                            lower_chain1=rate.GetChain(chain).GetLowerChain()
                            if not result.HasChain(lower_chain1):
                                raise  TrigCostAnalysis.VarNotAvailable("Empty lower chain for "+chain)
                            Var1_lower = rate.GetChain(lower_chain).GetAttrWithCheck(varname)
                            lower_chain2=ratio_rate.GetChain(chain).GetLowerChain()
                            if not result.HasChain(lower_chain2):
                                raise  TrigCostAnalysis.VarNotAvailable("Empty lower chain for "+chain)
                            Var2_lower = ratio_rate.GetChain(lower_chain2).GetAttrWithCheck(varname)
                            if Var1_lower>0 and Var2_lower>0:
                                Var1 /= Var1_lower
                                Err1 /= Var1_lower
                                Var2 /= Var2_lower
                                Err2 /= Var2_lower
                            else:
                                Var1 = 0
                                Var2 = 0
                                Err1 = 0
                                Err2 = 0
                                if Var1_lower>0 and Var1 > 0:
                                    print "Zero lower chain (2)?! ",chain,Var1,Var1_lower
                                if Var2_lower>0 and Var2 > 0:
                                    print "Zero lower chain (3)?! ",chain,Var2,Var2_lower
                        except  TrigCostAnalysis.VarNotAvailable,e:
                            pass
                    
                    if varname=="rate":
                        Var1 = Var1*scale_factor_1
                        Err1 = Err1*scale_factor_1
                        Var2 = Var2*scale_factor_2
                        Err2 = Err2*scale_factor_2
                    diff = Var1 - Var2
                    
                    ratio = 0.0   
                    if Var2 != 0.0 :
                        ratio = Var1/Var2
                    ratio_err = 0.0
                    if Var1 != 0.0 and Var2 != 0.0 :
                        ratio_err = ratio * math.sqrt( (Err1/Var1)*(Err1/Var1) + (Err2/Var2)*(Err2/Var2))
                        ratio_diff = 0.0
                    if Err1 != 0.0:
                        ratio_diff = (Var1-Var2)/Err1
	
                    if options.do_plots and Var1 != 0.0 and Var2 != 0.0:
                        if hist_frac != None:
                            hist_frac.Fill(ratio)        
                        if hist_sfrac != None:
                            hist_sfrac.Fill(ratio_diff)        
                        if hist_chfrac != None:
                            chfrac_maxchains += 2
                            hist_chfrac.SetBinContent(chfrac_maxchains, ratio)        
                            hist_chfrac.SetBinError(chfrac_maxchains, ratio_err)        
                            hist_chfrac.GetXaxis().SetBinLabel(chfrac_maxchains, chain)
                            # print "%d %s"% (chfrac_maxchains,chain)
                    
                    if options.show_err :
                        table.add_entry('%s/%s' %(label_num, label_denom), '%6.2f +- %6.2f' %(ratio, ratio_err))
                    else :
                        table.add_entry('%s/%s' %(label_num, label_denom), '%6.2f' %ratio)
                    table.add_entry('%s-%s' %(label_num, label_denom), '%6.2f' %diff)
        table.add_entry('rate_isZero', isZero)
    
    # Print the canvas
    if options.do_plots:
        print_frac = 1
        if hist_frac != None and hist_frac.GetEntries() == 0:
            print_frac = 0
        if canv_frac != None and print_frac == 1:
            canv_frac.Divide(2,1);
            canv_frac.Draw()
            canv_frac.cd(1)
            hist_frac.Draw()
            canv_frac.cd(2)
            hist_sfrac.Draw()
            canv_frac.Update()
            canv_frac.Print(gpath+canv_frac.GetName()+".gif")
        
        # complicated canvas in order to get the plotting pretty
        print_chfrac = 1
        if hist_chfrac != None and hist_chfrac.GetEntries() == 0:
            print_chfrac = 0
        if canv_chfrac != None and print_chfrac == 1:
            
            canv_chfrac.cd()
            canv_chfrac.SetBottomMargin(0.35)
            canv_chfrac.Draw()
            hist_chfrac.SetMinimum(0.0)
            hist_chfrac.SetMaximum(2.5)
            hist_chfrac.GetXaxis().SetRange(1,chfrac_maxchains+1)
            
            hist_chfrac.SetFillColor(38)
            hist_chfrac.SetMarkerStyle(2)
            
            hist_chfrac.GetXaxis().LabelsOption("v")
            #        hist_chfrac.GetXaxis().SetLabelOffset(0.015)
            hist_chfrac.GetXaxis().SetLabelSize(0.03)
            hist_chfrac.Draw("hep")
            
            line = GetLine()
            line.DrawLine(0.0, 1.0, 1.0 ,1.0)
            
            canv_chfrac.Update()
            canv_chfrac.Print(gpath+canv_chfrac.GetName()+".gif")
            #        canv_chfrac.Print(gpath+canv_chfrac.GetName()+".root")
    
    if options.latex :
        table.write_latex(ofile)
    else :
        table.write_html(ofile)
    if index_html != None :
        # write the top line
        index_html.write("<a STYLE=\"text-decoration:none\">Compare %s %s %s</a>\n" %(ratetype, level, varname))
        
        #link to table
        index_html.write("<a STYLE=\"text-decoration:none\" href=\"%s\">\tTable\n" %('compare_'+ratetype+varname+'_'+level+'.html'))
        
        #link to histograms
        if options.do_plots and print_frac == 1:
            index_html.write("<a STYLE=\"text-decoration:none\" href=\"%s\">\tHistogram - compare rate differences</a>\n" %(canv_frac.GetName()+".gif"));
        if options.do_plots and print_chfrac == 1:
            index_html.write("<a STYLE=\"text-decoration:none\" href=\"%s\">\tHistogram - compare rates chain-by-chain</a>\n" %(canv_chfrac.GetName()+".gif"));
        # do padding
        index_html.write("<a STYLE=\"text-decoration:none\">\n")
        
#---------------------------------------------------------------------
# Compare multiple results
def CompareMultiRates(opath, level, varname, input_rates, column_labels):

    if len(inupt_rates) !=  len(column_labels) :
        print "column lables don't match input "
        return 
 
    ofile = open(opath+'compare_multi_'+varname+'_'+level+'.html', 'w')
    global newRates, oldRates, options

    tot_chains = set()
    max_len = []
    for result in input_rates :
        for chain in result.GetChainNames(level) :
            tot_chains.add(chain)
    
    # Find the max length of chain names for formatting
    max_len.append(0)
    for chain in tot_chains :
        if len(chain) > max_len[0] :
            max_len[0] = len(chain)

    # Find the max length of data values for formatting
    counter = 0
    for result in input_rates :
        counter += 1
        max_len.append( 0)
        for chain in tot_chains :
            if chain not in result.GetChainNames(level) :
                continue
            if len( str( getattr(result.GetChain(chain),varname) ) ) > max_len[counter] :
                max_len[counter] = len( str( getattr(result.GetChain(chain),varname) ) )
    
    ofile.write('<script src=\"%ssorttable.js\"></script>\n'%options.js_prefix)
    ofile.write('<script src=\"%shighlight.js\"></script>\n'%options.js_prefix)
    ofile.write('<table border=\"0\" class=\"sortable\">\n')
    ofile.write('<tr>\n')
    ofile.write('  <th>chain</th>\n')

    if show_ps==True or show_pspt==True:
        ofile.write('  <th class="ps">PS ratio</th>\n')
    else:
        ofile.write('  <th class="ps" style="display:none">PS ratio</th>\n')
    if show_ps==True or show_pspt==True:
        ofile.write('  <th class="pt">PT</th>\n')
    else:
        ofile.write('  <th class="pt" style="display:none">PT</th>\n')
    
    save_rates = dict() 
    #data columns
    for i in xrange(0, len(input_rates)) :
        ofile.write('  <th>%s</th>\n' %column_labels[i])

    #ratio columns
    for i in xrange(0, len(input_rates)-1) :
        #ofile.write('<th>%s/%s ratio</th>' %(column_labels[-1], column_labels[i]))
        ofile.write('  <th>%s/%s ratio</th>\n' %(column_labels[i], column_labels[-1]))
        ofile.write('  <th>err</th>\n')
    
    ofile.write('</tr>\n')
    
    rateTable = []
    
    #collect a complete list of trigger names

    for chain in tot_chains :
        first_rate = 0.0
        first_pass = 0
        
        lineS = '<tr>\n'
        tdStr = '</td>\n'
        counter = 0
        for result in input_rates :
            counter += 1
            if first_pass == 0 :
                lineS += ('  <td>'+chain+tdStr).ljust(max_len[0]+len('<td></td>\n'))
                first_pass = 1
        
            if chain not in result.GetChainNames(level) :
                lineS += '  <td>'+tdStr
                continue
                
            if counter == len(input_rates) :
                save_rates = result
            
             
            lineS += ('  <td>'+'%.3f'%(getattr(result.GetChain(chain),varname)*1000.0)+tdStr).ljust(max_len[counter]+len('  <td></td>\n'))
            
        for result in input_rates :
            if result == save_rates :
                continue

            if chain not in result.GetChainNames(level) or  \
                      chain not in save_rates.GetChainNames(level) :
                
                lineS += '  <td>'+tdStr
                lineS += '  <td>'+tdStr
                continue
                
            if  getattr(save_rates.GetChain(chain),varname) == 0.0 or \
                            getattr(result.GetChain(chain),varname) == 0.0  :
            
                lineS += '  <td>%.3f</td>\n  <td>%.3f</td>\n'%(0.0, 0.0)

            else :

                num_rate = getattr(save_rates.GetChainName(chain),varname)
                num_err = getattr(save_rates.GetChainName(chain), varname+'err')
                denom_rate = getattr(result.GetChainName(chain), varname)
                denom_err = getattr(result.GetChainName(chain), varname+'err')


                #lineS += ('<td>%.3f</td>'%(num_rate/denom_rate)).ljust(8)
                #lineS += ('<td>%.3f</td>'%((num_rate/denom_rate)*math.sqrt((num_err/num_rate)*(num_err/num_rate)+(denom_err/denom_rate)*(denom_err/denom_rate)) )).ljust(16)
                lineS += ('  <td>%.3f</td>\n'%(denom_rate/num_rate)).ljust(8)
                lineS += ('  <td>%.3f</td>\n'%((denom_rate/num_rate)*math.sqrt((num_err/num_rate)*(num_err/num_rate)+(denom_err/denom_rate)*(denom_err/denom_rate)) )).ljust(16)

        
            if first_rate < 1e-100 :
                first_rate = getattr(result.GetChainName(chain),varname)
        lineS += '</tr>\n'
        rateTable += [ [ first_rate, lineS] ]

    rateTable.sort(lambda x, y: cmp(y[0], x[0]))

    for iline in rateTable:
        ofile.write(iline[1])

    ofile.write('</table>\n')
    ofile.write('<script src=\"highlight.js\"></script>\n') ;
    ofile.write('<META HTTP-EQUIV=\"PRAGMA\" CONTENT=\"NO-CACHE\">')
    ofile.close()
         
    
        
#---------------------------------------------------------------------
# Compare two results
def CompareAttr(gpath, level, varname, option, index):
    
    hname = varname+'_'+option+'_'+level    
    hist = GetHist(hname, varname, option)
    canv = GetCanvas(hname, hname)

    global newRates, oldRates

    for newChain in newRates.values():
        if len(newChain.name) < 2 or newChain.name[0:2] != level:
            continue
        
        if newChain.name not in oldRates.keys():
            print "CompareAttr error: chain not found:",newChain.name
            continue

        oldChain = oldRates[newChain.name]

        if option.count("noPT") > 0 and newChain.pt > 0.0:
            continue

        oldAtt = getattr(oldChain, varname)
        newAtt = getattr(newChain, varname)
        diffAtt = newAtt-oldAtt

        if varname == 'nevt' and oldAtt > 0.0 and math.fabs(diffAtt)/oldAtt > 0.1:
            print newChain.name,':',newAtt,'-',oldAtt,'=',newAtt-oldAtt
 
        if option.count('ratio') > 0:
            oldErr = getattr(oldChain, varname+'_err')
            newErr = getattr(newChain, varname+'_err')
           
            if oldErr > 0.0:
                diffAtt = diffAtt/oldErr
            else:
                pass

            if diffAtt > 3.0 and False:
                ps = newChain.ps
                pt = newChain.pt

        if hist != None:
            hist.Fill(diffAtt)        

    if canv != None:
        canv.cd()
        canv.Draw()
        hist.Draw()
        canv.Update()
        canv.Print(gpath+canv.GetName()+".gif")

        if index != None :
            index.write("<a STYLE=\"text-decoration:none\" href=\"%s\">Histogram - compare %s %s %s</a>\n\n" %(canv.GetName()+".gif", level, varname, option));

#---------------------------------------------------------------------    
# Compare two results
def ComparePS_PT():

    compResult = True
    for newChain in newRates.values():
        if newChain.name not in oldRates.keys():
            print "ComparePS_PT - chain not found:",newChain.name
            compResult = False
            continue

        oldChain = oldRates[newChain.name]
        
        if newChain.ps != oldChain.ps:
            print newChain.name,' PS difference:',newChain.ps,'!=',oldChain.ps
            compResult = False
            continue
        
        if newChain.pt != oldChain.pt:
            print newChain.name,' PT difference:',newChain.pt,'!=',oldChain.pt
            compResult = False            
            continue
        
    return compResult

#---------------------------------------------------------------------
# Normalize rates to indicated trigger

def NormalizeRates(chains, ntrigger) :
   
    if ntrigger not in chains.keys() : 
        print 'did not find normalization chain...will not edit rates'
        return chains
    
    if chains[ntrigger].rate < 1e-10 :
        print 'Normalization chain has 0 rate...will not edit rates'
        return chains

    for ch in chains.values() :
       
        old_rate = ch.rate; 
        ch.rate = ch.rate/chains[ntrigger].rate
        if ch.rate < 1e-10 :
            ch.rate_err = 0.0
        else :    
            ch.rate_err = ch.rate*math.sqrt((ch.rate_err/old_rate)*(ch.rate_err/old_rate) + (chains[ntrigger].rate_err/chains[ntrigger].rate)*(chains[ntrigger].rate_err/chains[ntrigger].rate));

    return chains
      
#---------------------------------------------------------------------
# Scale rates 

def ScaleRates(chains, val) :
   
    for ch in chains.values() :
       
        ch.rate *= val
        ch.rate_err *= val;

    return chains
      
#---------------------------------------------------------------------        
# Read XML files 

if len(args) < 2:
    p.error("need path to two input files")    

print 'output dir = "%s"'%options.output_path

if not os.path.isdir(options.output_path):
    os.mkdir(options.output_path.replace(',','-'))
    print "Created output directory:",options.output_path

#
# Copy java script for sorting tables
#
from subprocess import Popen
try :
    datapath=os.environ.get("DATAPATH")
    sorttablefile=search_file("TrigCostPython/sorttable.js",datapath);
#    sortPath = sys.argv[0].replace("compRates.py",sorttablefile)
    pCt = Popen('cp %s %s'%(sorttablefile, options.output_path), shell=True)
    pCt.wait()  
except TypeError :
    print 'Could not get TestArea to locate sorttable.js - it will not be copied to output location.'

try :
    datapath=os.environ.get("DATAPATH")
    highlightfile=search_file("TrigCostPython/highlight.js",datapath);
#    sortPath = sys.argv[0].replace("compRates.py",highlightfile)
    pCt = Popen('cp ' + highlightfile + ' ' + options.output_path, shell=True)
    pCt.wait()  
except TypeError :
    print 'Could not get TestArea to locate highlight.js - it will not be copied to output location.'
    
if options.labels != '' and len(args) != len(options.labels.split(',')) :
    print "Number of column labels must be equal to the number of inputs"
    sys.exit(-1)

if options.key_list == "physics":
    key_list = ['L1_EM3', 'L1_TAU', 'L1_MU', 'L1_XE', 'L1_J', 'L1_TE']
elif options.key_list == "minbias":
    key_list = ['L1_MBTS']
else:
    key_list = options.key_list.split(',')
    
# Parse the input files
# Need to deal with key_list and options.ignore after reading the xml 
input_results = []
for i in args :
    if i.count(".txt") > 0 : 
        input_results.append(ReadTextFile(i))
    elif i.count(".xml") > 0 :
        input_results.append(TrigCostXML.ReadXmlFile(i))
    elif i.count("http") > 0 :
        if i.count('TriggerCosts.xml') == 0 :
            i += '/TriggerCosts.xml'
        input_results.append(TrigCostXML.ReadXmlFile(i))

gpath = options.output_path+"/"



# Copy the xml if requested
if options.copy_xml :
    counter = 0
    for i in args :
        if i.count("http") == 0  :
            shutil.copyfile(i, gpath+'TriggerRates_%d.xml' %counter)
    counter += 1


# Sort ut chains to ignore
options.hide_chains = options.hide_chains.split(',')
if options.hide_empty_cosmic:
    options.hide_chains.append("_empty")
    options.hide_chains.append("_cosmic")
if options.hide_firstempty:
    options.hide_chains.append("_firstempty")
if options.hide_unpaired:
    options.hide_chains.append("_unpaired")
    
index_html = None    
if options.summary_html :
    html_name = ''
    if options.summary_name != '' :
        html_name = options.summary_name
        if html_name.count("html") == 0 :
            html_name += '.html'
    elif options.labels != '' :
        html_name = 'Compare_'+options.labels.replace(',','_') + '.html'
    else :
        import datetime
        html_name = 'Comparison%d_%d_%d_%d_%d.html' %(datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute)
    index_html = open(gpath+html_name, "w"); 
    index_html.write("<pre>\n")
    

levels = ['L1','HLT']

#ComparePS_PT()

if options.undo_PS :
    for result in input_results : 
        print " unprescaling L1"
        for ch in result.GetChains('L1') : # now L1
            cut = ch.GetPrescale()
            if cut != 1 and cut > 0 :
                ps = 0xFFFFFF / (float)(0x1000000 - cut)
                print " change PS for L1: ", ch.GetName() , " R:" , ch.GetRate() , " +- " , ch.GetRateErr() , " ps:" , ps
                ch.SetRate( ch.GetRate()*ps )
                ch.SetRateErr( ch.GetRateErr()*ps );
                ch.SetPrescale( 1. )
                setattr(ch, "undoPSFactor", ps)
                print " [conv cut ",cut,"to PS",ps,"] ----------> unprescaled rate: ", ch.GetRate() , " +- " , ch.GetRateErr()

        print " unprescaling HLT"
        for ch in result.GetChains('HLT') :
            ps = ch.GetPrescale()
            psl1 = 1
            lowerchain = ch.GetAttrWithDefault("lowerchain", "none")
            if lowerchain!='none' and result.HasChain(lowerchain) :
                lchain = result.GetChain(ch.lowerchain)
                psl1 = ch.GetAttrWithDefault("undoPSFactor", 1 )
            pstot = ps * psl1
            if pstot != 1 and pstot >0 :
                print " change PS for HLT: ", ch.GetName() , " R:" , ch.GetRate() , " +- " , ch.GetRateErr() , " psL1:" , psl1 , " psHLT:" , ps , " psTot:", pstot
                ch.SetRate( ch.GetRate()*pstot )
                ch.SetRateErr( ch.GetRateErr()*pstot )
                ch.SetPrescale( 1.)
                ch.prescale = 1
                if ps > 1 : print "       ----------> unprescaled rate ", ch.GetRate() , " +- " , ch.GetRateErr()

for level in levels:    
    #CompareAttr(gpath, level, 'rate', 'diff', index_html)
    #CompareAttr(gpath, level, 'nevts', 'diff', index_html)
    #CompareAttr(gpath, level, 'rate', 'ratio', index_html)
    #CompareAttr(gpath, level, 'nevts', 'ratio', index_html)
  
    CompareRates(gpath, level, 'rate', input_results, index_html)
    #WriteJS(input_results, 'rates.js')
    #CompareRates(gpath, level, 'passthrough', input_results, index_html)
    #CompareRates(gpath, level, 'prescale', input_results, index_html)
    #CompareRates(gpath, level, 'nevts', input_results, index_html)

for level in levels:

    if options.do_groups :
        options.show_pspt = False
        CompareRates(gpath, level, 'rate',input_results, index_html, "Groups")
    
    if options.do_multi :
        CompareMultiRates(gpath, level, 'rate', multi_file, labels)

if index_html != None :
    index_html.write("</pre>\n")
    index_html.close()

