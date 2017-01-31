import re, unittest
from watch_list import WATCH_LIST

class TestWatchListDictionary(unittest.TestCase):
    def test_WatchListIsDict(self):
        self.assertIsInstance(WATCH_LIST,dict)

    def test_ValuesAreSets(self):
        for watchers in WATCH_LIST.values():
            self.assertIsInstance(watchers,set)

    def test_UsernamesAreStrings(self):
        for watchers in WATCH_LIST.values():
            for username in watchers:
                self.assertIsInstance(username,str)

    def test_PatternsAreValidRegex(self):
        for package_pattern in WATCH_LIST.keys():
            try:
                _ = re.compile(package_pattern)
            except:
                self.fail("compiling '%s' as regular expression failed" % package_pattern)

if __name__ == "__main__":
    unittest.main()
