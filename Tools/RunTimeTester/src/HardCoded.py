# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from os.path import join, basename, normpath

class HardCodedBase:
    def __init__(self):
        self.installArea      = 'InstallArea'
        self.shareArea        = 'share'
        self.leafStampDir     = 'AtlasOffline'
        self.cmtArea          = 'cmt'
        self.binArea          = 'bin'
        self.limitedNightlies = ['atlrel_0', 'atlrel_3']
        self.projectsWithReleaseName = ['AtlasOffline','AtlasProduction']
        
    def installAreaDirName(self):
        return self.installArea

    def binAreaDirName(self):
        return self.binArea
    
    def shareAreaDirName(self):
        return self.shareArea
   
    def cmtAreaDirName(self):
        return self.cmtArea

    # noticed by PyLint
    # commented out as attribute not seat and method never called.
    # def projectWithRTT(self):
    #    return self.RTTproject

    def projectWithRecExCommon(self):
        return 'AtlasReconstruction'

    def relativeRecExCommonLinks(self,projectShareArea):
        release = basename(normpath(join(projectShareArea,'../..')))
        return normpath(join(projectShareArea,
                             '../../../..',
                             self.projectWithRecExCommon(),
                             release,
                             self.installAreaDirName(),
                             self.shareAreaDirName(),
                             self.binAreaDirName(),
                             'RecExCommon_links.sh'))
    
    def nightlyDirIfAny(self, branch, isNightly):
        """return the nightly offset (eg nightlies/dev) if the branch is in
        the dictionary, otherwise retuens an empty string. Branch is set
        to Numbered if the reelase is numbered, and as this is not in the
        dictionary, numbered releases return an empty string"""

        if not isNightly: return ''
        # branch must be in the dictionaries - otherwise RTT error
        return self.getNightlyPathFragment(branch)

    def pathFragmentToReleaseNotes(self,release):
        return join(release,'cmt/ReleaseNotes.txt')
    
    def getNightlyPathFragment(self, branch):
        pass
    
    def relativeRelease(self, branch, release):
        return join(self.getNightlyPathFragment(branch), release)

    def getLatestCopiedReleaseLinkName(self, key):
        pass
        
# ==========================================================================

class HardCodedProjectRelease(HardCodedBase):

    def __init__(self):
        HardCodedBase.__init__(self)
        # self.distBase    = '/afs/cern.ch/atlas/software/builds'
        self.rttProject  = 'AtlasCore' # package containing the RunTimeTester package
        
    def getNightlyPathFragment(self, branch):
        if branch is 'Numbered': return ''
        return 'nightlies/%s' % branch.strip()
        
    def getLatestCopiedReleaseLinkName(self, key):
        d = {'i686'  : '32B', 'x86_64' : '64B',
             'slc4'  : 'S4' , 'slc5'   : 'S5',
             'dbg'   : 'Dbg', 'opt'    : 'Opt',
             'gcc43' : 'G4' , 'gcc34'  : ''}
        i, os, gcc, bin = key[0].split('-')
        return 'latest_copied_release%s%s%sAll%s' % (d[i], d[os], d[gcc], d[bin])

# ==========================================================================

class HardCodedProjectKit(HardCodedBase):
    def __init__(self):
        HardCodedBase.__init__(self)
        self.nightlyPathFragment = {'Numbered':'',
                                    'val':'',
                                    'bugfix':'',
                                    'pcache':'',
                                    'dev':''}

    def nightlyStamp(self,branch):
        return None

