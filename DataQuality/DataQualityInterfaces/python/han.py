# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from cppyy.gbl import dqi
import ROOT
import re
import os
import string
from typing import List, Dict, Tuple, Optional
import logging
log = logging.getLogger('DataQualityInterfaces')

def copyMetadata(newgroup, oldgroup, input=None):
    """Copy the configuration of an old algorithm to a new one, but without the
    tree structure"""
    newgroup.SetAlgName(oldgroup.GetAlgName())
    newgroup.SetAlgLibName(oldgroup.GetAlgLibName())
    newgroup.SetAlgRefName(oldgroup.GetAlgRefName())
    for par in oldgroup.GetAllAlgPars():
        newgroup.AddAlgPar(par)
    for lim in oldgroup.GetAllAlgLimits():
        newgroup.AddAlgLimit(lim)
    for ann in oldgroup.GetAllAnnotations():
        if input is not None and ann.GetName() == 'inputname':
            ann.SetValue(input)
        newgroup.AddAnnotation(ann)
    newgroup.SetWeight(oldgroup.GetWeight())


def Initialize( configName: str, inputFileName: str, prefix: str ) -> Optional[dqi.HanConfigGroup]:
    """ Handle everything in Python, testing only """

    m_config = ROOT.TFile.Open( configName, "READ" )
    if not m_config:
        log.error(f"HanConfig::Initialize() cannot open file \"{configName}\"\n")
        return None

    key = m_config.FindKey("top_level")
    if not key:
        log.error(f"HanConfig::Initialize() cannot find configuration in file \"{configName}\"\n")
        return None

    top_level = key.ReadObj()
    if not isinstance(top_level, dqi.HanConfigGroup):
        log.error(f"HanConfig::Initialize() cannot find configuration in file \"{configName}\"\n")
        return None

    inputFile = ROOT.TFile.Open(inputFileName, 'READ')

    return FixRegion(top_level, inputFile.Get(prefix))


def FixRegion(top_level: dqi.HanConfigGroup, td: ROOT.TDirectory) -> dqi.HanConfigGroup:
    """Main code to translate the configuration given by 'top_level' into the
    final configuration by expanding the regexes using the objects in 'td'"""
    log.info('Translating regexes...')

    mapping = {}
    mapping_regex = {}
    mapping_groups = {'top_level': top_level}
    mapping_assessors_final = {}
    mapping_groups_final = {}
    iterate_hcfg(top_level, mapping, mapping_regex, mapping_groups)
    mapping_assessors_final.update(mapping)
    for path, g in mapping_groups.items():
        newgroup = dqi.HanConfigGroup()
        newgroup.SetName(g.GetName())
        newgroup.SetPathName(path)
        copyMetadata(newgroup, g)
        mapping_groups_final[path] = newgroup

    
    l = []
    iterate_objs(td, l, td.GetPath())
    for path, (a, p) in mapping_regex.items():
        pre = re.compile(p + '$')
        for h in l:
            m = pre.match(h)
            if m:
                tokenized_path = path.split('/')
                orig_fullpath = []
                new_fullpath = []
                for tok in tokenized_path[:-1]:
                    formatted_tok = string.Template(tok).substitute(m.groupdict())
                    orig_fullpath.append(tok)
                    new_fullpath.append(formatted_tok)
                    if formatted_tok != tok:
                        orig = '/'.join(orig_fullpath)
                        target = '/'.join(new_fullpath)
                        log.debug(f'Need to adapt {mapping_groups[orig]} to {target}')
                        if orig in mapping_groups_final:
                            log.debug(f'Deleting {orig}')
                            del mapping_groups_final[orig]
                        if target not in mapping_groups_final:
                            newgroup = dqi.HanConfigGroup()
                            newgroup.SetName(formatted_tok)
                            newgroup.SetPathName(target)
                            oldgroup = mapping_groups[orig]
                            copyMetadata(newgroup, oldgroup)
                            mapping_groups_final[target] = newgroup
                tok = tokenized_path[-1]
                orig_fullpath.append(tok)
                hname = h.split('/')[-1]
                # special case: hname@LABEL in the original token
                hextra = '' if '@' not in tok else ('@' + tok.rsplit('@',1)[1])
                hname += hextra
                new_fullpath.append(hname)
                orig = '/'.join(orig_fullpath)
                target = '/'.join(new_fullpath)

                newass = dqi.HanConfigAssessor()
                newass.SetName(h + hextra)
                copyMetadata(newass, mapping_regex[orig][0], input=h)
                mapping_assessors_final[target] = (newass, newass.GetHistPath())
                log.debug(f'change {orig} to {target}')

    # assemble new tree
    # need to do it in this order or structure will be lost (there is a lot of cloning going on)
    for p, (a, _) in mapping_assessors_final.items():
        try:
            mapping_groups_final[os.path.dirname(p)].AddAssessor(a)
        except Exception as e:
            log.error(f'{e}: {p}, {a}, {a.GetName()}')
    for p, g in reversed(list(mapping_groups_final.items())):
        log.debug(f'Final additions for {p}, {g}, {g.GetPathName()} into {os.path.dirname(p)}')
        if p != 'top_level':
            mapping_groups_final[os.path.dirname(p)].AddGroup(g)

    return mapping_groups_final['top_level']


def iterate_hcfg(group: dqi.HanConfigGroup, 
                 mapping: Dict[str,Tuple[dqi.HanConfigAssessor, str]], 
                 mapping_regex: Dict[str,Tuple[dqi.HanConfigAssessor, str]], 
                 mapping_groups: Dict[str, dqi.HanConfigGroup]) -> None:
    """Recurse to collect all information on DQ configuration"""
    import os.path
    for a in group.GetAllAssessors():
        m = mapping_regex if a.GetIsRegex() else mapping
        m[os.path.join(group.GetPathName(), os.path.basename(a.GetName()))] = (a, a.GetHistPath())
    for g in group.GetAllGroups():
        mapping_groups[g.GetPathName()] = g
        iterate_hcfg(g, mapping, mapping_regex, mapping_groups)


def iterate_objs(thisdir: ROOT.TDirectory, l: List[str], prefix: str):
    """Recurse to find the names of all objects"""
    import os.path
    # inconsistent ROOT handling of top directory name of file
    prefix = os.path.normpath(prefix)
    prefixlen = len(prefix) + 1
    for k in thisdir.GetListOfKeys():
        if k.GetClassName().startswith('TDirectory'):
            iterate_objs(k.ReadObj(), l, prefix)
        else:
            if k.GetName() != 'metadata':
                l.append(os.path.join(thisdir.GetPath()[prefixlen:], k.GetName()))
