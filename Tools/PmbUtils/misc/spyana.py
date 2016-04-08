# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import PmbUtils.LogFileParsingUtils
do_html=False
openul=0
openli=0
def list_item_start():
    global openli
    openli+=1
    if do_html: print '<li>',
def list_item_end():
    global openli
    openli-=1
    if do_html: print '</li>'
def list_start():
    global openul
    openul+=1
    if do_html: print '<ul>'
def list_end():
    global openul
    openul-=1
    if do_html: print '</ul>'
def list_item(s):
    print '<li>%s</li>'%s if do_html else s

def html_head():
    l=[]
    l+=['<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">']
    l+=['<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">']
    l+=['<head>']
    l+=['  <title>libvectorspy results</title>']
    l+=['  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />']
    l+=['  <meta name="expires" content="0" />']
    l+=['']
    l+=['<script type="text/javascript">']
    l+=['  function toggleDiv(divid){']
    l+=['    if(document.getElementById(divid).style.display == \'none\'){']
    l+=['      document.getElementById(divid).style.display = \'block\';']
    l+=['    }else{']
    l+=['      document.getElementById(divid).style.display = \'none\';']
    l+=['    }']
    l+=['  }']
    l+=['</script>']
    l+=['</head>']
    l+=['<body>']
    return l

def html_foot():
    l=[]
    l+=['</body>']
    l+=['</html>']
    return l

