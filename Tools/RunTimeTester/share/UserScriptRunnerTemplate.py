import os
import os.path
import sys
import cPickle

class UserScriptRunner:
    def run(self, picklefile):
        sys.path.append("%USER_MODULE_LOCATION%")
        sys.path.append("%RTT_SHARE_DIR%")
        sys.path.append("%RTT_SRC_DIR%")

        # sys.path = ["/afs/cern.ch/atlas/project/RTT/Results/dozer/api"] + sys.path
        import USER_MODULE_NAME
        inst = USER_MODULE_NAME.USER_MODULE_NAME(cPickle.load(file(picklefile)))
        inst.run()

if __name__ == '__main__':
    UserScriptRunner().run(sys.argv[1].strip())
