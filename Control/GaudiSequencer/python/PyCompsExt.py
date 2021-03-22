__doc__     = 'Library defines a class for filtering events. Allows to provide run/event numbers from a file.'
__author__  = 'Oldrich Kepka <oldrich.kepka@cern.ch'

from GaudiSequencer.PyComps import PyEvtFilter
from AthenaPython.PyAthena import StatusCode

class PyEvtFilterFromFile (PyEvtFilter):
    """ Algorithm which loads list of runnunber/eventnumbers from file for filtering and pass it to PyEvtFilter
    """

    def __init__(self, name='filterFromFile', **kw):
        super(PyEvtFilterFromFile, self).__init__(name,**kw)

        self.input_file = kw.get('selectorInputFile', None)

    def initialize(self):

        _info  = self.msg.info
        _error = self.msg.error

        _info('==> PyEvtFilterFromFile initialize')
        if not self.input_file:
            _error('Input_file is required.')
            return StatusCode.Failure
        try: 
            f = open(self.input_file, 'r')
        except IOError:
            _error = self.msg.error('File {} cannot be opened.'.format(self.input_file))
            return StatusCode.Failure
        else:
            _info('==> File {} opened. Loading Runnumber/Eventnumber list'.format(self.input_file))
            with f: 
                self.evt_list = []
                for line in f: 
                    if '#' in line:
                        continue

                    numbers = line.split() 

                    if not numbers:
                        continue
                    if len(numbers) != 2:
                        self.msg.warning('Following line cannot be parsed: {}'.format(line))

                    self.evt_list.append((int(numbers[0]), int(numbers[1])))

        super(PyEvtFilterFromFile,self).initialize()
        return StatusCode.Success
