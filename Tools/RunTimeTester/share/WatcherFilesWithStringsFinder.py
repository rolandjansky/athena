######################################################
#                                                    #
# Do NOT bind any objects (self.xxx) which contain   #
# file objects (such as self.logger in this class    #
# otherwise cannit shelve the objects                #
# Instead, unload the necessary variables in         #
# __init__                                           #
#                                                    #
######################################################


from FilesWithStringsFinder import FilesWithStringsFinder

class WatcherFilesWithStringsFinder(FilesWithStringsFinder):
    def __init__(self, dict):
        FilesWithStringsFinder.__init__(self, dict)
        jDescriptor        = dict['JobDescriptor']
        self.confederation = jDescriptor.confederation
        
    def filesInDirs(self, jDescriptor):
        """return a list of files found using the minders of the Watcher's
        confederation of Worker jobs"""

        files = []
        for minder in self.confederation:
            files.extend(FilesWithStringsFinder.filesInDirs())

        return files

