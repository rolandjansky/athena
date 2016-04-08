######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################

from Logger            import Logger

import os

class FilesWithStringsFinder:
    
    def __init__(self, paramDict):

        self.logger            = Logger()
        self.inputFilePatterns = paramDict.get('inputFilePatterns', [])
        self.searchPatterns    = paramDict.get('searchPatterns',    [])
        self.dirWithFiles      = paramDict.get('dirWithFiles', 'resultsDir')
        self.writeTxt          = paramDict.get('writeTxt', 'yes')
        self.writeHTML         = paramDict.get('writeHTML','yes')
        jDescriptor            = paramDict.get('JobDescriptor', None)
        self.runPath           = jDescriptor.runPath
        outputFile             = paramDict.get('outputFile','filesWithStrings')
        self.outputFile        = os.path.join(self.runPath,outputFile)
        self.resultsDirs       = jDescriptor.paths.resultsDirs
        self.writers           =  []


        dict = {
            'runPath':           self.runPath,
            'outputFile':        self.outputFile,
            'inputFilePatterns': self.inputFilePatterns,
            'searchPatterns':    self.searchPatterns,
            'logger':            self.logger
            }

        class Writer:
            def __init__(self, dict):
                self.ofile  = None
                self.outfn = os.path.join( dict['runPath'],
                                           dict['outputFile'])

                
                fpats = dict['inputFilePatterns'] 
                spats = dict['searchPatterns']

                header  = ['Files to match:']
                header.extend([('  %s ' % i ) for i in fpats])
                header.append(' ')
                header.append('\n\nStrings to match:')
                header.extend([('  %s ' % s ) for s in spats])
                header.append(' ')
                self.header = [self.terminateLine(line) for line in header]
                self.logger   = dict['logger']

            
            def openFile(self):
                """File is opened when there is a need to write (is not
                possible to open the file at contruction). On opening
                write out (once!) the header. This part of the action writing
                an item is common to all derived sclasses - they do the rest.
                """

                if self.ofile: return

                self.ofile = open(self.outfn, 'w')
                if not self.ofile:
                    msg = 'FilesEithStringsFinder: could not open file %s' % self.outfn
                    self.logger.error(msg)
                    return
                    
                [self.ofile.write(line) for line in self.header]

        class HTMLWriter(Writer):
            def __init__(self, dict):
                Writer.__init__(self, dict)
                self.outfn  += '.html'
                self.logger   = dict['logger']

        
                htmlHeader = []
                htmlHeader.append('<html>\n')
                htmlHeader.append('<head>\n')
                htmlHeader.append('   <title>FilesWithStringsFinder</title>\n')
                htmlHeader.append('</head>\n')
                htmlHeader.append('<body>\n')
                htmlHeader.extend(self.header)
                htmlHeader.append('<table>\n')
                self.header = htmlHeader

            def terminateLine(self, line):
                line+='<br>\n'
                return line

            def outputItem(self, item):

                if not self.ofile: return
                
                fn  = os.path.basename(item[0])
                href ='<a href="%s">  %s </a>' % (fn, fn)
                htext =  'contains "%s"'% (item[1])



                line  = '<tr>\n'
                line += '<td>'+href+'</td><td>'+htext+'</td>\n'
                line += '</tr>\n'
                
                self.ofile.write(line)

                
            def closeFile(self):
                if not self.ofile: return

                self.ofile.write('</table>\n')
                self.ofile.write('</body>\n')
                self.ofile.write('</html>\n')
                self.ofile.close()
            
        class TxtWriter(Writer):
            def __init__(self, dict):
                Writer.__init__(self, dict)
                self.outfn  += '.txt'
                self.logger   = dict['logger']
                    
            def terminateLine(self, line):
                line+='\n'
                return line


            def outputItem(self, item):
                # output to the html file needs to be htmlified
                # delay opening the files until now, otherwise there is
                # the danger the output directories have not been constructed.

                if not self.ofile: return

                width = 60
                fn = os.path.basename(item[0])
                line = 'file: %s contains "%s" \n' % (string.ljust(fn, width),
                                                      item[1]
                                                      )
                self.ofile.write(line)

            def closeFile(self):
                if not self.ofile: return
                self.ofile.close()
            
        if self.writeTxt == 'yes':
            self.writers.append(TxtWriter(dict))
        if self.writeHTML == 'yes':
            self.writers.append(HTMLWriter(dict))

        self.logger.debug('FilesWithStringsFinder Writers: %s' % (
            str(self.writers)))
    def run(self):

        files = self.filesInDirs()
        self.logger.debug('FilesWithStringsFinder: filesInDirs %s' % str(files))

        fPatterns     = self.inputFilePatterns
        filesToSearch = []
        
        [filesToSearch.extend(fnmatch.filter(files,
                                             ipfp)) for ipfp in fPatterns]

        strs         = self.searchPatterns

        m = """FilesWithStringsFinder: files to search %s\nsearchPatterns:
        %s""" % (str(filesToSearch), str(strs))
        self.logger.debug(m)

        class StringInFile:
            def __init__(self, base, file):
                ifn = os.path.join(base,file)
                print 'checking file ',ifn,'... '
                self.contents = open(ifn, 'r').read()
            def check(self, string):
                print '    ... for string', string
                if self.contents.find(string) > 0: return 0
                return -1
            
        filesWithString = []
        for f in filesToSearch:
            sFile = StringInFile(dwf, f)
            [filesWithString.append((f,s))  for s in strs if sFile.check(s)]

        self.logger.debug('filesWithString %s' % str(filesWithString))

        [writer.openFile() for writer in self.writers]

        for writer in self.writers:
            [writer.outputItem(item) for item in filesWithString]

        [writer.closeFile() for writer in self.writers]

        return 0

    def filesInDirs(self):
        """
        return a list of files found using the descriptor Paths object scan
        the descriptor is passed as an argument, rather than getting
        it from the dictionary asthe descriptor passed may not be the local
        on: watchers pass the descriptors of their confederation minders, not
        their own descriptors.
        """

        msgHead ='FilesWithStringsFinder.filesInDirs: '

        logicalDir = self.dirWithFiles
        files = []
        
        self.logger.debug('%s files to search %s' % (msgHead, str(logicalDir)))

        if logicalDir == 'resultsDir':
            key = 'resBase/build/branch/runType/release/jobGroup'
            if key not in self.resultsDirs:
                self.logger.error('results directory not in Paths %s' % key)
                return files
            dwf = self.resultsDirs[key]
            self.logger.debug('FilesWithStringsFinder violfile directory is %s' % dwf)
            files = os.listdir(dwf)
            return files
        elif logicalDir == 'runDir':
            files = os.listdir(self.runPath)
            self.logger.debug('FilesWithStringsFinder violfile directory is %s' % self.runPath)
            return files
        else:
            msg = 'FilesWithStringFinder: Unknown directory to scan %s' % dwf
            self.logger.error(msg)
            return files

