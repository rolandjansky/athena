# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

"""Define TileFolders class to keep and return Tile folders"""

class TileFolders(object):
    """Keep Tile folders taking into account whether it is online/offline and MC/Data"""

    def __init__(self, isMC, isOnline):
        """Initialize taking into account whether it is online/offline and MC/Data"""
        self._folders = []
        self._isMC = isMC
        self._isOnlineAndNotMC = isOnline and not isMC

    def add(self, folder, db):
        """Add and return Tile folder using given db Tile schema"""
        self._folders.append((folder + '<key>' + folder + '</key>', db, 'CondAttrListCollection'))
        return folder

    def addSplitMC(self, folder, folderMC):
        """Add and return Tile folder, using folder (online) for real data, folderMC (offline) for MC"""
        actualFolder, db = (folderMC, 'TILE_OFL') if self._isMC else (folder, 'TILE')
        return self.add(actualFolder, db)

    def addSplitOnline(self, onlineFolder, oflineFolder):
        """Add and return Tile folder, using onlineFolder online, oflineFolder offline"""
        actualFolder, db = (onlineFolder, 'TILE') if self._isOnlineAndNotMC else (oflineFolder, 'TILE_OFL')
        return self.add(actualFolder, db)

    def get(self):
        """Return Tile folders as list of (folder, DB, 'CondAttrListCollection')"""
        return self._folders



import unittest
class TestTileFolders( unittest.TestCase ):
    def test_OfflineData(self):
        folders = TileFolders(isMC = False, isOnline = False)
        self.assertEqual(folders.get(), [], 'Initial folders are not empty')

        self.assertEqual(folders.add('TEST', 'DB'), 'TEST', 'Wrong folder is added')
        self.assertTrue(('TEST<key>TEST</key>', 'DB', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitOnline('ONL', 'OFL'), 'OFL', 'Wrong folder is added')
        self.assertTrue(('OFL<key>OFL</key>', 'TILE_OFL', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitMC('DATA', 'MC'), 'DATA', 'Wrong folder is added')
        self.assertTrue(('DATA<key>DATA</key>', 'TILE', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(len(folders.get()), 3, 'Number of added folders is wrong')


    def test_OfflineMC(self):
        folders = TileFolders(isMC = True, isOnline = False)
        self.assertEqual(folders.get(), [], 'Initial folders are not empty')

        self.assertEqual(folders.add('TEST', 'DB'), 'TEST', 'Wrong folder is added')
        self.assertTrue(('TEST<key>TEST</key>', 'DB', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitOnline('ONL', 'OFL'), 'OFL', 'Wrong folder is added')
        self.assertTrue(('OFL<key>OFL</key>', 'TILE_OFL', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitMC('DATA', 'MC'), 'MC', 'Wrong folder is added')
        self.assertTrue(('MC<key>MC</key>', 'TILE_OFL', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(len(folders.get()), 3, 'Number of added folders is wrong')


    def test_OnlineData(self):
        folders = TileFolders(isMC = False, isOnline = True)
        self.assertEqual(folders.get(), [], 'Initial folders are not empty')

        self.assertEqual(folders.add('TEST', 'DB'), 'TEST', 'Wrong folder is added')
        self.assertTrue(('TEST<key>TEST</key>', 'DB', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitOnline('ONL', 'OFL'), 'ONL', 'Wrong folder is added')
        self.assertTrue(('ONL<key>ONL</key>', 'TILE', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitMC('DATA', 'MC'), 'DATA', 'Wrong folder is added')
        self.assertTrue(('DATA<key>DATA</key>', 'TILE', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(len(folders.get()), 3, 'Number of added folders is wrong')


    def test_OnlineMC(self):
        folders = TileFolders(isMC = True, isOnline = True)
        self.assertEqual(folders.get(), [], 'Initial folders are not empty')

        self.assertEqual(folders.add('TEST', 'DB'), 'TEST', 'Wrong folder is added')
        self.assertTrue(('TEST<key>TEST</key>', 'DB', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitOnline('ONL', 'OFL'), 'OFL', 'Wrong folder is added')
        self.assertTrue(('OFL<key>OFL</key>', 'TILE_OFL', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(folders.addSplitMC('DATA', 'MC'), 'MC', 'Wrong folder is added')
        self.assertTrue(('MC<key>MC</key>', 'TILE_OFL', 'CondAttrListCollection') in folders.get(), 'No correct folder')

        self.assertEqual(len(folders.get()), 3, 'Number of added folders is wrong')



if __name__ == "__main__":
    unittest.main()