#tcmalloc 0.99 data found by a test c++ program:
__alloc_to_overhead={1:7.11,2:6.115,3:5.08875,4:4.12,5:3.11875,6:2.13,7:1.12,8:0.09,9:7.25625,10:6.2,11:5.1975,12:4.14,13:3.25,14:2.2925,15:1.1625,16:0.2,17:7.50125,18:6.435,19:5.51,20:4.275,21:3.54375,22:2.5025,23:1.3225,24:0.510004,25:7.375,26:6.5,27:5.26501,28:4.725,29:3.2625,30:2.625,31:1.35625,32:0.36,33:7.54875,34:6.9275,35:5.55626,36:4.86,37:3.6075,38:2.75501,39:1.80376,40:0.55,41:7.94376,42:7.08751,43:5.69752,44:4.73001,45:3.76877,46:3.105,47:1.99752,48:0.540007,49:8.33,50:6.6875,51:5.99251,52:4.81001,53:4.10751,54:2.36251,55:1.71877,56:1.54,57:7.5525,58:6.81502,59:6.19502,60:4.65002,61:3.73625,62:3.25502,63:1.81126,64:0.72,65:9.66879,66:8.08502,67:6.86754,68:6.03503,69:5.00252,70:4.72504,71:3.01753,72:1.89003,73:8.39503,74:7.58503,75:6.56252,76:5.51002,77:4.4275,78:3.31501,79:2.56752,80:1.3,81:9.11252,82:9.02002,83:6.84754,84:6.51006,85:5.20631,86:3.65503,87:3.37131,88:2.75005,89:9.56752,90:9.33754,91:8.07628,92:6.67005,93:5.69626,94:4.46503,95:4.03752,96:3.60006,97:8.97252,98:8.20755,99:7.54882,100:6.125,101:5.80751,102:3.31507,103:3.21885,104:2.99002,105:10.7626,106:9.27507,107:7.75757,108:8.37008,109:5.85879,110:5.63761,111:4.16251,112:3.50007,113:8.75755,114:8.40757,115:7.18753,116:6.67011,117:4.97261,118:4.2776,119:2.82629,120:1.35005,121:9.37761,122:7.62508,123:7.22637,124:5.11512,125:5.93762,126:2.67753,127:2.85756,128:2.24,129:18.5439,130:19.1752,131:17.5214,132:16.1701,133:14.63,134:15.0751,135:13.1627,136:12.5801,137:10.7889,138:10.0051,139:10.0775,140:8.05017,141:7.22643,142:6.92266,143:4.64769,144:5.22016,145:20.8439,146:19.3452,147:19.11,148:18.1301,149:17.5076,150:15.5627,151:16.4213,152:14.0602,153:12.6227,154:11.9351,155:11.2377,156:10.725,157:8.2427,158:8.88768,159:6.75769,160:5.2,161:20.7289,162:18.225,163:17.5225,164:17.0152,165:15.4689,166:14.5251,167:13.7777,168:13.0201,169:10.5625,170:11.6876,171:8.76385,172:7.31006,173:8.21759,174:7.17777,175:5.03134,176:5.50025,177:20.134,178:19.1352,179:18.5715,180:19.3501,181:14.9328,182:17.7452,183:13.2676,184:13.5701,185:14.3378,186:11.6252,187:10.7528,188:9.87007,189:8.97754,190:6.17504,191:7.87899,192:5.28012,193:25.814,194:24.9775,195:23.8876,196:20.3353,197:23.1475,198:20.5428,199:19.4028,200:17.5,201:18.5928,202:15.655,203:14.7179,204:15.0454,205:13.5815,206:12.8752,207:11.9027,208:10.92,209:22.7288,210:19.4254,211:19.5177,212:18.5501,213:17.8388,214:16.5854,215:15.5879,216:15.6604,217:12.4779,218:13.8976,219:12.3191,220:9.35022,221:10.4979,222:9.15776,223:7.52647,224:5.88014,225:18.8442,226:19.7754,227:16.1741,228:16.8154,229:15.4577,230:14.3753,231:13.2825,232:12.4702,233:10.7766,234:9.94522,235:9.9878,236:8.55548,237:7.99882,238:6.24758,239:5.37791,240:2.70037,241:18.678,242:17.848,243:16.403,244:15.2502,245:15.0064,246:14.1452,247:10.8067,248:14.2606,249:7.78177,250:10.6252,251:9.09898,252:5.98536,253:9.17183,254:3.17512,255:5.41892,256:4.48,257:39.5139,258:38.3781,259:37.2317,260:38.3503,261:35.2352,262:33.4056,263:35.1763,264:29.7006,265:34.1188,266:30.2579,267:28.7026,268:30.1505,269:26.2277,270:28.0129,271:25.4066,272:23.8007,273:22.5231,274:23.9758,275:20.2815,276:21.3907,277:20.0827,278:17.028,279:19.1818,280:17.5003,281:16.1581,282:14.8054,283:11.6742,284:13.8457,285:12.1128,286:10.3683,287:9.3279,288:6.12072,289:60.6901,290:53.2884,291:57.8366,292:50.7354,293:55.6707,294:51.083,295:52.7317,296:48.4706,297:50.4909,298:48.4251,299:47.4666,300:42.7503,301:47.7839,302:43.0356,303:42.0419,304:43.7003,305:40.7942,306:39.0159,307:37.992,308:39.6558,309:38.2398,310:36.4253,311:31.8782,312:37.0501,313:32.4741,314:32.1854,315:30.7128,316:29.2309,317:31.3038,318:26.2359,319:27.9127,320:26.4,321:55.774,322:54.7408,323:56.1219,324:51.4357,325:52.8134,326:52.1601,327:50.2768,328:51.661,329:47.2939,330:47.4379,331:45.9266,332:44.4052,333:45.7883,334:44.6736,335:43.1322,336:41.5802,337:39.5977,338:38.0257,339:38.5618,340:40.3759,341:34.9529,342:36.7652,343:31.7279,344:37.8408,345:28.4626,346:32.0058,347:30.3629,348:28.7112,349:30.9745,350:25.3752,351:30.7133,352:22.0011,353:61.775,354:62.8363,355:58.132,356:63.6362,357:54.8898,358:60.8605,359:54.7481,360:57.1502,361:51.8941,362:53.8488,363:52.1826,364:51.8705,365:50.6439,366:48.9528,367:47.252,368:48.7602,369:47.5101,370:41.6263,371:47.7663,372:42.3154,373:41.9633,374:40.2064,375:38.9066,376:40.8909,377:38.6438,378:36.8551,379:35.0577,380:35.6251,381:37.1487,382:31.5155,383:33.5135,384:31.681,385:77.4819,386:74.7889,387:71.1127,388:77.1159,389:71.9664,390:69.7136,391:71.3575,392:70.0715,393:68.7754,394:66.4876,395:64.6817,396:66.8266,397:65.0104,398:63.183,399:61.3463,400:59,401:61.6542,402:59.2964,403:57.9322,404:60.6009,405:53.6637,406:56.3342,407:53.9282,408:51.0016,409:56.2385,410:50.2254,411:48.808,412:50.4713,413:49.0446,414:47.0929,415:48.7642,416:42.641,417:43.7851,418:44.9359,419:42.949,420:40.9518,421:38.9442,422:41.6739,423:39.1281,424:35.5112,425:37.1889,426:35.6786,427:33.0928,428:35.3107,429:33.2481,430:30.6392,431:27.4764,432:33.4818,433:27.0632,434:29.2957,435:22.8393,436:27.7952,437:24.5827,438:22.9968,439:20.8536,440:22.5514,441:21.4994,442:18.7858,443:18.2754,444:16.6515,445:14.4626,446:16.7264,447:14.529,448:14.0003,449:71.2807,450:69.752,451:66.523,452:69.4969,453:66.8191,454:60.7245,455:65.9756,456:62.702,457:61.1253,458:62.9766,459:57.3771,460:63.2507,461:56.4744,462:59.4837,463:52.6669,464:57.4205,465:54.638,466:52.4257,467:50.2036,468:53.8216,469:50.4198,470:48.1756,471:43.5689,472:51.3322,473:44.3444,474:46.2163,475:43.9395,476:46.4102,477:39.9493,478:39.4359,479:41.9134,480:39.6008,481:37.2779,482:35.5496,483:37.4337,484:35.0922,485:35.1631,486:33.4134,487:30.4383,488:32.3303,489:30.5642,490:25.7252,491:28.8469,492:25.8305,493:27.7335,494:25.9359,495:23.5138,496:26.0411,497:21.1227,498:18.6761,499:20.5849,500:15.6255}

