import re, unittest
from domain_map import DOMAIN_MAP

class TestDomainMapDictionary(unittest.TestCase):
    def test_DomainMapIsDict(self):
        self.assertIsInstance(DOMAIN_MAP,dict)

    def test_KeysAreStrings(self):
        for domain_name in DOMAIN_MAP.keys():
            self.assertIsInstance(domain_name,str)

    def test_ValuesAreSets(self):
        for package_patterns in DOMAIN_MAP.values():
            self.assertIsInstance(package_patterns,set)

    def test_PatternsAreValidRegex(self):
        for package_patterns in DOMAIN_MAP.values():
            for pattern in package_patterns:
                try:
                    _ = re.compile(pattern)
                except:
                    self.fail("compiling '%s' as regular expression failed" % pattern)

if __name__ == "__main__":
    unittest.main()