def malloc_overhead(nbytes):
    return __alloc_to_overhead.get(int(nbytes+0.5),16)

def cppfilt(s):
    if '+0x' in s: s=s.split('+0x')[0]
    if '(' in s: s=s.split('(')[1]
    import subprocess
    p = subprocess.Popen(['/usr/bin/c++filt',s], stdout=subprocess.PIPE)
    c=p.communicate()[0].strip()
    #compactify a bit:
    c=c.replace(', ',',').replace('unsigned int','unsigned')
    c=c.replace('std::basic_string<char,std::char_traits<char>,std::allocator<char> >','std::string')
    i=0
    while ',std::allocator<' in c:
        i+=1
        if i==100: break
        templ=c.split(',std::allocator<',2)[1].split('>',1)[0]
        c=c.replace('<%s,std::allocator<%s> >'%(templ,templ),'<%s>'%templ)
    return c

def escape(s):
    if not do_html: return s
    import cgi
    return cgi.escape(s)

def div_start(this_id,controls_id=None):
    if this_id: this_id=this_id.replace('(','__').replace(')','__')
    if controls_id: controls_id=controls_id.replace('(','__').replace(')','__')
    if not do_html: return
    idstr=' id="%s" style="display:none;white-space: nowrap;"'%escape(this_id) if this_id else ' style="white-space: nowrap;"'
    if not controls_id: print '<div%s>'%idstr,
    else: print '<div%s><a href="javascript:;" onmousedown="toggleDiv(\'%s\');" >'%(idstr,escape(controls_id)),

def div_end(controls_id=None):
    if not do_html: return
    if not controls_id: print '</div>'
    else: print '</a></div>'

def div(contents,this_id,controls_id):
    if not do_html:
        print contents
        return
    div_start(this_id,controls_id)
    print escape(contents),
    div_end(controls_id)

def print_var(idtype,data_onereport,title,format,ntop,width,value_fct):
    #  -> entries in data_onereport['vects'] have format n,bytes,used,fileid,symbid
    #  -> entries in data_onereport['strings'] have format n,bytes,wasted,fileid,symbid
    class File(object):
        def __init__(self,fileid):
            self.__data=[]
            self.__fileid=fileid
            self.__total='notinit'
        def register(self,entry):
            v=value_fct(entry[0],entry[1],entry[2])
            if v!=None: self.__data+=[(v,entry[4])]#(value,symbid) pairs
        def name(self):
            return os.path.basename(data_onereport['files'][self.__fileid])
        def total(self):
            if self.__total=='notinit':
                self.__total=sum(val for val,symbid in self.__data)
            return self.__total
        def symb_itr(self,cut=None,demangle=True):
            symb2tot={}
            for v,sid in self.__data:
                if not sid in symb2tot: symb2tot[sid]=v
                else: symb2tot[sid]+=v
            l=[(t,s) for s,t in symb2tot.items()]
            l.sort(reverse=True)
            first=True
            for v,sid in l:
                if v==None: continue
                if not first and (cut!=None and v<cut): continue
                first=False
                sb=data_onereport['symbs'][sid]
                if demangle:
                    sb=cppfilt(sb)
                yield (v,sb)

    fileid2obj={}
    entrykey='vects' if 'vects' in data_onereport else 'strings' 
    for entry in data_onereport[entrykey]:
        fid=entry[3]
        if not fid in fileid2obj: fileid2obj[fid]=File(fid)
        fileid2obj[fid].register(entry)
    tot2fileobj=[(fo.total(),fo) for _,fo in fileid2obj.items() if fo.total()!=None]
    tot2fileobj.sort(reverse=True)
    grandtotal=sum(v for v,_ in tot2fileobj)
    id=idtype+title.replace(' ','').lower()
    list_item_start()
    div("%s: Total of %s"%(title,format%grandtotal),None,id)
    div_start(id)
    cumul=0
    list_start()
    for tot,fo in tot2fileobj[0:ntop]:
        if tot==0.0: continue
        cumul+=tot
        if cumul>0.995*grandtotal: break
        t="  %s [%.1f%%] in %s "%((format%tot).rjust(6),cumul*100.0/grandtotal,fo.name())
        if not do_html:
            if len(t)<width: t+='='*(width-len(t))
        tid=id+'__'+fo.name()
        list_item_start()
        div(escape(t),None,tid)
        div_start(tid)
        list_start()
        for v,sb in fo.symb_itr(cut=tot*0.01):
            s="%s in %s"%((format%v).rjust(10),sb)
            if not do_html: s="      + "+s
            list_item(escape(s if len(s)<width else s[0:width-3]+'...'))
        list_end()
        div_end()
        list_item_end()
    list_end()
    div_end()
    list_item_end()

def analyse(filename):
    data=PmbUtils.LogFileParsingUtils.parseall(filename,quiet=True)['misc']

    if do_html:
        print '\n'.join(html_head())

    list_start()
    list_item_start()
    div("Job information",None,'jobinfo')
    div_start('jobinfo')
    if do_html: print '<pre>'
    print "Information about test job:"
    print "  Job finished OK:",data['statusok']
    print "  Job input file:",data['infile']
    print "  Job number of evts:",data['inputevts']
    print "  Job ran on build:",data['build_branch'],'/',data['atlas_release'],'/',data['build_date']
    print "  Full job command:",data['command']
    if do_html: print '</pre>'
    div_end()
    ptr_size_bytes=8
    #malloc_overhead=ptr_size_bytes*4
    sizeof_vector=ptr_size_bytes*3
    sizeof_stringrep=ptr_size_bytes*3#length, capacity, refcount.
    ntop=40
    width=140
    if do_html: width=99999
    mb=1.0/(1024*1024)
    list_item_end()
    for irep,rep in enumerate(data['vectorspy']):
        list_item_start()
        rep_id='vectreport%i_results'%irep
        div("std::vector<..> usage report #%i"%(irep+1),None,rep_id)
        div_start(rep_id)
        list_start()
        print_var("vect",rep,"Total vector memory","%.3f mb",ntop,width,
                  lambda n,bytes,used : n*(bytes+malloc_overhead(bytes)+sizeof_vector)*mb)
        print_var("vect",rep,"Allocations (without malloc overhead)","%.3f mb",ntop,width,lambda n,bytes,used : n*bytes*mb)
        print_var("vect",rep,"Overallocations","%.3f mb",ntop,width,
                  lambda n,bytes,used : n*(bytes-used)*mb if used!=None else None)
        print_var("vect",rep,"Overhead","%.3f mb",ntop,width,
                  lambda n,bytes,used : n*(malloc_overhead(bytes)+sizeof_vector)*mb)
        print_var("vect",rep,"Number of vectors","%i",ntop,width,
                  lambda n,bytes,used : n)
        print_var("vect",rep,"Number of vectors without usage info","%i",ntop,width,
                  lambda n,bytes,used : n if used==None else None)
        list_end()
        div_end()
        list_item_end()

    for irep,rep in enumerate(data['stringspy']):
        list_item_start()
        rep_id='stringreport%i_results'%irep
        div("std::string and TString usage report #%i"%(irep+1),None,rep_id)
        div_start(rep_id)
        list_start()
        #approximate malloc cost by assuming bytes/n+sizeof_stringrep in each string rep, add 1 for null char, and the string rep size itself.
        print_var("str",rep,"Total string memory","%.3f mb",ntop,width,lambda n,bytes,wasted : mb*(bytes+n*sizeof_stringrep+malloc_overhead(sizeof_stringrep+bytes*1.0/n)))
        print_var("str",rep,"Allocations (without malloc overhead)","%.3f mb",ntop,width,lambda n,bytes,wasted : bytes*mb)
        print_var("str",rep,"Bytes wasted to non-sharing or overallocations (without malloc overhead)","%.3f mb",ntop,width,lambda n,bytes,wasted : wasted*mb)
        print_var("str",rep,"Number of unshared strings","%i",ntop,width,lambda n,bytes,wasted : n)
        print_var("str",rep,"Overhead (anything except the actual string contents)","%.3f mb",ntop,width,lambda n,bytes,wasted : mb*n*(sizeof_stringrep+1+malloc_overhead(sizeof_stringrep+bytes*1.0/n)))
        
        list_end()
        div_end()
        list_item_end()


    list_end()

    if do_html:
        #print '</pre>'
        print '\n'.join(html_foot())
    global openul
    global openli
    assert openul==0 and openli==0

if __name__=='__main__':
    import sys
    args=sys.argv[1:]
    if '--html' in args:
        do_html=True
        args.remove('--html')
    help='--help' in args or '-h' in args
    if help or len(args)!=1 or not os.path.exists(args[0]):
        print "Usage:"
        print
        print "%s [--html] <logfile>"%sys.argv[0]
        sys.exit(0 if help else 1)
    analyse(args[0])

#todo: plots
#frequency curve: N vs. bytes allocated (or wasted)
#lib -> usage, show MB allocated(or wasted) vs. nlibs needed to investigate.
